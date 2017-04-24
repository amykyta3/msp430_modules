/*
* Copyright (c) 2012, Alexander I. Mykyta
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/**
* \addtogroup MOD_COTHREADS Cooperative Threads
* \brief Cooperative Processor Threads
* \author Alex Mykyta 
*
* Provides a lightweight method of implementing cooperative threads for the MSP430. Alternate thread
* contexts operate in separate stack space that can be dynamically allocated in the heap or from
* within the base thread's stack.
* 
* \warning No measures are taken to prevent stack overflows! Be sure to allocate enough stack space
* for the alternate thread. Make sure to account for any interrupt service routines that may trigger
* from within it.
* 
* \todo Add support for TI compiler. cothread_setjmp is currently only designed for msp430-elf's ABI
* 
* \b Example \n
* In this simple example, two different threads control two separate LED outputs. When executed, the
* LED on P2.1 blinks three times, stops, then the LED on P2.2 blinks twice. This sequence repeats
* forever.
* 
* \code
* 
*    #include <msp430.h> 
*    #include <stdint.h>
*    
*    #include <cothread.h>
*    
*    
*    stack_t my_alt_stack[64];
*    
*    cothread_t home_thread; // Home thread object
*    cothread_t my_alt_thread;    // Alternate thread object
*    
*    int alt_thread_func(void);
*    
*    int main(void) {
*        WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
*        
*        // Lets say that P2.1 and P2.2 are LEDs
*        P2DIR |= BIT1 | BIT2;
*        P2OUT &= ~(BIT1 | BIT2);
*        
*        // Initialize cothreads and populate the home thread object
*        cothread_init(&home_thread);
*        
*        // Setup the my_alt_thread object and create the new thread context
*        my_alt_thread.alt_stack = my_alt_stack; // it will use my_alt_stack for its stack storage
*        my_alt_thread.alt_stack_size = sizeof(my_alt_stack); // I hope its big enough!
*        my_alt_thread.co_exit = &home_thread; // If the thread exits, it will return to the home_thread
*        cothread_create(&my_alt_thread,alt_thread_func);
*        
*        while(1){
*            P2OUT |= BIT1; // P2.1 ON
*            __delay_cycles(250000);
*            P2OUT &= ~BIT1; // P2.1 OFF
*            __delay_cycles(250000);
*            P2OUT |= BIT1; // P2.1 ON
*            __delay_cycles(250000);
*            P2OUT &= ~BIT1; // P2.1 OFF
*            __delay_cycles(250000);
*            
*            cothread_switch(&my_alt_thread);
*        }
*        
*        
*        return(0);
*    }
*    
*    int alt_thread_func(void){
*        while(1){
*            P2OUT |= BIT2; // P2.2 ON
*            __delay_cycles(250000);
*            P2OUT &= ~BIT2; // P2.2 OFF
*            __delay_cycles(250000);
*            
*            cothread_switch(&home_thread);
*        }
*        
*        return(0);
*    }
* 
* \endcode
* 
* 
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_COTHREADS "Cooperative Threads"
* \author Alex Mykyta 
**/


#ifndef COTHREAD_H
#define COTHREAD_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "cothread_setjmp.h"

typedef uintptr_t stack_t __attribute__ ((aligned (__BIGGEST_ALIGNMENT__)));

typedef struct{
    ct_jmp_buf env __attribute__ ((aligned (__BIGGEST_ALIGNMENT__)));
    uint8_t valid;
} m_state_t;

typedef struct cothread{
    struct cothread    *co_exit; ///< Thread to switch to once the current thread exits
    stack_t *alt_stack; ///< Pointer to the base of an alternate stack.
    size_t alt_stack_size; ///< The size (in bytes) of the stack which 'alt_stack' points to.
    int (*func_start) (void); ///< Stores the startup function pointer. Do not access.
    m_state_t m_state; ///< This element stores the machine state of the process. Its definition should be treated as opaque
} cothread_t __attribute__ ((aligned (__BIGGEST_ALIGNMENT__)));

//--------------------------------------------------------------------------------------------------
/**
 * \brief Initializes the home thread object
 * 
 * This function should be called prior to using any other cothread routines
 * \param home_thread pointer to an uninitialized cothread_t object
 **/
void cothread_init(cothread_t *home_thread);

//--------------------------------------------------------------------------------------------------
/**
 * \brief Initializes a new cooperative thread
 * 
 * Before the call to this function, the \c co_exit, \c alt_stack, and \c alt_stack_size elements of
 * the \c thread structure must be initialized. The \c alt_stack and \c alt_stack_size elements
 * describe the stack which is used for this thread. No two threads which are used at the same time
 * should use the same memory region for a stack.
 * 
 * The \c co_exit element of the object pointed to by \c thread must be set to the thread to 
 * be switched to when the function func returns or when the thread exits using cothread_exit()
 * 
 * \note When the entry function \c func launches, global interrupts are disabled regardless of the
 * parent thread's state.
 * 
 * \param thread    Pointer to a user context
 * \param func      Entry function for the new thread
**/
void cothread_create(cothread_t *thread, int (*func) (void));

//--------------------------------------------------------------------------------------------------
/**
 * \brief Switch to a different context
 * 
 * Upon calling cothread_switch(), the state of the current thread is saved and the thread pointed
 * to by \c dest_thread is loaded. The current thread resumes after the function call once it has
 * been switched back to.
 * 
 * If \c dest_thread has been previously terminated, execution will continue in the current thread.
 * 
 * \param dest_thread    Pointer to the destination thread to switch to
 * \retval 0        If the previous thread switched here normally using a call to cothread_switch()
 * \retval -1         If it was not possible to switch to \c dest_thread
 * \retval other     Returns the exit value that the thread terminated with.
 **/
int cothread_switch(cothread_t *dest_thread);

//--------------------------------------------------------------------------------------------------
/**
 * \brief Terminates the current thread
 * \param retval This value becomes the return value of the cothread_switch() function in the other
 * thread
 **/
void cothread_exit(int retval);

//--------------------------------------------------------------------------------------------------
/**
 * \name Stack Monitor Functions
 * 
 * \details These functions allow the developer to determine how much space remains in a thread's
 * alternate stack. The stackmon_init() function fills the stack with a pseudorandom number
 * sequence. As the stack is used, these values are overwritten. The stackmon_get_unused() function
 * determines how many bytes of the pseudorandom sequence remain.
 * 
 * \{
 **/
 
 /**
  * \brief Fill the space allocated for an alternate stack with a pseudorandom sequence.
  * \note This must be done \e prior to calling cothread_create()
  * \param stack        Pointer to the allocated stack
  * \param stack_size    The size of the stack in bytes
  **/
void stackmon_init(stack_t *stack, size_t stack_size);
//--------------------------------------------------------------------------------------------------

/**
 * \brief Determine how much of the stack was untouched by program execution
 * \param stack        Pointer to the allocated stack
 * \return Number of bytes remaining in the stack.
 **/
size_t stackmon_get_unused(stack_t *stack);

///\}
//--------------------------------------------------------------------------------------------------
#ifdef __cplusplus
    }
#endif


#endif

///\}


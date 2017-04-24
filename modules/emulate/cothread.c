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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       12/12/2012   born
* 
*=================================================================================================*/

/**
* \addtogroup MOD_EMU_COTHREADS
* \{
**/

/**
* \file
* \brief Code for \ref MOD_EMU_COTHREADS "Cooperative Threads (emulated)"
* \author Alex Mykyta (amykyta3@gmail.com)
**/

#include <stdint.h>
#include <setjmp.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "cothread.h"


/*
 * -----------------------------------------
 *  For the home thread:
 * -----------------------------------------
 * 
 * typedef struct cothread{
 * 		struct cothread	*co_exit; == Null. Unique property of a home thread since it has nowhere to exit to.
 * 		void *alt_stack; == Null. Stack is self-defined in home thread
 * 		size_t alt_stack_size; Any nonzero value
 * 		m_state_t m_state; the 'valid' element is nonzero
 * } cothread_t;
 * 
 * -----------------------------------------
 *  For an alternate thread:
 * -----------------------------------------
 * 
 * typedef struct cothread{
 * 		struct cothread	*co_exit; ///< Thread to switch to once the current thread exits
 * 		void *alt_stack; ///< Pointer to the base of an alternate stack.
 * 		size_t alt_stack_size; ///< The size (in bytes) of the stack which 'alt_stack' points to.
 * 		m_state_t m_state; the 'valid' element is nonzero
 * } cothread_t;
 * 
 * ------------------------------------------
 *  For a dead thread: (one that has exited)
 * ------------------------------------------
 * 
 * typedef struct cothread{
 * 		struct cothread	*co_exit; ///< Thread to switch to once the current thread exits
 * 		void *alt_stack; ///< Pointer to the base of an alternate stack.
 * 		size_t alt_stack_size; ///< The size (in bytes) of the stack which 'alt_stack' points to.
 * 		m_state_t m_state; the 'valid' element is zero
 * } cothread_t;
 * 
 */

//--------------------------------------------------------------------------------------------------
 
static cothread_t *CurrentThread;
static int ThreadRetval;

//--------------------------------------------------------------------------------------------------
void cothread_init(cothread_t *home_thread){
	home_thread->co_exit = NULL;
	home_thread->alt_stack = NULL;
	home_thread->alt_stack_size = 1;
	home_thread->m_state.valid = 1;
	
	home_thread->m_state.thread_mutex = PTHREAD_MUTEX_INITIALIZER;
	home_thread->m_state.thread_condition = PTHREAD_COND_INITIALIZER;
	
	CurrentThread = home_thread;
}

//--------------------------------------------------------------------------------------------------

static void *cothread_startup(void *arg){
	
	// new context startup routine
	cothread_t *old_current;
	cothread_t *thread = (cothread_t *) arg;
	
	// Thread has been created but it isn't allowed to start yet. Freeze it.
	pthread_mutex_lock(&thread->m_state.thread_mutex);
	pthread_cond_wait(&thread->m_state.thread_condition, &thread->m_state.thread_mutex);
	pthread_mutex_unlock(&thread->m_state.thread_mutex);
	
	// kick-off the new thread
	ThreadRetval = thread->m_state.func();
	
	// the thread is no longer valid
	CurrentThread->m_state.valid = 0;

	
	// if co_exit is valid, switch to it
	if(CurrentThread->co_exit){
		old_current = CurrentThread;
		CurrentThread = CurrentThread->co_exit;
		
		// signal the dest thread to continue
		pthread_mutex_lock(&old_current->co_exit->m_state.thread_mutex);
		pthread_cond_signal(&old_current->co_exit->m_state.thread_condition); 
		pthread_mutex_unlock(&old_current->co_exit->m_state.thread_mutex);
		
		// kill this thread
		return(NULL);
	}
	
	//Otherwise, I have no idea where to go! I guess its time for an infinite loop
	while(1);
	// ...Does not return
}

//--------------------------------------------------------------------------------------------------
void cothread_create(cothread_t *thread, int (*func) (void)){
	
	thread->m_state.func = func;
	
	thread->m_state.valid = 1;
	
	thread->m_state.thread_mutex = PTHREAD_MUTEX_INITIALIZER;
	thread->m_state.thread_condition = PTHREAD_COND_INITIALIZER;

	
	pthread_create( &thread->m_state.thread, NULL, (void *) &cothread_startup, (void *) thread);
	
	usleep(1000); // Need to give the new thread time to startup
}

//--------------------------------------------------------------------------------------------------
int cothread_switch(cothread_t *dest_thread){
	cothread_t *old_current;
	
	if(dest_thread == CurrentThread) return(-1); // already in the dest_thread. nothing to do
	if(!(dest_thread->m_state.valid)) return(-1); // dest_thread is not valid. Don't switch
	
	old_current = CurrentThread;
	
	CurrentThread = dest_thread;
	
	ThreadRetval = 0;
	
	// signal the dest thread to continue
	pthread_mutex_lock(&dest_thread->m_state.thread_mutex);
	pthread_cond_signal(&dest_thread->m_state.thread_condition); 
	pthread_mutex_unlock(&dest_thread->m_state.thread_mutex);
	
	
	// put the current thread into a wait state
	pthread_mutex_lock(&old_current->m_state.thread_mutex);
	pthread_cond_wait( &old_current->m_state.thread_condition, &old_current->m_state.thread_mutex);
	pthread_mutex_unlock(&old_current->m_state.thread_mutex);
	
	return(ThreadRetval);
}

//--------------------------------------------------------------------------------------------------
void cothread_exit(int retval){
	cothread_t *old_current;
	
	// exit only if it has a valid exit destination
	if(CurrentThread->co_exit){
		// the thread is no longer valid
		CurrentThread->m_state.valid = 0;
		
		old_current = CurrentThread;
		
		CurrentThread = CurrentThread->co_exit;
		
		ThreadRetval = retval;
		
		// signal the dest thread to continue
		pthread_mutex_lock(&old_current->co_exit->m_state.thread_mutex);
		pthread_cond_signal(&old_current->co_exit->m_state.thread_condition); 
		pthread_mutex_unlock(&old_current->co_exit->m_state.thread_mutex);
		
		// kill this thread
		pthread_exit(NULL);
	}
}

//--------------------------------------------------------------------------------------------------
size_t dummy_stack_size = 0xFFFFFFFFL;
void stackmon_init(stack_t *stack, size_t stack_size){
	if(stack_size < dummy_stack_size){
		dummy_stack_size = stack_size;
	}
}

//--------------------------------------------------------------------------------------------------
size_t stackmon_get_unused(stack_t *stack){
	return(dummy_stack_size);
}

///\}
///\}

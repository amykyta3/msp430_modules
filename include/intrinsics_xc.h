/**
 * \file
 * \brief MSP430 Intrinsics cross-compatibility Header
 * \author Alex Mykyta
 * This include allows for code compatibility between the following compilers:
 *    - MSPGCC
 *    - TI Compiler
 * 
 * The following intrinsics are enforced by this header:
 * 
 *    - __no_operation()
 *    - __disable_interrupt()
 *    - __enable_interrupt()
 *    - __get_SR_register()
 *    - __get_interrupt_state ()
 *    - __set_interrupt_state (sv)
 *    - __get_SP_register()          or  _get_SP_register()
 *    - __set_SP_register(src)       or _set_SP_register(src)
 *    - __bic_SR_register(x)
 *    - __bis_SR_register(x)
 *    - __bic_SR_register_on_exit(x)
 *    - __bis_SR_register_on_exit(x)
 *    - __delay_cycles (delay)       or _delay_cycles (delay)
 *    - __swap_bytes (v)
 *    - __bcd_add_short(op1, op2)
 *    - __bcd_add_long(op1, op2)
 * 
 **/
#ifndef INTRINSICS_XC_H
#define INTRINSICS_XC_H

#ifndef MSP430_XC_H
    #error Do not include this file directly. Include msp430_xc.h instead
#endif
 
//--------------------------------------------------------------------------------------------------
#if defined(__GNUC__) && defined(__MSP430__)
    
    #define _delay_cycles(x)        __delay_cycles(x)
    
    // Disable incompatible TI intrinsics
    #define _never_executed
    
//--------------------------------------------------------------------------------------------------
#elif defined(__TI_COMPILER_VERSION__)
    
    #define __get_interrupt_state       _get_interrupt_state
    #define __set_interrupt_state(x)    _set_interrupt_state(x)


//--------------------------------------------------------------------------------------------------
#else
    #error "Compiler not supported."
#endif


#endif

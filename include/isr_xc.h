/*
 * Copyright (c) 2005 Steve Underwood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
* \file
* \brief MSP430 Cross compiler interrupt service routine compatibility definitions
* \author Steve Underwood
* \author Modified by: Alex Mykyta
* \date 7/11/2011
* 
* This code currently allows for:
*    - MSPGCC - the GNU tools for the MSP430
*    - IAR Version 1 (old syntax)
*    - IAR Versions 2 and 3 (new syntax)
*    - Rowley Crossworks
*    - Code Composer Studio 4
*    - Code Composer Studio 5
*
* These macros allow us to define interrupt routines for all compilers with a common syntax:
* \code
*    ISR(<interrupt vector>){
*        <routine>
*    }
* \endcode
* For example:
* \code
*    ISR(ADC12_VECTOR){
*        ADC12CTL0 &= ~ENC;
*        ADC12CTL0 |= ENC;
*    }
* \endcode
* 
* 
**/
 
#ifndef ISR_XC_H
#define ISR_XC_H

///\cond NODOC

#ifndef MSP430_XC_H
    #error Do not include this file directly. Include msp430_xc.h instead
#endif

/* A tricky #define to stringify _Pragma parameters */
#define __PRAGMA__(x) _Pragma(#x)


//==================================================================================================
// MSPGCC Compiler
//==================================================================================================
#if defined(__GNUC__)  &&  defined(__MSP430__)
    /* This is the MSPGCC compiler */
#define _ISR(a,b)    void __attribute__((interrupt (a##_VECTOR))) b(void)        // Edit by Alex Mykyta


//==================================================================================================
// IAR v1.xx Compiler
//==================================================================================================
#elif defined(__IAR_SYSTEMS_ICC__)  &&  (((__TID__ >> 8) & 0x7f) == 43)  &&  (__VER__ < 200)
    /* This is V1.xx of the IAR compiler. */
#define _ISR(a,b) interrupt[a ## _VECTOR] void b(void)


//==================================================================================================
// IAR v2.xx or v3.xx Compiler
//==================================================================================================
#elif defined(__IAR_SYSTEMS_ICC__)  &&  (((__TID__ >> 8) & 0x7f) == 43)  &&  (__VER__ < 400)
    /* This is V2.xx or V3.xx of the IAR compiler. */
#define _ISR(a,b) \
__PRAGMA__(vector=a ##_VECTOR) \
__interrupt void b(void)


//==================================================================================================
// Rowley Crossworks Compiler
//==================================================================================================
#elif defined(__CROSSWORKS_MSP430)
    /* This is the Rowley Crossworks compiler */
#define _ISR(a,b) void b(void) __interrupt[a##_VECTOR] // Edit by Alex Mykyta


//==================================================================================================
// TI CCS Compiler
//==================================================================================================
#elif defined(__TI_COMPILER_VERSION__)
    /* This is the Code Composer Studio compiler. */
#define _ISR(a,b) EMIT_PRAGMA(vector=a##_VECTOR)\
        __interrupt void b (void)

        
//==================================================================================================
#else
    #error Compiler not supported.
#endif

//==================================================================================================

#define ISR(x,...)    _ISR(x,isr_##x)

///\endcond

#endif

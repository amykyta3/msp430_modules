/*
* Copyright (c) 2013, Alexander I. Mykyta
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
* \addtogroup MOD_TIMER
* \{
**/

/**
* \file
* \brief Internal include for \ref MOD_TIMER
*    Abstracts register names between MSP430 devices
* \author Alex Mykyta 
**/

///\}

#ifndef TIMER_INTERNAL_H
#define TIMER_INTERNAL_H

#include <msp430_xc.h>
#include <stdint.h>






// Potential future support:
// __MSP430_HAS_TB3__
// __MSP430_HAS_TB7__
// __MSP430_HAS_T0B3__
// __MSP430_HAS_T0B7__
// __MSP430_HAS_T1B3__
// __MSP430_HAS_T2B3__



//==================================================================================================
// Device Abstraction
//==================================================================================================
#if TIMER_USE_DEV == 0
    #if (defined(__MSP430_HAS_TA3__ ) || \
         defined(__MSP430_HAS_TA5__ ) || \
         defined(__MSP430_HAS_T0A3__) || \
         defined(__MSP430_HAS_T0A5__))
    
        #define TMR_TCTL        TA0CTL
        #define TMR_TCCTL0      TA0CCTL0
        #define TMR_TCCTL1      TA0CCTL1
        #define TMR_TCCTL2      TA0CCTL2
        #define TMR_TR          TA0R
        #define TMR_TCCR0       TA0CCR0
        #define TMR_TCCR1       TA0CCR1
        #define TMR_TCCR2       TA0CCR2
        #define TMR_TIV         TA0IV
        #if defined(TA0EX0)
            #define TMR_TEX0    TA0EX0
        #endif
        
        #define TMR_TIMER_ISR_VECTOR    TIMER0_A0_VECTOR
    #else
        #error "Invalid TIMER_USE_DEV in timer_config.h"
    #endif
//--------------------------------------------------------------------------------------------------
#elif TIMER_USE_DEV == 1
    #if (defined(__MSP430_HAS_T1A3__) || \
         defined(__MSP430_HAS_T1A5__))
    
        #define TMR_TCTL        TA1CTL
        #define TMR_TCCTL0      TA1CCTL0
        #define TMR_TCCTL1      TA1CCTL1
        #define TMR_TCCTL2      TA1CCTL2
        #define TMR_TR          TA1R
        #define TMR_TCCR0       TA1CCR0
        #define TMR_TCCR1       TA1CCR1
        #define TMR_TCCR2       TA1CCR2
        #define TMR_TIV         TA1IV
        #if defined(TA1EX0)
            #define TMR_TEX0    TA1EX0
        #endif
        
        #define TMR_TIMER_ISR_VECTOR    TIMER1_A0_VECTOR
    
    #else
        #error "Invalid TIMER_USE_DEV in timer_config.h"
    #endif
//--------------------------------------------------------------------------------------------------
#elif TIMER_USE_DEV == 2
    #if (defined(__MSP430_HAS_T2A3__))
    
        #define TMR_TCTL        TA2CTL
        #define TMR_TCCTL0      TA2CCTL0
        #define TMR_TCCTL1      TA2CCTL1
        #define TMR_TCCTL2      TA2CCTL2
        #define TMR_TR          TA2R
        #define TMR_TCCR0       TA2CCR0
        #define TMR_TCCR1       TA2CCR1
        #define TMR_TCCR2       TA2CCR2
        #define TMR_TIV         TA2IV
        #if defined(TA2EX0)
            #define TMR_TEX0    TA2EX0
        #endif
        
        #define TMR_TIMER_ISR_VECTOR    TIMER2_A0_VECTOR
    
    #else
        #error "Invalid TIMER_USE_DEV in timer_config.h"
    #endif
//--------------------------------------------------------------------------------------------------
#else
    #error "Invalid TIMER_USE_DEV in timer_config.h"
#endif


//==================================================================================================
// Clock Setup
//==================================================================================================

#if TIMER_CLK_SRC == 1
    // ACLK
    #define TMR_FCLK    ACLK_FREQ
#elif TIMER_CLK_SRC == 2
    // SMCLK
    #define TMR_FCLK    SMCLK_FREQ
#else
    #error "Invalid TIMER_CLK_SRC in timer_config.h"
#endif

#if defined(TMR_TEX0)
    #define TMR_FCLKDIV        (TMR_FCLK/((1<<TIMER_IDIV)*(TIMER_IDIVEX+1)))
#else
    #define TMR_FCLKDIV        (TMR_FCLK/(1<<TIMER_IDIV))
#endif



//==================================================================================================
// Declarations
//==================================================================================================

#define TMR_INTERVAL_MIN    ((1000L/TMR_FCLK)+1)
#define TMR_INTERVAL_MAX    (((0xFFFFFFFFUL)/TMR_FCLK)*1000UL)


//--------------------------------------------------------------------------------------------------
#endif /*_TIMER_INTERNAL_H_*/

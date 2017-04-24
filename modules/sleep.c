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
* Alex M.       2011-08-01   born
* Alex M.       2012-02-22   Portions rewritten in ASM for reliability
* 
*=================================================================================================*/

/**
* \addtogroup MOD_SLEEP
* \{
**/

/**
* \file
* \brief Code for \ref MOD_SLEEP "Sleep"
* \author Alex Mykyta 
* 
* 
**/



#include <stdint.h>
#include <msp430_xc.h>

#include "clock_sys.h"
#include "sleep.h"

#define USE_ASM_SPINNING    1
//--------------------------------------------------------------------------------------------------
#if(USE_ASM_SPINNING == 1)
    #if defined(__GNUC__) && defined(__MSP430__)
        #if(defined(__MSPGCC__) && (__MSPGCC__ <= 20120406))
            // Peter Bigot's MSPGCC
            #define ARG32_MSB    "R15"
            #define ARG32_LSB    "R14"
        #else
            // Red Hat's MSPGCC-ELF
            #define ARG32_MSB    "R13"
            #define ARG32_LSB    "R12"
        #endif
    #elif defined(__TI_COMPILER_VERSION__)
        // TI's Code Composer Studio
        #define ARG32_MSB    "R13"
        #define ARG32_LSB    "R12"
        
    #else
        #error "Compiler not supported yet."
    #endif
    
    static void asmspin(uint32_t count){
        // count SHOULD always be located at R13:R12 when compiling with CCS
        // count SHOULD always be located at R15:R14 when compiling with MSPGCC
        
        // Loop until count == 0 (8 cycles/loop)
        __asm__("  nop             ");        // 1 // Loop1:
        __asm__("  nop             ");        // 1
        __asm__("  nop             ");        // 1
        __asm__("  nop             ");        // 1 // Loop2:
        __asm__("  dec.w " ARG32_LSB);        // 1
        __asm__("  sbc.w " ARG32_MSB);        // 1
        __asm__("  jnz $-12        ");        // 2 // jnz Loop1
        __asm__("  tst.w " ARG32_LSB);        // 1
        __asm__("  jnz $-10        ");        // 2 // jnz Loop2
    }
#endif

//--------------------------------------------------------------------------------------------------
void usleep(register uint16_t us){
    register volatile uint32_t count;
    register uint32_t tmp;
    
    // fast approximation of:
    // count = (us*MCLK_FREQ)/(8*1000000)
    count = us; //8
    
    tmp = MCLK_FREQ >> 10; //76
    tmp = tmp * count; // 102
    tmp >>= 13; // 136
    count = tmp; // 138
    tmp >>= 5; // 156
    count += tmp; // 158
    tmp >>= 1; // 160
    count += tmp; // 162
        
    #if(USE_ASM_SPINNING == 1)
        if(count <= 31) return; //169
        count = count - 31; //172; 172/8 = 21; adjusted to 31
        
        asmspin(count);
    #else
        if(count <= 25) return; //169
        count = count - 25; //172; 172/8 = 21; adjusted to 25
        
        while(count){ // 8 cyc per loop
            count--;    
        }
    #endif
    
}

//--------------------------------------------------------------------------------------------------
void msleep(register uint16_t ms){
    while(ms != 0){
        usleep(1000);
        ms--;
    }
}
//--------------------------------------------------------------------------------------------------
void sleep(register uint16_t s){
    while(s != 0){
        msleep(1000);
        s--;
    }
}
//--------------------------------------------------------------------------------------------------
///\}

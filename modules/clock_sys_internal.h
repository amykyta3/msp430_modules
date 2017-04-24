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
* \addtogroup MOD_CLOCKSYS
* \{
**/

/**
* \file
* \brief Internal include for \ref MOD_CLOCKSYS "Clock System".
* \author Alex Mykyta 
**/

///\}

#ifndef CLOCK_SYS_INTERNAL_H
#define CLOCK_SYS_INTERNAL_H

//##################################################################################################
//# Basic Clock Module                                                                             #
//# 1xx Devices                                                                                    #
//##################################################################################################
/* Notes:
 *  - ACLK is always from XT1
 *  - 11xx and 12xx devices don't have XT2
 *  - SMCLK is only from the DCO or XT2
 *      - Except for 11xx and 12xx. SMCLK uses DCO or XT1
 * 
 */
#ifdef __MSP430_HAS_BASIC_CLOCK__
    
// check for incorrect routing settings
#if(ACLK_SRC != 1) // XT1
    #error Invalid ACLK_SRC
#endif

#if(SMCLK_SRC == 0) // DCO
#elif(SMCLK_SRC == 2) // XT2
#else
    #error Invalid SMCLK_SRC
#endif

#if(MCLK_SRC == 0) // DCO
#elif(MCLK_SRC == 1) // XT1
#elif(MCLK_SRC == 2) // XT2
#else
    #error Invalid MCLK_SRC
#endif

// what sources are being used?
// DCO
#if((SMCLK_SRC == 0)||(MCLK_SRC == 0))
    #define _USING_DCO
#endif

// XT1
#if(XT1_FREQ != 0)
    #define _USING_XT1
#endif

// XT2
#if((SMCLK_SRC == 2)||(MCLK_SRC == 2))
    #define _USING_XT2
#endif

/*--------------------------------------------------------------------------------------------------
*  DCO Setup:
* 
*  Resulting Definitions:
*     DCO_RSEL
*     DCO_DCO
*     DCO_MOD
*     
*     DCO_FREQ
*/
#ifdef _USING_DCO
    #if(MANUALLY_CONFIG_DCO)
        
        #if(MANUAL_DCO_RSEL > 7)
            #error Invalid MANUAL_DCO_RSEL
        #endif
        
        #if(MANUAL_DCO_DCO > 7)
            #error Invalid MANUAL_DCO_DCO
        #endif
        
        #if(MANUAL_DCO_MOD > 31)
            #error Invalid MANUAL_DCO_MOD
        #endif
        
        #define DCO_RSEL    MANUAL_DCO_RSEL
        #define DCO_DCO     MANUAL_DCO_DCO
        #define DCO_MOD     MANUAL_DCO_MOD
        
        #define DCO_FREQ    TARGET_DCO_FREQ
        
    #else
        #error Automatic DCO configuration is not available for 1xx devices
    #endif
#endif


// ACLK
#define        _ACLK_FREQ        (XT1_FREQ<<ACLK_DIV)

// SMCLK
#if (SMCLK_SRC == 0) // DCO
    #define        _SMCLK_FREQ      (DCO_FREQ<<SMCLK_DIV)
#elif (SMCLK_SRC == 2) // XT2
    #define        _SMCLK_FREQ      (XT2_FREQ<<SMCLK_DIV)
#else
    #error "Invalid SMCLK_SRC"
#endif

// MCLK
#if (MCLK_SRC == 0) // DCO
    #define        _MCLK_FREQ        (DCO_FREQ<<MCLK_DIV)
#elif (MCLK_SRC == 1) // XT1
    #define        _MCLK_FREQ        (XT1_FREQ<<MCLK_DIV)
#elif (MCLK_SRC == 2) // XT2
    #define        _MCLK_FREQ        (XT2_FREQ<<MCLK_DIV)
#else
    #error "Invalid MCLK_SRC"
#endif

#endif

//##################################################################################################
//# Basic Clock Module+                                                                            #
//# 2xx Devices                                                                                    #
//##################################################################################################
#ifdef __MSP430_HAS_BC2__
    #error "Clock System not implemented yet"
#endif

//##################################################################################################
//# FLL+ Clock Module                                                                              #
//# 4xx Devices                                                                                    #
//##################################################################################################
/* Notes:
 *  - ACLK is always from XT1 or VLO
 *  - SMCLK is only from the DCO or XT2
 *  - MCLK can get XT1, XT2, VLO or DCO... sometimes
 * 
 * only 41x2 has a VLO. XT1 can't be used at the same time.
 *         #ifdef LF1XT1S_2
 * 
 * 41x and 42x do not have an XT2
 *         #ifdef XT2OFF
 * 
 * 41x and 42x do not have SELM except for 41x2
 *         MCLK is always from the DCO
 *         #ifdef SELM0
 *         
 * 47x3/4, 471xx have an XT2S setting for XT2
 *         #ifdef XT2S_0
 * 
 */
#ifdef __MSP430_HAS_FLLPLUS__

// check for incorrect routing settings
#if(ACLK_SRC == 1) // XT1
#elif(ACLK_SRC == 3) // VLO
    #ifndef LF1XT1S_2 // if doesn't have a VLO
        #error Invalid ACLK_SRC: This device does not have a VLO
    #endif
#else
    #error Invalid ACLK_SRC
#endif

#if(SMCLK_SRC == 0) // DCO
#elif(SMCLK_SRC == 2) // XT2
    #ifndef XT2OFF // if doesn't have an XT2
        #error Invalid SMCLK_SRC: This device does not have an XT2
    #endif
#else
    #error Invalid SMCLK_SRC
#endif

#if(MCLK_SRC == 0) // DCO
#elif(MCLK_SRC == 1) // XT1
    #ifndef SELM0
        #error Invalid MCLK_SRC
    #endif
#elif(MCLK_SRC == 2) // XT2
    #ifndef XT2OFF // if doesn't have an XT2
        #error Invalid MCLK_SRC: This device does not have an XT2
    #else
        #ifndef SELM0
            #error Invalid MCLK_SRC
        #endif
    #endif
#elif(MCLK_SRC == 3) // VLO
    #ifndef LF1XT1S_2 // if doesn't have a VLO
        #error Invalid MCLK_SRC: This device does not have a VLO
    #else
        #ifndef SELM0
            #error Invalid MCLK_SRC
        #endif
    #endif
#else
    #error Invalid MCLK_SRC
#endif



// what sources are being used?
// DCO
#if((SMCLK_SRC == 0)||(MCLK_SRC == 0))
    #define _USING_DCO
#endif

// XT1
#if((ACLK_SRC == 1)||(MCLK_SRC == 1)||(defined(_USING_DCO) && (FLL_REF_SRC == 0)))
    #define _USING_XT1
#endif

// XT2
#if((SMCLK_SRC == 2)||(MCLK_SRC == 2))
    #define _USING_XT2
#endif

// VLO
#if((ACLK_SRC == 3)||(MCLK_SRC == 3)||(defined(_USING_DCO) && (FLL_REF_SRC == 2)))
    #ifndef _USING_XT1
        #define _USING_VLO
    #else
        #error XT1 and VLO cant be used at the same time.
    #endif
#endif


/*--------------------------------------------------------------------------------------------------
*  DCO & FLL Setup:
* 
*  Resulting Definitions:
*     FLL_FN
*     FLL_FLLD
*     FLL_N
*     
*     DCO_FREQ
*/
#ifdef _USING_DCO
    // DCO always uses DCOPLUS=1
    
    #ifdef _USING_XT1
        #define FLL_REF_FREQ    XT1_FREQ
    #else
        #define FLL_REF_FREQ    12000L
    #endif
    
    #if(MANUALLY_CONFIG_DCO)
        #if(MANUAL_FLLPLUS_N > 127)
            #error Invalid MANUAL_FLLPLUS_N
        #endif
        
        #if(MANUAL_FLLPLUS_FLLD > 3)
            #error Invalid MANUAL_FLLPLUS_FLLD
        #endif
        
        #define FLL_FLLD    (MANUAL_FLLPLUS_FLLD << 6)
        #define FLL_N        MANUAL_FLLPLUS_N
        #define DCO_FREQ    (FLL_REF_FREQ*(FLL_N+1)*(1<<MANUAL_FLLPLUS_FLLD))
    #else
        #if(((TARGET_DCO_FREQ/(FLL_REF_FREQ*1))-1) <= 127)
            #define FLL_FLLD    FLLD_1
            #define FLL_N        ((TARGET_DCO_FREQ/(FLL_REF_FREQ*1))-1)
            #define DCO_FREQ    (FLL_REF_FREQ*(FLL_N+1)*1)
        #elif(((TARGET_DCO_FREQ/(FLL_REF_FREQ*2))-1) <= 127)
            #define FLL_FLLD    FLLD_2
            #define FLL_N        ((TARGET_DCO_FREQ/(FLL_REF_FREQ*2))-1)
            #define DCO_FREQ    (FLL_REF_FREQ*(FLL_N+1)*2)
        #elif(((TARGET_DCO_FREQ/(FLL_REF_FREQ*4))-1) <= 127)
            #define FLL_FLLD    FLLD_4
            #define FLL_N        ((TARGET_DCO_FREQ/(FLL_REF_FREQ*4))-1)
            #define DCO_FREQ    (FLL_REF_FREQ*(FLL_N+1)*4)
        #elif(((TARGET_DCO_FREQ/(FLL_REF_FREQ*8))-1) <= 127)
            #define FLL_FLLD    FLLD_8
            #define FLL_N        ((TARGET_DCO_FREQ/(FLL_REF_FREQ*8))-1)
            #define DCO_FREQ    (FLL_REF_FREQ*(FLL_N+1)*8)
        #else
            #error TARGET_DCO_FREQ is outside of the feasible range 
        #endif
    #endif
    
    #if((DCO_FREQ >= 650000L) && (DCO_FREQ < 2820000))
        #define FLL_FN    0        // 0.65 - 6.1 MHz
    #elif((DCO_FREQ >= 2820000L) && (DCO_FREQ < 4920000L))
        #define FLL_FN    FN_2    // 1.3 - 12.1 MHz
    #elif((DCO_FREQ >= 4920000L) && (DCO_FREQ < 7080000L))
        #define FLL_FN    FN_3    // 2.0 - 17.9 MHz
    #elif((DCO_FREQ >= 7080000L) && (DCO_FREQ < 10570000L))
        #define FLL_FN    FN_4    // 2.8 - 26.6 MHz
    #elif((DCO_FREQ >= 10570000L) && (DCO_FREQ < 46000000L))
        #define FLL_FN    FN_8    // 4.2 - 46.0 MHz
    #else
        #error Resulting DCO frequency is outside of the FLLs range 
    #endif
#endif


// ACLK
#if (ACLK_SRC == 1) // XT1
    #define        _ACLK_FREQ        XT1_FREQ
#elif (ACLK_SRC == 3) // VLO
    #define        _ACLK_FREQ        12000L
#else
    #error "Invalid ACLK_SRC"
#endif

// SMCLK
#if (SMCLK_SRC == 0) // DCO
    #define        _SMCLK_FREQ        DCO_FREQ
#elif (SMCLK_SRC == 2) // XT2
    #define        _SMCLK_FREQ        XT2_FREQ
#else
    #error "Invalid SMCLK_SRC"
#endif

// MCLK
#if (MCLK_SRC == 0) // DCO
    #define        _MCLK_FREQ        DCO_FREQ
#elif (MCLK_SRC == 1) // XT1
    #define        _MCLK_FREQ        XT1_FREQ
#elif (MCLK_SRC == 2) // XT2
    #define        _MCLK_FREQ        XT2_FREQ
#elif (MCLK_SRC == 3) // VLO
    #define        _MCLK_FREQ        12000L
#else
    #error "Invalid MCLK_SRC"
#endif



#endif

//##################################################################################################
//# Unified Clock System (UCS)                                                                     #
//# 5xx and 6xx Devices                                                                            #
//##################################################################################################
#ifdef __MSP430_HAS_UCS__

#if (XT1_FREQ != 0)
    #define _HAS_XT1
#endif

#if (XT2_FREQ != 0)
    #define _HAS_XT2
#endif

#if ((ACLK_SRC == 0)||(SMCLK_SRC == 0)||(MCLK_SRC == 0))
    #define _USING_DCO
#endif

#if ((ACLK_SRC == 1)||(SMCLK_SRC == 1)||(MCLK_SRC == 1))
    #ifdef _HAS_XT1
        #define _USING_XT1
    #else
        #error "XT1 Is not available!"
    #endif
#endif

#if ((ACLK_SRC == 2)||(SMCLK_SRC == 2)||(MCLK_SRC == 2))
    #ifdef _HAS_XT2
        #define _USING_XT2
    #else
        #error "XT2 Is not available!"
    #endif
#endif

/*--------------------------------------------------------------------------------------------------
*  UCS DCO & FLL Setup:
* 
*  Resulting Definitions:
*     FLL_FLLREFDIV
*     FLL_SELREF
*     FLL_FLLD
*     FLL_FLLN
* 
*     DCO_FREQ
*/    
#ifdef _USING_DCO
    #if(FLL_REF_SRC == 0)
        // XT1
        #ifdef _HAS_XT1
            #define FLL_SELREF    SELREF__XT1CLK
            #define _USING_XT1
            #define FLL_REF_FREQ    XT1_FREQ
        #else
            #error "Invalid FLL_REF_SRC"
        #endif
    #elif(FLL_REF_SRC == 1)
        // XT2
        #ifdef _HAS_XT2
            #define FLL_SELREF    SELREF__XT2CLK
            #define _USING_XT2
            #define FLL_REF_FREQ    XT2_FREQ
        #else
            #error "Invalid FLL_REF_SRC"
        #endif
    #elif(FLL_REF_SRC == 3)
        // REFO
        #define FLL_SELREF    SELREF__REFOCLK
        #define FLL_REF_FREQ    (32768L)
    #else
        #error "Invalid FLL_REF_SRC"
    #endif
    
    #if(MANUALLY_CONFIG_DCO == 0)
        
        #define _CALC_FLLN(rd, ld)    (((rd)*TARGET_DCO_FREQ)/(FLL_REF_FREQ*(ld))-1)
        #define _CALC_FREQ(rd, ld)    (((ld)*(_CALC_FLLN(rd, ld)+1)*FLL_REF_FREQ)/(rd))

        #if(_CALC_FLLN(16, 1) == 0)
            #error Desired target DCO frequency is out of feasible range!
        #endif

        #if(_CALC_FLLN(16, 1) <= 511)
            #if(_CALC_FREQ(16,1) > TARGET_DCO_FREQ)
                #define FRQ_ERROR    (_CALC_FREQ(16,1) - TARGET_DCO_FREQ)
            #else
                #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(16,1))
            #endif
        #else
            #define FRQ_ERROR    TARGET_DCO_FREQ
        #endif

        // Try REFDIV=12, FLLD=1
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 1) <= 511)
                #if(_CALC_FREQ(12,1) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,1) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(16,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__16
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(16,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,1) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,1))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(16,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__16
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(16,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,1))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=8, FLLD=1
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(8, 1) <= 511)
                #if(_CALC_FREQ(8,1) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(8,1) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(12,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(8,1) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(8,1))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(12,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(8,1))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=12, FLLD=2
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 2) <= 511)
                #if(_CALC_FREQ(12,2) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,2) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(8,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__8
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(8,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,2) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,2))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(8,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__8
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(8,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,2))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=4, FLLD=1
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(4, 1) <= 511)
                #if(_CALC_FREQ(4,1) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(4,1) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,2)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__2
                        #define DCO_FREQ        _CALC_FREQ(12,2)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(4,1) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(4,1))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,2)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__2
                        #define DCO_FREQ        _CALC_FREQ(12,2)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(4,1))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=12, FLLD=4
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 4) <= 511)
                #if(_CALC_FREQ(12,4) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,4) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(4,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__4
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(4,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,4) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,4))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(4,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__4
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(4,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,4))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=2, FLLD=1
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(2, 1) <= 511)
                #if(_CALC_FREQ(2,1) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(2,1) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,4)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__4
                        #define DCO_FREQ        _CALC_FREQ(12,4)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(2,1) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(2,1))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,4)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__4
                        #define DCO_FREQ        _CALC_FREQ(12,4)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(2,1))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=12, FLLD=8
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 8) <= 511)
                #if(_CALC_FREQ(12,8) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,8) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(2,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__2
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(2,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,8) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,8))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(2,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__2
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(2,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,8))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=1
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 1) <= 511)
                #if(_CALC_FREQ(1,1) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,1) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,8)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__8
                        #define DCO_FREQ        _CALC_FREQ(12,8)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(1,1) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,1))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,8)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__8
                        #define DCO_FREQ        _CALC_FREQ(12,8)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(1,1))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=12, FLLD=16
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 16) <= 511)
                #if(_CALC_FREQ(12,16) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,16) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(1,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,16) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,16))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,1)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__1
                        #define DCO_FREQ        _CALC_FREQ(1,1)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,16))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=2
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 2) <= 511)
                #if(_CALC_FREQ(1,2) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,2) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,16)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__16
                        #define DCO_FREQ        _CALC_FREQ(12,16)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(1,2) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,2))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,16)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__16
                        #define DCO_FREQ        _CALC_FREQ(12,16)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(1,2))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=12, FLLD=32
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(12, 32) <= 511)
                #if(_CALC_FREQ(12,32) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(12,32) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,2)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__2
                        #define DCO_FREQ        _CALC_FREQ(1,2)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(12,32) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(12,32))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,2)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__2
                        #define DCO_FREQ        _CALC_FREQ(1,2)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(12,32))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=4
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 4) <= 511)
                #if(_CALC_FREQ(1,4) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,4) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,32)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__32
                        #define DCO_FREQ        _CALC_FREQ(12,32)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(1,4) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,4))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(12,32)
                        #define FLL_FLLREFDIV    FLLREFDIV__12
                        #define FLL_FLLD        FLLD__32
                        #define DCO_FREQ        _CALC_FREQ(12,32)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(1,4))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=8
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 8) <= 511)
                #if(_CALC_FREQ(1,8) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,8) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,4)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__4
                        #define DCO_FREQ        _CALC_FREQ(1,4)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(1,8) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,8))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,4)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__4
                        #define DCO_FREQ        _CALC_FREQ(1,4)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(1,8))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=16
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 16) <= 511)
                #if(_CALC_FREQ(1,16) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,16) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,8)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__8
                        #define DCO_FREQ        _CALC_FREQ(1,8)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (_CALC_FREQ(1,16) - TARGET_DCO_FREQ)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,16))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,8)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__8
                        #define DCO_FREQ        _CALC_FREQ(1,8)
                    #else // otherwise update the error
                        #undef FRQ_ERROR
                        #define FRQ_ERROR    (TARGET_DCO_FREQ - _CALC_FREQ(1,16))
                    #endif
                #endif
            #endif
        #endif

        // Try REFDIV=1, FLLD=32
        #ifndef FLL_FLLD
            #if(_CALC_FLLN(1, 32) <= 511)
                #if(_CALC_FREQ(1,32) > TARGET_DCO_FREQ) // if error is positive
                    #if(FRQ_ERROR < (_CALC_FREQ(1,32) - TARGET_DCO_FREQ)) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,16)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__16
                        #define DCO_FREQ        _CALC_FREQ(1,16)
                    #else // otherwise update the error
                        #define FLL_FLLN        _CALC_FLLN(1,32)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__32
                        #define DCO_FREQ        _CALC_FREQ(1,32)
                    #endif
                #else // if error is negative
                    #if(FRQ_ERROR < (TARGET_DCO_FREQ - _CALC_FREQ(1,32))) // if prev error is better, use those values
                        #define FLL_FLLN        _CALC_FLLN(1,16)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__16
                        #define DCO_FREQ        _CALC_FREQ(1,16)
                    #else // otherwise update the error
                        #define FLL_FLLN        _CALC_FLLN(1,32)
                        #define FLL_FLLREFDIV    FLLREFDIV__1
                        #define FLL_FLLD        FLLD__32
                        #define DCO_FREQ        _CALC_FREQ(1,32)
                    #endif
                #endif
            #else
                #error Desired target DCO frequency is out of feasible range!
            #endif
        #endif
        
    #else //(MANUALLY_CONFIG_DCO != 0)
        
        #define FLL_FLLD        MANUAL_FLLD
        #define FLL_FLLN        MANUAL_FLLN

        #if (MANUAL_FLLREFDIV == 0)
            #define DCO_FREQ        (FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))
            #define FLL_FLLREFDIV    FLLREFDIV__1
        #elif (MANUAL_FLLREFDIV == 1)
            #define DCO_FREQ        ((FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))/2)
            #define FLL_FLLREFDIV    FLLREFDIV__2
        #elif (MANUAL_FLLREFDIV == 2)
            #define DCO_FREQ        ((FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))/4)
            #define FLL_FLLREFDIV    FLLREFDIV__4
        #elif (MANUAL_FLLREFDIV == 3)
            #define DCO_FREQ        ((FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))/8)
            #define FLL_FLLREFDIV    FLLREFDIV__8
        #elif (MANUAL_FLLREFDIV == 4)
            #define DCO_FREQ        ((FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))/12)
            #define FLL_FLLREFDIV    FLLREFDIV__12
        #elif (MANUAL_FLLREFDIV == 5)
            #define DCO_FREQ        ((FLL_REF_FREQ * (FLL_FLLN+1) * (1 << FLL_FLLD))/16)
            #define FLL_FLLREFDIV    FLLREFDIV__16
        #else
            #error "Invalid MANUAL_FLLREFDIV"
        #endif
        
    #endif
#endif

//--------------------------------------------------------------------------------------------------

// ACLK
#if (ACLK_SRC == 0)
    #define        _ACLK_FREQ        (DCO_FREQ >> ACLK_DIV)
    #define        _SELA            SELA__DCOCLK
#elif (ACLK_SRC == 1)
    #define        _ACLK_FREQ        (XT1_FREQ >> ACLK_DIV)
    #define        _SELA            SELA__XT1CLK
#elif (ACLK_SRC == 2)
    #define        _ACLK_FREQ        (XT2_FREQ >> ACLK_DIV)
    #define        _SELA            SELA__XT2CLK
#elif (ACLK_SRC == 3)
    #define        _ACLK_FREQ        ((10000L) >> ACLK_DIV)
    #define        _SELA            SELA__VLOCLK
#elif (ACLK_SRC == 4)
    #define        _ACLK_FREQ        ((32768L) >> ACLK_DIV)
    #define        _SELA            SELA__REFOCLK
#else
    #error "Invalid ACLK_SRC"
#endif

// SMCLK
#if (SMCLK_SRC == 0)
    #define        _SMCLK_FREQ        (DCO_FREQ >> SMCLK_DIV)
    #define        _SELS            SELS__DCOCLK
#elif (SMCLK_SRC == 1)
    #define        _SMCLK_FREQ        (XT1_FREQ >> SMCLK_DIV)
    #define        _SELS            SELS__XT1CLK
#elif (SMCLK_SRC == 2)
    #define        _SMCLK_FREQ        (XT2_FREQ >> SMCLK_DIV)
    #define        _SELS            SELS__XT2CLK
#elif (SMCLK_SRC == 3)
    #define        _SMCLK_FREQ        ((10000L) >> SMCLK_DIV)
    #define        _SELS            SELS__VLOCLK
#elif (SMCLK_SRC == 4)
    #define        _SMCLK_FREQ        ((32768L) >> SMCLK_DIV)
    #define        _SELS            SELS__REFOCLK
#else
    #error "Invalid SMCLK_SRC"
#endif

// MCLK
#if (MCLK_SRC == 0)
    #define        _MCLK_BASE_FREQ    DCO_FREQ
    #define        _MCLK_FREQ        (DCO_FREQ >> (UCSCTL5 & 0x07))
    #define        _SELM            SELM__DCOCLK
#elif (MCLK_SRC == 1)
    #define        _MCLK_BASE_FREQ    XT1_FREQ
    #define        _MCLK_FREQ        (XT1_FREQ >> (UCSCTL5 & 0x07))
    #define        _SELM            SELM__XT1CLK
#elif (MCLK_SRC == 2)
    #define        _MCLK_BASE_FREQ    XT2_FREQ
    #define        _MCLK_FREQ        (XT2_FREQ >> (UCSCTL5 & 0x07))
    #define        _SELM            SELM__XT2CLK
#elif (MCLK_SRC == 3)
    #define        _MCLK_BASE_FREQ    (10000L)
    #define        _MCLK_FREQ        ((10000L) >> (UCSCTL5 & 0x07))
    #define        _SELM            SELM__VLOCLK
#elif (MCLK_SRC == 4)
    #define        _MCLK_BASE_FREQ    (32768L)
    #define        _MCLK_FREQ        ((32768L) >> (UCSCTL5 & 0x07))
    #define        _SELM            SELM__REFOCLK
#else
    #error "Invalid MCLK_SRC"
#endif

// XCAP calculation
#ifdef _USING_XT1
    #if (XT1_FREQ < 40000L)
        #define LFXT_REQUIRED_CAP    (20*LFXT_LOAD_CAP) // Includes pin cap. Ignores any trace cap.
                                  // ^^ Multiplied by 10 to include tenths digit in calculation
        // Typical 5xx and 6xx caps available:
        //    XCAP_0 = 2.0
        //    XCAP_1 = 5.5
        //    XCAP_2 = 8.5
        //    XCAP_3 = 12.0
        
        // Midpoints derived from above:
        //    3.75
        //    7
        //    10.25
        
        #if (LFXT_REQUIRED_CAP <= 37)
            #define XT1_XCAP    XCAP_0
        #elif (LFXT_REQUIRED_CAP <= 70)
            #define XT1_XCAP    XCAP_1
        #elif (LFXT_REQUIRED_CAP <= 102)
            #define XT1_XCAP    XCAP_2
        #else
            #define XT1_XCAP    XCAP_3
        #endif
        
    #endif
#endif

// Maximum system frequency
#if ((_ACLK_FREQ > _SMCLK_FREQ) && (_ACLK_FREQ > _MCLK_BASE_FREQ))
    // ACLK is highest
    #define _MAX_SYS_FREQ    _ACLK_FREQ
#elif ((_SMCLK_FREQ > _ACLK_FREQ) && (_SMCLK_FREQ > _MCLK_BASE_FREQ))
    // SMCLK is highest
    #define _MAX_SYS_FREQ    _SMCLK_FREQ
#else
    // MCLK can be highest
    #define _MAX_SYS_FREQ    _MCLK_BASE_FREQ
#endif

// cleanup
#ifndef XT1HFOFFG
    #define XT1HFOFFG    0x0000
#endif

#ifndef XT2OFFG
    #define XT2OFFG    0x0000
#endif

#define        _MCLK_MAXLEVEL    5

//--------------------------------------------------------------------------------------------------
#endif

//##################################################################################################
#endif

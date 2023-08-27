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
* Alex M.       2012-07-09   born
* Alex M.       2014-01-28   Faster conversion for 32-bit decimals.
* 
*=================================================================================================*/

/**
* \addtogroup MOD_STRING_EXT
* \{
**/

/**
* \file
* \brief Code for \ref MOD_STRING_EXT "Extended String Functions"
* \author Alex Mykyta 
**/

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "string_ext.h"

//--------------------------------------------------------------------------------------------------
///\cond INTERNAL

#if defined(__TI_COMPILER_VERSION__)
    #define USE_DADD_ENHANCEMENT    1
#elif defined(__GNUC__) && defined(__MSP430__)
    #define USE_DADD_ENHANCEMENT    1
#else
    #define USE_DADD_ENHANCEMENT    0
#endif

//--------------------------------------------------------------------------------------------------

#if USE_DADD_ENHANCEMENT
    #include <msp430_xc.h>
    static uint16_t u8_to_bcd(uint8_t n){
        uint16_t bcd;
        uint16_t bcd_weight;
        
        bcd = 0;
        bcd_weight = 1;
        
        while(n){
            if(n & 0x01){
                bcd = __bcd_add_short(bcd, bcd_weight);
            }
            
            bcd_weight = __bcd_add_short(bcd_weight,bcd_weight); // double bcd_weight
            n >>= 1;
        }
        
        return(bcd);
    }

    //----------------------------------------------------------------------------------------------
    static uint32_t u16_to_bcd(uint16_t n){
        uint32_t bcd;
        uint32_t bcd_weight;
        
        bcd = 0;
        bcd_weight = 1;
        
        while(n){
            if(n & 0x0001){
                bcd = __bcd_add_long(bcd, bcd_weight);
            }
            
            bcd_weight = __bcd_add_long(bcd_weight,bcd_weight); // double bcd_weight
            n >>= 1;
        }
        
        return(bcd);
    }
#else
    // MSP430 BCD add instruction not available. Use "double-dabble" method
    static uint16_t u8_to_bcd(uint8_t n){
        uint16_t bcd;
        
        uint8_t bit_mask;
        
        bcd = 0;
        bit_mask = 0x80;
        do{
            
            // Check all bcd digits. If a digit is greater than 4, add 3 to it.
            uint16_t digit_mask;
            uint16_t digit_compare;
            uint16_t digit_add;
            digit_mask = 0xF00;
            digit_compare = 0x400;
            digit_add = 0x300;
            do{
                if((bcd & digit_mask) > digit_compare){
                    bcd += digit_add;
                }
                
                digit_mask >>= 4;
                digit_compare >>= 4;
                digit_add >>= 4;
            } while(digit_mask != 0);
            
            // shift upper bit of n into lower bit of bcd
            bcd <<= 1;
            if(n & bit_mask){
                bcd |= 1;
            }
            
            bit_mask >>= 1;
        } while(bit_mask != 0);
        
        return(bcd);
    }
    
    //----------------------------------------------------------------------------------------------
    static uint32_t u16_to_bcd(uint16_t n){
        uint32_t bcd;
        
        uint16_t bit_mask;
        
        bcd = 0;
        bit_mask = 0x8000;
        do{
            
            // Check all bcd digits. If a digit is greater than 4, add 3 to it.
            uint32_t digit_mask;
            uint32_t digit_compare;
            uint32_t digit_add;
            digit_mask = 0xF0000L;
            digit_compare = 0x40000L;
            digit_add = 0x30000L;
            do{
                if((bcd & digit_mask) > digit_compare){
                    bcd += digit_add;
                }
                
                digit_mask >>= 4;
                digit_compare >>= 4;
                digit_add >>= 4;
            } while(digit_mask != 0);
            
            // shift upper bit of n into lower bit of bcd
            bcd <<= 1;
            if(n & bit_mask){
                bcd |= 1;
            }
            
            bit_mask >>= 1;
        } while(bit_mask != 0);
        
        return(bcd);
    }

#endif

// 32-bit numbers always use double-dabble
static uint64_t u32_to_bcd(uint32_t n){
    uint64_t bcd;
    
    uint32_t bit_mask;
    
    bcd = 0;
    bit_mask = 0x80000000L;
    do{
        
        // Check all bcd digits. If a digit is greater than 4, add 3 to it.
        uint64_t digit_mask;
        uint64_t digit_compare;
        uint64_t digit_add;
        digit_mask = 0xF000000000L;
        digit_compare = 0x4000000000L;
        digit_add = 0x3000000000L;
        do{
            if((bcd & digit_mask) > digit_compare){
                bcd += digit_add;
            }
            
            digit_mask >>= 4;
            digit_compare >>= 4;
            digit_add >>= 4;
        } while(digit_mask != 0);
        
        // shift upper bit of n into lower bit of bcd
        bcd <<= 1;
        if(n & bit_mask){
            bcd |= 1;
        }
        
        bit_mask >>= 1;
    } while(bit_mask != 0);
    
    return(bcd);
}

///\endcond

//--------------------------------------------------------------------------------------------------
uint8_t snprint_x8(char *buffer, size_t buf_size, uint8_t num){
    char str[sizeof(num)*2];
    uint8_t i = sizeof(num)*2;
    uint8_t tmp;
    
    while(i) {
        i--;
        tmp = num & 0x0F;
        if(tmp < 10){
            str[i] = '0' + tmp;
        }else{
            str[i] = 'A' - 10 + tmp;
        }
        num >>= 4;
    }
    
    while(i <= sizeof(num)*2-1){
        buffer[i] = str[i];
        i++;
        if(i == buf_size) return(sizeof(num)*2);
    }
    buffer[i] = 0;
    
    return(sizeof(num)*2);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_x16(char *buffer, size_t buf_size, uint16_t num){
    char str[sizeof(num)*2];
    uint8_t i = sizeof(num)*2;
    uint8_t tmp;
    
    while(i) {
        i--;
        tmp = num & 0x0F;
        if(tmp < 10){
            str[i] = '0' + tmp;
        }else{
            str[i] = 'A' - 10 + tmp;
        }
        num >>= 4;
    }
    
    while(i <= sizeof(num)*2-1){
        buffer[i] = str[i];
        i++;
        if(i == buf_size) return(sizeof(num)*2);
    }
    buffer[i] = 0;
    
    return(sizeof(num)*2);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_x32(char *buffer, size_t buf_size, uint32_t num){
    char str[sizeof(num)*2];
    uint8_t i = sizeof(num)*2;
    uint8_t tmp;
    
    while(i) {
        i--;
        tmp = num & 0x0F;
        if(tmp < 10){
            str[i] = '0' + tmp;
        }else{
            str[i] = 'A' - 10 + tmp;
        }
        num >>= 4;
    }
    
    while(i <= sizeof(num)*2-1){
        buffer[i] = str[i];
        i++;
        if(i == buf_size) return(sizeof(num)*2);
    }
    buffer[i] = 0;
    
    return(sizeof(num)*2);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_d8(char *buffer, size_t buf_size, uint8_t num){
    uint8_t i, nchars;
    char str[3];
    uint16_t bcd;
    
    i = 3; // max of 3 digits
    
    bcd = u8_to_bcd(num);
    
    do{
        i -=1;
        str[i]=(bcd & 0x0F) + '0';
        bcd >>= 4;
    }while(bcd != 0);
    
    nchars = 3-i;
    
    if(buf_size == 0) return(nchars);
    
    do{
        *buffer = str[i];
        i++;
        buffer++;
        if((3-i) == buf_size) return(nchars);
    }while(i<3);
    
    *buffer = 0;
    return(nchars);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_d16(char *buffer, size_t buf_size, uint16_t num){
    uint8_t i, nchars;
    char str[5];
    uint32_t bcd;
    
    i = 5; // max of 5 digits
    
    bcd = u16_to_bcd(num);
    
    do{
        i -=1;
        str[i]=(bcd & 0x0F) + '0';
        bcd >>= 4;
    }while(bcd != 0);
    
    nchars = 5-i;
    
    if(buf_size == 0) return(nchars);
    
    do{
        *buffer = str[i];
        i++;
        buffer++;
        if((5-i) == buf_size) return(nchars);
    }while(i<5);
    
    *buffer = 0;
    return(nchars);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_d32(char *buffer, size_t buf_size, uint32_t num){
    uint8_t i, nchars;
    char str[10];
    uint64_t bcd;
    
    i = 10; // max of 10 digits
    
    bcd = u32_to_bcd(num);
    
    do{
        i -=1;
        str[i]=(bcd & 0x0F) + '0';
        bcd >>= 4;
    }while(bcd != 0);
    
    nchars = 10-i;
    
    if(buf_size == 0) return(nchars);
    
    do{
        *buffer = str[i];
        i++;
        buffer++;
        if((10-i) == buf_size) return(nchars);
    }while(i<10);
    
    *buffer = 0;
    return(nchars);
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_sd8(char *buffer, size_t buf_size, int8_t num){
    if(num < 0){
        buffer[0] = '-';
        buffer++;
        buf_size--;
        return(snprint_d8(buffer,buf_size,-num)+1);
    }else{
        return(snprint_d8(buffer,buf_size,num));
    }
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_sd16(char *buffer, size_t buf_size, int16_t num){
    if(num < 0){
        buffer[0] = '-';
        buffer++;
        buf_size--;
        return(snprint_d16(buffer,buf_size,-num)+1);
    }else{
        return(snprint_d16(buffer,buf_size,num));
    }
}

//--------------------------------------------------------------------------------------------------
uint8_t snprint_sd32(char *buffer, size_t buf_size, int32_t num){
    if(num < 0){
        buffer[0] = '-';
        buffer++;
        buf_size--;
        return(snprint_d32(buffer,buf_size,-num)+1);
    }else{
        return(snprint_d32(buffer,buf_size,num));
    }
}

///\}

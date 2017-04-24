/*
* Copyright (c) 2013, Alexander I. Mykyta
* All rights reserved.
* Supported and funded in part by RIT / NTID Center on Access Technology
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
* Alex M.       2013-09-12   born
* 
*=================================================================================================*/

/**
* \addtogroup MOD_RTC
* \{
**/

/**
* \file
* \brief Code for \ref MOD_RTC
* \author Alex Mykyta 
* 
* \todo Add support for other RTC variants.
**/
#include <msp430_xc.h>

#include "rtc.h"

//--------------------------------------------------------------------------------------------------
/**
* \brief Calculates and fills in the \c day_of_week member based on the date.
* \param [in] T #TIME_t object without valid \c day_of_week
* \param [out] T #TIME_t object \e with valid \c day_of_week
* \return Nothing
**/
static void CalcDOW(TIME_t *T){
	static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	uint16_t y;
	y = T->year - (T->month < 3);
	T->dayofweek = (y + y/4 - y/100 + y/400 + t[T->month-1] + T->day) % 7;
}

//--------------------------------------------------------------------------------------------------
void rtc_init(void){
    #if defined(__MSP430_HAS_RTC__) || defined(__MSP430_HAS_RTC_B__)
        RTCCTL01 = RTCHOLD + RTCMODE;
        RTCCTL23 = 0;
        
        RTCSEC = 0;
        RTCMIN = 0;
        RTCHOUR = 0;
        RTCDOW = 0;
        RTCDAY = 0;
        RTCMON = 0;
        RTCYEARL = 0;
        RTCYEARH = 0;
        
        RTCCTL1 &= ~RTCHOLD;
    #elsif defined(__MSP430_HAS_RTC_C__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_RTC_D__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_BT_RTC__)
        #error RTC is not supported yet
    #else
        #error This device does not have an RTC peripheral.
    #endif
}

//--------------------------------------------------------------------------------------------------
void rtc_uninit(void){
    #if defined(__MSP430_HAS_RTC__) || defined(__MSP430_HAS_RTC_B__)
        RTCCTL01 = RTCHOLD;
    #elsif defined(__MSP430_HAS_RTC_C__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_RTC_D__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_BT_RTC__)
        #error RTC is not supported yet
    #else
        #error This device does not have an RTC peripheral.
    #endif
}

//--------------------------------------------------------------------------------------------------
void rtc_set_time(time_t *T){
    if(T->dayofweek == UNKNOWN_DOW){
		CalcDOW(T);
	}
    
    #if defined(__MSP430_HAS_RTC__) || defined(__MSP430_HAS_RTC_B__)
        RTCSEC = T->second;
        RTCMIN = T->minute;
        RTCHOUR = T->hour;
        RTCDOW = T->dayofweek;
        RTCDAY = T->day;
        RTCMON = T->month;
        RTCYEARL = T->year & 0xFF;
        RTCYEARH = T->year >> 8;
    #elsif defined(__MSP430_HAS_RTC_C__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_RTC_D__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_BT_RTC__)
        #error RTC is not supported yet
    #else
        #error This device does not have an RTC peripheral.
    #endif
}

//--------------------------------------------------------------------------------------------------
void rtc_get_time(time_t *T){
    #if defined(__MSP430_HAS_RTC__) || defined(__MSP430_HAS_RTC_B__)
        do{
            while(RTCCTL01 & RTCRDY == 0); // wait until time is ok to read.
            T->second = RTCSEC;
            T->minute = RTCMIN;
            T->hour = RTCHOUR;
            T->dayofweek = RTCDOW;
            T->day = RTCDAY;
            T->month = RTCMON;
            T->year = RTCYEARL & (RTCYEARH << 8);
        }while(RTCCTL01 & RTCRDY == 0); // if time became invalid during read, try again
    #elsif defined(__MSP430_HAS_RTC_C__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_RTC_D__)
        #error RTC is not supported yet
    #elsif defined(__MSP430_HAS_BT_RTC__)
        #error RTC is not supported yet
    #else
        #error This device does not have an RTC peripheral.
    #endif
}


///\}

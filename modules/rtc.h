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
* \addtogroup MOD_RTC Real-time Clock
* \brief Basic real-time clock functions.
* \author Alex Mykyta 
*
* 
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | N/A
*   2xx     | N/A
*   4xx     | No
*   5xx     | Yes
*   6xx     | Yes
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_RTC
* \author Alex Mykyta 
**/

#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct{
	uint16_t year;		///< Full year integer
	uint8_t month;		///< Month (Jan=0, Feb=1, etc...)
	uint8_t day;		///< Day of the month (1-31)
	uint8_t dayofweek;	///< Day of the week (Sun=0, Mon=1, etc...)
	uint8_t hour;		///< Hour in 24hr format (0-23)
	uint8_t minute;		///< Minute (0-59)
	uint8_t second;		///< Second (0-59)
}time_t;

#define UNKNOWN_DOW	0xFF ///< Use when \c dayofweek is not known.

/**
* \brief Initialize the RTC
* \note ACLK must already be configured to receive a 32.768 kHz clock.
**/
void rtc_init(void);

/**
* \brief Uninitialize the RTC
**/
void rtc_uninit(void);

/**
* \brief Sets the RTC to the current time
* \param [in] T Pointer to #time_t object
* \return Nothing
**/
void rtc_set_time(time_t *T);

/**
* \brief Gets the current time from the RTC
* \param [out] T Current time
* \return Nothing
**/
void rtc_get_time(time_t *T);



#ifdef __cplusplus
}
#endif

#endif

///\}

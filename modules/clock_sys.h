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
* \addtogroup MOD_CLOCKSYS Clock System
* \brief Initializes the clock system
* \author Alex Mykyta 
* 
* This module provides a generic method of initializing an MSP430's system clocks. Using the
* configuration file, the clock sources for MCLK SMCLK and ACLK are defined as well as any clock
* division. If a clock network uses a DCO/PLL as its source, the clock system configures that as
* well to produce the desired frequency.
*
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | Yes
*   2xx     | -
*   4xx     | Yes
*   5xx     | Yes
*   6xx     | Yes
* 
* 
* \todo Auto-FLL for 5xx and 6xx
*
* \todo Add support for 2xx series
* 
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_CLOCKSYS "Clock System"
* \author Alex Mykyta 
**/


#ifndef CLOCK_SYS_H
#define CLOCK_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <result.h>
#include "clock_sys_config.h"
#include "clock_sys_internal.h"

//==================================================================================================
// Universal SYS defines:
//==================================================================================================

///\brief ACLK frequency \hideinitializer
#define ACLK_FREQ            _ACLK_FREQ

///\brief SMCLK frequency \hideinitializer
#define SMCLK_FREQ            _SMCLK_FREQ

///\brief MCLK frequency \hideinitializer
#define MCLK_FREQ            _MCLK_FREQ

//==================================================================================================
// Universal Functions
//==================================================================================================

/**
* \brief Initializes the clock system
* \return Nothing
* \attention The initialization routine does \e not setup the IO ports!
**/
void clock_init(void);

/**
* \brief Sets MCLK clock division level
* \param div Frequency division F_out = F_in/(2^div)
* \retval RES_OK
* \retval RES_PARAMERR \c div outside the accepted range.
* 
* \note Not available for the MSP430F4xx series.
**/
RES_t clock_SetDivMCLK(uint8_t div);

#ifdef __cplusplus
}
#endif

#endif /*CLOCK_SYS_H_*/

///\}

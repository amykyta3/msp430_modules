/*
* Copyright (c) 2012, Alexander I. Mykyta
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

/**
* \addtogroup MOD_BUTTON Pushbutton Events
* \brief Debounces pushbutton inputs and generates events based on their state.
* \author Alex Mykyta 
*
* This module is for debouncing pushbutton signals with the MSP430's interrupt-generating ports. By
* connecting a pushbutton to Ports 1 or 2,  the programmer can generate the following events:
*    - onButtonDown(): Button has been pressed down
*    - onButtonUp(): Button has been released
*    - onButtonHold(): Button has been held for a timeout period.
*
* \ref MOD_BUTTON also requires the following modules:
*    - \ref MOD_EVENT_QUEUE
* 
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | Yes
*   2xx     | Yes
*   4xx     | Yes
*   5xx     | Yes
*   6xx     | Yes
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_BUTTON
* \author Alex Mykyta 
**/

#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <button_config.h>


//==================================================================================================
// Functions
//==================================================================================================

/**
* \brief Initializes the Pushbutton Events module for the first time
* \return Nothing
**/
void button_init(void);

/**
* \brief Configures the Pushbutton Events module to trigger events based on changes seen on a port
* \param [in] en_mask Bit mask for which pins are to be treated as pushbuttons.
*                - 0 = Disabled
*                - 1 = Enabled
*
* \param [in] inverted_mask Bit mask for which pins should be treated as inverted logic.
*                - 0 = Pin is active-high
*                - 1 = Pin is active-low
* 
* \param [in] port Px Port number (1 or 2)
*
* \return Nothing
**/
void button_SetupPort(uint8_t en_mask, uint8_t inverted_mask, uint8_t port);

/**
* \brief Uninitializes the button module
* \warning This routine also turns off the MSP430's timer peripheral. If the timer is being shared
* by anything else such as the \ref MOD_TIMER module, they will be affected as well.
**/
void button_uninit(void);

//==================================================================================================
// Events
//==================================================================================================
///\name Events
///\{

/**
* \brief Button-down event
* \param port Port number indicating which input port the event occured on.
* \param b Bit mask indicating which buttons have been pressed down.
* \details This event is called once a button-down trigger has been detected and debounced.
*    The programmer must supply this event routine.
**/
extern void onButtonDown(uint8_t port, uint8_t b);

/**
* \brief Button-up event
* \param port Port number indicating which input port the event occured on.
* \param b Bit mask indicating which buttons have been released.
* \details This event is called once a button-release trigger has been detected and debounced.
*    The programmer must supply this event routine.
**/
extern void onButtonUp(uint8_t port, uint8_t b);

/**
* \brief Button-hold event
* \param port Port number indicating which input port the event occured on.
* \param b Bit mask indicating which buttons have been held.
* \details This event is called once a button has been held for the timeout period defined by 
*    #BUTTON_DEBOUNCETIME.
*    The programmer must supply this event routine.
**/
extern void onButtonHold(uint8_t port, uint8_t b);
///\}
///\}


/**
* \page EVENT_LIST_PAGE Event Listing
* 
* \section SEC_BUTTON_EVENTS Pushbutton Events
* \{
*    onButtonDown()    \n
*    onButtonUp()    \n
*    onButtonHold()
* \} 
**/

#ifdef __cplusplus
}
#endif

#endif /*BUTTON_H_*/


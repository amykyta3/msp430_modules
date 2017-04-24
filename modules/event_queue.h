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
* \addtogroup MOD_EVENT_QUEUE Event Queue
* \brief A simple first-in first-out event handler
* \author Alex Mykyta 
* 
* <b> Template main.c using the Event Queue </b> \n
* 
* \code
*     #include <msp430_xc.h>
*     #include <event_queue.h>
*     
*     int main(void){
*         WDTCTL = WDTPW + WDTHOLD; // Stop the Watchdog Timer
*         __disable_interrupt(); // Disable Interrupts
*         
*         // Initialize Port IO
*         ...
*         
*         // Initialize the system's clocks
*         ...
*         
*         // Initialize the event handler
*         event_init();
*         
*         // Initialize any modules that generate events
*         ...
*         
*         // Turn on interrupts
*         __enable_interrupt();
*         
*         // Start the event handler
*         event_StartHandler();
*         
*         // Program never reaches this
*         return(0);
*     }
*     
*     
*     void onIdle(void){
*         // onIdle is called when no events occur.
*         // To save power, the CPU can be put into a sleep state here.
*     }
* \endcode
* 
* \ref MOD_EVENT_QUEUE also requires the following modules:
*    - \ref MOD_FIFO
*
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_EVENT_QUEUE
* \author Alex Mykyta 
**/

#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H    

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <result.h>
#include <stdbool.h>

#include "event_queue.h"

//==================================================================================================
// Functions
//==================================================================================================

/**
* \brief Initializes the event handler's queue
* 
* This function \e must be called prior to starting the event handler
**/
void event_init(void);

/**
* \brief Starts the event handler
* \details After initializing the system after a reset and initializing the event module, Call this
* function to start the scheduler. This routine does not return.
* 
* \note Global interrupts should be enabled prior to starting the handler
**/
void event_StartHandler(void);

/**
* \brief Schedule a function to be called in the event queue along with related data
* \param [in] fptr Pointer to the function to be called
* \param [in] eventData Pointer to the data to be pushed into the queue (If not used, enter \c NULL)
* \param [in] size Number of bytes to be pushed (if none required, use size of 0)
* \retval RES_OK    Event added successfully
* \retval RES_FULL    Not enough room in the event queue. Event was not added.
* \details The function to be called must not take any input parameters nor can it return any
*    values. If pusing additional data with the event, the event called \e MUST have a matching
*    event_PopEventData(). Every additional byte pushed into the event queue \e MUST be popped out
*    regardless if it is used or not.
**/
RES_t event_PushEvent(void (*fptr)(void), void *eventData, size_t size);

/**
* \brief Pop event-related data out of the event queue
* \param [in] dst Pointer to where the data will be read into
* \param [in] size Number of bytes to be read
* \details Use this function inside the event function scheduled by event_PushEvent()to retreive any
* related event data that was pushed in using event_PushEventData()
**/
void event_PopEventData(void *dst, size_t size);

/**
* \brief Yields execution of the current event to the next pending event in the queue.
* \details Calling this function allows the next event in the queue to be executed. If no events are
*   in the queue or the next event is already active, the onIdle() event is processed.
* 
*   event_YieldEvent() can be called occasionally when performing a time-consuming operation
*   within an event such as a polling loop. Doing so allows other events that may have piled up in
*   the meantime to be processed.
* 
**/
void event_YieldEvent(void);

/**
* \brief Returns /c true if an event is in the queue
* \retval true An event is in the queue
* \retval false The event queue is empty
**/
bool event_Pending(void);

//==================================================================================================
// Events
//==================================================================================================

///\name Events
///\{

/**
* \brief Idle process event
* \details This event is called repeatedly when there are no events pending. \n
*    \b NOTE: As with any other event, a new event cannot be called until the current one exits.
**/
extern void onIdle(void);

///\}
///\}

/**
* \page EVENT_LIST_PAGE Event Listing
* 
* \section SEC_EVENT_QUEUE_EVENTS Event Queue Events
* \{
*    onIdle()
* \}
**/

#ifdef __cplusplus
}
#endif

#endif

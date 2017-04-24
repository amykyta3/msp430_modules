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
* \addtogroup MOD_TIMER Timer
* \brief Timer Driver
* \author Alex Mykyta 
*
*
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_TIMER
* \author Alex Mykyta 
**/

#ifndef TIMER_H
#define TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include <timer_config.h>

// Public struct that the user uses to setup a timer
/**
 * \brief Public structure used to define a new timer's behavior
 **/
struct timerctl{
    uint16_t interval_ms;   ///< Timer interval in milliseconds
    bool repeat;            ///< Should the timer repeat? True or False
    void (*fptr)(void*);    ///< Pointer to the function to call each time the timer expires
    void *ev_data;          ///< Pointer to a data object that will be passed into fptr
};


// Timer object. User doesn't need to touch this.
typedef struct timer_s timer_t;
#ifdef __DOXYGEN__

#else
struct timer_s{
    uint32_t ticks_remaining;
    uint32_t ticks_reload; // if reload is 0, timer does not repeat.
    void (*fptr)(void*); // Callback function
    void *ev_data; // callback function data
    timer_t *next; // pointer to next timer object in the linked list
};
#endif


/**
* \brief Initializes the timer driver
**/
void timer_init(void);

/**
 * \brief Uninitializes the timer driver
 * \warning This routine turns off the MSP430's timer peripheral. If the timer is being shared
 * by anything else such as the \ref MOD_BUTTON module, they will be affected as well.
**/
void timer_uninit(void);

/**
 * \brief Creates a new or resumes an existing interval timer.
 * 
 * The new timer object is returned in the buffer pointed to by \c timerid, which must be a non-NULL
 * pointer.  This timer object can not be deallocated until after the timer has been stopped.
 * 
 * The \c settings argument points to a \ref timerctl structure that specifies how the timer
 * operates. A prevoiously stopped timer can be resumed by passing a NULL pointer into the 
 * \c settings argument.
 * 
 * \param timerid Pointer to the timer object
 * \param settings Pointer to a \ref timerctl struct which defines the behavior of a new timer.
 *     A NULL pointer will resume a previously stopped timer defined by \c timerid
 **/
void timer_start(timer_t *timerid, struct timerctl *settings);

/**
 * \brief Stops a currently running timer
 * 
 * When stopping a timer, the state of the timer count is preserved allowing it to be resumed at a
 * later time. A stopped timer can be resumed by passing it into timer_start() along with a NULL
 * pointer in place of the \c settings argument
 * 
 * \param timerid Pointer to the timer object to stop
 **/
void timer_stop(timer_t *timerid);

#ifdef __cplusplus
}
#endif

#endif
///\}

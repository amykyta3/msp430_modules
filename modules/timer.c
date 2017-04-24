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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       2013-03-05   born
* Alex M.       2013-08-08   Fixed interrupt overrun bug
* Alex M.       2013-09-02   Wakes up CPU from LPM3-0
* 
*=================================================================================================*/

 /**
* \addtogroup MOD_TIMER Timer
* \brief Timer Driver
* \author Alex Mykyta 
* \{
**/

/**
* \file
* \brief Code for \ref MOD_TIMER
* \author Alex Mykyta 
**/

#include <stdint.h>
#include <stdbool.h>

#include <msp430_xc.h>

#include "timer.h"
#include "timer_internal.h"
#include "event_queue.h"

//--------------------------------------------------------------------------------------------------

static timer_t *tmr_first;
static uint16_t prev_tr = 0;

typedef struct{
    void *ev_data;
    void (*fptr)(void*);
} timer_EventData_t;
//--------------------------------------------------------------------------------------------------
static void timer_event_wrapper(void){
    timer_EventData_t dat;
    event_PopEventData(&dat, sizeof(dat));
    
    dat.fptr(dat.ev_data);
}


//--------------------------------------------------------------------------------------------------

// returns the minimum ticks remaining out of all the timers. Returns 0xFFFFFFFFL if no timers active
static uint32_t RefreshTimers(uint16_t current_tr){
    uint16_t ticks_elapsed;
    uint32_t ticks_min = 0xFFFFFFFFL;
    timer_t *tmr;
    timer_t *tmr_prev;
    
    // Loop through all timers in the list
    if(tmr_first){
        ticks_elapsed = current_tr - prev_tr;
        prev_tr = current_tr;
        tmr = tmr_first;
        tmr_prev = NULL;
        
        while(1){
            if(tmr->ticks_remaining <= ticks_elapsed){
                // Timer has expired
                timer_EventData_t dat;
                
                dat.ev_data = tmr->ev_data;
                dat.fptr = tmr->fptr;
                
                // Push event
                event_PushEvent(timer_event_wrapper, &dat, sizeof(dat));
                
                if(tmr->ticks_reload){
                    // Timer repeats. Reload it
                    tmr->ticks_remaining = tmr->ticks_reload;
                    if((tmr->ticks_remaining < ticks_min) || (tmr->ticks_remaining == 0)){
                        ticks_min = tmr->ticks_remaining;
                    }
                }else{
                    // Does not repeat
                    
                    // remove timer from list
                    if(tmr_prev){
                        tmr_prev->next = tmr->next;
                    }else{
                        // removing first in list
                        tmr_first = tmr_first->next;
                    }
                    
                }
                
            }else{
                // Not expired. Deduct ticks
                tmr->ticks_remaining -= ticks_elapsed;
                if((tmr->ticks_remaining < ticks_min) || (tmr->ticks_remaining == 0)){
                    ticks_min = tmr->ticks_remaining;
                }
            }
            
            // goto next (if exists)
            if(tmr->next){
                tmr_prev = tmr;
                tmr = tmr->next;
            }else{
                break;
            }
        }
    }
    
    return(ticks_min);    
}

//--------------------------------------------------------------------------------------------------
ISR(TMR_TIMER_ISR_VECTOR){
    uint32_t ticks_min;
    
    while(1){
        ticks_min = RefreshTimers(TMR_TCCR0);
        
        if(ticks_min == 0xFFFFFFFFL){
            // no timers active. Disable interrupt
            TMR_TCCTL0 &= ~CCIE;
            break;
        }else if(ticks_min < 0x10000){
            
            // Get new TR value to see how far it moved since the start of the ISR
            uint16_t current_tr;
            do{
                current_tr = TMR_TR;
            }while(current_tr != TMR_TR);
            
            if((current_tr-(uint16_t)TMR_TCCR0+1) >= ((uint16_t)ticks_min)){
                // Counter overran the next scheduled interrupt.
                // re-run the ISR.
                TMR_TCCR0 += ticks_min;
                continue;
            }else{
                // No overrun occurred
                TMR_TCCR0 += ticks_min;
                break;
            }
        }
    }
    
    if(event_Pending()){
        // Exit LPM0-3
        __bic_SR_register_on_exit(LPM3_bits);
        __no_operation();
    }
}
//--------------------------------------------------------------------------------------------------
void timer_init(void){
    
    tmr_first = NULL;
    
    // Setup Hardware Timer
    TMR_TCTL = (TIMER_CLK_SRC << 8) + (TIMER_IDIV << 6) + TACLR;
    #if defined(TMR_TEX0)
        TMR_TEX0 = TIMER_IDIVEX;
    #endif
    
    // Start Timer
    TMR_TCTL |= (MC1);
}

//--------------------------------------------------------------------------------------------------
void timer_uninit(void){
    // Stop timer
    TMR_TCTL = TACLR;
    TMR_TCCTL0 = 0;
    TMR_TCCR0 = 0;
    #if defined(TMR_TEX0)
        TMR_TEX0 = 0;
    #endif
    tmr_first = NULL;
}

//--------------------------------------------------------------------------------------------------
void timer_start(timer_t *timerid, struct timerctl *settings){
    
    // If the timer is already running, stop it.
    timer_stop(timerid);
    
    if(settings){
        // New timer settings.
        
        if(settings->interval_ms < TMR_INTERVAL_MIN) return;
        if(settings->interval_ms > TMR_INTERVAL_MAX) return;
        
        
        // calculate the interval in ticks
        timerid->ticks_remaining = settings->interval_ms;
        timerid->ticks_remaining *= TMR_FCLKDIV;
        timerid->ticks_remaining /= 1000;
        
        if(settings->repeat){
            timerid->ticks_reload = timerid->ticks_remaining;
        }else{
            timerid->ticks_reload = 0;
        }
        
        timerid->fptr = settings->fptr;
        timerid->ev_data = settings->ev_data;
    }
    
    if((timerid->ticks_remaining == 0) && (timerid->ticks_reload == 0)){
        return;
    }
    
    // disable timer interrupt
    TMR_TCCTL0 &= ~CCIE;
    
    // Get the current TR value (TR must read the same value twice in a row.)
    uint16_t current_tr;
    do{
        current_tr = TMR_TR;
    }while(current_tr != TMR_TR);
    
    uint32_t ticks_min;
    
    ticks_min = RefreshTimers(current_tr);
    
    // Insert timer into the front of the list
    timerid->next = tmr_first;
    tmr_first = timerid;
    
    if((timerid->ticks_remaining < ticks_min) || (ticks_min == 0)){
        ticks_min = timerid->ticks_remaining;
    }
    
    if(ticks_min == 0xFFFFFFFFL){
        // no timers active. Leave interrupt disabled
        return;
    }else if(ticks_min < 0x10000){
        TMR_TCCR0 = current_tr + ticks_min;
    }else{
        TMR_TCCR0 = current_tr;
    }
    
    // Enable timer interrupt
    TMR_TCCTL0 |= CCIE;
}

//--------------------------------------------------------------------------------------------------
void timer_stop(timer_t *timerid){
    timer_t *tmr;
    timer_t *tmr_prev;
    
    if(timerid && tmr_first){
        
        // Find the timer in the list
        tmr = tmr_first;
        tmr_prev = NULL;
        
        while(1){
            
            if(tmr == timerid){
                // Found it
                
                uint16_t current_tr;
                uint16_t ticks_elapsed;
                
                // Update the timer ticks just so it can be validly resumed...
                
                // Get the current TR value (TR must read the same value twice in a row.)
                do{
                    current_tr = TMR_TR;
                }while(current_tr != TMR_TR);
                
                ticks_elapsed = current_tr - prev_tr;
                
                if(timerid->ticks_remaining <= ticks_elapsed){
                    // expired timer
                    timerid->ticks_remaining = timerid->ticks_reload;
                }else{
                    // Not expired. Deduct ticks
                    tmr->ticks_remaining -= ticks_elapsed;
                }
                
                // remove timer from list
                if(tmr_prev){
                    tmr_prev->next = tmr->next;
                }else{
                    // removing first in list
                    tmr_first = tmr_first->next;
                }
                
                break;
            }
            
            // goto next (if exists)
            if(tmr->next){
                tmr_prev = tmr;
                tmr = tmr->next;
            }else{
                break;
            }
        }
    }
}

///\}


#include <stdint.h>
#include <stdbool.h>

#define _INCLUDED_FROM_EMU_SRC // disables renaming of timer_t. Actually using the POSIX one here
#include "timer.h"

#include <event_queue.h>

//--------------------------------------------------------------------------------------------------

static emu_timer_t *tmr_first;

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

void emu_timer_sigev(union sigval sv){
    emu_timer_t *tmr = sv.sival_ptr;
    
    // A timer has expired
    
    timer_EventData_t dat;
    
    dat.ev_data = tmr->ev_data;
    dat.fptr = tmr->fptr;
    
    // Push event
    event_PushEvent(timer_event_wrapper, &dat, sizeof(dat));
    
    if((tmr->its.it_interval.tv_sec == 0) && (tmr->its.it_interval.tv_nsec == 0)){
        // Does not repeat. Delete the timer.
        timer_stop(tmr);
    }
}

//--------------------------------------------------------------------------------------------------
void timer_init(void){
    tmr_first = NULL;
}

//--------------------------------------------------------------------------------------------------
void timer_uninit(void){
    //++ Stop and delete all timers
    tmr_first = NULL;
}

//--------------------------------------------------------------------------------------------------
void timer_start(emu_timer_t *timerid, struct timerctl *settings){
    
    if(timerid->posix_timer_valid){
        // If the timer is already running, stop it. (and delete it)
        timer_stop(timerid);
    }
    
    if(settings){
        // Starting a timer with new settings
        
        // Apply the settings to the timerid struct
        timerid->fptr = settings->fptr;
        timerid->ev_data = settings->ev_data;
        
        timerid->its.it_value.tv_sec = settings->interval_ms/1000;
        timerid->its.it_value.tv_nsec = settings->interval_ms % 1000;
        timerid->its.it_value.tv_nsec *= 1000000;
        
        if(settings->repeat){
            timerid->its.it_interval = timerid->its.it_value;
        }else{
            timerid->its.it_interval.tv_sec = 0;
            timerid->its.it_interval.tv_nsec = 0;
        }
        
    }
    
    // don't start the timer if all time values are 0
    if((timerid->its.it_value.tv_sec == 0) && (timerid->its.it_value.tv_nsec == 0) &&
                (timerid->its.it_interval.tv_sec == 0) && (timerid->its.it_interval.tv_nsec == 0)){
        return;
    }
    
    
    // Create the timer
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = emu_timer_sigev;
    sev.sigev_value.sival_ptr = timerid;
    sev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME, &sev, &timerid->posix_timer);
    timerid->posix_timer_valid = true;
    
    // Insert timer into the front of the list
    timerid->next = tmr_first;
    tmr_first = timerid;
    
    // Start the timer
    timer_settime(timerid->posix_timer, 0, &timerid->its, NULL);
    
}

//--------------------------------------------------------------------------------------------------
void timer_stop(emu_timer_t *timerid){
    emu_timer_t *tmr;
    emu_timer_t *tmr_prev;
    
    if(timerid && tmr_first){
        
        // Find the timer in the list
        tmr = tmr_first;
        tmr_prev = NULL;
        
        while(1){
            
            if(tmr == timerid){
                // Found it
                
                if(timerid->posix_timer_valid){
                    // Get the timer info just so it can be validly resumed...
                    timer_gettime(timerid->posix_timer, &timerid->its);
                    
                    // Delete the posix timer
                    timer_delete(timerid->posix_timer);
                    timerid->posix_timer_valid = false;
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

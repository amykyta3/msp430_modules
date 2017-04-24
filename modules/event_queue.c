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
* Alex M.       2011-07-25   born
* Alex M.       2011-09-11   Added cooperative pseudo-multitasking with root_YieldEvent()
* Alex M.       2011-07-21   Detached root process from main(). Now root_StartRootProcess()
* Alex M.       2013-02-10   Simplified module to only be an event handler. Renamed to "event_queue"
* Alex M.       2013-09-02   Added event_Pending()
* 
*=================================================================================================*/

/**
* \addtogroup MOD_EVENT_QUEUE
* \{
**/

/**
* \file
* \brief Code for \ref MOD_EVENT_QUEUE
* \author Alex Mykyta 
**/

#include <stdint.h>
#include <stdbool.h>

#include "fifo.h"
#include "event_queue.h"
#include <event_queue_config.h>

//==================================================================================================
// Internal Variables
//==================================================================================================

static uint8_t EventQueueBuffer[EVENT_QUEUE_SIZE]; // Allocated array for event queue buffer
static FIFO_t EventFIFO; // FIFO object for event queue

static uint8_t YieldDepth;
static void (*YieldedEvents[MAX_YIELD_DEPTH+1])(void);

//==================================================================================================
// Event Handler Loop Process
//==================================================================================================

void event_StartHandler(void){
    void (*EventProcess)(void);
    
    while(1){
        if(fifo_rdcount(&EventFIFO)){ // If there is an event in the queue
            // pop the pointer to the event handler out of the queue
            fifo_read(&EventFIFO,&EventProcess,sizeof(EventProcess)); 
            
            // Store which event is going to happen
            YieldedEvents[0] = EventProcess;
            
            // Call the event handler routine. If additional event-related data is stored in the 
            // queue, the event handler MUST pop it out before exiting!
            EventProcess();
        }else{
            // Only enter the idle process if there are no events pending
            
            // Store which event is going to happen
            YieldedEvents[0] = onIdle;
            
            onIdle();    // Idle process event
        }
    }
}

//==================================================================================================
// Functions
//==================================================================================================

void event_init(void){
    fifo_init(&EventFIFO,EventQueueBuffer,EVENT_QUEUE_SIZE);
    YieldDepth = 0;
    YieldedEvents[0] = NULL;
}

//--------------------------------------------------------------------------------------------------

RES_t event_PushEvent(void (*fptr)(void), void *eventData, size_t size){
    if(fifo_wrcount(&EventFIFO) >= (sizeof(fptr)+size)){
        // Enough room in Event Queue. Write event.
        fifo_write(&EventFIFO,&fptr,sizeof(fptr));
        if(size != 0){
            fifo_write(&EventFIFO,eventData,size);
        }
        return(RES_OK);
    }else{
        // Not enough room in event queue.
        return(RES_FULL);
    }
}

//--------------------------------------------------------------------------------------------------

void event_PopEventData(void *dst, size_t size){
    fifo_read(&EventFIFO,dst,size);
}

//--------------------------------------------------------------------------------------------------

void event_YieldEvent(void){
    void (*EventProcess)(void);
    uint8_t i,skip;
    
    if(YieldDepth >= MAX_YIELD_DEPTH){
        // hit the max yield depth. Quit
        return;
    }
    
    if(fifo_rdcount(&EventFIFO)){ // If there is an event in the queue
        // pop the pointer to the event handler out of the queue
        fifo_peek(&EventFIFO,&EventProcess,sizeof(EventProcess)); 
        
        skip = 0;
        for(i=0;i<=YieldDepth;i++){
            if(EventProcess == YieldedEvents[i]){
                // Event is already active. Try the Idle process.
                skip = 1;
                break;
            }
        }
        
        if(skip == 0){
            // Event is safe to call
            
            // flush the peeked data.
            fifo_read(&EventFIFO,NULL,sizeof(EventProcess)); 
            
            YieldDepth++;
            // Store which event is going to happen
            YieldedEvents[YieldDepth] = EventProcess;
            EventProcess(); // Call the event process
            YieldDepth--;
            return;
        }
    }
    
    // Either no events pending or the event pending has been yielded already.
    // Lets try the idle process
    
    skip = 0;
    for(i=0;i<=YieldDepth;i++){
        if(onIdle == YieldedEvents[i]){
            // onIdle is already active. Can't do anything... Exit.
            return;
        }
    }
    
    YieldDepth++;
    // Store which event is going to happen
    YieldedEvents[YieldDepth] = onIdle;
    onIdle();    // Idle process event
    YieldDepth--;
}
//--------------------------------------------------------------------------------------------------

bool event_Pending(void){
    if(fifo_rdcount(&EventFIFO)){
        return(true);
    }else{
        return(false);
    }
}

//--------------------------------------------------------------------------------------------------
///\}

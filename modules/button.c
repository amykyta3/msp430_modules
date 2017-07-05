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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       2011-07-22   born
* Alex M.       2013-08-10   Added uninit routine. Wakes up CPU from LPM3-0
* 
*=================================================================================================*/

/**
* \addtogroup MOD_BUTTON
* \{
**/

/**
* \file
* \brief Code for \ref MOD_BUTTON
* \author Alex Mykyta 
* 
**/

#include <stdint.h>
#include <msp430_xc.h>
#include "button.h"
#include "button_internal.h"
#include "event_queue.h"


#if BUTTON_PORT1 == 1
static BUTTON_t buttonP1_obj;
#endif

#if BUTTON_PORT2 == 1
static BUTTON_t buttonP2_obj;
#endif

//==================================================================================================
// Internal Functions
//==================================================================================================
static void buttonDownEventProcess(void){
    struct{
        uint8_t port;
        uint8_t flags;
    }b;
    event_PopEventData(&b,sizeof(b));
    onButtonDown(b.port,b.flags);
}

static void buttonUpEventProcess(void){
    struct{
        uint8_t port;
        uint8_t flags;
    }b;
    event_PopEventData(&b,sizeof(b));
    onButtonUp(b.port,b.flags);
}

static void buttonHoldEventProcess(void){
    struct b{
        uint8_t port;
        uint8_t flags;
    }b;
    event_PopEventData(&b,sizeof(b));
    onButtonHold(b.port,b.flags);
}

///\cond INTERNAL
//--------------------------------------------------------------------------------------------------
#if BUTTON_PORT1 == 1
ISR(PORT1){
    uint8_t pifg;
    uint16_t tr_tmp;
    pifg = P1IFG;
    P1IFG = 0;
    // register any new button down triggers
    buttonP1_obj.hit_down |= (buttonP1_obj.en & P1IE & pifg & ~(P1IES ^ buttonP1_obj.inverted));

    // register any new button up triggers
    buttonP1_obj.hit_up |= (buttonP1_obj.en & P1IE & pifg & (P1IES ^ buttonP1_obj.inverted));
    
    // disable interrupts for any triggers in debounce
    P1IE = buttonP1_obj.en & ~(buttonP1_obj.hit_up | buttonP1_obj.hit_down);
    
    // Start timer debounce interval
    // Sample the TR register until it is steady
    do{
        tr_tmp = BUT_TR;
    } while(tr_tmp != BUT_TR);
    
    BUT_TCCTL1 = CCIE;
    BUT_TCCR1 = tr_tmp + BUTTON_DEBOUNCETICKS;
}
#endif

#if BUTTON_PORT2 == 1
ISR(PORT2){
    uint8_t pifg;
    uint16_t tr_tmp;
    pifg = P2IFG;
    P2IFG = 0;
    // register any new button down triggers
    buttonP2_obj.hit_down |= (buttonP2_obj.en & P2IE & pifg & ~(P2IES ^ buttonP2_obj.inverted));

    // register any new button up triggers
    buttonP2_obj.hit_up |= (buttonP2_obj.en & P2IE & pifg & (P2IES ^ buttonP2_obj.inverted));
    
    // disable interrupts for any triggers in debounce
    P2IE = buttonP2_obj.en & ~(buttonP2_obj.hit_up | buttonP2_obj.hit_down);
    
    // Start timer debounce interval
    // Sample the TR register until it is steady
    do{
        tr_tmp = BUT_TR;
    } while(tr_tmp != BUT_TR);
    
    BUT_TCCTL1 = CCIE;
    BUT_TCCR1 = tr_tmp + BUTTON_DEBOUNCETICKS;
}
#endif

//--------------------------------------------------------------------------------------------------

ISR(BUT_TIMER_ISR){
    struct{
        uint8_t port;
        uint8_t flags;
    }b;
    
    switch(BUT_TIV){
        case 0x02: // Debouncer timer
            #if BUTTON_PORT1 == 1
            b.port = 1;
            b.flags = buttonP1_obj.en & (P1IN ^ buttonP1_obj.inverted) & buttonP1_obj.hit_down;
            P1IE |= buttonP1_obj.hit_down;
            buttonP1_obj.hit_down = 0;
            if(b.flags != 0){
                // Button down event occured. Schedule the event
                event_PushEvent(buttonDownEventProcess,&b,sizeof(b));
                
                // Flip port interrupt to look for up trigger
                P1IES &= ~(b.flags & buttonP1_obj.inverted);// L->H
                P1IES |= (b.flags & ~buttonP1_obj.inverted);// H->L
                P1IFG &= ~b.flags;
                
                // if no hold pending, start hold timer
                if(buttonP1_obj.pending_hold == 0){
                    buttonP1_obj.pending_hold = b.flags;
                    BUT_TCCTL2 = CCIE;
                    BUT_TCCR2 = BUT_TCCR1 + BUTTON_HOLDTICKS;
                }
            }
            
            b.flags = buttonP1_obj.en & ~(P1IN ^ buttonP1_obj.inverted) & buttonP1_obj.hit_up;
            P1IE |= buttonP1_obj.hit_up;
            buttonP1_obj.hit_up = 0;
            if(b.flags != 0){
                // Button up event occured. Schedule the event
                event_PushEvent(buttonUpEventProcess,&b,sizeof(b));
                
                // Flip port interrupt to look for down trigger
                P1IES &= ~(b.flags & ~buttonP1_obj.inverted);// L->H
                P1IES |= (b.flags & buttonP1_obj.inverted);// H->L
                P1IFG &= ~b.flags;
                
                // clear corresponding holds
                buttonP1_obj.pending_hold &= ~b.flags;
            }
            #endif
            
            #if BUTTON_PORT2 == 1
            b.port = 2;
            b.flags = buttonP2_obj.en & (P2IN ^ buttonP2_obj.inverted) & buttonP2_obj.hit_down;
            P2IE |= buttonP2_obj.hit_down;
            buttonP2_obj.hit_down = 0;
            if(b.flags != 0){
                // Button down event occured. Schedule the event
                event_PushEvent(buttonDownEventProcess,&b,sizeof(b));
                
                // Flip port interrupt to look for up trigger
                P2IES &= ~(b.flags & buttonP2_obj.inverted);// L->H
                P2IES |= (b.flags & ~buttonP2_obj.inverted);// H->L
                P2IFG &= ~b.flags;
                
                // if no hold pending, start hold timer
                if(buttonP2_obj.pending_hold == 0){
                    buttonP2_obj.pending_hold = b.flags;
                    BUT_TCCTL2 = CCIE;
                    BUT_TCCR2 = BUT_TCCR1 + BUTTON_HOLDTICKS;
                }
            }
            
            b.flags = buttonP2_obj.en & ~(P2IN ^ buttonP2_obj.inverted) & buttonP2_obj.hit_up;
            P2IE |= buttonP2_obj.hit_up;
            buttonP2_obj.hit_up = 0;
            if(b.flags != 0){
                // Button up event occured. Schedule the event
                event_PushEvent(buttonUpEventProcess,&b,sizeof(b));
                
                // Flip port interrupt to look for down trigger
                P2IES &= ~(b.flags & ~buttonP2_obj.inverted);// L->H
                P2IES |= (b.flags & buttonP2_obj.inverted);// H->L
                P2IFG &= ~b.flags;
                
                // clear corresponding holds
                buttonP2_obj.pending_hold &= ~b.flags;
            }
            #endif
            
            // stop debounce timer
            BUT_TCCTL1 = 0;
            break;
        case 0x04: // Hold timer
            #if BUTTON_PORT1 == 1
            b.port = 1;
            b.flags = buttonP1_obj.en & (P1IN ^ buttonP1_obj.inverted) & buttonP1_obj.pending_hold;
            if(b.flags != 0){
                // Button hold event occured. Schedule the event
                event_PushEvent(buttonHoldEventProcess,&b,sizeof(b));
                
                // clear holds
                buttonP1_obj.pending_hold = 0;
            }
            #endif
            
            #if BUTTON_PORT2 == 1
            b.port = 2;
            b.flags = buttonP2_obj.en & (P2IN ^ buttonP2_obj.inverted) & buttonP2_obj.pending_hold;
            if(b.flags != 0){
                // Button hold event occured. Schedule the event
                event_PushEvent(buttonHoldEventProcess,&b,sizeof(b));
                
                // clear holds
                buttonP2_obj.pending_hold = 0;
            }
            #endif
            
            // stop hold timer
            BUT_TCCTL2 = 0;
            break;
    }
    
    if(event_Pending()){
        // Exit LPM0-3
        __bic_SR_register_on_exit(LPM3_bits);
        __no_operation();
    }
}

///\endcond
//==================================================================================================
// User Functions
//==================================================================================================
void button_init(void){
    #if BUTTON_PORT1 == 1
        buttonP1_obj.en = 0;
        buttonP1_obj.hit_up = 0;
        buttonP1_obj.hit_down = 0;
        buttonP1_obj.pending_hold = 0;
    #endif
    
    #if BUTTON_PORT2 == 1
        buttonP2_obj.en = 0;
        buttonP2_obj.hit_up = 0;
        buttonP2_obj.hit_down = 0;
        buttonP2_obj.pending_hold = 0;
    #endif
    
    // Setup Timer
    BUT_TCTL = (BUTTON_CLK_SRC << 8) + (BUTTON_IDIV << 6) + TACLR;
    #if defined(BUT_TEX0)
        BUT_TEX0 = BUTTON_IDIVEX;
    #endif
    
    // Start Timer
    BUT_TCTL |= (MC1);
}

//--------------------------------------------------------------------------------------------------
void button_uninit(void){
    // Disable button interrupts
    #if BUTTON_PORT1 == 1
        P1IE = 0;
    #endif
    
    #if BUTTON_PORT2 == 1
        P2IE = 0;
    #endif
    
    // Disable the timer
    BUT_TCTL = TACLR;
    BUT_TCCTL1 = 0;
    BUT_TCCTL2 = 0;
    BUT_TCCR1 = 0;
    BUT_TCCR2 = 0;
    #if defined(BUT_TEX0)
        BUT_TEX0 = 0;
    #endif
}

//--------------------------------------------------------------------------------------------------
void button_SetupPort(uint8_t en_mask,uint8_t inverted_mask, uint8_t port){
    #if (BUTTON_PORT1 == 1) && (BUTTON_PORT2 == 1)
        // Both ports enabled
        if((port != 1)&&(port != 2)) return;
    #elif (BUTTON_PORT1 == 1)
        // Only Port1
        if(port != 1) return;
    #elif (BUTTON_PORT2 == 1)
        // Only Port2
        if(port != 2) return;
    #else
        // No ports enabled
        return;
    #endif
    
    if(port == 1){
        #if BUTTON_PORT1 == 1
        // Initialize Port1
        P1DIR &= ~en_mask; // set to inputs
        P1OUT |= (en_mask & inverted_mask); // active low pullups
        P1OUT &= ~(en_mask & ~inverted_mask); // active high pulldowns
        #if defined(P1REN)
            P1REN |= en_mask; // Enable pullup & pulldowns
        #endif
        
        P1IES &= ~(en_mask & ~inverted_mask); // first edge. L->H
        P1IES |= (en_mask & inverted_mask); // first edge. H->L
        
        // Populate button object
        buttonP1_obj.en = en_mask;
        buttonP1_obj.inverted = inverted_mask;
        buttonP1_obj.hit_up = 0;
        buttonP1_obj.hit_down = 0;
        buttonP1_obj.pending_hold = 0;
        
        P1IFG &= ~en_mask; // clear any flags that would cause an interrupt
        // Enable Port Interrupts
        P1IE = en_mask;
        #endif
    }else{
        #if BUTTON_PORT2 == 1
        // Initialize Port2
        P2DIR &= ~en_mask; // set to inputs
        P2OUT |= (en_mask & inverted_mask); // active low pullups
        P2OUT &= ~(en_mask & ~inverted_mask); // active high pulldowns
        #if defined(P2REN)
            P2REN |= en_mask; // Enable pullup & pulldowns
        #endif
        
        P2IES &= ~(en_mask & ~inverted_mask); // first edge. L->H
        P2IES |= (en_mask & inverted_mask); // first edge. H->L
        
        // Populate button object
        buttonP2_obj.en = en_mask;
        buttonP2_obj.inverted = inverted_mask;
        buttonP2_obj.hit_up = 0;
        buttonP2_obj.hit_down = 0;
        buttonP2_obj.pending_hold = 0;
        
        P2IFG &= ~en_mask; // clear any flags that would cause an interrupt
        // Enable Port Interrupts
        P2IE = en_mask;
        #endif
    }
}


///\}




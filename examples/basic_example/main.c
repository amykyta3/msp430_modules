
#include <stdint.h>

#include <msp430_xc.h>
#include <clock_sys.h>
#include <event_queue.h>
#include <button.h>
#include <timer.h>

#if defined(__MSP430F5529__)
    // Compiling for MSP430F5529. Assuming this is for the USB Launchpad dev board
    // MSP-EXP430F5529LP
    #define BOARD_MSP_EXP430F5529LP
    
#elif defined(__MSP430F4618__)
    // Compiling for MSP430F4618. Assuming this is for the MSP430 Experimenter Board
    // MSP-EXP430FG4618
    #define BOARD_MSP_EXP430FG4618
    
#else
    #error Board not supported. Please enter your own board settings
#endif

timer_t Timer1;
timer_t Timer2;

//--------------------------------------------------------------------------------------------------
#if defined(BOARD_MSP_EXP430F5529LP)
    #define LED1_PORT_OUT   P1OUT
    #define LED1_PORT_DIR   P1DIR
    #define P_LED1_bm       BIT0
    
    #define LED2_PORT_OUT   P4OUT
    #define LED2_PORT_DIR   P4DIR
    #define P_LED2_bm       BIT7
    
    static void board_init(void){
        
        // Setup LEDs
        LED1_PORT_OUT &= ~P_LED1_bm;
        LED1_PORT_DIR |= P_LED1_bm;
        LED2_PORT_OUT &= ~P_LED2_bm;
        LED2_PORT_DIR |= P_LED2_bm;
        
        // Setup XT1 Pins
        P5SEL |= BIT4 | BIT5;
        
        clock_init();
        event_init();
        button_init();
        timer_init();
        
        button_SetupPort(BIT1, BIT1, 2);
        button_SetupPort(BIT1, BIT1, 1);
    }
    
#elif defined(BOARD_MSP_EXP430FG4618)
    #define LED1_PORT_OUT   P2OUT
    #define LED1_PORT_DIR   P2DIR
    #define P_LED1_bm       BIT2
    
    #define LED2_PORT_OUT   P2OUT
    #define LED2_PORT_DIR   P2DIR
    #define P_LED2_bm       BIT1
    
    static void board_init(void){
        
        // Setup LEDs
        LED1_PORT_OUT &= ~P_LED1_bm;
        LED1_PORT_DIR |= P_LED1_bm;
        LED2_PORT_OUT &= ~P_LED2_bm;
        LED2_PORT_DIR |= P_LED2_bm;
        
        clock_init();
        event_init();
        button_init();
        timer_init();
        
        button_SetupPort(BIT0 | BIT1, BIT0 | BIT1, 1);
    }
#endif

//--------------------------------------------------------------------------------------------------
void OnTimerExpire1(void *data){
    // Left LED
    LED1_PORT_OUT ^= P_LED1_bm;
}

void OnTimerExpire2(void *data){
    // Right LED
    LED2_PORT_OUT ^= P_LED2_bm;
}

//--------------------------------------------------------------------------------------------------
int main(void){
    WDTCTL = WDTPW + WDTHOLD; // Stop the Watchdog Timer
    __disable_interrupt(); // Disable Interrupts
    
    board_init();
    
    struct timerctl timer_settings;
    
    timer_settings.interval_ms = 400;
    timer_settings.repeat = true;
    timer_settings.fptr = OnTimerExpire1;
    timer_settings.ev_data = NULL;
    timer_start(&Timer1,&timer_settings);
    
    timer_settings.interval_ms = 500;
    timer_settings.repeat = true;
    timer_settings.fptr = OnTimerExpire2;
    timer_settings.ev_data = NULL;
    timer_start(&Timer2,&timer_settings);
    
    __enable_interrupt();
    event_StartHandler();
    
    return(0);
}

//--------------------------------------------------------------------------------------------------
void onIdle(void){
    
}

//--------------------------------------------------------------------------------------------------
void onButtonDown(uint8_t port, uint8_t b){
    #if defined(BOARD_MSP_EXP430F5529LP)
        if((port == 2) && (b & BIT1)){
            // Left button
            timer_start(&Timer1,NULL);
        }else if((port == 1) && (b & BIT1)){
            // Right button
            timer_stop(&Timer1);
        }
    #elif defined(BOARD_MSP_EXP430FG4618)
        if(b & BIT1){
            // Left button
            timer_start(&Timer1,NULL);
        }else if(b & BIT0){
            // Right button
            timer_stop(&Timer1);
        }
    #endif
}

void onButtonUp(uint8_t port, uint8_t b){
    
}

void onButtonHold(uint8_t port, uint8_t b){
    
}

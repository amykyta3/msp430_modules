
#include <msp430.h> 
#include <stdint.h>

#include <cothread.h>


stack_t my_alt_stack[64];

cothread_t home_thread; // Home thread object
cothread_t my_alt_thread;    // Alternate thread object

int alt_thread_func(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    
    // Lets say that P2.1 and P2.2 are LEDs
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    P4DIR |= BIT7;
    P4OUT &= ~BIT7;
    
    // Initialize cothreads and populate the home thread object
    cothread_init(&home_thread);
    
    // Setup the my_alt_thread object and create the new thread context
    my_alt_thread.alt_stack = my_alt_stack; // it will use my_alt_stack for its stack storage
    my_alt_thread.alt_stack_size = sizeof(my_alt_stack); // I hope its big enough!
    my_alt_thread.co_exit = &home_thread; // If the thread exits, it will return to the home_thread
    cothread_create(&my_alt_thread,alt_thread_func);
    
    while(1){
        P1OUT |= BIT0;
        __delay_cycles(250000);
        P1OUT &= ~BIT0;
        __delay_cycles(250000);
        P1OUT |= BIT0;
        __delay_cycles(250000);
        P1OUT &= ~BIT0;
        __delay_cycles(250000);
        
        if(cothread_switch(&my_alt_thread) == 1234){
            break;
        }
    }
    
    while(1){
        P1OUT &= ~BIT0;
        P4OUT &= ~BIT7;
        __delay_cycles(250000);
        
        P1OUT |= BIT0;
        P4OUT |= BIT7;
        __delay_cycles(250000);
    }
    
    return(0);
}

int alt_thread_func(void){
    int loop_counter = 0;
    
    while(1){
        P4OUT |= BIT7;
        __delay_cycles(250000);
        P4OUT &= ~BIT7;
        __delay_cycles(250000);
        
        if(loop_counter == 10){
            break;
        }else{
            loop_counter++;
            cothread_switch(&home_thread);
        }
    }
    
    return(1234);
}
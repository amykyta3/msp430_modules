
#include <stdint.h>

#include <msp430_xc.h>
#include <clock_sys.h>
#include <uart_io.h>
#include <cli.h>
#include <sleep.h>

#include "board_settings.h"

int main(void){
    WDTCTL = WDTPW + WDTHOLD; // Stop the Watchdog Timer
    __disable_interrupt(); // Disable Interrupts
    
    // Setup UART pins
    UART_PORT_SEL |= P_UART_RX_bm | P_UART_TX_bm;
    
    // XT1 Pins
    #ifdef XT1_PORT_SEL
        XT1_PORT_SEL |= P_XT1_gm;
    #endif
    
    clock_init();
    uart_init();
    
    __enable_interrupt();
    
    msleep(500);
    
    cli_print_prompt();
    
    while(1){
        cli_process_char(uart_getc());
    }
    
    return(0);
}

#if(UIO_RX_MODE == 2)
ISR(DMA_VECTOR){
    if(is_uart_rx_dma_isr()){
        uart_rx_dma_isr();
    }
}
#endif

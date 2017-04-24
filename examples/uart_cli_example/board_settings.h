#ifndef _BOARD_SETTINGS_H
#define _BOARD_SETTINGS_H

#include <msp430_xc.h>

#if defined(__MSP430F5529__)
    // Compiling for MSP430F5529. Assuming this is for the USB Launchpad dev board
    
    #define BOARD_UART_DEV      1   // USCIA1
    #define BOARD_RX_DMA_TSEL   DMA0TSEL_20  // DMA channel 0 transfer select 20:  USCIA0 receive
    
    #define UART_PORT_SEL   P4SEL
    #define P_UART_RX_bm    BIT5
    #define P_UART_TX_bm    BIT4
    
    // XT1 is located on port pins
    #define XT1_PORT_SEL    P5SEL
    #define P_XT1_gm        (BIT4 | BIT5)
    
#elif defined(__MSP430F4618__)
    // Compiling for MSP430F4618. Assuming this is for the MSP430 Experimenter Board
    
    #define BOARD_UART_DEV      0   // USCIA0
    #define BOARD_RX_DMA_TSEL   DMA0TSEL_3  // DMA channel 0 transfer select 3:  USCIA0 receive
    
    #define UART_PORT_SEL   P2SEL
    #define P_UART_RX_bm    BIT5
    #define P_UART_TX_bm    BIT4
    
    // 4xx devices have dedicated XT1 pins. No port definitions needed
    
#else
    #error Board not supported. Please enter your own board settings
#endif


#endif

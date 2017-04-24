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
* Alex M.       2010-11-15   born
* Alex M.       2013-02-10   Renamed functions to be consistent with generic string IO format
* Alex M.       2013-08-10   Added uninit routine
* Alex M.       2013-11-04   - Reorganized configuration header
*                            - Added support for all MSP430s
*                            - Added DMA option
* 
*=================================================================================================*/

/**
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Code for \ref MOD_UART "UART IO"
* \author Alex Mykyta 
**/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <msp430_xc.h>
#include <result.h>
#include <atomic.h>
#include "uart_io.h"
#include "uart_io_internal.h"

#if((UIO_RX_MODE == 1) || (UIO_TX_MODE == 1)) // Interrupt Mode
    #include "fifo.h"
#endif

#if(UIO_RX_MODE == 1) // Interrupt Mode
    static char rxbuf[UIO_RXBUF_SIZE];
    static FIFO_t RXFIFO;
#elif(UIO_RX_MODE == 2) // DMA Mode
    static char rxbuf[UIO_RXBUF_SIZE];
    static volatile int8_t rx_laplead;
    static uint16_t rx_rdidx;
#endif

#if(UIO_TX_MODE == 1) // Interrupt Mode
    static char txbuf[UIO_TXBUF_SIZE];
    static FIFO_t TXFIFO;
#elif(UIO_TX_MODE == 2) // DMA Mode
    #error DMA TX mode not supported yet.
#endif

//==================================================================================================
// Init/Uninit
//==================================================================================================
void uart_init(void){
    #if defined(__MSP430_HAS_1xx_UART__)    // - - - - - - - - - - - - - - - - - - - - - - - - - - -
        UIO_CTL  = CHAR | SWRST; // soft reset
        UIO_TCTL = (UIO_CLK_SRC<<4);
        UIO_RCTL = 0;
        UIO_BR0  = UIO_BR0_VALUE;
        UIO_BR1  = UIO_BR1_VALUE;
        UIO_MCTL = UIO_MCTL_VALUE;
        UIO_ME   |= UIO_RXEN | UIO_TXEN;
        UIO_CTL  &= ~SWRST;
        
        #if(UIO_RX_MODE == 1) // Interrupt Mode
            fifo_init(&RXFIFO, rxbuf, UIO_RXBUF_SIZE);
            UIO_IE |= UIO_RXIE;
        #endif
        
        #if(UIO_TX_MODE == 1) // Interrupt Mode
            fifo_init(&TXFIFO, txbuf, UIO_TXBUF_SIZE);
        #endif
        
    #elif defined(__MSP430_HAS_2xx_USCI__) || defined(__MSP430_HAS_5xx_USCI__) || defined(__MSP430_HAS_6xx_EUSCI__)
        UIO_CTL1 |= UCSWRST; // soft reset
        UIO_CTL0 = 0;
        UIO_CTL1 = (UIO_CLK_SRC<<6)+UCSWRST;
        UIO_BR0  = UIO_BR0_VALUE;
        UIO_BR1  = UIO_BR1_VALUE;
        UIO_MCTL = UIO_MCTL_VALUE;
        UIO_CTL1 &= ~UCSWRST;
        
        #if(UIO_RX_MODE == 1) // Interrupt Mode
            fifo_init(&RXFIFO, rxbuf, UIO_RXBUF_SIZE);
            UIO_IE |= UIO_RXIE;
        #endif
        
        #if(UIO_TX_MODE == 1) // Interrupt Mode
            fifo_init(&TXFIFO, txbuf, UIO_TXBUF_SIZE);
        #endif   
    #endif
    
    #if(UIO_RX_MODE == 2) // DMA Mode
        RX_DMA_CTL = 0;
        RX_DMA_TRG &= ~RX_DMA_TSEL_MASK;
        RX_DMA_TRG |= RX_DMA_TSEL;
        RX_DMA_SA = (uintptr_t)&UIO_RXBUF;
        RX_DMA_DA = (uintptr_t)rxbuf;
        RX_DMA_SZ = sizeof(rxbuf);
        rx_laplead = 0;
        rx_rdidx = 0;
        RX_DMA_CTL = DMADT_4 | DMADSTINCR_3 | DMASRCINCR_0 | DMASRCBYTE | DMADSTBYTE | DMAEN | DMAIE;
    #endif
    
}

//--------------------------------------------------------------------------------------------------
void uart_uninit(void){
    
    #if defined(__MSP430_HAS_1xx_UART__)
        UIO_CTL  |= SWRST;
    #elif defined(__MSP430_HAS_2xx_USCI__) || defined(__MSP430_HAS_5xx_USCI__) || defined(__MSP430_HAS_6xx_EUSCI__)
        UIO_CTL1 = UCSWRST;
        UIO_CTL0 = 0;
        UIO_MCTL = 0;
        UIO_IE = 0;
    #endif
    
    #if(UIO_RX_MODE == 2) // DMA Mode
        RX_DMA_CTL = 0;
        RX_DMA_TRG &= ~RX_DMA_TSEL_MASK;
    #endif
    
    
}

//==================================================================================================
// RX Functions
//==================================================================================================
void uart_read(void *buf, size_t size){
    #if (UIO_RX_MODE == 1) // Interrupt Mode
        size_t rdcount;
        uint8_t* u8buf = (uint8_t*)buf;
        
        while(size > 0){
            // Get number of bytes that can be read.
            rdcount = fifo_rdcount(&RXFIFO);
            if(rdcount > size){
                rdcount = size;
            }
            
            if(rdcount != 0){
                if(u8buf){
                    fifo_read(&RXFIFO, u8buf, rdcount);
                    u8buf += rdcount;
                }else{
                    fifo_read(&RXFIFO, NULL, rdcount);
                }
                size -= rdcount;
            }
        }
    #elif (UIO_RX_MODE == 2) // DMA Mode
        int8_t laplead;
        uint16_t wridx;
        uint8_t* u8buf = (uint8_t*)buf;
        uint16_t rdcount;
        
        while(size > 0){
            
            // get snapshot of DMA buffer status
            // This CANNOT be done with interrupts disabled as it could skew the time that laplead gets
            // incremented.
            do{
                laplead = rx_laplead; //read lap
                wridx = RX_DMA_SZ; //read size
            }while(laplead != rx_laplead); //if laplead changed, may be invalid. try again
            wridx = sizeof(rxbuf) - wridx;
            
            if((laplead == 0) && (wridx >= rx_rdidx)){
                // Data doesn't wrap
                rdcount = wridx - rx_rdidx;
                if(rdcount > size){
                    rdcount = size;
                }
                
                // copy rdcount into u8buf
                memcpy(u8buf, &rxbuf[rx_rdidx], rdcount);
                
                u8buf += rdcount;
                size -= rdcount;
                rx_rdidx += rdcount;
                
            }else if(((laplead == 1) && (wridx <= rx_rdidx)) || ((laplead == 0) && (wridx < rx_rdidx))){
                // Available data wraps.
                
                // number of bytes to the end of the buffer
                rdcount = sizeof(rxbuf) - rx_rdidx;
                if(rdcount > size){
                    rdcount = size;
                }
                
                // copy rdcount into u8buf
                memcpy(u8buf, &rxbuf[rx_rdidx], rdcount);
                
                u8buf += rdcount;
                size -= rdcount;
                rx_rdidx += rdcount;
                
                if(rx_rdidx == sizeof(rxbuf)){
                    // read to the end. Wrap back
                    rx_rdidx = 0;
                    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
                        rx_laplead--;
                    }
                }
            }else{
                // Overrun!
                
                // Move read pointer to a safe position
                rx_rdidx = wridx;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
                    rx_laplead -= laplead;
                }
                
                // Abort reading.
                return;
            }
        }
    #else // Polling Mode
        uint8_t* u8buf = (uint8_t*)buf;
        while(size > 0){
            while((UIO_IFG & UIO_RXIFG) == 0); // wait until char received
            if(u8buf){
                *u8buf = UIO_RXBUF;
                u8buf++;
            }else{
                // discard
                UIO_IFG &= ~UIO_RXIFG;
            }
            size--;
        }
    #endif
}
    
//--------------------------------------------------------------------------------------------------
size_t uart_rdcount(void){
    #if (UIO_RX_MODE == 1) // Interrupt Mode
        return(fifo_rdcount(&RXFIFO));
    #elif (UIO_RX_MODE == 2) // DMA Mode
        int8_t laplead;
        uint16_t wridx;
        
        // get snapshot of DMA buffer status
        do{
            laplead = rx_laplead; //read lap
            wridx = RX_DMA_SZ; //read size
        }while(laplead != rx_laplead); //if laplead changed, may be invalid. try again
        wridx = sizeof(rxbuf) - wridx;
        
        if((laplead == 0) && (wridx >= rx_rdidx)){
            // Data doesn't wrap
            return(wridx - rx_rdidx);
            
        }else if(((laplead == 1) && (wridx <= rx_rdidx)) || ((laplead == 0) && (wridx < rx_rdidx))){
            // Available data wraps.
            return(wridx + sizeof(rxbuf) - rx_rdidx);
        }else{
            // Overrun!
            
            // Move read pointer to a safe position
            rx_rdidx = wridx;
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
                rx_laplead -= laplead;
            }
            
            return(0);
        }
    #else // Polling Mode
        if((UIO_IFG & UIO_RXIFG) != 0){
            return(1);
        }else{
            return(0);
        }
    #endif
}

//--------------------------------------------------------------------------------------------------
void uart_rdflush(void){
    #if (UIO_RX_MODE == 1) // Interrupt Mode
        fifo_clear(&RXFIFO);
    #elif (UIO_RX_MODE == 2) // DMA Mode
        uint16_t wridx;
        
        wridx = sizeof(rxbuf);
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            wridx -= RX_DMA_SZ;
        }
        
        // Discard data by moving rdidx
        rx_rdidx = wridx;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
            rx_laplead = 0;
        }
        
    #else // Polling Mode
        UIO_IFG &= ~UIO_RXIFG;
    #endif
}

//--------------------------------------------------------------------------------------------------
char uart_getc(void){
    char c;
    uart_read(&c, 1);
    return(c);
}

//--------------------------------------------------------------------------------------------------
char *uart_gets_s(char *str, size_t n){
    char c;
    size_t idx = 0;
    
    // write chars to buffer
    while(idx < (n-1)){
        c = uart_getc();
        if(c == '\n'){
            str[idx] = 0;
            return(str);
        }else{
            str[idx] = c;
            idx++;
        }
    }
    
    str[idx] = 0;
    
    // discard chars
    while(1){
        c = uart_getc();
        if(c == '\n'){
            return(str);
        }
    }
}

//==================================================================================================
// TX Functions
//==================================================================================================
void uart_write(void *buf, size_t size){
    #if (UIO_TX_MODE == 1) // Interrupt Mode
        size_t wrcount;
        uint8_t* u8buf = (uint8_t*)buf;
        
        while(size > 0){
            // Get number of bytes that can be written.
            wrcount = fifo_wrcount(&TXFIFO);
            if(wrcount > size){
                wrcount = size;
            }
            
            if(wrcount != 0){
                fifo_write(&TXFIFO, u8buf, wrcount);
                u8buf += wrcount;
                size -= wrcount;
                // Since TX is inactive and should be empty, the interrupt should occur immediately.
                UIO_IE |= UIO_TXIE;
            }
        }
    #else // Polling Mode
        uint8_t* u8buf = (uint8_t*)buf;
        while(size > 0){
            while((UIO_IFG & UIO_TXIFG) == 0); // wait until txbuf is empty
            UIO_TXBUF = *u8buf;
            u8buf++;
            size--;
        }
    #endif
}

//--------------------------------------------------------------------------------------------------
void uart_putc(char c){
    uart_write(&c, 1);
}

//--------------------------------------------------------------------------------------------------
void uart_puts(char *s){
    uart_write(s, strlen(s));
}

//==================================================================================================
// ISRs
//==================================================================================================

#if(UIO_RX_MODE == 2) // DMA Mode
    bool is_uart_rx_dma_isr(void){
        return(RX_DMA_CTL & DMAIFG);
    }
    
    void uart_rx_dma_isr(void){
        // RX DMA has wrapped around rxbuf
        rx_laplead++;
        
        // clear the flag
        RX_DMA_CTL &= ~DMAIFG;
    }
#endif
//--------------------------------------------------------------------------------------------------

///\cond INTERNAL

#if defined(__MSP430_HAS_1xx_UART__) || defined(__MSP430_HAS_2xx_USCI__)  // - - - - - - - - - - - -
    #if (UIO_RX_MODE == 1) // Interrupt Mode
        // RX Interrupt Service Routine
        ISR(UIO_RXISR_VECTOR){
            char chr;
            chr = UIO_RXBUF;
            fifo_write(&RXFIFO, &chr, 1);
        }
    #endif
    
    #if (UIO_TX_MODE == 1) // Interrupt Mode
        // TX Interrupt Service Routine
        ISR(UIO_TXISR_VECTOR){
            char chr;
            if(fifo_read(&TXFIFO, &chr, 1) == RES_OK){
                    UIO_TXBUF = chr;
            }else{
                UIO_IE &= ~UIO_TXIE; // disable tx interrupt
            }
        }
    #endif
#elif defined(__MSP430_HAS_5xx_USCI__)  || defined(__MSP430_HAS_6xx_EUSCI__)  // - - - - - - - - - -
    #if (UIO_RX_MODE == 1) || (UIO_TX_MODE == 1)
        // RX/TX Interrupt Service Routine
        ISR(UIO_ISR_VECTOR){
            char chr;
            
            #if (UIO_RX_MODE == 1)
            if(UIO_IFG & UIO_RXIFG){
                // Data Recieved
                chr = UIO_RXBUF;
                fifo_write(&RXFIFO, &chr, 1);
            }
            #endif
            
            #if (UIO_TX_MODE == 1)
            if(UIO_IFG & UIO_TXIFG){
                // Transmit Buffer Empty
                if(fifo_read(&TXFIFO, &chr, 1) == RES_OK){
                    UIO_TXBUF = chr;
                }else{
                    UIO_IE &= ~UIO_TXIE; // disable tx interrupt
                }
            }
            #endif
        }
    #endif
#endif

///\endcond


///\}

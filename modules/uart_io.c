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
    static FIFO_t rx_fifo;
#elif(UIO_RX_MODE == 2) // DMA Mode
    static char rxbuf[UIO_RXBUF_SIZE];
    static volatile int8_t rx_laplead;
    static uint16_t rx_rdidx;
#endif

#if(UIO_TX_MODE == 1) // Interrupt Mode
    static char txbuf[UIO_TXBUF_SIZE];
    static FIFO_t tx_fifo;
#elif(UIO_TX_MODE == 2) // DMA Mode
    char txbuf[UIO_TXBUF_SIZE];
    FIFO_t tx_fifo;
    unsigned tx_dma_count;
#endif

#ifndef UIO_SLEEP_IRQ
#define UIO_SLEEP_IRQ()
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
            fifo_init(&rx_fifo, rxbuf, UIO_RXBUF_SIZE);
            UIO_IE |= UIO_RXIE;
        #endif
        
        #if(UIO_TX_MODE == 1) // Interrupt Mode
            fifo_init(&tx_fifo, txbuf, UIO_TXBUF_SIZE);
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
            fifo_init(&rx_fifo, rxbuf, UIO_RXBUF_SIZE);
            UIO_IE |= UIO_RXIE;
        #endif
        
        #if(UIO_TX_MODE == 1) // Interrupt Mode
            fifo_init(&tx_fifo, txbuf, UIO_TXBUF_SIZE);
        #endif   
    #endif
    
    #if(UIO_RX_MODE == 2) // DMA Mode
        RX_DMA_CTL = 0;
        RX_DMA_TRG &= ~RX_DMA_TSEL_MASK;
        RX_DMA_TRG |= RX_DMA_TSEL;
        RX_DMA_SA = (uio_dma_addr)&UIO_RXBUF;
        RX_DMA_DA = (uio_dma_addr)rxbuf;
        RX_DMA_SZ = sizeof(rxbuf);
        rx_laplead = 0;
        rx_rdidx = 0;
        RX_DMA_CTL = DMADT_4 | DMADSTINCR_3 | DMASRCINCR_0 | DMASRCBYTE | DMADSTBYTE | DMAEN | DMAIE;
    #endif
    #if(UIO_TX_MODE == 2) // DMA Mode
        fifo_init(&tx_fifo, txbuf, UIO_TXBUF_SIZE);
        TX_DMA_CTL = 0;
        TX_DMA_TRG &= ~TX_DMA_TSEL_MASK;
        TX_DMA_TRG |= TX_DMA_TSEL;
        TX_DMA_DA = (uio_dma_addr)&UIO_TXBUF;
        TX_DMA_SA = (uio_dma_addr)txbuf;
        TX_DMA_SZ = sizeof(txbuf);
        tx_dma_count = 0;
        TX_DMA_CTL = DMADT_0 | DMADSTINCR_0 | DMASRCINCR_3 | DMASRCBYTE | DMADSTBYTE | DMALEVEL;
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
    
    #if(UIO_TX_MODE == 2) // DMA Mode
        TX_DMA_CTL = 0;
        TX_DMA_TRG &= ~TX_DMA_TSEL_MASK;
    #endif
}

//==================================================================================================
// RX Functions
//==================================================================================================
#ifdef UIO_AWAKE_IRQ
#define UIO_RX_SANITY_WAKE  1
volatile size_t uio_rx_await_chars = 0;
static inline
void UIO_AWAIT_CHARS(size_t num){
    uio_rx_await_chars = num;
    UIO_SLEEP_IRQ();
}
#else
inline
#define UIO_RX_SANITY_WAKE  0
void UIO_AWAIT_CHARS(size_t num){};
#endif

void uart_read(void *buf, size_t size){
    #if (UIO_RX_MODE == 1) // Interrupt Mode
        size_t rdcount;
        uint8_t* u8buf = (uint8_t*)buf;
        
        while(size > 0){
            // Get number of bytes that can be read.
            rdcount = fifo_rdcount(&rx_fifo);
            if(rdcount > size){
                rdcount = size;
            }
            else if (rdcount == 0){
                UIO_AWAIT_CHARS(size);
                continue;
            }

            if(rdcount != 0){
                if(u8buf){
                    fifo_read(&rx_fifo, u8buf, rdcount);
                    u8buf += rdcount;
                }else{
                    fifo_read(&rx_fifo, NULL, rdcount);
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
                else if (rdcount == 0){
                    UIO_AWAIT_CHARS(size);
                    continue;
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
                else if (rdcount == 0){
                    UIO_SLEEP_IRQ();
                    continue;
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
        return(fifo_rdcount(&rx_fifo));
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
        fifo_clear(&rx_fifo);
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
    int c;
    for (c = fifo_pop_byte(&rx_fifo); c < 0; c = fifo_pop_byte(&rx_fifo))
        UIO_AWAIT_CHARS(1);
    return((char)(uint8_t)c);
}

/**
* \brief nonblocking Reads the next character from the UART
* \details If a character is not immediately available, function return -1
* \return >= 0 The next available character
* \return < 0  input empty, no chars availiable
**/
int uart_getchar(void){
    return fifo_pop_byte(&rx_fifo);
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
inline
void uart_tx_resume(void);

void uart_write(const void *buf, size_t size){
    #if (UIO_TX_MODE >= 1) // Interrupt Mode
        size_t wrcount;
        const uint8_t* u8buf = (const uint8_t*)buf;
        
        while(size > 0){
            // Get number of bytes that can be written.
            wrcount = fifo_wrcount(&tx_fifo);
            if(wrcount > size){
                wrcount = size;
            }
            
            if(wrcount != 0){
                fifo_write(&tx_fifo, u8buf, wrcount);
                u8buf += wrcount;
                size -= wrcount;
                uart_tx_resume();
            }
            else
                UIO_SLEEP_IRQ();
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

inline
void uart_tx_resume(void){
#if (UIO_TX_MODE <= 1) // Interrupt Mode
    UIO_IE |= UIO_TXIE;
#elif (UIO_TX_MODE == 2) // DMA Mode
    if ( (TX_DMA_CTL & DMAEN) == 0) {
        tx_dma_count = fifo_rdbuf_count(&tx_fifo);
        TX_DMA_SA = (uio_dma_addr)(&(txbuf[tx_fifo.rdidx]));
        TX_DMA_SZ = tx_dma_count;
        TX_DMA_CTL |= DMAEN | DMAIE;
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

int uart_send(const void *buf, size_t size){
#if (UIO_TX_MODE == 0) // polling Mode
    uart_write(buf, size);
    return size;
#else
    size_t wrcount = fifo_wrcount(&tx_fifo);
    if(wrcount > size){
        wrcount = size;
    }

    if(wrcount > 0){
        const uint8_t* u8buf = (const uint8_t*)buf;
        fifo_write(&tx_fifo, u8buf, wrcount);
        // Since TX is inactive and should be empty, the interrupt should occur immediately.
        UIO_IE |= UIO_TXIE;
        return wrcount;
    }
    return 0;
#endif
}

bool  uart_tx_busy(void){
#if (UIO_TX_MODE == 0) // polling Mode
#else
    size_t wrcount = fifo_rdcount(&tx_fifo);
    if (wrcount > 0)
        return true;
#endif
    if ((UIO_IFG & UIO_TXIFG) == 0)
        return true;
    while ((UIO_STAT & UIO_BUSY) != 0);
    return false;
}

//==================================================================================================
// ISRs
//==================================================================================================

#if(UIO_RX_MODE == 2) // DMA Mode
    void uart_rx_dma_isr(void){
        // RX DMA has wrapped around rxbuf
        rx_laplead++;
        
        // clear the flag
        RX_DMA_CTL &= ~DMAIFG;
    }
#endif

#if(UIO_TX_MODE == 2) // DMA Mode
    bool uart_tx_dma_isr(void){
        // clear the flag
        TX_DMA_CTL &= ~DMAIFG;
        fifo_read(&tx_fifo, NULL, tx_dma_count);
        tx_dma_count = 0;
        size_t have = fifo_rdbuf_count(&tx_fifo);
        if (have > 0){
            uart_tx_resume();
            return false;
        }
        else {
            return true;
        }
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
            fifo_write(&rx_fifo, &chr, 1);
        }
    #endif
    
    #if (UIO_TX_MODE == 1) // Interrupt Mode
        // TX Interrupt Service Routine
        ISR(UIO_TXISR_VECTOR){
            char chr;
            if(fifo_read(&tx_fifo, &chr, 1) == RES_OK){
                    UIO_TXBUF = chr;
            }else{
                UIO_IE &= ~UIO_TXIE; // disable tx interrupt
                UIO_AWAKE_IRQ();
            }
        }
    #endif
#elif defined(__MSP430_HAS_5xx_USCI__)  || defined(__MSP430_HAS_6xx_EUSCI__)  // - - - - - - - - - -
    #if (UIO_RX_MODE == 1) || (UIO_RX_SANITY_WAKE > 0) || (UIO_TX_MODE == 1)
        // RX/TX Interrupt Service Routine
        ISR(UIO_ISR){
            char chr;
            char awake = 0;

            #if (UIO_RX_MODE == 1) || (UIO_RX_SANITY_WAKE > 0)
            if(UIO_IFG & UIO_RXIFG){
                // Data Recieved
                #if (UIO_RX_MODE == 1)
                chr = UIO_RXBUF;
                fifo_push_byte(&rx_fifo, chr);
                #endif
                #if (UIO_RX_SANITY_WAKE > 0)
                if (uio_rx_await_chars > 0)
                    uio_rx_await_chars--;
                if (uio_rx_await_chars == 0)
                    awake = 1;
                #endif
            }
            #endif
            
            #if (UIO_TX_MODE == 1)
            if(UIO_IFG & UIO_TXIFG){
                // Transmit Buffer Empty
                if(fifo_read(&tx_fifo, &chr, 1) == RES_OK){
                    UIO_TXBUF = chr;
                }else{
                    UIO_IE &= ~UIO_TXIE; // disable tx interrupt
                    awake = 1;
                }
            }
            #endif

            #if defined(UIO_AWAKE_IRQ)
            if (awake)
                UIO_AWAKE_IRQ();
            #endif
        }
    #endif
#endif

///\endcond


///\}

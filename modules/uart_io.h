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

/**
* \addtogroup MOD_UART UART IO
* \brief Provides basic text IO functions for the MSP430 UART controller
* \author Alex Mykyta 
*
* This module provides basic text input and output functions to the UART. \n
*
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | Yes
*   2xx     | Yes
*   4xx     | Yes
*   5xx     | Yes
*   6xx     | Yes
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_UART "UART IO"
* \author Alex Mykyta 
**/

#ifndef UART_IO_H
#define UART_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "uart_io_config.h"

//==================================================================================================
// Function Prototypes
//==================================================================================================

/**
* \brief Initializes the UART controller
* \attention The initialization routine does \e not setup the IO ports!
**/
void uart_init(void);

/**
* \brief Uninitializes the UART controller
**/
void uart_uninit(void);

/**
* \brief Read data from the UART. Blocks until all data has been received.
* \param [out] buf Destination buffer of the data to be read. A \c NULL pointer discards the data.
* \param [in] size Number of bytes to be read.
**/
void uart_read(void *buf, size_t size);

/**
* \brief Write data to the UART. Blocks until all data has been sent.
* \param [in] buf Pointer to the data to be written.
* \param [in] size Number of bytes to be written.
**/
void uart_write(const void *buf, size_t size);

/**
* \brief nonblocking Write data to the UART. send only awailiable FIFO space bytes.
* \param [in] buf Pointer to the data to be written.
* \param [in] size Number of bytes to be written.
* \return     size of sended data
**/
int uart_send(const void *buf, size_t size);

/**
* \brief Get the number of bytes available to be read
* \return Number of bytes
**/
size_t uart_rdcount(void);

bool  uart_tx_busy(void);

/**
* \brief Discard any data that has already been received
**/
void uart_rdflush(void);

/**
* \brief Reads the next character from the UART
* \details If a character is not immediately available, function will block until it receives one.
* \return The next available character
**/
char uart_getc(void);

/**
* \brief Reads in a string of characters until a new-line character ( \c \\n) is received
* 
* - Reads at most n-1 characters from the UART
* - Resulting string is null-terminated
* - If n is zero, a null character is written to str[0], and the function reads and discards
*     characters until a new-line character is received.
* - If n-1 characters have been read, the function continues reading and discarding characters until
*     a new-line character is received.
* - If an entire line is not immediately available, the function will block until it 
*     receives a new-line character.
* 
* \param [out] str Pointer to the destination string buffer
* \param n The size of the string buffer \c str
* \return \c str on success. \c NULL otherwise
**/
char *uart_gets_s(char *str, size_t n);

/**
* \brief Writes a character to the UART
* \param c character to be written
**/
void uart_putc(char c);

/**
* \brief Writes a character string to the UART
* \param s Pointer to the Null-terminated string to be sent
**/
void uart_puts(char *s);


#if (UIO_RX_MODE == 2) || defined(__DOXYGEN__) // DMA Mode
    
    /**
    * \brief UART RX DMA Interrupt Service Routine
    * \details If the UART RX mode is set to use DMA, the user must implement the DMA controller's
    * ISR function. The ISR must call this function if the interrupt is for the RX DMA channel.
    * This can be done using the is_uart_rx_dma_isr() function as follows:
    * \code
    *   if(is_uart_rx_dma_isr()){
    *       uart_rx_dma_isr();
    *   }
    * \endcode
    **/
    void uart_rx_dma_isr(void);
    
    /**
    * \brief Test to check if the current DMA ISR is for the uart_rx DMA
    * \retval true  Interrupt flag corresponding to the UART RX DMA channel.
    * \retval false RX DMA interrupt flag is not set.
    **/
    #include <msp430_xc.h>
    #include "uart_io_internal.h"
    static __inline__
    bool is_uart_rx_dma_isr(void){
        return(RX_DMA_CTL & DMAIFG);
    };
    
#endif

#if (UIO_TX_MODE == 2) || defined(__DOXYGEN__) // DMA Mode

    /**
    * \brief UART RX DMA Interrupt Service Routine
    * \details If the UART RX mode is set to use DMA, the user must implement the DMA controller's
    * ISR function. The ISR must call this function if the interrupt is for the RX DMA channel.
    * This can be done using the is_uart_rx_dma_isr() function as follows:
    * \code
    *   if(is_uart_rx_dma_isr()){
    *       uart_rx_dma_isr();
    *   }
    * \endcode
    * \return - true if tx fifo empty
    **/
    bool uart_tx_dma_isr(void);

    /**
    * \brief Test to check if the current DMA ISR is for the uart_rx DMA
    * \retval true  Interrupt flag corresponding to the UART RX DMA channel.
    * \retval false RX DMA interrupt flag is not set.
    **/
    #include <msp430_xc.h>
    #include "uart_io_internal.h"
    static __inline__
    bool is_uart_tx_dma_isr(void){
        return(TX_DMA_CTL & DMAIFG);
    };

#endif


#ifdef __cplusplus
}
#endif

#endif
///\}

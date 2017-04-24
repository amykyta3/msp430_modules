/*
* Copyright (c) 2014, Alexander I. Mykyta
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
* \addtogroup MOD_SPI
* \{
**/

/**
* \file
* \brief Internal include for \ref MOD_SPI "SPI Bus". Abstracts register names between MSP430 devices
* \author Alex Mykyta
**/

#ifndef SPI_INTERNAL_H
#define SPI_INTERNAL_H


#include <msp430_xc.h>

//==================================================================================================

#if SPI_CLK_SRC > 2
    #error "Invalid SPI_CLK_SRC in spi_config.h"
#elif SPI_CLK_SRC < 1
    #error "Invalid SPI_CLK_SRC in spi_config.h"
#endif


#if SPI_USE_USCI == 0
    #if defined(__MSP430_HAS_USCI_A0__)
        #define SPI_UCCTL0      UCA0CTL0
        #define SPI_UCCTL1      UCA0CTL1
        #define SPI_UCBR        UCA0BRW
        #define SPI_UCMCTL      UCA0MCTL
        #define SPI_UCSTAT      UCA0STAT
        #define SPI_UCRXBUF     UCA0RXBUF
        #define SPI_UCTXBUF     UCA0TXBUF
        #define SPI_UCIE        UCA0IE
        #define SPI_UCIFG       UCA0IFG
        #define SPI_UCIV        UCA0IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 1
    #if defined(__MSP430_HAS_USCI_A1__)
        #define SPI_UCCTL0      UCA1CTL0
        #define SPI_UCCTL1      UCA1CTL1
        #define SPI_UCBR        UCA1BRW
        #define SPI_UCMCTL      UCA1MCTL
        #define SPI_UCSTAT      UCA1STAT
        #define SPI_UCRXBUF     UCA1RXBUF
        #define SPI_UCTXBUF     UCA1TXBUF
        #define SPI_UCIE        UCA1IE
        #define SPI_UCIFG       UCA1IFG
        #define SPI_UCIV        UCA1IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 2
    #if defined(__MSP430_HAS_USCI_A2__)
        #define SPI_UCCTL0      UCA2CTL0
        #define SPI_UCCTL1      UCA2CTL1
        #define SPI_UCBR        UCA2BRW
        #define SPI_UCMCTL      UCA2MCTL
        #define SPI_UCSTAT      UCA2STAT
        #define SPI_UCRXBUF     UCA2RXBUF
        #define SPI_UCTXBUF     UCA2TXBUF
        #define SPI_UCIE        UCA2IE
        #define SPI_UCIFG       UCA2IFG
        #define SPI_UCIV        UCA2IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 3
    #if defined(__MSP430_HAS_USCI_A3__)
        #define SPI_UCCTL0      UCA3CTL0
        #define SPI_UCCTL1      UCA3CTL1
        #define SPI_UCBR        UCA3BRW
        #define SPI_UCMCTL      UCA3MCTL
        #define SPI_UCSTAT      UCA3STAT
        #define SPI_UCRXBUF     UCA3RXBUF
        #define SPI_UCTXBUF     UCA3TXBUF
        #define SPI_UCIE        UCA3IE
        #define SPI_UCIFG       UCA3IFG
        #define SPI_UCIV        UCA3IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 4
    #if defined(__MSP430_HAS_USCI_B0__)
        #define SPI_UCCTL0      UCB0CTL0
        #define SPI_UCCTL1      UCB0CTL1
        #define SPI_UCBR        UCB0BRW
        #define SPI_UCMCTL      UCB0MCTL
        #define SPI_UCSTAT      UCB0STAT
        #define SPI_UCRXBUF     UCB0RXBUF
        #define SPI_UCTXBUF     UCB0TXBUF
        #define SPI_UCIE        UCB0IE
        #define SPI_UCIFG       UCB0IFG
        #define SPI_UCIV        UCB0IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 5
    #if defined(__MSP430_HAS_USCI_B1__)
        #define SPI_UCCTL0      UCB1CTL0
        #define SPI_UCCTL1      UCB1CTL1
        #define SPI_UCBR        UCB1BRW
        #define SPI_UCMCTL      UCB1MCTL
        #define SPI_UCSTAT      UCB1STAT
        #define SPI_UCRXBUF     UCB1RXBUF
        #define SPI_UCTXBUF     UCB1TXBUF
        #define SPI_UCIE        UCB1IE
        #define SPI_UCIFG       UCB1IFG
        #define SPI_UCIV        UCB1IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 6
    #if defined(__MSP430_HAS_USCI_B2__)
        #define SPI_UCCTL0      UCB2CTL0
        #define SPI_UCCTL1      UCB2CTL1
        #define SPI_UCBR        UCB2BRW
        #define SPI_UCMCTL      UCB2MCTL
        #define SPI_UCSTAT      UCB2STAT
        #define SPI_UCRXBUF     UCB2RXBUF
        #define SPI_UCTXBUF     UCB2TXBUF
        #define SPI_UCIE        UCB2IE
        #define SPI_UCIFG       UCB2IFG
        #define SPI_UCIV        UCB2IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#elif SPI_USE_USCI == 7
    #if defined(__MSP430_HAS_USCI_B3__)
        #define SPI_UCCTL0      UCB3CTL0
        #define SPI_UCCTL1      UCB3CTL1
        #define SPI_UCBR        UCB3BRW
        #define SPI_UCMCTL      UCB3MCTL
        #define SPI_UCSTAT      UCB3STAT
        #define SPI_UCRXBUF     UCB3RXBUF
        #define SPI_UCTXBUF     UCB3TXBUF
        #define SPI_UCIE        UCB3IE
        #define SPI_UCIFG       UCB3IFG
        #define SPI_UCIV        UCB3IV
    #else
        #error "Invalid SPI_USE_USCI in spi_config.h"
    #endif
#else
    #error "Invalid SPI_USE_USCI in spi_config.h"
#endif

#endif

///\}

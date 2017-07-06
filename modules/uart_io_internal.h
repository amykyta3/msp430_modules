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
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Internal include for \ref MOD_UART "UART IO".
*    Abstracts register names between MSP430 devices
* \author Alex Mykyta 
**/

///\}

#ifndef UART_IO_INTERNAL_H
#define UART_IO_INTERNAL_H

//==================================================================================================

#if UIO_CLK_SRC > 2
    #error "Invalid UIO_CLK_SRC in uart_io_config.h"
#endif

//==================================================================================================
// USCI/USART Device Selection
//==================================================================================================
#if UIO_USE_DEV == 0
    #if defined(__MSP430_HAS_UART0__) // 1xx and 4xx variant
        #define __MSP430_HAS_1xx_UART__
        
        // Registers
        #define UIO_CTL     U0CTL
        #define UIO_TCTL    U0TCTL
        #define UIO_RCTL    U0RCTL
        #define UIO_BR0     U0BR0
        #define UIO_BR1     U0BR1
        #define UIO_MCTL    U0MCTL
        #define UIO_RXBUF   U0RXBUF
        #define UIO_TXBUF   U0TXBUF
        #define UIO_ME      U0ME
        #define UIO_IE      U0IE
        #define UIO_IFG     U0IFG
        #define UIO_STAT    U0STAT
        
        // Flags
        #define UIO_RXEN    URXE0
        #define UIO_TXEN    UTXE0
        #define UIO_RXIE    URXIE0
        #define UIO_TXIE    UTXIE0
        #define UIO_RXIFG   URXIFG0
        #define UIO_TXIFG   UTXIFG0
        #define UIO_BUSY    UCBUSY
        
        // ISR
        #define UIO_TXISR_VECTOR    USART0TX_VECTOR
        #define UIO_RXISR_VECTOR    USART0RX_VECTOR
    
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_EUSCI_A0__) // 6xx variant
        #define __MSP430_HAS_6xx_EUSCI__
        
        // Registers
        #define UIO_CTL0        UCA0CTL0
        #define UIO_CTL1        UCA0CTL1
        #define UIO_BR0         UCA0BR0
        #define UIO_BR1         UCA0BR1
        #define UIO_MCTL        UCA0MCTLW
        #define UIO_RXBUF       UCA0RXBUF
        #define UIO_TXBUF       UCA0TXBUF
        #define UIO_IFG         UCA0IFG
        #define UIO_STAT        UCA0STAT
        #define UIO_IV          UCA0IV
        #define UIO_IE          UCA0IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        #define UIO_BUSY        UCBUSY
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A0_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_USCI_A0__) // 5xx and 6xx variant
        #define __MSP430_HAS_5xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA0CTL0
        #define UIO_CTL1        UCA0CTL1
        #define UIO_BR0         UCA0BR0
        #define UIO_BR1         UCA0BR1
        #define UIO_MCTL        UCA0MCTL
        #define UIO_RXBUF       UCA0RXBUF
        #define UIO_TXBUF       UCA0TXBUF
        #define UIO_IFG         UCA0IFG
        #define UIO_STAT        UCA0STAT
        #define UIO_IV          UCA0IV
        #define UIO_IE          UCA0IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        #define UIO_BUSY        UCBUSY
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A0_VECTOR
        
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_USCI__) // 2xx and 4xx variant
        #define __MSP430_HAS_2xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA0CTL0
        #define UIO_CTL1        UCA0CTL1
        #define UIO_BR0         UCA0BR0
        #define UIO_BR1         UCA0BR1
        #define UIO_MCTL        UCA0MCTL
        #define UIO_RXBUF       UCA0RXBUF
        #define UIO_TXBUF       UCA0TXBUF
        #define UIO_IE          IE2
        #define UIO_IFG         IFG2
        
        // Flags
        #define UIO_TXIE        UCA0TXIE
        #define UIO_RXIE        UCA0RXIE
        #define UIO_TXIFG       UCA0TXIFG
        #define UIO_RXIFG       UCA0RXIFG
        
        // ISR
        #define UIO_TXISR_VECTOR    USCIAB0TX_VECTOR
        #define UIO_RXISR_VECTOR    USCIAB0RX_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #else
        #error "Invalid UIO_USE_DEV in uart_io_config.h"
    #endif
//==================================================================================================
#elif UIO_USE_DEV == 1
    #if defined(__MSP430_HAS_UART1__) // 1xx and 4xx variant
        #define __MSP430_HAS_1xx_UART__
        
        // Registers
        #define UIO_CTL     U1CTL
        #define UIO_TCTL    U1TCTL
        #define UIO_RCTL    U1RCTL
        #define UIO_BR0     U1BR0
        #define UIO_BR1     U1BR1
        #define UIO_MCTL    U1MCTL
        #define UIO_RXBUF   U1RXBUF
        #define UIO_TXBUF   U1TXBUF
        #define UIO_ME      U1ME
        #define UIO_IE      U1IE
        #define UIO_IFG     U1IFG
        
        // Flags
        #define UIO_RXEN    URXE1
        #define UIO_TXEN    UTXE1
        #define UIO_RXIE    URXIE1
        #define UIO_TXIE    UTXIE1
        #define UIO_RXIFG   URXIFG1
        #define UIO_TXIFG   UTXIFG1
        
        // ISR
        #define UIO_TXISR_VECTOR    USART1TX_VECTOR
        #define UIO_RXISR_VECTOR    USART1RX_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_EUSCI_A1__) // 6xx variant
        #define __MSP430_HAS_6xx_EUSCI__
        
        // Registers
        #define UIO_CTL0        UCA1CTL0
        #define UIO_CTL1        UCA1CTL1
        #define UIO_BR0         UCA1BR0
        #define UIO_BR1         UCA1BR1
        #define UIO_MCTL        UCA1MCTLW
        #define UIO_RXBUF       UCA1RXBUF
        #define UIO_TXBUF       UCA1TXBUF
        #define UIO_IFG         UCA1IFG
        #define UIO_IV          UCA1IV
        #define UIO_IE          UCA1IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A1_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_USCI_A1__) // 5xx and 6xx variant
        #define __MSP430_HAS_5xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA1CTL0
        #define UIO_CTL1        UCA1CTL1
        #define UIO_BR0         UCA1BR0
        #define UIO_BR1         UCA1BR1
        #define UIO_MCTL        UCA1MCTL
        #define UIO_RXBUF       UCA1RXBUF
        #define UIO_TXBUF       UCA1TXBUF
        #define UIO_IFG         UCA1IFG
        #define UIO_IV          UCA1IV
        #define UIO_IE          UCA1IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        
        // ISR
        #define UIO_ISR_VECTOR   USCI_A1_VECTOR
        
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_USCI_AB1__) // 2xx and 4xx variant
        #define __MSP430_HAS_2xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA1CTL0
        #define UIO_CTL1        UCA1CTL1
        #define UIO_BR0         UCA1BR0
        #define UIO_BR1         UCA1BR1
        #define UIO_MCTL        UCA1MCTL
        #define UIO_RXBUF       UCA1RXBUF
        #define UIO_TXBUF       UCA1TXBUF
        #define UIO_IE          UC1IE
        #define UIO_IFG         UC1IFG
        
        // Flags
        #define UIO_TXIE        UCA1TXIE
        #define UIO_RXIE        UCA1RXIE
        #define UIO_TXIFG       UCA1TXIFG
        #define UIO_RXIFG       UCA1RXIFG
        
        // ISR
        #define UIO_TXISR_VECTOR    USCIAB1TX_VECTOR
        #define UIO_RXISR_VECTOR    USCIAB1RX_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #else
        #error "Invalid UIO_USE_DEV in uart_io_config.h"
    #endif
//==================================================================================================
#elif UIO_USE_DEV == 2
    #if defined(__MSP430_HAS_EUSCI_A2__) // 6xx variant
        #define __MSP430_HAS_6xx_EUSCI__
        
        // Registers
        #define UIO_CTL0        UCA2CTL0
        #define UIO_CTL1        UCA2CTL1
        #define UIO_BR0         UCA2BR0
        #define UIO_BR1         UCA2BR1
        #define UIO_MCTL        UCA2MCTLW
        #define UIO_RXBUF       UCA2RXBUF
        #define UIO_TXBUF       UCA2TXBUF
        #define UIO_IFG         UCA2IFG
        #define UIO_IV          UCA2IV
        #define UIO_IE          UCA2IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A2_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #elif defined(__MSP430_HAS_USCI_A2__) // 5xx and 6xx variant
        #define __MSP430_HAS_5xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA2CTL0
        #define UIO_CTL1        UCA2CTL1
        #define UIO_BR0         UCA2BR0
        #define UIO_BR1         UCA2BR1
        #define UIO_MCTL        UCA2MCTL
        #define UIO_RXBUF       UCA2RXBUF
        #define UIO_TXBUF       UCA2TXBUF
        #define UIO_IFG         UCA2IFG
        #define UIO_IV          UCA2IV
        #define UIO_IE          UCA2IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A2_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #else
        #error "Invalid UIO_USE_DEV in uart_io_config.h"
    #endif
//==================================================================================================
#elif UIO_USE_DEV == 3
    #if defined(__MSP430_HAS_USCI_A3__) // 5xx and 6xx variant
        #define __MSP430_HAS_5xx_USCI__
        
        // Registers
        #define UIO_CTL0        UCA3CTL0
        #define UIO_CTL1        UCA3CTL1
        #define UIO_BR0         UCA3BR0
        #define UIO_BR1         UCA3BR1
        #define UIO_MCTL        UCA3MCTL
        #define UIO_RXBUF       UCA3RXBUF
        #define UIO_TXBUF       UCA3TXBUF
        #define UIO_IFG         UCA3IFG
        #define UIO_IV          UCA3IV
        #define UIO_IE          UCA3IE
        
        // Flags
        #define UIO_TXIE        UCTXIE
        #define UIO_RXIE        UCRXIE
        #define UIO_TXIFG       UCTXIFG
        #define UIO_RXIFG       UCRXIFG
        
        // ISR
        #define UIO_ISR_VECTOR  USCI_A3_VECTOR
        
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    #else
        #error "Invalid UIO_USE_DEV in uart_io_config.h"
    #endif
//==================================================================================================
#else
    #error "Invalid UIO_USE_DEV in uart_io_config.h"
#endif


//==================================================================================================
// Baud Rates
//==================================================================================================

#if defined(__MSP430_HAS_1xx_UART__)  // - - - - - - - - - - - - - - - - - - - - - - - - - -
    #define UIO_BR0_VALUE   USART_BR0_VALUE
    #define UIO_BR1_VALUE   USART_BR1_VALUE
    #define UIO_MCTL_VALUE  USART_MCTL_VALUE
    
#elif defined(__MSP430_HAS_2xx_USCI__) || defined(__MSP430_HAS_5xx_USCI__)  // - - - - - - -
    #define UIO_BR0_VALUE   USCI_BR0_VALUE
    #define UIO_BR1_VALUE   USCI_BR1_VALUE
    #define UIO_MCTL_VALUE  USCI_MCTL_VALUE
    
#elif defined(__MSP430_HAS_6xx_EUSCI__)   // - - - - - - - - - - - - - - - - - - - - - - - -
    #define UIO_BR0_VALUE   EUSCI_BR0_VALUE
    #define UIO_BR1_VALUE   EUSCI_BR1_VALUE
    #define UIO_MCTL_VALUE  EUSCI_MCTL_VALUE
    
#endif

//==================================================================================================
// DMA Controller Selection
//==================================================================================================

// Check if requested DMA channel exists
#if defined(__MSP430_HAS_DMA_1__)
    #if(UIO_RX_MODE == 2) && (UIO_RX_DMA_CHANNEL > 0)
        #error The selected UIO_RX_DMA_CHANNEL is invalid
    #endif
#elif defined(__MSP430_HAS_DMA_3__)
    #if(UIO_RX_MODE == 2) && (UIO_RX_DMA_CHANNEL > 2)
        #error The selected UIO_RX_DMA_CHANNEL is invalid
    #endif
#elif defined(__MSP430_HAS_DMAX_3__)
    #if(UIO_RX_MODE == 2) && (UIO_RX_DMA_CHANNEL > 2)
        #error The selected UIO_RX_DMA_CHANNEL is invalid
    #endif
#elif defined(__MSP430_HAS_DMAX_6__)
    #if(UIO_RX_MODE == 2) && (UIO_RX_DMA_CHANNEL > 5)
        #error The selected UIO_RX_DMA_CHANNEL is invalid
    #endif
#endif


#define _TPASTE3(a,b,c)  a##b##c
#define TPASTE3(a,b,c)  _TPASTE3(a,b,c)

// Registers
#define RX_DMA_CTL      TPASTE3(DMA, UIO_RX_DMA_CHANNEL, CTL)
#define RX_DMA_SA       TPASTE3(DMA, UIO_RX_DMA_CHANNEL, SA)
#define RX_DMA_DA       TPASTE3(DMA, UIO_RX_DMA_CHANNEL, DA)
#define RX_DMA_SZ       TPASTE3(DMA, UIO_RX_DMA_CHANNEL, SZ)

#if defined (__TI_COMPILER_VERSION__)
typedef __SFR_FARPTR uio_dma_addr;
#else
typedef uintptr_t    uio_dma_addr;
#endif

// Determine Trigger control register and corresponding mask.
#if defined(__MSP430_HAS_DMA_1__)
    #define RX_DMA_TRG          DMACTL0
    #define RX_DMA_TSEL_MASK    0x000F
#elif defined(__MSP430_HAS_DMA_3__) || defined(__MSP430_HAS_DMAX_3__)    
    #define RX_DMA_TRG          DMACTL0
    
    #if(UIO_RX_DMA_CHANNEL == 0)
        #define RX_DMA_TSEL_MASK    0x000F
    #elif(UIO_RX_DMA_CHANNEL == 1)
        #define RX_DMA_TSEL_MASK    0x00F0
    #elif(UIO_RX_DMA_CHANNEL == 2)
        #define RX_DMA_TSEL_MASK    0x0F00
    #endif
#elif defined(__MSP430_HAS_DMAX_6__)
    #if(UIO_RX_DMA_CHANNEL == 0)
        #define RX_DMA_TRG          DMACTL0
        #define RX_DMA_TSEL_MASK    0x001F
    #elif(UIO_RX_DMA_CHANNEL == 1)
        #define RX_DMA_TRG          DMACTL0
        #define RX_DMA_TSEL_MASK    0x1F00
    #elif(UIO_RX_DMA_CHANNEL == 2)
        #define RX_DMA_TRG          DMACTL1
        #define RX_DMA_TSEL_MASK    0x001F
    #elif(UIO_RX_DMA_CHANNEL == 3)
        #define RX_DMA_TRG          DMACTL1
        #define RX_DMA_TSEL_MASK    0x1F00
    #elif(UIO_RX_DMA_CHANNEL == 4)
        #define RX_DMA_TRG          DMACTL2
        #define RX_DMA_TSEL_MASK    0x001F
    #elif(UIO_RX_DMA_CHANNEL == 5)
        #define RX_DMA_TRG          DMACTL2
        #define RX_DMA_TSEL_MASK    0x1F00
    #elif(UIO_RX_DMA_CHANNEL == 6)
        #define RX_DMA_TRG          DMACTL3
        #define RX_DMA_TSEL_MASK    0x001F
    #elif(UIO_RX_DMA_CHANNEL == 6)
        #define RX_DMA_TRG          DMACTL3
        #define RX_DMA_TSEL_MASK    0x1F00
    #endif
#endif

#endif

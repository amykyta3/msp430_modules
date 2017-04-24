/**
* \addtogroup MOD_UART
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_UART
* \author Alex Mykyta 
**/

#ifndef UART_IO_CONFIG_H
#define UART_IO_CONFIG_H

//==================================================================================================
/// \name Configuration
/// Configuration defines for the \ref MOD_UART module
/// \{
//==================================================================================================

//  ===================================================
//  = NOTE: Actual ports must be configured manually! =
//  ===================================================

#include "../board_settings.h"

//--------------------------------------------------------------------------------------------------
// RX Settings
//--------------------------------------------------------------------------------------------------

#define UIO_RX_MODE         2
/**<    0 = Polling Mode    : No buffers used. RX is done directly by polling within function calls. \n
*       1 = Interrupt Mode  : RX triggers an interrupt and data is pushed into a FIFO. \n
*       2 = DMA Mode        : RX data is automatically loaded into a buffer using DMA.
**/

// RX buffer size (modes 1 & 2 only)
#define UIO_RXBUF_SIZE      64    ///< \hideinitializer

// RX DMA Channel (mode 2 only)
#define UIO_RX_DMA_CHANNEL  0

// RX DMA Trigger Source
//  Choose the corresponding trigger (see datasheet) that matches the DMA channel and UART RX device 
#define RX_DMA_TSEL         BOARD_RX_DMA_TSEL

//--------------------------------------------------------------------------------------------------
// TX Settings
//--------------------------------------------------------------------------------------------------

#define UIO_TX_MODE         1
/**<    0 = Polling Mode    : No buffers used. TX functions will block until data is transmitted. \n
*       1 = Interrupt Mode  : TX data is buffered. TX operations are handled using interrupts. \n
**/

// TX buffer size (mode 1 only)
#define UIO_TXBUF_SIZE      32    ///< \hideinitializer


//--------------------------------------------------------------------------------------------------
// Common Settings
//--------------------------------------------------------------------------------------------------

/// Select which USCI/USART module to use
#define UIO_USE_DEV         BOARD_UART_DEV    ///< \hideinitializer
/**<    0 = USCIA0 \n
*       1 = USCIA1 \n
*       2 = USCIA2 \n
*       3 = USCIA3
**/

/// Select which clock source to use
#define UIO_CLK_SRC         2    ///< \hideinitializer
/**<    0 = External \n
*       1 = ACLK     \n
*       2 = SMCLK
**/


// Modulation register settings for MSP430 are annoying to automatically determine at compile time.
// Instead, fill them out here.
// Note: MCTL implementations vary based on UART architectures. Most online calculators will only
//   generate MCTL values for the 1xx version.

// UART clock = 4.9152 MHz
// Baud = 9600

// Baud rate register settings for USART (found in 1xx and 4xx devices)
#define USART_BR0_VALUE     0x00
#define USART_BR1_VALUE     0x02
#define USART_MCTL_VALUE    0x00

// Baud rate register settings for USCI (found in 2xx, 4xx, 5xx, and 6xx devices)
#define USCI_BR0_VALUE      0x00
#define USCI_BR1_VALUE      0x02
#define USCI_MCTL_VALUE     0x00

// Baud rate register settings for USCI (found in 6xx devices)
#define EUSCI_BR0_VALUE     0x00
#define EUSCI_BR1_VALUE     0x02
#define EUSCI_MCTL_VALUE    0x0000

///\}
    
#endif
///\}

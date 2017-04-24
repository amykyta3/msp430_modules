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
* \addtogroup MOD_SPI SPI Bus
* \brief Provides basic functions for the MSP430 SPI controller
* \author Alex Mykyta
*
* This module abstracts out the SPI controller to operate as a master to slave-only peripherals. \n
*
* ### MSP430 Processor Families Supported: ###
*   Family  | Supported
*   ------- | ----------
*   1xx     | -
*   2xx     | -
*   4xx     | -
*   5xx     | Yes
*   6xx     | Yes
* 
* \todo Add support for 1xx, 2xx, and 4xx series
*
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_SPI "SPI Bus"
* \author Alex Mykyta
**/

#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "spi_config.h"
//==================================================================================================
// Defines
//==================================================================================================

/** \name SPI Modes
*    \brief Use these defines to set the SPI mode.
*    \details Used for the \c spi_mode parameter of spiInit() \n
*    \image html http://upload.wikimedia.org/wikipedia/commons/6/6b/SPI_timing_diagram2.svg "Various SPI modes"
* \{
**/
#define SPI_MODE0    (0+UCCKPH)         ///< \brief CPOL = 0, CPHA = 0 \hideinitializer
#define SPI_MODE1    (0+UCCKPL+UCCKPH)  ///< \brief CPOL = 0, CPHA = 1 \hideinitializer
#define SPI_MODE2    (0)                ///< \brief CPOL = 1, CPHA = 0 \hideinitializer
#define SPI_MODE3    (0+UCCKPL)         ///< \brief CPOL = 1, CPHA = 1 \hideinitializer
///\}

//==================================================================================================
// Function Prototypes
//==================================================================================================

/**
* \brief Initializes the SPI controller
* \param spi_mode The SPI mode the controller will operate in
* \attention The initialization routine does \e not setup the IO ports!
**/
void spi_init(uint8_t spi_mode);

/**
* \brief Uninitializes the SPI controller
**/
void spi_uninit(void);

/**
* \brief Sends and receives a single byte
* \param data Byte to be sent
* \returns Byte received
**/
uint8_t spi_transfer_byte(uint8_t data);

/**
* \brief Read a series of bytes from the SPI slave
* \param [in] size Number of bytes to read
* \param [out] dst Data read
**/
void spi_rx_frame(uint8_t* dst, uint16_t size);

/**
* \brief Write a series of bytes to the SPI slave
* \param [in] size Number of bytes to write
* \param [in] src Data to be written
**/
void spi_tx_frame(const uint8_t* src, uint16_t size);

/**
* \brief Read and write a series of bytes to the SPI slave
* \param [in] size Number of bytes to transfer
* \param [out] dst Data read
* \param [in] src Data to be written
**/
void spi_transfer_frame(uint8_t* dst, const uint8_t* src, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
///\}

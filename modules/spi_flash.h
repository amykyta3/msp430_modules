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
* \addtogroup MOD_SPI_FLASH SPI Flash
* \brief Interface driver for most SPI Flash devices
* 
* \author Alex Mykyta
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_SPI_FLASH
* \author Alex Mykyta
* This module also requires the following module:
*    - \ref MOD_SPI
* 
**/

#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <result.h>

/**
 * \brief Initializes the array of SPI flash devices
 * \retval RES_OK
 * \retval RES_FAIL
 * \attention The initialization routine does \e not setup the IO ports!
 **/
RES_t spi_flash_init(void);

/**
 * \brief Uninitializes the array of SPI flash devices
 **/
void spi_flash_uninit(void);

/**
 * \brief Read data from the spi flash volume
 * \param address Start address of read operation
 * \param nBytes Number of bytes to be read
 * \param [out] dst Destination buffer
 * \retval RES_OK
 * \retval RES_PARAMERR Invalid address range
 **/
RES_t spi_flash_read(uint32_t address, void *dst, uint16_t nBytes);

/**
 * \brief Write data to the spi flash volume
 * 
 * \note Writing a multiple of 2 bytes at a time and aligned to an even address is recommended
 * 
 * \param address Start address of write operation
 * \param nBytes Number of bytes to be written
 * \param [in] src Data to be written
 * \retval RES_OK
 * \retval RES_PARAMERR Invalid address range
 **/
RES_t spi_flash_write(uint32_t address, const void *src, uint16_t nBytes);

/**
 * \brief Initiates an erase of a 4k region
 * 
 * Determine if the operation is complete by polling spi_flash_erase_busy()
 * 
 * \param [in] address Address within the erase region
 * \retval RES_OK
 * \retval RES_PARAMERR Invalid address
 **/
RES_t spi_flash_4k_erase(uint32_t address);

/**
 * \brief Initiates an erase of a 32k region
 * 
 * Determine if the operation is complete by polling spi_flash_erase_busy()
 * 
 * \param [in] address Address within the erase region
 * \retval RES_OK
 * \retval RES_PARAMERR Invalid address
 **/
RES_t spi_flash_32k_erase(uint32_t address);

/**
 * \brief Initiates an erase of a 64k region
 * 
 * Determine if the operation is complete by polling spi_flash_erase_busy()
 * 
 * \param [in] address Address within the erase region
 * \retval RES_OK
 * \retval RES_PARAMERR Invalid address
 **/
RES_t spi_flash_64k_erase(uint32_t address);

/**
 * \brief Initiates a bulk erase the entire flash volume
 * \warning On some devices this can take several \e minutes. Determine if the operation is complete
 * by polling spi_flash_erase_busy()
 **/
void spi_flash_bulk_erase(void);

/**
 * \brief Determines if an erase operation is in progress
 **/
bool spi_flash_erase_busy(void);

/**
 * \brief Initiates a sequential write operation
 * 
 * Sequential accesses are intended for situations where data will be written to flash sequentially
 * but bulk buffer transfers are not practical.
 * A sequential write must be initiated using spi_flash_seq_wr_start(). Following this, multiple
 * calls to spi_flash_seq_wr_byte() or spi_flash_seq_wr() can be made to write a stream of data.
 * The operation must be terminated using spi_flash_seq_wr_end().
 * 
 * \warning Sequential accesses will not automatically roll over to the next SPI device in the
 * array! These functions are intended to be lightweight and do not check address bounds!
 * 
 * \param address start address
 **/
void spi_flash_seq_wr_start(uint32_t address);

/**
 * \brief Reads data sequentially
 * \param nBytes Number of bytes to be written
 * \param [in] src Data to be written
 **/
void spi_flash_seq_wr(const void *src, uint16_t nBytes);

/**
 * \brief Writes the next byte in a sequential write operation
 * \param b Byte to be written
 **/
void spi_flash_seq_wr_byte(uint8_t b);

/**
 * \brief Terminates a sequential write operation
 **/
void spi_flash_seq_wr_end(void);

/**
 * \brief Initiates a sequential read operation
 * 
 * Sequential accesses are intended for situations where data will be read from flash sequentially
 * but bulk buffer transfers are not practical.
 * A sequential read must be initiated using spi_flash_seq_rd_start(). Following this, multiple
 * calls to spi_flash_seq_rd_byte() or spi_flash_seq_rd() can be made to read a stream of data.
 * The operation must be terminated using spi_flash_seq_rd_end().
 * 
 * \warning Sequential accesses will not automatically roll over to the next SPI device in the
 * array! These functions are intended to be lightweight and do not check address bounds!
 * 
 * \param address start address
 **/
void spi_flash_seq_rd_start(uint32_t address);

/**
 * \brief Reads data sequentially
 * \param nBytes Number of bytes to be read
 * \param [out] dst Destination buffer
 **/
void spi_flash_seq_rd(void *dst, uint16_t nBytes);

/**
 * \brief Reads the next byte in a sequential read operation
 * \returns byte
 **/
uint8_t spi_flash_seq_rd_byte(void);

/**
 * \brief Terminates a sequential read operation
 **/
void spi_flash_seq_rd_end(void);

/**
 * \brief Get the size of the flash array in bytes
 * \return number of bytes
 **/
uint32_t spi_flash_size(void);

#ifdef __cplusplus
}
#endif

#endif
///\}

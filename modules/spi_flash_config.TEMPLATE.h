/**
* \addtogroup MOD_SPI_FLASH
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_SPI_FLASH
* \author Alex Mykyta
**/

#ifndef SPI_FLASH_CONFIG_H
#define SPI_FLASH_CONFIG_H

//==================================================================================================
/// \name Configuration
/// Configuration defines for the \ref MOD_SPI_FLASH module
/// \{
//==================================================================================================

// CE output port where the CE pins are located
#define CE_POUT         P1OUT

// Array of one-hot bit locations for each of the flash devices' CE pin
#define CE_DEV_BITMAP   {BIT6, BIT7}
#define CE_DEV_BITMASK  (BIT6|BIT7)
#define DEVICE_COUNT    2

// Port and pin where the SPI serial-in pin is located
#define MISO_DIR_PORT   P2DIR
#define MISO_IN_PORT    P2IN
#define MISO_IN_PIN     BIT0

// Minimum MCLK frequency (Hz) at the time that spi_flash_init() is called
#define F_CPU   1000000UL

//--------------------------------------------------------------------------------------------------
// Device features
//--------------------------------------------------------------------------------------------------

// Expected 3-byte JEDEC device ID of the part being used
#define DEVICE_ID       0x000000L

// Capacity of a single device (bytes)
#define DEVICE_SIZE     0x1000L

#define DEVICE_HAS_DEEP_POWER_DOWN  1

///\}
#endif

///\}

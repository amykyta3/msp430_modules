/**
* \addtogroup MOD_I2C
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_I2C
* \author Alex Mykyta 
**/

#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

//==================================================================================================
/// \name Configuration
/// Configuration defines for the \ref MOD_I2C module
/// \{
//==================================================================================================

//  ===================================================
//  = NOTE: Actual ports must be configured manually! =
//  ===================================================

/// Select which USCI module to use
#define I2C_USE_DEV        0    ///< \hideinitializer
/**<    0 = USCIB0 \n
*         1 = USCIB1 \n
*         2 = USCIB2 \n
*         3 = USCIB3
**/

/// Select which clock source to use
#define I2C_CLK_SRC        2    ///< \hideinitializer
/**<    0 = External \n
*        1 = ACLK    \n
*        2 = SMCLK
**/

/// Clock Division (0x0004 to 0xFFFF)
#define I2C_CLK_DIV        100

///\}
    
#endif
///\}

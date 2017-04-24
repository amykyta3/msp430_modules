/**
* \addtogroup MOD_TIMER
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_TIMER
* \author Alex Mykyta 
**/

#ifndef TIMER_CONFIG_H
#define TIMER_CONFIG_H

//==================================================================================================
/** \name Configuration Defines
*    \brief Configuration defines for the \ref MOD_TIMER module
*
* The timer module uses the MSP430's hardware timer. Since it only uses Capture-Control block 0, it
* can share the same hardware timer device with the following other modules:
*    - \ref MOD_BUTTON (Only uses Capture-Control blocks 1 and 2)
*
*    To ensure proper operation when sharing the timer, All of the timer settings must be identical.
*    Other signals using the same IO port cannot use interrupts outside of this module.
* \{ **/
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// Clock Setup
//--------------------------------------------------------------------------------------------------

// If using the Clock System module, #include the clock_sys.h header to provide clock information.
// Otherwise, comment it out and enter the SMCLK or ACLK frequencies manually below.
#include <clock_sys.h>

///\brief Enter the ACLK clock frequency in Hz
///\note This is not required if clock_sys.h is included above
#ifndef ACLK_FREQ
    #define ACLK_FREQ   32768    ///< \hideinitializer
#endif

///\brief Enter the SMCLK clock frequency in Hz
///\note This is not required if clock_sys.h is included above
#ifndef SMCLK_FREQ
    #define SMCLK_FREQ  4000000    ///< \hideinitializer
#endif

//--------------------------------------------------------------------------------------------------
// Timer Setup
//--------------------------------------------------------------------------------------------------

/// Select which Timer module to use
#define TIMER_USE_DEV       0    ///< \hideinitializer
/**<    0 = Timer A0 \n
*       1 = Timer A1 \n
*       2 = Timer A2
**/

/// Select which timer clock source to use
#define TIMER_CLK_SRC       1    ///< \hideinitializer
/**<    1 = ACLK    \n
*       2 = SMCLK
**/

/// Select which clock division to use
#define TIMER_IDIV          3    ///< \hideinitializer
/**<    0 = /1 \n
*       1 = /2 \n
*       2 = /4 \n
*       3 = /8 \n
**/

/// Select which extended clock division to use (only available for 5xx and 6xx devices)
#define TIMER_IDIVEX        0    ///< \hideinitializer
/**<    0 = /1 \n
*       1 = /2 \n
*       2 = /3 \n
*       3 = /4 \n
*       4 = /5 \n
*       5 = /6 \n
*       6 = /7 \n
*       7 = /8 \n
**/


///\}
    
#endif /*_TIMER_CONFIG_H_*/
///\}

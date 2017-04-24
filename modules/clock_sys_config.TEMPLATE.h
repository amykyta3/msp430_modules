/**
* \addtogroup MOD_CLOCKSYS
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_CLOCKSYS "Clock System"
* \author Alex Mykyta 
**/

///\}

#ifndef CLOCK_SYS_CONFIG_H
#define CLOCK_SYS_CONFIG_H

//==================================================================================================
// Crystals
//==================================================================================================
#define XT1_FREQ        32768L    // use 0 to disable
#define LFXT_LOAD_CAP   9        // crystal's rated load cap in pF (NOT the required effective cap)

#define XT2_FREQ        4000000L    // use 0 to disable

//==================================================================================================
// Clock Routing
//==================================================================================================
// See device-specific datasheet to see what kinds of routing are feasible
#define ACLK_SRC    1
#define SMCLK_SRC   0
#define MCLK_SRC    0
//        0 = DCO
//        1 = XT1
//        2 = XT2
//        3 = VLO
//        4 = REFO

//==================================================================================================
// DCO
//==================================================================================================
#define TARGET_DCO_FREQ         1000000L

// If the clock system has an FLL, which clock should be used as a reference?
#define FLL_REF_SRC             3
//        0 = XT1
//        1 = XT2
//        2 = VLO
//        3 = REFO

// Set to 1 to enable manual configuration of the DCO.
// If set to 0, best-fit settings will be attempted.
#define MANUALLY_CONFIG_DCO     0 // 1 or 0
//--------------------------------------------------------------------------------------------------
// Manual Configuration: 1xx and 2xx devices
//--------------------------------------------------------------------------------------------------
    #define MANUAL_DCO_RSEL     0
    #define MANUAL_DCO_DCO      0
    #define MANUAL_DCO_MOD      0

//--------------------------------------------------------------------------------------------------
// Manual Configuration: 4xx devices
//--------------------------------------------------------------------------------------------------
    #define MANUAL_FLLPLUS_FLLD     1
    //        0 = /1
    //        1 = /2
    //        2 = /4
    //        3 = /8
    
    #define MANUAL_FLLPLUS_N        60
    
//--------------------------------------------------------------------------------------------------
// Manual Configuration: 5xx and 6xx devices
//--------------------------------------------------------------------------------------------------
    #define MANUAL_FLLREFDIV    1
    //        0 = /1
    //        1 = /2
    //        2 = /4
    //        3 = /8
    //        4 = /12
    //        5 = /16

    #define MANUAL_FLLD         0
    //        0 = /1
    //        1 = /2
    //        2 = /4
    //        3 = /8
    //        4 = /16
    //        5 = /32

    #define MANUAL_FLLN         60

//==================================================================================================
// Initial Conditions
//==================================================================================================
// See device-specific datasheet to see what kinds of division factors are feasible

#define ACLK_DIV    0
/**<    0 = /1      \n
*       1 = /2      \n
*       2 = /4      \n
*       3 = /8      \n
*       4 = /16     \n
*       5 = /32
**/

#define SMCLK_DIV    2
/**<    0 = /1       \n
*       1 = /2       \n
*       2 = /4       \n
*       3 = /8       \n
*       4 = /16      \n
*       5 = /32
**/

#define MCLK_DIV    2
/**<    0 = /1      \n
*       1 = /2      \n
*       2 = /4      \n
*       3 = /8      \n
*       4 = /16     \n
*       5 = /32
**/

// Restrict the minimum clock division to set the maximum MCLK frequency allowed. Doing so lets the
// clock system reduce the core voltage (when supported) to save power
#define MCLK_DIV_MINIMUM_RESTRICT    0
/**<    0 = /1      \n
*       1 = /2      \n
*       2 = /4      \n
*       3 = /8      \n
*       4 = /16     \n
*       5 = /32
**/

#endif

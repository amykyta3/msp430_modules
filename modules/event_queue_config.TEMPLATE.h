/**
* \addtogroup MOD_EVENT_QUEUE
* \{
**/

/**
* \file
* \brief Configuration include file for \ref MOD_EVENT_QUEUE
* \author Alex Mykyta 
**/

#ifndef EVENT_QUEUE_CONFIG_H
#define EVENT_QUEUE_CONFIG_H
//==================================================================================================
/// \name Configuration
/// Configuration for the Event Queue module
/// \{
//==================================================================================================


/// Number of bytes to reserve for the event queue
#define EVENT_QUEUE_SIZE    128 ///< \hideinitializer


/// Maximum number of yielded event levels
#define MAX_YIELD_DEPTH        2 ///< \hideinitializer

///\}    
#endif
///\}

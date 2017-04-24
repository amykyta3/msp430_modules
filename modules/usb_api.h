/*
* Copyright (c) 2012, Alexander I. Mykyta
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
* \addtogroup MOD_USB USB API
* \brief API for MSP430's USB peripheral. Supports HID-Datapipe, Traditional HID and CDC
* \author Links to original USB API by Texas Instruments
* \author Modified by Alex Mykyta  to work with personal library
* 
* \details
* <b> MSP430 Processor Families Supported: </b>
*    - MSP430x5xx
*    - MSP430x6xx
*
* <b> Compilers Supported: </b>
*    - TI CCS v4
*    - MSPGCC
*
* <b> Implementation Suggestion: </b> \n
*    The following code should be placed in the initialization routine event to properly set up the 
*    USB API:
* \code

USB_init();

// Enable various USB event handling routines
USB_setEnabledEvents(USBEV_ALLEVENTS_EN);

// See if we're already attached physically to USB, and if so, connect to it 
if (USB_connectionInfo() & USB_VBUS_PRESENT){
    if (USB_enable() == 0){
        USB_reset();
        USB_connect();
    }
}
* \endcode
* 
* The following code can be used in the onIdle() routine in order to poll the USB state:
* \code
switch(USB_connectionState()){
    case USBST_DISCONNECTED:
        // This state indicates that VBUS is not present, meaning there is no USB connection.  
        break;
    case USBST_CONNECTED_NO_ENUM:
        // This state indicates that VBUS is present but USB_connect() has not yet been called   
        break;
    case USBST_ENUM_ACTIVE:             
        // This state indicates that the device has successfully enumerated on a USB host and is 
        // connected and active.
        // The MSP430 should not be placed into a mode below LPM0 (no LPM3/4/5).  
        break;
    case USBST_ENUM_SUSPENDED:
        // This state indicates the device is enumerated on a USB host, but the host has suspended it.
        // During suspend, the power mode may enter LPM3 or LPM4. 
        break;
    case USBST_ENUM_IN_PROGRESS:
        // This state indicates the device is in the process of being enumerated on a USB host.
        // The MSP430 should not be placed into a mode below LPM0 (no LPM3/4/5).  
        break;
    case USBST_NOENUM_SUSPENDED:
        // This state indicates that VBUS is present, the device attempted to enumerate by pulling
        // PUR high, but then the host suspended the device.
        break;
    case USBST_ERROR:
        break;
}
* \endcode
*
* \todo Include MSC calls into this header (Very low priority)
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_USB "USB API"
* \author Alex Mykyta 
**/

#ifdef __DOXYGEN__
    #define _CDC_
    #define _HID_
    #define _MSC_
#endif

#ifndef USB_API_H
#define USB_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "USB_config/descriptors.h"
#include <result.h>

//==================================================================================================
// Definitions
//==================================================================================================



///\addtogroup USB_DEFINITIONS Definitions
///\{

///\name USB Event Identifiers
/// Passed into onUSB_Event() and onUSB_InterfaceEvent() to indicate which event occurred.
///\{
typedef enum{
    USBEV_CLOCKFAULT,       ///< Output of the USB PLL has failed
    USBEV_VBUSON,           ///< A valid voltage has just been applied to VBUS
    USBEV_VBUSOFF,          ///< A valid voltage has just been removed from VBUS
    USBEV_ENUMERATED,       ///< USB host has enumerated this device
    USBEV_RESET,            ///< USB host has issued a USB reset event to the device
    USBEV_SUSPEND,          ///< USB host has chosen to suspend this device from active state
    USBEV_RESUME,           ///< USB host has chosen to resume this device from suspended state
    USBEV_CDC_DATARECV,     ///< Data has been received but no receive operation is active
    USBEV_HID_DATARECV,     ///< Data has been received but no receive operation is active
    USBEV_CDC_SENDCOMPLETE, ///< USB_cdcIsend() completed
    USBEV_HID_SENDCOMPLETE, ///< USB_hidIsend() completed
    USBEV_CDC_RECVCOMPLETE, ///< USB_cdcIrecv() completed
    USBEV_HID_RECVCOMPLETE, ///< USB_hidIrecv() completed
    USBEV_MSC_BUFFEREVENT   ///< MSC Event...?
}USB_EVENT_t;
///\}

///\name Event Enable Flags
///\anchor USB_EVENT_EN
/// Used in the USB_setEnabledEvents and USB_getEnabledEvents functions
///\{
#define USBEV_CLOCKFAULT_EN     0x0001    ///< Enables the #USBEV_CLOCKFAULT event
#define USBEV_VBUSON_EN         0x0002    ///< Enables the #USBEV_VBUSON event
#define USBEV_VBUSOFF_EN        0x0004    ///< Enables the #USBEV_VBUSOFF event
#define USBEV_RESET_EN          0x0008    ///< Enables the #USBEV_RESET event
#define USBEV_SUSPEND_EN        0x0010    ///< Enables the #USBEV_SUSPEND event
#define USBEV_RESUME_EN         0x0020    ///< Enables the #USBEV_RESUME event
#define USBEV_DATARECV_EN       0x0040    ///< Enables both #USBEV_CDC_DATARECV and #USBEV_HID_DATARECV events
#define USBEV_SENDCOMPLETE_EN   0x0080    ///< Enables both #USBEV_CDC_SENDCOMPLETE and #USBEV_HID_SENDCOMPLETE events
#define USBEV_RECVCOMPLETE_EN   0x0100    ///< Enables both #USBEV_CDC_RECVCOMPLETE and #USBEV_HID_RECVCOMPLETE events
#define USBEV_ALLEVENTS_EN      0x01FF    ///< Enables all USB events
///\}

///\name Interface Status Flags
///\anchor USB_INTERFACE_STATUS
/// Returned from USB_cdcStatus() and USB_hidStatus()
///\{
#define USB_ISENDACTIVE         0x01
#define USB_IRECVACTIVE         0x02
#define USB_DATAWAITING         0x04 
#define USB_BUSNOTAVAIL         0x08
///\}

///\name Connection Info Flags
///\anchor USB_CONN_INFO
/// Returned from USB_connectionInfo()
///\{
#define USB_VBUS_PRESENT        0x01
#define USB_BUS_ACTIVE          0x02    // frame sync packets are being received
#define USB_CONNECT_NO_VBUS     0x04
#define USB_SUSPENDED           0x08
#define USB_NOT_SUSPENDED       0x10
#define USB_ENUMERATED          0x20
#define USB_PUR_HIGH            0x40
///\}

///\name Connection States
///\anchor USB_CONN_STATES
/// Returned from USB_connectionState()
///\{
#define USBST_DISCONNECTED          0x80 ///< USB is disconnected
#define USBST_CONNECTED_NO_ENUM     0x81 ///< USB connected but device is not enumerated
#define USBST_ENUM_IN_PROGRESS      0x82 ///< Enumeration is in progress
#define USBST_ENUM_ACTIVE           0x83 ///< Device is enumerated and the bus is active
#define USBST_ENUM_SUSPENDED        0x84 ///< Device is enumerated and the bus is supended
#define USBST_ERROR                 0x86 ///< Error!
#define USBST_NOENUM_SUSPENDED      0x87 ///< USB connected but the device was suspended without being enumerated
///\}

///\}

//==================================================================================================
// Functions
//==================================================================================================

///\name General USB Functions
///\{

/**
* \brief Initializes the USB module
* \retval 0
* \details This call prepares the USB module to detect the application of power to VBUS, after which
*    the application may choose to enable the module and connect to USB
**/
uint8_t USB_init(void); // always succeeds

/**
* \brief Enables the USB module and PLL
* \retval 0 Success
* \retval 1 General Error
* \details This call should only be made after an earlier call to USB_init(), and prior to any other
*    calls except USB_setEnabledEvents(), or USB_getEnabledEvents()
**/
uint8_t USB_enable(void); 

/**
* \brief Disables the USB module and PLL
* \retval 0
* \details If USB_connectionState() begins returning #USBST_DISCONNECTED, USB_disconnect() should be
*    called and then USB_disable() in order to avoid unnecessary current draw
**/
uint8_t USB_disable(void); // always succeeds

/**
* \brief Enables/disables various USB events
* \param events Collection of \ref USB_EVENT_EN "Event Enable Flags"
* \retval 0
* \details This function can be called at any time after a call to USB_init()
**/
uint8_t USB_setEnabledEvents(uint16_t events); // always succeeds

/**
* \brief Returns which events are enabled and which are disabled
* \returns Collection of \ref USB_EVENT_EN "Event Enable Flags"
* \details This function can be called at any time after a call to USB_init()
**/
uint16_t USB_getEnabledEvents(void);

/**
* \brief Resets the USB module and also the internal state of the API
* \retval 0
* \details This function is most often called immediately before a call to USB_connect(). It should 
*    not be called prior to USB_enable()
**/
uint8_t USB_reset(void); // always succeeds

/**
* \brief Instructs the USB module to make itself available to the host for connection
* \retval 0
* \details This call should only be made after a call to USB_enable()
**/
uint8_t USB_connect(void); // always succeeds

/**
* \brief Forces a logical disconnect from the USB host
* \retval 0
**/
uint8_t USB_disconnect(void); // always succeeds

/**
* \brief Returns low-level status information about the USB connection
* \return See \ref USB_CONN_INFO "Connection Info Flags"
**/
uint8_t USB_connectionInfo(void); // returns info bits

/**
* \brief Returns the state of the USB connection
* \return See \ref USB_CONN_STATES "Connection States"
**/
uint8_t USB_connectionState(void); // returns state

///\}

///\name CDC Functions
///\{
#ifdef _CDC_

/**
* \brief Send data over the USB CDC. (Blocking)
* \details Function returns once data has been sent and it is safe to start another send operation.
* \param [in] src        Pointer to the data to be sent
* \param [in] size        Number of bytes to send
* \param [in] intfNum    Interface number
* \param [in] Timeout    Number of times to poll the interface status. 0 disables timeout.
* \retval RES_OK        Send operation completed successfully
* \retval RES_BUSY        Another send operation is already in progress.
* \retval RES_FAIL        Send operation failed.
**/
RES_t USB_cdcSend(void *src, uint16_t size, uint8_t intfNum, uint32_t Timeout);

/**
* \brief Initiate a send operation over the USB CDC. (Immediate)
* \details Function initiates a send operation and returns immediately. The USB API begins fetching
*    data from the \c src buffer. Data stored in the \c src buffer must not be changed until the send
*    operation completes. This can be checked by polling the status using USB_cdcStatus()
* \param [in] src        Pointer to the data to be sent
* \param [in] size        Number of bytes to send
* \param [in] intfNum    Interface number
* \retval RES_OK        Send operation started successfully
* \retval RES_BUSY        Another send operation is already in progress.
* \retval RES_FAIL        Send operation failed.
**/
RES_t USB_cdcIsend(void *src, uint16_t size, uint8_t intfNum);

/**
* \brief Receives any data that has already been internally received by the USB API
* \param [in] dst        Pointer to the destination buffer
* \param [in] maxSize    Maximum number of bytes to receive
* \param [in] intfNum    Interface number
* \returns    Number of bytes received
**/
uint16_t USB_cdcRecvAvailable(void *dst, uint16_t maxSize, uint8_t intfNum);

/**
* \brief Receive data over the USB CDC. (Blocking)
* \details Function returns once data has been received and it is safe to start another receive
*    operation or if the timeout count has been exceeded.
* \param [in] dst        Pointer to the destination buffer
* \param [in] size        Number of bytes to receive
* \param [in] intfNum    Interface number
* \param [in] Timeout    Number of times to poll the interface status. 0 disables timeout.
* \retval RES_OK        Receive operation completed successfully
* \retval RES_BUSY        Another receive operation is already in progress.
* \retval RES_FAIL        Receive operation failed.
**/
RES_t USB_cdcRecv(void *dst, uint16_t size, uint8_t intfNum, uint32_t Timeout);

/**
* \brief Initiate a receive operation over the USB CDC. (Immediate)
* \details Function initiates a receive operation and returns immediately. The USB API begins
*    fetching data from the \c dst buffer. Data in the \c dst buffer is not valid until the receive
*    operation completes. This can be checked by polling the status using USB_cdcStatus()
* \param [in] dst        Pointer to the destination buffer
* \param [in] size        Number of bytes to receive
* \param [in] intfNum    Interface number
* \retval RES_OK        Receive operation started successfully
* \retval RES_BUSY        Another receive operation is already in progress.
* \retval RES_FAIL        Receive operation failed.
**/
RES_t USB_cdcIrecv(void *dst, uint16_t size, uint8_t intfNum);

/**
* \brief Checks the status of a USB interface
* \param [in] intfNum    Interface number
* \return Byte containing bit-flags which indicate the status of the USB API
* \retval USB_ISENDACTIVE    Bit indicates that a send operation is open on this interface
* \retval USB_IRECVACTIVE    Bit indicates that a receive operation is open on this interface
* \retval USB_DATAWAITING    Bit indicates that data has been received from the host for this
*    interface, waiting in the USB receive buffers, lacking an open receive operation to accept it.
* \retval USB_BUSNOTAVAIL    Bit indicates that the bus is either suspended or disconnected.
*    Any operations that had previously been underway are now aborted.
**/
uint8_t USB_cdcStatus(uint8_t intfNum);

/**
* \brief Returns the number of bytes recieved so far during an USB_cdcIrecv() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes received
**/
uint16_t USB_cdcIrecvCount(uint8_t intfNum);

/**
* \brief Aborts an active USB_cdcIsend() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes sent
**/
uint16_t USB_cdcAbortIsend(uint8_t intfNum);

/**
* \brief Aborts an active USB_cdcIrecv() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes received
**/
uint16_t USB_cdcAbortIrecv(uint8_t intfNum);

/**
* \brief Rejects any data in the USB buffer, for which a receive operation has not yet been started
* \param [in] intfNum    Interface number
* \return Nothing
**/
void USB_cdcRejectData(uint8_t intfNum);


#endif
///\}

///\name HID Datapipe Functions
///\{
#ifdef _HID_
/**
* \brief Send data over the USB HID-Datapipe. (Blocking)
* \details Function returns once data has been sent and it is safe to start another send operation
*    or if the timeout count has been exceeded.
* \param [in] src        Pointer to the data to be sent
* \param [in] size        Number of bytes to send
* \param [in] intfNum    Interface number
* \param [in] Timeout    Number of times to poll the interface status. 0 disables timeout.
* \retval RES_OK        Send operation completed successfully
* \retval RES_BUSY        Another send operation is already in progress.
* \retval RES_FAIL        Send operation failed.
**/
RES_t USB_hidSend(void *src, uint16_t size, uint8_t intfNum, uint32_t Timeout);

/**
* \brief Initiate a send operation over the USB HID-Datapipe. (Immediate)
* \details Function initiates a send operation and returns immediately. The USB API begins fetching
*    data from the \c src buffer. Data stored in the \c src buffer must not be changed until the send
*    operation completes. This can be checked by polling the status using USB_hidStatus()
* \param [in] src        Pointer to the data to be sent
* \param [in] size        Number of bytes to send
* \param [in] intfNum    Interface number
* \retval RES_OK        Send operation started successfully
* \retval RES_BUSY        Another send operation is already in progress.
* \retval RES_FAIL        Send operation failed.
**/
RES_t USB_hidIsend(void *src, uint16_t size, uint8_t intfNum);

/**
* \brief Receives any data that has already been internally received by the USB API
* \param [in] dst        Pointer to the destination buffer
* \param [in] maxSize    Maximum number of bytes to receive
* \param [in] intfNum    Interface number
* \returns    Number of bytes received
**/
uint16_t USB_hidRecvAvailable(void *dst, uint16_t maxSize, uint8_t intfNum);

/**
* \brief Receive data over the USB HID-Datapipe. (Blocking)
* \details Function returns once data has been received and it is safe to start another receive
*    operation or if the timeout count has been exceeded.
* \param [in] dst        Pointer to the destination buffer
* \param [in] size        Number of bytes to receive
* \param [in] intfNum    Interface number
* \param [in] Timeout    Number of times to poll the interface status. 0 disables timeout.
* \retval RES_OK        Receive operation completed successfully
* \retval RES_BUSY        Another receive operation is already in progress.
* \retval RES_FAIL        Receive operation failed.
**/
RES_t USB_hidRecv(void *dst, uint16_t size, uint8_t intfNum, uint32_t Timeout);

/**
* \brief Initiate a receive operation over the USB HID-Datapipe. (Immediate)
* \details Function initiates a receive operation and returns immediately. The USB API begins
*    fetching data from the \c dst buffer. Data in the \c dst buffer is not valid until the receive
*    operation completes. This can be checked by polling the status using USB_hidStatus()
* \param [in] dst        Pointer to the destination buffer
* \param [in] size        Number of bytes to receive
* \param [in] intfNum    Interface number
* \retval RES_OK        Receive operation started successfully
* \retval RES_BUSY        Another receive operation is already in progress.
* \retval RES_FAIL        Receive operation failed.
**/
RES_t USB_hidIrecv(void *dst, uint16_t size, uint8_t intfNum);

/**
* \brief Checks the status of a USB interface
* \param [in] intfNum    Interface number
* \return Byte containing bit-flags which indicate the status of the USB API
* \retval USB_ISENDACTIVE    Bit indicates that a send operation is open on this interface
* \retval USB_IRECVACTIVE    Bit indicates that a receive operation is open on this interface
* \retval USB_DATAWAITING    Bit indicates that data has been received from the host for this
*    interface, waiting in the USB receive buffers, lacking an open receive operation to accept it.
* \retval USB_BUSNOTAVAIL    Bit indicates that the bus is either suspended or disconnected.
*    Any operations that had previously been underway are now aborted.
**/
uint8_t USB_hidStatus(uint8_t intfNum);

/**
* \brief Returns the number of bytes recieved so far during an USB_hidIrecv() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes received
**/
uint16_t USB_hidIrecvCount(uint8_t intfNum);

/**
* \brief Aborts an active USB_hidIsend() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes sent
**/
uint16_t USB_hidAbortIsend(uint8_t intfNum);

/**
* \brief Aborts an active USB_hidIrecv() operation
* \param [in] intfNum    Interface number
* \return uint16_t Number of bytes received
**/
uint16_t USB_hidAbortIrecv(uint8_t intfNum);

/**
* \brief Rejects any data in the USB buffer, for which a receive operation has not yet been started
* \param [in] intfNum    Interface number
* \return Nothing
**/
void USB_hidRejectData(uint8_t intfNum);

///\}

///\name Traditional HID Functions
///\{

/**
* \brief Sends a pre-built report to the host
* \param [in] reportData Pointer to HID report data
* \param [in] intfNum    Interface number
* \retval RES_OK Data has been written to the USB transmit buffers, and will be transferred to the
*    host in the next polling frame
* \retval RES_BUSY USB buffer for the interface has data in it, suggesting that the host has not yet
*    fetched the previously-loaded report
* \retval RES_FAIL The bus has either been disconnected or the device is suspended, allowing no
*    reports to be sent
* \details \c reportData must be organized to reflect the format defined by the report descriptor in
*    descriptors.c.
**/
RES_t USB_hidSendReport(void *reportData, uint8_t intfNum);

/**
* \brief Receives a report from the host
* \param [in] reportData Pointer to HID report data
* \param [in] intfNum    Interface number
* \retval RES_OK Data has been successfully copied from the USB receive buffers into \c reportData
* \retval RES_FAIL The bus has either been disconnected or the device is suspended, allowing no
*    reports to be sent
* \details It is expected that the host will organize the report in the format defined by the report
*    descriptor in descriptors.c
**/
RES_t USB_hidRecvReport(void *reportData, uint8_t intfNum);
///\}
#endif

//==================================================================================================
// Events
//==================================================================================================

///\name Events
///\{

/**
* \brief Triggered by events relating to the state of the USB device
* \param event    See #USB_EVENT_t
* \details The following USB events may occur here:
*    - USBEV_CLOCKFAULT
*    - USBEV_VBUSON
*    - USBEV_VBUSOFF
*    - USBEV_ENUMERATED
*    - USBEV_RESET
*    - USBEV_SUSPEND
*    - USBEV_RESUME
*
* <b> Implementation Suggestion: </b> \n
*    The following code should be used to handle USB events
* \code
void onUSB_Event(USB_EVENT_t event){
    switch(event){
        case USBEV_CLOCKFAULT:
            
            break;
        case USBEV_VBUSON:
            
            break;
        case USBEV_VBUSOFF:
            
            break;
        case USBEV_ENUMERATED:
            
            break;
        case USBEV_RESET:
            
            break;
        case USBEV_SUSPEND:
            
            break;
        case USBEV_RESUME:
            
            break;
    }
}
* \endcode
**/
extern void onUSB_Event(USB_EVENT_t event);

//--------------------------------------------------------------------------------------------------
/**
* \brief Triggered by events relating to a CDC, HID, or MSC interface
* \param event    See #USB_EVENT_t
* \param intfNum Interface Number
* \details The following USB events may occur here:
*    - USBEV_CDC_DATARECV
*    - USBEV_CDC_SENDCOMPLETE
*    - USBEV_CDC_RECVCOMPLETE
*    - USBEV_HID_DATARECV
*    - USBEV_HID_SENDCOMPLETE
*    - USBEV_HID_RECVCOMPLETE
*    - USBEV_MSC_BUFFEREVENT
*
* <b> Implementation Suggestion: </b> \n
*    The following code should be used to handle interface events
* \code
void onUSB_InterfaceEvent(USB_EVENT_t event,uint8_t intfNum){
    switch(event){
        case USBEV_CDC_DATARECV:
            
            break;
        case USBEV_CDC_SENDCOMPLETE:
            
            break;
        case USBEV_CDC_RECVCOMPLETE:
            
            break;
        case USBEV_HID_DATARECV:
            
            break;
        case USBEV_HID_SENDCOMPLETE:
            
            break;
        case USBEV_HID_RECVCOMPLETE:
            
            break;
        case USBEV_MSC_BUFFEREVENT:
            
            break;
    }
}
* \endcode
*
**/
extern void onUSB_InterfaceEvent(USB_EVENT_t event,uint8_t intfNum);

///\}

/**
* \page EVENT_LIST_PAGE Event Listing
* 
* \section SEC_USB_EVENTS USB API Events
* \{
*    onUSB_Event() \n
*    onUSB_InterfaceEvent()
* \} 
**/

#ifdef __cplusplus
}
#endif

#endif

///\}

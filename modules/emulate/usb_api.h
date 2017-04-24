
#define _CDC_
#define _HID_
#define _MSC_

#ifndef USB_API_H_
#define USB_API_H_

#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif

///\}

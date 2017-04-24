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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       2011-09-07   born
* Alex M.       2013-08-01   Upgraded API to v3.20.02. Now compiles with MSPGCC
* 
*=================================================================================================*/

/**
* \addtogroup MOD_USB
* \{
**/
 
/**
* \file
* \brief User-interface code for \ref MOD_USB "USB API"
* \author Alex Mykyta 
**/

#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"          // Basic Type declarations
#include "USB_API/USB_Common/defMSP430USB.h"
#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/usb.h"        // USB-specific functions

#ifdef _CDC_
    #include "USB_API/USB_CDC_API/UsbCdc.h"
#endif
#ifdef _HID_
    #include "USB_API/USB_HID_API/UsbHid.h"
#endif
#ifdef _MSC_
    #include "USB_API/USB_MSC_API/UsbMsc.h"
#endif

#include <stdint.h>
#include <result.h>
#include "usb_api.h"
#include "event_queue.h"
#include "sleep.h"

///\cond INTERNAL

//==================================================================================================
// Internal Event Handling
//==================================================================================================

// blocking send/recv statuses
static union{
    struct{
        uint8_t        cdcSendComplete    :1;
        uint8_t        cdcRecvComplete    :1;
        uint8_t        hidSendComplete    :1;
        uint8_t        hidRecvComplete    :1;
    }flags;
    
    uint8_t all;
} usb_disable_event;

typedef struct{
    uint8_t eventid;
    uint8_t intfNum;
}EV_DATA_t;

//--------------------------------------------------------------------------------------------------
static void ev_USB_Event(void){
    uint8_t eventid;
    event_PopEventData(&eventid,sizeof(uint8_t));
    onUSB_Event((USB_EVENT_t)eventid);
}

//--------------------------------------------------------------------------------------------------
static void ev_USB_InterfaceEvent(void){
    EV_DATA_t event_data;
    event_PopEventData(&event_data,sizeof(EV_DATA_t));
    onUSB_InterfaceEvent((USB_EVENT_t)event_data.eventid, event_data.intfNum);
}

//--------------------------------------------------------------------------------------------------
/*
It's a sign that the output of the USB PLL has failed.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleClockEvent(void){
    uint8_t eventid;
    eventid = USBEV_CLOCKFAULT;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));
    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a valid voltage has just been applied to VBUS.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleVbusOnEvent(void){
    uint8_t eventid;
    eventid = USBEV_VBUSON;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));
    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a valid voltage has just been removed from VBUS.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleVbusOffEvent(void){
    uint8_t eventid;
    eventid = USBEV_VBUSOFF;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that the USB host has issued a USB reset event to the device.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleResetEvent(void){
    uint8_t eventid;
    eventid = USBEV_RESET;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that the USB host has chosen to suspend this device after a period of active operation.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleSuspendEvent(void){
    uint8_t eventid;
    eventid = USBEV_SUSPEND;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that the USB host has chosen to resume this device after a period of suspended operation.
returns TRUE to keep CPU awake
*/
uint8_t USB_handleResumeEvent(void){
    uint8_t eventid;
    eventid = USBEV_RESUME;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that the USB host has enumerated this device :
after host assigned the address to the device. 
returns TRUE to keep CPU awake
*/
uint8_t USB_handleEnumCompleteEvent(void){
    uint8_t eventid;
    eventid = USBEV_ENUMERATED;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
#ifdef _CDC_

/*
Indicates that data has been received for interface intfNum, but no receive operation is underway.
returns TRUE to keep CPU awake
*/
uint8_t USBCDC_handleDataReceived(uint8_t intfNum){
    EV_DATA_t event_data;
    event_data.eventid = USBEV_CDC_DATARECV;
    event_data.intfNum = intfNum;
    event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));

    return TRUE;   //return TRUE to wake the main loop (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a send operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
uint8_t USBCDC_handleSendCompleted(uint8_t intfNum){
    EV_DATA_t event_data;
    
    if(usb_disable_event.flags.cdcSendComplete == 0){
        event_data.eventid = USBEV_CDC_SENDCOMPLETE;
        event_data.intfNum = intfNum;
        event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));
    }
    
    return TRUE;   //sleep after interrupt (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a receive operation on interface intfNum has just been completed.
*/
uint8_t USBCDC_handleReceiveCompleted(uint8_t intfNum){
    EV_DATA_t event_data;
    
    if(usb_disable_event.flags.cdcRecvComplete == 0){
        event_data.eventid = USBEV_CDC_RECVCOMPLETE;
        event_data.intfNum = intfNum;
        event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));
    }

    return TRUE;   //sleep after interrupt (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
 * This event indicates that new line coding params have been received from the host
 */
BYTE USBCDC_handleSetLineCoding (BYTE intfNum, ULONG lBaudrate){
    //TO DO: You can place your code here

    return (FALSE); //return FALSE to go asleep after interrupt (in the case the CPU slept before interrupt)
}

/*
 * This event indicates that a SetControlLineState request was received from the host. 
 * Basically new RTS and DTR states have been sent. Bit 0 of lineState is DTR and Bit 1 is RTS.
 *
 */
BYTE USBCDC_handleSetControlLineState (BYTE intfNum, BYTE lineState){
    return (FALSE);
}

#endif // _CDC_

//--------------------------------------------------------------------------------------------------
#ifdef _HID_
/*
Indicates that data has been received for interface intfNum, but no receive operation is underway.
returns TRUE to keep CPU awake
*/
uint8_t USBHID_handleDataReceived(uint8_t intfNum){
    EV_DATA_t event_data;
    event_data.eventid = USBEV_HID_DATARECV;
    event_data.intfNum = intfNum;
    event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));

    return TRUE;   //sleep after interrupt (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a send operation on interface intfNum has just been completed.
returns TRUE to keep CPU awake
*/
uint8_t USBHID_handleSendCompleted(uint8_t intfNum){
    EV_DATA_t event_data;
    
    if(usb_disable_event.flags.hidSendComplete == 0){
        event_data.eventid = USBEV_HID_SENDCOMPLETE;
        event_data.intfNum = intfNum;
        event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));
    }
    
    return TRUE;   //sleep after interrupt (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
Indicates that a receive operation on interface intfNum has just been completed.
*/
uint8_t USBHID_handleReceiveCompleted(uint8_t intfNum){
    EV_DATA_t event_data;
    
    if(usb_disable_event.flags.hidRecvComplete == 0){
        event_data.eventid = USBEV_HID_RECVCOMPLETE;
        event_data.intfNum = intfNum;
        event_PushEvent(ev_USB_InterfaceEvent,&event_data,sizeof(EV_DATA_t));
    }
    
    return TRUE;   //sleep after interrupt (in the case the CPU slept before interrupt)
}

//--------------------------------------------------------------------------------------------------
/*
 * This event indicates that a Set_Protocol request was received from the host
 */
uint8_t USBHID_handleBootProtocol (uint8_t protocol, uint8_t intfnum){
    return (FALSE);
}

//--------------------------------------------------------------------------------------------------
/*
 * This event indicates that a Set_Report request was received from the host
 * The application needs to supply a buffer to retrieve the data that will be sent
 * as part of this request. This handler is passed the reportTypereportId(lower byte), the length of data
 * phase as well as the interface number.
 */
uint8_t *USBHID_handleSetReport(uint8_t reportType, uint8_t reportId, uint16_t requestedLength,
                                uint8_t intfnum){
	return (0);
}

//--------------------------------------------------------------------------------------------------
/*
 * This event indicates that data as part of Set_Report request was received from the host
 * Tha application can return TRUE to wake up the CPU. If the application supplied a buffer
 * as part of USBHID_handleSetReport, then this buffer will contain the Set Report data.
 */
uint8_t USBHID_handleSetReportDataAvailable (uint8_t intfnum){
	//Process received data based on currentReportType
    return (TRUE);
}

//--------------------------------------------------------------------------------------------------
/*
 * This event indicates that a Get_Report request was received from the host
 * The application can supply a buffer of data that will be sent to the host.
 * This handler is passed the reportType, reportId, the requested length as well
 * as the interface number.
 */
uint8_t *USBHID_handleGetReport(uint8_t reportType, uint8_t reportId, uint16_t requestedLength,
                                uint8_t intfnum){
	return (0);
}

#endif // _HID_

//--------------------------------------------------------------------------------------------------
#ifdef _MSC_
uint8_t USBMSC_handleBufferEvent(void){
    uint8_t eventid;
    eventid = USBEV_MSC_BUFFEREVENT;
    event_PushEvent(ev_USB_Event,&eventid,sizeof(uint8_t));
    return TRUE;    //sleep after interrupt (in the case the CPU slept before interrupt)
}
#endif // _MSC_

///\endcond

//==================================================================================================
// User HID Functions
//==================================================================================================

#ifdef _HID_
RES_t USB_hidSend(void* src, uint16_t size, uint8_t intfNum, uint32_t Timeout){
    uint32_t sendCounter = 0;
    uint16_t bytesSent, bytesReceived;
    uint8_t ret;
    uint16_t tmp;
    
    usb_disable_event.flags.hidSendComplete = 1;
    
    switch(USBHID_sendData(src,size,intfNum)) {
        case kUSBHID_sendStarted:
            break;
        case kUSBHID_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }

    // Operation successfully started.  Now wait til it's finished.
    while(1) {
        ret = USBHID_intfStatus(intfNum,&bytesSent,&bytesReceived);
        if(ret & kUSBHID_busNotAvailable){   /* This may happen at any time */
            return RES_FAIL;
        }
        if(ret & kUSBHID_waitingForSend){
            if(Timeout && (sendCounter++ >= Timeout)){  /* Incr counter & try again */
                USBHID_abortSend(&tmp, intfNum);
                return RES_FAIL; /* Timed out */
            }
        }else{
            usleep(1000);
            return RES_OK;    /* If neither busNotAvailable nor waitingForSend, it succeeded */
        }
    }
}

//--------------------------------------------------------------------------------------------------
RES_t USB_hidIsend(void* src, uint16_t size, uint8_t intfNum){
    usb_disable_event.flags.hidSendComplete = 0;
    switch(USBHID_sendData(src,size,intfNum)) {
        case kUSBHID_sendStarted:
            return RES_OK;
        case kUSBHID_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }
}                                  

//--------------------------------------------------------------------------------------------------
uint16_t USB_hidRecvAvailable(void *dst, uint16_t maxSize, uint8_t intfNum){
    uint16_t bytesInBuf,rxCount;
    uint8_t* currentPos = dst;
    
    usb_disable_event.flags.hidRecvComplete = 1;
    
    rxCount = 0;
    while((bytesInBuf = USBHID_bytesInUSBBuffer(intfNum))) {
        if((rxCount+bytesInBuf) > maxSize) {
            bytesInBuf = maxSize - rxCount;
            USBHID_receiveData(currentPos,bytesInBuf,intfNum);
            rxCount += bytesInBuf;
            break;
        }else{
            USBHID_receiveData(currentPos,bytesInBuf,intfNum); 
            currentPos += bytesInBuf;
            rxCount += bytesInBuf;
        }
    }
    return (rxCount);
}

//--------------------------------------------------------------------------------------------------
RES_t USB_hidRecv(void *dst, uint16_t size, uint8_t intfNum, uint32_t Timeout){
    uint32_t recvCounter = 0;
    uint16_t bytesSent, bytesReceived;
    uint8_t ret;
    uint16_t tmp;
    
    usb_disable_event.flags.hidRecvComplete = 1;
    
    switch(USBHID_receiveData(dst,size,intfNum)){
        case kUSBHID_receiveStarted:
            break;
        case kUSBHID_receiveCompleted:
            return RES_OK;
        case kUSBHID_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }

    // Operation successfully started.  Now wait til it's finished.
    while(1) {
        ret = USBHID_intfStatus(intfNum,&bytesSent,&bytesReceived);
        if(ret & kUSBHID_busNotAvailable){                /* This may happen at any time */
            return RES_FAIL;
        }
        if(ret & kUSBHID_waitingForReceive) {
            if(Timeout && (recvCounter++ >= Timeout)){  /* Incr counter & try again */
                USBHID_abortReceive(&tmp, intfNum);
                return RES_FAIL ;    /* Timed out */
            }
        }else{
            return RES_OK;    /* If neither busNotAvailable nor waitingForReceive, it succeeded */
        }
    }
}

//--------------------------------------------------------------------------------------------------
RES_t USB_hidIrecv(void *dst, uint16_t size, uint8_t intfNum){
    usb_disable_event.flags.hidRecvComplete = 0;
    switch(USBHID_receiveData(dst,size,intfNum)){
        case kUSBHID_receiveStarted:
        case kUSBHID_receiveCompleted:
            return RES_OK;
        case kUSBHID_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }
}

//--------------------------------------------------------------------------------------------------
uint8_t USB_hidStatus(uint8_t intfNum){
    uint16_t bytesSent, bytesReceived;
    return(USBHID_intfStatus(intfNum,&bytesSent,&bytesReceived));
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_hidIrecvCount(uint8_t intfNum){
    uint16_t bytesSent, bytesReceived;
    USBHID_intfStatus(intfNum,&bytesSent,&bytesReceived);
    return(bytesReceived);
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_hidAbortIsend(uint8_t intfNum){
    uint16_t size;
    USBHID_abortSend(&size, intfNum);
    return(size);
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_hidAbortIrecv(uint8_t intfNum){
    uint16_t size;
    USBHID_abortReceive(&size, intfNum);
    return(size);
}

//--------------------------------------------------------------------------------------------------
void USB_hidRejectData(uint8_t intfNum){
    USBHID_rejectData(intfNum);
}

//--------------------------------------------------------------------------------------------------
RES_t USB_hidSendReport(void *reportData, uint8_t intfNum){
    switch(USBHID_sendReport(reportData,intfNum)){
        case kUSBHID_busNotAvailable:
            return(RES_FAIL);
        case kUSBHID_intfBusyError:
            return(RES_BUSY);
        default:
            return(RES_OK);
    }
}
//--------------------------------------------------------------------------------------------------
RES_t USB_hidRecvReport(void *reportData, uint8_t intfNum){
    switch(USBHID_receiveReport(reportData,intfNum)){
        case kUSBHID_receiveCompleted:
            return(RES_OK);
        default:
            return(RES_FAIL);
    }
}


#endif

//==================================================================================================
// User CDC Functions
//==================================================================================================
#ifdef _CDC_

RES_t USB_cdcSend(void* src, uint16_t size, uint8_t intfNum, uint32_t Timeout){
    uint32_t sendCounter = 0;
    uint16_t bytesSent, bytesReceived;
    uint8_t ret;
    uint16_t tmp;
    
    usb_disable_event.flags.cdcSendComplete = 1;
    
    switch(USBCDC_sendData(src,size,intfNum)){
        case kUSBCDC_sendStarted:
            break;
        case kUSBCDC_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }

    // Operation successfully started.  Now wait til it's finished.
    while(1) {
        ret = USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived);
        if(ret & kUSBCDC_busNotAvailable){                /* This may happen at any time */
            return RES_FAIL;
        }
        if(ret & kUSBCDC_waitingForSend) {
            if(Timeout && (sendCounter++ >= Timeout)){  /* Incr counter & try again */
                USBCDC_abortSend(&tmp, intfNum);
                return RES_FAIL ;                                   /* Timed out */
            }
        }else{
            usleep(1000);
            return RES_OK;    /* If neither busNotAvailable nor waitingForSend, it succeeded */
        }
    }
}

//--------------------------------------------------------------------------------------------------
RES_t USB_cdcIsend(void* src, uint16_t size, uint8_t intfNum){
    usb_disable_event.flags.cdcSendComplete = 0;
    switch(USBCDC_sendData(src,size,intfNum)){
        case kUSBCDC_sendStarted:
            return RES_OK;
        case kUSBCDC_intfBusyError:
            return RES_BUSY;
        default:
            return RES_UNKNOWN;
    }
}                                  

//--------------------------------------------------------------------------------------------------
uint16_t USB_cdcRecvAvailable(void *dst, uint16_t maxSize, uint8_t intfNum){
    uint16_t bytesInBuf,rxCount;
    uint8_t* currentPos = dst;
    
    usb_disable_event.flags.cdcRecvComplete = 1;
    
    rxCount = 0;
    while((bytesInBuf = USBCDC_bytesInUSBBuffer(intfNum))) {
        if((rxCount+bytesInBuf) > maxSize) {
            bytesInBuf = maxSize - rxCount;
            USBCDC_receiveData(currentPos,bytesInBuf,intfNum);
            rxCount += bytesInBuf;
            break;
        }else{
            USBCDC_receiveData(currentPos,bytesInBuf,intfNum); 
            currentPos += bytesInBuf;
            rxCount += bytesInBuf;
        }
    }
    return (rxCount);
}

//--------------------------------------------------------------------------------------------------
RES_t USB_cdcRecv(void *dst, uint16_t size, uint8_t intfNum, uint32_t Timeout){
    uint32_t recvCounter = 0;
    uint16_t bytesSent, bytesReceived;
    uint8_t ret;
    uint16_t tmp;
    
    usb_disable_event.flags.cdcRecvComplete = 1;
    
    switch(USBCDC_receiveData(dst,size,intfNum)){
        case kUSBCDC_receiveStarted:
            break;
        case kUSBCDC_receiveCompleted:
            return RES_OK;
        case kUSBCDC_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }

    // Operation successfully started.  Now wait til it's finished.
    while(1) {
        ret = USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived);
        if(ret & kUSBCDC_busNotAvailable){                /* This may happen at any time */
            return RES_FAIL;
        }
        if(ret & kUSBCDC_waitingForReceive) {
            if(Timeout && (recvCounter++ >= Timeout)){  /* Incr counter & try again */
                USBCDC_abortReceive(&tmp, intfNum);
                return RES_FAIL ;                                   /* Timed out */
            }
        }else{
            return RES_OK;    /* If neither busNotAvailable nor waitingForReceive, it succeeded */
        }
    }
}

//--------------------------------------------------------------------------------------------------
RES_t USB_cdcIrecv(void *dst, uint16_t size, uint8_t intfNum){
    usb_disable_event.flags.cdcRecvComplete = 0;
    switch(USBCDC_receiveData(dst,size,intfNum)){
        case kUSBCDC_receiveStarted:
        case kUSBCDC_receiveCompleted:
            return RES_OK;
        case kUSBCDC_intfBusyError:
            return RES_BUSY;
        default:
            return RES_FAIL;
    }
}

//--------------------------------------------------------------------------------------------------
uint8_t USB_cdcStatus(uint8_t intfNum){
    uint16_t bytesSent, bytesReceived;
    return(USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived));
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_cdcIrecvCount(uint8_t intfNum){
    uint16_t bytesSent, bytesReceived;
    USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived);
    return(bytesReceived);
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_cdcAbortIsend(uint8_t intfNum){
    uint16_t size;
    USBCDC_abortSend(&size, intfNum);
    return(size);
}

//--------------------------------------------------------------------------------------------------
uint16_t USB_cdcAbortIrecv(uint8_t intfNum){
    uint16_t size;
    USBCDC_abortReceive(&size, intfNum);
    return(size);
}

//--------------------------------------------------------------------------------------------------
void USB_cdcRejectData(uint8_t intfNum){
    USBCDC_rejectData(intfNum);
}

#endif

///\}


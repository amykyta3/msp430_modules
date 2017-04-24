

#include <stdint.h>
#include "usb_api.h"

static uint16_t enabled_events;

uint8_t USB_init(void){
    return(0);
}

uint8_t USB_enable(void){
    return(0);
}

uint8_t USB_disable(void){
    return(0);
}

uint8_t USB_setEnabledEvents(uint16_t events){
    enabled_events = events;
    return(0);
}

uint16_t USB_getEnabledEvents(void){
    return(enabled_events);
}

uint8_t USB_reset(void){
    return(0);
}

uint8_t USB_connect(void){
    return(0);
}

uint8_t USB_disconnect(void){
    return(0);
}

uint8_t USB_connectionInfo(void){
    return(USB_VBUS_PRESENT);
}

uint8_t USB_connectionState(void){
    return(USBST_CONNECTED_NO_ENUM);
}


///\}


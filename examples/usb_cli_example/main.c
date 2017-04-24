
#include <stdint.h>
#include <msp430_xc.h>
#include <clock_sys.h>
#include <event_queue.h>
#include <usb_api.h>
#include <cli.h>

int main(void){
    WDTCTL = WDTPW + WDTHOLD; // Stop the Watchdog Timer
    __disable_interrupt(); // Disable Interrupts
    
    P5SEL |= 0x0C;  //enable XT2 pins for F5529
    
    clock_init();
    event_init();
    USB_init();
    
    // Generate an event every time data is recvd
    USB_setEnabledEvents(USBEV_DATARECV_EN | USBEV_VBUSON_EN | USBEV_VBUSOFF_EN);
    
    // See if we're already attached physically to USB, and if so, connect to it 
    if (USB_connectionInfo() & USB_VBUS_PRESENT){
        if (USB_enable() == 0){
            USB_reset();
            USB_connect();
        }
    }
    
    __enable_interrupt();
    event_StartHandler();
    
    return(0);
}

void onIdle(void){
    
}

void onUSB_Event(USB_EVENT_t event){
    switch(event){
        case USBEV_VBUSON:
            // User plugged the USB cable in
            if (USB_enable() == 0){
                USB_reset();
                USB_connect();
            }
            break;
        case USBEV_VBUSOFF:
            // User unplugged the USB cable
            USB_disconnect();
            USB_disable();
            break;
        default:
            break;
    }
}

void onUSB_InterfaceEvent(USB_EVENT_t event,uint8_t intfNum){
    if(event == USBEV_CDC_DATARECV){
        // pass any characters recvd into the CLI handler
        char strbuf[128];
        uint16_t n, i;
        
        n = USB_cdcRecvAvailable(strbuf,sizeof(strbuf),0);

        for(i=0;i<n;i++){
            cli_process_char(strbuf[i]);
        }
    }
}

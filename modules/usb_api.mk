
########################################### Module Setup ###########################################
MODULE_SOURCES += usb_api.c
MODULE_SOURCES += USB_API/USB_CDC_API/UsbCdc.c
MODULE_SOURCES += USB_API/USB_HID_API/UsbHid.c
MODULE_SOURCES += USB_API/USB_HID_API/UsbHidReq.c
MODULE_SOURCES += USB_API/USB_MSC_API/UsbMscReq.c
MODULE_SOURCES += USB_API/USB_MSC_API/UsbMscScsi.c
MODULE_SOURCES += USB_API/USB_MSC_API/UsbMscStateMachine.c
MODULE_SOURCES += USB_API/USB_PHDC_API/UsbPHDC.c
MODULE_SOURCES += USB_API/USB_Common/usbdma.c
MODULE_SOURCES += USB_API/USB_Common/usb.c

PROJECT_SOURCES += $(CONFIG_PATHTO)USB_config/descriptors.c
PROJECT_SOURCES += $(CONFIG_PATHTO)USB_config/UsbIsr.c

# Add linker script to place buffers
LDFLAGS+= -Wl,--just-symbols=$(MODULES_PATHTO)USB_API/msp430USB.ld

REQUIRED_MODULES += event_queue sleep driverlib

In your project's /config directory, create a folder called USB_config
Use TI's MSP430_USB_Descriptor_Tool to generate the following files:
	descriptors.c
	descriptors.h
	UsbIsr.c
	MSP430_CDC.inf  (for CDC interfaces)

Save the C and H files in the USB_config folder

The .inf file is used by the Windows driver setup process to identify your device

====================================================================================================
UsbIsr.c
====================================================================================================
Modify UsbIsr.c to use the isr_xc.h interrupt convention. This allows both TI and GCC compilers to
be used.
    from:
        #pragma vector=USB_UBM_VECTOR
        __interrupt VOID iUsbInterruptHandler(VOID)
        {
    to:
        ISR(USB_UBM_VECTOR){

====================================================================================================
descriptors.h
====================================================================================================
If compiling with MSPGCC, add the __attribute__((packed)) attribute to each struct declaration.
For example:
    struct struct_name{
        <struct body>
    }__attribute__((packed));

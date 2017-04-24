
########################################## Project Setup ###########################################
PROJECT_NAME:= usb_cli_example

MODULES_PATHTO:= ../../modules/
CONFIG_PATHTO:= config/

INCLUDE_PATHS:= ../../include/
PROJECT_SOURCES:= main.c
MODULES:=clock_sys event_queue fifo driverlib usb_api sleep cli

MSP430_DEVICE:= msp430f5529

ASFLAGS:=
CFLAGS:= -O2 -g -std=gnu99 -ffunction-sections -fdata-sections
CPPFLAGS:= -O2 -g -Wall
LDFLAGS:= -Wl,-gc-sections

####################################################################################################
all: executable
include $(MODULES_PATHTO)_make_project_mspgcc.mk
########################################## Custom Targets ##########################################

program: $(EXECUTABLE).hex
	MSP430Flasher -n $(MSP430_DEVICE) -w $^ -v -g -q -z [RESET, VCC]

.PHONY:clean
clean:
	rm -r -f $(BUILD_PATH)


########################################## Project Setup ###########################################
PROJECT_NAME:= uart_cli_example

MODULES_PATHTO:= ../../modules/
CONFIG_PATHTO:= config/

INCLUDE_PATHS:= ../../include/
PROJECT_SOURCES:= main.c
MODULES:=clock_sys fifo uart_io cli sleep

# uncomment below for USB Launchpad
MSP430_DEVICE:= msp430f5529

# uncomment below for Experimenter Board
#MSP430_DEVICE:= msp430f4618

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

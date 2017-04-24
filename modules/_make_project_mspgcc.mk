# This is designed to be used as a generic makefile for MSP430 projects using the TI/Red Hat
# MSPGCC-ELF compiler.
# 
# The following environment variables must be set:
#	PATH - Add the compiler bin folder to the PATH variable
#	MSP430_HEADER_PATH - points to the MSP430 header folder
#
# This file is to be included from a project-specific Makefile.
# The project's Makefile must prepare the following variables:
#	PROJECT_NAME
#	MODULES_PATHTO - Relative path from the project directory to the modules folder
#	CONFIG_PATHTO - Relative path to the project's config folder (if necessary)
#	INCLUDE_PATHS - List of any additional include paths
#	PROJECT_SOURCES - List of c, cpp, S files to be compiled
#	MODULES - List of modules to also compile
#	MSP430_DEVICE - Device name. Ex: msp430f5529
#	ASFLAGS, CFLAGS, CPPFLAGS, LDFLAGS - Additional project-specific compile flags
#
####################################################################################################

AS:= @msp430-elf-gcc
CC:= @msp430-elf-gcc
CXX:= @msp430-elf-gcc
LD:= @msp430-elf-gcc
OBJCOPY:= @msp430-elf-objcopy
OBJDUMP:= @msp430-elf-objdump

####################################################################################################
BUILD_PATH:=build/
MODULES_BUILD_PATH:= $(BUILD_PATH)modules/

########################################## Gather Modules ##########################################
include $(addprefix $(MODULES_PATHTO),$(addsuffix .mk,$(MODULES)))

MISSING_MODULES:= $(sort $(filter-out $(MODULES),$(REQUIRED_MODULES)))

ifneq ($(strip $(MISSING_MODULES)),)
  $(warning Module dependancies are missing! Add the following modules to your makefile: $(MISSING_MODULES))
endif

######################################### MSPGCC Compiler ##########################################

INCLUDE_FLAGS:= $(addprefix -I ,$(MODULES_PATHTO) $(CONFIG_PATHTO) $(INCLUDE_PATHS))
ASFLAGS+= $(INCLUDE_FLAGS) -mmcu=$(MSP430_DEVICE)
CFLAGS += $(INCLUDE_FLAGS) -mmcu=$(MSP430_DEVICE) -isystem $(MSP430_HEADER_PATH)
CPPFLAGS += $(INCLUDE_FLAGS) -mmcu=$(MSP430_DEVICE) -isystem $(MSP430_HEADER_PATH)
LDFLAGS+= -mmcu=$(MSP430_DEVICE) -L $(MSP430_HEADER_PATH) -T $(MSP430_HEADER_PATH)/$(MSP430_DEVICE).ld

EXECUTABLE:=$(BUILD_PATH)$(PROJECT_NAME).elf

OBJECTS:= $(addprefix $(BUILD_PATH),$(addsuffix .o,$(basename $(PROJECT_SOURCES))))
OBJECTS+= $(addprefix $(MODULES_BUILD_PATH),$(addsuffix .o,$(basename $(MODULE_SOURCES))))

DEPEND:= $(OBJECTS:.o=.d)

BUILD_DIRECTORIES:= $(addprefix $(BUILD_PATH),$(dir $(PROJECT_SOURCES)))
BUILD_DIRECTORIES+= $(addprefix $(MODULES_BUILD_PATH),$(dir $(MODULE_SOURCES)))
$(shell mkdir -p $(BUILD_DIRECTORIES))


# Assembler ----------------------------------------------------------------------------------------
$(BUILD_PATH)%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<
	
$(MODULES_BUILD_PATH)%.o: $(MODULES_PATHTO)%.S
	$(AS) $(ASFLAGS) -c -o $@ $<
	
# Generate Dependencies	----------------------------------------------------------------------------
$(BUILD_PATH)%.d: %.c
	$(CC) -MM -MT $(@:.d=.o) -MT $@ $(CFLAGS) $< -MF $@

$(MODULES_BUILD_PATH)%.d: $(MODULES_PATHTO)%.c
	$(CC) -MM -MT $(@:.d=.o) -MT $@ $(CFLAGS) $< -MF $@

$(BUILD_PATH)%.d: %.cpp
	$(CXX) -MM -MT $(@:.d=.o) -MT $@ $(CPPFLAGS) $< -MF $@

$(MODULES_BUILD_PATH)%.d: $(MODULES_PATHTO)%.cpp
	$(CXX) -MM -MT $(@:.d=.o) -MT $@ $(CPPFLAGS) $< -MF $@
	
# C Compiler ---------------------------------------------------------------------------------------
$(BUILD_PATH)%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(MODULES_BUILD_PATH)%.o: $(MODULES_PATHTO)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# C++ Compiler -------------------------------------------------------------------------------------
$(BUILD_PATH)%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<
	
$(MODULES_BUILD_PATH)%.o: $(MODULES_PATHTO)%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

# Linker -------------------------------------------------------------------------------------------
.PHONY:executable
executable: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

#Other outputs -------------------------------------------------------------------------------------

.PHONY:hex
hex: $(EXECUTABLE).hex

$(EXECUTABLE).hex: $(EXECUTABLE)
	$(OBJCOPY) -O ihex $< $@

dump: $(EXECUTABLE)
	$(OBJDUMP) -S $< > $<.dump.txt	

####################################################################################################

ifneq ($(MAKECMDGOALS), clean)
 -include $(DEPEND)
endif

####################################################################################################



MK := mkdir -p
RM := rm -rf

OUTPUT_FILENAME := main
TEMPLATE_PATH   := ~/SHIELD/embedded/shield/linker_scripts
LINKER_SCRIPT   := nrf51_xxaa.ld

# echo suspend
ifeq ("$(V)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

GNU_INSTALL_ROOT	:= /usr/local/gcc-arm-none-eabi-5_4-2016q3
GNU_VERSION		:= 5.4
GNU_PREFIX		:= arm-none-eabi


# Toolchain commands
CC              	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
CXX             	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-g++'
AS              	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            	:= '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

# Include paths for compiler
INC_PATHS  = -I./src
INC_PATHS += -I./src/arch 
INC_PATHS += -I./src/arch/nrf51
INC_PATHS += -I./src/asm
INC_PATHS += -I./src/ble
INC_PATHS += -I./src/ble/ble_error_log
INC_PATHS += -I./src/ble/common
INC_PATHS += -I./src/bsp
INC_PATHS += -I./src/bsp/pca10028
INC_PATHS += -I./src/common
INC_PATHS += -I./src/config
INC_PATHS += -I./src/device
INC_PATHS += -I./src/driver
INC_PATHS += -I./src/ds
INC_PATHS += -I./src/hal
INC_PATHS += -I./src/libraries
INC_PATHS += -I./src/libraries/app
INC_PATHS += -I./src/libraries/ble_flash
INC_PATHS += -I./src/libraries/delay
INC_PATHS += -I./src/libraries/experimental_section_vars
INC_PATHS += -I./src/libraries/hal
INC_PATHS += -I./src/libraries/timer
INC_PATHS += -I./src/libraries/trace
INC_PATHS += -I./src/libraries/util
INC_PATHS += -I./src/main
INC_PATHS += -I./src/softdevice
INC_PATHS += -I./src/softdevice/common
INC_PATHS += -I./src/softdevice/common/softdevice_handler
INC_PATHS += -I./src/softdevice/s110
INC_PATHS += -I./src/softdevice/s110/headers
INC_PATHS += -I./src/system
INC_PATHS += -I./src/toolchain
INC_PATHS += -I./src/utils

# C++ Source Files List
CXXSOURCES  = main.cpp _main.cpp
CXXSOURCES += delay.cpp gpio.cpp time.cpp uart.cpp

# C Source Files
CSOURCES   = nrf51_adc.c nrf51_clock.c nrf51_delay.c nrf51_gpio.c nrf51_lpcmp.c nrf51_power.c nrf51_rng.c nrf51_rtc.c nrf51_temp.c nrf51_timer.c
CSOURCES  += nrf51_uart.c nrf51_wdt.c
CSOURCES  += ble_advdata.c ble_srv_common.c
CSOURCES  += app_error.c nrf_drv_timer.c nrf_drv_common.c
CSOURCES  += nrf_delay.c app_trace.c app_util_platform.c nrf_assert.c nrf_log.c sdk_mapped_flags.c
CSOURCES  += conn_manager.c gatts_handler.c peer_manager.c
CSOURCES  += softdevice_handler.c softdevice_handler_appsh.c
CSOURCES  += system_nrf51.c
CSOURCES  += assert.c


# Assembly files
ASM_SOURCE = gcc_startup_nrf51.s

# Build Directory
OBJECT_DIRECTORY 		= _build
LISTING_DIRECTORY 		= $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_EXE_DIRECTORY	= _exe

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) $(OUTPUT_EXE_DIRECTORY))

# Macros to be defined across C and C++ files
DEFINE_MACROS  = -DBOARD_PCA10028
DEFINE_MACROS += -DSOFTDEVICE_PRESENT
DEFINE_MACROS += -DNRF51
DEFINE_MACROS += -DS110
DEFINE_MACROS += -DBLE_STACK_SUPPORT_REQD
DEFINE_MACROS += -DSWI_DISABLE0
# DEFINE_MACROS += -DENABLE_DEBUG_LOG_SUPPORT

# Common Compiler flags of CC and CXX compiler.
COMMON_FLAGS  = -mcpu=cortex-m0  -mabi=aapcs
COMMON_FLAGS += -O3 -g
COMMON_FLAGS += -mfloat-abi=soft
# Keep every function in separate section. This will allow linker to dump unused functions
COMMON_FLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
COMMON_FLAGS += -fno-builtin --short-enums

# CXX compiler flags
CXXFLAGS  = $(DEFINE_MACROS)
CXXFLAGS += $(COMMON_FLAGS) -mthumb
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit

# CC compiler flags
CFLAGS  = $(DEFINE_MACROS)
CFLAGS += $(COMMON_FLAGS) 
CFLAGS += --std=gnu99 -mthumb

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += $(DEFINE_MACROS)

# Linker Flags
# Keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# Let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# Use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

#default target - first one defined
default: main

# building all targets
all:
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf51422_xxac_s110

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	main
	@echo 	flash_softdevice


VPATH  = ./src/arch/nrf51
VPATH +=./src/asm
VPATH += ./src/ble:./src/ble/ble_error_log:./src/ble/common
VPATH += ./src/bsp:./src/bsp/pca10028
VPATH += ./src/common
VPATH += ./src/config
VPATH += ./src/device
VPATH += ./src/driver
VPATH += ./src/ds
VPATH += ./src/hal
VPATH += ./src/libraries:./src/libraries/delay:./src/libraries/experimental_section_vars:./src/libraries/hal:./src/libraries/timer:./src/libraries/trace:./src/libraries/util
VPATH += ./src/main
VPATH += ./src/softdevice
VPATH += ./src/softdevice/common/softdevice_handler
VPATH += ./src/system
VPATH += ./src/toolchain
VPATH += ./src/utils

OBJECTC    = $(CSOURCES:.c=.o)
OBJECTCXX  = $(CXXSOURCES:.cpp=.o)
OBJECTASM  = $(ASM_SOURCE:.s=.o)

OBJC   = $(patsubst %,$(OUTPUT_BINARY_DIRECTORY)/%,$(OBJECTC))
OBJCXX = $(patsubst %,$(OUTPUT_BINARY_DIRECTORY)/%,$(OBJECTCXX))
OBJASM = $(patsubst %,$(OUTPUT_BINARY_DIRECTORY)/%,$(OBJECTASM))
EXE    = $(patsubst %,$(EXE_DIRECTORY)/%,$(EXE_FILES))

OBJECTS = $(OBJC) $(OBJCXX) $(OBJASM)

main: $(OBJECTS) | $(BUILD_DIRECTORIES)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) finalize

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c | $(BUILD_DIRECTORIES)
	@echo Compiling c file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<


# Create objects from C++ SRC files
$(OBJECT_DIRECTORY)/%.o: %.cpp | $(BUILD_DIRECTORIES)
	@echo Compiling c++ file: $(notdir $<)
	$(NO_ECHO)$(CXX) $(CXXFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s | $(BUILD_DIRECTORIES)
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<


## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).hex

echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_EXE_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o


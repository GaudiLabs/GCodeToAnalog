#
#             LUFA Library
#     Copyright (C) Dean Camera, 2012.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.crg
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = LEONARDO
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = grbl
SRC          = main.c motion_control.c gcode.c serial.c \
                         protocol.c stepper.c serial.c protocol.c report.c \
                         eeprom.c settings.c planner.c nuts_bolts.c limits.c print.c config/Descriptors.c \
			 $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = deps/LUFA
#CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iconfig/ -lm -Wl,--gc-sections -ffunction-sections
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -Iconfig/ -lm -Wl,--gc-sections -ffunction-sections
LD_FLAGS     =

USB_PORT = /dev/ttyACM0
PROGRAMMER = -c avr109 -P $(USB_PORT)
#PROGRAMMER = -c stk500 -P /dev/ttyACM0
AVRDUDE = avrdude -p $(MCU) $(PROGRAMMER) -b57600 -D

RESET_CMD_START = python -c "from serial import Serial;from time import sleep;s=Serial ('
RESET_CMD_END = ', 115200);s.close();s.open();s.close();s.setBaudrate(1200);s.open();s.close()"
RESET_CMD = $(RESET_CMD_START)$(USB_PORT)$(RESET_CMD_END)

#If you already have .hex, seems to work better given reset required...
#can implement a software reset by connecting on 1200 baud, TODO


# Default target
all:

# Include LUFA build script makefiles
include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk
include $(LUFA_PATH)/Build/lufa_cppcheck.mk
include $(LUFA_PATH)/Build/lufa_doxygen.mk
include $(LUFA_PATH)/Build/lufa_dfu.mk
include $(LUFA_PATH)/Build/lufa_hid.mk
include $(LUFA_PATH)/Build/lufa_avrdude.mk
include $(LUFA_PATH)/Build/lufa_atprogram.mk

upload:		all	
		$(RESET_CMD)
		$(AVRDUDE) -U flash:w:grbl.hex:i 




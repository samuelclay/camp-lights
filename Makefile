# LED Dress
# Author: Samuel Clay
# Date: 2014-04-27
# Copyright: (c) 2014

DEVICE      = atmega328p
DEVICE_MCCU = atmega328p     # See http://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
PROGRAMMER ?= usbtiny
F_CPU       = 16000000
# Ext Clock
FUSE_L      = 0xFF
FUSE_H      = 0xD9
# Int Clock
# FUSE_L      = 0xE2
# FUSE_H      = 0xD9
FUSE_E      = 0x01

AVRDUDE_FUSE= avrdude -v -c $(PROGRAMMER) -p $(DEVICE) -P usb -F
AVRDUDE     = $(AVRDUDE_FUSE) -B1

LIBS        =  -I./libs/arduino
LIBS        += -I./libs/atmega328
LIBS        += -I./libs/avr
LIBS        += -I./libs/neopixel
LIBS        += -I./libs/si114
LIBS        += -I./libs/easing
LIBS        += -I./libs
LIBS        += -I.

CFLAGS      = $(LIBS) \
              -fno-exceptions -ffunction-sections -fdata-sections \
              -Wl,--gc-sections
CPPFLAGS    = -std=c++0x \
              -felide-constructors

C_SRC   := libs/arduino/wiring.o \
           libs/arduino/wiring_analog.o \
           libs/arduino/wiring_digital.o \
           libs/arduino/wiring_pulse.o \
           libs/arduino/wiring_shift.o \
           libs/arduino/hooks.o
CPP_SRC := libs/arduino/main.o \
           libs/arduino/HardwareSerial.o \
           libs/arduino/Print.o \
           libs/arduino/Tone.o \
           libs/arduino/WMath.o \
           libs/arduino/WString.o \
           libs/neopixel/Adafruit_NeoPixel.o \
           libs/si1143/si1143.o \
           libs/easing/EasingBase.o \
           libs/easing/QuadraticEase.o
SRC     := camplights.o

OBJECTS = $(C_SRC:.c=.o) $(CPP_SRC:.cpp=.o) $(SRC:.cpp=.o)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE_MCCU)

all: clean dress.hex fuse program

# symbolic targets:
help:
	@echo "Use one of the following:"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make hex ....... to build dress.hex"
	@echo "make clean ..... to delete objects and hex file"

hex: dress.hex

# Add fuse to program once you're in production
program: flash

# rule for programming fuse bits:
fuse:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE_FUSE) -U lfuse:w:$(FUSE_L):m -U hfuse:w:$(FUSE_H):m -U efuse:w:$(FUSE_E):m

# rule for uploading firmware:
flash: dress.hex
	$(AVRDUDE) -U flash:w:build/dress.hex:i

read:
	$(AVRDUDE_FUSE) -U lfuse:r:lfuse.txt:h -U hfuse:r:hfuse.txt:h -U efuse:r:efuse.txt:h -U lock:r:lock.txt:h 

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f build/* *.o **/*.o **/**/*.o *.d **/*.d **/**/*.d **/**/*.lst

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -g -c $< -o $@

.cpp.o:
	$(COMPILE) $(CPPFLAGS) -g -c $< -o $@

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

core:
	avr-ar rcs build/core.a $(C_SRC:.c=.o) $(CPP_SRC:.cpp=.o)

# file targets:
dress.elf: $(OBJECTS) core
	$(COMPILE) -o build/dress.elf $(SRC:.cpp=.o) build/core.a -L. -lm

dress.hex: dress.elf
	rm -f build/dress.hex build/dress.eep.hex
	avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 -O ihex build/dress.elf build/dress.hex
	avr-objcopy -O ihex -R .eeprom build/dress.elf build/dress.hex
	avr-size build/dress.hex

disasm: dress.elf
	avr-objdump -d build/dress.elf


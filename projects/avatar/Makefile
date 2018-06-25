# Based on the Makefile from "Make: AVR Programming" by Elliot Williams. Awesome reading.
# https://github.com/hexagon5un/AVR-Programming

NAME = AVR Playground
VERSION = 0.1

MCU   = atmega328p
F_CPU = 1000000UL  
BAUD  = 9600UL
LIBDIR = ../../lib
PROGRAMMER_TYPE = usbasp
PROGRAMMER_ARGS = 	
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

TARGET = $(lastword $(subst /, ,$(CURDIR)))

SOURCES=$(wildcard *.c $(LIBDIR)/*.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I. -I$(LIBDIR)
CFLAGS = -Os -g -std=gnu99 -Wall -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections 
LDFLAGS = -Wl,-Map,$(TARGET).map -Wl,--gc-sections 
TARGET_ARCH = -mmcu=$(MCU)

%.o: %.c $(HEADERS) Makefile
	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

## These targets don't have files named after them
.PHONY: clean flash

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj $(TARGET).map $(TARGET).o $(LIBDIR)/*.o

flash: $(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<


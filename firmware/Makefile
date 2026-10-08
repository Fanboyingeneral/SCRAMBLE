# SCRAMBLE firmware — ATmega32 master + slave
#
#   make                 build master.hex and slave.hex
#   make flash-master    flash master.hex (one chip in the programmer)
#   make flash-slave     flash slave.hex
#   make clean
#
# Override the programmer on the command line, e.g.
#   make flash-master PROGRAMMER=usbasp PORT=usb

MCU        ?= atmega32
F_CPU      ?= 1000000UL
PROGRAMMER ?= usbasp
PORT       ?= usb

CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size
CFLAGS  = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu99

all: master.hex slave.hex

master.elf: master/master_controller.c
	$(CC) $(CFLAGS) -o $@ $<
	$(SIZE) $@

slave.elf: slave/slave_controller.c
	$(CC) $(CFLAGS) -o $@ $<
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash-master: master.hex
	avrdude -c $(PROGRAMMER) -P $(PORT) -p m32 -U flash:w:$<:i

flash-slave: slave.hex
	avrdude -c $(PROGRAMMER) -P $(PORT) -p m32 -U flash:w:$<:i

clean:
	rm -f *.elf *.hex

.PHONY: all clean flash-master flash-slave

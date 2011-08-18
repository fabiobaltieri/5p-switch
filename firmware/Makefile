# Apps
CC	= avr-gcc
AS	= avr-as
LD	= avr-ld
OBJCOPY	= avr-objcopy
SIZE	= avr-size
GDB	= avr-gdb
AVRDUDE	= avrdude

# Options
CFLAGS	= -O2 -Wall -mmcu=atmega168
#CFLAGS	= -O2 -Wall -mmcu=atmega8
ASFLAGS	= 
LDFLAGS	= 

AVRDUDEFLAGS = -c usbasp -p m168
#AVRDUDEFLAGS = -c usbasp -p m8

targets	= main.elf main_text.hex main_eeprom.hex
objects	= main.o uart.o spi.o commands.o
deps	= Makefile defines.h

include Makefile.targets

# Apps
CC	= avr-gcc
AS	= avr-as
LD	= avr-ld
OBJCOPY	= avr-objcopy
SIZE	= avr-size
GDB	= avr-gdb
AVRDUDE	= avrdude

BOOTLOADER = bootloadHID
BOOTLOADER_FLAGS = -r main_text.hex

# Options
F_CPU = 12000000
MCU = atmega168

CFLAGS	= -Os -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Iusbdrv -I.
ASFLAGS	= -Os -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Iusbdrv -I.
LDFLAGS	= 

AVRDUDEFLAGS = -cusbasp -Pusb -p$(MCU)

LFUSE = 0xef
HFUSE = 0xdd

#LOCK = -U lock:w:0x2f:m

targets	= main.elf main_text.hex main_eeprom.hex
objects	= main.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o
deps	= Makefile

# Targets

all: $(targets)

main.elf: $(objects) $(deps)
	$(CC) $(CFLAGS) -o $@ $(objects) $(LDFLAGS)
	$(SIZE) $@

%_text.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%_text.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

hex: main_text.hex main_eeprom.hex

load: $(targets)
	if ! $(SIZE) -A main.elf | grep -q "eeprom" ; then \
	  $(AVRDUDE) $(AVRDUDEFLAGS) -Uflash:w:main_text.hex $(LOCK); \
	else \
	  $(AVRDUDE) $(AVRDUDEFLAGS) -Uflash:w:main_text.hex -Ueeprom:w:main_eeprom.hex $(LOCK); \
	fi

fuse:
	$(AVRDUDE) $(AVRDUDEFLAGS) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

boot: $(targets)
	$(BOOTLOADER) $(BOOTLOADER_FLAGS)

.PHONY: clean

clean:
	rm -f $(targets) $(objects) *~


MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT = /dev/ttyACM0

CC = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -fdata-sections -ffunction-sections
LDFLAGS = -Wl,-gc-sections -Wl,-relax -mmcu=$(MCU)

TARGET = main
SRC = \
	asi_string.c \
	mem.c \
	uart.c \
	log.c \
	ds3231.c \
	fs/ff.c \
	fs/ffsystem.c \
	fs/ffunicode.c \
	fs/diskio.c \
	console.c \
	mcu.c \
	spi.c \
	networking/w5100.c \
	networking/networking.c \
	main.c

all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(TARGET).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b 115200 -D -U flash:w:$(TARGET).hex:i

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean
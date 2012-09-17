MCU = atmega64

CFLAGS = -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -std=gnu99

PROGRAMMER = usbasp

SRCS = i2cmaster.c keypad.c clock.c lcd.c main.c
OBJS = $(SRCS:.c=.o)

TARGET = avr

all: flash

# External crystal/resonator high-freq, startup time 256CK + 64ms
fuse:
	sudo avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xde:m -U hfuse:w:0x99:m -U efuse:w:0xff:m

build:
	avr-gcc $(CFLAGS) -mmcu=$(MCU) -c $(SRCS)
	avr-gcc $(CFLAGS) -mmcu=$(MCU) -o $(TARGET).elf $(OBJS)
	avr-objcopy -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

flash: build
	sudo avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$(TARGET).hex	

clean:
	rm *.o
	rm *.elf
	rm *.hex


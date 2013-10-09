# Outputs debugging information over UART pin unless this is defined
#DEFINES += -DNDEBUG

# Which port and pin to run ADB through
DEFINES += -DADB_PORT=PORTF
DEFINES += -DADB_PIN=PINF
DEFINES += -DADB_DDR=DDRF
DEFINES += -DADB_DATA_BIT=0

MCU      = at90usb1286 # Teensy++ 2.0
#MCU      = atmega32u4  # Teensy 2.0

F_CPU    = 16000000
INCLUDES = -I.
SOURCES  = main.c adb.c usb_keyboard.c

all:
	@avr-gcc -std=gnu99 -W -Wall -Wundef \
		-mmcu=$(MCU) -DF_CPU=$(F_CPU)UL $(DEFINES) \
		-Wno-unused-function -Wall -Wstrict-prototypes -Werror=implicit-function-declaration \
		-Os \
		-Dinline='inline __attribute__((always_inline))' \
		-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
		-Wl,--relax -o main.elf \
		$(INCLUDES) \
		$(SOURCES)

	@# -R options get rid of everything except the code in flash
	@avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex
	@avr-objdump -h -S -z main.elf > main.lss

clean:
	rm main.hex
	rm main.elf
	rm main.lss

run: all
	teensy_loader_cli -mmcu=$(MCU) -w main.hex

.PHONY: all clean run

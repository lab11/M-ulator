BINS = blink.bin echo.bin echo_str.bin fib.bin

all: $(BINS) basic.bin tests

blink.bin: lib/led.o

echo.bin: lib/uart.o

echo_str.bin: lib/uart.o lib/printf.o

################################################################################
## DO NOT EDIT ANYTHING BELOW THIS LINE
################################################################################

LEVEL = .

# Basic is special, so we don't put it in BINS
basic.bin:	basic.s
	$(CROSS_COMPILE)-as $(ASFLAGS) basic.s -o basic.o
	@#Force off the default linking behavoir of whatever X-compiler is used
	@rm -f basic.memmap
	@touch basic.memmap
	$(CROSS_COMPILE)-ld -Tbasic.memmap $(LDFLAGS) basic.o -o basic.elf
	@rm -f basic.memmap
	$(CROSS_COMPILE)-objcopy -O binary basic.elf basic.bin

clean: lib/clean tests/clean
	rm -f *.o
	rm -f *.bin
	rm -f *.elf

clean-all: clean lib/clean-all tests/clean-all

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common.target

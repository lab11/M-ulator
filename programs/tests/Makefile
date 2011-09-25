# Some quick configuration...
LEVEL = ..
LIB_LED = $(LEVEL)/lib/led.o
LIB_UART = $(LEVEL)/lib/uart.o
LIB_PRINTF = $(LEVEL)/lib/printf.o

all:

# Follow the examples to add your testbench to the build suite
# Assembly or C testbench:
BINS += trivialS.bin
BINS += trivialC.bin

# Testbench that uses peripherals / libraries:
BINS += trivialPrintf.bin
trivialPrintf.bin: $(LIB_UART) $(LIB_PRINTF)

############################################
# \/ Add Your Testbenches below this line \/



# /\ Add Your Testbenches above this line /\
############################################

################################################################################
## DO NOT EDIT ANYTHING BELOW THIS LINE
################################################################################

all: $(BINS)

CFLAGS += -I$(LEVEL)

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.bin

clean-all: clean

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common

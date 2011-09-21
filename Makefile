CFLAGS += -g -Wall -Werror -pthread -m32 -D_GNU_SOURCE
LDFLAGS += -g -pthread -m32
#CC=clang

SIMFLAGS += --showledtoggles

ifeq ($(debug), 1)
	CFLAGS += -DDEBUG1
	SIMFLAGS += --printcycles
endif

ifeq ($(debug), 2)
	CFLAGS += -DDEBUG1 -DDEBUG2
	SIMFLAGS += --printcycles --dumpatcycle 1
endif

OP_CFILES = $(wildcard operations/*.c)
OP_OBJS   = $(patsubst operations/%.c,operations/%.o,$(OP_CFILES))

operations/%.o:	operations/%.c operations/opcodes.h cortex_m3.h

simulator:	$(OP_OBJS) simulator.o core.o misc.o cortex_m3.h

all:	simulator

README.pdf:	README.tex
	pdflatex README.tex
	pdflatex README.tex

doc:	README.pdf

basic:	programs/basic.bin simulator
	./simulator --flash programs/basic.bin --dumpallcycles $(SIMFLAGS)

blink:	programs/blink.bin simulator
	./simulator --flash programs/blink.bin $(SIMFLAGS)

echo:	programs/echo.bin simulator
	echo e | nc -4 localhost 4100 > /tmp/echo_out &
	./simulator --flash programs/echo.bin $(SIMFLAGS)

echo_str:	programs/echo_str.bin simulator
	./simulator --flash programs/echo_str.bin $(SIMFLAGS)

testflash:	simulator
	./simulator --usetestflash $(SIMFLAGS)

.PHONY: all clean doc blink echo echo_str testflash

clean:
	rm -f *.o operations/*.o simulator
	rm -f flash.mem
	rm -f README.log README.aux README.out

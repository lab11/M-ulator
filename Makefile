CFLAGS += -g3 -Wall -Wextra -Werror -Wno-type-limits -pthread -m32 -D_GNU_SOURCE -fno-strict-overflow
LDFLAGS += -g3 -pthread -m32

SIMFLAGS += --showledwrites

################################################################################
## DO NOT TOUCH ANYTHING BELOW THIS LINE
################################################################################

#################
# Default Targets

all:	simulator programs


###########
# Simulator

$(warning)

ifeq ($(debug), 1)
	CFLAGS += -DDEBUG1
	SIMFLAGS += --printcycles
endif

# WTF? w/out empty warning make isn't picking up debug??
$(warning)

ifeq ($(debug), 2)
	CFLAGS += -DDEBUG1 -DDEBUG2
	SIMFLAGS += --printcycles --dumpatcycle 1
endif

OP_CFILES = $(wildcard operations/*.c)
OP_OBJS   = $(patsubst operations/%.c,operations/%.o,$(OP_CFILES))

operations/%.o:	operations/%.c operations/opcodes.h cortex_m3.h

simulator:	$(OP_OBJS) simulator.o core.o misc.o cortex_m3.h

clean-simulator:
	rm -f *.o operations/*.o simulator

clean-simulator-all: clean-simulator
	rm -f flash.mem

.PHONY: all clean-simulator clean-simulator-all

##########
# Programs

programs:
	$(MAKE) -C programs

programs/%:
	$(MAKE) $* -C programs

.PHONY: programs

###############
# Documentation

README.pdf:	README.tex
	pdflatex README.tex
	pdflatex README.tex

doc:	README.pdf

clean-doc:
	rm -f README.log README.aux README.out

clean-doc-all: clean-doc
	rm -f README.pdf

.PHONY: doc clean-doc clean-doc-all

#########
# Testing

tester = +$(MAKE) $(2).bin -C $(1);\
	 ./simulator --flash $(1)/$(2).bin $(SIMFLAGS)

SIM_EXE = ./simulator --flash $< $(SIMFLAGS)

basic:	programs/basic.bin simulator
	$(eval SIMFLAGS += --dumpallcycles)
	$(call tester,programs,$@)

blink:	programs/blink.bin simulator
	$(call tester,programs,$@)

echo:	programs/echo.bin simulator
	echo e | nc -4 localhost 4100 > /tmp/echo_out &
	$(call tester,programs,$@)

echo_str:	programs/echo_str.bin simulator
	$(call tester,programs,$@)

testflash:	simulator
	$(call tester,programs,$@)

.PHONY: blink echo echo_str testflash

trivialS: simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,programs/tests,$@)

trivialC: simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,programs/tests,$@)

trivialPrintf: simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,programs/tests,$@)

.PHONY: trivialS trivialC trivialPrintf

#####
# Etc

clean:	clean-doc clean-simulator programs/clean

clean-all: clean-doc-all clean-simulator-all programs/clean-all

.PHONY: clean clean-all

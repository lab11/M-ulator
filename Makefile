SIMFLAGS += --showledwrites

################################################################################
## DO NOT TOUCH ANYTHING BELOW THIS LINE
################################################################################

LEVEL = .

#################
# Default Targets

all:	simulator programs

###########
# Debugging

$(warning)

ifeq ($(debug), 1)
CFLAGS += -DDEBUG1
SIMFLAGS += --printcycles
endif

# WTF? w/out empty warning make isn't picking up debug??
$(warning)

ifeq ($(debug), 2)
CFLAGS += -DDEBUG1 -DDEBUG2
SIMFLAGS += --printcycles --dumpatcycle 0
endif

###########
# Simulator

CPU_OBJS += $(patsubst %.c,%.o,$(wildcard cpu/*.c))
CPU_OBJS += $(patsubst %.c,%.o,$(wildcard cpu/operations/*.c))

#simulator:	$(CPU_OBJS) simulator.o
simulator:	cpu simulator.o
	$(CC) $(LDFLAGS) $(CPU_OBJS) simulator.o -o $@

clean-simulator: cpu/clean
	rm -f simulator
	rm -f simulator.o

clean-simulator-all: clean-simulator cpu/clean-all
	rm -f flash.mem

.PHONY: all clean-simulator clean-simulator-all

#####
# CPU

cpu:
	$(MAKE) -C cpu

cpu/%:
	$(MAKE) $* -C cpu

.PHONY: cpu

##########
# Programs

programs:
	$(MAKE) -C programs

programs/%:
	$(MAKE) $* -C programs

.PHONY: programs

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

clean:	clean-simulator programs/clean

clean-all: clean-simulator-all doc/clean-all programs/clean-all

.PHONY: clean clean-all

include $(LEVEL)/Makefile.common.host

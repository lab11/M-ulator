LEVEL = .
include $(LEVEL)/Makefile.common.host

#################
# Default Targets

all:	simulator

###########
# Simulator

CLI_OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))

CPU_OBJS += $(patsubst %.c,%.o,$(wildcard cpu/*.c))
CPU_OBJS += $(patsubst %.c,%.o,$(wildcard cpu/common/*.c))
CPU_OBJS += $(patsubst %.c,%.o,$(wildcard cpu/$(PLATFORM)/*.c))
CPU_OBJS += cpu/common/private_peripheral_bus/ppb.o

CORE_OBJS += $(patsubst %.c,%.o,$(wildcard core/*.c))
CORE_OBJS += $(patsubst %.c,%.o,$(wildcard core/operations/*.c))

simulator:	cpu cli core
	$(CC) $(LDFLAGS) $(CLI_OBJS) $(CPU_OBJS) $(CORE_OBJS) -o $@

clean: cpu/clean cli/clean core/clean
	rm -f simulator

clean-simulator-all: clean cpu/clean-all cli/clean-all core/clean-all

.PHONY: all clean clean-all

################################################################################

#####
# CLI

cli:
	$(MAKE) -C cli

cli/%:
	$(MAKE) $* -C cli

.PHONY: cli

#####
# CPU

cpu:
	$(MAKE) -C cpu

cpu/%:
	$(MAKE) $* -C cpu

.PHONY: cpu

######
# CORE

core:
	$(MAKE) -C core

core/%:
	$(MAKE) $* -C core

.PHONY: core

##########
# Programs

../programs:
	$(MAKE) -C ../programs

../programs/%:
	$(MAKE) $* -C ../programs

.PHONY: ../programs

#########
# Testing

ifeq ($(debug), 2)
tester = \
	 gdb --args ./simulator --flash $(1)/$(2).bin $(SIMFLAGS)
else
tester = \
	 ./simulator --flash $(1)/$(2).bin $(SIMFLAGS)
endif

SIM_EXE = ./simulator --flash $< $(SIMFLAGS)

basic:	../programs/basic.bin simulator
	$(eval SIMFLAGS += -p --dumpallcycles)
	$(call tester,../programs,$@)

blink:	../programs/blink.bin simulator
	$(call tester,../programs,$@)

echo:	../programs/echo.bin simulator
	sh -c 'sleep 1s; echo -e "e\n" | nc -4 localhost 4100 > /dev/null' &
	$(call tester,../programs,$@)

echo_str:	../programs/echo_str.bin simulator
	$(call tester,../programs,$@)

fib:	../programs/fib.bin simulator
	$(call tester,../programs,$@)

testflash:	simulator
	$(call tester,../programs,$@)

.PHONY: blink echo echo_str testflash

trivialS: ../programs/tests/trivialS.bin simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,../programs/tests,$@)

trivialC: ../programs/tests/trivialC.bin simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,../programs/tests,$@)

trivialPrintf: ../programs/tests/trivialPrintf.bin simulator
	$(eval SIMFLAGS += --returnr0)
	$(call tester,../programs/tests,$@)

.PHONY: trivialS trivialC trivialPrintf

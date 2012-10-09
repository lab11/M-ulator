################################################################################
## DO NOT TOUCH ANYTHING BELOW THIS LINE
################################################################################

LEVEL = ../..

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all:	$(OBJS) private_peripheral_bus

CFLAGS += -I$(LEVEL)/cpu/$(PLATFORM)/

###########
# REGISTERS

private_peripheral_bus:
	$(MAKE) -C private_peripheral_bus

private_peripheral_bus/%:
	$(MAKE) $* -C private_peripheral_bus

.PHONY: private_peripheral_bus

##

clean: private_peripheral_bus/clean
	rm -f *.o

clean-all: clean private_peripheral_bus/clean-all

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common.host

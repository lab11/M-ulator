################################################################################
## DO NOT TOUCH ANYTHING BELOW THIS LINE
################################################################################

LEVEL = ..

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all:	$(OBJS) operations

core.c:	private_peripheral_bus

###########
# REGISTERS

private_peripheral_bus:
	$(MAKE) -C private_peripheral_bus

private_peripheral_bus/%:
	$(MAKE) $* -C private_peripheral_bus

.PHONY: private_peripheral_bus

############
# OPERATIONS

operations:
	$(MAKE) -C operations

operations/%:
	$(MAKE) $* -C operations

.PHONY: operations

##

clean: operations/clean private_peripheral_bus/clean
	rm -f *.o

clean-all: clean operations/clean-all private_peripheral_bus/clean-all

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common.host

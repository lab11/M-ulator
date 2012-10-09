LEVEL = ..

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all:	$(OBJS) operations

############
# OPERATIONS

operations:
	$(MAKE) -C operations

operations/%:
	$(MAKE) $* -C operations

.PHONY: operations

#####
# CPU

cpu:
	$(MAKE) -C cpu

cpu/%:
	$(MAKE) $* -C cpu

.PHONY: cpu

##

clean: operations/clean
	rm -f *.o

clean-all: clean operations/clean-all

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common.host

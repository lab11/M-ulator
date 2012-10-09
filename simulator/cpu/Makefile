LEVEL = ..
include $(LEVEL)/Makefile.common.host

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all:	$(OBJS) common $(PLATFORM)

core.o: common $(PLATFORM)

##########
# COMMON

common:
	$(MAKE) -C common

common/%:
	$(MAKE) $* -C common

.PHONY: common

##########
# PLATFORM

$(PLATFORM):
	$(MAKE) -C $(PLATFORM)

$(PLATFORM)/%:
	$(MAKE) $* -C $(PLATFORM)

.PHONY: $(PLATFORM)

##

clean: common/clean $(PLATFORM)/clean
	rm -f *.o

clean-all: clean common/clean-all $(PLATFORM)/clean-all

.PHONY: all clean clean-all

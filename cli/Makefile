################################################################################
## DO NOT TOUCH ANYTHING BELOW THIS LINE
################################################################################

LEVEL = ..

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all:	$(OBJS)

clean:
	rm -f *.o

clean-all: clean

.PHONY: all clean clean-all

include $(LEVEL)/Makefile.common.host

LEVEL = ..

AS_SOURCES = $(wildcard *.s)
C_SOURCES  = $(wildcard *.c)

all: $(subst .s,.o,$(AS_SOURCES)) $(subst .c,.o,$(C_SOURCES))

clean:
	rm -f *.o

clean-all: clean

.PHONY: clean clean-all

include $(LEVEL)/Makefile.common

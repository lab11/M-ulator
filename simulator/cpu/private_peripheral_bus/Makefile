LEVEL=../..

all:	ppb.o

ppb.c:	gen_registers.py $(wildcard *.conf)
	python gen_registers.py $(wildcard *.conf)

clean:
	rm -f ppb.o
	rm -f ppb.c
	rm -f ppb.h

clean-all: clean

include $(LEVEL)/Makefile.common.host

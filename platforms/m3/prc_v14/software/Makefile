################################################################################
## You should not be required to edit this makefile. It attempts to divine your
## ARM toolchain, if it fails, you may explicitly define the CROSS_COMPILE
## variable. If you wish to add extras {AS,C}FLAGS, please do so here
################################################################################

CFLAGS :=
ASFLAGS :=

##########################
## Unlikely configruation:

#CFLAGS  += -Wall -Wextra -Werror -mthumb -mcpu=cortex-m0 -ffreestanding
# Don't care about the warnings not compiling. It's annoying.
CFLAGS  += -Os -Wall -Wextra -mthumb -mcpu=cortex-m0 -ffreestanding -fdata-sections -ffunction-sections -g
LDFLAGS += -Os -static -S -nostartfiles --gc-sections -e _start #-dead_strip #-why_live read_mbus_register_RADv4
#LDFLAGS += -Os -static -S -nostartfiles -nodefaultlibs -nostdlib --gc-sections -e _start #-dead_strip #-why_live read_mbus_register_RADv4
ASFLAGS += -mthumb --warn -mcpu=cortex-m0

# Experiments in not linking unused sections
#CFLAGS  += -fvtable-gc -fdata-sections -ffunction-sections -Wl,-static -Wl,--gc-sections -s
#LDFLAGS += -nostartfiles -nodefaultlibs -nostdlib

# Attempt to divine ARM toolchain
SHELL=/bin/bash
CROSS_COMPILE=$(shell if `hash arm-linux-gnueabi-gcc >& /dev/null`; then echo arm-linux-gnueabi; elif `hash arm-linux-gnu-gcc >& /dev/null`; then echo arm-linux-gnu; elif `hash arm-none-linux-gnueabi-gcc >& /dev/null`; then echo arm-none-linux-gnueabi; elif `hash arm-none-eabi-gcc >& /dev/null`; then echo arm-none-eabi; else echo -n ""; fi)

ifeq (,$(CROSS_COMPILE))
SPACE_HACK=
  $(info No cross compiler found. Have you installed an ARM toolchain?)
  $(info )
  $(info  For debian based systems:)
  $(info  $(SPACE_HACK)   sudo apt-get install gcc-arm-linux-gnueabi)
  $(info  For rpm based systems:)
  $(info  $(SPACE_HACK)   sudo yum install gcc-arm-linux-gnu)
  $(info  For Mac OS X (assuming port (macports.org)))
  $(info  $(SPACE_HACK)   sudo port install arm-none-eabi-gcc)
  $(info  For more details / other systems, see the ``Installation'' section of)
  $(info  $(SPACE_HACK)   http://eecs.umich.edu/~prabal/teaching/eecs373/notes/notes-toolchain.pdf)
  $(info )
  $(error A cross compiler is required)
endif

#############################
## Unlikely-er configruation:

# If you use a cross compiler with a different naming scheme (e.g. clang)
# you'll need to update these as well:
AS      := $(CROSS_COMPILE)-as
CC      := $(CROSS_COMPILE)-gcc
CXX     := $(CROSS_COMPILE)-g++
LD      := $(CROSS_COMPILE)-ld
OBJDUMP := $(CROSS_COMPILE)-objdump
OBJCOPY := $(CROSS_COMPILE)-objcopy
NM		:= $(CROSS_COMPILE)-nm

################################################################################
## You should not need to edit this makefile beyond this point
################################################################################

########
## Build

ASFLAGS += -Iinclude/
CFLAGS += -Iinclude/

LIBS := $(subst .c,.o,$(wildcard libs/*.c))
LIBS += $(subst .s,.o,$(wildcard libs/*.s))
HEADERS := $(wildcard include/*.h)

GDB:= $(subst .c,.o,$(wildcard gdb/*.c))
GDB+= $(subst .s,.o,$(wildcard gdb/*.s))

DIRS := $(patsubst %/,%,\
			$(filter-out libs/,\
				$(filter-out include/,\
					$(filter-out gdb/,\
						$(wildcard */)))))
BINS := $(foreach var,$(DIRS),$(var)/$(var).bin)
HEXS := $(foreach var,$(DIRS),$(var)/$(var).hex)
ELFS := $(foreach var,$(DIRS),$(var)/$(var).elf)

.PRECIOUS: $(LIBS) $(BINS) $(ELFS) $(GDB)

.PHONY: all

all: $(GDB) $(HEXS)

$(DIRS):
	$(MAKE) $@/$@.hex

.PHONY: $(DIRS)

.SECONDEXPANSION:

#%.bin:	$$(wildcard $$(dir $$*)*.c)

%.o: $(HEADERS) 

%.hex:	%.bin
	xxd -g1 -c1 $< | cut -d' ' -f2 > $@
	./createMemMap $@
	./createBootHexFLX $*.memmap

%.bin:	%.elf
	$(OBJCOPY) -O binary $< $@

# Mind the order here to align -T$^
# This will link in GDB support if you #include "gdb.h" in any of the c/s files
%.elf: COBJ=$(patsubst %.c,%.o,$(wildcard $(dir $@)*.c))
%.elf: SOBJ=$(patsubst %.s,%.o,$(wildcard $(dir $@)*.s))
%.elf:	libs/memmap $(LIBS) $$(COBJ) $$(SOBJ) 
	@if [ -n "$$(echo $$( $(NM) $(filter-out $<,$^) | grep use_gdb))" ]; then  \
		$(LD) $(LDFLAGS) -T$^ $(GDB) \
			"$$(echo $$( $(CC) -print-libgcc-file-name) | tr -d '\r')" -o $@ ;\
	else \
		$(LD) $(LDFLAGS) -T$^ \
			"$$(echo $$( $(CC) -print-libgcc-file-name) | tr -d '\r')" -o $@ ;\
	fi				
	$(OBJDUMP) -Sd $@ > $*.asm

.PRECIOUS: %.elf %.o


##########
## Cleanup

clean:
	rm -f $(BINS)
	rm -f $(HEXS)
	rm -f $(LIBS)
	rm -f $(ELFS)
	rm -f $(GDB)

clean-%:
	rm -f $*/$*.bin $*/$*.hex

.PHONY: clean clean-%


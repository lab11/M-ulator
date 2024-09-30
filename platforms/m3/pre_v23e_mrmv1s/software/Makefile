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
LDFLAGS += -Os -static -nostartfiles --gc-sections -e _start #-dead_strip #-why_live read_mbus_register_RADv4
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
SIZE	:= $(CROSS_COMPILE)-size

################################################################################
## You should not need to edit this makefile beyond this point
################################################################################

########
## Build

ASFLAGS += -Iinclude/
CFLAGS += -Iinclude/
OBJDUMP_FLAGS += --disassemble-all --source --disassembler-options=force-thumb -C --section-headers

LIBS := $(subst .c,.o,$(wildcard libs/*.c))
LIBS += $(subst .s,.o,$(wildcard libs/*.s))
GDB	 := $(subst .c,.o,$(wildcard gdb/*.c))
GDB	 += $(subst .s,.o,$(wildcard gdb/*.s))
HEADERS := $(wildcard include/*.h)

DIRS := $(patsubst %/,%,\
			$(filter-out libs/,\
				$(filter-out include/,\
					$(filter-out gdb/,\
						$(wildcard */)))))
BINS := $(foreach var,$(DIRS),$(var)/$(var).bin)
HEXS := $(foreach var,$(DIRS),$(var)/$(var).hex)
ELFS := $(foreach var,$(DIRS),$(var)/$(var).elf)
LSTS := $(foreach var,$(DIRS),$(var)/$(var).lst)
ASMS := $(foreach var,$(DIRS),$(var)/$(var).asm)
OBJS := $(foreach var,$(DIRS),$(var)/$(var).o)
BOOT := $(foreach var,$(DIRS),$(var)/$(var).boot*)
CRCS := $(foreach var,$(DIRS),$(var)/$(var).crc)
MEMS := $(foreach var,$(DIRS),$(var)/$(var).memmap)

FW_VER = $(shell pwd | sed 's/^.*\/xt1_firmware_\(.*\)$$/\1/')

.PRECIOUS: $(LIBS) $(BINS) $(ELFS) $(GDB)

.PHONY: all

all: $(GDB) $(HEXS) $(LSTS)

others:
	## Make '_nodevel' version
	#mkdir -p xt1_firmware_$(FW_VER)_nodevel
	#cp -f xt1_firmware_$(FW_VER)_devel/xt1_firmware_$(FW_VER)_devel.c xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c
	#sed -i "s/^#define DEVEL/\/\/#define DEVEL/" xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c
	#sed -i "s/^\/\/#define ENABLE_GOC_CRC/#define ENABLE_GOC_CRC/" xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c
	#sed -i "s/^\/\/#define ENABLE_RAW/#define ENABLE_RAW/" xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c
	#sed -i "s/(devel)/(no_devel)/" xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c
	## Make '_std' version
	#mkdir -p xt1_firmware_$(FW_VER)_std
	#cp -f xt1_firmware_$(FW_VER)_nodevel/xt1_firmware_$(FW_VER)_nodevel.c xt1_firmware_$(FW_VER)_std/xt1_firmware_$(FW_VER)_std.c
	#sed -i "s/(no_devel)/(standard)/" xt1_firmware_$(FW_VER)_std/xt1_firmware_$(FW_VER)_std.c
	#sed -i "s/^#define WAKEUP_BY_NFC\( *\)WAKEUP_BY_GPIO0/#define WAKEUP_BY_NFC\1WAKEUP_BY_GPIO1/" xt1_firmware_$(FW_VER)_std/xt1_firmware_$(FW_VER)_std.c
	## Make '_std_short' version
	#mkdir -p xt1_firmware_$(FW_VER)_std_short
	#cp -f xt1_firmware_$(FW_VER)_std/xt1_firmware_$(FW_VER)_std.c xt1_firmware_$(FW_VER)_std_short/xt1_firmware_$(FW_VER)_std_short.c
	#sed -i "s/^\/\/#define USE_SHORT_REFRESH/#define USE_SHORT_REFRESH/" xt1_firmware_$(FW_VER)_std_short/xt1_firmware_$(FW_VER)_std_short.c
	#sed -i "s/(standard)/(standard-short_interval)/" xt1_firmware_$(FW_VER)_std_short/xt1_firmware_$(FW_VER)_std_short.c
	## Make '_30s' version
	#mkdir -p xt1_firmware_$(FW_VER)_std_30s
	#cp -f xt1_firmware_$(FW_VER)_std/xt1_firmware_$(FW_VER)_std.c xt1_firmware_$(FW_VER)_std_30s/xt1_firmware_$(FW_VER)_std_30s.c
	#sed -i "s/^#define WAKEUP_INTERVAL_IN_MINUTES/\/\/#define WAKEUP_INTERVAL_IN_MINUTES/" xt1_firmware_$(FW_VER)_std_30s/xt1_firmware_$(FW_VER)_std_30s.c
	#sed -i "s/(standard)/(standard-30s)/" xt1_firmware_$(FW_VER)_std_30s/xt1_firmware_$(FW_VER)_std_30s.c

$(DIRS):
	$(MAKE) $@/$@.hex

.PHONY: $(DIRS)

.SECONDEXPANSION:

#%.bin:	$$(wildcard $$(dir $$*)*.c)

%.o: $(HEADERS) $(GDB)

%.hex:	%.bin
	xxd -g1 -c1 $< | cut -d' ' -f2 > $@
	./createMemMap $@
	./createBootHexFLX $*.memmap

# Generate debug listings as well
%.lst:	%.elf
	$(OBJDUMP) $(OBJDUMP_FLAGS) $< > $@

%.bin:	%.elf
	$(OBJCOPY) -O binary $< $@

# This will link together the object files into a single ELF
# It will link in GDB support if any of the source files use '#include "gdb.h"'
# Mind the order here to align -T$^
%.elf: COBJ=$(patsubst %.c,%.o,$(wildcard $(dir $@)*.c))
%.elf: SOBJ=$(patsubst %.s,%.o,$(wildcard $(dir $@)*.s))
%.elf:	libs/memmap $(LIBS) $$(COBJ) $$(SOBJ)
	if [ -n "$$(echo $$( $(NM) $(filter-out $<,$^) | grep _use_gdb))" ];\
	then  \
		$(LD) $(LDFLAGS) -T$^ $(GDB) \
			"$$(echo $$( $(CC) -print-libgcc-file-name) | tr -d '\r')" \
			-o $@ ;\
 	else \
		$(LD) $(LDFLAGS) -T$^ \
			"$$(echo $$( $(CC) -print-libgcc-file-name) | tr -d '\r')" \
			-o $@ ;\
	fi
	$(OBJDUMP) -Sd $@ > $*.asm
	$(SIZE) $*.elf

.PRECIOUS: %.elf %.o


##########
## Cleanup

clean:
	rm -f $(BINS)
	rm -f $(HEXS)
	rm -f $(LIBS)
	rm -f $(ELFS)
	rm -f $(ASMS)
	rm -f $(LSTS)
	rm -f $(OBJS)
	rm -f $(GDB)
	rm -f $(BOOT)
	rm -f $(CRCS)
	rm -f $(MEMS)

clean_libs:
	rm -f $(LIBS)

clean-%:
	rm -f $*/$*.bin $*/$*.hex

nuke: clean
	#rm -rf xt1_firmware_v*_nodevel
	#rm -rf xt1_firmware_v*_std*

cp: 
	scp \
        ./*/*.hex \
        yejoong@thurston.cubeworks.io:~/Dropbox/Designs/202401_MRAM/Testing/MRMv1S/hex/.

#--------------------------------------------------------------------------------
# Extracting XT1 Version Information (used for 'version_change' recipe)
#--------------------------------------------------------------------------------
# Example:
# 	XT1_DIR 		= xt1_firmware_v3p19
# 	XT1_VER 		= v3p19
# 	XT1_OLD 		= v3p17 (extracted from the existing xt1_firmware_*_devel directory (i.e., the previous version))
#	XT1_VER_NUM 	= 3.19
#	XT1_OLD_NUM 	= 3.17
#	XT1_VER_INT 	= 3
#	XT1_VER_DEC 	= 19
#	XT1_VER_INT_HEX = 03 (hexadecimal number for 3)
#	XT1_VER_DEC_HEX = 13 (hexadecimal number for 19)
#	XT1_FW_ID       = 0x0313
#--------------------------------------------------------------------------------
XT1_DIR			:= $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))
XT1_VER 		:= $(patsubst xt1_firmware_%,%,$(XT1_DIR))
XT1_OLD 		:= $(patsubst xt1_firmware_%_devel,%,$(wildcard xt1_firmware_*_devel))
XT1_VER_NUM 	:= $(shell echo $(patsubst v%,%,$(XT1_VER)) | sed 's/p/\./')
XT1_OLD_NUM 	:= $(shell echo $(patsubst v%,%,$(XT1_OLD)) | sed 's/p/\./')
XT1_VER_INT 	:= $(basename $(XT1_VER_NUM))
XT1_VER_DEC 	:= $(patsubst .%,%,$(suffix $(XT1_VER_NUM)))
XT1_VER_INT_HEX := $(shell printf "%02X" $(XT1_VER_INT))
XT1_VER_DEC_HEX := $(shell printf "%02X" $(XT1_VER_DEC))
XT1_FW_ID 		:= 0x$(XT1_VER_INT_HEX)$(XT1_VER_DEC_HEX)

version_change: nuke
	#rename $(XT1_OLD) $(XT1_VER) *
	#rename $(XT1_OLD) $(XT1_VER) */*
	#sed -i 's/\/\/ Version $(XT1_OLD_NUM)/\/\/ Version $(XT1_VER_NUM)/' xt1_firmware_$(XT1_VER)_devel/xt1_firmware_$(XT1_VER)_devel.c
	#sed -i 's/#define FIRMWARE_ID 0x..../#define FIRMWARE_ID $(XT1_FW_ID)/' xt1_firmware_$(XT1_VER)_devel/xt1_firmware_$(XT1_VER)_devel.c

.PHONY: clean clean_libs clean-% nuke cp version_change


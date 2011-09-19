PROGS = basic.bin blink.bin echo.bin echo_str.bin
MEMMAP = memmap
VECTORS = vectors.s

CROSS_COMPILE=$(shell if `hash arm-linux-gnueabi-gcc >& /dev/null`; then echo arm-linux-gnueabi; elif `hash arm-none-linux-gnueabi >& /dev/null`; then echo arm-none-linux-gnueabi; elif `hash arm-none-eabi-gcc >& /dev/null`; then echo arm-none-eabi; else echo -n ""; fi)
CC=$(CROSS_COMPILE)-gcc

ifeq (,$(CROSS_COMPILE))
  $(error "No cross compiler found. Have you installed an ARM toolchain?")
endif

OBJS  = $(PROGS:%.bin=%.o)
VECTORS_OBJS = $(VECTORS:%.s=%.o)

CFLAGS += -g -Wall -Wextra -Werror -mthumb -mcpu=cortex-m3 -nostdlib -nostartfiles -ffreestanding
ASFLAGS += -g -mthumb

LIB_DIR = lib
LIB_SOURCES  = $(wildcard $(LIB_DIR)/*.c)
LIB_SOURCES += $(wildcard $(LIB_DIR)/*.s)
LIB_OBJS := $(LIB_SOURCES:%.c=%.o)
LIB_OBJS := $(LIB_OBJS:%.s=%.o)

$(LIB_DIR)/%.o: $(LIB_DIR)/%.s
	$(CROSS_COMPILE)-as $(ASFLAGS) $< -o $@

$(LIB_DIR)/%.o: $(LIB_DIR)/%.c

all:	$(PROGS)

%.o:	%.c $(LIB_OBJS)

%.o:	%.s
	$(CROSS_COMPILE)-as $< -o $@

%.bin:	$(LIB_OBJS) $(VECTORS_OBJS) %.o 
	$(CROSS_COMPILE)-ld -T $(MEMMAP) $(VECTORS_OBJS) $(LIB_OBJS) $*.o -o $*.elf
	$(CROSS_COMPILE)-objdump -D $*.elf > $*.list
	$(CROSS_COMPILE)-objcopy -O binary $*.elf $*.bin

basic.bin:	basic.s
	$(CROSS_COMPILE)-as $(ASFLAGS) basic.s -o basic.o
	$(CROSS_COMPILE)-ld -T $(MEMMAP) basic.o -o basic.elf
	$(CROSS_COMPILE)-objdump -D basic.elf > basic.list
	$(CROSS_COMPILE)-objcopy -O binary basic.elf basic.bin

.PHONY:		all clean
.DEFAULT:	all

clean:
	rm -f *.o $(LIB_DIR)/*.o
	rm -f *.elf
	rm -f *.list
	rm -f $(PROGS)

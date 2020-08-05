CROSS_COMPILE = arm-linux-

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/include
LDFLAGS =
OBJCOPYFLAGS =

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := kernel.bin


obj-y += boot/
obj-y += init/
obj-y += kernel/
obj-y += drivers/
obj-y += fs/
obj-y += mm/


all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	# $(CC) $(LDFLAGS) -o $(TARGET) built-in.o mylibc/mylibc.a

	$(LD) $(LDFLAGS) -T kernel.lds -o kernel.elf 	built-in.o mylibc/mylibc.a
	$(OBJCOPY) $(OBJCOPYFLAGS) -O binary kernel.elf kernel.bin
	$(OBJDUMP) -D -m arm kernel.elf > kernel.dis


clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.bin")
	rm -f $(shell find -name "*.elf")
	rm -f $(shell find -name "*.dis")

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	
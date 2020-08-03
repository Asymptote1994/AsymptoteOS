CC = arm-linux-gcc
LD = arm-linux-ld
OBJCOPY = arm-linux-objcopy
OBJJUMP = arm-linux-objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/include

BUILTIN = boot/start.o
BUILTIN += init/main.o

BUILTIN += kernel/sched.o \
		   kernel/fork.o  \
		   kernel/list.o

BUILTIN += drivers/lcd.o  \
		   drivers/nand.o \
		   drivers/uart.o \
		   drivers/sd.o	  \
		   drivers/init.o

BUILTIN += fs/fs.o \
		   fs/romfs.o \
		   fs/simple_ext2.o

BUILTIN += mm/mem.o

.PHONY:

kernel.bin: $(BUILTIN) lib/libc.a 
	make -C boot/
	make -C init/
	make -C kernel/
	make -C fs/
	make -C mm/
	make -C drivers/
	
	$(LD) -T kernel.lds $^ -o kernel.elf
	$(OBJCOPY) -S kernel.elf -O binary $@
	$(OBJJUMP) -D -m arm kernel.elf > kernel.dis
	cp kernel.bin /mnt/hgfs/vmware_share

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find -name "*.o")
	rm -rf *.o *.elf *.dis *.bin

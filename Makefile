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

BUILTIN += fs/romfs.o
#file.o

BUILTIN += mm/simple_mem.o

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

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.elf *.dis *.bin


























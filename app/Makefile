CC = arm-linux-gcc
LD = arm-linux-ld
OBJCOPY = arm-linux-objcopy
OBJJUMP = arm-linux-objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/../include

BUILTIN = ../kernel/sched.o \
		  ../kernel/fork.o  \
		  ../kernel/list.o

BUILTIN += ../drivers/uart.o \
		   ../drivers/sd.o

BUILTIN += ../fs/romfs.o
#file.o

BUILTIN += ../mm/simple_mem.o

.PHONY:

cat.bin: cat.o $(BUILTIN) ../lib/libc.a 
	make -C ../kernel/
	make -C ../fs/
	make -C ../mm/
	make -C ../drivers/
	
	$(LD) -T cat.lds $^ -o cat.elf
	$(OBJCOPY) -S cat.elf -O binary $@
	$(OBJJUMP) -D -m arm cat.elf > cat.dis

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.elf *.dis *.bin


























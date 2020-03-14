CC = arm-linux-gcc
LD = arm-linux-ld
OBJCOPY = arm-linux-objcopy
OBJJUMP = arm-linux-objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/ -I$(shell pwd)/include

.PHONY: 

all: start.bin	task2.bin task3.bin

start.bin: head.o init.o main.o sched.o fork.o mem.o nand.o lcd.o list.o uart.o romfs.o sd.o lib/libc.a #print.o file.o
	$(LD) -Tstart.lds $^ -o start.elf
	$(OBJCOPY) -S start.elf -O binary $@
	$(OBJJUMP) -D -m arm start.elf > start.dis

task2.bin: task2.o lcd.o uart.o
	$(LD) -Ttask2.lds $^ -o task2.elf
	$(OBJCOPY) -S task2.elf -O binary $@
	$(OBJJUMP) -D -m arm task2.elf > task2.dis

task3.bin: task3.o lcd.o
	$(LD) -Ttask3.lds $^ -o task3.elf
	$(OBJCOPY) -S task3.elf -O binary $@
	$(OBJJUMP) -D -m arm task3.elf > task3.dis

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.elf *.dis *.bin


























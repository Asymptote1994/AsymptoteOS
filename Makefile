CC = arm-linux-gcc
LD = arm-linux-ld
OBJCOPY = arm-linux-objcopy
OBJJUMP = arm-linux-objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/ -I$(shell pwd)/include

.PHONY: cp_file

all: start.bin	task2.bin task3.bin

start.bin: head.o init.o main.o sched.o fork.o mem.o nand.o lcd.o list.o uart.o file.o sd.o lib/libc.a #print.o 
	$(LD) -Tstart.lds $^ -o start.elf
	$(OBJCOPY) -S start.elf -O binary $@
	$(OBJJUMP) -D -m arm start.elf > start.dis

task2.bin: task2.o lcd.o
	$(LD) -Ttask2.lds $^ -o task2.elf
	$(OBJCOPY) -S task2.elf -O binary $@
	$(OBJJUMP) -D -m arm task2.elf > task2.dis

task3.bin: task3.o lcd.o
	$(LD) -Ttask3.lds $^ -o task3.elf
	$(OBJCOPY) -S task3.elf -O binary $@
	$(OBJJUMP) -D -m arm task3.elf > task3.dis

cp_file:	
	cp start.bin /mnt/hgfs/Asymptote/start.bin
	cp start.dis /mnt/hgfs/Asymptote/start.dis
	
	cp task2.bin /mnt/hgfs/Asymptote/task2.bin
	cp task2.dis /mnt/hgfs/Asymptote/task2.dis
	
	cp task3.bin /mnt/hgfs/Asymptote/task3.bin
	cp task3.dis /mnt/hgfs/Asymptote/task3.dis

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.elf *.dis *.bin


























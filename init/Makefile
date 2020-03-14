CC = arm-linux-gcc
LD = arm-linux-ld
OBJCOPY = arm-linux-objcopy
OBJJUMP = arm-linux-objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I $(shell pwd)/../include

.PHONY: all

all: main.o

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.elf *.dis *.bin


























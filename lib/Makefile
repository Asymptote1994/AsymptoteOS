# CROSS_COMPILE = arm-none-linux-gnueabi-
CROSS_COMPILE = arm-linux-gnueabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJJUMP = $(CROSS_COMPILE)objdump

# CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/include
CFLAGS += -marm -mno-thumb-interwork -mabi=aapcs-linux -mword-relocations -fno-pic -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -msoft-float -pipe -march=armv4t
CFLAGS = -g -Wall -O2 -I$(shell pwd)/

objs := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o

libc.a: $(objs)
	${LD} -r -o $@ $^
	
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f libc.a *.o		
	

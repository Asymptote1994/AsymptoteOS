CROSS_COMPILE = arm-linux-
# CROSS_COMPILE = arm-none-linux-gnueabi-
# CROSS_COMPILE = arm-linux-gnueabi-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJJUMP = $(CROSS_COMPILE)objdump

CFLAGS = -g -Wall -O2 -nostdlib -fno-builtin -I$(shell pwd)/include
# CFLAGS += -marm -mno-thumb-interwork -mabi=aapcs-linux -mword-relocations -fno-pic -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -msoft-float -pipe -march=armv4t
# CFLAGS += -nostdinc -isystem -D__KERNEL__ -D__UBOOT__ -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding -std=gnu11 -fshort-wchar -fno-strict-aliasing -fno-PIE -Os -fno-stack-protector -fno-delete-null-pointer-checks -g -fstack-usage -Wno-format-nonliteral -Wno-unused-but-set-variable -Werror=date-time -D__ARM__ -marm -mno-thumb-interwork -mabi=aapcs-linux -mword-relocations -fno-pic -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -msoft-float -pipe -march=armv4t -D__LINUX_ARM_ARCH__=4

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

kernel.bin: $(BUILTIN) mylibc/mylibc.a 
	# make -C boot/
	# make -C init/
	# make -C kernel/
	# make -C fs/
	# make -C mm/
	# make -C drivers/
	
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

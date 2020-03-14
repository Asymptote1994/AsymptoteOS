CC = arm-linux-gcc
LD = arm-linux-ld
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
	

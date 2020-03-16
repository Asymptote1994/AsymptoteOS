#include <file.h>

void cat(char *args)
{
	int i, n = 0;
	int fd = -5;
	char buf_read[] = {0};
	
	printk("this is cat task, args = %s\n\r", args);

	fd = open(args, O_CREAT);
	if (fd == -1) {
		printk("open %s failed!\r\n", args);
		do_exit();
	} else {
		printk("open %s success, fd = %d\r\n", fd, args);
	}

	n = read(fd, buf_read, strlen(""));				

	printk("file length: %d\r\n", n);	
	for (i = 0; i < n; i++)
		printk("%c", buf_read[i]);
	printk("\r\n");

	close(fd);
	do_exit();
}


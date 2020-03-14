#include <init.h>
#include <s3c2440_regs.h>
#include <sched.h>
#include <fork.h>
#include <nand.h>
#include <lcd.h>
//#include <file.h>
//#include <fs.h>
#include <romfs.h>

//struct list_head *wait_queue_head = (struct list_head *)0x31300000;
struct list_head wait_queue_head;

void irq_handler(void)
{
	struct list_head *wq = &wait_queue_head;
	GPFDAT = 0;
	wake_up(wq);

	list_entry(wq->next, struct task_struct, list)->state = 0;
	list_add(wq->next, &(((struct task_struct *)0x30300000)->list));
}

void clear_irq(void)
{
	//清中断
    SRCPND = 1 << INTOFFSET;
    INTPND = INTPND;     
}

void print_task(void *p)
{
	int i;

	for (i = 0; i < 3; i++) {
		printk("this is the print task, arg = %d\n\r", (int)p);
		delay();
	}
	do_exit();
}

void lcd_task(void *p)
{
	int flag = 0;	

	list_init((&wait_queue_head));

	while (1) {
		printk("this is the lcd task.\n\r");
	
		GPFDAT = ~(1 << 6);
		draw_rect(150, 150, 190, 190, 0xff0000);
		delay();

		GPFDAT = 0xff;
		draw_rect(150, 150, 190, 190, 0xffffff);
		delay();

		if (flag == 0) {
			sleep_on((&wait_queue_head));
			flag = 1;
		}
	}
}

char *buf_read = (char *)0x33f00000;

void fs_task(char *path_name)
{
	int i, n = 0;
	int fd = -5;
	char buf_write[] = "    But a man is not made for defeat. A man can be destroyed but not defeated.";
//	char buf_read[] = {0};
		
	printk("reading data....\r\n");
	
	fd = open(path_name, O_CREAT);
	if (fd == -1) {
		printk("open %s failed!\r\n", path_name);
		return -1;
	} else {
		printk("open %s success, fd = %d\r\n", fd, path_name);
	}

	// n = write(fd, buf_write, strlen(buf_write));
	// printk("写入的文件长度: %d\r\n", n);
	
	// close(fd);

	// fd = open("/first", O_RDWR);
	// if (fd == -1) {
	// 	printk("open failed!\r\n");
	// 	return -1;
	// }
	
	// printk("文件打开成功，返回的fd: %d\r\n", fd);
	
	n = read(fd, buf_read, strlen(buf_write));				
	//buffer[n] = 0;
	printk("file length: %d\r\n", n);	
	for (i = 0; i < n; i++)
		printk("%c",buf_read[i]);
	printk("\r\n");

	close(fd);
	printk("leave fs_task()\r\n");
	do_exit();
}

int read_file(char *path_name)
{
	int i, n = 0;
	int fd = -5;
			
	fd = open(path_name, O_CREAT);
	if (fd == -1) {
		printk("open %s failed!\r\n", path_name);
		return -1;
	} else {
		printk("open %s success, fd = %d\r\n", fd, path_name);
	}
	
	n = read(fd, buf_read, strlen(""));				

	printk("file length: %d\r\n", n);	
	for (i = 0; i < n; i++)
		printk("%c",buf_read[i]);
	printk("\r\n");

	close(fd);
	return n;
}

int execv(char *path_name)
{
	int i, size;
	
	size = read_file(path_name);

	printk("read(): buf_read = 0x%x\r\n", buf_read);
	memcpy(0x33500000, buf_read, size);
//	for(i=0;i<20;i++){
//		printk("%x ",((unsigned char *)0x33500000)[i]);
//	}
//	printk("\r\n");

	do_fork(0x33500000,(void *)0x1);

	return 0;
}

void run_commond(char *command)
{
	if (strcmp(command, "ls") == 0) {
		show_dir_entry();	
	} else if (strcmp(command, "zhangxu") == 0 || strcmp(command, "Makefile") == 0) {
		do_fork(fs_task, (void *)command);
//		fs_task(command);
	} else if (strcmp(command, "task2.bin") == 0) {
		execv(command);
	} else if (strcmp(command, "wakeup") == 0) {
		wake_up(&wait_queue_head);
	} else if (strcmp(command, "") == 0) {

	} else {
		printk("%s: command not found\r\n", command);
	}
}

void shell(void *p)
{
	int i;
	char c;
	char command[30];
	
	do {
		printk("root@zhangxu:/# ");

		i = 0;
	
		do {
			c = getc();
			if (c != '\r') {
				putc(c);
				command[i++] = c;
			}	
		} while (c != '\r');

		command[i] = '\0';
			
		printk("\r\n");
		run_commond(command);
	} while (1);
}

void init(void)
{	
	int pid;

	key_init();
	uart0_init();
	led_init();
	lcd_init();
	task_init();
	sdi_init();
	timer0_init();
	
//	do_fork(exec(0x100000), (void *)0x1);
	pid = do_fork(shell, (void *)0x3);
//	do_fork(lcd_task, (void *)0x3);

//	init_fs();
//	show_dir_entry();

	puts("this is the init task.\n\r");

	while (1) {
		GPFDAT = ~(1 << 4);
		draw_rect(10, 10, 50, 50, 0x0000ff);
		delay();
		
		GPFDAT = 0xff;
		draw_rect(10, 10, 50, 50, 0xffffff);
		delay();		
	}
}


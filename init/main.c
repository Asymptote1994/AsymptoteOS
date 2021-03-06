#include <init.h>
#include <s3c2440_regs.h>
#include <sched.h>
#include <fork.h>
#include <nand.h>
#include <lcd.h>
#include <simple_ext2.h>
#include <fs.h>
#include <romfs.h>
#include <mem.h>

#define NULL ((void *)0)

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
	
//	fd = do_open("romfs", path_name, O_CREAT);
	fd = do_open("simple_ext2", path_name, O_CREAT);
	if (fd == -1) {
		printk("open %s failed!\r\n", path_name);
		return -1;
	} else {
		printk("open %s success, fd = %d\r\n", fd, path_name);
	}

	n = do_write(fd, buf_write, strlen(buf_write));
	printk("file length: %d\r\n", n);	
	
	do_close(fd);


	fd = do_open("simple_ext2", path_name, O_RDWR);
	if (fd == -1) {
		printk("open %s failed!\r\n", path_name);
		return -1;
	} else {
		printk("open %s success, fd = %d\r\n", fd, path_name);
	}
		
	n = do_read(fd, buf_read, strlen(buf_write));				
	//buffer[n] = 0;
	printk("file length: %d\r\n", n);	
	for (i = 0; i < n; i++)
		printk("%c",buf_read[i]);
	printk("\r\n");

	do_close(fd);
	printk("leave fs_task()\r\n");
	do_exit();
}

int read_file(char *path_name)
{
	int i, n = 0;
	int fd = -5;
			
	fd = do_open(path_name, O_CREAT);
	if (fd == -1) {
		printk("open %s failed!\r\n", path_name);
		return -1;
	} else {
		printk("open %s success, fd = %d\r\n", fd, path_name);
	}
	
	n = do_read(fd, buf_read, strlen(""));				

	printk("file length: %d\r\n", n);	
//	for (i = 0; i < n; i++)
//		printk("%c",buf_read[i]);
//	printk("\r\n");

	do_close(fd);
	return n;
}

int execv(char *exec_file_name, char *args)
{
	int i, size;
	
	size = read_file(exec_file_name);

	printk("read(): buf_read = 0x%x\r\n", buf_read);
	memcpy(0x33500000, buf_read, size);
//	for(i=0;i<20;i++){
//		printk("%x ",((unsigned char *)0x33500000)[i]);
//	}
//	printk("\r\n");

	do_fork(0x33500000,(void *)"zhangxu");

	return 0;
}

void run_commond(char *command)
{
	if (strcmp(command, "ls") == 0) {
		show_dir_entry();	
	} else if (strcmp(command, "zhangxu") == 0 || 
			strcmp(command, "/hahaha") == 0 || 
			strcmp(command, "Makefile") == 0) {
		do_fork(fs_task, (void *)command);
	} else if (strcmp(command, "cat.bin") == 0) {
		execv(command, NULL);
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

#if 1
void test_friend_mem(void)
{
	char *p1,*p2,*p3,*p4;

	p1=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages is 0x%x\r\n",p1);

	p2=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages is 0x%x\r\n",p2);

	put_free_pages(p2,6);
	put_free_pages(p1,6);

	p3=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages is 0x%x\r\n",p3);

	p4=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages is 0x%x\r\n",p4);
}

void test_kmalloc_mem(void)
{
	char *p1,*p2,*p3,*p4;

	p1=kmalloc(127);
	printk("the first alloced address is 0x%x\r\n",p1);

	p2=kmalloc(124);
	printk("the second alloced address is 0x%x\r\n",p2);

	kfree(p1);
	kfree(p2);

	p3=kmalloc(119);
	printk("the third alloced address is 0x%x\r\n",p3);

	p4=kmalloc(512);
	printk("the forth alloced address is 0x%x\r\n",p4);
}
#endif

void init(void)
{	
	int pid;
	char *p;

	printk("\n\rAsymptote v0.1\n\r");

	key_init();
	uart0_init();
	led_init();
	lcd_init();
	task_init();
	// sdi_init();
	timer0_init();
	vfs_init();
	init_page_map();
	kmalloc_init();

	pid = do_fork(shell, (void *)0x3);
	do_fork(lcd_task, (void *)0x3);

	test_friend_mem();
	test_kmalloc_mem();

	printk("\n\rlinux driver model demo\n\r");
	platform_bus_init();
	s3c2440_mmc_init();

	while (1) {
		GPFDAT = ~(1 << 4);
		draw_rect(10, 10, 50, 50, 0x0000ff);
		delay();
		
		GPFDAT = 0xff;
		draw_rect(10, 10, 50, 50, 0xffffff);
		delay();		
	}
}


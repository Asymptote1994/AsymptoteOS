#include <init.h>
#include <s3c2440_regs.h>
#include <sched.h>
#include <fork.h>
#include <nand.h>
#include <lcd.h>
//#include <file.h>
//#include <fs.h>
#include <romfs.h>


//int addr = 0x31300000;
//struct list_head *wait_queue_head = (struct list_head *)0x31300000;
struct list_head wait_queue_head;

void irq_handler(void)
{
	struct list_head *wq = &wait_queue_head;
	GPFDAT = 0;
//	wake_up(wq);


	list_entry(wq->next, struct task_struct, list)->state = 0;
	list_add(wq->next, &(((struct task_struct *)0x30300000)->list));

	
	delay();
	draw_rect(200, 200, 250, 250, 0xffff00);
	delay();
	
	draw_rect(200, 200, 250, 250, 0xffffff);
	delay();
}

void clear_irq(void)
{
	//清中断
    SRCPND = 1 << INTOFFSET;
    INTPND = INTPND;     
}

unsigned char *buffer = (unsigned char *)0x33600000;

void romfs_task(char *file_name)
{
	int i;
//	unsigned char *b = buf;
	struct inode node;
	unsigned int addr;

//	node=fs_type[ROMFS]->namei(fs_type[ROMFS],"number.txt");
//	fs_type[ROMFS]->device->dout(fs_type[ROMFS]->device,buf,fs_type[ROMFS]->get_daddr(node),node->dsize);

	printk("enter romfs_task()\r\n");

	simple_romfs_namei(&node, NULL, file_name);
	printk("romfs_task(): leave simple_romfs_namei\r\n");

	addr = romfs_get_daddr(&node);

	printk("romfs_task(): addr = %d\r\n", addr);

	sd_read_sector(buffer, addr/512, node.dsize/512 + 2);
	buffer = buffer + addr - (addr / 512) * 512;
	printk("romfs_task(): node.dsize = %d\r\n", node.dsize);

//	for(i=0;i<node.dsize;i++){
	for(i=0;i<node.dsize;i++){
		printk("%x ",buffer[i]);
	}
	printk("\r\n");
	for(i=0;i<node.dsize;i++){
		printk("%c",buffer[i]);
	}
	printk("\r\n");

	printk("romfs_task(): buffer = 0x%x\r\n", buffer);
	memcpy(0x33500000, buffer, 1500);
	for(i=0;i<20;i++){
		printk("%x ",((unsigned char *)0x33500000)[i]);
	}
	printk("\r\n");

}

void lcd_task(void *p)
{
	int flag = 0;	

	list_init((&wait_queue_head));

	while (1) {
//		puts("this is the 2nd task.\n\r");
	
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
/*
void fs_task(void *param)
{
	int n = 0;
	int fd = -5;
//	char key;
	char buf_write[] = "    But a man is not made for defeat. A man can be destroyed but not defeated.";
	char buf_read[] = {0};
		
	printk("reading data....r\n");
	
	fd = open("/first", O_CREAT);
	if (fd == -1) {
		printk("返回的fd: %d\r\n", fd);
	} else {
		printk("文件创建成功，返回的fd: %d\r\n", fd);
		
		n = write(fd, buf_write, strlen(buf_write));
		printk("写入的文件长度: %d\r\n", n);
		
		close(fd);
	}

	fd = open("/first", O_RDWR);
	if (fd == -1) {
		printk("返回的fd: %d\r\n", fd);
	} else {
		printk("文件打开成功，返回的fd: %d\r\n", fd);
		
		n = read(fd, buf_read, strlen(buf_write));				
		buf_read[n] = 0;
		printk("读出的文件长度: %d, 文件内容为: %s\r\n", n, buf_read);
		
		close(fd);
	}			
}
*/

int execv(unsigned int start){
	asm volatile(
		"mov pc,r0\n\t"
	);
	
	return 0;
}

void run_commond(char *command)
{
	if (strcmp(command, "") != 0) {
//		fs_task(NULL);
//		show_dir_entry();

		romfs_task(command);
	
		//execv(0x33500000);
		do_fork(0x33500000,(void *)0x1);
	} else if (strcmp(command, "") == 0) {
		//printk("root@zhangxu:/# ");
	} else {
		printk("%s: command not found", command);
	}
}

void shell(void *p)
{
	int i;
	char c;
	char command[30];
	
	do {
		printk("\r\nroot@zhangxu:/# ");

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
	key_init();
	uart0_init();
	led_init();
	lcd_init();
	task_init();
	sdi_init();
	timer0_init();
	
//	do_fork(exec(0x100000),(void *)0x1);
//	do_fork(fun_task2,(void *)0x2);	
	do_fork(shell,(void *)0x3);

//	init_fs();
//	show_dir_entry();

	while (1) {
//		puts("this is the init task.\n\r");

		GPFDAT = ~(1 << 4);
		draw_rect(10, 10, 50, 50, 0x0000ff);
		delay();
		
		GPFDAT = 0xff;
		draw_rect(10, 10, 50, 50, 0xffffff);
		delay();		
	}
}


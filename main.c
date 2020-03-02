#include <init.h>
#include <s3c2440_regs.h>
#include <sched.h>
#include <fork.h>
#include <nand.h>
#include <lcd.h>
#include <ctype.h>


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

void run_commond(char *comm)
{
	if (comm[0] == 'l' && comm[1] == 's') {
		puts("hello，my boy!");
		
	} else if (comm[0] == 'g' && comm[1] == 'r' && comm[2] == 'e') {		
		draw_rect(200, 100, 400, 200, 0x0000ff);
		puts("lcd test success");
		
	} else if (comm[0] == 'b' && comm[1] == 'l' && comm[2] == 'u') {		
		draw_rect(100, 100, 200, 200, 0x00ff00);
		puts("lcd test success");
			
	} else if (comm[0] == 'b' && comm[1] == 'o' && comm[2] == 'o' && comm[3] == 't') {		
		//load_kernel();
		
	} else {
		puts(comm);
		puts(": command not found");
	}
}

void fun_task1(void *p)
{
	while (1) {
//		puts("this is the 1st task.\n\r");

		GPFDAT = ~(1 << 5);
		draw_rect(80, 80, 120, 120, 0x00ff00);
		delay();
		
		GPFDAT = 0xff;
		draw_rect(80, 80, 120, 120, 0xffffff);
		delay();
	}
}

void fun_task3(void *p);

void fun_task2(void *p)
{
	int flag = 0;	

	list_init((&wait_queue_head));
//	do_fork(fun_task3,(void *)0x3);

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

void fun_task3(void *p)
{
	int i;
	char c;
	char comm[30];
	
	putc('\r');
	putc('\n');
	puts("my_bootloader# ");

	while (1) {
		i = 0;
	
		do {
			c = getc();
			if (c != '\r') {
				putc(c);
				comm[i++] = c;
			}
				
		} while (c != '\r');
		comm[i] = '\0';
			
		putc('\r');
		putc('\n');
		run_commond(comm);

		putc('\r');
		putc('\n');
		puts("my_bootloader# ");		
	}
}

void init(void)
{	
	key_init();
	uart0_init();
	led_init();
	lcd_init();
	task_init();
	
	do_fork(exec(0x100000),(void *)0x1);
	do_fork(fun_task2,(void *)0x2);	
	do_fork(fun_task3,(void *)0x3);
	timer0_init();

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


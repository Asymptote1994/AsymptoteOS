#include <sched.h>
#include <nand.h>
#include <mem.h>
#include <fork.h>
#include <list.h>


struct task_struct *get_task_struct_addr(void)
{
	static int task_stack_addr_base = TASK_STRUCT_ADDR_BASE;

	struct task_struct *tmp = (struct task_struct *)task_stack_addr_base;
	task_stack_addr_base += TASK_SIZE;

	return tmp;
}


#define DO_INIT_SP(sp,fn,args,lr,cpsr,pt_base)	\
		do {\
				(sp)=(sp)-4;/*r15*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(fn);/*r15*/		\
				(sp)=(sp)-4;/*r14*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(lr);/*r14*/		\
				(sp)=(sp)-4*13;/*r12,r11,r10,r9,r8,r7,r6,r5,r4,r3,r2,r1,r0*/	\
				*(volatile unsigned int *)(sp)=(unsigned int)(args);  \
				(sp)=(sp)-4;/*cpsr*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(cpsr);  \
		} while (0)



unsigned int get_cpsr(void)
{
	unsigned int p;
	asm volatile(
		"mrs %0,cpsr\n"
		:"=r"(p)
		:
	);
	
	return p;
}

int do_fork(void (*fn)(void *), void *args)
{
	struct task_struct *tsk,*tmp;

	/* 分配一个4KB大小的内存地址，用以存放进程堆栈和进程描述符struct task_struct */
	if((tsk = get_task_struct_addr()) == (void *)0)
		return -1;

	/* 让sp指向上述分配的地址的高地址顶端 */
	tsk->sp = ((unsigned int)(tsk) + TASK_SIZE);
	/* 为sp赋予寄存器初值 */
	DO_INIT_SP(tsk->sp, fn, args, 0, 0x1f&get_cpsr(), 0);

	tsk->state = 0;
	current->state = 0;

	disable_schedule();
	list_add_tail(&(tsk->list), &(current->list));
	enable_schedule();

	return 0;
}

unsigned char *exec(unsigned long nand_addr)
{
	unsigned char *addr = (unsigned char *)get_free_page();

	nand_read(nand_addr, addr, PAGE_SIZE);
	
	return addr;
}


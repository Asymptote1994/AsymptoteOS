#include <map.h>
#include <s3c2440_regs.h>
#include <sched.h>
#include <fork.h>
#include <list.h>
#include <uart.h>


struct task_struct *get_current(void)
{
	register unsigned long sp asm ("sp");
	return (struct task_struct *)(sp & ~(TASK_SIZE - 1));
}

void task_init(void)
{
	list_init(&(current->list));
}

void enable_irq(void)
{
	asm volatile (
		"mrs r4,cpsr\n\t"
		"bic r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

void disable_irq(void)
{
	asm volatile (
		"mrs r4,cpsr\n\t"
		"orr r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

struct list_head *tmp;

void *__common_schedule(void)
{
	if (current->state == 0)
		return (void *)(list_entry(current->list.next, struct task_struct, list));
	else
		return (void *)(list_entry(tmp, struct task_struct, list));
}

void switch_to(struct task_struct *task)
{
	
}

void schedule(void)
{
	
}

void wake_up(struct list_head *wq)
{
	disable_schedule();
	list_entry(wq->next, struct task_struct, list)->state = 0;
	list_add(wq->next, &(current->list));
	enable_schedule();	
}


void sleep_on(struct list_head *wq)
{
	tmp = current->list.next;

	current->state = 1;

	disable_schedule();
	list_del(&(current->list));
	list_add(&(current->list), wq);
	enable_schedule();	
}




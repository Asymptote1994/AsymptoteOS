#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <file.h>


struct task_struct {
	unsigned int sp;				/* sp必须放在第一个位置，保证其地址和结构体的首地址相同 */
	unsigned int pid;				/* 进程标识号 */
	unsigned int state;				/* 进程状态 */
	struct file *filp[FILE_NR];		/* 进程打开文件数组 */
	struct list_head list;			/* 内嵌通用链表 */
};

void task_init(void);
struct task_struct *get_current(void);

#define current get_current()


extern void __switch_to(void);
//void switch_to(struct task_struct *task);

#define disable_schedule(x)	disable_irq()
#define enable_schedule(x)	enable_irq()

void enable_irq(void);
void disable_irq(void);

void sleep_on(struct list_head *wq);
void wake_up(struct list_head *wq);

#endif


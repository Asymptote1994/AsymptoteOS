#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <fs.h>


struct task_struct {
	unsigned int sp;				/* sp������ڵ�һ��λ�ã���֤���ַ�ͽṹ����׵�ַ��ͬ */
	unsigned int pid;				/* ���̱�ʶ�� */
	unsigned int state;				/* ����״̬ */
	struct file *filp[FILE_NR];		/* ���̴��ļ����� */
	struct list_head list;			/* ��Ƕͨ������ */
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


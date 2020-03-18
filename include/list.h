#ifndef __LIST_H__
#define __LIST_H__

#include <ctype.h>
struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

void list_init(struct list_head *head);
void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head *new, struct list_head *head);
void list_del(struct list_head *entry);
int list_empty(struct list_head *head);

/*
 * list_entry - 由list_head类型变量获得对应的结构体指针
 *
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/****************************************************************************/

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_entry(pos, head, member)                \
    for (pos = list_entry((head)->next, typeof(*pos), member);    \
         &pos->member != (head);     \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#endif

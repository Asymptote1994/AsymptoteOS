#include <list.h>

void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

void list_init(struct list_head *head)
{
	head->prev = head;
	head->next = head;
}

/*
void list_add(struct list_head *new, struct list_head *head)
{
	new->prev = head;
	new->next = head->next;
	head->next->prev = new;
	head->next = new;
}

void list_add_tail(struct list_head *new, struct list_head *head)
{
	new->prev = head->prev;
	new->next = head;
	head->prev->next = new;
	head->prev = new;
}


void list_del(struct list_head *entry)
{
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
}
*/

void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	//entry->next = LIST_POISON1;
	//entry->prev = LIST_POISON2;
}

int list_empty(struct list_head *head)
{
	return head->next == head;
}

void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

void list_remove_chain(struct list_head *ch,struct list_head *ct){
	ch->prev->next=ct->next;
	ct->next->prev=ch->prev;
}

void list_add_chain(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head;
		ct->next=head->next;
		head->next->prev=ct;
		head->next=ch;
}

void list_add_chain_tail(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head->prev;
		head->prev->next=ch;
		head->prev=ct;
		ct->next=head;
}

/*
 * �˴����������ã�������ʹ��
 */
struct example {
	char a;
	int b;
	struct list_head *list;
	long c;
};

 #if 0
void *list_entry(struct list_head *ptr, struct example, struct list_head *list)
{
	const typeof( ((struct example *)0)->list ) *__mptr = (ptr);
	return (struct example *)( (char *)__mptr - ((unsigned int) &((struct example *)0)->list) );
}          
#endif


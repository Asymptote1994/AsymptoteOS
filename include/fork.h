#ifndef __FORK_H__
#define __FORK_H__

#define SECTION_SIZE 	 	 (4 * 1024)
#define PROCESS_NR 			 64
#define SP_ADDR_BASE 		 0x34000000
#define SP_ADDR_SIZE 		 512
#define TASK_SIZE			 4096
#define TASK_STRUCT_ADDR_BASE	 0x30300000


int do_fork(void (*fn)(void *),void *args);
unsigned char *exec(unsigned long nand_addr);

#endif


#ifndef __MEM_H__
#define __MEM_H__

#define PAGES_NR 64*1024*1024/4/1024			/* 总共的内存页数，64MB/4KB */

#define MEM_ADDRESS_START 0x32000000			/* 内存分配的起始地址，在外部SDRAM */
#define SECTIONS_ADDRESS_START 0x32000000
#define PAGE_SIZE 4*1024                      	/* 内存分配的最小单位：一页，大小为4KB */

unsigned long *get_free_page(void);

#endif


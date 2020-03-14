/*
 * mem.c
 *
 * 2018.12.31 内存管理文件，现在用的是固定分配方式，大小为4KB，
 * 			之后打算改成基本分页分配方式。
 * 
 * 2019.01.07 
 *
 *
 *
 *
 */
 
#include <error.h>
#include <mem.h>

unsigned char mem_map[PAGES_NR] = {0};		/* 内存分配位图数组，0-当前页未被用 1-当前页已被用                      */


/*
 * get_free_page()
 *
 * 获取一个空闲的4KB大小的页面，成功返回获取到的页面的起始物理地址，
 * 失败返回0		
 * 
 *
 *
 *
 *
 */
unsigned long *get_free_page(void)
{
	int i;

	for (i = 0; i < PAGES_NR; i++) {
		if (mem_map[i] == 0) {
			mem_map[i] = 1;
			return (unsigned long *)(MEM_ADDRESS_START + i * PAGE_SIZE);
		}
	}

	return -ENOMEM;
}

/*
 * free_page()
 *
 * 释放物理地址起始为addr的4KB大小的页面，
 * 失败返回0		
 * 
 *
 *
 *
 *
 */
int free_page(unsigned long *addr)
{
	int page = (int)addr / (PAGE_SIZE);

	if (mem_map[page] != 0) {
		mem_map[page] = 0;
	} else {
		//return -ESDF;
	}

	return 0;
}


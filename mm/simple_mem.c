/*
 * mem.c
 *
 * 2018.12.31 �ڴ�����ļ��������õ��ǹ̶����䷽ʽ����СΪ4KB��
 * 			֮�����ĳɻ�����ҳ���䷽ʽ��
 * 
 * 2019.01.07 
 *
 *
 *
 *
 */
 
#include <error.h>
#include <mem.h>

unsigned char mem_map[PAGES_NR] = {0};		/* �ڴ����λͼ���飬0-��ǰҳδ���� 1-��ǰҳ�ѱ���                      */


/*
 * get_free_page()
 *
 * ��ȡһ�����е�4KB��С��ҳ�棬�ɹ����ػ�ȡ����ҳ�����ʼ�����ַ��
 * ʧ�ܷ���0		
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
 * �ͷ������ַ��ʼΪaddr��4KB��С��ҳ�棬
 * ʧ�ܷ���0		
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


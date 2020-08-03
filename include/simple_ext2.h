#ifndef __SIMPLE_EXT2_H__
#define __SIMPLE_EXT2_H__

#include <fs.h>

#define   TOTAL_SECTS               8388608                     //���ݳ���������,����Ϊ: MAX_DRIVE_SIZE / SECTOR_SIZE

#define   MAGIC_V1                	777

#define   MAX_DRIVE_SIZE    		4*1024*1024*1024            //�豸�������ֽڣ�
#define   SECTOR_SIZE       		512                         //�豸һ�������Ĵ�С���ֽڣ�
#define   INODE_SIZE        		32                          //inode�ṹ��Ĵ�С���ֽڣ�
#define   DIR_ENTRY_SIZE    		sizeof(struct dir_entry)    //dir_entry�ṹ��Ĵ�С���ֽڣ�
#define   SUPER_BLOCK_SIZE  		56                          //һ��������ṹ��Ĵ�С���ֽڣ�
#define   NUM_ROOT_INODE    		1                           //��Ŀ¼/��inode��
#define   SIZE_DEFAULT_FILE_SECTS   2048                        //ÿ���ļ�����ռ�õ������������
																  //��ÿ���ļ����Ϊ2048 * 512 = 1MB

#define   I_DIRECTORY         		0                           //�ļ����ͣ�Ŀ¼�ļ�
#define   I_CHAR_SPECIAL            1                           //�ļ����ͣ��ַ��豸�ļ�
#define   I_REGULAR                 2							//�ļ����ͣ���ͨ�ļ�

#define   TOTAL_CONSOLES            3                           //����̨������

#define   NUM_FILES                 64
#define   NUM_FILE_DESC             64
#define   NUM_INODE                 64
#define   NUM_SUPER_BLOCK           8                           /* ����������������Ŀ��Ҳ�����֧�ֵ��ļ�ϵͳ��Ŀ��*/

#define   O_CREAT                   0
#define   O_RDWR                    1

#define   NO_DEV                    0
#define   ROOT_DEV                  1

#define   ROOT_INODE                1                           /* ��Ŀ¼��inode */

#define   FSBUF_SIZE                512

#define   SECTOR_SIZE_SHIFT         9

/* 
struct super_block
{
	int magic;                   //�ļ�ϵͳ��ʶ
	int total_inodes;            //�ļ�ϵͳ��inode��
	unsigned int total_sects;    //�ļ�ϵͳ��������
		
	int size_inode_sects;        //inodeռ�õ�������
	int size_imap_sects;         //inodeλͼռ�õ�������
	int size_smap_sects;		 //sectorλͼռ�õ�������
	
	int num_root_inode;          //��Ŀ¼��Ӧ��inode��    
	int num_1st_sect;            //��һ������������������    
	int inode_size;              //һ��inode�ṹ���ж����ֽ�
//	int inode_isize_off;
//	int inode_start_off;
	int dir_entry_size;          //�ļ�ϵͳ���������ļ���
//	int dir_ent_inode_off;
//	int dir_ent_fname_off;
	
	//���µ�ֻ�����ڴ��д���
	int sb_dev;
};

struct inode
{
	int i_mode;                //�ļ�����
	int i_size;                //�ļ���С
	int i_start_sect;          //�ļ���ʼ������
	int i_total_sects;         //�ļ�ռ�õ���������
	char i_unused[16];         //

	//���µ�ֻ�����ڴ��д���
	int i_dev;				   //
	int i_cnt;                 //��inodeͬʱ�����ٸ����̹���
	int i_num;                 //inode��
};

void create_fs(void);
void init_fs(void);

int open(const char *path_name, int flags);
int close(int fd);

int write(int fd, char *buffer, int count);
int read(int fd, char *buffer, int count);

void show_dir_entry(void);
*/
int simple_ext2_init(void);

#endif

#ifndef FILE_H
#define FILE_H

#define   TOTAL_SECTS               8388608                     //数据长度有问题,数据为: MAX_DRIVE_SIZE / SECTOR_SIZE

#define   MAGIC_V1                	777

#define   MAX_FILENAME_LEN         	12                          //文件名的最大长度
#define   MAX_DRIVE_SIZE    		4*1024*1024*1024            //设备容量（字节）
#define   SECTOR_SIZE       		512                         //设备一个扇区的大小（字节）
#define   INODE_SIZE        		32                          //inode结构体的大小（字节）
#define   DIR_ENTRY_SIZE    		sizeof(struct dir_entry)    //dir_entry结构体的大小（字节）
#define   SUPER_BLOCK_SIZE  		56                          //一个超级块结构体的大小（字节）
#define   NUM_ROOT_INODE    		1                           //根目录/的inode号
#define   SIZE_DEFAULT_FILE_SECTS   2048                        //每个文件允许占用的最大扇区数，
																  //即每个文件最大为2048 * 512 = 1MB

#define   I_DIRECTORY         		0                           //文件类型：目录文件
#define   I_CHAR_SPECIAL            1                           //文件类型：字符设备文件
#define   I_REGULAR                 2							//文件类型：普通文件

#define   TOTAL_CONSOLES            3                           //控制台的数量

#define   NUM_FILES                 64
#define   NUM_FILE_DESC             64
#define   NUM_INODE                 64
#define   NUM_SUPER_BLOCK           8                           /* 允许超级块的最大数目（也即最多支持的文件系统数目）*/

#define   O_CREAT                   0
#define   O_RDWR                    1

#define   NO_DEV                    0
#define   ROOT_DEV                  1

#define   ROOT_INODE                1                           /* 根目录的inode */

#define   FSBUF_SIZE                512

#define   SECTOR_SIZE_SHIFT         9

#define FILE_NR 64												/* 一个进程最多可以打开的文件数目 */
 
struct super_block
{
	int magic;                   //文件系统标识
	int total_inodes;            //文件系统总inode数
	unsigned int total_sects;    //文件系统总扇区数
		
	int size_inode_sects;        //inode占用的扇区数
	int size_imap_sects;         //inode位图占用的扇区数
	int size_smap_sects;		 //sector位图占用的扇区数
	
	int num_root_inode;          //根目录对应的inode号    
	int num_1st_sect;            //第一个数据扇区的扇区号    
	int inode_size;              //一个inode结构体有多少字节
//	int inode_isize_off;
//	int inode_start_off;
	int dir_entry_size;          //文件系统允许的总文件数
//	int dir_ent_inode_off;
//	int dir_ent_fname_off;
	
	//以下的只会在内存中存在
	int sb_dev;
};

struct inode
{
	int i_mode;                //文件类型
	int i_size;                //文件大小
	int i_start_sect;          //文件起始扇区号
	int i_total_sects;         //文件占用的总扇区数
	char i_unused[16];         //

	//以下的只会在内存中存在
	int i_dev;				   //
	int i_cnt;                 //此inode同时被多少个进程共享
	int i_num;                 //inode号
};

struct dir_entry
{
	int inode_num;                       //目录的inode所在的扇区号
	char name[MAX_FILENAME_LEN];         //文件名称
};

struct file
{
	int fd_mode;                        //文件访问方式: 创建/读/写
	int fd_pos;                         //文件当前访问位置
	struct inode *fd_inode_ptr;         //文件inode指针
};


void create_fs(void);
void init_fs(void);

int open(const char *path_name, int flags);
int close(int fd);

int write(int fd, char *buffer, int count);
int read(int fd, char *buffer, int count);

void show_dir_entry(void);

#endif

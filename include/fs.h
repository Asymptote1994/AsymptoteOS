#ifndef FS_H__
#define FS_H__

#define MAX_SUPER_BLOCK	(8)

#define ROMFS	0

//struct super_block;

struct file_operations {
	// int (*read) (struct file *, char __user *, size_t, loff_t *);
	// int (*write) (struct file *, const char __user *, size_t, loff_t *);
	// int (*open) (struct inode *, struct file *);
	// int (*release) (struct inode *, struct file *);
	int (*read) (int, char *, int);
	int (*write) (int, char *, int);
	int (*open) (const char *, int);
	int (*release) (int);
};

struct file
{
	int fd_mode;                        //文件访问方式: 创建/读/写
	int fd_pos;                         //文件当前访问位置
	struct inode *fd_inode_ptr;         //文件inode指针
};

struct inode{
//	char *name;
	char name[50];
	unsigned int flags;
	unsigned int dsize;			//file data size
	unsigned int daddr;				//文件头信息在存储设备中的位置
	struct super_block *super;
};

struct super_block{
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

	struct inode *(*namei)(struct super_block *super,char *p);
	unsigned int (*get_daddr)(struct inode *);
	
//	struct storage_device *device;
	char *name;
};

struct file_system_type {
	const char *name;
	const struct file_operations *fops;
	struct dentry *(*mount) (struct file_system_type *, int,
		       const char *, void *);
	struct list_head next;
};

// extern struct super_block *fs_type[];

#endif


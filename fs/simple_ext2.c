/*
 * file.c - 文件系统核心代码
 *
 * 该文件实现了
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */


#include <string.h>
#include <file.h>
#include <sched.h>
#include <sd.h>
#include <fs.h>

#define u32 unsigned int
#define u16 unsigned short
#define u8  unsigned char

u8 buf[FSBUF_SIZE];

struct inode *root_inode;

struct file fd_table[NUM_FILE_DESC];
struct inode inode_table[NUM_INODE];
struct super_block super_block[NUM_SUPER_BLOCK];


void buf_set_0(u8 *buf)
{
	int i;
	for(i=0;i<FSBUF_SIZE;i++)
	{
		buf[i] = 0;
	}
}

/*
 * create_fs - 创建文件系统的头部信息，并写到存储设备中
 *
 * 具体的头部信息包括： 
 *   		1.超级块super block 
 *      	2.inode位图
 *      	3.扇区位图
 *			4.inode具体数据
 *  		5.具体文件数据
 *
 */
void create_fs(void)
{
	int i,j;
	int bits_per_sect = SECTOR_SIZE * 8;               //一个扇区的比特数
	int num_sects;
	u8 sb_temp[FSBUF_SIZE] = {0};
	struct super_block *sb;
	struct inode *pi;
	struct dir_entry *pde;
	
	/************************/
	/*      super block     */
	/************************/
	sb = (struct super_block *)sb_temp;
	
	sb->magic = MAGIC_V1;
	
	//inode的最大数量: 4096
	sb->total_inodes = bits_per_sect;                                     //inode的最大数量为一个扇区共512*8位所能索引的最大数量
	
	//扇区的最大数量: 8388608 （838万8608）
	sb->total_sects = TOTAL_SECTS;                                        //扇区的最大数量为  设备总容量/每个扇区的大小
	
//	printk("赋值时的sb->total_sects = %d\r\n",sb->total_sects);		
	
	//inode占用的扇区数: 256
	sb->size_inode_sects = sb->total_inodes * INODE_SIZE / SECTOR_SIZE;   //inode总数 * 每个inode结构体有多少字节 / 一个扇区的字节数  
	
	//inode位图占用的扇区数: 1
	sb->size_imap_sects = 1;                                              //inode索引位图占用的扇区数
	
	//扇区位图占用的扇区数: 2048
	sb->size_smap_sects = sb->total_sects / bits_per_sect;                //总扇区数 / 一个扇区的比特数
	
	//第一个数据扇区的编号: 1+1+2048+256 = 2306
	sb->num_1st_sect = 1 + sb->size_imap_sects + sb->size_smap_sects + sb->size_inode_sects;
									//超级快占用的扇区数 + inode位图占用的扇区数 + 扇区位图占用的扇区数 + inode占用的扇区数
	/* inode结构体的大小（字节）: 32 */
	sb->inode_size = INODE_SIZE;
	
	/* 根目录的inode号: 1      */
	sb->num_root_inode = NUM_ROOT_INODE;
	
	/* dir_entry结构体的大小（字节）: 16           */
	sb->dir_entry_size = DIR_ENTRY_SIZE;
	
	if(sd_write_sector(sb_temp,0,1)==0)	//写入0扇区	
		printk("write super_block data success！\r\n"); 
	else
		printk("write super_block data failed！\r\n"); 
	
	/************************/
	/*      inode map       */
	/************************/
	buf_set_0(buf);
	buf[0] = 0x1f;
	
	if(sd_write_sector(buf, sb->size_imap_sects, 1)==0)	//写入1扇区	
		printk("inode索引位图数据写入成功！\r\n"); 
	else
		printk("inode索引位图数据写入失败！\r\n"); 
	
	/************************/
	/*     sectors map      */
	/************************/
	buf_set_0(buf);
	num_sects = SIZE_DEFAULT_FILE_SECTS + 1;               //第0位被保留，所以加1
	
	for (i = 0; i < num_sects / 8; i++)
		buf[i] = 0xff;
	
	for (j = 0; j < num_sects % 8; j++)
		buf[i] |= (1 << j);
	
	if(sd_write_sector(buf,1+sb->size_imap_sects,1)==0)	//写入2扇区	
		printk("扇区索引位图第一个扇区数据写入成功！\r\n"); 
	else
		printk("扇区索引位图第一个扇区数据写入失败！\r\n"); 
	
//	buf_set_0(buf);
//	for(i=1; i<sb->size_smap_sects ;i++)
//	{
//		if(sd_write_sector(buf, 1+sb->size_imap_sects+i, 1)==0)	//写入扇区	
//			printk("扇区索引位图剩余扇区数据写入成功！\r\n"); 
//		else
//			printk("扇区索引位图剩余扇区数据写入失败！\r\n"); 
//	}
	
	/************************/
	/*     inode_array      */
	/************************/
	
	/* inode of '/' */
	buf_set_0(buf);
	pi = (struct inode *)buf;
	
	pi->i_mode = I_DIRECTORY;
	pi->i_size = DIR_ENTRY_SIZE * 4;             //4个文件：".", tty0, tty1, tty2
	pi->i_start_sect = sb->num_1st_sect;
	pi->i_total_sects = SIZE_DEFAULT_FILE_SECTS;
	
//	printk("pi->i_mode = %d\r\n", pi->i_mode);

	/* inode of `/dev_tty0~2' */
	for (i = 0; i < TOTAL_CONSOLES; i++) {
		pi = (struct inode *)(buf + (INODE_SIZE * (i + 1)));
		pi->i_mode = I_CHAR_SPECIAL;
		pi->i_size = 0;
		pi->i_start_sect = i;
		pi->i_total_sects = 0;
	}
	
//	printk("1+sb->size_imap_sects+sb->size_smap_sects = %d\r\n", 1+sb->size_imap_sects+sb->size_smap_sects);
	
	if(sd_write_sector(buf, 1+sb->size_imap_sects+sb->size_smap_sects, 1)==0)	//写入扇区	
		printk("inode数据写入成功！\r\n"); 
	else
		printk("inode数据写入失败！\r\n"); 
	
	/************************/
	/*          "/"         */
	/************************/
	
	/* dir entries of "." */
	buf_set_0(buf);
	pde = (struct dir_entry *)buf;
	
	pde->inode_num = 1;
	strcpy(pde->name, ".");

	/* dir entries of `/dev_tty0~2' */
	for (i = 0; i < TOTAL_CONSOLES; i++) 
	{
		pde++;
		pde->inode_num = i + 2;               /* dev_tty0's inode_nr is 2 */
		switch(i)
		{
			case 0:
				strcpy(pde->name, "dev_tty0");
				break;
			case 1:
				strcpy(pde->name, "dev_tty1");
				break;
			case 2:
				strcpy(pde->name, "dev_tty2");
				break;
		}	
	}
	if(sd_write_sector(buf, sb->num_1st_sect, 1)==0)	   //写入扇区
		printk("根目录/数据写入成功！\r\n"); 
	else
		printk("根目录/数据写入失败！\r\n"); 	
}	


/*
 * read_super_block - 将存储设备中的超级块数据读入内存中的全局变量super_block[]数组中
 *
 * @dev 
 *   	  			 	
 * 在全局变量super_block[]数组中寻找第一个sb_dev为NO_DEV的数组项，然后将存储设备的
 * 第0个扇区（存储超级块的扇区）读出到该数组项
 */
void read_super_block(int dev)
{
	int i;
	struct super_block *psb;
	
	/* find a free slot in super_block[] */
	for (i = 0; i < NUM_SUPER_BLOCK; i++)
		if (super_block[i].sb_dev == NO_DEV)
			break;
		
	if (i == NUM_SUPER_BLOCK)
		printk("super_block slots used up\r\n");

	if(sd_read_sector(buf, 0, 1)==0)	   //读扇区
		printk("超级快数据读入成功！\r\n"); 
	else
		printk("超级快数据读入失败！\r\n"); 	
	
	psb = (struct super_block *)buf;

	super_block[i] = *psb;
	super_block[i].sb_dev = dev;
}


/*
 * get_super_block - 从全局变量super_block[]数组中取得指定的超级块内容
 *
 * @dev 
 *   		 	
 * @return 取得的超级块指针
 *
 */
struct super_block *get_super_block(int dev)
{
	struct super_block *sb = super_block;
	for (; sb < &super_block[NUM_SUPER_BLOCK]; sb++)
		if (sb->sb_dev == dev)
			return sb;

	printk("super block of devie %d not found.\n", dev);
	return 0;
}


/*
 * get_inode - 由@dev和@num得到对应的inode结构体数据
 *
 * @dev 文件系统对应的设备
 * @num inode结构体序号 	
 *
 * @return 取得的inode结构体指针
 *
 * 
 * 先在inode_table[]全局变量中查找，如果没有，则去存储设备中查找并复制到inode_table[]中
 *
 */
struct inode *get_inode(int dev, int num)
{
	struct inode *p;
	struct inode *q = NULL;
	struct super_block *sb;
	int blk_nr;
	struct inode *pinode;
	
	if (num == 0)
		return 0;

	/* 先在inode_table[]全局变量中查找，如果没有，则去存储设备中查找并复制到inode_table[]中 */
	for (p = &inode_table[0]; p < &inode_table[NUM_INODE]; p++) {
		/* not a free slot */
		if (p->i_cnt) {	
			if ((p->i_dev == dev) && (p->i_num == num)) {
				p->i_cnt++;
				return p;
			}
		/* a free slot */
		} else {		
			if (!q) /* q hasn't been assigned yet */
				q = p; /* q <- the 1st free slot */
		}
	}

	if (!q)
		printk("the inode table is full\r\n");

	q->i_dev = dev;
	q->i_num = num;
	q->i_cnt = 1;

	sb = get_super_block(dev);

	/* 由inode序号num得到对应的存储inode结构体所在的扇区编号 */
	blk_nr = 1 + sb->size_imap_sects + sb->size_smap_sects + ((num - 1) / (SECTOR_SIZE / INODE_SIZE));
	
//	printk("sb->size_imap_sects = %d\r\n", sb->size_imap_sects);
//	printk("sb->size_smap_sects = %d\r\n", sb->size_smap_sects);
	printk("blk_nr = %d\r\n", blk_nr);
	
	if (sd_read_sector(buf, blk_nr, 1) == 0)	   //读inode所在扇区到内存buf缓冲区
		printk("根目录文件的inode数据读入成功！\r\n"); 
	else
		printk("根目录文件的inode数据读入失败！\r\n"); 	

	/* 由inode序号num得到该inode在一个扇区中的具体位置 */
	pinode = (struct inode *)((u8 *)buf + ((num - 1 ) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE);
	
	printk("((num - 1 ) % (SECTOR_SIZE / INODE_SIZE)) = %d\r\n", ((num - 1 ) % (SECTOR_SIZE / INODE_SIZE)));
	
	q->i_mode = pinode->i_mode;
	q->i_size = pinode->i_size;
	q->i_start_sect = pinode->i_start_sect;
	q->i_total_sects = pinode->i_total_sects;
	
	return q;
}


/*
 * put_inode - 
 *
 * @pinode 
 *
 */
void put_inode(struct inode *pinode)
{
	pinode->i_cnt--;
}


/*
 * sync_inode - 将@p指向的inode结构体数据写到存储设备中
 *
 * @p 指向待存储的inode结构体的指针
 *
 */
void sync_inode(struct inode *p)
{
	struct inode *pinode;
	struct super_block *sb = get_super_block(p->i_dev);
	int blk_nr = 1 + sb->size_imap_sects + sb->size_smap_sects + ((p->i_num - 1) / (SECTOR_SIZE / INODE_SIZE));
	
	if(sd_read_sector(buf, blk_nr, 1) == 0)	   //读入扇区
		printk("inode数据读入成功！\r\n"); 
	else
		printk("inode数据读入失败！\r\n"); 	
	
	pinode = (struct inode*)((u8*)buf + (((p->i_num - 1) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE));
	
	pinode->i_mode = p->i_mode;
	pinode->i_size = p->i_size;
	pinode->i_start_sect = p->i_start_sect;
	pinode->i_total_sects = p->i_total_sects;
	
	if(sd_write_sector(buf, blk_nr, 1)==0)	
		printk("inode数据回写成功！\r\n");
	else
		printk("inode数据回写失败！\r\n"); 
}


/*
 * strip_path - 由文件路径得到文件名
 *
 * @filename 文件名
 * @pathname 文件路径
 * @ppinode 
 *
 * @return
 *		-1 失败     0 成功
 */
int strip_path(char *filename, const char *pathname, struct inode **ppinode)
{
	const char *s = pathname;
	char *t = filename;

	if (s == 0)
		return -1;

	if (*s == '/')
		s++;

	/* check each character */
	while (*s) {		
		if (*s == '/')
			return -1;
		
		*t++ = *s++;
		
		/* if filename is too long, just truncate it */
		if (t - filename >= MAX_FILENAME_LEN)
			break;
	}
	
	*t = 0;

	*ppinode = root_inode;

	return 0;
}


/*
 * alloc_imap_bit - 在inode位图中寻找空闲的一位并进行分配
 *
 * @dev 文件系统对应的设备
 * @return 分配的一位对应的inode编号（即在inode位图扇区中的位编号）
 *
 */
int alloc_imap_bit(int dev)
{
	int inode_num = 0;
	int i, j, k;

	int imap_blk0_nr = 1;            /* 1 super block */
	struct super_block *sb = get_super_block(dev);

	for (i = 0; i < sb->size_imap_sects; i++) {
		if(sd_read_sector(buf, imap_blk0_nr + i, 1) == 0)	   //读入扇区
			printk("inode索引位图数据读入成功！\r\n"); 
		else
			printk("inode索引位图数据读入失败！\r\n"); 	

		for (j = 0; j < SECTOR_SIZE; j++) {
			/* skip `11111111' bytes */
			if (buf[j] == 0xFF)
				continue;
			
			/* skip `1' bits */
			for (k = 0; ((buf[j] >> k) & 1) != 0; k++);
				
			/* i: sector index; j: byte index; k: bit index */
			inode_num = (i * SECTOR_SIZE + j) * 8 + k;
			buf[j] |= (1 << k);
			
			/* write the bit to imap */
			if(sd_write_sector(buf, imap_blk0_nr + i, 1)==0)	
				printk("inode索引位图数据增加成功！\r\n"); 
			else
				printk("inode索引位图数据增加失败！\r\n"); 
			
			break;
		}

		return inode_num;
	}

	/* no free bit in imap */
	printk("inode-map is probably full!\r\n");

	return 0;
}


/*
 * alloc_smap_bit - 在sector位图中寻找空闲的一位并进行分配
 *
 * @dev 文件系统对应的设备
 * @nr_sects_to_alloc 
 *
 * @return 分配的一位对应的sector编号（即在sector位图扇区中的位编号）
 *
 */
int alloc_smap_bit(int dev, int nr_sects_to_alloc)
{
	/* int nr_sects_to_alloc = NR_DEFAULT_FILE_SECTS; */

	int i; /* sector index */
	int j; /* byte index */
	int k; /* bit index */

	struct super_block * sb = get_super_block(dev);

	int smap_blk0_num = 1 + sb->size_imap_sects;
	int free_sect_num = 0;

	for (i = 0; i < sb->size_smap_sects; i++) 
	{ 
		/* smap_blk0_nr + i :current sect nr. */
		if(sd_read_sector(buf, smap_blk0_num + i, 1) == 0)	   //读入扇区
			printk("扇区索引位图数据读入成功！\r\n"); 
		else
			printk("扇区索引位图数据读入失败！\r\n"); 	

		/* byte offset in current sect */
		for (j = 0; j < SECTOR_SIZE && nr_sects_to_alloc > 0; j++) 
		{
			k = 0;
			if (!free_sect_num) 
			{
				/* loop until a free bit is found */
				if (buf[j] == 0xFF)
					continue;
				for (; ((buf[j] >> k) & 1) != 0; k++);
				free_sect_num = (i * SECTOR_SIZE + j) * 8 + k - 1 + sb->num_1st_sect;
			}

			for (; k < 8; k++) 
			{ /* repeat till enough bits are set */
				buf[j] |= (1 << k);
				if (--nr_sects_to_alloc == 0)
					break;
			}
		}
		
		/* free bit found, write the bits to smap */
		if (free_sect_num) 
		{
			if(sd_write_sector(buf, smap_blk0_num + i, 1)==0)	
				printk("扇区索引位图数据增加成功！\r\n"); 
			else
				printk("扇区索引位图数据增加失败！\r\n"); 
		}
		if (nr_sects_to_alloc == 0)
			break;
	}

	return free_sect_num;
}


/*
 * new_inode - 初始化一个新的inode结构体，并同步到存储设备dev
 *
 * @dev 文件系统对应的设备
 * @inode_num inode编号
 * @start_sect 起始扇区
 *
 * @return 新的inode结构体指针
 *
 */
struct inode *new_inode(int dev, int inode_num, int start_sect)
{
	struct inode *new_inode = get_inode(dev, inode_num);

	new_inode->i_mode = I_REGULAR;
	new_inode->i_size = 0;
	new_inode->i_start_sect = start_sect;
	new_inode->i_total_sects = SIZE_DEFAULT_FILE_SECTS;

	new_inode->i_dev = dev;
	new_inode->i_cnt = 1;
	new_inode->i_num = inode_num;

	/* 写回到存储设备上的inode结构体数组区域 */
	sync_inode(new_inode);

	return new_inode;
}


/*
 * new_dir_entry - 分配并初始化一个dir_entry结构体
 *
 * @dir_inode 目录的inode结构体指针（此处为根目录）
 * @inode_nr inode序号
 * @filename 文件名
 *
 */
void new_dir_entry(struct inode *dir_inode, int inode_nr, char *filename)
{
	/* write the dir_entry */
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE) / SECTOR_SIZE;
	int nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE; /**
						                                      * including unused slots
															  * (the file has been
															  * deleted but the slot
															  * is still there)
															  */
	int i, j;															  
	int m = 0;
	struct dir_entry *pde;
	struct dir_entry *new_de = 0;
	
	for (i = 0; i < nr_dir_blks; i++) {
		if (sd_read_sector(buf, dir_blk0_nr + i, 1) == 0)	   //读入扇区
			printk("目录数据读入成功！\r\n"); 
		else
			printk("目录数据读入失败！\r\n"); 	

		pde = (struct dir_entry *)buf;
		
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;

			/* it's a free slot */
			if (pde->inode_num == 0) { 
				new_de = pde;
				break;
			}
		}
		if (m > nr_dir_entries ||/* all entries have been iterated or */
		    new_de)              /* free slot is found */
			break;
	}

	/* reached the end of the dir */
	if (!new_de) { 
		new_de = pde;
		dir_inode->i_size += DIR_ENTRY_SIZE;
	}
	
	new_de->inode_num = inode_nr;
	strcpy(new_de->name, filename);

	/* write dir block -- ROOT dir block */
	if(sd_write_sector(buf, dir_blk0_nr + i, 1)==0)	
		printk("目录数据增加成功！\r\n"); 
	else
		printk("目录数据增加失败！\r\n"); 

	/* update dir inode */
	sync_inode(dir_inode);
}

/*
 * search_file - 
 *
 * @path 
 *
 */
int search_file(const char *path)
{
	int a;
	int i, j;
	
	int dir_blk0_nr ;
	int nr_dir_blks;
	int nr_dir_entries;
	
	int m = 0;
	struct dir_entry *pde;
	
	struct inode *dir_inode;
	char filename[MAX_FILENAME_LEN];
	
	for(a=0; a<MAX_FILENAME_LEN; a++)
	{
		filename[a] = 0;
	}
		
	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;

	if (filename[0] == 0)	/* path: "/" */
		return dir_inode->i_num;

	/**
	 * Search the dir for the file.
	 */
	//起始扇区号
	dir_blk0_nr = dir_inode->i_start_sect;
	//占用的扇区数
	nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	//
	nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE; /**
					       * including unused slots
					       * (the file has been deleted
					       * but the slot is still there)
					       */
	
	for (i = 0; i < nr_dir_blks; i++) 
	{
		if(sd_read_sector(buf, dir_blk0_nr + i, 1) == 0)	   //读入扇区
			printk("search函数: 目录数据读入成功！\r\n"); 
		else
			printk("search函数: 目录数据读入失败！\r\n"); 	
		
		pde = (struct dir_entry *)buf;
		
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) 
		{
			if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
				return pde->inode_num;
			if (++m > nr_dir_entries)
				break;
		}
		if (m > nr_dir_entries) /* all entries have been iterated */
			break;
	}

	/* file not found */
	return 0;
}


/*
 * create_file - 创建并初始化一个新的文件
 *
 * @path 文件路径
 * @flags 文件打开权限
 *
 * @return 新文件对应的inode结构体指针
 *
 */
struct inode *create_file(const char *path, int flags)
{
	char filename[MAX_FILENAME_LEN];
	int inode_num;
	int free_sect_num;
	struct inode *dir_inode;
	struct inode *newinode;
	
	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;

	printk("dir_inode->i_mode: %d\r\n", dir_inode->i_mode);
	printk("dir_inode->i_size: %d\r\n", dir_inode->i_size);
	printk("dir_inode->i_start_sect: %d\r\n", dir_inode->i_start_sect);
	printk("dir_inode->i_total_sects: %d\r\n", dir_inode->i_total_sects);
	printk("dir_inode->i_dev: %d\r\n", dir_inode->i_dev);
	
	//在inode位图中寻找一个空闲的inode号
	inode_num = alloc_imap_bit(dir_inode->i_dev);	
	printk("inode_num: %d\r\n", inode_num);
	
	//在sector位图中寻找一个空闲的sector号
	free_sect_num = alloc_smap_bit(dir_inode->i_dev, SIZE_DEFAULT_FILE_SECTS);	
	printk("free_sect_num: %d\r\n", free_sect_num);

	/* 分配并初始化一个新的inode结构体并写到存储设备中 */
	newinode = new_inode(dir_inode->i_dev, inode_num, free_sect_num);

	/* 分配并初始化一个dir_entry结构体 */
	new_dir_entry(dir_inode, newinode->i_num, filename);

	return newinode;
}


/*
 * simple_ext2_open - 
 *
 * @path 
 *
 */
int simple_ext2_open(const char *path_name, int flags)
{
	int fd = -1;
	int inode_num;
	int i;
	struct inode *pin = NULL;
	int imode;
	
	char filename[MAX_FILENAME_LEN];
	struct inode * dir_inode;
	
	/* 在进程中寻找一个空闲的fd */
	for (i = 0; i < NUM_FILES; i++) 
	{
		if (current->filp[i] == NULL) 
		{
			fd = i;
			break;
		}
	}
	
//	printk("fd = %d", fd);
	
	if ((fd < 0) || (fd >= NUM_FILES))
		printk("\r\n已超过此进程能打开的文件最大值！\r\n");

	/* find a free slot in f_desc_table[] */
	for (i = 0; i < NUM_FILE_DESC; i++)
		if (fd_table[i].fd_inode_ptr == 0)
			break;
		
	if (i >= NUM_FILE_DESC)
		printk("\r\n文件描述符已用尽！\r\n");

	
	inode_num = search_file(path_name);
	
//	printk("inode_num = %d\r\n", inode_num);
	
//	if (flags & O_CREAT) 
	if (flags == O_CREAT) 
	{
		if (inode_num) 
		{
			printk("文件已存在！\r\n");
			return -1;
		}
		else 
		{
			pin = create_file(path_name, flags);
		}
	}
	else //if (flags == O_RDWR) 
	{
		if (strip_path(filename, path_name, &dir_inode) != 0)
			return -1;
		pin = get_inode(dir_inode->i_dev, inode_num);
	}
	
	if (pin) 
	{
		/* connects proc with file_descriptor */
		current->filp[fd] = &fd_table[i];

		/* connects file_descriptor with inode */
		fd_table[i].fd_inode_ptr = pin;

		fd_table[i].fd_mode = flags;
		/* f_desc_table[i].fd_cnt = 1; */
		fd_table[i].fd_pos = 0;

		imode = pin->i_mode; //& I_TYPE_MASK;

		//如果是字符设备文件
		if (imode == I_CHAR_SPECIAL) 
		{
//			MESSAGE driver_msg;
//			driver_msg.type = DEV_OPEN;
//			int dev = pin->i_start_sect;
//			driver_msg.DEVICE = MINOR(dev);
//			assert(MAJOR(dev) == 4);
//			assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
//			send_recv(BOTH,
//				  dd_map[MAJOR(dev)].driver_nr,
//				  &driver_msg);
		}
		//如果是目录文件
		else if (imode == I_DIRECTORY) 
		{
			//assert(pin->i_num == ROOT_INODE);
		}
		else {
			//assert(pin->i_mode == I_REGULAR);
		}
	}
	else
	{
		return -1;
	}

	return fd;	
}

void init_fs(void)
{
	struct super_block *sb = super_block;

	memset(fd_table, 0, sizeof(struct file) * NUM_FILE_DESC);
	memset(inode_table, 0, sizeof(struct inode) * NUM_INODE);
	memset(super_block, 0, sizeof(struct super_block) * NUM_SUPER_BLOCK);

	/* 初始化全局变量super_block[]中所有的sb_dev参数为NO_DEV */
	for (; sb < &super_block[NUM_SUPER_BLOCK]; sb++)
		sb->sb_dev = NO_DEV;

	/* 生成文件系统头部信息，并写到存储设备中 */
	create_fs();

	/* 从存储设备中读入超级块到super_block[]全局变量 */
	read_super_block(ROOT_DEV);

	/* 从super_block[]全局变量中获取指定的超级块 */
	sb = get_super_block(ROOT_DEV);

	/* 打印获取到的指定的超级块信息 */
	printk("文件魔数: %d\r\n", sb->magic);
	
	printk("sb->total_inodes = %d\r\n",sb->total_inodes);
	printk("sb->total_sects = %d\r\n",sb->total_sects);				
	printk("sb->size_inode_sects = %d\r\n",sb->size_inode_sects);
	printk("sb->size_imap_sects = %d\r\n",sb->size_imap_sects);
	printk("sb->size_smap_sects = %d\r\n",sb->size_smap_sects);
	printk("sb->num_1st_sect = %d\r\n",sb->num_1st_sect);				
	printk("sb->inode_size = %d\r\n",sb->inode_size);
	printk("sb->dir_entry_size = %d\r\n",sb->dir_entry_size);
	printk("sb->num_root_inode = %d\r\n",sb->num_root_inode);		

	/* 获取inode */
	root_inode = get_inode(ROOT_DEV, ROOT_INODE);

	/* 打印信息 */
	printk("root_inode->i_mode: %d\r\n", root_inode->i_mode);
	printk("root_inode->i_size: %d\r\n", root_inode->i_size);
	printk("root_inode->i_start_sect: %d\r\n", root_inode->i_start_sect);
	printk("root_inode->i_total_sects: %d\r\n", root_inode->i_total_sects);
}

int simple_ext2_close(int fd)
{
	put_inode(current->filp[fd]->fd_inode_ptr);
	current->filp[fd]->fd_inode_ptr = 0;
	current->filp[fd] = 0;

	return 0;
}

//缓冲区数据复制指定字节: buf --> buffer
void phys_copy(u8 *buffer, u8 *buf, int bytes)  
{
	int i;
	
	for(i=0; i<bytes; i++)
	{
		buffer[i] = buf[i];	
	}
}

int min(int a, int b)
{
	return a >= b ? b : a;
}

int do_rdwt(int fd, void *buffer, int count, int is_read_write)   //1:读    0:写
{	
	//文件对应的inode指针
	struct inode *pin = current->filp[fd]->fd_inode_ptr;
	
	//文件当前访问位置
	int pos = current->filp[fd]->fd_pos + pin->i_size;
	
	//文件类型
	int imode = pin->i_mode; //& I_TYPE_MASK;
	
	int pos_end;
	
	int off;
	int rw_sect_min;
	int rw_sect_max;
	int chunk;
	
	int bytes_rw = 0;
	int bytes_left = count;
	int i;
	int bytes;
	
//	if (!(current->filp[fd]->fd_mode & O_RDWR))
//		return 0;

	if (imode == I_CHAR_SPECIAL) 
	{
//		int t = fs_msg.type == READ ? DEV_READ : DEV_WRITE;
//		fs_msg.type = t;

//		int dev = pin->i_start_sect;
//		assert(MAJOR(dev) == 4);

//		fs_msg.DEVICE	= MINOR(dev);
//		fs_msg.BUF	= buf;
//		fs_msg.CNT	= len;
//		fs_msg.PROC_NR	= src;
//		assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
//		send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &fs_msg);
//		assert(fs_msg.CNT == len);

//		return fs_msg.CNT;
	}
	else 
	{	
		if (is_read_write)  //read
			//读位置的最大值
			pos_end = min(pos + count, pin->i_size);
		else		        //write
			//写位置的最大值
			pos_end = min(pos + count, pin->i_total_sects * SECTOR_SIZE);

		//读写位置在一个扇区内的偏移量
		off = pos % SECTOR_SIZE;

		//读写扇区的最小值
		rw_sect_min = pin->i_start_sect + (pos >> SECTOR_SIZE_SHIFT);
		
		//读写扇区的最大值
		rw_sect_max = pin->i_start_sect + (pos_end >> SECTOR_SIZE_SHIFT);

		//每次的读写单位
		chunk = min(rw_sect_max - rw_sect_min + 1, FSBUF_SIZE >> SECTOR_SIZE_SHIFT);

		for (i = rw_sect_min; i <= rw_sect_max; i += chunk) 
		{
			/* read/write this amount of bytes every time */
			bytes = min(bytes_left, chunk * SECTOR_SIZE - off);
			
			if(sd_read_sector(buf, i, chunk) == 0)	   
				printk("文件待读数据读入成功！\r\n"); 
			else
				printk("文件待读数据读入失败！\r\n"); 	
			
//			rw_sector(DEV_READ,
//				  pin->i_dev,
//				  i * SECTOR_SIZE,
//				  chunk * SECTOR_SIZE,
//				  TASK_FS,
//				  fsbuf);

			if (is_read_write)     /* READ */   //全局缓冲区buf --> 文件缓冲区buffer
			{
				phys_copy((u8 *)buffer + bytes_rw, (u8 *)buf , bytes);
			}
			else      /* WRITE */
			{	
				phys_copy((u8 *)buf + off, (u8 *)buffer + bytes_rw, bytes);
		
				if(sd_write_sector(buf, i, chunk) == 0)	  
					printk("do_rdwt函数: 文件数据读入成功！\r\n"); 
				else
					printk("do_rdwt函数: 文件数据读入失败！\r\n"); 	
				
//				rw_sector(DEV_WRITE,
//					  pin->i_dev,
//					  i * SECTOR_SIZE,
//					  chunk * SECTOR_SIZE,
//					  TASK_FS,
//					  fsbuf);
			}
			off = 0;
			bytes_rw += bytes;
			current->filp[fd]->fd_pos += bytes;
			bytes_left -= bytes;
		}

		if (current->filp[fd]->fd_pos > pin->i_size) 
		{
			/* update inode::size */
			pin->i_size = current->filp[fd]->fd_pos;
			/* write the updated i-node back to disk */
			sync_inode(pin);
		}

		return bytes_rw;
	}
}

int simple_ext2_write(int fd, char *buffer, int count)
{
	return do_rdwt(fd, buffer, count, 0); 
}

int simple_ext2_read(int fd, char *buffer, int count)
{
	return do_rdwt(fd, buffer, count, 1); 
}

void show_dir_entry(void)
{
	int y = 31;
	struct dir_entry *de;
	
	if(sd_read_sector(buf, root_inode->i_start_sect, 1) == 0)	   //读入扇区
		printk("show_dir_entry函数: 目录数据读入成功！\r\n"); 
	else
		printk("show_dir_entry函数: 目录数据读入失败！\r\n"); 	
	
	de = (struct dir_entry *)buf;
	
	while(strcmp(de->name,""))
	{
		printk("文件名： %s\r\n", de->name);
//		LCD_ShowString(122,y,110,16,16, de->name);
		y += 20;
		de++;
	}
}

struct file_operations simple_ext2_fops {
	.open = simple_ext2_open,
	.read = simple_ext2_read,
	.write = simple_ext2_write,
	.release = simple_ext2_close,
};

// struct super_block simple_ext2_super_block = {
// 	.namei = simple_simple_ext2_namei,
// 	.get_daddr = simple_ext2_get_daddr,
// 	.name = "simple_ext2",
// };

static struct file_system_type simple_ext2_fs_type = {
	.name		= "simple_ext2",
	.mount		= simple_ext2_mount,
	.fops		= simple_ext2_fops,
};

int simple_ext2_init(void)
{
	int ret;
	
	ret = register_filesystem(&simple_ext2_fs_type);
	// simple_ext2_super_block.device = storage[RAMDISK];
	
	return ret;
}



#include <fs.h>
#include <string.h>
#include <romfs.h>
#include <sd.h>
#include <sched.h>

#define NULL ((void *)0)

static char *bmap(char *tmp,char *dir){
	unsigned int n;
	char *p=strchr(dir,'/');
	if(!p){
		strcpy(tmp,dir);
		return NULL;
	}
	n=p-dir;
	n=(n>ROMFS_MAX_FILE_NAME)?ROMFS_MAX_FILE_NAME:n;
	strncpy(tmp,dir,n);
	return p+1;
}

static char *get_the_file_name(char *p,char *name){
	char *tmp=p;
	int index;
	for(index=0;*tmp;tmp++){
		if(*tmp=='/'){
			index=0;
			continue;
		}else{
			name[index]=*tmp;
			index++;
		}
	}
	name[index]='\0';
	return name;
}

unsigned int romfs_get_daddr(struct inode *node){
	int name_size=strlen(node->name);

	printk("enter romfs_get_daddr()\r\n");
	printk("romfs_get_daddr(): name_size = %d\r\n", name_size);

	return romfs_get_file_data_offset(node->daddr,name_size);
}

struct romfs_inode romfs_inode;
struct romfs_super_block romfs_super_block;

void simple_romfs_namei(struct inode *inode, struct super_block *block, char *dir)
{
	struct romfs_inode *p = &romfs_inode;
	struct romfs_super_block *p_romfs_super_block = &romfs_super_block;

	unsigned int tmp, next, num;
	char name[ROMFS_MAX_FILE_NAME], fname[ROMFS_MAX_FILE_NAME];
	//unsigned int max_p_size = (ROMFS_MAX_FILE_NAME + sizeof(struct romfs_inode));

//	max_p_size = max_p_size>(block->device->sector_size)?max_p_size:(block->device->sector_size);
	//max_p_size  = 512;

	printk("enter simple_romfs_namei()\r\n");
	
	get_the_file_name(dir, fname);

//	if ((p = (struct romfs_inode *)kmalloc(max_p_size,0))==NULL){
//		goto ERR_OUT_NULL;
//	}

	dir = bmap(name, dir);

	printk("simple_romfs_namei(): dir = %s, fname = %s, name = %s\r\n", dir, fname, name);

//	if(block->device->dout(block->device,p,0,block->device->sector_size))
//		goto ERR_OUT_KMALLOC;
	sd_read_sector((unsigned int *)p_romfs_super_block, 0, 1);
//	sd_read_sector(buffer, 0, 1);
#if 0
	printk("simple_romfs_namei(): *(char *)p = %x\r\n", *((char *)p));
	printk("simple_romfs_namei(): p->next = %x\r\n", p->next);
	printk("simple_romfs_namei(): p->spec = %x\r\n", p->spec);
	printk("simple_romfs_namei(): p->size = %x\r\n", p->size);
	printk("simple_romfs_namei(): p->checksum = %x\r\n", p->checksum);
	printk("simple_romfs_namei(): p->name = %s\r\n", p->name);
#endif
	next = romfs_get_first_file_header(p_romfs_super_block);
//	printk("simple_romfs_namei(): next = %x\r\n", next);
//	tmp=(be32_to_le32(next))&ROMFS_NEXT_MASK;

	while (1) {
		printk("\r\nsimple_romfs_namei(): begin while\r\n");
//		delay();
		tmp = (be32_to_le32(next)) & ROMFS_NEXT_MASK;
//		tmp = next;
		printk("simple_romfs_namei(): tmp = %x(%d), next = %x(%d)\r\n", tmp, tmp, next, next);
//		if(tmp>=block->device->storage_size)
//			goto ERR_OUT_KMALLOC;
		if (tmp != 0) {
//			if(block->device->dout(block->device,p,tmp,block->device->sector_size)){
//				goto ERR_OUT_KMALLOC;
//			}
			sd_read_sector((unsigned int *)p, tmp / 512, 1);
			p = (struct romfs_inode *)((char *)p + tmp - (tmp / 512) * 512);
			printk("simple_romfs_namei(): p->next = %x\r\n", p->next);
			printk("simple_romfs_namei(): p->spec = %x\r\n", p->spec);
			printk("simple_romfs_namei(): p->size = %x\r\n", p->size);
			printk("simple_romfs_namei(): p->checksum = %x\r\n", p->checksum);
			printk("simple_romfs_namei(): p->name = %s\r\n", p->name);

			if (!strcmp(p->name,name)) {
				if (!strcmp(name,fname)) {
					printk("simple_romfs_namei(): go to found\r\n");
					goto FOUND;
				} else {
					dir = bmap(name, dir);					
					printk("simple_romfs_namei(): dir = %s, fname = %s, name = %s\r\n", dir, fname, name);
					next = p->spec;	
					if (dir == NULL) {
						printk("simple_romfs_namei(): go to founddir\r\n");
						goto FOUNDDIR;
					}
				}
			} else {
				next=p->next;	
//				next=(be32_to_le32(p->next))&ROMFS_NEXT_MASK;	
			}
		} else {
			goto ERR_OUT_KMALLOC;
		}
		//		tmp = next&ROMFS_NEXT_MASK;
	}

FOUNDDIR:
	while (1) {
		printk("simple_romfs_namei(): enter founddir\r\n");
		tmp = (be32_to_le32(next)) & ROMFS_NEXT_MASK;

		if (tmp != 0) {
//			if(block->device->dout(block->device,p,tmp,block->device->sector_size)){
//				goto ERR_OUT_KMALLOC;
//			}
			sd_read_sector(p, tmp / 512, 1);
			p = (struct romfs_inode *)((char *)p + tmp / 512 - 1 +tmp);

			if (!strcmp(p->name,name)) {
				goto FOUND;
			} else {
				next=p->next;
			}
		} else {
			goto ERR_OUT_KMALLOC;
		}
	}

FOUND:
	printk("simple_romfs_namei(): enter found\r\n");

//	if((inode = (struct inode *)kmalloc(sizeof(struct inode), 0))==NULL){
//		goto ERR_OUT_KMALLOC;
//	}
	num = strlen(p->name);

//	if((inode->name=(char *)kmalloc(num,0))==NULL){
//		goto ERR_OUT_KMEM_CACHE_ALLOC;
//	}
	strcpy(inode->name,p->name);	
	inode->dsize=be32_to_le32(p->size);
	inode->daddr=tmp;	
	
//	printk("simple_romfs_namei(): p->name = %s\r\n", p->name);
//	inode->super=&romfs_super_block;
//	printk("simple_romfs_namei(): num = %d\r\n", num);
//	kfree(p);		
//	printk("simple_romfs_namei(): inode->name = %s\r\n", inode->name);
//	return ;
//	printk("simple_romfs_namei(): inode->dsize = %d\r\n", inode->dsize);

//	printk("simple_romfs_namei(): inode->daddr = %d\r\n", inode->daddr);

ERR_OUT_KMEM_CACHE_ALLOC:
	// kfree(inode);
ERR_OUT_KMALLOC:
	// kfree(p);
ERR_OUT_NULL:
	;//return NULL;
}

struct inode open_romfs_inode;
struct file romfs_file;

int romfs_open(const char *path_name, int flags)
{
	int i, fd = -1;
		
	/* 在进程中寻找一个空闲的fd */
	for (i = 0; i < 64; i++) {
		if (current->filp[i] == NULL) {
			fd = i;
			break;
		}
	}

	/* 通过文件路径名称得到inode结构�?*/
	simple_romfs_namei(&open_romfs_inode, NULL, path_name);

	current->filp[fd] = &romfs_file;

	romfs_file.fd_inode_ptr = &open_romfs_inode;
	romfs_file.fd_mode = flags;
	romfs_file.fd_pos = 0;

	return fd;
}

int romfs_read(int fd, char *buffer, int count)
{
	int i;
	unsigned int file_data_addr;
	//文件对应的inode指针
	struct inode *pinode = current->filp[fd]->fd_inode_ptr;
	//文件当前访问位置
	//int pos = current->filp[fd]->fd_pos + pinode->i_size;
//	char file_buffer[] = {0};
	char *file_buffer = (char *)0x33600000;

	printk("enter read()\r\n");

//	node=fs_type[ROMFS]->namei(fs_type[ROMFS],"number.txt");
//	fs_type[ROMFS]->device->dout(fs_type[ROMFS]->device,buf,fs_type[ROMFS]->get_daddr(node),node->dsize);
	
	file_data_addr = romfs_get_daddr(pinode);
	printk("read(): file_data_addr = %d\r\n", file_data_addr);
	printk("read(): node.dsize = %d\r\n", pinode->dsize);

	sd_read_sector(file_buffer, file_data_addr/512, pinode->dsize/512 + 2);
	//buffer = file_buffer + file_data_addr - (file_data_addr / 512) * 512;
	memcpy(buffer, file_buffer + file_data_addr - (file_data_addr / 512) * 512, pinode->dsize);
/*
	for(i=0;i<pinode->dsize;i++){
		printk("%x ",buffer[i]);
	}
	printk("\r\n");
	for(i=0;i<pinode->dsize;i++){
		printk("%c",buffer[i]);
	}
	printk("\r\n");
*/
	return pinode->dsize;
}

int romfs_close(int fd)
{
	current->filp[fd]->fd_inode_ptr = 0;
	current->filp[fd] = 0;

	return 0;
}

struct romfs_inode show_dir_romfs_inode;
struct romfs_super_block show_dir_romfs_super_block;

void show_dir_entry(void)
{
	struct romfs_inode *p = &show_dir_romfs_inode;
	struct romfs_super_block *p_romfs_super_block = &show_dir_romfs_super_block;

	unsigned int tmp,next,num;
	char name[ROMFS_MAX_FILE_NAME], fname[ROMFS_MAX_FILE_NAME];
	//unsigned int max_p_size = (ROMFS_MAX_FILE_NAME + sizeof(struct romfs_inode));

	// max_p_size = max_p_size>(block->device->sector_size)?max_p_size:(block->device->sector_size);
	//max_p_size  = 512;

	printk("enter show_dir_entry()\r\n");
	
	sd_read_sector((unsigned int *)p_romfs_super_block, 0, 1);

//	printk("show_dir_entry(): *(char *)p = %x\r\n", *((char *)p));
//	printk("show_dir_entry(): p->word0 = %x\r\n", p_romfs_super_block->word0);
//	printk("show_dir_entry(): p->word1 = %x\r\n", p_romfs_super_block->word1);
//	printk("show_dir_entry(): p->size = %x\r\n", p_romfs_super_block->size);
//	printk("show_dir_entry(): p->checksum = %x\r\n", p_romfs_super_block->checksum);
//	printk("show_dir_entry(): p->name = %s\r\n", p_romfs_super_block->name);
//	printk("\r\n");
	
	next = romfs_get_first_file_header(p_romfs_super_block);
//	printk("show_dir_entry(): next = %x\r\n", next);

	while (1) {
//		printk("\r\show_dir_entry(): begin while\r\n");
		
		tmp = (be32_to_le32(next)) & ROMFS_NEXT_MASK;
//		printk("show_dir_entry(): tmp = %x(%d), next = %x(%d)\r\n", tmp, tmp, next, next);

		if (tmp != 0) {
			sd_read_sector((unsigned int *)p, tmp / 512, 1);
			p = (struct romfs_inode *)((char *)p + tmp - (tmp / 512) * 512);
		
//			printk("show_dir_entry(): p->next = %x\r\n", p->next);
//			printk("show_dir_entry(): p->spec = %x\r\n", p->spec);
//			printk("show_dir_entry(): p->size = %x\r\n", p->size);
//			printk("show_dir_entry(): p->checksum = %x\r\n", p->checksum);
			printk("show_dir_entry(): p->name = %s\r\n", p->name);
			printk("\r\n");

			next=p->next;	
		} else {
			goto ERR_OUT_KMALLOC;
		}
	}

ERR_OUT_KMEM_CACHE_ALLOC:
	// kfree(inode);
ERR_OUT_KMALLOC:
	// kfree(p);
ERR_OUT_NULL:
	return NULL;
}

void romfs_mount()
{

}
struct file_operations romfs_fops {
	.open = romfs_open,
	.read = romfs_read,
	.release = romfs_close,
}

// struct super_block romfs_super_block = {
// 	.namei = simple_romfs_namei,
// 	.get_daddr = romfs_get_daddr,
// 	.name = "romfs",
// };

static struct file_system_type romfs_fs_type = {
	.name		= "romfs",
	.mount		= romfs_mount,
	.fops 		= romfs_fops,
};

int romfs_init(void)
{
	int ret;
	
	ret = register_file_system(&romfs_fs_type);
	// romfs_super_block.device = storage[RAMDISK];
	
	return ret;
}

#include <fs.h>
//#include "storage.h"
#include <string.h>
#include <romfs.h>
#include <sd.h>

#define NULL (void *)0


struct romfs_super_block {
	unsigned int word0;
	unsigned int word1;
	unsigned int size;
	unsigned int checksum;
	char name[0];
};

struct romfs_inode {
	unsigned int next;
	unsigned int spec;
	unsigned int size;
	unsigned int checksum;
	char name[0];
};

//struct super_block romfs_super_block;

#define ROMFS_MAX_FILE_NAME	(128)
#define ROMFS_NAME_ALIGN_SIZE	(16)
#define ROMFS_SUPER_UP_MARGIN	(16)
#define ROMFS_NAME_MASK	(~(ROMFS_NAME_ALIGN_SIZE-1))
#define ROMFS_NEXT_MASK 0xfffffff0

#define romfs_get_first_file_header(p)	((((strlen(((struct romfs_inode *)(p))->name)+ROMFS_NAME_ALIGN_SIZE+ROMFS_SUPER_UP_MARGIN))&ROMFS_NAME_MASK)<<24)
#define romfs_get_file_data_offset(p,num)	(((((num)+ROMFS_NAME_ALIGN_SIZE)&ROMFS_NAME_MASK)+ROMFS_SUPER_UP_MARGIN+(p)))



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
//unsigned int buffer[10];
struct romfs_inode temp_romfs_inode;

void simple_romfs_namei(struct inode *inode, struct super_block *block, char *dir)
{
//	struct inode tmp_inode;
//	struct inode *inode = &tmp_inode;
	struct romfs_inode *p = &temp_romfs_inode;

	unsigned int tmp,next,num;
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
	sd_read_sector((unsigned int *)p, 0, 1);
//	sd_read_sector(buffer, 0, 1);
#if 0
	printk("simple_romfs_namei(): *(char *)p = %x\r\n", *((char *)p));
	printk("simple_romfs_namei(): p->next = %x\r\n", p->next);
	printk("simple_romfs_namei(): p->spec = %x\r\n", p->spec);
	printk("simple_romfs_namei(): p->size = %x\r\n", p->size);
	printk("simple_romfs_namei(): p->checksum = %x\r\n", p->checksum);
	printk("simple_romfs_namei(): p->name = %s\r\n", p->name);
#endif
	next = romfs_get_first_file_header(p);
//	printk("simple_romfs_namei(): next = %x\r\n", next);
//	tmp=(be32_to_le32(next))&ROMFS_NEXT_MASK;

	while(1){
		printk("\r\nsimple_romfs_namei(): begin while\r\n");
//		delay();
		tmp=(be32_to_le32(next))&ROMFS_NEXT_MASK;
//		tmp = next;
		printk("simple_romfs_namei(): tmp = %x(%d), next = %x(%d)\r\n", tmp, tmp, next, next);
//		if(tmp>=block->device->storage_size)
//			goto ERR_OUT_KMALLOC;
		if(tmp!=0){
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

			if(!strcmp(p->name,name)){
				if(!strcmp(name,fname)){
					printk("simple_romfs_namei(): go to found\r\n");
					goto FOUND;
				}else{
					dir=bmap(name,dir);					
					printk("simple_romfs_namei(): dir = %s, fname = %s, name = %s\r\n", dir, fname, name);
					next=p->spec;	
					if(dir==NULL){
						printk("simple_romfs_namei(): go to founddir\r\n");
						goto FOUNDDIR;
					}
				}
			}else{
				next=p->next;	
//				next=(be32_to_le32(p->next))&ROMFS_NEXT_MASK;	
			}
		}else{
	//		goto ERR_OUT_KMALLOC;
		}
		
		//		tmp = next&ROMFS_NEXT_MASK;
	}
FOUNDDIR:
		/*
	while(1){
		printk("simple_romfs_namei(): enter founddir\r\n");
		tmp=(be32_to_le32(next))&ROMFS_NEXT_MASK;

		if(tmp!=0){
//			if(block->device->dout(block->device,p,tmp,block->device->sector_size)){
//				goto ERR_OUT_KMALLOC;
//			}
			sd_read_sector(p, tmp / 512, 1);
			p = (struct romfs_inode *)((char *)p + tmp / 512 - 1 +tmp);

			if(!strcmp(p->name,name)){
				goto FOUND;
			}else{
				next=p->next;
			}
		}else{
			goto ERR_OUT_KMALLOC;
		}
	}
*/
FOUND:
	printk("simple_romfs_namei(): enter found\r\n");

//	if((inode = (struct inode *)kmalloc(sizeof(struct inode), 0))==NULL){
//		goto ERR_OUT_KMALLOC;
//	}
	num=strlen(p->name);

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

//ERR_OUT_KMEM_CACHE_ALLOC:
//	kfree(inode);
//ERR_OUT_KMALLOC:
//	kfree(p);
//ERR_OUT_NULL:
//	return NULL;
//printk("simple_romfs_namei(): romfs_get_daddr(inode) = %d\r\n", romfs_get_daddr(inode));
return;
//#endif
}

//struct super_block romfs_super_block={
//	.namei=simple_romfs_namei,
//	.get_daddr=romfs_get_daddr,
//	.name="romfs",
//};


//int romfs_init(void){
//	int ret;
//	ret=register_file_system(&romfs_super_block,ROMFS);
//	romfs_super_block.device=storage[RAMDISK];
//	return ret;
//}

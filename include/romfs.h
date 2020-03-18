#ifndef __ROMFS_H__
#define __ROMFS_H__

#include <fs.h>

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

#define ROMFS_MAX_FILE_NAME	(128)
#define ROMFS_NAME_ALIGN_SIZE	(16)
#define ROMFS_SUPER_UP_MARGIN	(16)
#define ROMFS_NAME_MASK	(~(ROMFS_NAME_ALIGN_SIZE-1))
#define ROMFS_NEXT_MASK 0xfffffff0

#define romfs_get_first_file_header(p)	((((strlen(((struct romfs_inode *)(p))->name)+ROMFS_NAME_ALIGN_SIZE+ROMFS_SUPER_UP_MARGIN))&ROMFS_NAME_MASK)<<24)
#define romfs_get_file_data_offset(p,num)	(((((num)+ROMFS_NAME_ALIGN_SIZE)&ROMFS_NAME_MASK)+ROMFS_SUPER_UP_MARGIN+(p)))

#define be32_to_le32(x) \
	((unsigned int)( \
		(((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) | \
		(((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) | \
		(((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) | \
		(((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24) ))

//extern struct super_block romfs_super_block;
void simple_romfs_namei(struct inode *inode, struct super_block *block, char *dir);
unsigned int romfs_get_daddr(struct inode *node);

#endif

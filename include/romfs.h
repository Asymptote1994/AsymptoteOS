#ifndef __ROMFS_H__
#define __ROMFS_H__

#include <fs.h>

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

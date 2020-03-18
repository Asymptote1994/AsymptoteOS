#include <string.h>
#include <list.h>

#define MAX_SUPER_BLOCK 8
#define NULL (void *)0

// struct file_system_type *fs_type[MAX_SUPER_BLOCK];
struct list_head fs_list;

void vfs_init(void)
{
	list_init(&fs_list);

	romfs_init();
	simple_ext2_init();
}

int register_filesystem(struct file_system_type *fs_type)
{
	struct file_system_type *temp_fs_type;	
	
	list_for_each_entry(temp_fs_type, &fs_list, next) {
		if (strcmp(fs_type->name, temp_fs_type->name) == 0) {
			printk("VFS: %s has been registered!\r\n", fs_type->name);
			return -1;
		}		
	}
	
	list_add_tail(fs_type->next, fs_list.next);
	printk("VFS: %s registers successfully!\r\n", fs_type->name);

	return 0;
}

void unregister_file_system(struct file_system_type *fs_type)
{
	struct file_system_type *temp_fs_type;	
	
	list_for_each_entry(temp_fs_type, &fs_list, next) {
		if (strcmp(fs_type->name, temp_list->name) == 0) {
			printk("VFS: %s is not registered!\r\n", fs_type->name);
			return -1;
		}
	}
	
	list_del(fs_type->next, fs_list.next);
	printk("VFS: %s unregisters successfully!\r\n", fs_type->name);

	return 0;
}

// struct file_system_type *find_filesystem(char *fs_name)
// {

// }

struct file_system_type *temp_fs_type;	

int do_open(char *fs_name, const char *path_name, int flags)
{
	int fd;
	int flag = 0;

	list_for_each_entry(temp_fs_type, &fs_list, next) {
		if (strcmp(fs_name, temp_fs_type->name) == 0) {
			printk("VFS: %s is found!\r\n", fs_type->name);
			flag = 1;
			break;
		}
	}

	if (flag == 0) {
		printk("VFS: %s is not found! Please register %s first!\r\n", fs_type->name, fs_type->name);
		return NULL;
	}

	temp_fs_type = find_filesystem(fs_name);
	fd = temp_fs_type->fops->open(path_name, flags);

	return fd;
}

int do_read(int fd, char *buffer, int count)
{
	return temp_fs_type->fops->read(fd, buffer, count);
}

int do_write(int fd, char *buffer, int count)
{
	return temp_fs_type->fops->write(fd, buffer, count);
}

int do_close(int fd)
{
	return temp_fs_type->fops->release(fd);
}

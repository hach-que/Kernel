#include <system.h>
#include <vfs.h>

struct fs_node* fs_root = 0;	/* The root filesystem */

/* Read from an inode in the filesystem */
unsigned int read_fs(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer)
{
	if (node->read != 0)
		return node->read(node, offset, size, buffer);
	else
		return 0;
}

/* Write to an inode in the filesystem */
unsigned int write_fs(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer)
{
	if (node->write != 0)
		return node->write(node, offset, size, buffer);
	else
		return 0;
}

/* Opens an inode in the filesystem */
void open_fs(struct fs_node* node, unsigned char read, unsigned char write)
{
	if (node->open != 0)
		return node->open(node);
}

/* Closes an inode in the filesystem */
void close_fs(struct fs_node* node)
{
	if (node->close != 0)
		return node->close(node);
}

/* Reads from a directory in the filesystem */
struct dirent* readdir_fs(struct fs_node* node, unsigned int index)
{
	if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0)
		return node->readdir(node, index);
	else
		return 0;
}

/* Finds a child in a directory in the filesystem */
struct fs_node* finddir_fs(struct fs_node* node, char* name)
{
	if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0)
		return node->finddir(node, name);
	else
		return 0;
}

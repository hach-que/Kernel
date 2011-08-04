#ifndef __VFS_H
#define __VFS_H
#include <system.h>

#define FS_FILE		0x01
#define FS_DIRECTORY	0x02
#define FS_CHARDEVICE	0x03
#define FS_BLOCKDEVICE	0x04
#define FS_PIPE		0x05
#define FS_SYMLINK	0x06
#define FS_MOUNTPOINT	0x08

struct fs_node;

/* Function definitions for virtual filesystems to implement */
typedef unsigned int (*read_type_t)(struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*write_type_t)(struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent* (*readdir_type_t)(struct fs_node*, unsigned int);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*, char* name);

/* The definition of a filesystem inode */
typedef struct fs_node
{
	signed char name[128];	/* The filename */
	unsigned int inode;	/* The node ID */
	unsigned int flags;	/* The node type */
	unsigned int mask;	/* The permissions mask */
	unsigned int uid;	/* The owner ID */
	unsigned int gid;	/* The group ID */
	unsigned int length;	/* The length */
	unsigned int impl;	/* Implementation-specific number */
	read_type_t read;
	write_type_t write;
	open_type_t open;
	close_type_t close;
	readdir_type_t readdir;
	finddir_type_t finddir;
	struct fs_node* ptr;	/* Used by mountpoints and symlinks */
} fs_node_t;

/* The structure of a directory entry */
struct dirent
{
	char name[128];
	unsigned int inode;	/* The node ID */
};

/* Variable declaring the root filesystem */
extern struct fs_node* fs_root;

/* Functions called by the kernel to read and write to
 * the filesystem */
unsigned int read_fs(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int write_fs(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer);
void open_fs(struct fs_node* node, unsigned char read, unsigned char write);
void close_fs(struct fs_node* node);
struct dirent* readdir_fs(struct fs_node* node, unsigned int index);
struct fs_node* finddir_fs(struct fs_node* node, char* name);

#endif

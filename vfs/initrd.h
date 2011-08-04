#ifndef __VFS_INITRD_H
#define __VFS_INITRD_H
#include <vfs.h>

struct initrd_header
{
	unsigned int nfiles;	/* Number of files in the ramdisk */
};

struct initrd_file_header
{
	unsigned char magic;	/* Magic number, used for error checking */
	signed char name[64];	/* Filename */
	unsigned int offset;	/* Offset in the initrd that the file starts */
	unsigned int length;	/* Length of the file */
};

/* Initializing the initial ramdisk.  It gets passed the address of the
 * multiboot module and returns a completed filesystem node */
struct fs_node* initrd_install(unsigned int location);

#endif

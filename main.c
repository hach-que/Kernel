#include <system.h>

#include <multiboot.h>
#include <main.h>
#include <gdt.h>
#include <idt.h>
#include <isrs.h>
#include <tss.h>
#include <task.h>

#include <timer.h>
#include <scrn.h>
#include <page.h>

#include <vfs.h>
#include <initrd.h>

void* memcpy(void* dest, const void* src, int count)
{
	/* Add code here to copy 'count' bytes of data from 'src' to
	 * 'dest', finally return 'dest' */
	unsigned char* destC = (unsigned char*)dest;
	unsigned char* srcC = (unsigned char*)src;
	int i;
	for (i = 0; i < count; i++)
		destC[i] = srcC[i];
	return dest;
}

void* memset(void* dest, unsigned char val, int count)
{
	/* Add code here to set 'count' bytes in 'dest' to 'val'.
	 * Again, return 'dest' */
	unsigned char* destC = (unsigned char*)dest;
	int i;
	for (i = 0; i < count; i++)
		destC[i] = val;
	return dest;
}

unsigned short* memsetw(unsigned short* dest, unsigned short val, int count)
{
	/* Same as above, but this time, we're working with a 16-bit
	 * 'val' and dest pointer.  Your code can be an exact copy of
	 * the above, provided that your local variables if any, are
	 * unsigned short */
	int i;
	for (i = 0; i < count; i++)
		dest[i] = val;
	return dest;
}

/* We will use this later on for reading from the I/O ports to get data
 * from devices such as the keyboard.  We are using what is called
 * 'inline assembly' in these routines to actually do the work */
unsigned char inportb(unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

/* We will use this to write to I/O ports to send bytes to devices.  This
 * will be used in the next tutorial for changing the textmode cursor
 * position.  Again, we use some inline assembly for the stuff that simply
 * cannot be done in C */
void outportb(unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

/* The definition for the entry point of the built-in userland
 * application */
extern void entry();

/* A global variable for storing the stack position */
addr initial_esp;

/* This is a very simple main() function.  All it does is sit in an
 * infinite loop.  This will be like our 'idle' loop */
void _main(struct multiboot_info* mbt, addr stack)
{
	/* Store the stack position in a global variable */
	initial_esp = stack;

	/* Setup the very core components of the kernel / CPU operation */
	unsigned char itoa_buffer[256];
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	init_video();

	/* Enable IRQs */
	asm volatile("sti");

	/* Install the initrd filesystem before memory management
	 * so that it doesn't get overwritten before we read it */
	ASSERT(mbt->mods_count > 0);
	puts("Initializing initrd... ");
	fs_root = initrd_install(*((addr*)(mbt->mods_addr)));
	puts("done.\n");

	/* Install memory and task management */
	kmem_install(mbt);
	page_install();
	task_install();

	/* Install and handle various devices in the system */
	puts("Enabling devices... ");
	timer_install();
	kb_install();
	puts("done.\n");

	/* Test the task management system */
	puts("Forking kernel...\n");
	int ret = fork();
	puts("fork() returned ");
	puts(itoa(ret, itoa_buffer, 10));
	puts(", and getpid() returned ");
	puts(itoa(getpid(), itoa_buffer, 10));
	puts("\n==========================================\n");

	/* The next section of code is not re-entrant (because the initrd
	 * VFS uses global variables which will be shared between both
	 * processes), so make sure we aren't interrupted during listing
	 * the contents of /. */
	asm volatile("cli");

	/* List the contents of the initrd */
	int i = 0;
	struct dirent* node = 0;
	while ((node = readdir_fs(fs_root, i)) != 0)
	{
		puts("Found file ");
		puts(node->name);
		struct fs_node* fsnode = finddir_fs(fs_root, node->name);

		if ((fsnode->flags & 0x7) == FS_DIRECTORY)
			puts("\n\t(directory)\n");
		else
		{
			puts("\n\t contents: \"");
			char buf[256];
			unsigned int sz = read_fs(fsnode, 0, 256, buf);
			int j;
			for (j = 0; j < sz; j++)
				putch(buf[j]);
			puts("\"\n");
		}
		i++;
	}
	puts("\n");

	/* Re-enable interrupts */
	asm volatile("sti");

	ret = fork();
	puts("This message SHOULD BE REPEATED 4 TIMES!\n");
	
	/* ...and leave this loop in.  There is an endless loop in
	 * 'start.asm' also, if you accidently delete this next line */
	for (;;);
}


#include <system.h>

#include <multiboot.h>
#include <main.h>
#include <gdt.h>
#include <idt.h>
#include <isrs.h>
#include <tss.h>

#include <timer.h>
#include <scrn.h>
#include <page.h>

#include <vfs.h>
#include <initrd.h>

/* You will need to code these up yourself! */
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

void printmem()
{
	/*unsigned char itoa_buffer[256];
	puts(" [pzero: ");
	puts(itoa(mem_getpage0usage(), itoa_buffer, 10));
	puts("] [usage: ");
	puts(itoa(mem_getpageusage(), itoa_buffer, 10));
	puts("] [avail: ");
	puts(itoa(mem_getpageavail(), itoa_buffer, 10));
	puts("]\n");*/
}

/* The definition for the entry point of the built-in userland
 * application */
extern void entry();

/* This is a very simple main() function.  All it does is sit in an
 * infinite loop.  This will be like our 'idle' loop */
void _main(struct multiboot_info* mbt, unsigned int magic)
{
	unsigned char itoa_buffer[256];
	void* test1 = 0;
	void* test2 = 0;

	/* Setup the very core components of the kernel / CPU operation */
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	kmem_install(mbt);
	
	/* Enable IRQs */
	__asm__ __volatile__ ("sti");

	/* Install and handle various devices in the system */
	timer_install();
	kb_install();
	init_video();

	/* Install the initrd filesystem so we can use it */
	ASSERT(mbt->mods_count > 0);
	fs_root = initrd_install(*((unsigned int*)(mbt->mods_addr)));
	
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
	
	/* ...and leave this loop in.  There is an endless loop in
	 * 'start.asm' also, if you accidently delete this next line */
	for (;;);
}


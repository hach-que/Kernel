#include <system.h>

#include <multiboot.h>
#include <main.h>
#include <gdt.h>
#include <idt.h>
#include <isrs.h>
#include <tss.h>

#include <timer.h>
#include <scrn.h>
#include <mem.h>
#include <page.h>

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
	unsigned char itoa_buffer[256];
	puts(" [pzero: ");
	puts(itoa(mem_getpage0usage(), itoa_buffer, 10));
	puts("] [usage: ");
	puts(itoa(mem_getpageusage(), itoa_buffer, 10));
	puts("] [avail: ");
	puts(itoa(mem_getpageavail(), itoa_buffer, 10));
	puts("]\n");
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
	tss_install(0);
	
	/* Enable IRQs */
	__asm__ __volatile__ ("sti");

	/* Install and handle various devices in the system */
	timer_install();
	kb_install();
	init_video();
	mem_install(mbt, magic);
	page_install(mem_gettotal());

	/* Jump to userland */
	tss_to_user();

	/* Run the application */
	entry();

	for (;;);

	/* You would add commands after here */
	puts("=== Memory tests ===\n");
	printmem();
	puts("Allocating 20 bytes to test1... ");
	test1 = palloc(20);
	puts(itoa((addr)test1, itoa_buffer, 16));
	puts(".\n");
	printmem();
	puts("Freeing 20 bytes from test1...\n");
	pfree(test1, 20);
	printmem();
	puts("Allocating 20 bytes to test1... ");
	test1 = palloc(20);
	puts(itoa((addr)test1, itoa_buffer, 16));
	puts(".\n");
	printmem();
	puts("Allocating 20 bytes to test2... ");
	test2 = palloc(20);
	puts(itoa((addr)test2, itoa_buffer, 16));
	puts(".\n");
	printmem();
	puts("Freeing 20 bytes from test1...\n");
	pfree(test1, 20);
	printmem();
	puts("Freeing 20 bytes from test2...\n");
	pfree(test2, 20);
	printmem();

	/* ...and leave this loop in.  There is an endless loop in
	 * 'start.asm' also, if you accidently delete this next line */
	for (;;);
}


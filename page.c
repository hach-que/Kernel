#include <system.h>
#include <isrs.h>
#include <page.h>

struct page_directory* current_directory = 0;
struct page_directory* kernel_directory = 0;

/* Maps the block address + 4MB into the paging system
 * with the specified flags set.  Returns the address
 * of the new page table which should be added to the
 * page directory */
addr* page_table_new(addr address, addr flags)
{
	addr i = 0;
	addr* table = (addr*)palloc_aligned(sizeof(addr) * 1024);
	memset((void*)table, 0, sizeof(addr) * 1024);
	for (i = 0; i < 1024; i++)
	{
		table[i] = address | flags; /* Attributes: supervisor level, read/write, present */
		address += 4096; /* Advanced the address to the next page boundry */
	}
	return table;
}

/* Handles setting the page directory */
void page_switch(addr* dir)
{
	unsigned int cr0;
	page_directory = dir;
	asm volatile("mov %0, %%cr3":: "b"(page_directory));
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}

/* Handles a page fault */
void _page_fault(struct regs* r)
{
	/* Get the fault location */
	addr fault_location;
	asm volatile("mov %%cr2, %0" : "=r" (fault_location));
	unsigned char itoa_buffer[256];
	
	/* Determine the type of fault that occurred */
	int present  = !(r->err_code & 0x1);	// Page not present
	int rw       = r->err_code & 0x2;	// Write operation?
	int us       = r->err_code & 0x4;	// Processor was in user-mode?
	int reserved = r->err_code & 0x8;	// Overwritten CPU-reserved bits
						// of page entry?
	int id       = r->err_code & 0x10;	// Caused by an instruction fetch?

	/* Output an error message */
	putch('\n');
	settextcolor(4, 0);
	puts("Page Fault ( ");
	if (present) puts("present ");
	if (rw) puts("read-only ");
	if (us) puts("user-mode ");
	if (reserved) puts("reserved ");
	puts(") at 0x");
	puts(itoa(fault_location, itoa_buffer, 16));
	puts(".\nSystem Halted!\n\0");
	for (;;);
}

/* Installs the paging system */
void page_install(addr upper)
{
	unsigned char itoa_buffer[256];
	int i = 0;

	/* Detect if paging should not be enabled */
	if (upper == 0)
	{
		puts("Paging is not enabled.\n");
		return;
	}

	/* Initalize the page directory area */
	puts("Initializing memory for page directory... ");
	kernel_directory = (struct page_directory*)kmalloc_a(sizeof(struct page_directory));
	memset(kernel_directory, 0, sizeof(struct page_directory));
	current_directory = kernel_directory;
	puts("done at 0x");
	puts(itoa((addr)kernel_directory, itoa_buffer, 16));
	puts(".\n");

	/* Set the initial state of the page directory */
	puts("Initializing contents of page directory... ");
	for (i = 0; i < 1024; i++)
	{
		/* Attributes: supervisor level, read/write, not present */
		kernel_directory[i] = 0 | 2;
	}
	puts("done.\n");

	/* Create the page tables */
	for (i = 0; i < upper / (4 * 1024 * 1024); i += 1)
	{
		puts("Initializing page table for ");
		puts(itoa(i * 4, itoa_buffer, 10));
		puts("MB - ");
		puts(itoa((i + 1) * 4, itoa_buffer, 10));
		puts("MB... ");
		kernel_directory[i]  = (addr)page_table_new(i * 4 * 1024 * 1024, 3);
		kernel_directory[i] |= 3;
		puts("done.\n");
	}

	/* Now enable paging */
	puts("Enabling paging... ");
	page_switch(kernel_directory);
	puts("done.\n");
}

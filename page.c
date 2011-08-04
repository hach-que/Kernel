#include <system.h>
#include <frame.h>
#include <isrs.h>
#include <page.h>
#include <vmem.h>

struct page_directory* current_directory = 0;
struct page_directory* kernel_directory = 0;
extern addr kmem_addr;
extern struct vmem_heap* kmem_heap;
extern unsigned int* frames;
extern unsigned int nframes;
extern unsigned int end;

/* Gets the specified page */
page_t* get_page(addr address, int make, struct page_directory* dir)
{
	/* Turn the address into an index */
	address /= 0x1000;

	/* Find the page table containing this address */
	unsigned int table_idx = address / 1024;
	if (dir->tables[table_idx]) /* If this table is already assigned */
	{
		return &dir->tables[table_idx]->pages[address%1024];
	}
	else if (make)
	{
		unsigned int tmp;
		dir->tables[table_idx] = (struct page_table*)kmalloc_ap(sizeof(struct page_table), &tmp);
		memset(dir->tables[table_idx], 0, 0x1000);
		dir->tables_phys[table_idx] = tmp | 0x7;
		return &dir->tables[table_idx]->pages[address%1024];
	}
	else
		return 0;
}

/* Handles setting the page directory */
void page_switch(struct page_directory* dir)
{
	unsigned int cr0;
	current_directory = dir;

	asm volatile("mov %0, %%cr3":: "r"(&dir->tables_phys));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
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

	addr memend = kmem_total();

	/* Set the frames and frame count */
	puts("Initializing physical frames... ");
	nframes = memend / 0x1000;
	frames = (unsigned int*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	puts("done (");
	puts(itoa(nframes, itoa_buffer, 10));
	puts(").\n");

	/* Initalize the page directory area */
	puts("Initializing memory for page directory... ");
	kernel_directory = (struct page_directory*)kmalloc_a(sizeof(struct page_directory));
	memset(kernel_directory, 0, sizeof(struct page_directory));
	current_directory = kernel_directory;
	puts("done at 0x");
	puts(itoa((addr)kernel_directory, itoa_buffer, 16));
	puts(".\n");

	/* Map some pages in the kernel heap area */
	i = 0;
	for (i = VMEM_START; i < VMEM_START + VMEM_INITIAL_SIZE; i += 0x1000)
		get_page(i, 1, kernel_directory);

	/* Set the initial state of the page directory */
	puts("Initializing contents of page directory... ");
	i = 0;
	while (i < kmem_addr + 0x1000)
	{
		/* Kernel code is readable but not writable from userspace */
		frame_alloc(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;	
	}
	puts("done.\n");

	/* Now allocate those pages we mapped earlier */
	for (i = VMEM_START; i < VMEM_START + VMEM_INITIAL_SIZE; i += 0x1000)
		frame_alloc(get_page(i, 1, kernel_directory), 0, 0);

	/* Now enable paging */
	puts("Enabling paging... ");
	page_switch(kernel_directory);
	puts("done.\n");

	/* Initalize the kernel's virtual memory */
	kmem_heap = create_heap(VMEM_START, VMEM_START + VMEM_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}

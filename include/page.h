#ifndef __PAGE_H
#define __PAGE_H

/* Structures */
typedef struct page
{
	unsigned int present	: 1;	/* Page present in memory */
	unsigned int rw		: 1;	/* Read-only if clear, read-write if set */
	unsigned int user	: 1;	/* Supervisor level if clear */
	unsigned int accessed	: 1;	/* Has the page been accessed since last refresh? */
	unsigned int dirty	: 1;	/* Has the page been written since last refresh? */
	unsigned int unused	: 7;	/* Amalgamation of unused and reserved bits */
	unsigned int frame	: 20;	/* Frame address (shifted 12 bits */
} page_t;

struct page_table
{
	struct page pages[1024];
};

struct page_directory
{
	/* Array of pointers to page tables */
	struct page_table* tables[1024];

	/* Array of pointers to the page tables above, but
	 * gives their physical location, for loading into
	 * the CR3 register */
	addr tables_phys[1024];

	/* The physical address of tables_phys.  This comes
	 * into play when we get our kernel heap allocated
	 * and the directory may be in a different location
	 * in virtual memory */
	addr phys_addr;
};

extern struct page_directory* current_directory;
extern struct page_directory* kernel_directory;

/* PAGE.C */
extern void page_install();
extern void page_switch(struct page_directory* dir);
extern page_t page_get(addr address, int make, struct page_directory* dir);
extern addr* page_table_new(addr address, addr flags);

#endif

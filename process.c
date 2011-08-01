#include <system.h>
#include <process.h>
#include <tss.h>
#include <mem.h>

/* Handles the creation of new virtual memory for a
 * process and returns it */
process_t* process_new()
{
	int i = 0;

	/* Allocate memory for the process structure */
	struct process* result = (struct process*)palloc(sizeof(struct process));
	memset((void*)result, 0, sizeof(struct process));

	/* Create a page directory for new process */
	result->page_directory = (addr*)palloc_aligned(sizeof(addr) * 1024);
	memset((void*)result->page_directory, 0, sizeof(addr) * 1024);
	
	/* Set the initial state of the page directory */
	for (i = 0; i < 1024; i++)
	{
		/* Attributes: user level, read/write, not present */
		result->page_directory[i] = 0 | 2 | 4;
	}
	
	/* Create the page tables */
	for (i = 0; i < mem_gettotal() / (4 * 1024 * 1024); i += 1)
	{
		result->page_directory[i]  = (addr)page_table_new(i * 4 * 1024 * 1024, 1 | 2 | 4);
		result->page_directory[i] |= 1 | 2 | 4;
	}

	return result;
}

/* Enters user-mode for the execution of the process */
void process_enter(struct process* proc)
{
	/* Install the new page directory*/
	page_switch(proc->page_directory);

	/* Set the TSS to user mode */
	tss_to_user();
}

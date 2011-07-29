#include <system.h>
#include <multiboot.h>

/* Linked list structure for memory page allocate */
struct mem_llist
{
	struct mem_llist* prev;
	unsigned long long int start;
	unsigned long long int length;
	struct mem_llist* next;
};

/* Preallocated page to store the page frame allocation information */
unsigned long long int page0 = 0;
unsigned char page0alloc[1024] = { };
unsigned int page0usage = 0;
unsigned long long int pageusage = 0;
unsigned long long int pagetotal = 0;
struct mem_llist* pages = 0;

/* Halts the system because there was a critical memory allocation
 * failure within the kernel */
void mem_fail_msg(unsigned char* msg)
{
	putch('\n');
	settextcolor(4, 0);
	puts("Kernel Memory Exception.  System Halted!\n");
	puts("Specific reason: ");
	puts(msg);
	puts(".\n");
	for (;;);
}
void mem_fail()
{
	mem_fail_msg("Out of Memory");
}

/* Allocates raw memory within the 0th page and returns it */
void* ralloc(unsigned int size)
{
	unsigned char itoa_buffer[256];

	unsigned long long int pagestart = page0;
	unsigned long long int chunkstart = 0;
	unsigned long pagelen = 4096;
	unsigned int i = 0;
       	unsigned int s = 0;
	for (i = 0; i < pagelen / 4; i++)
	{
		if (page0alloc[i] == 0 && s == 0)
		{
			chunkstart = pagestart + i * 4;
			s++;
		}
		else if (page0alloc[i] == 0 && s < size / 4)
			s++;
		else if (page0alloc[i] == 0 && s == size / 4)
		{
			/* Found a chunk of appropriate size, so we
			 * mark all of the appropriate indicies in
			 * page0alloc as 1 */
			for (i = 0; i < size / 4; i++)
				page0alloc[(chunkstart - pagestart) / 4 + i] = 1;
			page0usage += size;
			return chunkstart;
		}
		else if (page0alloc[i] == 1)
			s = 0;
		else
			mem_fail_msg("Page 0 allocation array is corrupt");
	}

	/* We were unable to allocate any more memory
	 * into page 0 */
	mem_fail_msg("No more space in page 0");
}

/* Frees raw memory within the 0th page */
void rfree(void* pos, unsigned int size)
{
	unsigned long long int pagestart = page0;
	unsigned long long int chunkstart = pos;
	unsigned int i = 0;
	for (i = 0; i < size / 4; i++)
		page0alloc[(chunkstart - pagestart) / 4 + i] = 1;
	page0usage -= size;
}

/* Returns how much of the page 0 memory is in use */
unsigned int mem_getpage0usage()
{
	return page0usage * 4;
}

/* Returns how much of the other page memory is in use */
unsigned int mem_getpageusage()
{
	return pageusage;
}

/* Returns how much memory is available in the paging system */
unsigned int mem_getpageavail()
{
	/* TODO: This should actually calculate the amount
	 * of RAM available instead of total - usage to
	 * ensure that the figure is absolutely correct */
	return pagetotal - pageusage;
}

/* Allocates a section of memory */
void* palloc(unsigned long long int size)
{
	struct mem_llist* p = pages;
	unsigned long long int s = 0;
	while (p != 0)
	{
		if (p->length >= size)
		{
			s = p->start;
			p->start += size;
			pageusage += size;
			return s;
		}
		p = p->next;
	}
	
	/* Fail if we can't allocate this memory */
	mem_fail();
}

/* Frees a section of memory */
void pfree(void* pos, unsigned long long int size)
{
	unsigned char itoa_buffer[256];
	struct mem_llist* p = pages;
	struct mem_llist* t = 0;
	pageusage -= size;
	while (p != 0)
	{
		if (p->prev == 0 && pos + size <= p->start)
		{
			/* This memory is before the start of
			 * the first available memory (i.e. it
			 * was allocated from the first block) */
			if (pos + size == p->start)
			{
				/* The position + size of this free chunk
				 * is directly before the first start
				 * position of available memory, so
				 * simply move the start position back */
				p->start -= size;
				return;
			}
			else if (p == pages)
			{
				/* This memory is before the first available
				 * memory and doesn't line up with the
				 * start, so we have to allocate a new
				 * linked list element */
				t = palloc(sizeof(struct mem_llist));
				memset(t, 0, sizeof(struct mem_llist));
				t->prev = 0;
				t->next = pages;
				t->start = pos;
				t->length = size;

				/* Now set the global pages to t */
				pages = t;
				return;
			}
		}
		else if (p->start + p->length <= pos && (p->next == 0 || p->next->start >= pos + size))
		{
			/* Does this block of memory fit on the end or
			 * start of p? */
			if (p->start + p->length == pos)
			{
				/* The position of this free chunk is
				 * on the end of the current page, so
				 * we simply increase the length of p */
				p->length += size;
				return;
			}
			else if (p->next != 0 && pos + size == p->next->start)
			{
				/* The position + length of this chunk
				 * is at the start of the next page, so
				 * we simply move the start position of
				 * the next page */
				p->next->start -= size;
				return;
			}
			else if (p->next != 0) /* If we allowed the execution of this
		       				* code under that situation, then it
						* would be possible to free memory
						* beyond the last free page (essentially
						* adding a new element to the end of
						* the page allocation list) which would
						* not be good at all, since then we'd
						* think we have more memory than we
						* really do. */
			{
				/* This memory is inbetween the current
				 * page's end and the next page's start
				 * so we need to insert a new linked list
				 * element */
				t = palloc(sizeof(struct mem_llist));
				memset(t, 0, sizeof(struct mem_llist));
				t->prev = p;
				t->next = p->next;
				t->start = pos;
				t->length = size;

				/* Adjust the surrounding elements so they
				 * point to the next element in the list */
				p->next = t;
				t->next->prev = t;
				return;
			}
		}

		p = p->next;
	}

	/* If we got to here, then we're trying to free
	 * memory that doesn't exist, or is otherwise
	 * invalid */
	mem_fail_msg("Attempt to free non-existant memory");
}

/* Registers the memory management system */
void mem_install(struct multiboot_info* mbt, unsigned int magic)
{
	struct multiboot_memory_map* mmap = mbt->mmap_addr;
	unsigned long long int pagel = 0;
	struct mem_llist* t = 0;
	struct mem_llist* p = 0;
	unsigned char itoa_buffer[256];

	/* Find the first available usable area above 1MB (so we
	 * don't accidently run into kernel memory) */
	puts("Searching memory map to find first available chunk... ");
	while (mmap < mbt->mmap_addr + mbt->mmap_length && page0 == 0)
	{
		if (mmap->base_addr >= 1024 * 1024 /* addr >= 1MB */ && mmap->type == 1 /* is it usable memory? */)
		{
			/* Set page 0 to this address and then
			 * exit the initial loop */
			puts("found at 0x");
			puts(itoa(mmap->base_addr, itoa_buffer, 16));
		       	puts(".\n");
			page0 = mmap->base_addr;
			pagel = mmap->length;
			pagetotal += pagel - 4096;
			break;
		}

		mmap = (struct multiboot_memory_map*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}

	/* Check to see whether page0 is 0 (unset); if it is, we fail
	 * because we don't have enough memory to store our page allocation
	 * system */
	if (page0 == 0)
		mem_fail_msg("No space for page 0");
	else
		memset(page0alloc, 0, 1024);

	/* Alright, so now we create the list by allocating the first element */
	puts("Creating raw page allocation list... ");
	pages = ralloc(sizeof(struct mem_llist));
	memset(pages, 0, sizeof(struct mem_llist));
	pages->prev = 0;
	pages->start = page0 + 4096;
	pages->length = page0 + pagel;
	pages->next = 0;
	puts("done.\n");

	/* Now we mark the rest of our usable memory */
	puts("Marking the rest of usable memory in raw page allocation list... ");
	p = pages;
	mmap = mbt->mmap_addr;
	while (mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		puts("\n");
		puts(itoa(mmap->base_addr, itoa_buffer, 16));
		puts(" -> ");
		puts(itoa(mmap->base_addr + mmap->length, itoa_buffer, 16));
		if (mmap->type == 1)
			puts(": USABLE");
		else
			puts(": NOT");

		if ((mmap->base_addr != page0 || mmap->length != pagel) && mmap->type == 1)
		{
			/* This is a usable section of memory that was not
			 * used as page0 */
			t = ralloc(sizeof(struct mem_llist));
			memset(t, 0, sizeof(struct mem_llist));
			p->next = t;
			t->prev = p;
			t->start = mmap->base_addr;
			t->length = mmap->length;
			t->next = 0;
			pagetotal += mmap->length;
			p = t;
		}

		mmap = (struct multiboot_memory_map*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}
	puts("\n");
}

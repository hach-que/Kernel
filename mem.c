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
struct mem_llist* pages = 0;

/* Halts the system because there was a critical memory allocation
 * failure within the kernel */
void mem_fail()
{
	putch('\n');
	settextcolor(4, 0);
	puts("Kernel Out of Memory.  System Halted!\n");
	for (;;);
}

/* Allocates raw memory within the 0th page and returns it */
void* ralloc(unsigned int size)
{
	unsigned long long int pagestart = page0;
	unsigned long long int chunkstart = 0;
	unsigned long pagelen = 4096;
	unsigned int i = 0;
       	unsigned int s = 0;
	while (i < pagelen / 4)
	{
		if (page0alloc[i] == 0 && s == 0)
			chunkstart = pagestart + i * 4;
		else if (page0alloc[i] == 0 && s < size / 4)
			s++;
		else if (page0alloc[i] == 0 && s == size / 4)
		{
			/* Found a chunk of appropriate size, so we
			 * mark all of the appropriate indicies in
			 * page0alloc as 1 */
			for (i = 0; i < size / 4; i++)
				page0alloc[(chunkstart - pagestart) / 4 + i] = 1;
			return chunkstart;
		}
		else if (page0alloc[i] == 1)
			s = 0;
	}
}

/* Frees raw memory within the 0th page */
void rfree(void* pos, unsigned int size)
{
	unsigned long long int pagestart = page0;
	unsigned long long int chunkstart = pos;
	unsigned int i = 0;
	for (i = 0; i < size / 4; i++)
		page0alloc[(chunkstart - pagestart) / 4 + i] = 1;
}

/* Registers the memory management system */
void mem_install(struct multiboot_info* mbt, unsigned int magic)
{
	struct multiboot_memory_map* mmap = mbt->mmap_addr;
	unsigned long long int pagel = 0;
	struct mem_llist* t = 0;
	struct mem_llist* p = 0;

	/* Find the first available usable area above 1MB (so we
	 * don't accidently run into kernel memory) */
	while (mmap < mbt->mmap_addr + mbt->mmap_length && page0 == 0)
	{
		if (mmap->base_addr >= 1024 * 1024 /* addr >= 1MB */ && mmap->type == 1 /* is it usable memory? */)
		{
			/* Set page 0 to this address and then
			 * exit the initial loop */
			page0 = mmap->base_addr;
			pagel = mmap->length;
			break;
		}

		mmap = (struct multiboot_memory_map*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}

	/* Check to see whether page0 is 0 (unset); if it is, we fail
	 * because we don't have enough memory to store our page allocation
	 * system */
	if (page0 == 0)
		mem_fail();
	else
		memset(page0alloc, 0, 1024);

	/* Alright, so now we create the list by allocating the first element */
	pages = ralloc(sizeof(struct mem_llist));
	memset(pages, 0, sizeof(struct mem_llist));
	pages->prev = 0;
	pages->start = page0 + 4096;
	pages->length = page0 + pagel;
	pages->next = 0;

	/* Now we mark the rest of our usable memory */
	p = pages;
	mmap = mbt->mmap_addr;
	while (mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if (mmap->base_addr != page0 || mmap->length != pagel)
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
			p = t;
		}

		mmap = (struct multiboot_memory_map*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}
}

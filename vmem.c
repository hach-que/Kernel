#include <system.h>
#include <frame.h>
#include <vmem.h>
#include <page.h>

static signed int find_smallest_hole(addr size, unsigned char page_align, struct vmem_heap* heap)
{
	/* Find the smallest hole that will fit */
	unsigned int iterator = 0;
	while (iterator < heap->index.size)
	{
		struct vmem_header* header = (struct vmem_header*)lookup_ordered_array(iterator, &heap->index);
		/* If the user has request the memory be page-aligned */
		if (page_align > 0)
		{
			/* Page-align the starting point of this header */
			addr location = (addr)header;
			signed int offset = 0;
			if ((location+sizeof(struct vmem_header)) & 0xFFFFF000 != 0)
				offset = 0x1000 /* page size */ - (location+sizeof(struct vmem_header)) % 0x1000;
			signed int hole_size = (signed int)header->size - offset;
			/* Can we fit now? */
			if (hole_size >= (signed int)size)
				break;
		}
		else if (header->size >= size)
			break;
		iterator++;
	}

	/* Why did the loop exit? */
	if (iterator == heap->index.size)
		return -1; /* We got to the end and didn't find anything */
	else
		return iterator;
}

static signed char vmem_header_less_than(void* a, void* b)
{
	return (((struct vmem_header*)a)->size < ((struct vmem_header*)b)->size)?1:0;
}

vmem_heap_t* create_heap(addr start, addr end_addr, addr max, unsigned char supervisor, unsigned char readonly)
{
	struct vmem_heap* heap = (struct vmem_heap*)kmalloc(sizeof(struct vmem_heap));

	/* All of our assumptions are made on start and end being page-aligned */
	ASSERT(start % 0x1000 == 0);
	ASSERT(end_addr % 0x1000 == 0);

	/* Initalize the index */
	heap->index = place_ordered_array((void*)start, VMEM_INDEX_SIZE, &vmem_header_less_than);

	/* Shift the start address forward to resemble where we can start putting data */
	start += sizeof(type_t) * VMEM_INDEX_SIZE;

	/* Make sure the start address is page aligned */
	if (start & 0xFFFFF000 != 0)
	{
		start &= 0xFFFFF000;
		start += 0x1000;
	}

	/* Write the start, end and max addresses into the heap structure */
	heap->start_address = start;
	heap->end_address = end_addr;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	/* We start off with one large hole in the index */
	struct vmem_header* hole = (struct vmem_header*)start;
	hole->size = end_addr - start;
	hole->magic = VMEM_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((void*)hole, &heap->index);

	return heap;
}

static void expand(addr new_size, struct vmem_heap* heap)
{
	/* Sanity check */
	ASSERT(new_size > heap->end_address - heap->start_address);

	/* Get the nearest following page boundary */
	if (new_size & 0xFFFFF000 != 0)
	{
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}

	/* Make sure we are not overreaching ourselves */
	ASSERT(heap->start_address + new_size <= heap->max_address);

	/* This should always be on a page boundary */
	addr old_size = heap->end_address - heap->start_address;
	addr i = old_size;

	while (i < new_size)
	{
		frame_alloc( get_page(heap->start_address+i, 1, kernel_directory),
				(heap->supervisor)?1:0, (heap->readonly)?0:1);
		i += 0x1000; /* page size */
	}

	heap->end_address = heap->start_address+new_size;
}

static addr contract(addr new_size, struct vmem_heap* heap)
{
	/* Sanity check */
	ASSERT(new_size < heap->end_address - heap->start_address);

	/* Get the nearest following page boundary */
	if (new_size & 0x1000)
	{
		new_size &= 0x1000;
		new_size += 0x1000;
	}

	/* Don't contract too far */
	if (new_size < VMEM_MIN_SIZE)
		new_size = VMEM_MIN_SIZE;
	addr old_size = heap->end_address - heap->start_address;
	addr i = old_size;
	while (new_size < i)
	{
		frame_free(get_page(heap->start_address + i, 0, kernel_directory));
		i -= 0x1000;
	}
	heap->end_address = heap->start_address + new_size;
	return new_size;
}

void* vmalloc(addr size, unsigned char page_align, struct vmem_heap* heap)
{
	/* Make sure we take the size of the header / footer into account */
	addr new_size = size + sizeof(struct vmem_header) + sizeof(struct vmem_footer);

	/* Find the smallest hole that will fit */
	signed int iterator = find_smallest_hole(new_size, page_align, heap);

	if (iterator == -1) /* If we didn't find a suitable hole */
	{
		/* Save some previous data */
		addr old_length = heap->end_address - heap->start_address;
		addr old_end_address = heap->end_address;

		/* We need to allocate some more space */
		expand(old_length + new_size, heap);
		addr new_length = heap->end_address - heap->start_address;

		/* Find the endmost header (not endmost in size, but in location) */
		iterator = 0;
		/* Vars to hold the index of, and value of, the endmost header found so far */
		addr idx = -1; addr value = 0x0;
		while (iterator < heap->index.size)
		{
			addr tmp = (addr)lookup_ordered_array(iterator, &heap->index);
			if (tmp > value)
			{
				value = tmp;
				idx = iterator;
			}
			iterator++;
		}

		/* If we didn't find any headers, we need to add one */
		if (idx == -1)
		{
			struct vmem_header* header	= (struct vmem_header*)old_end_address;
			header->magic			= VMEM_MAGIC;
			header->size			= new_length - old_length;
			header->is_hole			= 1;
			struct vmem_footer* footer	= (struct vmem_footer*)(old_end_address + header->size
							  - sizeof(struct vmem_footer));
			footer->magic			= VMEM_MAGIC;
			footer->header			= header;
			insert_ordered_array((void*)header, &heap->index);
		}
		else
		{
			/* The last header needs adjusting */
			struct vmem_header* header = lookup_ordered_array(idx, &heap->index);
			header->size += new_length - old_length;
			
			/* Rewrite the footer */
			struct vmem_footer* footer = (struct vmem_footer*)((addr)header + header->size - 
						     sizeof(struct vmem_footer));
			footer->header = header;
			footer->magic = VMEM_MAGIC;
		}
	}

	struct vmem_header* orig_hole_header = (struct vmem_header*)lookup_ordered_array(iterator, &heap->index);
	addr orig_hole_pos = (addr)orig_hole_header;
	addr orig_hole_size = orig_hole_header->size;

	/* Here we work out if we should split the hole we found into two parts.
	 * Is the original hole size - requested hole size less than the overhead for adding a new hole? */
	if (orig_hole_size - new_size < sizeof(struct vmem_header) + sizeof(struct vmem_footer))
	{
		/* Then just increase the requested size to the size of the hole we found */
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	/* If we need to page-align the data, do it now and make a new hole in front of our block */
	if (page_align && (orig_hole_pos & 0xFFFFF000))
	{
		addr new_location		= orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - 
					  	  sizeof(struct vmem_header);
		struct vmem_header* hole_header = (struct vmem_header*)orig_hole_pos;
		hole_header->size		= 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) -
						  sizeof(struct vmem_header);
		hole_header->magic		= VMEM_MAGIC;
		hole_header->is_hole		= 1;
		struct vmem_footer* hole_footer	= (struct vmem_footer*)((addr)new_location - sizeof(struct vmem_footer));
		hole_footer->magic		= VMEM_MAGIC;
		hole_footer->header		= hole_header;
		orig_hole_pos			= new_location;
		orig_hole_size			= orig_hole_size - hole_header->size;
	}
	else
	{
		/* Else we don't need this hole any more, delete it from the index */
		remove_ordered_array(iterator, &heap->index);
	}

	/* Overwrite the original header... */
	struct vmem_header* block_header	= (struct vmem_header*)orig_hole_pos;
	block_header->magic			= VMEM_MAGIC;
	block_header->is_hole			= 0;
	block_header->size			= new_size;
	/* ... and the footer */
	struct vmem_footer* block_footer	= (struct vmem_footer*)(orig_hole_pos + sizeof(struct vmem_header) + size);
	block_footer->magic			= VMEM_MAGIC;
	block_footer->header			= block_header;

	/* We may need to write a new hole after the allocated block.
	 * We do this only if the new hole would have positive size */
	if (orig_hole_size - new_size > 0)
	{
		struct vmem_header* hole_header	= (struct vmem_header*)(orig_hole_pos + sizeof(struct vmem_header) + size +
						  sizeof(struct vmem_footer));
		hole_header->magic		= VMEM_MAGIC;
		hole_header->is_hole		= 1;
		hole_header->size		= orig_hole_size - new_size;
		struct vmem_footer* hole_footer	= (struct vmem_footer*)((addr)hole_header + orig_hole_size - new_size -
						  sizeof(struct vmem_footer));
		if ((addr)hole_footer < heap->end_address)
		{
			hole_footer->magic	= VMEM_MAGIC;
			hole_footer->header	= hole_header;
		}
		
		/* Put the new hole in the array */
		insert_ordered_array((void*)hole_header, &heap->index);
	}

	/* ... and we're done! */
	return (void*)((addr)block_header + sizeof(struct vmem_header));
}

void vfree(void *p, struct vmem_heap* heap)
{
	/* Exit gracefully for null pointers */
	if (p == 0)
		return;

	/* Get the header and footer associated with this pointer */
	struct vmem_header* header = (struct vmem_header*)((addr)p - sizeof(struct vmem_header));
	struct vmem_footer* footer = (struct vmem_footer*)((addr)header + header->size - sizeof(struct vmem_footer));

	/* Sanity checks */
	ASSERT(header->magic == VMEM_MAGIC);
	ASSERT(footer->magic == VMEM_MAGIC);

	/* Make us a hole */
	header->is_hole = 0;

	/* Do we want to add this header to the free holes index */
	char do_add = 1;

	/* Unify left */
	struct vmem_footer* test_footer = (struct vmem_footer*)((addr)header - sizeof(struct vmem_footer));
	if (test_footer->magic == VMEM_MAGIC && test_footer->header->is_hole == 1)
	{
		addr cache_size = header->size;	/* Cache our current size */
		header = test_footer->header;	/* Rewrite our header with the new one */
		footer->header = header;	/* Rewrite our footer to point to the new header */
		header->size += cache_size;	/* Change the size */
		do_add = 0;			/* Since this header is already in the index, we don't want to add it */
	}

	/* Unify right */
	struct vmem_header* test_header = (struct vmem_header*)((addr)footer + sizeof(struct vmem_footer));
	if (test_header->magic == VMEM_MAGIC && test_header->is_hole)
	{
		header->size += test_header->size;	/* Increase our size */
		test_footer = (struct vmem_footer*)((addr)test_header + test_header->size - sizeof(struct vmem_footer));
		footer = test_footer;

		/* Find and remove this kernel from the index */
		unsigned int iterator = 0;
		while ((iterator < heap->index.size) &&
			(lookup_ordered_array(iterator, &heap->index) != (void*)test_header))
			iterator++;

		/* Make sure we actually found the item */
		ASSERT(iterator < heap->index.size);

		/* Remove it */
		remove_ordered_array(iterator, &heap->index);
	}

	/* If the footer location is the end address, we can contract */
	if ((addr)footer + sizeof(struct vmem_footer) == heap->end_address)
	{
		addr old_length = heap->end_address - heap->start_address;
		addr new_length = contract((addr)header - heap->start_address, heap);

		/* Check how big we will be after resizing */
		if (header->size - (old_length - new_length) > 0)
		{
			/* We will still exist, so resize us */
			header->size -= old_length - new_length;
			footer = (struct vmem_footer*)((addr)header + header->size - sizeof(struct vmem_footer));
			footer->magic = VMEM_MAGIC;
			footer->header = header;
		}
		else
		{
			/* We will no longer exist, remove us from the index */
			unsigned int iterator = 0;
			while ((iterator < heap->index.size) &&
				(lookup_ordered_array(iterator, &heap->index) != (void*)test_header))
				iterator++;

			/* If we didn't find ourselves, we have nothing to remove */
			if (iterator < heap->index.size)
				remove_ordered_array(iterator, &heap->index);
		}
	}

	/* Add ourselves if we need to */
	if (do_add == 1)
		insert_ordered_array((void*)header, &heap->index);
}

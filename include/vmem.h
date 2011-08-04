#ifndef __VMEM_H
#define __VMEM_H

#include <types/ordered_array.h>

#define VMEM_START		0xC0000000
#define VMEM_INITIAL_SIZE	0x100000
#define VMEM_INDEX_SIZE		0x20000
#define VMEM_MAGIC		0x123890AB
#define VMEM_MIN_SIZE		0x70000

/* Structure definitions */
struct vmem_header
{
	unsigned int magic;	/* Magic number, used for error checking and identification */
	unsigned char is_hole;	/* 1 if this is a hole, 0 if this is a block */
	unsigned int size;	/* Size of the block, including this and the footer */
};

struct vmem_footer
{
	unsigned int magic;		/* Magic number, same as in header */
	struct vmem_header* header;	/* Pointer to the block header */
};

typedef struct vmem_heap
{
	struct ordered_array index;	
	addr start_address;		/* The start of our allocated space */
	addr end_address;		/* The end of our allocated space.  May be expanded up to max_address */
	addr max_address;		/* The maximum address the heap can be expanded to */
	unsigned char supervisor;	/* Should extra pages requested by us be mapped as supervisor-only? */
	unsigned char readonly;		/* Should extra pages requested by us be mapped as read-only? */
} vmem_heap_t;

/* VMEM.C */
extern vmem_heap_t* create_heap(addr start, addr end, addr max, unsigned char supervisor, unsigned char readonly);
extern void* vmalloc(addr size, unsigned char page_align, struct vmem_heap* heap);
extern void vfree(void* p, struct vmem_heap* heap);

#endif

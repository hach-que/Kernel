#include <system.h>
#include <frame.h>
#include <page.h>

/* A bitset of frames - used or free */
unsigned int* frames;
unsigned int nframes;

/* Macros used in the bitset algorithms */
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/* Static function to set a bit in the frames bitset */
static void set_frame(addr frame_addr)
{
	addr frame = frame_addr/0x1000;
	addr idx = INDEX_FROM_BIT(frame);
	addr off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

/* Static function to clear a bit in the frames bitset */
static void clear_frame(addr frame_addr)
{
	addr frame = frame_addr/0x1000;
	addr idx = INDEX_FROM_BIT(frame);
	addr off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

/* Static function to test if a bit is set */
static unsigned int test_frame(addr frame_addr)
{
	addr frame = frame_addr/0x1000;
	addr idx = INDEX_FROM_BIT(frame);
	addr off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

/* Static function to find the first free frame */
static addr first_frame()
{
	unsigned int i, j;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
	{
		if (frames[i] != 0xFFFFFFFF)
		{
			for (j = 0; j < 32; j++)
			{
				unsigned int test = 0x1 << j;
				if ( !(frames[i]&test) )
				{
					return i*4*8+j;
				}
			}
		}
	}
}

/* Function to allocate a frame */
void frame_alloc(struct page* p, int is_kernel, int is_writable)
{
	if (p->frame != 0)
	{
		/* Frame was already allocated */
		return;
	}
	else
	{
		addr idx = first_frame(); /* The first free frame */
		if (idx == (addr) - 1)
		{
			puts("No free frames!\n");
			for(;;);
		}
		set_frame(idx*0x1000);		/* This frame is now ours */
		p->present = 1;			/* Mark it as present */
		p->rw = (is_writable)?1:0;	/* Should the page be writable? */
		p->user = (is_kernel)?0:1;	/* Should the page be user-mode? */
		p->frame = idx;
	}
}

/* Function to deallocate a frame */
void frame_free(struct page* p)
{
	addr frame;
	if (!(frame=page->frame))
	{
		/* The given page didn't actually have an allocate frame */
	}
	else
	{
		clear_frame(frame);		/* Frame is now free again */
		page->frame =0x0;		/* Page now doesn't have a frame */
	}
}

#ifndef __FRAME_H
#define __FRAME_H

#include <page.h>

/* Macros used in the bitset algorithms */
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/* FRAME.C */
extern void frame_alloc(struct page* p, int is_kernel, int is_writable);
extern void frame_free(struct page* p);

#endif

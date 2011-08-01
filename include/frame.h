#ifndef __FRAME_H
#define __FRAME_H

/* FRAME.C */
extern void frame_alloc(struct page* p, int is_kernel, int is_writable);
extern void frame_free(struct page* p)

#endif

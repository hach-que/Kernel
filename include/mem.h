#ifndef __MEM_H
#define __MEM_H

/* MEM.C */
extern void mem_install(struct multiboot_info* mbt, unsigned int magic);
extern void* palloc(addr size);
extern void pfree(void* pos, addr size);
addr mem_getpage0usage();
addr mem_getpageusage();
addr mem_getpageavail();
addr mem_gettotal();

#endif

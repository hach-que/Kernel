#ifndef __KMEM_H
#define __KMEM_H

extern addr kmalloc_a(addr size);
extern addr kmalloc_p(addr size, addr* phys);
extern addr kmalloc_ap(addr size, addr* phys);
extern addr kmalloc(addr size);
extern void kfree(addr pos);

extern addr kmem_total();
extern void kmem_install(struct multiboot_info* mbt);

#endif

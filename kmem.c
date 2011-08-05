#include <system.h>
#include <page.h>
#include <kmem.h>
#include <vmem.h>

extern unsigned int end;
addr kmem_addr = (addr)&end;
addr kmem_memtotal = 0;
struct vmem_heap* kmem_heap = 0;

/* Internal kernel memory allocation */
static addr kmalloc_i(addr size, int align, addr* phys)
{
	if (kmem_heap != 0)
	{
		void* address = vmalloc(size, (unsigned char)align, kmem_heap);
		if (phys != 0)
		{
			struct page* p = (struct page*)get_page((addr)address, 0, kernel_directory);
			*phys = p->frame*0x1000 + ((addr)address&0xFFF);
		}
		return (addr)address;
	}

	if (align == 1 && (kmem_addr & 0xFFFFF000))
	{
		/* Align the address */
		kmem_addr &= 0xFFFFF000;
		kmem_addr += 0x1000;
	}
	if (phys)
	{
		*phys = kmem_addr;
	}
	addr tmp = kmem_addr;
	kmem_addr += size;
	return tmp;
}

addr kmalloc_a(addr size)
{
	return kmalloc_i(size, 1, 0);
}

addr kmalloc_p(addr size, addr* phys)
{
	return kmalloc_i(size, 0, phys);
}

addr kmalloc_ap(addr size, addr* phys)
{
	return kmalloc_i(size, 1, phys);
}

addr kmalloc(addr size)
{
	return kmalloc_i(size, 0, 0);
}

void kfree(addr pos)
{
	if (kmem_heap != 0)
		return vfree((void*)pos, kmem_heap);
}

/* Returns the total memory in the system */
addr kmem_total()
{
	return kmem_memtotal;
}

/* Counts the total amount of memory in the system and stores it */
void kmem_install(struct multiboot_info* mbt)
{
	struct multiboot_memory_map* mmap = (struct multiboot_memory_map*)mbt->mmap_addr;
	while ((addr)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if ((addr)mmap->base_addr + (addr)mmap->length > kmem_memtotal)
			kmem_memtotal = (addr)mmap->base_addr + (addr)mmap->length;

		mmap = (struct multiboot_memory_map*)((addr)mmap + mmap->size + sizeof(addr));
	}

	/* Adjust kmem_addr for multiboot module information */
	kmem_addr += mbt->mods_count * sizeof(unsigned int);
}

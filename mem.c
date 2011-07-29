#include <system.h>
#include <multiboot.h>

/* Registers the memory management system */
void mem_install(struct multiboot_info* mbt, unsigned int magic)
{
	struct multiboot_memory_map* mmap = mbt->mmap_addr;
	unsigned char itoabufferA[256];
	unsigned char itoabufferB[256];
	unsigned char itoabufferC[256];
	unsigned char itoabufferD[256];
//	long long addr = 0;
//	long long length = 0;

	puts("Flags are: 0b");
	itoa(mbt->flags, &itoabufferA, 2);
	puts(itoabufferA);
	puts(".\n");

	puts("Memory map starts at 0x");
	itoa(mbt->mmap_addr, &itoabufferA, 16);
	puts(itoabufferA);
	puts(" and ends at 0x");
	itoa(mbt->mmap_addr + mbt->mmap_length, &itoabufferA, 16);
	puts(itoabufferA);
	puts(" (length is 0x");
	itoa(mbt->mmap_length, &itoabufferA, 16);
	puts(itoabufferA);
	puts(").\n");

	puts("Lower memory: 0x");
	itoa(mbt->mem_lower, &itoabufferA, 16);
	puts(itoabufferA);
	puts(".\n");
	puts("Upper memory: 0x");
	itoa(mbt->mem_upper, &itoabufferA, 16);
	puts(itoabufferA);
	puts(".\n");

	while (mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		// Show what sections of memory are usable.
		//addr = (mmap->base_addr_high << 32) | mmap->base_addr_low;
		//length = (mmap->length_high << 32) | mmap->length_low;

		itoa(mmap->base_addr, &itoabufferA, 16);
		//itoa(mmap->base_addr_high, &itoabufferB, 16);
		itoa(mmap->length, &itoabufferC, 16);
		//itoa(mmap->length_high, &itoabufferD, 16);

		puts(itoabufferA);
		//puts(itoabufferB);
		puts(" -> ");
		puts(itoabufferC);
		//puts(itoabufferD);
		puts(": ");

		if (mmap->type == 1)
			puts("USABLE\n");
		else
			puts("UNUSABLE\n");

		// ...
		mmap = (struct multiboot_memory_map*)((unsigned int)mmap + mmap->size + sizeof(unsigned int));
	}
}


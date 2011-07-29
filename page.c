#include <system.h>

unsigned int* page_directory = 0;

/* Installs the paging system */
void page_install()
{
	unsigned char itoa_buffer[256];
	int i = 0;

	/* Initalize the page directory area */
	puts("Initializing memory for page directory... ");
	page_directory = palloc_aligned(sizeof(unsigned int) * 1024);
	memset(page_directory, 0, sizeof(unsigned int) * 1024);
	puts("done at 0x");
	puts(itoa(page_directory, itoa_buffer, 16));
	puts(".\n");

	puts("Initializing contents of page directory... ");
	for (i = 0; i < 1024; i++)
	{
		//puts(itoa(i, itoa_buffer, 10));
		//puts("\n");
		// Attribute: supervisor level, read/write, not present *:
		//page_directory[i] = 0 | 2;
	}
	puts("done.\n");
}

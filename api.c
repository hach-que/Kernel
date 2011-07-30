#include <system.h>

/* Handles a system call made by the userland code */
void _syscall_handler()
{
	int type;
	addr ebx, ecx, edx;
	unsigned char itoa_buffer[256];

	asm volatile("mov %%eax, %0": "=m"(type));
	asm volatile("mov %%ebx, %0": "=m"(ebx));
	asm volatile("mov %%ecx, %0": "=m"(ecx));
	asm volatile("mov %%edx, %0": "=m"(edx));

	/* Handle the type of system call */
	/*puts("[syscall] type: ");
	puts(itoa(type, itoa_buffer, 10));
	puts(" ebx: ");
	puts(itoa(ebx, itoa_buffer, 16));
	puts(" ecx: ");
	puts(itoa(ecx, itoa_buffer, 16));
	puts(" edx: ");
	puts(itoa(edx, itoa_buffer, 16));
	puts("\n");*/
	switch (type)
	{
		case 1:
			/* A totally not safe way to call this function :) */
			puts((unsigned char*)ebx);
			break;
		default:
			break;
	}
}

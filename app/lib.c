#include <app/kernel.h>

extern void puts(unsigned char* str);

addr syscall(int type, addr ebx, addr ecx, addr edx)
{
	asm volatile("mov %0, %%eax":: "m"(type));
	asm volatile("mov %0, %%ebx":: "m"(ebx));
	asm volatile("mov %0, %%ecx":: "m"(ecx));
	asm volatile("mov %0, %%edx":: "m"(edx));
	asm volatile("int $80");
}

void write(const char* msg)
{
	syscall(1, (addr)msg, 0, 0);
}

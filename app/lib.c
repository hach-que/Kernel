#include <app/kernel.h>

extern void puts(unsigned char* str);

addr syscall(int type, addr bx, addr cx, addr dx)
{
	puts("Sending interrupt 80...");
	asm volatile("int $80");
}

void write(const char* msg)
{
	syscall(1, (addr)msg, 0, 0);
}

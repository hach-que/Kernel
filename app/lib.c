#include "kernel.h"

int syscall(int type, addr p1, addr p2, addr p3, addr p4, addr p5)
{
	int a;
	asm volatile("int $0x80" : "=a" (a) : "0" (type), "b" ((int)p1), "c" ((int)p2),
			"d" ((int)p3), "S" ((int)p4), "D" ((int)p5));
	return a;
}

void write(const char* msg)
{
	syscall(0, (addr)msg, 0, 0, 0, 0);
}

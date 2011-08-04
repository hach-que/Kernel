#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <multiboot.h>
#include <string.h>

/* Macro definitions */
#define __QUOTEME_(x) #x
#define __QUOTEME(x) __QUOTEME_(x)
#define ASSERT(expr) \
	if (!(expr)) \
		panic("kernel panic (" __FILE__ ":" __QUOTEME(__LINE__) ") - assert \"" __QUOTEME(expr) "\" failed");
#define PANIC(msg) \
	panic("kernel panic (" __FILE__ ":" __QUOTEME(__LINE__) ") - " msg);

/* Type definitions */
typedef unsigned long int addr;

/* SYSTEM.C */
extern void panic(unsigned char* msg);

#endif

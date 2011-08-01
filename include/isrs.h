#ifndef __ISRS_H
#define __ISRS_H

/* This defines what the stack looks like
 * after an ISR was running */
struct regs
{
	unsigned int ds;					/* data segment selector */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;	/* pushed by 'pusha' */
	unsigned int int_no, err_code;				/* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */
};

/* ISRS.C */
extern void isrs_install();

#endif

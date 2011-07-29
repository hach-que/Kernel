#include <multiboot.h>

#ifndef __SYSTEM_H
#define __SYSTEM_H

/* MAIN.C */
extern unsigned char* memcpy(unsigned char* dest, const unsigned char* src, int count);
extern unsigned char* memset(unsigned char* dest, unsigned char val, int count);
extern unsigned short* memsetw(unsigned short* dest, unsigned short val, int count);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);

/* STRING.C */
extern int strlen(const unsigned char* str);
unsigned char* strrev(unsigned char* str);
unsigned char* itoa(unsigned long num, unsigned char* str, int base);

/* SCRN.C */
extern void cls();
extern void putch(unsigned char c);
extern void puts(unsigned char* str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

/* GDT.C */
extern void _gdt_flush();
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/* IDT.C */
extern void _idt_load();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
struct regs /* This defines what the stack looks like after an ISR was running */
{
	unsigned int gs, fs, es, ds;                            /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;    /* pushed by 'pusha' */
	unsigned int int_no, err_code;                          /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;              /* pushed by the processor automatically */
};
extern void isrs_install();

/* IRQ.C */
extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs* r));
extern void irq_uninstall_handler(int irq);

/* TIMER.C */
extern void timer_install();
extern void timer_wait(int ticks);

/* KB.C */
extern void kb_install();

/* MEM.C */
extern void mem_install(struct multiboot_info* mbt, unsigned int magic);
extern void* palloc(unsigned long long int size);
extern void pfree(void* pos, unsigned long long int size);
unsigned int mem_getpage0usage();
unsigned int mem_getpageusage();
unsigned int mem_getpageavail();

/* PAGE.C */
extern void page_install();

#endif

/* Based off the "Getting to User Mode" tutorial on
 * http://www.germsoft.com/Software/OS/Tut/Html/um.html */

#include <system.h>
#include <tss.h>

tss_t sys_tss;

void tss_install(int cpu_no)
{
	/* Now fill each value, setting values as
	 * necessary */
	sys_tss.ss0 = 0x10;

	/* Now set the IO bitmap (not necessary, so set
	 * above limit */
	sys_tss.iomap = (unsigned short) sizeof(tss_t);

	/* The fourth entry in our GDT is for our User
	 * Mode Code Segment */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

	/* The fifth entry in our GDT is for our User
	 * Mode Data Segment */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	/* The sixth entry in the GDT is for the Task
	 * State Segment */
	unsigned long address = (unsigned long)&sys_tss;
	int size = sizeof(tss_t) + 1;
	gdt_set_gate(5, address, address + size, 0x89, 0xCF);
}

/* Jumps the system to user mode */
void tss_to_user()
{
	asm volatile("\
cli; \
mov $0x23, %ax; \
mov %ax, %ds; \
mov %ax, %es; \
mov %ax, %fs; \
mov %ax, %gs; \
\
mov %esp, %eax; \
pushl $0x23; \
pushl %eax; \
pushf; \
mov $0x200, %eax; \
push %eax; \
pushl $0x1B; \
push $1f; \
iret; \
1: \
");
}

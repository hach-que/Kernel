#include <system.h>
#include <task.h>
#include <tss.h>

tss_t sys_tss;

/* Refreshs the TSS location within the GDT so the
 * processor knows where to find it */
extern void _tss_flush();

/* Sets the kernel stack */
void tss_set_kernel_stack(unsigned int stack)
{
	sys_tss.esp0 = stack;
}

/* Installs the TSS and user mode descriptors into the
 * GDT */
void tss_install(signed int num, unsigned short ss0, unsigned short esp0)
{
	/* Computer the base and limit of our entry into
	 * the GDT */
	addr base = (addr)&sys_tss;
	addr size = base + sizeof(tss_t);

	/* Add the TSS's descriptor to the GDT */
	gdt_set_gate(num, base, size, 0xE9, 0x00);

	/* Ensure the descriptor is initially zero */
	memset(&sys_tss, 0, sizeof(sys_tss));

	/* Set the default kernel stack segments and
	 * pointer positions */
	sys_tss.ss0	= ss0;
	sys_tss.esp0	= esp0;

	/* Here we should set the cs, ss, ds, es, fs and gs
	 * entries in the TSS.  These specify what segemnts
	 * should be loaded when the processor switches to
	 * kernel mode.  Therefore, they are just our normal
	 * kernel code/data segments - 0x08 and 0x10
	 * respectively, but with the last two bits set,
	 * making 0x0b and 0x13.  The setting of these bits
	 * sets the RPL (requested privilege level) to 3,
	 * meaning that this TSS can be used to switch to 
	 * kernel mode from ring 3 */
	sys_tss.cs = 0x0B;
	sys_tss.ss = sys_tss.ds = sys_tss.es = sys_tss.fs = sys_tss.gs = 0x13;
}

/* Extern definition to access the current task */
extern volatile struct task* current_task;

/* Jumps the system to user mode */
void tss_switch()
{
	// Set up the kernel stack.
	tss_set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);

	// Jump into user mode.
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
\
pop %eax; \
or $0x200, %eax; \
push %eax; \
\
pushl $0x1B; \
push $1f; \
iret; \
1: \
");
}

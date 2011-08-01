#include <system.h>
#include <syscall.h>
#include <isrs.h>
#include <scrn.h>

/* Definition for callable kernel functions */
static void *syscalls[1] =
{
	&puts
};
unsigned int num_syscalls = 1;

/* Handles a system call made by the userland code */
void _syscall_handler(struct regs* r)
{
	/* Check to ensure the syscall number is valid */
	if (r->eax > num_syscalls)
		return;

	/* Get the required syscall location */
	void* location = syscalls[r->eax];

	/* We don't know how many parameters the function
	 * wants, so we just push them all onto the stack
	 * in the correct order.  The function will use
	 * all of the parameters it wants, and we can pop
	 * then all back off afterwards */
	int ret;
	asm volatile (" \
push %1; \
push %2; \
push %3; \
push %4; \
push %5; \
call *%6; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx; \
pop %%ebx; \
" : "=a" (ret) : "r" (r->edi), "r" (r->esi), "r" (r->edx), "r" (r->ecx), "r" (r->ebx), "r" (location));
	r->eax = ret;
}

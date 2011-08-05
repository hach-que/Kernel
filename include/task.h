#ifndef __PROCESS_H
#define __PROCESS_H

#include <system.h>
#include <page.h>

/* Structure for a process */
struct task
{
	int id;					/* Process ID */
	addr esp, ebp;				/* Stack and base pointers */
	addr eip;				/* Instruction pointer */
	struct page_directory* page_directory;	/* Page directory */
	struct task* next;			/* Next task in a linked list */
};

/* TASK.C */
extern void task_install();
extern void task_switch();
extern void move_stack(void* new_stack_start, addr size);

extern int fork();
extern int getpid();

#endif

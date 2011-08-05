#include <system.h>
#include <frame.h>
#include <task.h>
#include <page.h>

/* The currently running task */
volatile struct task* current_task;

/* The start of the task linked list */
volatile struct task* ready_queue;

/* Some externs are required to access members
 * in page.c / process.asm */
extern addr initial_esp;
extern addr read_eip();

/* The next available process ID */
unsigned int next_pid = 1;

/* Moves the current stack into the new position */
void move_stack(void* new_stack_start, addr size)
{
	addr i;

	/* Allocate some space for the new stack */
	for (i = (addr)new_stack_start; i >= ((addr)new_stack_start-size); i -= 0x1000)
	{
		/* General-purpose stack is in user-mode */
		frame_alloc((struct page*)get_page(i, 1, current_directory), 0 /* User mode */, 1 /* Writable */);
	}

	/* Flush the TLB by reading and writing the page directory
	 * address again */
	addr pd_addr;
	asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
	asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

	/* Copy the old ESP and EBP registers and create
	 * new stack and base registers */
	addr old_stack_pointer;
	addr old_base_pointer;
	asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
	asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));
	addr offset = (addr)new_stack_start - initial_esp;
	addr new_stack_pointer = old_stack_pointer + offset;
	addr new_base_pointer = old_base_pointer + offset;

	/* Copy the stack */
	memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp - old_stack_pointer);

	/* Backtrace through the original stack, copying new values into
	 * the new stack */
	for (i = (addr)new_stack_start; i > (addr)new_stack_start - size; i -= 4)
	{
		addr tmp = *(addr*)i;

		/* If the value of tmp is inside the range of the old stack, assume
		 * it is a base pointer and remap it.  This will unfortunately remap
		 * ANY value in this range, whether they are base pointers or not */
		if ((old_stack_pointer < tmp) && (tmp < initial_esp))
		{
			tmp = tmp + offset;
			addr* tmp2 = (addr*)i;
			*tmp2 = tmp;
		}
	}

	/* Change stacks */
	asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
	asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}

/* Forks the current process */
int fork()
{
	/* We are modifying kernel structures, so disable interrupts */
	asm volatile("cli");

	/* Take a pointer to this process' task struct for later reference */
	struct task* parent_task = (struct task*)current_task;

	/* Clone the address space */
	struct page_directory* directory = (struct page_directory*)clone_directory(current_directory);

	/* Create a new process */
	struct task* new_task = (struct task*)kmalloc(sizeof(struct task));
	new_task->id = next_pid++;
	new_task->esp = new_task->ebp = 0;
	new_task->eip = 0;
	new_task->page_directory = directory;
	new_task->next = 0;

	/* Add it to the end of the ready queue
	 * Find the end of the ready queue */
	struct task* tmp_task = (struct task*)ready_queue;
	while (tmp_task->next)
		tmp_task = tmp_task->next;
	/* And extend it */
	tmp_task->next = new_task;

	/* This will be the entry point for the new process */
	addr eip = read_eip();

	/* When we fork the child process, it will begin executing
	 * here, so we need to be able to tell whether or not we
	 * are executing as the parent task or the child task.  We
	 * can check current_task against parent_task to determine
	 * who we are */

	/* We could be the parent of the child here, check */
	if (current_task == parent_task)
	{
		/* We are the parent, so set up the esp / ebp / eip
		 * for our child */
		addr esp; asm volatile("mov %%esp, %0" : "=r"(esp));
		addr ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
		new_task->esp = esp;
		new_task->ebp = ebp;
		new_task->eip = eip;

		/* All finished; re-enable interrupts */
		asm volatile("sti");

		return new_task->id;
	}
	else
	{
		/* We are the child, by convention return 0 */
		return 0;
	}
}

/* Gets the current process ID */
int getpid()
{
	return current_task->id;
}

/* Switchs between tasks; called by the timer interrupt automatically */
void switch_task()
{
	/* If we haven't initialized tasking yet, just return */
	if (!current_task)
		return;

	/* Quickly grab the esp, ebp now for usage later on */
	addr esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	/* Read the instruction pointer.  We do some cunning logic here;
	 * as one of two things could have happened when this function exits:
	 *
	 * (a) We call the function and it returns EIP as requested.
	 * (b) We have just switched tasks, and because the saved EIP is
	 * essentially the instruction after read_eip(), it will seem as
	 * if read_eip has just returned.
	 * 
	 * In the second case we need to return immediately.  To detect it,
	 * we put a dummy value in EAX further down at the end of this
	 * function.  As C returns values in EAX, it will look like the
	 * return value is this dummy value (0x12345)! */
	eip = read_eip();

	/* Have we just switched tasks? */
	if (eip == 0x12345)
		return;

	/* No, we didn't switch tasks.  Let's save some register values
	 * and then switch */
	current_task->eip = eip;
	current_task->esp = esp;
	current_task->ebp = ebp;

	/* Go to the next task to run */
	current_task = current_task->next;
	if (!current_task) current_task = ready_queue;

	/* Store the new esp and ebp values */
	eip = current_task->eip;
	esp = current_task->esp;
	ebp = current_task->ebp;

	/* Make sure the memory manager knows we've changed
	 * page directory */
	current_directory = current_task->page_directory;

	/* Here we:
	 * - Stop interrupts so we don't get interrupted.
	 * - Temporarily put the new EIP location in ECX.
	 * - Load the stack and base pointers from the new
	 *   task structure.
	 * - Change page directory to the physical address
	 *   of the new page directory.
	 * - Put a dummy value (0x12345) in EAX so that above
	 *   we can recognise that we've just switched tasks.
	 * - Restart interrupts.  The STI instruction has a
	 *   delay; it doesn't take effect until after the
	 *   next instruction.
	 * - Jump to the location in ECX (where EIP is stored).
	 */
	asm volatile("				\
			cli;			\
			mov %0, %%ecx;		\
			mov %1, %%esp;		\
			mov %2, %%ebp;		\
			mov %3, %%cr3;		\
			mov $0x12345, %%eax;	\
			sti;			\
			jmp *%%ecx		"
			: : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->phys_addr));
}

/* Installs the task manager */
void task_install()
{
	puts("Enabling multitasking... ");

	/* Disable interrupts */
	asm volatile("cli");

	/* Relocate the stack so we know where it is */
	move_stack((void*)0xE0000000, 0x2000);

	/* Initialize the first task (kernel task) */
	current_task = ready_queue = (struct task*)kmalloc(sizeof(struct task));
	current_task->id = next_pid++;
	current_task->esp = current_task->ebp = 0;
	current_task->eip = 0;
	current_task->page_directory = current_directory;
	current_task->next = 0;

	/* Re-enable interrupts */
	asm volatile("sti");

	puts("done.\n");
}

#ifndef __PROCESS_H
#define __PROCESS_H

/* Structure definition of a process */
typedef struct process
{
	/* The page directory for the process */
	addr* page_directory;

} process_t;

/* PROCESS.C */
extern process_t* process_new();
extern void process_enter(struct process* proc);

#endif

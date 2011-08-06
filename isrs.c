#include <system.h>
#include <isrs.h>

/* These are function prototypes for all of the exception
 * handlers: The first 32 entries in the IDT are reserved
 * by Intel, and are designed to service exceptions! */
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();
extern void _isr80();

/* This is a very repetitive function... it's not hard, it's
 * just annoying.  As you can see, we set the first 32 entries
 * in the IDT to the first 32 ISRs.  We can't use a for loop
 * for this, because there is no way to get the function names
 * that correspond to that given entry.  We set the access flags
 * to 0x8E.  This means that the entry is present, is running
 * in ring 0 (kernel level), and has the lower 5 bits set to the
 * required '14' which is represented by 'E' in hex. */
void isrs_install()
{
	idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)_isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)_isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)_isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)_isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)_isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)_isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)_isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)_isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)_isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)_isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)_isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)_isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)_isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)_isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)_isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)_isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)_isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)_isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)_isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)_isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)_isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)_isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)_isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)_isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)_isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)_isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);
	idt_set_gate(0x80, (unsigned)_isr80, 0x08, 0x8E | 0x60);
}

/* This is a simple string array.  It contains the message that
 * corresponds to each and every exception.  We get the correct
 * message by accessing like:
 * exception_message[interrupt_number] */
unsigned char* exception_messages[] = 
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

/* All of the externally defined fault handlers that we don't
 * want the entire kernel to see.. probably could put these in
 * a header file though */
extern void _page_fault(struct regs* r);

/* All of our Exception handling Interrupt Service Routines will
 * point to this function.  This will tell us what exception has
 * happened!  Right now, we simply halt the system by hitting an
 * endless loop.  All ISRs disable interrupts while they are being
 * serviced as a 'locking' mechanism to prevent an IRQ from
 * happening and messing up kernel data structures */
void _fault_handler(struct regs r)
{
	/* Is this a fault we want to handle? */
	switch (r.int_no)
	{
		case 13:
			return;
		case 14:
			/* Page fault; send to page.c */
			_page_fault(&r);
			return;
		case 80:
			/* System call; send to syscall.c */
			_syscall_handler(&r);
			return;
		default:
			/* Not something we want to handle
			 * specially, so let it fall through
			 * to the if statement below */
			break;
	}

	/* Is this a fault whose number is from 0 to 31? */
	if (r.int_no < 32)
	{
		/* Display the description for the Exception that
		 * occurred.  In this tutorial, we will simply halt
		 * the system using an infinite loop. */
		putch('\n');
		settextcolor(4, 0);
		puts(exception_messages[r.int_no]);
		puts(" Exception.\nSystem Halted!\n\0");
		for (;;);
	}
}


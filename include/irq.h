#include <isrs.h>

#ifndef __IRQ_H
#define __IRQ_H

/* IRQ.C */
extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs* r));
extern void irq_uninstall_handler(int irq);

#endif

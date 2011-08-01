#include <system.h>
#include <timer.h>
#include <irq.h>

/* This will keep track of how many ticks that the system
 * has been running for */
int timer_ticks = 0;

/* Handles the timer.  In this case, it's very simple: We
 * increment the 'timer_ticks' variable every time the
 * timer fires.  By default, the timer fires 18.222 times
 * per second */
void timer_handler(struct regs* r)
{
	/* Increment our 'tick count' */
	timer_ticks++;
}

/* Sets the phase of the hardware timer */
void timer_phase(int hz)
{
	int divisor = 1193180 / hz;	/* Calculate our divisor */
	outportb(0x43, 0x36);		/* Set our command byte to 0x36 */
	outportb(0x40, divisor & 0xFF);	/* Set low byte of divisor */
	outportb(0x40, divisor >> 8);	/* Set high byte of divisor */
}

/* This will continously loop until the given time has
 * been reached */
void timer_wait(int ticks)
{
	unsigned long eticks;

	eticks = timer_ticks + ticks;
	while (timer_ticks < eticks);
}

/* Sets up the system clock by installing the timer
 * handler into IRQ0 */
void timer_install()
{
	/* Sets the phase so the timer fires every
	 * 100ms */
	timer_phase(100);

	/* Installs 'timer_handler' to IRQ0 */
	irq_install_handler(0, timer_handler);
}

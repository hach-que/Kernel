#include <system.h>
#include <irq.h>
#include <kb.h>

/* Keyboard constants */
#define KB_SHIFT 0x11
#define KB_ALT 0x12
#define KB_CTRL 0x13
#define KBD_LAYOUT kbd_us

/* Include keyboard layouts here */
#include <kbd/us.h>

/* Global indicator as to the status of
 * various keyboard controls */
unsigned short keystatus = 0x0000;

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs* r)
{
	unsigned char scancode;

	/* Read from the keyboard's data buffer */
	scancode = inportb(0x60);

	/* If the top bit of the byte we read from the keyboard is
	 * set, that means that a key has just been released */
	if (scancode & 0x80)
	{
		/* You can use this one to see if the user released
		 * shift, alt or control keys... */
		if (KBD_LAYOUT[scancode] == KB_SHIFT)
			keystatus &= ~0x0100;
		else if (KBD_LAYOUT[scancode] == KB_CTRL)
			keystatus &= ~0x0010;
		else if (KBD_LAYOUT[scancode] == KB_ALT)
			keystatus &= ~0x0001;
	}
	else
	{
		/* Here, a key was just pressed.  Please note that if
		 * you hold a key down, you will get repeated key press
		 * interrupts. */
		if (KBD_LAYOUT[scancode] == KB_SHIFT)
			keystatus |= 0x0100;
		else if (KBD_LAYOUT[scancode] == KB_CTRL)
			keystatus |= 0x0010;
		else if (KBD_LAYOUT[scancode] == KB_ALT)
			keystatus |= 0x0001;

		/* Example showing keyboard characters outputted to the
		 * screen */
		if ((keystatus & 0x0100) == 0x0100)
			putch(KBD_LAYOUT[scancode+128]);
		else
			putch(KBD_LAYOUT[scancode]);
	}
}

#undef KBD_LAYOUT

/* Sets up the keyboard by installing the handler
 * into IRQ1 */
void kb_install()
{
	/* Installs the IRQ handler */
	irq_install_handler(1, keyboard_handler);
}

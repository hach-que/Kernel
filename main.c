#include <system.h>

/* You will need to code these up yourself! */
unsigned char* memcpy(unsigned char* dest, const unsigned char* src, int count)
{
	/* Add code here to copy 'count' bytes of data from 'src' to
	 * 'dest', finally return 'dest' */
	int i;
	for (i = 0; i < count; i++)
		dest[i] = src[i];
	return dest;
}

unsigned char* memset(unsigned char* dest, unsigned char val, int count)
{
	/* Add code here to set 'count' bytes in 'dest' to 'val'.
	 * Again, return 'dest' */
	int i;
	for (i = 0; i < count; i++)
		dest[i] = val;
	return dest;
}

unsigned short* memsetw(unsigned short* dest, unsigned short val, int count)
{
	/* Same as above, but this time, we're working with a 16-bit
	 * 'val' and dest pointer.  Your code can be an exact copy of
	 * the above, provided that your local variables if any, are
	 * unsigned short */
	int i;
	for (i = 0; i < count; i++)
		dest[i] = val;
	return dest;
}

int strlen(const unsigned char* str)
{
	/* This loops through character array 'str', returning how
	 * many characters it needs to check before it finds a 0.
	 * In simple words, it returns the length in bytes of a string */
	
	// strlen implementation from FreeBSD 6.2
	const unsigned char* s;
	for (s = str; *s; ++s);
	return (s - str);
}

/* We will use this later on for reading from the I/O ports to get data
 * from devices such as the keyboard.  We are using what is called
 * 'inline assembly' in these routines to actually do the work */
unsigned char inportb(unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

/* We will use this to write to I/O ports to send bytes to devices.  This
 * will be used in the next tutorial for changing the textmode cursor
 * position.  Again, we use some inline assembly for the stuff that simply
 * cannot be done in C */
void outportb(unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

/* This is a very simple main() function.  All it does is sit in an
 * infinite loop.  This will be like our 'idle' loop */
void main()
{
	unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', 0 };

	/* You would add commands after here */
	init_video();
	putch('H');
	puts(msg);
	puts("Hello World Again!\0");

	/* ...and leave this loop in.  There is an endless loop in
	 * 'start.asm' also, if you accidently delete this next line */
	for (;;);
}


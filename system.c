#include <system.h>
#include <scrn.h>

void panic(unsigned char* msg)
{
	putch('\n');
	settextcolor(4, 0);
	puts(msg);
	putch('\n');
	for(;;);
}


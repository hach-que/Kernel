#include "kernel.h"

int entry()
{
	write("This is a userland application.\n");
	write("This is another message.\n");
	return 0;
}

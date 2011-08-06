#include "kernel.h"

int entry()
{
	int i;
	unsigned char itoa_buffer[256];

	write("This is done using write.\n");
	for (i = 0; i < 100; i++)
	{
		write(itoa(i, itoa_buffer, 10));
		write(".. ");
	}
	return 0;
}

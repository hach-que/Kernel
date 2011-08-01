#include <system.h>
#include <string.h>

/* String length function */
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

/* String reverse function
 * Sourced from: http://www.daniweb.com/software-development/c/threads/11049 */
unsigned char* strrev(unsigned char* str)
{
	unsigned char* p1;
	unsigned char* p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}

	return str;
}

/* Integer to string function
 * Sourced from: http://www.daniweb.com/software-development/c/threads/11049 */
unsigned char* itoa(unsigned long num, unsigned char* str, int base)
{
	static unsigned char digits[] = "0123456789abcdefghigjklmnopqrstuvwxyz";
	unsigned long i = 0, sign;

	if ((sign = num) < 0)
		num = -num;

	do
	{
		str[i++] = digits[num % base];
	} while ((num /= base) > 0);

	if (sign < 0)
		str[i++] = '-';
	str[i] = '\0';

	return strrev(str);
}

#ifndef __STRING_H
#define __STRING_H

/* STRING.C */
extern int strlen(const unsigned char* str);
unsigned char* strrev(unsigned char* str);
unsigned char* itoa(unsigned long num, unsigned char* str, int base);

#endif

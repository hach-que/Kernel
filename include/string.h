#ifndef __STRING_H
#define __STRING_H

/* STRING.C */
extern int strlen(const unsigned char* str);
extern int strcpy(unsigned char* dest, const unsigned char* src, unsigned int size);
extern int strcmp(const unsigned char* a, const unsigned char* b);
extern unsigned char* strrev(unsigned char* str);
extern unsigned char* itoa(unsigned long num, unsigned char* str, int base);

#endif

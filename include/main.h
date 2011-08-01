#ifndef __MAIN_H
#define __MAIN_H

/* MAIN.C */
extern void* memcpy(void* dest, const void* src, int count);
extern void* memset(void* dest, unsigned char val, int count);
extern unsigned short* memsetw(unsigned short* dest, unsigned short val, int count);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);

#endif

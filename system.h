#ifndef __SYSTEM_H
#define __SYSTEM_H

/* MAIN.C */
extern unsigned char* memcpy(unsigned char* dest, const unsigned char* src, int count);
extern unsigned char* memset(unsigned char* dest, unsigned char val, int count);
extern unsigned short* memsetw(unsigned short* dest, unsigned short val, int count);
extern int strlen(const unsigned char* str);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);

/* SCRN.C */
extern void cls();
extern void putch(unsigned char c);
extern void puts(unsigned char* str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

#endif

#ifndef __SCRN_H
#define __SCRN_H

/* SCRN.C */
extern void cls();
extern void putch(unsigned char c);
extern void puts(unsigned char* str);
extern void putp(addr pointer);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

#endif

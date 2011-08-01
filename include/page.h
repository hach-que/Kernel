#ifndef __PAGE_H
#define __PAGE_H

/* PAGE.C */
extern void page_install();
extern void page_switch(addr* dir);
extern addr* page_table_new(addr address, addr flags);

#endif

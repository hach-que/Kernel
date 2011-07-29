#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

/* Multiboot structure definitions for information passed
 * by GRUB to the main() function */
struct multiboot_header
{
	unsigned long magic, flags, checksum;
	unsigned long header_addr;
	unsigned long load_addr, load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
} __attribute__((packed));

struct aout_symbol_table
{
	unsigned long tabsize;
	unsigned long strsize;
	unsigned long addr;
	unsigned long reserved;
} __attribute__((packed));

struct multiboot_info
{
	unsigned long flags;
	unsigned long mem_lower, mem_upper;
	unsigned long boot_device;
	unsigned long cmdline;
	unsigned long mods_count, mods_addr;
	struct aout_symbol_table symbols;
	unsigned long mmap_length;
	unsigned long mmap_addr;
} __attribute__((packed));

struct multiboot_memory_map
{
	unsigned int size;
	unsigned long long int base_addr;
	unsigned long long int length;
	unsigned int type;
} __attribute__((packed));

#endif

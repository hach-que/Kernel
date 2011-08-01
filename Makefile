# Makefile for custom kernel.

# Architecture flags (doesn't change the architecture set
# inside the assembly file).
ARCH=i386
ARCH_FLAGS=-march=$(ARCH) -m32

# General flags for tools.
CC_FLAGS=$(ARCH_FLAGS) -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include/ -c
LD_FLAGS=-m elf_$(ARCH)

all:
	@echo Assembling start.asm...
	@nasm -f aout -o out/start.o start.asm
	
	@# We will add 'gcc' commands here later to compile
	@# C sources.
	@echo Compiling main.c...
	@gcc $(CC_FLAGS) -o out/main.o main.c
	@echo Compiling string.c...
	@gcc $(CC_FLAGS) -o out/string.o string.c
	@echo Compiling scrn.c...
	@gcc $(CC_FLAGS) -o out/scrn.o scrn.c
	@echo Compiling gdt.c...
	@gcc $(CC_FLAGS) -o out/gdt.o gdt.c
	@echo Compiling idt.c...
	@gcc $(CC_FLAGS) -o out/idt.o idt.c
	@echo Compiling isrs.c...
	@gcc $(CC_FLAGS) -o out/isrs.o isrs.c
	@echo Compiling irq.c...
	@gcc $(CC_FLAGS) -o out/irq.o irq.c
	@echo Compiling timer.c...
	@gcc $(CC_FLAGS) -o out/timer.o timer.c
	@echo Compiling kb.c...
	@gcc $(CC_FLAGS) -o out/kb.o kb.c
	@echo Compiling mem.c...
	@gcc $(CC_FLAGS) -o out/mem.o mem.c
	@echo Compiling page.c...
	@gcc $(CC_FLAGS) -o out/page.o page.c
	@echo Compiling tss.c...
	@gcc $(CC_FLAGS) -o out/tss.o tss.c
	@echo Compiling api.c...
	@gcc $(CC_FLAGS) -o out/api.o api.c
	@echo Compiling process.c...
	@gcc $(CC_FLAGS) -o out/process.o process.c
	@echo Compiling app/entry.c
	@gcc $(CC_FLAGS) -o out/app/entry.o app/entry.c
	@echo Compiling app/lib.c
	@gcc $(CC_FLAGS) -o out/app/lib.o app/lib.c

	@# Remember to add .o files to the end of this command
	@# as more C source files are added.
	@echo Linking kernel...
	@ld $(LD_FLAGS) -T link.ld -o out/kernel.bin out/start.o out/scrn.o out/gdt.o out/idt.o out/isrs.o out/irq.o out/timer.o out/kb.o out/mem.o out/string.o out/page.o out/tss.o out/api.o out/process.o out/app/entry.o out/app/lib.o out/main.o
	@echo Copying kernel to floppy folder...
	@cp out/kernel.bin floppy/boot/kernel.bin
	@echo Flushing floppy drive contents...
	@sudo /bin/umount /dev/fd0
	@sudo /bin/mount -o users,uid=1000 /dev/fd0 floppy
	@echo Kernel build complete.

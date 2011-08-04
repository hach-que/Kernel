# Makefile for custom kernel.

# Architecture flags (doesn't change the architecture set
# inside the assembly file).
ARCH=i386
ARCH_FLAGS=-march=$(ARCH) -m32

# General flags for tools.
CC_FLAGS=$(ARCH_FLAGS) -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include/ -I./vfs/ -c
LD_FLAGS=-m elf_$(ARCH)

all: kernel mkinitrd

kernel: force_look
	@mkdir out 2>/dev/null; true
	@mkdir out/vfs 2>/dev/null; true
	@mkdir out/types 2>/dev/null; true
	@mkdir out/app 2>/dev/null; true
	
	@# Assemble the entry point
	@echo Assembling start.asm...
	@nasm -f aout -o out/start.o start.asm
	
	@# We will add 'gcc' commands here later to compile
	@# C sources.
	@echo Compiling main.c...
	@gcc $(CC_FLAGS) -o out/main.o main.c
	@echo Compiling system.c...
	@gcc $(CC_FLAGS) -o out/system.o system.c
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
	@echo Compiling frame.c...
	@gcc $(CC_FLAGS) -o out/frame.o frame.c
	@echo Compiling kmem.c...
	@gcc $(CC_FLAGS) -o out/kmem.o kmem.c
	@echo Compiling vmem.c...
	@gcc $(CC_FLAGS) -o out/vmem.o vmem.c
	@echo Compiling page.c...
	@gcc $(CC_FLAGS) -o out/page.o page.c
	@echo Compiling tss.c...
	@gcc $(CC_FLAGS) -o out/tss.o tss.c
	@echo Compiling vfs.c...
	@gcc $(CC_FLAGS) -o out/vfs.o vfs.c
	@echo Compiling syscall.c...
	@gcc $(CC_FLAGS) -o out/syscall.o syscall.c
	@echo Compiling app/entry.c...
	@gcc $(CC_FLAGS) -o out/app/entry.o app/entry.c
	@echo Compiling app/lib.c...
	@gcc $(CC_FLAGS) -o out/app/lib.o app/lib.c
	@echo Compiling types/ordered_array.c...
	@gcc $(CC_FLAGS) -o out/types/ordered_array.o types/ordered_array.c
	@echo Compiling vfs/initrd.c...
	@gcc $(CC_FLAGS) -o out/vfs/initrd.o vfs/initrd.c
	
	@# Remember to add .o files to the end of this command
	@# as more C source files are added.
	@echo Linking kernel...
	@ld $(LD_FLAGS) -T link.ld -o out/kernel.bin out/start.o out/scrn.o out/gdt.o out/idt.o out/isrs.o out/irq.o out/timer.o out/kb.o out/kmem.o out/vmem.o out/string.o out/page.o out/tss.o out/syscall.o out/app/entry.o out/app/lib.o out/main.o out/types/ordered_array.o out/system.o out/frame.o out/vfs.o out/vfs/initrd.o
	@echo Copying kernel to floppy folder...
	@cp out/kernel.bin floppy/boot/kernel.bin
	@echo Flushing floppy drive contents...
	@sudo /bin/umount /dev/fd0
	@sudo /bin/mount -o users,uid=1000 /dev/fd0 floppy
	@echo Kernel build complete.

mkinitrd: force_look
	@cd mkinitrd; make; cd ..

force_look:
	@true

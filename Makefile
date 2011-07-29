# Makefile for custom kernel.

# Architecture flags (doesn't change the architecture set
# inside the assembly file).
ARCH=i386
ARCH_FLAGS=-march=$(ARCH) -m32

# General flags for tools.
CC_FLAGS=$(ARCH_FLAGS) -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./ -c
LD_FLAGS=-m elf_$(ARCH)

all:
	@echo Assembling start.asm...
	@nasm -f aout -o out/start.o start.asm
	
	@# We will add 'gcc' commands here later to compile
	@# C sources.
	@echo Compiling main.c...
	@gcc $(CC_FLAGS) -o out/main.o main.c
	@echo Compiling scrn.c...
	@gcc $(CC_FLAGS) -o out/scrn.o scrn.c
	
	@# Remember to add .o files to the end of this command
	@# as more C source files are added.
	@echo Linking kernel...
	@ld $(LD_FLAGS) -T link.ld -o out/kernel.bin out/start.o out/scrn.o out/main.o
	@echo Copying kernel to floppy folder...
	@cp out/kernel.bin floppy/boot/kernel.bin
	@echo Kernel build complete.

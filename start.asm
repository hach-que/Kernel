; This is the kernel's entry point.  We could either call main here,
; or we can use this to setup the stack or other nice stuff, like
; perhaps setting up the GDT and segments.  Please note that interrupts
; are disabled at this point:  More on interrupts later!
[BITS 32]
global start
start:
	mov esp, _sys_stack	; This points the stack to our new stack area
	jmp stublet

; This part MUST be 4 byte aligned, so we solve that issue using 'ALIGN 4'
ALIGN 4
mboot:
	; Multiboot macros to make a few lines later more readable
	MULTIBOOT_PAGE_ALIGN	equ 1<<0
	MULTIBOOT_MEMORY_INFO	equ 1<<1
	MULTIBOOT_AOUT_KLUDGE	equ 1<<16
	MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
	MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
	MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	EXTERN	code, bss, end

	; This is the GRUB multiboot header.  A boot signature
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

	; AOUT kludge - must be physical addresses.  Make a note of these:
	; the linker script fills in the data for these ones!
	dd mboot
	dd code
	dd bss
	dd end
	dd start

; This is an endless loop here.  Make a note of this:  Later on, we
; will insert an 'extern _main', followed by a 'call _main', right
; before the 'jmp $'.
stublet:
	push eax		; Pushs the magic number onto the stack
	push ebx		; Pushs the 32-bit address of the multiboot info structure to the stack
	extern _main
	call _main
	jmp $

; This will set up our new segment registers.  We need to do
; something special in order to set CS.  We do what is called
; a far jump.  A jump that includes a segment as well as an
; offset.  This is declared in C as 'extern void gdt_flush();'
global _gdt_flush	; Allows the C code to link to this
extern _gp		; Says that '_gp' is in another file
_gdt_flush:
	lgdt [_gp]	; Load the GDT with our '_gp' which is a special pointer
	mov ax, 0x10	; 0x10 is the offset in the GDT to our data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:flush2	; 0x08 is the offset to our code segment: Far jump!
flush2:
	ret		; Returns back to the C code!

; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void _idt_load();'
global _idt_load
extern _idtp
_idt_load:
	lidt [_idtp]
	ret

; Define the ISRS handlers and the ISRS stub code.
%include "isrs.asm"

; Define the IRQ handlers and the IRQ stub code.
%include "irq.asm"

; Here is the definition of our BSS section.  Right now, we'll use
; it just to store the stack.  Remember that a stack actually grows
; downwards, so we declare the size of the data before declaring
; the identifier '_sys_stack'
SECTION .bss
	resb 8192	; This reserves 8 kilobytes of memory here
_sys_stack:


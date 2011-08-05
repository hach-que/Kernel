[GLOBAL copy_page_physical]
copy_page_physical:
	push ebx		; According to __cdecl, we must preserve the contents
				; of EBX.
	pushf			; Push EFLAGS so we can pop it and re-enable interrupts
				; later, if they were enabled anyway.
	cli			; Disable interrupts, so we aren't interrupted.
				; Load these in before we disable paging!
	mov ebx, [esp+12]	; Source address
	mov ecx, [esp+16]	; Destination address
	
	mov edx, cr0		; Get the control register...
	and edx, 0x7fffffff	; and...
	mov cr0, edx		; disable paging.

	mov edx, 1024		; 1024 * 4 bytes = 4096 bytes to copy

.loop:
	mov eax, [ebx]		; Get the word at the source address
	mov [ecx], eax		; Store it at the destination address
	add ebx, 4		; Source address += sizeof(word)
	add ecx, 4		; Destination address += sizeof(word)
	dec edx			; One less word to do
	jnz .loop

	mov edx, cr0		; Get the control register again...
	or edx, 0x80000000	; and...
	mov cr0, edx		; enable paging.
	
	popf			; Pop EFLAGS back.
	pop ebx			; Get the original value of EBX back.
	ret

[GLOBAL read_eip]
read_eip:
	pop eax
	jmp eax

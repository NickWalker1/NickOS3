
PageTableEntry equ 0x1000

switchToLongMode:
    call checkCPUID

    call checkLongMode

    call setupPaging

    call EditGDT

    ret

checkCPUID:
    ;get flags into eax
    pushfd
    pop eax

    ; copy eax into ecx
    mov ecx, eax

    ; flip bit 21
    xor eax, 1<<21

    ; push eax back into stack
    push eax
    ; pop into flags
    popfd

    ; return to original values
    push ecx
    popfd

    ; see if equal to see if bit stayed flipped
    xor eax,ecx
    jz NoCPUID
    ret

checkLongMode:
    mov eax, 0x80000001
    cpuid
    test edx, 1<<29
    jz NoLongMode
    ret

setupPaging:
    mov edi, PageTableEntry
	mov cr3, edi

	mov dword [edi], 0x2003
	add edi, 0x1000
	mov dword [edi], 0x3003
	add edi, 0x1000
	mov dword [edi], 0x4003
	add edi, 0x1000

	mov ebx, 0x00000003
	mov ecx, 512

	.SetEntry:
		mov dword [edi], ebx
		add ebx, 0x1000
		add edi, 8
		loop .SetEntry

	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	ret


NoLongMode:
    mov ebx, MSG_NO_LONGMODE
    call printString32
    jmp $

NoCPUID:
    mov ebx, MSG_NO_CPUID
    call printString32
    jmp $


MSG_NO_LONGMODE db "Longmode not supported",0
MSG_NO_CPUID    db "CPUID not supported",0

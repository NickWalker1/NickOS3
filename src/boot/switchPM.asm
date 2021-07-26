[bits 16]

%include "detectMemory.asm"

; Switch to protected mode
switchToProtMode:
    
    ; WHy the fuck this uses 32 bit registers and breaks
    ; in 32bit mode I'll never know but it works ha
    call DetectMemory
    
    ; Set VGA mode to be normal
    mov ax,0x3
    int 0x10

    cli ; Switch off interups
    call enableA20

    lgdt [gdt_descriptor]

    mov eax, cr0                ; To switch to protected mode must 
    or  eax, 0x1                ; set first bit of CR0 to 1
    mov cr0, eax 

    jmp CODE_SEQ:initProtMode   ; Make a far jump to flush pipeline

enableA20:
	in al, 0x92
	or al, 2
	out 0x92, al
	ret

[bits 32]


initProtMode:
    mov ax, DATA_SEQ        ; Now in PM< old segments are meaningless
    mov ds, ax              ; use segment registers to point to the data selector
    mov ss, ax              ; defined in the GDT
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;pre paging code
    ;mov ebp, 0x90000        ; Update the stack pointer so it is right
    ;mov esp,ebp             ; at the top of the free space


    jmp BEGIN_PM            ; call some well-known label


[bits 16]

    mov bx, MSG_BOOT
    call printString16

    call switchToProtMode


%include "print.asm"
%include "gdt.asm"
%include "switchPM.asm"



[bits 32]
[extern main]
; Where we arrive after switching to protected mode

%include "paging.asm"


BEGIN_PM:

    mov ebx, MSG_PROT_MODE
    call printString32

    call clear_page_directory
    call clear_page_tables

    call create_page_directory
    call create_identity_page_table
    call create_kernel_page_table
    call enable_paging

    ; Set new stack with virtual address
    ; starts right beneath where the code is loaded, and right after the page directory.
    ; 0xC0007000-0xC0008000 will store the page for the kernel thread
    mov eax, 0xC0008000 
    mov esp, eax

    ; Init FPU
    finit
    

    ; Jump to main kernel entry point in C
    jmp main

    ; Main should never return, but if it does: spin

    jmp $



MSG_PROT_MODE   db "Successfully in 32-bit protected mode with paging",0
MSG_BOOT db "Successfully loaded from floppy disk",0
MSG_PAGING db "Now using Paging",0

times 2048-($-$$) db 0
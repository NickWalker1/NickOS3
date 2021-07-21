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


BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call printString32


    ; Disable the pic
    mov al, 0xff
    out 0xa1, al
    out 0x21, al

    jmp main



MSG_PROT_MODE   db "Successfully in 32-bit protected mode",0
MSG_BOOT db "Successfully loaded from floppy disk",0

times 2048-($-$$) db 0
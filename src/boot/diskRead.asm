BOOTLOADER_ENTRY equ 0x8000

ReadDisk:

    mov ah, 0x02
    mov bx, BOOTLOADER_ENTRY
    mov al, 32
    mov dl, [BOOT_DISK]
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

    int 0x13
    jc DiskReadFail

    ret

%include "print.asm"

DiskReadErrorString:
    db "Disk Read Failed",0

DiskReadFail:
    mov bx, DiskReadErrorString
    call printString16


BOOT_DISK: db 0

curr_offset equ 20
next_offset equ 24

global context_switch
; thread* context_switch(thread *cur, thread *next);
; pushes registers 
;
context_switch:
    ; Save old callee-preserved registers
    ; No need to store eax-edx as they are handled 
    ; by the interrupt trap gate
    push  ebp
    push  ebx
    push  esi
    push  edi


    ; Store the current stack pointer in the Thread struct    
    mov edx, 0 ; currently just have stack* at offset 0 in struct but this can change
    mov eax, [curr_offset+esp]
    mov [eax+edx*1],esp


    ; Load the stack pointer of the next Thread struct
    mov ecx, [next_offset+esp]
    mov esp, [ecx+edx*1]


    ; Load new callee-preserved registers
    pop  edi
    pop  esi
    pop  ebx
    pop  ebp

    ret



first_switch:
    add esp, 8 ; get rid of switch thread arguments
    
    extern schedule_tail

    push eax ; TODO figure out why this is needed
    call schedule_tail

    add esp,4

    ;start thread properly
    ret


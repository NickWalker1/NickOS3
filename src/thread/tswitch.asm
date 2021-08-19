%include "switch.h"

global context_switch
; context* context_switch(context *old, context *new);
; pushes registers 
;
context_switch:
    ; Save old callee-save registers
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


    ; Load new callee-save registers
    pop  edi
    pop  esi
    pop  ebx
    pop  ebp

    ret






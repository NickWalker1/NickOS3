#pragma once

#include "thread.h"

#include "../lib/typedefs.h"

//TODO UPDATE THIS
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in tswitch.asm
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
typedef struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
    thread* curr;
    thread* next;
}context;

/* Defined in tswitch.asm. It pushes the save regsiters onto the stack, uses the 
 * context structs to store the current esp in the current threads struct,
 * then uses the next structs into to load the stack value and pop the save registers */
context* context_switch(context* cur, context* next);


#define curr_offset 20
#define next_offset 24
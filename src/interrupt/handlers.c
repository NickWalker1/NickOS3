#include "handlers.h"

static exception_definition exceptions[];

const char ASCIITable[] = {
        0 ,  0 , '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=',  0 ,  0 ,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
        0 ,  0 , 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'','`',  0 , '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/',  0 , '*',
        0 , ' '
    };

void default_exception_handler(exception_state* state){

    PANIC_EXC(exceptions[state->interrupt_number].description,state);
}

void keyboard_handler(interrupt_state* state){
    uint8_t scancode=inportb(0x60);
    print_char_loc(ASCIITable[scancode],0,0,GREEN_ON_BLACK);

}


static exception_definition exceptions[] =
    {
        {
            .interrupt_number = 0,
            .description = "Divide-by-zero error",
        },
        {
            .interrupt_number = 1,
            .description = "Debug",
        },
        {
            .interrupt_number = 2,
            .description = "Non-maskable Interrupt",
        },
        {
            .interrupt_number = 3,
            .description = "Breakpoint",
        },
        {
            .interrupt_number = 4,
            .description = "Overflow",
        },
        {
            .interrupt_number = 5,
            .description = "Bound Range Exceeded",
        },
        {
            .interrupt_number = 6,
            .description = "Invalid Opcode",
        },
        {
            .interrupt_number = 7,
            .description = "Device Not Available",
        },
        {
            .interrupt_number = 8,
            .description = "Double Fault",
        },
        {
            .interrupt_number = 9,
            .description = "Coprocessor Segment Overrun",
        },
        {
            .interrupt_number = 10,
            .description = "Invalid TSS",
        },
        {
            .interrupt_number = 11,
            .description = "Segment Not Present",
        },
        {
            .interrupt_number = 12,
            .description = "Stack-Segment Fault",
        },
        {
            .interrupt_number = 13,
            .description = "General Protection Fault",
        },
        {
            .interrupt_number = 14,
            .description = "Page Fault",
        },
        {
            .interrupt_number = 15,
            .description = "Reserved",
        },
        {
            .interrupt_number = 16,
            .description = "x87 Floating-Point Exception",
        },
        {
            .interrupt_number = 17,
            .description = "Alignment Check",
        },
        {
            .interrupt_number = 18,
            .description = "Machine Check",
        },
        {
            .interrupt_number = 19,
            .description = "SIMD Floating-Point Exception",
        },
        {
            .interrupt_number = 20,
            .description = "Virtualization Exception",
        },
        {
            .interrupt_number = 21,
            .description = "Reserved",
        },
        {
            .interrupt_number = 22,
            .description = "Reserved",
        },
        {
            .interrupt_number = 23,
            .description = "Reserved",
        },
        {
            .interrupt_number = 24,
            .description = "Reserved",
        },
        {
            .interrupt_number = 25,
            .description = "Reserved",
        },
        {
            .interrupt_number = 26,
            .description = "Reserved",
        },
        {
            .interrupt_number = 27,
            .description = "Reserved",
        },
        {
            .interrupt_number = 28,
            .description = "Reserved",
        },
        {
            .interrupt_number = 29,
            .description = "Reserved",
        },
        {
            .interrupt_number = 30,
            .description = "Security Exception",
        },
        {
            .interrupt_number = 31,
            .description = "Reserved",
        }};

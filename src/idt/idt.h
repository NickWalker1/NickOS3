#pragma once
#include "../lib/typedefs.h"
#include "cpu_state.h"

#define IDT_MAX_DESCRIPTORS 256

typedef struct idt_entry
{
    uint16_t isr_low;				// The lower 16 bits of the ISR's address
    uint16_t kernel_cs;				// The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t reserved;				// Set to zero
    uint8_t type : 4;				// Type and attributes; see the IDT page
    uint8_t storage_segment : 1;
    uint8_t privilege_level : 2;
    uint8_t present : 1;
    uint16_t isr_high; 				// The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry;

__attribute__((aligned(0x10)))
static idt_entry idt[256];

typedef struct {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed)) idtr_t;

typedef struct exception_definition
{
    char description[32];
    uint8_t interrupt_number;
} exception_definition;

static idtr_t idtr;

__attribute__((noreturn))
void exception_handler(void);
void idt_set_descriptor(uint8_t vector, uint32_t (*handler)(interrupt_state *state), bool user_interrupt);
void idt_init(void);


//Exceptions
extern uint32_t idt_exc0();
extern uint32_t idt_exc1();
extern uint32_t idt_exc2();
extern uint32_t idt_exc3();
extern uint32_t idt_exc4();
extern uint32_t idt_exc5();
extern uint32_t idt_exc6();
extern uint32_t idt_exc7();
extern uint32_t idt_exc8();
extern uint32_t idt_exc9();
extern uint32_t idt_exc10();
extern uint32_t idt_exc11();
extern uint32_t idt_exc12();
extern uint32_t idt_exc13();
extern uint32_t idt_exc14();
extern uint32_t idt_exc15();
extern uint32_t idt_exc16();
extern uint32_t idt_exc17();
extern uint32_t idt_exc18();
extern uint32_t idt_exc19();
extern uint32_t idt_exc20();
extern uint32_t idt_exc21();
extern uint32_t idt_exc22();
extern uint32_t idt_exc23();
extern uint32_t idt_exc24();
extern uint32_t idt_exc25();
extern uint32_t idt_exc26();
extern uint32_t idt_exc27();
extern uint32_t idt_exc28();
extern uint32_t idt_exc29();
extern uint32_t idt_exc30();
extern uint32_t idt_exc31();

// Hardware interrupts
extern uint32_t idt_int32();
extern uint32_t idt_int33();
extern uint32_t idt_int34();
extern uint32_t idt_int35();
extern uint32_t idt_int36();
extern uint32_t idt_int37();
extern uint32_t idt_int38();
extern uint32_t idt_int39();
extern uint32_t idt_int40();
extern uint32_t idt_int41();
extern uint32_t idt_int42();
extern uint32_t idt_int43();
extern uint32_t idt_int44();
extern uint32_t idt_int45();
extern uint32_t idt_int46();
extern uint32_t idt_int47();

// Software interrupts
extern uint32_t idt_int50();
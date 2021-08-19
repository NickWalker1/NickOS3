#include "idt.h"
#include "../lib/screen.h"
#include "cpu_state.h"
#include "../lib/int.h"

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


void idt_set_descriptor(uint8_t vector, uint32_t (*handler)(interrupt_state *state), bool user_interrupt){
    idt_entry* descriptor = &idt[vector];
    uint32_t handler_addr = (uint32_t)handler;

    descriptor->isr_low         =handler_addr & 0xFFFF;
    descriptor->kernel_cs       =8; //change this to where kernel offset is if I set that up later or something
    descriptor->isr_high        =(uint32_t)handler_addr>>16;
    descriptor->reserved        =0;
    descriptor->present         =1;
    descriptor->privilege_level = user_interrupt ? 3 : 1;
    descriptor->type            = 15; //32bit Trap, change to enum struct thingy later
}

void idt_init(){
    idtr.base=(uint32_t)&idt;
    idtr.limit=(uint16_t)sizeof(idt_entry) * IDT_MAX_DESCRIPTORS -1;

    // Exceptions
    idt_set_descriptor(0, idt_exc0, false);
    idt_set_descriptor(1, idt_exc1, false);
    idt_set_descriptor(2, idt_exc2, false);
    idt_set_descriptor(3, idt_exc3, false);
    idt_set_descriptor(4, idt_exc4, false);
    idt_set_descriptor(5, idt_exc5, false);
    idt_set_descriptor(6, idt_exc6, false);
    idt_set_descriptor(7, idt_exc7, false);
    idt_set_descriptor(8, idt_exc8, false);
    idt_set_descriptor(9, idt_exc9, false);
    idt_set_descriptor(10, idt_exc10, false);
    idt_set_descriptor(11, idt_exc11, false);
    idt_set_descriptor(12, idt_exc12, false);
    idt_set_descriptor(13, idt_exc13, false);
    idt_set_descriptor(14, idt_exc14, false);
    idt_set_descriptor(15, idt_exc15, false);
    idt_set_descriptor(16, idt_exc16, false);
    idt_set_descriptor(17, idt_exc17, false);
    idt_set_descriptor(18, idt_exc18, false);
    idt_set_descriptor(19, idt_exc19, false);
    idt_set_descriptor(20, idt_exc20, false);
    idt_set_descriptor(21, idt_exc21, false);
    idt_set_descriptor(22, idt_exc22, false);
    idt_set_descriptor(23, idt_exc23, false);
    idt_set_descriptor(24, idt_exc24, false);
    idt_set_descriptor(25, idt_exc25, false);
    idt_set_descriptor(26, idt_exc26, false);
    idt_set_descriptor(27, idt_exc27, false);
    idt_set_descriptor(28, idt_exc28, false);
    idt_set_descriptor(29, idt_exc29, false);
    idt_set_descriptor(30, idt_exc30, false);
    idt_set_descriptor(31, idt_exc31, false);

    // Hardware interrupts (IRQ)
    idt_set_descriptor(32, idt_int32, false); // Programmable Interrupt Timer
    idt_set_descriptor(33, idt_int33, false); // Keyboard
    idt_set_descriptor(34, idt_int34, false); // Cascade
    idt_set_descriptor(35, idt_int35, false); // COM2
    idt_set_descriptor(36, idt_int36, false); // COM1
    idt_set_descriptor(37, idt_int37, false); // LPT2
    idt_set_descriptor(38, idt_int38, false); // Floppy
    idt_set_descriptor(39, idt_int39, false); // LPT1
    idt_set_descriptor(40, idt_int40, false); // CMOS
    idt_set_descriptor(41, idt_int41, false); // Free
    idt_set_descriptor(42, idt_int42, false); // Free
    idt_set_descriptor(43, idt_int43, false); // Free
    idt_set_descriptor(44, idt_int44, false); // Mouse
    idt_set_descriptor(45, idt_int45, false); // FPU
    idt_set_descriptor(46, idt_int46, false); // Primary ATA Hard Disk
    idt_set_descriptor(47, idt_int47, false); // Secondary ATA Hard Disk

    // Software interrupts
    idt_set_descriptor(50, idt_int50, true);

    irq_remap();

    __asm__ volatile("lidt %0" : : "memory"(idtr)); //load the new IDT
    __asm__ volatile("sti"); //Set interrupt flag

}


/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}


void int_set_level(int level){
    if(level)int_enable();
}

int int_get_level(){
    uint32_t flags;

    __asm__ volatile("pushfl; pop %0" : "=g" (flags));
    return flags & 0x200 ? 1 : 0;
}

/* enables interrupts and returns previous status */
int int_enable(){
    int level = int_get_level();
    __asm__ volatile("sti");
    return level;
}

/* disables interrupts and returns previous status */
int int_disable(){
    int level = int_get_level();
    __asm__ volatile("cli");
    return level;
}

void page_fault_handler(exception_state *state){
    //TODO make swap pages

}

void idt_global_int_handler(interrupt_state *state){
    if(state->interrupt_number==32) timer_handler(state);
        /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (state->interrupt_number >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);
}

void idt_global_exc_handler(exception_state *state){
    println("EXCEPTION: ");
    if(state->interrupt_number==8){
        println("DOUBLE FAULT");
        __asm__ volatile("cli;hlt");
    }
    state_dump(state);
    __asm__ volatile("cli;hlt");
}

    /*
    uint8_t code=state->error_code&0b1111111111111110;
    print(itoa(state->interrupt_number,str,BASE_DEC));
    print(" : ");
    print(exceptions[state->interrupt_number].description);
    print(" : ");
    print(itoa(code,str,BASE_BIN));
    */
    

void state_dump(exception_state *state){
    char str[128];
    println("---CORE DUMP---");
    println("IDTR:");
    print(itoa(state->idtr,str,BASE_HEX));
    println("GDTR:");
    print(itoa(state->gdtr,str,BASE_HEX));
    println("GS:");
    print(itoa(state->gs,str,BASE_HEX));
    println("FS:");
    print(itoa(state->fs,str,BASE_HEX));
    println("ES:");
    print(itoa(state->es,str,BASE_HEX));
    println("CR4:");
    print(itoa(state->cr4,str,BASE_BIN));
    println("CR3:");
    print(itoa(state->cr3,str,BASE_BIN));
    println("CR2:");
    print(itoa(state->cr2,str,BASE_HEX));
    println("CR0:");
    print(itoa(state->cr0,str,BASE_BIN));
    println("DS:");
    print(itoa(state->ds,str,BASE_HEX));
    println("INT_NUM:");
    print(itoa(state->interrupt_number,str,BASE_DEC));
    println("ERROR_CODE:");
    print(itoa(state->error_code,str,BASE_BIN));
    println("EIP:");
    print(itoa(state->eip,str,BASE_HEX));
    println("CS:");
    print(itoa(state->cs,str,BASE_HEX));
    println("EFLAGS:");
    print(itoa(state->eflags,str,BASE_HEX));
    println("ESP:");
    print(itoa(state->esp,str,BASE_HEX));
    println("SS:");
    print(itoa(state->ss,str,BASE_HEX));

}

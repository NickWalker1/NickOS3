#include "idt.h"
#include "../lib/screen.h"

extern void* isr_stub_table[];
extern void default_exception_handler();
extern void default_interrupt_handler();
extern void setupStub();

void exception_handler(){
    __asm__ volatile("cli; hlt"); //completely hangs the computer
    while(1); //needed to just make the compiler shut up
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){
    idt_entry* descriptor = &idt[vector];

    descriptor->isr_low         =(uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs       =0x0000; //change this to where kernel offset is if I set that up later
    descriptor->attributes      =flags;
    descriptor->isr_high        =(uint32_t)isr>>16;
    descriptor->reserved        =0;
}

void idt_init(){
    idtr.base=(uint32_t)&idt[0];
    idtr.limit=(uint16_t)sizeof(idt_entry) * IDT_MAX_DESCRIPTORS -1;

    unsigned char vector;
    for(vector=0; vector<255; vector++){
        idt_set_descriptor(vector,isr_stub_table[vector],0x8E);
    }
    /*
    for(;vector<256;++vector){
        idt_set_descriptor(vector,default_interrupt_handler,0x8E);
    }
    */

    __asm__ volatile("lidt %0" : : "memory"(idtr)); //load the new IDT
    __asm__ volatile("sti"); //Set interrupt flag

}
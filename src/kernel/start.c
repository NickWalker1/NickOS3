#include "start.h"


void main(){

    //clear_screen();
    println("Kernel Mode.");

    boot();
    
    //Hold
    while(1);

}
/* Setups up kernel */
void boot(){
    print_attempt("Clearing BSS.");
    clear_bss();
    print_ok();

    print_attempt("Memory Regions Check:");
    MemoryMapEntry** usableMemoryRegions = getUsableMemoryRegions(MemoryRegionCount);
    if(UsableMemoryRegionCount<1)
        PANIC("MEMORY REGION COUNT FAIL");

    setupAvailablePages(UsableMemoryRegionCount, usableMemoryRegions);
    if(pagecount<1)
        PANIC("NO AVAILABLE PAGES");

    
    print_ok();

    print_attempt("GDT init");
    gdt_init();
    print_ok();
    

    print_attempt("Paging init");
    paging_init();
    print_ok();


    print_attempt("IDT init");
    idt_init();
    print_ok();
}

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

void clear_bss(){
    extern uint32_t BSS_SECTION_START;
    extern uint32_t BSS_SECTION_END;

    uint32_t* bss_start = (uint32_t*) BSS_SECTION_START;
    uint32_t* bss_end   = (uint32_t*) BSS_SECTION_END;

    int bss_length = bss_end-bss_start;
    memset(bss_start,0,bss_length);
}

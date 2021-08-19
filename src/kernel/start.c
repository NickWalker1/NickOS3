#include "start.h"


void main(){

    //clear_screen();
    println("Kernel Mode.");

    clear_bss();
    println("BSS cleared.");

    println("Memory Region Count:");
    print(itoa(MemoryRegionCount,str, BASE_DEC));

    //Detect usable Memory Regions
    MemoryMapEntry** usableMemoryRegions = getUsableMemoryRegions(MemoryRegionCount);
    println("Usable Memory Region Count:");
    print(itoa(UsableMemoryRegionCount,str,BASE_DEC));

    setupAvailablePages(UsableMemoryRegionCount, usableMemoryRegions);
    println("Number of dynamic pages available:");
    print(itoa(pagecount,str,BASE_DEC));
    
    gdt_init();
    

    paging_init();
    println("Paging intialised");


    //Initialise IDT
    idt_init();
    println("Initialised IDT.");

    // timer_phase();

    //Hold
    while(1);

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

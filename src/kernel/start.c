#include "start.h"


void main(){

    //clear_screen();
    println("Kernel Mode.");

    //printLinkerSections();

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
    int_enable();
    print_ok();

    //int x=6/0;
    // int_disable();
    
    print_attempt("Threading init");
    thread_init();
    print_ok();
    
    
}

/*
void printLinkerSections(){
    extern uint32_t BOOT_SECTION_START;
    extern uint32_t BOOT_SECTION_END;
    extern uint32_t TEXT_SECTION_START;
    extern uint32_t TEXT_SECTION_END;
    extern uint32_t RODATA_SECTION_START;
    extern uint32_t RODATA_SECTION_END;
    extern uint32_t DATA_SECTION_START;
    extern uint32_t DATA_SECTION_END;
    extern uint32_t BSS_SECTION_START;
    extern uint32_t BSS_SECTION_END;

    println("BOOT: ");
    print(itoa(BOOT_SECTION_START,str,BASE_HEX));
    print(" to ");
    print(itoa(BOOT_SECTION_END ,str,BASE_HEX));
    println("TEXT: ");
    print(itoa(TEXT_SECTION_START,str,BASE_HEX));
    print(" to ");
    print(itoa(TEXT_SECTION_END,str,BASE_HEX));
    println("RODATA: ");
    print(itoa(RODATA_SECTION_START,str,BASE_HEX));
    print(" to ");
    print(itoa(RODATA_SECTION_END,str,BASE_HEX));
    println("DATA: ");
    print(itoa(DATA_SECTION_START,str,BASE_HEX));
    print(" to ");
    print(itoa(DATA_SECTION_END,str,BASE_HEX));
    println("BSS: ");
    print(itoa(BSS_SECTION_START,str,BASE_HEX));
    print(" to ");
    print(itoa(BSS_SECTION_END,str,BASE_HEX));
}
*/
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

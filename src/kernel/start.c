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

    

    //Hold
    while(1);

}

void clear_bss(){
    extern uint32_t BSS_SECTION_START;
    extern uint32_t BSS_SECTION_END;

    uint32_t* bss_start = (uint32_t*) BSS_SECTION_START;
    uint32_t* bss_end   = (uint32_t*) BSS_SECTION_END;

    int bss_length = bss_end-bss_start;
    memset(bss_start,0,bss_length);
}

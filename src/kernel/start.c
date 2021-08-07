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
    println("Dynamic memory area physical start addr:");
    print(itoa(usableMemoryRegions[1]->Base,str,BASE_HEX));
    
    gdt_init();
    
    paging_init((void*) usableMemoryRegions[0]->Base);
    println("Paging intialised");

    //Initialise IDT
    //idt_init();
    //println("Initialised IDT. NOTTTT");

    println("First free page addr:");
    print(itoa((int) get_next_free_physical_page(),str,BASE_HEX));
    
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

#include "start.h"
#include "../lib/screen.h"
#include "../lib/int.h"
#include "../lib/smap.h"
#include "../lib/string.h"
#include "../idt/idt.h"
#include "../paging/paging.h"
#include "../gdt/gdt.h"

extern uint8_t MemoryRegionCount;


void main(){
    //temporary fix for some static memory to store strings in temporarily
    char str[128];

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

    gdt_init();
    paging_init();
    println("Identity Map removed.");

    //Test usable memory space
    println("First usable address:");
    print(itoa(usableMemoryRegions[0]->Base,str,BASE_HEX));
    
    //Initialise IDT

    //idt_init();
    //println("Initialised IDT.");

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
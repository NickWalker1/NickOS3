#include "../lib/screen.h"
#include "../lib/int.h"
#include "../lib/smap.h"
#include "../idt/idt.h"

extern uint8_t MemoryRegionCount;


void main(){
    //temporary fix for some static memory to store strings in temporarily
    char str[128];

    //clear_screen();
    println("Kernel Mode.");

    println("Memory Region Count:");
    print(itoa(MemoryRegionCount,str, BASE_DEC));

    //Detect usable Memory Regions
    MemoryMapEntry** usableMemoryRegions = getUsableMemoryRegions(MemoryRegionCount);
    println("Usable Memory Region Count:");
    print(itoa(UsableMemoryRegionCount,str,BASE_DEC));

    //Test usable memory space
    println("First usable address:");
    print(itoa(usableMemoryRegions[0]->Base,str,BASE_HEX));
    
    //Initialise IDT
    idt_init();
    println("Initialised IDT.");

}


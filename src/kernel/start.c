#include "../lib/screen.h"
#include "../lib/int.h"
#include "../lib/smap.h"
#include "../idt/idt.h"

extern uint8_t MemoryRegionCount;


void main(){
    //temporary fix for some static memory to store strings in temporarily
    char str[128];

    //clear_screen();
    println("Kernel Mode");

    println("Memory Region Count:");
    print(itoa(MemoryRegionCount,str, BASE_DEC));

    //printMemoryRegions(MemoryRegionCount);
    MemoryMapEntry** usableMemoryRegions = getUsableMemoryRegions(MemoryRegionCount);
    println("Usable Memory Region Count:");
    print(itoa(UsableMemoryRegionCount,str,BASE_DEC));

    idt_init();
    
    println("Throwing random exception:");
    __asm__ volatile("int $0x3");
    println("Ok.");
}


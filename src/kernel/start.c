#include "../lib/screen.h"
//#include "../lib/typedefs.h"
#include "../lib/int.h"
#include "../lib/smap.h"

extern uint8_t MemoryRegionCount;

void main(){
    //temporary fix for some static memory to store strings in temporarily
    char str[128];

    //clear_screen();
    println("Kernel Mode");

    println("Memory Region Count:");
    print(itoa(MemoryRegionCount,str, BASE_DEC));

    printMemoryRegions(MemoryRegionCount);
}


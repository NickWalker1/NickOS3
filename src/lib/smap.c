#include "smap.h"
#include "screen.h"
#include "int.h"

static MemoryMapEntry* usableMemoryRegions[10];
int UsableMemoryRegionCount;

void printMemoryRegions(uint8_t MRC){
    uint8_t i=0;
    MemoryMapEntry* mMap = (MemoryMapEntry*) 0x5000;
    for(;i<MRC;i++){
        println("Memory Region:");
        print(itoa((int)i,str,10));
        printMemoryMap(mMap);
        mMap++;
    }
}

void printMemoryMap(MemoryMapEntry* memoryMap){
    println("Base:");       print(itoa(memoryMap->Base,str,BASE_HEX));
    println("Length:");     print(itoa(memoryMap->Length,str,BASE_HEX));
    println("Type:");       print(itoa(memoryMap->Type,str,BASE_HEX));
    println("ACPI:");       print(itoa(memoryMap->ACPI,str,BASE_HEX));
}



MemoryMapEntry** getUsableMemoryRegions(uint8_t MRC){
    uint8_t i=0;
    MemoryMapEntry* mMap = (MemoryMapEntry*) 0x5000; //static physical address where shit is stored
    //still works as identity mapping still exists.
    int count=0;
    mMap--;
    for(;i<MRC;i++){
        mMap++;//skips first one also which is used for kernel shit
        if(mMap->Type!=1) continue;
        usableMemoryRegions[count++]=mMap;
    }

    UsableMemoryRegionCount=count;
    return usableMemoryRegions;
}
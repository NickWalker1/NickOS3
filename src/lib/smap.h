#include "typedefs.h"

typedef struct MemoryMapEntry{
    uint64_t Base;//these may not work?
    uint64_t Length;
    uint32_t Type;
    uint32_t ACPI;
}__attribute__((packed)) MemoryMapEntry;

extern uint8_t MemoryRegionCount;

void printMemoryMap(MemoryMapEntry* memoryMap);
void printMemoryRegions(uint8_t MRC);
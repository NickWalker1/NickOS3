#pragma once
#include "typedefs.h"

typedef struct MemoryMapEntry{
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
    uint32_t ACPI;
}__attribute__((packed)) MemoryMapEntry;

extern uint8_t MemoryRegionCount;
extern int UsableMemoryRegionCount;
extern char str[128];

void printMemoryMap(MemoryMapEntry* memoryMap);
void printMemoryRegions(uint8_t MRC);
void setupAvailablePages(uint8_t MRC, MemoryMapEntry** mRegions);
MemoryMapEntry** getUsableMemoryRegions(uint8_t MRC);
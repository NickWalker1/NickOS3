#pragma once

#include "../paging/paging.h"


typedef struct MemorySegmentHeader MemorySegmentHeader;

struct MemorySegmentHeader{
    int MemoryLength;
    MemorySegmentHeader* nextSegment;
    MemorySegmentHeader* previousSegment;
    MemorySegmentHeader* nextFreeSegment;
    MemorySegmentHeader* previousFreeSegment;
    bool free;
};


MemorySegmentHeader* firstFreeSegment;

void initialiseHeap(void* baseAddr, int length);
void* malloc(size_t size);
void free(void* addr);
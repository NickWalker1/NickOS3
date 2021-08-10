#include "heap.h"

MemorySegmentHeader* firstFreeSegment;

void initialiseHeap(void* baseAddr, int length){
    //currently length will just be PGSIZE
    //so some inefficiency possible if the very first request is
    //for >4096 bytes as it will ignore this first page and call palloc
    firstFreeSegment= (MemorySegmentHeader*) baseAddr;
    firstFreeSegment->MemoryLength=length-sizeof(MemorySegmentHeader);
    firstFreeSegment->nextFreeSegment=0;
    firstFreeSegment->nextSegment=0;
    firstFreeSegment->previousFreeSegment=0;
    firstFreeSegment->previousSegment=0;
    firstFreeSegment->free=true;
}

void* malloc(size_t size){
    
    MemorySegmentHeader* segment=firstFreeSegment;
    while(segment!=0 && segment->MemoryLength<size){
        segment=segment->nextFreeSegment;
    }
    if(segment->MemoryLength>size){

    }
    if(segment==0){
        //PALLOC PAGES
    }


}
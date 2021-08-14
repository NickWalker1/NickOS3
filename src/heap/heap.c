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
    //make it 8 byte aligned.
    int remainder = size%8;
    size-=remainder;
    if(remainder!=0) size+=8;

    //current memory segment
    MemorySegmentHeader* currentSegment = firstFreeSegment;
    
    //while the current segment is too small and the nextFree segment isn't null
    while(currentSegment->MemoryLength<size && currentSegment->nextFreeSegment!=0){
        //skip segment
        currentSegment=currentSegment->nextFreeSegment;
    }

    //unable to allocate memory with current pages allocated
    if(currentSegment->MemoryLength<size){
        int numPages;
        numPages=(size+sizeof(MemorySegmentHeader))/PGSIZE;
        int remainder=size%PGSIZE;
        if(remainder!=0) numPages++;

        //TODO make it get process info from thread_control block to know which pool to allocate from
        void* base_addr=palloc_heap(numPages,kernel_pool,F_KERN); //TODO MAKE IT GET FLAGS SOMEHOW

        if(base_addr==NO_ADDR) return NO_ADDR;
        MemorySegmentHeader* newSegment1 = (MemorySegmentHeader*) base_addr;
        newSegment1->free=false;


        //cross your fingers and pray to god this pointer shit is correct.
        //currentSegment->nextSegment=currentSegment;
        if(remainder>sizeof(MemorySegmentHeader)){
            void* newSegment2Addr= base_addr+sizeof(MemorySegmentHeader)+size;
            MemorySegmentHeader* newSegment2 = (MemorySegmentHeader*) newSegment2Addr; 

            newSegment2->free=true;
            newSegment2->MemoryLength=remainder-sizeof(MemorySegmentHeader);
            newSegment2->nextSegment=0;
            newSegment2->nextFreeSegment=0;
            newSegment2->previousSegment=newSegment1;
            newSegment2->previousFreeSegment = currentSegment->free ? currentSegment : currentSegment->previousFreeSegment;
            
            //get the last segment even if it's not free
            MemorySegmentHeader* tmp = currentSegment;
            while(tmp->nextSegment!=0) tmp=tmp->nextSegment;
            newSegment1->previousSegment=tmp;
            
            newSegment1->previousFreeSegment=currentSegment;
            newSegment1->nextFreeSegment=newSegment2;
            newSegment1->nextSegment=newSegment2;


            currentSegment->nextSegment=newSegment1;
            currentSegment->nextFreeSegment=newSegment2;


        }
        else{
            newSegment1->nextSegment=0;
            newSegment1->nextFreeSegment=0;
            newSegment1->previousSegment=currentSegment;
            newSegment1->previousFreeSegment = currentSegment->free ? currentSegment : currentSegment->previousFreeSegment;
            
            currentSegment->nextSegment=0;
            currentSegment->nextFreeSegment=0;
        }
        return base_addr+sizeof(MemorySegmentHeader);
    }

    //create new segment which is the remainder of the previous segment
    //which starts at the address directly after the malloced area
    void* newSegment_addr= ((void*)currentSegment)+sizeof(MemorySegmentHeader)+size;
    MemorySegmentHeader* newSegment= (MemorySegmentHeader*) newSegment_addr;
    newSegment->free=true;
    newSegment->MemoryLength=currentSegment->MemoryLength-(sizeof(MemorySegmentHeader)+ size);
    newSegment->nextFreeSegment=currentSegment->nextFreeSegment;
    newSegment->nextSegment=currentSegment->nextSegment;
    newSegment->previousFreeSegment=currentSegment->previousFreeSegment;
    newSegment->previousSegment=currentSegment;
    
    //update current segment data
    currentSegment->nextFreeSegment=newSegment;
    currentSegment->nextSegment=newSegment;
    currentSegment->MemoryLength=size;
    currentSegment->free=false;

    //update first free segment
    if(currentSegment==firstFreeSegment)firstFreeSegment=newSegment;

    //update the pointers for the previous segment
    //is ugly because doubly linked list 
    if (currentSegment->previousFreeSegment != 0) currentSegment->previousFreeSegment->nextFreeSegment = currentSegment->nextFreeSegment;
    if (currentSegment->nextFreeSegment != 0) currentSegment->nextFreeSegment->previousFreeSegment = currentSegment->previousFreeSegment;
    if (currentSegment->previousSegment != 0) currentSegment->previousSegment->nextFreeSegment = currentSegment->nextFreeSegment;
    if (currentSegment->nextSegment != 0) currentSegment->nextSegment->previousFreeSegment = currentSegment->previousFreeSegment;

    
    return currentSegment+1;
}

void CombineFreeSegments(MemorySegmentHeader* a, MemorySegmentHeader* b) {
	if (a == 0) return;
	if (b == 0) return;
	if (a < b) {
		a->MemoryLength += b->MemoryLength + sizeof(MemorySegmentHeader);
		a->nextSegment = b->nextSegment;
		a->nextFreeSegment = b->nextFreeSegment;
		b->nextSegment->previousSegment = a;
		b->nextSegment->previousFreeSegment = a;
		b->nextFreeSegment->previousFreeSegment = a;
	}
	else {
		b->MemoryLength += a->MemoryLength + sizeof(MemorySegmentHeader);
		b->nextSegment = a->nextSegment;
		b->nextFreeSegment = a->nextFreeSegment;
		a->nextSegment->previousSegment = b;
		a->nextSegment->previousFreeSegment = b;
		a->nextFreeSegment->previousFreeSegment = b;
	}
}
/* TODO FIX */
void free(void* address) {
	MemorySegmentHeader* currentMemorySegment;
		
    currentMemorySegment = ((MemorySegmentHeader*)address) - 1;
	currentMemorySegment->free = true;

	if (currentMemorySegment < firstFreeSegment) firstFreeSegment = currentMemorySegment;

	if (currentMemorySegment->nextFreeSegment != 0) {
		if (currentMemorySegment->nextFreeSegment->previousFreeSegment < currentMemorySegment)
			currentMemorySegment->nextFreeSegment->previousFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->previousFreeSegment != 0) {
		if (currentMemorySegment->previousFreeSegment->nextFreeSegment > currentMemorySegment)
			currentMemorySegment->previousFreeSegment->nextFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->nextSegment != 0) {
		currentMemorySegment->nextSegment->previousSegment = currentMemorySegment;
		if (currentMemorySegment->nextSegment->free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->nextSegment);
	}
	if (currentMemorySegment->previousSegment != 0) {
		currentMemorySegment->previousSegment->nextSegment = currentMemorySegment;
		if (currentMemorySegment->previousSegment->free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->previousSegment);

	}
    /* TODO 
    if(currentMemorySegment->MemoryLength>=4096){
        unmap_page(address, F_KERN);
    }
    */
}

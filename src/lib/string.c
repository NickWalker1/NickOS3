#include "string.h"

void* memset(void* str, int c, size_t n){
    uint32_t* ptr = (uint32_t*)str;
    for(size_t i=0;i<n;i++){
        *ptr=c;
        ptr++;
    }
}

void memcpy(void* to, void* from, size_t n){
    uint32_t* to_ptr=(uint32_t*)to;
    uint32_t* from_ptr=(uint32_t*)from;
    for(size_t i=0;i<n;i++){
        to_ptr[i]=from_ptr[i];
    }
}
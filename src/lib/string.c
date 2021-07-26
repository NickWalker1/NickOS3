#include "string.h"

void* memset(void* str, int c, size_t n){
    uint8_t* ptr = (uint8_t*)str;
    for(size_t i=0;i<n;i++){
        *ptr=c;
        ptr++;
    }
}
#include "tss.h"

void tss_init(){
    tss=palloc(1,F_ASSERT| F_ZERO);
    tss->ss0=0x10;
    tss->IOPB=sizeof(tss);
    tss_update();

}

void tss_update(){
    if(tss==0) PANIC("TSS null");
    //TODO
    //UNCOMMENT WHEN CURRENT_THREAD IMPLEMENTED
    //tss->esp0= (uint8_t*) current_thread() + PGSIZE;
}
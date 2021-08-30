#include "tss.h"

#include "../thread/thread.h"

/* Kernel TSS. */
static struct tss *kernel_tss;

void tss_init(){
    kernel_tss=palloc_kern(1,F_ASSERT| F_ZERO);
    kernel_tss->ss0=0x10;
    kernel_tss->IOPB=sizeof(tss);
    tss_update();

}

void tss_update(){
    if(kernel_tss==0) PANIC("TSS null");
    kernel_tss->esp0= (uint8_t*) current_thread() + PGSIZE;
}

tss* get_kernel_tss(){
    return kernel_tss;
}
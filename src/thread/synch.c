#include "synch.h"

void sema_init(semaphore* s, uint32_t value){
    if(s==0) PANIC("Semaphore NULL");

    s->value=value;
    s->waiters=list_init();
}

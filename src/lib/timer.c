#include "timer.h"


int ticks=0;
uint32_t timer_handler(interrupt_state* s){
    ticks++;
    if(ticks%18==0){
        println("1 second");
    }
}
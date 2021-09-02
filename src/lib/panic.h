#pragma once


#include "screen.h"
#include "../interrupt/cpu_state.h"

#define NULL (void*)0

void PANIC(char* msg);
void PANIC_EXC(char* msg,exception_state* state);

void draw_panic_screen();
void halt();
#pragma once

#include "cpu_state.h"
#include "idt.h"

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39


void default_exception_handler(exception_state* state);
void keyboard_handler(interrupt_state* state);
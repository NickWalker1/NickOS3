#pragma once

#include "../lib/screen.h"
#include "../drivers/low_level.h"
#include "../idt/cpu_state.h"


uint32_t timer_handler(interrupt_state* s);


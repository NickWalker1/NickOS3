#pragma once

#include "../lib/screen.h"
#include "../drivers/low_level.h"
#include "../interrupt/cpu_state.h"


uint32_t timer_handler(interrupt_state* s);
void time_tick();

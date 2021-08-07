#pragma once

#include "../lib/screen.h"
#include "../lib/int.h"
#include "../lib/smap.h"
#include "../lib/string.h"
#include "../idt/idt.h"
#include "../paging/paging.h"
#include "../gdt/gdt.h"


extern uint8_t MemoryRegionCount;
extern int pagecount;
extern int UsableMemoryRegionCount;


void main();
void clear_bss();
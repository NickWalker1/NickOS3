#pragma once

#include "fs.h"

#include "../lib/typedefs.h"
#include "../lib/list.h"


#define MAX_RD_FILES 32


extern kv* file_lookup;


typedef struct rd_header{
    uint32_t num_files;
} rd_header;

uint32_t initrd_read(fs_node* node, uint32_t offset, uint32_t size, uint32_t* buffer);

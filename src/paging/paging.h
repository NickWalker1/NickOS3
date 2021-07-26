#pragma once
#include "../lib/typedefs.h"

#define PAGE_DIRECTORY_BASE 0xC0006000
#define PAGE_TABLES_BASE    0xC1100000

typedef struct page_table_entry
{
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_supervisor : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t reserved : 1;
    uint8_t dirty : 1;
    uint8_t global : 1;

    uint8_t available : 3;
    uint32_t physical_page_address : 20;
} __attribute__((packed)) page_table_entry;


void paging_init();
void clear_identity_pages();


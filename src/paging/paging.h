#pragma once
#include "../lib/typedefs.h"
#include "../lib/smap.h"

//                                   NickOS3 virtual memory layout
// |---------------|------------|------------|-------------|------------|-------------------------|
// |     1 MiB     |   15 MiB   |   1 Mib    |    4 MiB    |   3 MiB    |        1017 MiB         |
// | BIOS & Kernel |  Reserved  |   Stack    | Page tables |  Reserved  | Dynamic allocation area |
// |               |            |            |             |            |                         | 
// |  0xC0000000   | 0xC0100000 | 0xC1000000 | 0xC1100000  | 0xC1500000 |       0xC1800000        |
// |  0xC00FFFFF   | 0xC0FFFFFF | 0xC10FFFFF | 0xC14FFFFF  | 0xC17FFFFF |       0xFFFFFFFF        |
// |---------------|------------|------------|-------------|--------------------------------------|
// {     Page 0       }{1}{2}{3}{        Page 4       }{     Page 5     }{       Page 6 - n       }


#define PAGE_DIRECTORY_BASE 0xC0006000
#define PAGE_TABLES_BASE    0xC1100000
#define KERN_BASE           0xC0000000

#define MAX_PHYS_PAGES 4096

int pagecount;

void* Kptov(void* phys);
void* Kvtop(void* virt);

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


typedef struct phys_page{
    uint8_t type;
    void* phys_addr;
}phys_page;

typedef enum memory_type{
    M_FREE=1,
    M_RESERVED=2,
    M_FILLED=3,
    M_AVAILABLE=4,
    M_NOT_AVAILABLE=5
}memory_type;

typedef struct virt_page virt_page;

struct virt_page
{
    uint8_t type;
    virt_page* nextPage;
    virt_page* nextFreePage;
    virt_page* previousPage;
    virt_page* previousFreePage;
};

/*
 * Pool struct containing mutex lock, permission level and pointer
 * to first virt_page struct
 */
typedef struct pool
{
    // mutex lock not implemented yet
    int perm; //permission level?
    virt_page* base_page;

} pool;

page_table_entry *kernel_pd;
page_table_entry *kernel_ptables;

void setupAvailablePages(uint8_t MRC, MemoryMapEntry** mRegions);
void paging_init(void* dynamic_phys_base);
void palloc_init();
void* get_next_free_physical_page();
void clear_identity_pages();


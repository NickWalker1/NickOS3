#pragma once

#include "pte.h"
#include "../lib/typedefs.h"
#include "../lib/smap.h"
#include "../lib/screen.h"
#include "../heap/heap.h"

//                                   NickOS3 virtual memory layout
// |---------------|------------|------------|-------------|------------|-------------------------|
// |     1 MiB     |   15 MiB   |   1 Mib    |    4 MiB    |   3 MiB    |        1017 MiB         |
// | BIOS & Kernel |  Reserved  |   Stack    | Page tables |  Reserved  | Dynamic allocation area |
// |               |            |            |             |            |                         | 
// |  0xC0000000   | 0xC0100000 | 0xC1000000 | 0xC1100000  | 0xC1500000 |       0xC1800000        |
// |  0xC00FFFFF   | 0xC0FFFFFF | 0xC10FFFFF | 0xC14FFFFF  | 0xC17FFFFF |       0xFFFFFFFF        |
// |---------------|------------|------------|-------------|--------------------------------------|
// {     Page 0       }{1}{2}{3}{        Page 4       }{     Page 5     }{       Page 6 - n       }


//PAGE TABLES SHIT NOT USED, ONLY USING PD ADDR, PAGE TABLES INSTEAD ALLOCATED IN DYNAMIC AREA AS REQUIRED

#define PAGE_DIRECTORY_BASE 0xC0006000
//#define PAGE_TABLES_BASE    0xC1100000 
#define KERN_BASE           0xC0000000

#define NO_ADDR  (void*)-1
#define MAX_PHYS_PAGES 4096


#define F_KERN 0x1 /* 1 for kernel, 0 for user */
#define F_ZERO 0x2 /* 1 for set all bytes to 0, 0 don't bother */

extern int pagecount;

void* Kptov(void* phys);
void* Kvtop(void* virt);
typedef struct page_directory_entry
{
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_supervisor : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t reserved : 1;
    uint8_t page_size : 1;
    uint8_t global : 1;

    uint8_t available : 3;
    uint32_t page_table_base_addr : 20;
} __attribute__((packed)) page_directory_entry;

typedef struct page_table_entry
{
    uint8_t present : 1; /* If this entry is present in the table */
    uint8_t read_write : 1; /* 1= R/W , 0=RO */
    uint8_t user_supervisor : 1; /* 1=supervisor , 0=nonsupervisor */
    uint8_t write_through : 1; /* 1=write through enabled , 0=not enabled */
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t page_table_attribute_index: 1;
    uint8_t global : 1;

    uint8_t available : 3;
    uint32_t page_base_addr : 20; /* Base physical address of page */
} __attribute__((packed)) page_table_entry;


typedef struct phys_page{
    uint8_t type;
    void* phys_addr;
}phys_page;

typedef enum memory_type{
    M_FREE=1,/* Not allocated to anyone */
    M_ALLOCATED=2, /* Allocated to someone */
    M_RESERVED=3, /* Cannot be allocated */
}memory_type;

typedef struct virt_page virt_page;

struct virt_page
{
    uint8_t type;
    void* base_vaddr;
    virt_page* nextPage;
    virt_page* previousPage;
};

/*
 * Pool struct containing mutex lock, permission level and pointer
 * to first virt_page struct
 */
typedef struct pool
{
    // mutex lock not implemented yet
    int ring; /* permission level 0=kernel 3=user*/
    int next_free_page_index; /* Index of next free page */
    void* heap_base_pointer; /* Base pointer to start of the heap */
    virt_page virtual_pages[MAX_PHYS_PAGES/2]; /* Both kernel and user processes get 2048 pages to allocate from. The base address of the first page will also be the base address of the first MemorySegmentHeader for the heap */

} pool;

extern page_directory_entry *kernel_pd;
extern pool* kernel_pool;
extern pool* user_pool;
//page_table_entry *kernel_ptables;

void setupAvailablePages(uint8_t MRC, MemoryMapEntry** mRegions);
void paging_init(void* dynamic_phys_base);
void kernel_mapping_init();
void init_pool(uint8_t flags);
void palloc_init();
void* palloc(int num_pages, uint8_t flags);
void* get_next_free_physical_page();
void map_page(void* paddr, void* vaddr, uint8_t flags);
bool unmap_page(void* vaddr, uint8_t flags);
void init_heap_page(uint8_t flags);
void clear_identity_pages();

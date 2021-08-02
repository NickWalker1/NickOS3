#include "paging.h"
#include "pte.h"
#include "../lib/screen.h"

page_table_entry *kernel_pd = (page_table_entry*)PAGE_DIRECTORY_BASE;
page_table_entry *kernel_ptables= (page_table_entry*)PAGE_TABLES_BASE;



pool* kernel_pool, user_pool;
phys_page physical_page_entries[MAX_PHYS_PAGES];

static phys_page nextFreePage;

/* Converts physical address to kernel virtual address */
void* Kptov(void* phys){
    return (void*) phys+KERN_BASE;
}
/* Converts virtual address to kernel physical address */
void* Kvtop(void* virt){
    return (void*) virt-KERN_BASE;
}

/* Initialises paging and page allocation mechanism.
    * Removes identity pages
    * Maps dynamic memory space to page heap */
void paging_init(void* dynamic_phys_base){
    clear_identity_pages();

    palloc_init(dynamic_phys_base);

}
/* Counts number of pages available in free memory*/
void setupAvailablePages(uint8_t UsableMemoryRegionCount, MemoryMapEntry** usableMemoryRegions){
    int i;
    pagecount=0;
    char str[128];
    // i is 1 to skip first space which is mostly just kernel essentials
    // so don't want to overwrite that.
    for(i=0;i<UsableMemoryRegionCount;i++){
        pagecount+=usableMemoryRegions[i]->Length/4096;
    }
    if (pagecount>MAX_PHYS_PAGES) pagecount=MAX_PHYS_PAGES;


    int x=0;
    int pg=0;

    /* For all free memory regions, set the appropriate pages to free that map 
     * directly 
     */
    for(i=0;i<UsableMemoryRegionCount;i++){
        void* baseAddr=usableMemoryRegions[i]->Base;
        void* endAddr= usableMemoryRegions[i]->Length + baseAddr;
        int maxPages = usableMemoryRegions[i]->Length/4096;
        x=0;
        while(x<maxPages && pg<MAX_PHYS_PAGES){
            physical_page_entries[pg].type=M_FREE;
            physical_page_entries[pg].phys_addr=baseAddr+(x*PGSIZE);
            pg++;
            x++;
        }
    }

    //reserved for kernel data so will not be overwriten. 
    for(i=0;i<6;i++){
        physical_page_entries[i].type=M_RESERVED;
    }
}

void palloc_init(){
    //setup kernel pool

}

/*
void* palloc_get_multiple(size_t pg_count, uint8_t flags){
    pool* pool = flags & PUSER ? &
}
*/

/* Returns the physical address of the next free physical page and sets
 * it to used. Returns 0 if no page is available
 */
void* get_next_free_physical_page(){
    //TODO update this to used linked list or something more intelligent
    //currently stupid and slow but is sufficient
    for(int i=0;i<MAX_PHYS_PAGES;i++){
        if(physical_page_entries[i].type==M_FREE){
            physical_page_entries[i].type=M_FILLED;
            return physical_page_entries[i].phys_addr;
        }
    }
    return 0;
}

void clear_identity_pages(){
    //clear first 24 bytes
    for(int i=0;i<6;i++){
        kernel_pd[i].present=0;
    }
}
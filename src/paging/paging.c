#include "paging.h"
#include "pte.h"
#include "../lib/screen.h"
#include "../heap/heap.h"

page_directory_entry *kernel_pd = (page_directory_entry*)PAGE_DIRECTORY_BASE;




phys_page physical_page_entries[MAX_PHYS_PAGES];

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
    init_heap_page(F_KERN);
    //init_heap_page(!F_KERN);
}

/* clears the first 6 identity pages used for converting to paging */
void clear_identity_pages(){
    //clear first 24 bytes
    for(int i=0;i<6;i++){
        kernel_pd[i].present=0;
    }
}
/* Initialises both user and kernel pools */
void palloc_init(){

    init_pool(true);

    init_pool(false);
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
        void* baseAddr=(void*) usableMemoryRegions[i]->Base;
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




/* Used for heap dynamic memory allocations.
 * Calculates virtual address of first pointer depending on
 * number of heap pages already allocated, and starts at 0.
 * Returns virtual base address of next page allocated 
 * given the number of pages to allocate and a flags variable
 */
void* palloc(int num_pages, uint8_t flags){
    pool* mem_pool;
    void* return_addr=0;
    if(flags & F_KERN){
        mem_pool=kernel_pool;
    }
    else{
        mem_pool=user_pool;
    }
    

    for(int i=0;i<num_pages;i++){
        virt_page vpage = kernel_pool->virtual_pages[kernel_pool->next_free_page_index];
        
        void* paddr=get_next_free_physical_page();

        if(paddr==0){
            //TODO PANIC CAN'T GET ANYMORE PAGES
            return 0;
        }
        //using idea that heap memory starts at 0x0 in virtual address space.
        void* vaddr=(void*) (mem_pool->next_free_page_index*4096);
        //void* vaddr=Kvtop(paddr);

        map_page(paddr,vaddr,flags);
        
        //ensure returned pointer points to start of the pages.
        if(i==0) return_addr=vaddr;
        
        vpage.base_vaddr=vaddr;
        vpage.type=M_ALLOCATED;
        vpage.nextPage=0;
        vpage.previousPage=&(kernel_pool->virtual_pages[kernel_pool->next_free_page_index-1]);
        vpage.previousPage->nextPage=&vpage;

        kernel_pool->next_free_page_index++;
    }

    return return_addr;
    
}

/* Adds pd, pt mappings for a new page given a virtual and physical address*/
void map_page(void* paddr, void* vaddr, uint8_t flags){

    if(flags & F_KERN){//IF KERNEL
        size_t pd_idx, pt_idx;
        page_table_entry* pt;

        pd_idx=pd_no(vaddr);
        pt_idx=pt_no(vaddr);

        //if the page table page does not exist, create one and fill out the entry
        //in the PD.
        if(kernel_pd[pd_idx].present==0){
            void* pt_addr = get_next_free_physical_page();
            kernel_pd[pd_idx].page_table_base_addr=((uint32_t)pt_addr >> PGBITS); //Only most significant 20bits
            kernel_pd[pd_idx].present=1;
            kernel_pd[pd_idx].read_write=1;
        }

        pt=(page_table_entry*) (kernel_pd[pd_idx].page_table_base_addr<<PGBITS); //Push back to correct address
        pt[pt_idx].page_base_addr=(uint32_t) paddr>>PGBITS; //Only 20 most significant bits
        pt[pt_idx].present=1;
        pt[pt_idx].read_write=1;

    }
    else{
        //TODO implement user shit
        //find out how many pages already allocated to user then virtual address will be
        //that number * 4096 :)
    }

}


void init_pool(uint8_t flags){
    //find out how much space is required to store the pool info
    int numBytes= sizeof(pool);//TODO THIS IS WRONG
    //do some maff to round up
    if(numBytes%PGSIZE!=0){
        numBytes=numBytes-(numBytes%PGSIZE)+PGSIZE;
    }
    int numPages=numBytes/PGSIZE;
    // println(itoa(numPages,str,BASE_DEC));
    void* paddr;
    void* ptaddr;

    for(int i=0;i<numPages;i++){
        paddr=get_next_free_physical_page();
        void* vaddr  = Kptov(paddr);
        if(i==0){
            //update pool pointer to point to start of this allocated section.
            if(flags & F_KERN)kernel_pool=(pool*) vaddr;
            else user_pool=(pool*) vaddr;
        }

        map_page(paddr,vaddr,F_KERN);
    }
}

void init_heap_page(uint8_t flags){
    if(flags & F_KERN){
        //Get a new fresh page to be used for heap
        void* paddr = get_next_free_physical_page();
        //Assuming virtual address space for heap starts at 0
        void* vaddr = 0;

        kernel_pool->ring=0;
        kernel_pool->next_free_page_index=1;
        kernel_pool->virtual_pages[0].type=M_ALLOCATED;
        map_page(paddr,vaddr,0x1);
        /*
        kernel_pool->virtual_pages[0].base_vaddr=vaddr;
        kernel_pool->virtual_pages[0].nextPage=0;
        kernel_pool->virtual_pages[0].previousPage=0;

        //initialise heap
        initialiseHeap(vaddr,PGSIZE);
        */
    }
    else{
        user_pool->ring=3;
        //user_pool->virtual_pages[0]=0; //Just ensure it's NULL
    }
    //TODO if doing the init_pd* thing then set cr3 to Kvtop(init_pd) but just check that it definitely just want's physical address not virtual.#

}

/* Returns the physical address of the next free physical page and sets
 * it to used and adds it's entry to the pd. Returns 0 if no page is available
 */
void* get_next_free_physical_page(){
    //TODO update this to used linked list or something more intelligent
    //currently stupid and slow but is sufficient
    for(int i=0;i<MAX_PHYS_PAGES;i++){
        if(physical_page_entries[i].type==M_FREE){
            physical_page_entries[i].type=M_ALLOCATED;
            return physical_page_entries[i].phys_addr; 
        }
    }
    return 0;
}

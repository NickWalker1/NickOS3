#include "paging.h"


page_directory_entry *kernel_pd;
pool* kernel_pool;
pool* user_pool;
int pagecount;


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
    
    //testFunc();
    init_heap_page(F_KERN);
    //init_heap_page(!F_KERN);
}

void testFunc(){
    /* IF I REMOVE THIS USELESS FUNCION IT BREAKS AND I HAVE NO IDEA WHY */
    void* paddr = get_next_free_physical_page();
    void* vaddr = 110010;
    map_page(paddr,vaddr,0x8);

    *(uint32_t*)vaddr=5;
    println(itoa(*(uint32_t*)vaddr,str,BASE_DEC));
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
 * given the number of pages to allocate and a flags variable.
 * Will return NO_ADDR if unable ot allocate memory.
 */
void* palloc(int num_pages, uint8_t flags){
    //TODO FIX UGLINESS WITH VPAGE

    pool* mem_pool;
    void* return_addr=0;
    if(flags & F_KERN){
        mem_pool=kernel_pool;
    }
    else{
        mem_pool=user_pool;
    }
    

    for(int i=0;i<num_pages;i++){
        virt_page next_free_vpage = kernel_pool->virtual_pages[kernel_pool->next_free_page_index];
        
        void* paddr=get_next_free_physical_page();

        if(paddr==0){
            
            //unable to allocate more pages
            return NO_ADDR;
        }
        //using idea that heap memory starts at 0x0 in virtual address space.
        void* vaddr = next_free_vpage.base_vaddr;
        // void* vaddr=(void*) (mem_pool->next_free_page_index*4096);
        //void* vaddr=Kvtop(paddr);

        map_page(paddr,vaddr,flags);
        
        //ensure returned pointer points to start of the pages.
        if(i==0) return_addr=vaddr;
        
        next_free_vpage.base_vaddr=vaddr;
        next_free_vpage.type=M_ALLOCATED;
        next_free_vpage.nextPage=0;
        next_free_vpage.previousPage=&(kernel_pool->virtual_pages[kernel_pool->next_free_page_index-1]);
        next_free_vpage.previousPage->nextPage=&next_free_vpage;

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
            println("Creating new PT entry");
            void* pt_addr = get_next_free_physical_page();
            kernel_pd[pd_idx].page_table_base_addr=((uint32_t)pt_addr >> PGBITS); //Only most significant 20bits
            kernel_pd[pd_idx].present=1;
            kernel_pd[pd_idx].read_write=1;
        }

        pt=(page_table_entry*) Kptov(kernel_pd[pd_idx].page_table_base_addr<<PGBITS); //Push back to correct address
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

/* unmaps physical page associated with the virtual address 
 * return false if vaddr is not on a page boundary */
bool unmap_page(void* vaddr, uint8_t flags){
    //check vaddr is on page boundary
    if(!(int)vaddr%4096){
        return false;
    }

    size_t pt_idx, pd_itx;
    pd_itx=pd_no(vaddr);
    if(flags & F_KERN){
        if(kernel_pd[pd_itx].present==0){
            //page not mapped anyway.
            return true;
        }
        pt_idx=pt_no(vaddr);
        page_table_entry* pt = (page_table_entry*)Kptov(kernel_pd[pd_itx].page_table_base_addr<<PGBITS);

        pt[pt_idx].present=0;
        return true;
    }
    else{
        //TODO
        //Repeat for user stuff
    }
}


/* Initialises page pool for either user or kernel.
 * It will calculate the number of pages required and 
 * manually allocate them and poin the pool struct to
 * the start of this region and returns it.
 */
void init_pool(uint8_t flags){
    //find out how much space is required to store the pool info
    int numBytes= sizeof(pool);//TODO THIS MAY BE WRONG
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


/* Initialses the first heap page for a user or the kernel.
 */
void init_heap_page(uint8_t flags){

    if(flags & F_KERN){
        println("Initialising kernel heap page");
        //Get a new fresh page to be used for heap
        void* paddr = get_next_free_physical_page();

        //Assuming virtual address space for heap starts at 0
        void* vaddr = 0;

        kernel_pool->ring=0;
        kernel_pool->next_free_page_index=1;
        kernel_pool->virtual_pages[0].type=M_ALLOCATED;

        map_page(paddr,vaddr,F_KERN);
        
        
        kernel_pool->virtual_pages[0].base_vaddr=vaddr;
        kernel_pool->virtual_pages[0].nextPage=0;
        kernel_pool->virtual_pages[0].previousPage=0;


        //initialise heap
        initialiseHeap(vaddr,PGSIZE);
        
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
    return NO_ADDR;
}

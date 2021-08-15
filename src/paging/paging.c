#include "paging.h"


pool* kernel_pool;
pool* user_pool;
int pagecount;


page_directory_entry *kernel_pd = (page_directory_entry*)PAGE_DIRECTORY_BASE;
page_directory_entry* base_pd;


phys_page physical_page_entries[MAX_PHYS_PAGES];

/* Converts physical address to kernel virtual address */
void* Kptov(void* phys){
    return (void*) (phys+(uint32_t)KERN_BASE);
}
/* Converts virtual address to kernel physical address */
void* Kvtop(void* virt){
    return (void*) virt-(uint32_t)KERN_BASE;
}


/* Initialises paging and page allocation mechanism.
    * Removes identity pages
    * Sets up kernel pool */
void paging_init(){
    clear_identity_pages();

    kernel_pool=(pool*) init_pool(F_KERN);

    init_heap_page(kernel_pool,F_KERN);

}

/* clears the first 6 identity pages used for converting to paging */
void clear_identity_pages(){
    //clear first 24 bytes
    for(int i=0;i<6;i++){
        kernel_pd[i].present=0;
    }
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
    //reserve the kernel pages.
    println("Mapping kernel pages");
    for(i=0;i<32;i++){
        physical_page_entries[i].type=M_RESERVED;
        void* phys_addr= i*4096;
        map_page(phys_addr,Kptov(phys_addr),F_KERN);
    }

    //map and reserve video memory space
    i=0xb8000/PGSIZE;
    physical_page_entries[i].type=M_RESERVED;
    void* phys_addr= i*4096;
    map_page(phys_addr,Kptov(phys_addr),F_KERN);

    //setup base_pd for processes to copy from.
    base_pd=palloc_kern(1,F_ZERO | F_ASSERT);
    memcpy(base_pd,kernel_pd,PGSIZE);
    
}

/* Basic function to allocate some pages in kernel virtual address space */
void* palloc_kern(int num_pages, uint8_t flags){

    void* return_addr=0;

    for(int i=0;i<num_pages;i++){
        void* paddr=get_next_free_physical_page();

        if(paddr==NO_ADDR){
            if(flags & F_ASSERT) PANIC("UNABLE TO ALLOCATE REQUIRED PAGES");
            //unable to allocate more pages
            return NO_ADDR;
        }

        void* vaddr=Kptov(paddr);

        map_page(paddr,vaddr,flags);
        
        
        //ensure returned pointer points to start of the pages.
        if(i==0) return_addr=vaddr;
        
    }

    if(flags&F_ZERO){
        memset(return_addr,0,num_pages*PGSIZE);
    }

    return return_addr;
}



/* Used for heap dynamic memory allocations.
 * Calculates virtual address of first pointer depending on
 * number of heap pages already allocated, and starts at 0.
 * Returns virtual base address of next page allocated 
 * given the number of pages to allocate and a flags variable.
 * Will return NO_ADDR if unable ot allocate memory.
 * TODO Change args to not require mem_pool, it will just get it from TCB
 */
void* palloc_heap(int num_pages,pool* mem_pool, uint8_t flags){

    void* return_addr=0;

    for(int i=0;i<num_pages;i++){
        virt_page next_free_vpage = kernel_pool->virtual_pages[kernel_pool->next_free_page_index];
        
        void* paddr=get_next_free_physical_page();

        if(paddr==NO_ADDR){
            if(flags & F_ASSERT) PANIC("UNABLE TO ALLOCATE REQUIRED PAGES");
            //unable to allocate more pages
            return NO_ADDR;
        }
        //using idea that heap memory starts at 0x0 in virtual address space.
        void* vaddr=(void*) ((mem_pool->next_free_page_index)*4096);

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

    if(flags&F_ZERO){
        memset(return_addr,0,num_pages*PGSIZE);
    }

    return return_addr;
    
}

/* Adds pd, pt mappings for a new page given a virtual and physical address*/
void map_page(void* paddr, void* vaddr, uint8_t flags){
    if((uint32_t)vaddr%PGSIZE || (uint32_t)paddr%4096) PANIC("VADDR NOT 4k ALIGNED"); 
        

    size_t pd_idx, pt_idx;
    page_table_entry* pt;

    pd_idx=pd_no(vaddr);
    pt_idx=pt_no(vaddr);

    //TODO load pd form thread_control_block instead of just using kernel.

    //if the page table page does not exist, create one and fill out the entry
    //in the PD.
    if(kernel_pd[pd_idx].present==0){
        void* pt_addr = get_next_free_physical_page();
        map_page(pt_addr,Kptov(pt_addr),F_KERN); /* so that you can write to this address in kernel address space */

        kernel_pd[pd_idx].page_table_base_addr=((uint32_t)pt_addr >> PGBITS); //Only most significant 20bits
        kernel_pd[pd_idx].present=1;
        kernel_pd[pd_idx].read_write=1;
    }
    pt=(page_table_entry*) Kptov(kernel_pd[pd_idx].page_table_base_addr<<PGBITS); //Push back to correct address
    pt[pt_idx].page_base_addr=(uint32_t) paddr>>PGBITS; //Only 20 most significant bits
    pt[pt_idx].present=1;
    pt[pt_idx].read_write=1;

    if(flags & F_KERN) pt[pt_idx].user_supervisor=1;
        
        
    if(flags & F_VERBOSE){
        println("Mapped Page (P:V) ");
        print(itoa((int)paddr,str,BASE_HEX));
        print(":");
        print(itoa((int)vaddr,str,BASE_HEX));
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
void* init_pool(uint8_t flags){
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

    void* pool_addr;

    for(int i=0;i<numPages;i++){
        paddr=get_next_free_physical_page();
        void* vaddr  = Kptov(paddr);
        
        if(i==0) pool_addr=vaddr;

        map_page(paddr,vaddr,flags);

        /*
        if(i==0){
            //update pool pointer to point to start of this allocated section.
            if(flags & F_KERN)kernel_pool=(pool*) vaddr;
            else user_pool=(pool*) vaddr;
        }
        */
    }

    return pool_addr;
}


/* Initialses the first heap page for a given pool
 */
void init_heap_page(pool* pl, uint8_t flags){
    if(pl->next_free_page_index) PANIC("ALREADY INITIALISED HEAP"); /* If next free page index is not 0 then PANIC */

    //Get a new fresh page to be used for heap
    void* paddr = get_next_free_physical_page();

    //Assuming virtual address space for heap starts at 0
    void* vaddr = 0;

    pl->next_free_page_index=1;
    pl->virtual_pages[0].type=M_ALLOCATED;

    map_page(paddr,vaddr,F_KERN);

    pl->virtual_pages[0].base_vaddr=vaddr;
    pl->virtual_pages[0].nextPage=0;
    pl->virtual_pages[0].previousPage=0;

    if(flags & F_KERN){
        pl->ring=0;
    }
    else{
        pl->ring=3;
    }

    //initialise heap
    initialiseHeap(vaddr,PGSIZE);
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


/* Returns physical base page address for that virtual address
 * Can also return bit breakdown of the associated PTE given 
 * dump=true */
void* lookup_phys(void* vaddr, bool dump){
    size_t pt_idx, pd_idx;
    page_table_entry* pt;

    pd_idx=pd_no(vaddr);
    pt_idx=pt_no(vaddr);

    if(kernel_pd[pd_idx].present==0){
        return NO_ADDR;
    }
    pt=(page_table_entry*) Kptov(kernel_pd[pd_idx].page_table_base_addr<<PGBITS);
    if(!dump){
        if(pt[pt_idx].present==0) return NO_ADDR;

    }

    page_table_entry pte=pt[pt_idx];
    println("\nPHYS DUMP:");
    println("Vaddr: ");
    print(itoa((uint32_t)vaddr,str,BASE_HEX));
    println("PTE DATA: ");
    println("Present:");
    print(itoa(pte.present,str,BASE_HEX));
    println("Read/Write:");
    print(itoa(pte.read_write,str,BASE_HEX));
    println("User Supervisor:");
    print(itoa(pte.user_supervisor,str,BASE_HEX));
    println("Write Through:");
    print(itoa(pte.write_through,str,BASE_HEX));
    println("Cache Disabled:");
    print(itoa(pte.cache_disabled,str,BASE_HEX));
    println("Accessed:");
    print(itoa(pte.accessed,str,BASE_HEX));
    println("Dirty:");
    print(itoa(pte.dirty,str,BASE_HEX));
    println("Page Table Attribute Index:");
    print(itoa(pte.page_table_attribute_index,str,BASE_HEX));
    println("Global:");
    print(itoa(pte.global,str,BASE_HEX));
    println("Avail:");
    print(itoa(pte.available,str,BASE_HEX));
    println("Page Base Addr:");
    print(itoa(pte.page_base_addr<<PGBITS,str,BASE_HEX));
    println("");
    
    return (void*)(pt[pt_idx].page_base_addr>>PGBITS);
}
#include "paging.h"
#include "pte.h"
#include "../lib/screen.h"

page_directory_entry *kernel_pd = (page_directory_entry*)PAGE_DIRECTORY_BASE;



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

    kernel_mapping_init();
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

void kernel_mapping_init(){
    
}

void palloc_init(){
    //setup kernel pool
    init_pool(kernel_pool,true);



}

/* Returns virtual address to base address of a new page of allocated memory */
/*
void* get_new_page(){
    //TODODODODO
    //BEFORE UNCOMMENTING
    //FIX THE BIT SHIFTS LIKE INIT POOL HAS


    //TODO figure out if you're kernel or user
    //currently assume just kernel hence using Kptov

    if(true){//IF KERNEL
        void* paddr;
        void* vaddr;
        size_t pd_idx, pt_idx;
        page_table_entry* pt;

        paddr = get_next_free_physical_page();
        vaddr = Kptov(paddr);
        pd_idx=pd_no(vaddr);
        pt_idx=pt_no(vaddr);

        //if the page table page does not exist, create one and fill out the entry
        //in the PD.
        if(kernel_pd[pd_idx].present==0){
            void* pt_addr = get_next_free_physical_page();
            kernel_pd[pd_idx].page_table_base_addr=pt_addr;
            kernel_pd[pd_idx].present=1;
            kernel_pd[pd_idx].read_write=1;
        }

        pt=(page_table_entry*) kernel_pd[pd_idx].page_table_base_addr;
        pt[pt_idx].page_base_addr=paddr;
        pt[pt_idx].present=1;
        pt[pt_idx].read_write=1;

        return vaddr;
    }
    else{
        //TODO implement user shit
        //find out how many pages already allocated to user then virtual address will be
        //that number * 4096 :)
        return 0;
    }

}
*/

void init_pool(pool* mempool, bool kernel){
    //find out how much space is required to store the pool info
    int numBytes= sizeof(pool);//TODO THIS IS WRONG
    //do some maff to round up
    if(numBytes%PGSIZE!=0){
        numBytes=numBytes-(numBytes%PGSIZE)+PGSIZE;
    }
    int numPages=numBytes/PGSIZE;
    println(itoa(numPages,str,BASE_DEC));
    void* paddr;
    void* ptaddr;

    for(int i=0;i<numPages;i++){
        paddr=get_next_free_physical_page();
        size_t vaddr  = Kptov(paddr);
        size_t pd_idx = pd_no(vaddr);
        size_t pt_idx = pt_no(vaddr);
        if(i==0){
            //update kernel pool pointer to point to start of this allocated section.
            kernel_pool=(pool*) vaddr;    
        }

        //check if there is a pde there, if not create one
        if(kernel_pd[pd_idx].present==0){
            ptaddr= get_next_free_physical_page();
            kernel_pd[pd_idx].page_table_base_addr=((uint32_t) ptaddr>>PGBITS);//Only want 20 most significant bits
            kernel_pd[pd_idx].present=1;
            kernel_pd[pd_idx].read_write=1;
            //TODO MEMSET THIS PAGE TO ALL 0
        }
        page_table_entry* pt=(page_table_entry*) (kernel_pd[pd_idx].page_table_base_addr<<PGBITS);
        pt[pt_idx].page_base_addr= ((uint32_t) paddr)>>PGBITS;//Only want 20 most significant bits
        pt[pt_idx].present=1;
        pt[pt_idx].read_write=0;
    }

    //TODO if doing the init_pd* thing then set cr3 to Kvtop(init_pd) but just check that it definitely just want's physical address not virtual.


}
/*
void* palloc_get_multiple(size_t pg_count, uint8_t flags){
    pool* pool = flags & PUSER ? &
}
*/

/* Returns the physical address of the next free physical page and sets
 * it to used and adds it's entry to the pd. Returns 0 if no page is available
 */
void* get_next_free_physical_page(){
    //TODO update this to used linked list or something more intelligent
    //currently stupid and slow but is sufficient
    for(int i=0;i<MAX_PHYS_PAGES;i++){
        if(physical_page_entries[i].type==M_FREE){
            physical_page_entries[i].type=M_FILLED;
            /*
            REMOVE THIS SHIT ITS IN WRONG PLACE
            if(kernel){
                size_t paddr = physical_page_entries[i].phys_addr; 
                size_t vaddr = Kptov(paddr));
                size_t pd_idx= pd_no(vaddr);
                size_t pt_idx= pt_no(vaddr);
                kernel_pd[pd_idx]=pde_create(vaddr);
            }
            */
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
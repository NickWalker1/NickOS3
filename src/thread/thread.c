#include "thread.h"

//setup data stuff
static thread* kernel_thread=(thread*) K_THREAD_BASE; /* Main kernel and initial thread stored at K_THREAD_BASE */
static thread* idle_thread; /* Just spins */

//Declare some useful data structures 
//TODO change these to doubly linked lists probably or queues at least idk
static thread*   all_threads[MAX_THREADS];
static thread* ready_threads[MAX_THREADS];

// really shitty solution, need to sort out static queue thing.
static int num_waiting;
static int ready_idx;

static int tick_count;

static num_threads=0;

/* Changes the current running code into the main kernel thread. */
void thread_init(){
    kernel_thread = (thread*) K_THREAD_BASE;

    if(kernel_thread!=current_thread()) PANIC("stack in wrong place");

    kernel_thread->magic=T_MAGIC;
    kernel_thread->id=create_id();
    // kernel_thread->name="KERNEL";
    kernel_thread->page_directory=get_pd();
    kernel_thread->pool=kernel_pool;
    kernel_thread->priority=1;
    kernel_thread->status=TS_RUNNING;
    kernel_thread->stack=get_esp(); //Goes to top of page and works downwards.


    num_waiting=0;
    ready_idx=0;

}

/* called by PIT interrupt handler */
void thread_tick(){
    //TODO get thread to do some analytics n shit
    
    /* preemption */
    if(++tick_count >= TIME_SLICE){
        thread_yield();
    }
}

void thread_yield(){
    thread* t = current_thread();

    //if not idle thread, push to back of list when list implemented


    //TODO disable interrupts
    t->status=TS_READY;

    //appending to ready threads
    ready_threads[num_waiting++]=t;
    schedule();
}

void schedule(){}

/* Returns address stored in cr3 register */
void* get_pd(){
    void* pd;
    asm("mov %%cr3, %0" : "=g"(pd));
    return (void*) pd;
}

/* Returns current esp value */
void* get_esp(){
    void* esp;

    asm("mov %%esp, %0" : "=g" (esp));
    return esp;
}

/* Returns pointer to current thread */
thread* current_thread(){
    thread* t= (thread*) get_base_page((uint32_t*)get_esp());
    if(t->magic != T_MAGIC) PANIC("TRIED TO RETRIEVE NON-THREAD PAGE");
    return t;
}


t_id create_id(){
    return ++num_threads;
}

/* Rounds down address to nearest 4k alligned number */
uint32_t* get_base_page(uint32_t* addr){
    //TODO this is so fucking ugly surely it must be fixable
    uint32_t remainder = (uint32_t)addr % PGSIZE;
    uint32_t base = (uint32_t)addr;

    return (uint32_t)base-remainder;
}

/* Gets the contents of the current thread and prints it */
void thread_dump(){
    thread* t= current_thread();
    if(t==0) PANIC("NO CURRENT THREAD");
    

    println("ID: ");
    print(itoa(t->id,str,BASE_DEC));
    println("Name: ");
    print(t->name);
    println("Status: ");
    print(itoa(t->status,str,BASE_DEC));
    println("Stack: ");
    print(itoa((uint32_t)t->stack,str,BASE_HEX));
    println("PD: ");
    print(itoa((uint32_t)t->page_directory,str,BASE_HEX));
    println("Pool: ");
    print(itoa((uint32_t)t->pool,str,BASE_HEX));
    println("Priority: ");
    print(itoa(t->priority,str,BASE_HEX));
    println("Magic EXPECTED(0x69696969): ");
    print(itoa(t->magic,str,BASE_HEX));
}
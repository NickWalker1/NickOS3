#include "thread.h"

//setup data stuff
static thread* kernel_thread=(thread*) K_THREAD_BASE; /* Main kernel and initial thread stored at K_THREAD_BASE */
static thread* idle_thread; /* Just spins */

//Declare some useful data structures 
//TODO  proper address space shit between threads
static list*   all_threads;
static list* ready_threads;


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
    

    all_threads=list_init_with(kernel_thread);
    ready_threads=list_init_with(kernel_thread);


}

/* allocates a page in kernel space for this thread and sets
 * some basic info in thread struct and returns 
 */
thread* thread_create(char* name, int priority, thread_func* func, void* aux){
    thread* this= (thread*) palloc_kern(1,F_ASSERT | F_KERN | F_ZERO);
    this->id=create_id();
    this->magic=T_MAGIC;
    this->page_directory=get_pd();
    this->pool=kernel_pool;
    this->priority=1;
    this->stack=((void*)this)+PGSIZE; /* initialise to top of page */
    this->status=TS_BLOCKED;

    int int_level = int_disable();

    //TODO alloc frames for stack entries for context switching
    // to right place (complicated af)


    int_set(int_level);

    //Add to all threads list
    append(all_threads,this);

    /* add to ready queue */
    thread_unblock(this);

    return this;
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
    int int_level=int_disable();
    t->status=TS_READY;

    //appending to ready threads
    append(ready_threads,t);

    schedule();
    //renable intterupts to previous level
    int_set(int_level);
}


void schedule_tail(thread* t){
    //TODO if thread is dying kill it and dealloc page

    t->status=TS_READY;
    append(ready_threads,t);

}

void schedule(){
    thread* curr = current_thread();
    thread* next = get_next_thread();
    thread* prev = 0;

    if(curr->status==TS_RUNNING) PANIC("current process is still running");

    if(curr!=next){
        prev=context_switch(curr,next)->curr;
    }


    //update new thread details
    curr->status=TS_RUNNING;
    tick_count=0;

    //TODO change PD if changing process owner 
    
    //schedule the old thread back into ready queue
    schedule_tail(prev);

}

thread* get_next_thread(){
    //aquire ready queue lock
    //super basic round robin approach
    if(isEmpty(ready_threads)){
        return idle_thread;
    }
    return pop(ready_threads);
}

/* function run by idle thread */
void idle(semaphore* idle_started){
    idle_thread=current_thread();
    sema_up(idle_started);
    for(;;){
        /* Let someone else run. */
        intr_disable ();
        thread_block ();

        /* Re-enable interrupts and wait for the next one.

            The `sti' instruction disables interrupts until the
            completion of the next instruction, so these two
            instructions are executed atomically.  This atomicity is
            important; otherwise, an interrupt could be handled
            between re-enabling interrupts and waiting for the next
            one to occur, wasting as much as one clock tick worth of
            time.

            See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
            7.11.1 "HLT Instruction". */
        asm volatile ("sti; hlt" : : : "memory");
    }
}

void thread_block(){
    if(int_get_level()) PANIC("Cannot block without interrupts off");

    current_thread()->status=TS_BLOCKED; 
    schedule();
    /* this is fine as when something is scheduled 
     * it is popped from the ready queue, 
     * so simply marking it as blocked and scheduling
     * something else will in effect block the thread
     */
}

void thread_unblock(thread* t){
    if(!is_thread(t)) PANIC("NOT THREAD");
    if(t->status!=TS_BLOCKED) PANIC("UNBLOCKING NON-BLOCKED Thread");

    int level=int_disable();
    
    append(ready_threads,t);
    t->stack=TS_READY;
    
    int_set(level);

}


//-----------------------HELPERS--------------------------------

bool is_thread(thread* t){
    return t->magic==T_MAGIC;
}

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
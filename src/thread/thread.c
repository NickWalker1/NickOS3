#include "thread.h"

//setup data stuff
static thread* kernel_thread=(thread*) K_THREAD_BASE; /* Main kernel and initial thread stored at K_THREAD_BASE */
static thread* idle_thread; /* Just spins */

//Declare some useful data structures 
//TODO  proper address space shit between threads
static list*   all_threads;
static list* ready_threads;


static int tick_count;

static int num_threads=0;

/* Changes the current running code into the main kernel thread.
    and creates idle thread.*/
void thread_init(){
    int_disable(); //should already be disabled but yeah
    //TODO change pit interrupt handler to thread tick handler from default
    // but currently just tick handler.
    kernel_thread = (thread*) K_THREAD_BASE;

    // if(kernel_thread!=current_thread()) PANIC("stack in wrong place");


    kernel_thread->magic=T_MAGIC;
    kernel_thread->id=create_id();
    // kernel_thread->name="KERNEL";
    kernel_thread->page_directory=get_pd();
    kernel_thread->pool=kernel_pool;
    kernel_thread->priority=1;
    kernel_thread->status=TS_RUNNING;
    kernel_thread->stack=get_esp(); //Goes to top of page and works downwards.
    

    all_threads=list_init_with(kernel_thread);
    ready_threads=list_init();


    println(itoa(get_esp(),str,BASE_HEX));
    semaphore init_started;
    sema_init(&init_started,0);
    thread* tmp=thread_create("idle thread",5,idle,&init_started);


    int_enable();
    sema_down(&init_started);

    println("FUCK YES");
}

/* allocates a page in kernel space for this thread and sets
 * some basic info in thread struct and returns 
 */
thread* thread_create(char* name, int priority, thread_func* func, void* aux){
    thread* new= (thread*) palloc_kern(1,F_ASSERT | F_KERN | F_ZERO |F_VERBOSE);
    new->id=create_id();
    new->magic=T_MAGIC;
    new->page_directory=get_pd();
    new->pool=kernel_pool;
    new->priority=1;
    new->stack=((uint32_t)new)+PGSIZE; /* initialise to top of page */
    new->status=TS_BLOCKED;

    int int_level = int_disable();

    //context for these next few stack pushes:
    //on each function call a few things are pushed to the stack
    //firstly: the address of the line to return to when the ret instruction
    //is called, this is how nested calls work
    //secondly: each of the function arguments
    //thus each stack struct starts with the eip value of the function of which
    //to 'return' to, but infact it has never been there before. Ha sneaky.

    //run stack frame
    runframe* run_stack=(runframe*) push_stack(new,sizeof(runframe));/*sizeof(runframe)=24*/
    run_stack->eip=NULL;
    run_stack->function=func;
    run_stack->aux=aux;

    //stack for first_switch function. All it does is push the eip of run to the stack so that when ret is
    //called at the end of first_switch it will jump to the start of the run function.
    switch_entry_stack* switch_stack=(switch_entry_stack*) push_stack(new,sizeof(switch_entry_stack));/*sizeof(switch_entry_stack)=8*/
    switch_stack->eip=(void (*) (void)) run;

    //stack contents for context_switch function
    context_switch_stack* context_stack=(context_switch_stack*) push_stack(new,sizeof(context_switch_stack));/*sizeof(context_switch_stack)=40*/
    context_stack->eip = first_switch;
    context_stack->ebp = 0;

    println("new->stack");
    println(itoa(new->stack,str,BASE_HEX));
    println("");
    println("runstack");
    println(itoa(run_stack,str,BASE_HEX));
    println(itoa(sizeof(runframe),str,BASE_HEX));
    println("");
    println("switch_stack");
    println(itoa(switch_stack,str,BASE_HEX));
    println(itoa(sizeof(switch_entry_stack),str,BASE_HEX));
    println("");
    println("context_stack");
    println(itoa(context_stack,str,BASE_HEX));
    println(itoa(sizeof(context_switch_stack),str,BASE_HEX));
    println("");

    println(itoa(run,str,BASE_HEX));
    println(itoa(first_switch,str,BASE_HEX));
    println(itoa((uint32_t)new->stack,str,BASE_HEX));

    
    int_set(int_level);

    //Add to all threads list
    append(all_threads,new);

    /* add to ready queue */
    thread_unblock(new);

    return new;
}

/* called by PIT interrupt handler */
void thread_tick(){
    //TODO get thread to do some analytics n shit
    time_tick();
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


void switch_complete(thread* prev){
    thread* curr = current_thread();

    curr->status=TS_RUNNING;
    tick_count=0;

    //TODO update cr3 if required.

    //TODO if thread is dying kill it 
    if(prev->status==TS_DYING) thread_kill(prev);

    // thread_dump(prev);
    // thread_dump(current_thread());
    /*
    uint32_t *esp = (uint32_t*)get_esp();
    esp+=1;
    println("esp:");
    print(itoa(esp,str,BASE_HEX));
    println("addr:");
    print(itoa(*esp,str,BASE_HEX));
    */
}

void printval(uint32_t val){
    println(itoa(*(uint32_t*)(val+4),str,BASE_HEX));
}

void schedule(){
    thread* curr = current_thread();
    thread* next = get_next_thread();
    thread* prev = 0;

    if(curr->status==TS_RUNNING) PANIC("current process is still running");

    print_attempt("switching from: ");
    print(itoa((uint32_t)curr->stack,str,BASE_HEX));
    print(" to ");
    print(itoa((uint32_t)next->stack,str,BASE_HEX));

    if(curr!=next){
        prev=context_switch(curr,next);
    }
    print_ok();

    //update new thread details
    //TODO change PD if changing process owner 

    //schedule the old thread back into ready queue
    switch_complete(prev);

}

thread* get_next_thread(){
    //aquire ready queue lock
    //super basic round robin approach
    if(is_empty(ready_threads)){
        return idle_thread;
    }
    thread* t = (thread*)pop(ready_threads);
    return t;
}

/* function run by idle thread */
void idle(semaphore* idle_started){
    idle_thread=current_thread();
    println("here");
    thread_dump(current_thread());
    sema_up(idle_started);
    for(;;){
        /* Let someone else run. */
        int_disable ();
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
    t->status=TS_READY;

    int_set(level);

}

void thread_kill(thread* t){
    remove(all_threads,t);
    remove(ready_threads,t);
    //TODO FREE PAGE
}

static void run(thread_func* function, void* aux){
    println("here");
    if(function==NULL) PANIC("NULL FUNCTION");

    int_enable();
    function(aux);

    thread* t= current_thread();
    t->status=TS_DYING;
    
    while(1); //wait to be killed :(
}


//-----------------------HELPERS--------------------------------


static void* push_stack(thread* t, uint32_t size){
    if(!is_thread(t)) PANIC("pushing stack to non-thread");
    t->stack-=size;
    return t->stack;
}


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

    return (uint32_t*)(base-remainder);
}

/* Gets the contents of the current thread and prints it */
void thread_dump(thread* t){
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
    println("Magic: ");
    print(itoa(t->magic,str,BASE_HEX));
}
#pragma once

#include "../lib/typedefs.h"

typedef uint32_t t_id;

typedef void thread_func(void* aux);

typedef enum thread_status
{
    TS_READY,
    TS_RUNNING,
    TS_BLOCKED,
    TS_DYING
} thread_status;



/* Also known as Thread/Process Control Block.
 * This is stored at the bottom of a 4k Page.
 * At the top of the page is the stack pointer
 * for it to grow downwards, hence the use of the magic
 * value which *probably* change when the stack grows too big
 * and this can be checked for corruption and the thread
 * can be killed before any more damage is done. */
typedef struct thread
{
    void *stack; /* just easy so offset is 0 */

    t_id id;
    char name[16];

    thread_status status;

    void *page_directory;
    void *pool; //was heap before

    int priority; /* 1 is highest priority, 5 is lowest NOT CURRENTLY USED WITH ROUND ROBIN AND SUBJECT TO CHANGE */

    // uint32_t current_cpu_usage;
    // uint32_t last_cpu_usage;
    // uint32_t sleep_deadline;
    // uint32_t thread_id_to_wait;

    // interrupt_state state;

    uint32_t magic;

}thread;


typedef struct runframe
{
    void* eip;              //Return addr
    thread_func* function;  //Function to run
    void* aux;              //function arguments
} __attribute__((packed)) runframe;

__attribute__((packed));
typedef struct context_switch_stack 
  {
    uint32_t edi;               /*  0: Saved %edi. */
    uint32_t esi;               /*  4: Saved %esi. */
    uint32_t ebp;               /*  8: Saved %ebp. */
    uint32_t ebx;               /* 12: Saved %ebx. */
    void (*eip) (void);         /* 16: Return address. */
    thread *cur;         /* 20: switch_threads()'s CUR argument. */
    thread *next;        /* 24: switch_threads()'s NEXT argument. */
  } __attribute__((packed)) context_switch_stack;


/* Stack frame for switch_entry(). */
typedef struct switch_entry_stack
  {
    void (*eip) (void);
  }__attribute__((packed)) switch_entry_stack;

#include "tswitch.h"
#include "synch.h"

#include "../lib/list.h"
#include "../lib/panic.h"
#include "../interrupt/cpu_state.h"
#include "../interrupt/idt.h"
#include "../paging/paging.h"

/*
        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
             |               stack             |
        0 kB +---------------------------------+

*/

#define T_MAGIC 0x69696969

#define MAX_THREADS 64

//so that this page ends right before the kernel code is loaded to
#define K_THREAD_BASE 0xC0007000

#define TIME_SLICE 4

/* number of ticks since last yield */



void thread_init();
void thread_tick();
thread* thread_create();
thread* current_thread();
void thread_block();
void thread_unblock(thread* t);
void schedule();
void switch_complete(thread* prev);
static void run(thread_func* function, void* aux);
void idle();
void thread_yield();
thread* get_next_thread();
void thread_kill();

//Helper functions
static void* push_stack(thread* t, uint32_t size);
bool is_thread(thread* t);
void* get_esp();
void* get_pd();
void set_pd(void* pd);
t_id create_id();
bool check_magic();
uint32_t* get_base_page(uint32_t* addr);
void thread_dump(thread* t);
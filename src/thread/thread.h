#pragma once

#include "../lib/typedefs.h"
#include "../idt/cpu_state.h"

typedef enum thread_status
{
    thread_status_ready,
    thread_status_working,
    thread_status_waiting_sleep,
    thread_status_waiting_key_press,
    thread_status_waiting_for_thread,
} thread_status;

typedef struct thread_control_block
{
    uint32_t id;
    char name[32];

    thread_status status;
    uint32_t size_in_memory;

    void *user_stack;
    void *page_directory;
    void *heap;

    // uint32_t current_cpu_usage;
    // uint32_t last_cpu_usage;
    // uint32_t sleep_deadline;
    // uint32_t thread_id_to_wait;

    interrupt_state state;

} thread_conrol_block;

#pragma once

#include "thread.h"

#include "../lib/typedefs.h"
#include "../lib/list.h"

/* A counting semaphore. */
typedef struct semaphore{
    uint32_t value;             /* Current value. */
    list* waiters;        /* List of waiting threads. */
} semaphore;

void sema_init(semaphore* s, unsigned value);
void sema_down(semaphore* s);
bool sema_try_down(semaphore* s);
void sema_up(semaphore* s);
void sema_self_test();

/* Lock. */
typedef struct lock {
    thread *holder;      /* Thread holding lock(for debugging). */
    semaphore semaphore; /* Binary semaphore controlling access. */
} lock;

void lock_init(lock* l);
void lock_acquire(lock* l);
bool lock_try_acquire(lock* l);
void lock_release(lock* l);
bool lock_held_by_current_thread(const lock* l);

/* Condition variable. */
typedef struct condition {
    list waiters;        /* List of waiting threads. */
} condition;

void cond_init(condition* c);
void cond_wait(condition* c, lock* l);
void cond_signal(condition* c, lock* l);
void cond_broadcast(condition* c, lock* l);

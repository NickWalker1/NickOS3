#pragma once

#include "typedefs.h"
#include "../heap/heap.h"

typedef struct kv kv;
typedef struct kv_elem kv_elem;

struct kv{
    uint32_t size;
    kv_elem* head;
    kv_elem* tail;
};

struct kv_elem{
    uint32_t key; /* Pointer to whatever is in it */
    void* value;
    kv_elem* next;
    kv_elem* prev;
};

kv* kv_init();
kv* kv_init_with(uint32_t key, void* value );

void add(kv* l, uint32_t key, void* value);
bool remove(kv* l, uint32_t key);
bool is_empty(kv* l);
void kv_dump(kv* l);
void* kv_get(kv* l, uint32_t key);
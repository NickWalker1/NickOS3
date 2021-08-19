#pragma once

#include "typedefs.h"
#include "../heap/heap.h"

typedef struct list list;
typedef struct list_elem list_elem;

struct list{
    uint32_t size;
    list_elem* head;
    list_elem* tail;
};

struct list_elem{
    void* data; /* Pointer to whatever is in it */
    list_elem* next;
    list_elem* prev;
};

list* list_init();
list* list_init_with(void* data);

void append(list* l, void* data);
void push(list* l, void* data);
void* pop(list* l);
int get_size(list* l);
bool is_empty(list* l);
#include "list.h"


/* creates a new empty list */
list* list_init(){
    list* new_list =(list*) malloc(sizeof(list));
    new_list->size=0;
    new_list->head=new_list->tail=0;
    return new_list;
}

/* Returns pointer to a new list with one element given */
list* list_init(void* data){
    list* new_list = (list*) malloc(sizeof(list));
    new_list->size=1;
    new_list->tail=new_list->head=(list_elem*) malloc(sizeof(list_elem));
    
    new_list->head->data=data;
    new_list->size=1;
    return new_list;
}

void* pop(list* l){
    list_elem* elem = l->head;
    l->head=elem->next;
    l->head->prev=0;
    void* data=elem->data;
    l->size--;
    free(elem);
    return data;
}

void push(list* l, void* data){
    list_elem* elem = (list_elem*) malloc(sizeof(list_elem));
    elem->data=data;
    elem->next=l->head;
    l->head->prev=elem;
    l->head=elem;
    l->size++;
}

void append(list* l, void* data){
    list_elem* elem = (list_elem*) malloc(sizeof(list_elem));
    elem->data=data;
    elem->prev=l->tail;
    l->tail->next=elem;
    l->tail=elem;
    l->size++;
}




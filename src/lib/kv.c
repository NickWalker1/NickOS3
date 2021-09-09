#include "kv.h"


/* creates a new empty kv */
kv* kv_init(){
    kv* new_kv =(kv*) malloc(sizeof(kv));
    new_kv->size=0;
    new_kv->head=new_kv->tail=NULL;
    return new_kv;
}

/* Returns pointer to a new kv with one element given */
kv* kv_init_with(uint32_t key, void* value){
    kv* new_kv = (kv*) malloc(sizeof(kv));
    new_kv->size=1;
    new_kv->tail=new_kv->head=(kv_elem*) malloc(sizeof(kv_elem));
    
    new_kv->head->key=key;
    new_kv->head->value=value;
    new_kv->size=1;
    return new_kv;
}



void add(kv* l, uint32_t key, void* value){
    kv_elem* elem = (kv_elem*) malloc(sizeof(kv_elem));
    elem->key=key;
    elem->value=value;
    if(is_empty(l)) {
        l->head=elem;
        l->tail=elem;
        elem->prev=NULL;
        elem->next=NULL;
        l->size++;
        return;
    }
    elem->prev=l->tail;
    l->tail=elem;
    elem->prev->next=elem;
    l->size++;
}

bool is_empty(kv* l){
    return l->size==0;
}

bool remove(kv* l, uint32_t key){
    kv_elem* elem=l->head;
    while(elem->key!=key && elem->next!=NULL){
        elem=elem->next;
    }
    if(elem->key==key){
        if(elem==l->head) {
            l->head=elem->next;
            l->head->prev=NULL;
        }else{
            elem->prev->next=elem->next;
        }

        if(elem==l->tail) {
            l->tail=elem->prev;
        }else{
            elem->next->prev=elem->prev;
        }
        // free(elem);
        l->size--;
        return true;
    }
    return false;

}

void kv_dump(kv* l){
    kv_elem* elem=l->head;
    println("[");
    while(elem!=NULL){
        print("(");
        print(itoa(elem->key,str,BASE_HEX));
        print(",");
        print(itoa((uint32_t)elem->value,str,BASE_HEX));
        print(")");

        if(elem->next==NULL)break;
        print(",");
        elem=elem->next;
    }
    print("]");
}

void* kv_get(kv* l, uint32_t idx){
    if(idx>=l->size) PANIC("LIST OUT OF BOUNDS EXCEPTION");
    kv_elem* elem= l->head;
    for(int i=0;i<idx;i++) elem=elem->next;
    
    return elem->value;
}


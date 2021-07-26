#include "paging.h"

page_table_entry *page_directory = (page_table_entry*)PAGE_DIRECTORY_BASE;
page_table_entry *page_tables= (page_table_entry*)PAGE_TABLES_BASE;

void paging_init(){
    clear_identity_pages();
}

void clear_identity_pages(){
    //clear first 24 bytes
    for(int i=0;i<6;i++){
        page_directory[i].present=0;
    }
}
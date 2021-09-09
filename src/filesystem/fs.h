#pragma once

#include "../lib/typedefs.h"
#include "../lib/list.h"
#include "../lib/kv.h"


#define MAGIC 0x12345678

typedef struct fs_node   fs_node;
typedef struct direntry direntry;

//https://stackoverflow.com/questions/1591361/understanding-typedefs-for-function-pointers-in-c
typedef uint32_t (*read_type_t)(fs_node*,uint32_t,uint32_t,uint32_t*);
typedef uint32_t (*write_type_t)(fs_node*,uint32_t,uint32_t,uint32_t*);
typedef void (*open_type_t)(fs_node*);
typedef void (*close_type_t)(fs_node*);
typedef direntry * (*readdir_type_t)(fs_node*,uint32_t);
typedef fs_node * (*finddir_type_t)(fs_node*,char *name);


typedef enum node_type{
    FILE,
    DIRECTORY,
    SYM_LINK, 
    DEVICE,
    SOCKET,
    PIPE
} node_type;

struct fs_node{
    char name[128];
    node_type type;
    uint32_t flags;
    uint32_t  size;
    uint32_t inode; //unique file identifier number
    read_type_t read;
    write_type_t write;
    readdir_type_t readdir;
    finddir_type_t finddir;
    fs_node* parent;
    list* dir_contents; /* list of *direntry TODO change to just fs_node */
};

typedef struct rd_file_header{
    uint32_t magic
    char name[128];
    uint32_t addr;      /* offset inside the initrd file */
    uint32_t length;    /* size of actual data stored in the file */
}rd_file_header;

struct direntry{
    char name[128];
    uint32_t inode;
};





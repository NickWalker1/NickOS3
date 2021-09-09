#include "initrd.h"


/* TODO:
 * FIX FLOPPY ISSUE
 * Test KV
 * implement strcpy and strcmp
 * fix link.ld
 * create initrd build script
 * test fs
 * test initrd
 * (fix free)
 * (fix 0x1004 pagefault) 
 * rethink direntry, surely just point to fs_node again?
 *      - in that case can add base addr to fs_node
 * really stupid method currently used to read initrd
 *      - instead should update fs_node addr pointer
 *      - then can just use ram disk as if it was 
 *      - normal memory, but instead maybe extra checks 
 *      - because cannot write to it, unless I move to 
 *      - new memory space where it's free to grow?
 *      - move to user space???
 */


/* Notes:
 * initrd is used to setup the fs such to allow reads from
 * the ram disk file(s) linked in. 
 * The files loaded should not be modified where they are loaded in
 * case of overflow into succeeding files as there is no buffer between
 * them.
 */

rd_header* initrd_header; /* base initram header pointer */
fs_node* initrd_root;



/* the call back function for read when in an initrd fs node 
 * ARGS: node, offset within the file from which to read, number of 
 * bytes to read, and the buffer to write to.
 */
uint32_t initrd_read(fs_node* node, 
                     uint32_t offset, uint32_t size,
                     uint32_t* buffer){
    
    rd_file_header* file =  kv_get(file_lookup,node->inode); //get the file header associated with that inode

    if(file==NULL) return 0;

    //offset too big for file
    if(offset>file->length) return 0;

    //adjust for read too long
    if(offset+size>file->length) size=file.length-offset; 

    //copy the bytes over.
    memcpy(buffer,(file.addr+offset),size);

    //return number of bytes copied
    return size; 
}


/* gets the directory entry stored in directory(node) at index(idx).
 * This function will return NULL if idx too big or if node is not a directory  */
direntry* initrd_readdir(fs_node* node, uint32_t idx){
    if(node->type!=DIRECTORY) return NULL; // cannot index non directory
    if(idx>=node->dir_contents->size) return NULL; //index too big

    return get(node->dir_contents,idx);
}

/* TODO implement */
direntry* initrd_finddir(fs_node* node, char* name){
    
}

bool is_rd_file(rd_file_header* f){
    return f->magic==MAGIC;
}


/* takes location which is found from the linker of where the ram disk
 * data starts at, and sets up the file system around it.
 */
fs_node* initialise_rd_fs(uint32_t location){
    initrd_header = (rd_header*) location;

    //setup root directory
    initrd_root = (fs_node*) malloc(sizeof(fs_node));
    strcpy(initrd_root->name,"/");
    initrd_root->type=DIRECTORY;
    initrd_root->readdir=&initrd_readdir;
    initrd_root->readdir=&initrd_finddir;
    initrd_root->dir_contents=list_init();


    //add dev directory for test
    fs_node* devdir = (fs_node*) malloc(sizeof(fs_node));
    strcpy(devdir->name,"dev");
    devdir->parent=initrd_root;
    devdir->type=DIRECTORY;
    devdir->readdir=&initrd_readdir;
    devdir->finddir=&initrd_finddir;
    devdir->dir_contents=list_init();


    //TODO implement being able to read directory structures from ram disk
    //and load those properly

    //populate root directory with ram disk files
    int rd_file_count =0;
    int i;
    uint32_t addr= location+sizeof(initrd_header);
    for(i=0;i<initrd_header->num_files;i++){
        rd_file_header* rd_file = (rd_file_header*) addr;
        if(!is_rd_file()) break;

        rd_file_count++;

        //setup fs node 
        fs_node* f = malloc(sizeof(fs_node));
        strcpy(f->name,rd_file->name);
        f->type=FILE;
        f->read=&initrd_read;
        f->size=rd_file->length;
        f->inode=rd_file_count;

        //add to inode lookup so inode can be translated to virt address
        file_lookup.add(f->inode,addr);

        //add to root dir contents
        direntry* de = malloc(sizeof(direntry));
        de->inode=f->inode;
        strcpy(de->name,f->name);
        append(initrd_root->dir_contents,de);

        //adjust the addr to point to the next file header in the list
        addr+=(rd_file->length + sizeof(rd_file_header);
    }

    return initrd_root;
}
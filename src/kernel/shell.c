#include "shell.h"


void shell_init(){
    //allocate page in kernel space

    //add struct to store info about which thread owns it and where
    //input buffers are
    // and also where it is currently in the VFS

    //initialise input buffer 
    //char input_buffer[5][128];
}
/* main  shell loop */
void shell_run(){
    if(0) PANIC("SHELL NOT INITIALISED");
    bool run=true;
    while(run){

    }
}

void shell_input_return(){
    //shift up input buffers to store the inputted string in the list

    //parses input buffer

    //if not okay print error & return
    //
}
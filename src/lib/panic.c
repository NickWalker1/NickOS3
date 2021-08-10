#include "panic.h"
#include "screen.h"


void PANIC(char* msg){
    //TODO add function to store CPU register data and print that
    draw_panic_screen();
    println("PANIC");
    println("System fucked");
    println(msg);
}


void draw_panic_screen(){
    int row,column;
    for(column=0;column<80;column++){
        for(row=0;row<25;row++){
            print_char_loc(' ',column,row,WHITE_ON_BLUE);
        }
    }
    set_cursor(get_screen_offset(0,0));
    
}

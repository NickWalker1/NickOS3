#include "screen.h"
#include "../drivers/low_level.h"
#include "memory.h"
#include "int.h"

char str[128];
static struct pos position;

static int attempt_stack[128];
static int stack_idx=0;

//Gets the screen offset for a given location
int get_screen_offset(int col, int row){
    return (row*80+col)*2;
}

//Get cursor
int get_cursor(){
    //The device uses its control regiser as an index
    // to select its internal registers, of which we are
    //interested in:
    //  reg 14: high byte of cursors offset
    //  reg 15: low byte of cursors offset
    //Once they have been selected we can read or write a 
    //byte on the data register.
    port_byte_out(REG_SCREEN_CTRL,14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; //as it's the high bit
    port_byte_out(REG_SCREEN_CTRL,15);
    offset+= port_byte_in(REG_SCREEN_DATA);
    //since the cursor offset is the number of characters 
    //we multiply it by 2 to get char cell offset
    return offset*2;
}

struct pos get_position(int offset){
    offset/=2;
    int row=offset%25;
    int col=offset-(row*80);
    position.row=row;
    position.col=col;
    return position;
}

void set_cursor(int offset){
    offset /=2; //convert from cell to char offset
    
    port_byte_out(REG_SCREEN_CTRL,14);
    port_byte_out(REG_SCREEN_DATA,(unsigned char)(offset>>8));
    port_byte_out(REG_SCREEN_CTRL,15);
    port_byte_out(REG_SCREEN_DATA,(unsigned char)(offset));
}


int handle_scrolling(int offset){
    //if the cursor is within the screen, return it unmodified
    if(offset < MAX_ROWS*MAX_COLS*2){ //this is probably wrong
        return offset;
    }

    //shuffle the rows back one
    int i;
    for(i=1; i<MAX_ROWS;i++){
        memory_copy((char*)get_screen_offset(0,i) + VIDEO_ADDRESS,
                    (char*)get_screen_offset(0,i-1)+ VIDEO_ADDRESS,
                    MAX_COLS*2);

    }
    char* last_line = (char*)get_screen_offset(0,MAX_ROWS-1) + VIDEO_ADDRESS;
    for(i=0;i<MAX_COLS*2;i++){
        last_line[i]=0;
    }
    offset-=2*MAX_COLS;

    return offset;
}

//slightly less efficient due to updating cursor after every char
void print_char(char character,char attribute_type){
    //Create a byte (char) pointer to the start of video memory
    unsigned char * vidmem = (unsigned char *) VIDEO_ADDRESS;

    //if attribute byte is zero, assume default style
    if(!attribute_type){
        attribute_type=WHITE_ON_BLACK;
    }

    int offset=get_cursor();
    if(character=='\n'){
        //set to last character so it wraps around when offset+2
        offset= get_screen_offset(79, offset/(2*MAX_COLS));
    }else{
        //otherwise write it to the correct bit
        vidmem[offset]=character;
        vidmem[offset+1]=attribute_type;
    }

    offset+=2;
    offset=handle_scrolling(offset);

    set_cursor(offset);

}

//Print a char on the screen at a specific poisition or at cursor position
//doesn't affect the cursor
void print_char_loc(char character, int col, int row, char attribute_type){
    //Create a byte (char) pointer to the start of video memory
    unsigned char * vidmem = (unsigned char *) VIDEO_ADDRESS;

    //if attribute byte is zero, assume default style
    if(!attribute_type){
        attribute_type=WHITE_ON_BLACK;
    }

    //Get the video memory offset for the screen location
    int offset;

    if (col>=0 && row>=0){
        offset=get_screen_offset(col,row);
    }else{
        offset=get_cursor();
    }

    if(character=='\n'){
        //doesn't make sense for this 'stupid' function
        //offset= get_screen_offset(79, offset/(2*MAX_COLS));
    }else{
        //otherwise write it to the correct bit
        vidmem[offset]=character;
        vidmem[offset+1]=attribute_type;
    }
}

void print_at(char* message, int col, int row){
    set_cursor(get_screen_offset(col,row));
    int i=0;
    while(message[i] !=0){
        print_char(message[i++],WHITE_ON_BLACK);
    }
}

void print(char* message){
    int i=0;
    while(message[i] !=0){
        print_char(message[i++],WHITE_ON_BLACK);
    }
}

void println(char* message){
    print("\n");
    print(message);
}


void print_attempt(char* message){
    println("[    ] ");
    print(message);
    push_row();
}

void print_ok(){
    print_char_loc('O',2,get_position(get_cursor()).row,GREEN_ON_BLACK);
    print_char_loc('K',3,pop_row(),GREEN_ON_BLACK);

}

void push_row(){
    attempt_stack[stack_idx++]=(get_cursor()/2)%80;
}
int pop_row(){
    return attempt_stack[stack_idx--];
}

void clear_screen(){
    int row;
    int col;

    for(row=0; row<MAX_ROWS;row++){
        for(col=0;col<MAX_COLS;col++){
            print_char_loc(' ',col,row,WHITE_ON_BLACK);
        }
    }
    set_cursor(get_screen_offset(0,0));
}


void test_colours(){
    int colour =0;
    for(int i=0;i<0xf;i++){
        for(int j=0;j<0xf;j++){
            print_char_loc('a',i,j,colour++);
        }
    }
}
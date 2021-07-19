#pragma once
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0f


//Screen device IO ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

struct pos{
    int row;
    int col;
};

int get_screen_offset(int col, int row);
int get_cursor();
struct pos get_position(int offset);
void set_cursor(int offset);
int handle_scrolling(int offset);
void print_char_loc(char character, int col, int row, char attribute_type);
void print_at(char* message, int col, int row);
void print(char* message);
void println(char* message);
void clear_screen();
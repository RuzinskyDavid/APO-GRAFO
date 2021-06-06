#ifndef __LCD__
#define __LCD__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "font_types.h"
#include "terminal.h"

typedef struct
{
	uint16_t red;
	uint16_t blue;
	uint16_t green;
	uint16_t black;
	uint16_t white;
	uint16_t yellow;
} rgb_color_16b_t;

extern rgb_color_16b_t Gcolor16b;

typedef struct
{
	uint64_t red;
	uint64_t blue;
	uint64_t green;
	uint64_t black;
	uint64_t white;
	uint64_t yellow;
} rgb_color_64b_t;

extern rgb_color_64b_t Gcolor64b;

//returns width of char
int get_width_of_char(font_descriptor_t *file_des, int ch_char);

//puts string into text field
void fill_line(int line_num, font_descriptor_t *fdes, char *text, uint16_t color, int font_size);

//colors pixels by start and block size
void color_pixels(uint16_t pix_color, int start_pos, int block_size, volatile uint16_t *lcd);

//get text lenght with file descriptor
int get_text_lenght(font_descriptor_t *file_des, int *size, char *text);

//write character to text field by given color and size
void write_character(int ch_size, int start, uint16_t char_color, font_descriptor_t *file_des, char character, int line_number, volatile uint16_t *lcd);

//draw whole line of characters
void draw_line(int size_of_font, int line_number, uint16_t text_color, font_descriptor_t *file_des, int idx, char *str_text, volatile uint16_t *lcd);

//input string and properites and write to lcd
void final_to_lcd(int size_of_font, int line_number, uint16_t text_color, font_descriptor_t *file_des, char *str_text);

//init messages
void init_lcd_message(int size_of_font, uint16_t text_color, font_descriptor_t *file_des, volatile uint16_t *lcd);

//pick color message
void pick_color(int size_of_font, uint16_t text_color, font_descriptor_t *file_des, volatile uint16_t *lcd);

//erase the lcd
void black_the_lcd(void);

//erase the draw frame
void black_the_draw_frame(void);

//function to make the draw frame
void make_draw_frame(uint16_t pix_color, volatile uint16_t *lcd);

#endif

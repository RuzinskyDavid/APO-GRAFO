#include "lcd.h"

#define LCD_FB_START 0xffe00000
#define LCD_FB_END 0xffe4afff

#define GAP 2 // Gap between two characters, size two
#define LCD_WIDTH 480
#define LCD_HEIGHT 320
#define DRAW_BORDER 255 // Border of the canvas

/* 16 bit colors */
rgb_color_16b_t Gcolor16b =
{
        .red = 0xF800,
        .blue = 0x1F,
        .green = 0x7E0,
        .black = 0x0000,
        .white = 0xFFFF,
        .yellow = 0xFF00,
};

/* 64 bit colors */
rgb_color_64b_t Gcolor64b =
{
        .red = 0xFF0000,
        .blue = 0x0000FF,
        .green = 0x00FF00,
        .black = 0x000000,
        .white = 0xFFFFFF,
        .yellow = 0xFFFF00,
};

/* Function for getting the width of a char from a font for lcd display */
int get_width_of_char(font_descriptor_t *file_des, int ch_char) {

    int ch_width = 0;
    int cond1 = file_des->firstchar <= ch_char;
    int cond2 = file_des->size > (ch_char - file_des->firstchar);
    if (cond1 && cond2)
    {
        ch_char = ch_char - file_des->firstchar;
        if (!file_des->width)
        {
            ch_width = file_des->maxwidth;
        }
        else
        {
            ch_width = file_des->width[ch_char];
        }
    }
    return ch_width;
}

/* Colors pixels in field according to inputs */
void color_pixels(uint16_t pix_color, int start_pos, int block_size, volatile uint16_t *lcd) {

    for (int m = 0; m < block_size; ++m)
    {
        for (int k = 0; k < block_size; ++k)
        {
            lcd[start_pos + m * LCD_WIDTH + k] = pix_color;
        }
    }
}

/* Makes draw frame in field */
void make_draw_frame(uint16_t pix_color, volatile uint16_t *lcd) {

    int size_of_num = 1;
    int side_num = DRAW_BORDER + 10;
    //vertical line
    int x = DRAW_BORDER + 5;
    int lower_line = x + 2;
    for (int y = 0; y < x; ++y)
    {
        lcd[x + y * LCD_WIDTH] = pix_color;
    }
    //horizontal line
    for (int y = 0; y < x; ++y)
    {
        lcd[y + x * LCD_WIDTH] = pix_color;
    }
    //draws numbers to the draw field
    draw_line(size_of_num, 1, Gcolor16b.white, &font_winFreeSystem14x16, side_num, "0", lcd);
    draw_line(size_of_num, 127, Gcolor16b.white, &font_winFreeSystem14x16, side_num, "127", lcd);
    draw_line(size_of_num, lower_line, Gcolor16b.white, &font_winFreeSystem14x16, side_num, "255", lcd);
    draw_line(size_of_num, lower_line, Gcolor16b.white, &font_winFreeSystem14x16, 2, "0", lcd);
    draw_line(size_of_num, lower_line, Gcolor16b.white, &font_winFreeSystem14x16, 127, "127", lcd);
}

/* Returns text length according to file descriptor */
int get_text_lenght(font_descriptor_t *file_des, int *text_size, char *str_text) {

    int ret = 0;
    int lenght = 0;
    *text_size = 0;
    while (str_text[lenght] != '\0')
    {
        lenght = lenght + 1;
        text_size = text_size + get_width_of_char(file_des, str_text[lenght]);
        if (LCD_WIDTH < lenght)
        {
            ret = LCD_WIDTH + 1;
            return ret;
        }
    }
    ret = lenght;
    return ret;
}

/* Writes one character into the text field */
void write_character(int ch_size, int start, uint16_t char_color, font_descriptor_t *file_des, char character, int line_number, volatile uint16_t *lcd) {

    int char_width = get_width_of_char(file_des, character);
    for (int i = 0; i < file_des->height; ++i)
    {
        int index = (character - file_des->firstchar) * file_des->height + i;
        uint16_t letter_of_bit = file_des->bits[index];
        uint16_t bit_mask = 0x8000;

        for (int j = 0; j < char_width; ++j)
        {
            int cond = (letter_of_bit & bit_mask);
            int start_idx = 0;
            if (cond)
            {
                start_idx = start + (line_number + i * ch_size) * LCD_WIDTH +
                            j * ch_size;
                color_pixels(char_color, start_idx, ch_size, lcd);
            }
            else
            {
                start_idx = start + (line_number + i * ch_size) * LCD_WIDTH +
                            j * ch_size;
                color_pixels(0x0, start_idx, ch_size, lcd);
            }
            bit_mask = bit_mask >> 1;
        }
    }
}

/* Writes one line of a string according to inputs */
void draw_line(int size_of_font, int line_number, uint16_t text_color, font_descriptor_t *file_des, int idx, char *str_text, volatile uint16_t *lcd) {

    int text_size = 0;
    int str_lenght = get_text_lenght(file_des, &text_size, str_text);
    int length = text_size * size_of_font + (str_lenght - 1) * size_of_font * GAP;
    int cond = (file_des->height * size_of_font + line_number) >= LCD_HEIGHT;
    if (length > LCD_WIDTH)
    {
        print_str_on_terminal("ERROR: LINE TOO LONG!\n");
    }
    else if (cond)
    {
        print_str_on_terminal("ERROR: BIG LINE NUM OR SIZE!\n");
    }
    else
    {
        int index = 0;
        if (idx == -1)
        {
            index = (LCD_WIDTH - length) / 4;
        }
        else
        {
            index = idx;
        }
        for (int j = 0; j < str_lenght; ++j)
        {
            write_character(size_of_font, index, text_color, file_des, str_text[j], line_number, lcd);
            index = index + (get_width_of_char(file_des, str_text[j]) + GAP) * size_of_font;
        }
    }
}

/* Draws initialization message to lcd display */
void init_lcd_message(int size_of_font, uint16_t text_color, font_descriptor_t *file_des, volatile uint16_t *lcd) {
                      
    int bigger_font = 2;
    int left_side = DRAW_BORDER + 40;
    int middle = -1;

    //first messages
    print_str_on_terminal("HELLO!\n");
    print_str_on_terminal("PLEASE WAIT FOR LCD TO LOAD!\n");

    //whole drawing of lcd screen
    make_draw_frame(Gcolor16b.white, lcd);
    draw_line(bigger_font, 1, text_color, file_des, left_side, " GRAFO", lcd);
    draw_line(size_of_font, 36, text_color, &font_winFreeSystem14x16, left_side, " <-- DRAW SCREEN", lcd);
    draw_line(size_of_font, 56, text_color, &font_winFreeSystem14x16, left_side, " PICK COLOR", lcd);
    pick_color(size_of_font, text_color, file_des, lcd);
    draw_line(size_of_font, 176, text_color, &font_winFreeSystem14x16, left_side, " TYPE 'P' TO ERASE", lcd);
    draw_line(size_of_font, 196, text_color, &font_winFreeSystem14x16, left_side, " TYPE 'Q' TO QUIT", lcd);
    draw_line(size_of_font, 290, Gcolor16b.green, &font_winFreeSystem14x16, middle, "BY: DAVID RUZINSKY", lcd);
	
    print_str_on_terminal("TYPE TO TERMINAL INPUT TO PICK COLOR!\n");
}

/* Draw color options to lcd */
void pick_color(int size_of_font, uint16_t text_color, font_descriptor_t *file_des, volatile uint16_t *lcd) {
                   
    //draws colors options
    int left_side = DRAW_BORDER + 40;
    draw_line(size_of_font, 76, Gcolor16b.red, &font_winFreeSystem14x16, left_side, " 'R' - RED", lcd);
    draw_line(size_of_font, 96, Gcolor16b.green, &font_winFreeSystem14x16, left_side, " 'G' - GREEN", lcd);
    draw_line(size_of_font, 116, Gcolor16b.blue, &font_winFreeSystem14x16, left_side, " 'B' - BLUE", lcd);
    draw_line(size_of_font, 136, Gcolor16b.yellow, &font_winFreeSystem14x16, left_side, " 'Y' - YELLOW", lcd);
    draw_line(size_of_font, 156, Gcolor16b.white, &font_winFreeSystem14x16, left_side, " 'W' - WHITE", lcd);
}

/* Erases the whole lcd display */
void black_the_lcd(void) {

    print_str_on_terminal("WAIT FOR BLACK SCREEN!\n");
    volatile uint16_t *lcd_t = (typeof(lcd_t))LCD_FB_START;
    for (int i = 0; i < LCD_HEIGHT * LCD_WIDTH; ++i)
    {
        lcd_t[i] = Gcolor16b.black;
    }
    print_str_on_terminal("READY TO DRAW!\n");
}

/* Erases just the draw frame for new drawing */
void black_the_draw_frame(void) {

    int block_size = DRAW_BORDER;
    print_str_on_terminal("WAIT FOR BLACK FRAME!\n");
    volatile uint16_t *lcd_t = (typeof(lcd_t))LCD_FB_START;
    for (int m = 0; m < block_size; ++m)
    {
        for (int k = 0; k < block_size; ++k)
        {
            lcd_t[m * LCD_WIDTH + k] = Gcolor16b.black;
        }
    }
    print_str_on_terminal("READY TO DRAW!\n");
}


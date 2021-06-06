At startup, the program loads peripherals status, sets drawing color, and writes directly to LCD.
Program loads positions of red and green knobs and in case of a difference it computes a suitable neighbour to the current cursor position.
Program can also change the drawing color if the value of blue knob has changed or user used the terminal to change the color.
Current drawing color is indicated by two LEDs.

The drawing is controlled by red and green knob. Red knob value is horizontal coordinate of the cursor. 
Green knob value is vertical coordinate of the cursor. In the moment when you change some of the two values, 
the cursor will start to move for that new coordinate that you just set.  
Line of the drawing will be continuous and whenever you change the values, it will move to new coordinate.

Files and their specific documentation:

- main.c {
	# typedef struct knobs_t
	This structure contains values from every knob

		* uint8_t R
		* uint8_t G
		* uint8_t B
	# typedef struct point_t
	This structure contains vertical and horizontal coordinates of draw pointer

		* uint16_t x
		* uint16_t y

	# int main()
		* main of the code, loops it to draw on lcd
	# void loadKnobs(volatile knobs_t *knobs)
		* gets values from each knob
	# void setPoint(point_t *newPoint, uint16_t x, uint16_t y)
		* prepares new point for draw
	# void show_color_on_led(uint64_t color)
		* set LED´s colors to input value
	# void drawPoint(point_t *oldPoint, point_t *newPoint, volatile uint16_t *lcd, uint16_t color_16b)
		* draw new point into the lcd display
	# void set_draw_color(uint16_t *color16b, uint64_t *color64b, int wait_time)
		* gets color from input and changes the draw color and led color to it
	# void setDrawColor_knob(uint16_t *color16b, uint64_t *color64b, volatile knobs_t *knobs)
		* changes the color based on blue knob value
}

- terminal.c {
	# volatile int terminalIn()
		* Gets one character from terminal input
	# volatile int my_terminalIn(int wait_time)
		* Gets one character from terminal input but with a timeout
	# void terminalOut(volatile int print)
		* Writes data to terminal output
	# void print_str_on_terminal(char *str)
		* Write string to terminal output
	# unsigned long myStrLen(const char *str)
		* Returns length of a string
	# void print_coordinates(uint16_t x, uint16_t y)
		* Prints out vertical and horizontal coordinate from input
	# char *my_int_to_str_convert(int number, char *outStr)
		* Converts number into string
	# void print_num_terminal(int num)
		* Prints number into terminal
	# volatile int pick_color_trm(uint16_t *color16b, uint64_t *color64b, int wait_time)
		* Gets color choice from terminal input with and without timeout
}

- lcd.c {
	**LCD_FB_START 0xffe00000** - start address of lcd display
	**LCD_FB_END 0xffe4afff** - end address of lcd display
	**GAB 2** - gab between two characters, size two
	**LCD_WIDTH 480** - width of lcd display
	**LCD_HEIGHT 320** - height of lcd display
	**DRAW_BORDER 255** - border in pixels for drawing

	uint16_t text_field[LCD_WIDTH * LCD_HEIGHT] - lcd buffer

	# structure Gcolor16b

	This structure contains colors in 16 bites.
	
	uint16_t red;
	uint16_t blue;
	uint16_t green;
	uint16_t black;
	uint16_t white;
	uint16_t yellow;

	# structure Gcolor64b

	This structure contains colors in 64 bites.
	
	uint64_t red;
	uint64_t blue;
	uint64_t green;
	uint64_t black;
	uint64_t white;
	uint64_t yellow;

	# int get_width_of_char(font_descriptor_t *file_des, int ch_char)
		* Function for getting the width of a char from a fond for lcd display
	# void color_pixels(uint16_t pix_color, int start_pos, int block_size)
		* Colors pixels in field according to inputs
	# void make_draw_frame(uint16_t pix_color)
		* Makes draw frame in field
	# int get_text_lenght(font_descriptor_t *file_des, int *text_size, char *str_text)
		* Returns text length according to file descriptor
	# void write_character(int ch_size, int start, uint16_t char_color, font_descriptor_t *file_des, char character, int line_number)
		* Writes one character into the text field
	# void draw_line(int size_of_font, int line_number, uint16_t text_color, font_descriptor_t *file_des, int idx, char *str_text)
		* Writes one line of a string according to inputs
	# void init_lcd_message(int size_of_font, uint16_t text_color, font_descriptor_t *file_des)
		* Draws initialization message to lcd display
	# void pick_color(int size_of_font, uint16_t text_color, font_descriptor_t *file_des)
		* Draw color options to lcd
	# void black_the_lcd(void)
		* Erases the whole lcd display
	# void black_the_draw_frame(void)
		* Erases just the draw frame for new drawing
}
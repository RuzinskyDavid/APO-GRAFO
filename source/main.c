// Defining all the adressses
#define SERIAL_PORT_BASE 0xffffc000

#define SERP_RX_ST_REG_o 0x00
#define SERP_RX_ST_REG_READY_m 0x1
#define SERP_RX_ST_REG_IE_m 0x2

#define SERP_RX_DATA_REG_o 0x04

#define SERP_TX_ST_REG_o 0x08
#define SERP_TX_ST_REG_READY_m 0x1
#define SERP_TX_ST_REG_IE_m 0x2

#define SERP_TX_DATA_REG_o 0x0c


#define SPILED_REG_BASE 0xffffc100

#define SPILED_REG_LED_LINE_o 0x004
#define SPILED_REG_LED_RGB1_o 0x010
#define SPILED_REG_LED_RGB2_o 0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o 0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o 0x024


#define LCD_FB_START 0xffe00000
#define LCD_FB_END 0xffe4afff
// End of defining adresses

#define LCD_WIDTH 480
#define LCD_HEIGHT 320

#include "terminal.h"
#include "lcd.h"
#include "font_types.h"

typedef struct { // Struct for knobs
	uint8_t R;
	uint8_t G;
	uint8_t B;
} knobs_t;

typedef struct { // Pointer for drawing
	uint16_t x;
	uint16_t y;
} point_t;

void loadKnobs(volatile knobs_t *knobs);
void setPoint(point_t *newPoint, uint16_t x, uint16_t y);
void show_color_on_led(uint64_t color);
void drawPoint(point_t *oldPoint, point_t *newPoint, volatile uint16_t *lcd, uint16_t color_16b);
int setDrawColor(uint16_t *color16b, uint64_t *color64b, int wait_time);
void setDrawColor_knob(uint16_t *color16b, uint64_t *color64b, volatile knobs_t *knobs);


int main(void) {
	
	//Init
	int small_font = 1;
	int wait_time = 100;
	int terminal_print_period = 100
	int inf_time = -1;
	volatile uint16_t *lcd_t = (typeof(lcd_t))LCD_FB_START;
	volatile knobs_t knobs = {0, 0, 0};

	uint16_t color16b;
	uint64_t color64b;

	// Init position
	loadKnobs(&knobs);
	point_t oldPoint;
	setPoint(&oldPoint, knobs.R, knobs.G);
	point_t newPoint;
	setPoint(&newPoint, knobs.R, knobs.G);

	// LCD draw
	init_lcd_message(small_font, Gcolor16b.yellow, &font_winFreeSystem14x16, lcd_t);
	setDrawColor(&color16b, &color64b, inf_time);

	// The main code cycle
	int print_c = 0;
	int i_break = 0;
	uint8_t last_col = 0;
	while (1) {	
		loadKnobs(&knobs);
		setPoint(&newPoint, knobs.R, knobs.G);
		drawPoint(&oldPoint, &newPoint, lcd_t, color16b);
		i_break = setDrawColor(&color16b, &color64b, wait_time);	
		
		if(knobs.B != last_col) { // Changing color with knob
			setDrawColor_knob(&color16b, &color64b, &knobs);
			last_col = knobs.B;
		}
		
		if(i_break == 1) { // End of program
			print_str_on_terminal("YOU EXITED THE GAME\n");
			print_str_on_terminal("HAVE A NICE DAY\n");
			break;
		}

		if (print_c % terminal_print_period == 0) { // Print coordinates and info
			print_coordinates(newPoint.x, newPoint.y);
			print_str_on_terminal("HOLD BUTTON TO CHANGE COLOR\n");
			print_c = 0;
		}
		++print_c;
	}
	return 0;
}

/* Getting knobs values */
void loadKnobs(volatile knobs_t *knobs) {

	volatile uint32_t *knobs_ptr = (uint32_t *)(SPILED_REG_BASE + SPILED_REG_KNOBS_8BIT_o);
	knobs->B = 0xFF & *knobs_ptr;
	knobs->G = 0xFF & (*knobs_ptr >> 8);
	knobs->R = 0xFF & (*knobs_ptr >> 16);
}

/* Setting new point */
void setPoint(point_t *newPoint, uint16_t x, uint16_t y) {

	newPoint->x = x;
	newPoint->y = y;
}

/* Changes color of leds */
void show_color_on_led(uint64_t color) {

	unsigned char *mem_base = (unsigned char *)SPILED_REG_BASE;
	*(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB1_o) = color;
	*(volatile uint32_t *)(mem_base + SPILED_REG_LED_RGB2_o) = color;
}

/* Draws new point */
void drawPoint(point_t *oldPoint, point_t *newPoint, volatile uint16_t *lcd, uint16_t color_16b) {

	int ii = newPoint->x > oldPoint->x ? 1 : -1;
	int ji = newPoint->y > oldPoint->y ? 1 : -1;
	int i = 0;
	int j = 0;
	if (oldPoint->x + i != newPoint->x) {
		i += ii;
	}
	if (oldPoint->y + j != newPoint->y) {
		j += ji;
	}
	lcd[(oldPoint->y + j) * LCD_WIDTH + (oldPoint->x + i)] = color_16b;

	oldPoint->x += i;
	oldPoint->y += j;
}

/* Changes draw color and led color */
int setDrawColor(uint16_t *color16b, uint64_t *color64b, int wait_time) {

	int ret = 0;
	ret = pick_color_trm(color16b, color64b, wait_time);
	if (ret == 'q')	{
		return 1;
	}
	show_color_on_led(*color64b);
	return 0;
}

/* Changes draw color and led color based on B knob data */
void setDrawColor_knob(uint16_t *color16b, uint64_t *color64b,
                       volatile knobs_t *knobs) {
	
	if(knobs->B < 51) {
		print_str_on_terminal("PICKED: RED\n");
        	*color16b = Gcolor16b.red;
        	*color64b = Gcolor64b.red;
	} else if(knobs->B < 102) {
		print_str_on_terminal("PICKED: GREEN\n");
        	*color16b = Gcolor16b.green;
        	*color64b = Gcolor64b.green;
	} else if(knobs->B < 153) {
		print_str_on_terminal("PICKED: BLUE\n");
        	*color16b = Gcolor16b.blue;
        	*color64b = Gcolor64b.blue;
	} else if(knobs->B < 204) {
		print_str_on_terminal("PICKED: YELLOW\n");
        	*color16b = Gcolor16b.yellow;
        	*color64b = Gcolor64b.yellow;
	} else if(knobs->B <= 255) {
		print_str_on_terminal("PICKED: WHITE\n");
        	*color16b = Gcolor16b.white;
        	*color64b = Gcolor64b.white;
	}
	show_color_on_led(*color64b);
}


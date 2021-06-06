//defining all the adresses
#define SERIAL_PORT_BASE 0xffffc000

#define SERP_RX_ST_REG_o 0x00
#define SERP_RX_ST_REG_READY_m 0x1
#define SERP_RX_ST_REG_IE_m 0x2

#define SERP_RX_DATA_REG_o 0x04
#define SERP_TX_ST_REG_o 0x08
#define SERP_TX_ST_REG_READY_m 0x1
#define SERP_TX_ST_REG_IE_m 0x2

#define SERP_TX_DATA_REG_o 0x0c

#include "terminal.h"
#include "lcd.h"

/* Gets one character from terminal input */
volatile int terminalIn() {

    volatile int *state = (int *)(SERIAL_PORT_BASE + SERP_RX_ST_REG_o);
    volatile int *data = (int *)(SERIAL_PORT_BASE + SERP_RX_DATA_REG_o);
    while (*state != SERP_RX_ST_REG_READY_m)
        ;
    return *data;
}

/* Gets one character from terminal input but with a timeout */
volatile int terminal_in_timeout(int wait_time) {

    volatile int *data = (int *)(SERIAL_PORT_BASE + SERP_RX_DATA_REG_o);
    int i = 0;
    while (i != wait_time)
    {
        ++i;
        if (*data != 0)
        {
            return *data;
        }
    }
    return 0;
}

/* Writes data to terminal output */
void terminalOut(volatile int print) {

    volatile int *state = (int *)(SERIAL_PORT_BASE + SERP_TX_ST_REG_o);
    volatile int *data = (int *)(SERIAL_PORT_BASE + SERP_TX_DATA_REG_o);
    while (*state != SERP_TX_ST_REG_READY_m)
        ;
    *data = print;
}

/* Write string to terminal output */
void print_str_on_terminal(char *str) {

    for (int i = 0; i < myStrLen(str); ++i)
    {
        terminalOut(str[i]);
    }
}

/* Returns length of a string */
unsigned long myStrLen(const char *str) {

    unsigned long ret = 0;
    while (str && str[ret] != '\0')
    {
        ++ret;
    }
    return ret;
}

/* Prints out vertical and horizontal coordinate from input */
void print_coordinates(uint16_t x, uint16_t y) {

    print_str_on_terminal("Coord are: ");
    char str_x[12];
    char str_y[12];
    char *str_x_ptr = str_x;
    char *str_y_ptr = str_y;
    str_x_ptr = my_int_to_str_convert(x, str_x_ptr);
    str_y_ptr = my_int_to_str_convert(y, str_y_ptr);
    print_str_on_terminal("X: ");
    print_str_on_terminal(str_x);
    print_str_on_terminal(" ");
    print_str_on_terminal("Y: ");
    print_str_on_terminal(str_y);
    print_str_on_terminal("\n");
}

/* Converts number into string */
char *my_int_to_str_convert(int number, char *outStr) {

    int str_len = 12;
    char rev_str[12];
    int num_len = 0;
    int i;
    for (i = 0; i < str_len; ++i)
    {
        char letter;
        letter = (number % 10) + 48;
        number /= 10;
        rev_str[i] = letter;
        if (!number)
        {
            num_len = i;
            break;
        }
    }

    //reversing
    i = 0;
    for (int j = num_len; j >= 0; --j)
    {
        if (rev_str[j])
        {
            outStr[i] = rev_str[j];
            ++i;
        }
    }
    outStr[i] = '\0';
    return outStr;
}

/* Prints number into terminal */
void print_num_terminal(int num) {

    char str_x[12];
    char *str_x_ptr = str_x;
    str_x_ptr = my_int_to_str_convert(num, str_x_ptr);
    print_str_on_terminal(str_x);
    print_str_on_terminal("\n");
}

/* Gets color choice from terminal input with and without timeout */
volatile int pick_color_trm(uint16_t *color16b, uint64_t *color64b, int wait_time) {

    int start_wait = -1;
    volatile int ret = -1;
    if (wait_time == -1)
    {
        ret = terminalIn();
    }
    else
    {
        ret = terminal_in_timeout(wait_time);
    }
    switch (ret)
    {
    case 0: // nothing picked
        break;
    case 'r':
        print_str_on_terminal("PICKED: RED\n");
        *color16b = Gcolor16b.red;
        *color64b = Gcolor64b.red;
        break;
    case 'g':
        print_str_on_terminal("PICKED: GREEN\n");
        *color16b = Gcolor16b.green;
        *color64b = Gcolor64b.green;
        break;
    case 'b':
        print_str_on_terminal("PICKED: BLUE\n");
        *color16b = Gcolor16b.blue;
        *color64b = Gcolor64b.blue;
        break;
    case 'y':
        print_str_on_terminal("PICKED: YELLOW\n");
        *color16b = Gcolor16b.yellow;
        *color64b = Gcolor64b.yellow;
        break;
    case 'w':
        print_str_on_terminal("PICKED: WHITE\n");
        *color16b = Gcolor16b.white;
        *color64b = Gcolor64b.white;
        break;
    case 'p':
        if (wait_time != -1)
        {
            print_str_on_terminal("ERASE DRAW SCREEN\n");
            black_the_draw_frame();
        }
        else
        {
            print_str_on_terminal("DRAW SCREEN IS BLANK\n");
        }
        break;
    case 'q':
        //quiting the code
        break;
    default:
        print_str_on_terminal("TRY STARTING LETTER OF SOME COLOR\n");
        if (wait_time == -1)
        {   
            //chosing color at the start
            ret = pick_color_trm(color16b, color64b, start_wait);
        }
        break;
    }
    return ret;
}


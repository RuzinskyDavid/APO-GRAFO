#ifndef __TERMINAL__
#define __TERMINAL__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//Gets one character from terminal input
volatile int terminalIn();

//Gets one character from terminal input but with a timeout
volatile int terminal_in_timeout(int wait_time);

//Writes data to terminal output
void terminalOut(volatile int print);

//Write string to terminal output
void print_str_on_terminal(char *str);

//Returns length of a string
unsigned long myStrLen(const char *str);

//Prints out vertical and horizontal coordinate from input
void print_coordinates(uint16_t x, uint16_t y);

//Converts number into string
char *my_int_to_str_convert(int number, char *outStr);

//Prints number into terminal
void print_num_terminal(int num);

//Gets color choice from terminal input with and without timeout
volatile int pick_color_trm(uint16_t *color16b, uint64_t *color64b, int wait_time);

#endif

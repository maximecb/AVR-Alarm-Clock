#ifndef _LCD_H_
#define _LCD_H_

// Header files
#include <stdio.h>
#include <stdbool.h>

// LCD module initialization function
extern void lcd_init();

// Function to clear the LCD display
extern void lcd_clear();

// Test that the LCD is still working properly
extern bool lcd_ping();

// Function to set the LCD printing line
extern void lcd_set_line(int lineNo);

// Write a character to the display
extern int lcd_putchar(char ch, FILE* stream);

// File handle to write to the LCD
extern FILE* lcd;

#endif // #ifndef _LCD_H_


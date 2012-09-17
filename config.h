#ifndef _CONFIG_H_
#define _CONFIG_H_

// Header files
#include "utility.h"
#include <avr/io.h>

// Define CPU frequency
#define F_CPU 8000000UL

// Define baud rate
#define USART_BAUD 19200UL
#define USART_UBBR_VALUE ((F_CPU / (USART_BAUD << 4)) - 1)

// Test LED
//MAKE_OUTPUT(TEST_LED, B, 0, 1)

// Alarm status LED
//MAKE_OUTPUT(ALARM_LED, B, 1, 1)

// Keypad configuration
MAKE_OUTPUT(KEY_OUT, C, 0, 4)
MAKE_INPUT(KEY_IN, C, 4, 4)

// LCD screen configuration
#define LCD_NUM_LINES 4
MAKE_OUTPUT(LCD_RS, A, 0, 1)
MAKE_OUTPUT(LCD_RW, A, 1, 1)
MAKE_OUTPUT(LCD_E, A, 2, 1)
MAKE_OUTPUT(LCD_DATA, A, 4, 4)

#endif // #ifndef _CONFIG_H_


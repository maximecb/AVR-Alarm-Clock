/*****************************************************************************
*
*                               AVR Alarm Clock
*
*  This file is part of the AVR Alarm Clock project. The project is
*  distributed at:
*  https://github.com/maximecb/AVR-Alarm-Clock
*
*  Copyright (c) 2012, Maxime Chevalier-Boisvert. All rights reserved.
*
*  This software is licensed under the following license (Modified BSD
*  License):
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are
*  met:
*   1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*   3. The name of the author may not be used to endorse or promote
*      products derived from this software without specific prior written
*      permission.
*
*  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
*  NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
*  NOT LIMITED TO PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
*  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

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
#define LCD_LINE_COUNT 4
#define LCD_LINE_LENGTH 20
MAKE_OUTPUT(LCD_RS, A, 0, 1)
MAKE_OUTPUT(LCD_RW, A, 1, 1)
MAKE_OUTPUT(LCD_E, A, 2, 1)
MAKE_OUTPUT(LCD_DATA, A, 4, 4)
MAKE_INPUT(LCD_DATA_IN, A, 4, 4)

#endif // #ifndef _CONFIG_H_


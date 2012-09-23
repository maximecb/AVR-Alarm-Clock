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

#ifndef _CLOCK_H_
#define _CLOCK_H_

// Header files
#include <stdbool.h>

// Time value structure
typedef struct timeval_t
{
    uint16_t year;

    // Month number in [1,12]
    uint8_t month;

    // Day of the month in [1, 31]
    uint8_t date;

    uint8_t hour;

    uint8_t min;

    uint8_t sec;

} TimeVal;

// Test whether a given year is a leap year
extern bool isLeapYear(uint16_t year);

// Compute the number of days in a given month
extern uint8_t daysInMonth(uint8_t month, uint16_t year);

// Compute the weekday for a given date
extern uint8_t getWeekday(TimeVal* time);

// Get the weekday string for a given date
extern const char* getDayStr(TimeVal* time);

// Get the month string for a given date
extern const char* getMonthStr(TimeVal* time);

// Compare two time values
extern int cmpTime(TimeVal* timeA, TimeVal* timeB);

// Increment the date of a time value
extern void nextDate(TimeVal* time);

// Set the current time
extern void setTime(TimeVal* timeVal);

// Get the current time
extern void getTime(TimeVal* timeVal);

#endif // #ifndef _CLOCK_H_


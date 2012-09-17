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


// Header files
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "utility.h"
#include "keypad.h"
#include "clock.h"
#include "lcd.h"

//=============================================================================
// Misc output handling
//=============================================================================

MAKE_OUTPUT(OUT_BUZZER, G, 0, 1)
MAKE_OUTPUT(OUT_LED_ALARM, G, 1, 1)
//MAKE_OUTPUT(OUT_RELAY, G, 0, 2)

void out_init()
{
    INIT_OUT_BUZZER();
    INIT_OUT_LED_ALARM();
    //INIT_OUT_RELAY();

    //OUT_RELAY(1);
}

void out_relay(uint8_t status)
{
    //OUT_RELAY(status? 0:1);
}

//=============================================================================
// Main function
//=============================================================================

typedef enum viewstate_t
{
    VIEW_MAIN,
    VIEW_ALARM,
    VIEW_TIME

} ViewState;

typedef enum setstate_t
{
    EDIT_NONE,

    EDIT_HOUR,
    EDIT_MIN,
    EDIT_SEC,

    EDIT_DATE,
    EDIT_MONTH,
    EDIT_YEAR,

} EditState;

// Current view
ViewState viewState = VIEW_MAIN;

// Time value being edited
EditState editState = EDIT_NONE;

// Index of the digit for the current edit
int8_t digitIdx = 0;

// Alarm time
TimeVal alarmTime;

// Current time
TimeVal curTime;

// Alarm on/off flag
bool alarmOn = false;

// Second count, used for basic timing functions
uint32_t secCount = 0;

void setState(ViewState vs, EditState es)
{
    lcd_clear();
    viewState = vs;
    editState = es;
    digitIdx = 0;
}

/**
Compute the date for the next alarm
*/
void setAlarmDate()
{
    alarmTime.year  = curTime.year;
    alarmTime.month = curTime.month;
    alarmTime.date  = curTime.date;

    // If the alarm is not past the current time,
    // move the alarm time to tomorrow
    if (cmpTime(&alarmTime, &curTime) <= 0)
        nextDate(&alarmTime);
}

void printTime(TimeVal* pTime, bool showDate)
{
    if (editState != EDIT_HOUR || (secCount % 2) == 0)
        fprintf(lcd, "%02d:", (int)pTime->hour);
    else
        fprintf(lcd, "  :");

    if (editState != EDIT_MIN || (secCount % 2) == 0)
        fprintf(lcd, "%02d:", (int)pTime->min);
    else
        fprintf(lcd, "  :");

    if (editState != EDIT_SEC || (secCount % 2) == 0)
        fprintf(lcd, "%02d\n", (int)pTime->sec);
    else
        fprintf(lcd, "  \n");

    if (showDate == false)
        return;

    // Always show the weekday, it isn't editable
    fprintf(lcd, "%s ", getDayStr(pTime));

    if (editState != EDIT_DATE || (secCount % 2) == 0)
        fprintf(lcd, "%02d ", (int)pTime->date);
    else
        fprintf(lcd, "   ");

    if (editState != EDIT_MONTH || (secCount % 2) == 0)
        fprintf(lcd, "%s ", getMonthStr(pTime));
    else
        fprintf(lcd, "    ");

    if (editState != EDIT_YEAR || (secCount % 2) == 0)
        fprintf(lcd, "%04d \n", (int)pTime->year);
    else
        fprintf(lcd, "     \n");
}

bool editTime(TimeVal* pTime, bool editDate, uint16_t keys)
{
    // Next value key
    if (keys & KEY_B)
    {
        if (editState < EDIT_SEC || (editDate && editState < EDIT_YEAR))
            editState++;
        else
            editState = EDIT_HOUR;

        digitIdx = 0;
    }

    // Previous value key
    if (keys & KEY_A)
    {
        if (editState > EDIT_HOUR)
            editState--;
        else
            editState = editDate? EDIT_YEAR:EDIT_SEC;

        digitIdx = 0;
    }

    // Get the edited value
    int curVal = 0;
    int minVal = 0;
    int maxVal = 99;
    bool numeric = true;
    switch (editState)
    {
        case EDIT_HOUR: 
        curVal = pTime->hour; 
        maxVal = 23;
        break;

        case EDIT_MIN: 
        curVal = pTime->min;
        maxVal = 59;
        break;

        case EDIT_SEC: 
        curVal = pTime->sec;
        maxVal = 59;
        break;

        case EDIT_DATE: 
        curVal = pTime->date;
        minVal = 1;
        maxVal = daysInMonth(pTime->month, pTime->year);
        break;

        case EDIT_MONTH:
        curVal = pTime->month;
        minVal = 1;
        maxVal = 12;
        numeric = false;
        break;

        case EDIT_YEAR:
        curVal = pTime->year;
        maxVal = 2099;
        break;

        default:
        break;
    }

    // Store the original value
    int origVal = curVal;

    // Increase value key
    if (keys == KEY_C)
    {
        if (curVal < maxVal)
            curVal++;
        else
            curVal = minVal;
    }

    // Decrease value key
    if (keys == KEY_D)
    {
        if (curVal > minVal)
            curVal--;
        else
            curVal = maxVal;
    }

    // Parse which digit was pressed, if any
    int digit = -1;
    switch (keys)
    {
        case KEY_0: digit = 0; break;
        case KEY_1: digit = 1; break;
        case KEY_2: digit = 2; break;
        case KEY_3: digit = 3; break;
        case KEY_4: digit = 4; break;
        case KEY_5: digit = 5; break;
        case KEY_6: digit = 6; break;
        case KEY_7: digit = 7; break;
        case KEY_8: digit = 8; break;
        case KEY_9: digit = 9; break;
    }

    // If a digit was pressed and the edited value is numeric
    if (digit != -1 && numeric == true)
    {
        switch (digitIdx)
        {
            case 0:
            curVal = 
                curVal - 
                ((curVal / 10) % 10) * 10 +
                10 * digit;
            break;

            case 1:
            curVal = 
                curVal - 
                (curVal % 10) +
                digit;
            break;
        }

        // Cap the value
        if (curVal < minVal)
            curVal = minVal;
        else if (curVal > maxVal)
            curVal = maxVal;

        // Move to the next digit
        digitIdx++;
    }

    // Set the edited value
    switch (editState)
    {
        case EDIT_HOUR: pTime->hour = curVal; break;
        case EDIT_MIN:  pTime->min = curVal; break;
        case EDIT_SEC:  pTime->sec = curVal; break;
        case EDIT_DATE: pTime->date = curVal; break;
        case EDIT_MONTH:pTime->month = curVal; break;
        case EDIT_YEAR: pTime->year = curVal; break;
        default: break;
    }

    // If all digits were entered for this value
    if (digitIdx > 1)
    {
        if (editState < EDIT_SEC || (editDate && editState < EDIT_YEAR))
            editState++;
        else
            editState = EDIT_HOUR;

        digitIdx = 0;
    }

    // Return a boolean indicating if a value was changed
    return (curVal != origVal);
}

void mainView(uint16_t keys)
{
    // Print the current time
    printTime(&curTime, true);
    
    fprintf(lcd, "Alarm %s\n", alarmOn? "ON ":"OFF");

    switch (secCount / 4 % 3)
    {
        case 0: fprintf(lcd, "A: alarm on/off\n"); break;
        case 1: fprintf(lcd, "B: set alarm time\n"); break;
        case 2: fprintf(lcd, "C: set current time\n"); break;
    }

    if (keys & KEY_A)
    {
        alarmOn = !alarmOn;
        setAlarmDate();
    }

    if (keys & KEY_B)
        setState(VIEW_ALARM, EDIT_HOUR);

    if (keys & KEY_C)
        setState(VIEW_TIME, EDIT_HOUR);
}

void alarmView(uint16_t keys)
{
    fprintf(lcd, "Set Alarm Time\n");

    // Print the alarm time, without the date
    printTime(&alarmTime, false);

    fprintf(lcd, "\n");

    switch (secCount / 4 % 3)
    {
        case 0: fprintf(lcd, "A/B: prev/next value\n"); break;
        case 1: fprintf(lcd, "C/D: incr/decr value\n"); break;
        case 2: fprintf(lcd, "#: back to main menu\n"); break;
    }

    // Edit the time, but not the date
    bool changed = editTime(&alarmTime, false, keys);

    // If the alarm time was changed, set the alarm date
    if (changed == true)
        setAlarmDate();

    // Return to menu key
    if (keys & KEY_SHARP)
        setState(VIEW_MAIN, EDIT_NONE);
}

void timeView(uint16_t keys)
{
    fprintf(lcd, "Set Current Time\n");

    printTime(&curTime, true);

    switch (secCount / 4 % 3)
    {
        case 0: fprintf(lcd, "A/B: prev/next value\n"); break;
        case 1: fprintf(lcd, "C/D: incr/decr value\n"); break;
        case 2: fprintf(lcd, "#: back to main menu\n"); break;
    }

    // Edit the time, including the date
    bool changed = editTime(&curTime, true, keys);

    // If the time value was changed, set the new time
    if (changed == true)
        setTime(&curTime);

    // Return to menu key
    if (keys & KEY_SHARP)
        setState(VIEW_MAIN, EDIT_NONE);
}

int main(void)
{
    // Initialize the outputs
    out_init();

    // Initialize the keypad
    keypad_init();

    // Initialize I2C library
    i2c_init();                             

    // Initialize the LCD
    lcd_init();

    // For each iteration
    for (;;)
    {
        // If the LCD is not responding, reset it
        if (lcd_ping() == false)
            lcd_init();

        //lcd_clear();
        lcd_set_line(0);

        // Read the keypad
        uint16_t keys = keypad_read();

        // If a key was pressed, add a debouncing delay
        if (keys)
            _delay_ms(350);

        // Get the seconds value at the last iteration
        uint8_t lastSec = curTime.sec;

        // Read the current time
        getTime(&curTime);

        // Update the seconds count
        if (curTime.sec != lastSec)
            secCount++;

        // If the current time is past the alarm time
        if (alarmOn == true && cmpTime(&curTime, &alarmTime) > 0)
        {
            // Ring the alarm
            OUT_BUZZER(secCount % 2);

            // If any key is pressed, or one hour has elapsed
            if (keys != 0 || alarmTime.hour != curTime.hour)
            {
                // Move the alarm time to tomorrow
                setAlarmDate();
                OUT_BUZZER(0);

                // Don't let this key trigger any other action
                keys = 0;
            }
        }

        // Indicate the alarm status with the alarm led
        OUT_LED_ALARM(alarmOn? 1:0);

        // Switch on the view state
        switch (viewState)
        {
            case VIEW_MAIN:
            mainView(keys);
            break;

            case VIEW_ALARM:
            alarmView(keys);
            break;

            case VIEW_TIME:
            timeView(keys);
            break;
        }
    }
}


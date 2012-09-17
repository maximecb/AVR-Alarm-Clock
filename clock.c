// Header files
#include "config.h"
#include "i2cmaster.h"
#include "lcd.h"
#include "clock.h"

//=============================================================================
// Real-Time Clock (RTC) handling
//=============================================================================

#define DEV_DS1307 0xD0

// Day of the week names
const char* dayNames[] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

// Month names
const char* monthNames[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

uint8_t fromBCD(uint8_t byte)
{
    return ((byte & 0xF0) >> 4) * 10 + (byte & 0xF);
}

uint8_t toBCD(uint8_t byte)
{
    return (((byte / 10) % 10) << 4) + (byte % 10);
}

bool isLeapYear(uint16_t year)
{
    return (
        (year % 400 == 0) || 
        ((year % 4 == 0) && (year % 100 != 0))
    );
}

uint8_t daysInMonth(uint8_t month, uint16_t year)
{
    switch (month)
    {
        // 30 days hath September, April, June and November
        case 9: // Sep
        case 4: // Apr
        case 6: // Jun
        case 11: // Nov
        return 30;

        // All the rest have 31
        default:
        return 31;

        case 2: // Feb
        return isLeapYear(year)? 29:28;
    }
}

uint8_t getWeekday(TimeVal* time)
{
    int year = time->year;
    int month = time->month;
    int date = time->date;

    int century = year / 100;

    int cyear = year - 100 * century;

    int c = 2 * (3 - (century % 4));

    int y = cyear + cyear / 4;
    
    int m = 0;
    switch (month)
    {
        case 1 : isLeapYear(year)? 6:0; break;
        case 2 : isLeapYear(year)? 2:3; break;
        case 3 : m = 3; break;
        case 4 : m = 6; break;
        case 5 : m = 1; break;
        case 6 : m = 4; break;
        case 7 : m = 6; break;
        case 8 : m = 2; break;
        case 9 : m = 5; break;
        case 10: m = 0; break;
        case 11: m = 3; break;
        case 12: m = 5; break;
    }

    // Compute the weekday
    return (c + y + m + date) % 7;
}

const char* getDayStr(TimeVal* time)
{
    return dayNames[getWeekday(time)];
}

const char* getMonthStr(TimeVal* time)
{
    return monthNames[time->month - 1];
}

int cmpTime(TimeVal* tA, TimeVal* tB)
{
    if (tA->year < tB->year)
        return -1;
    if (tA->year > tB->year)
        return 1;

    if (tA->month < tB->month)
        return -1;
    if (tA->month > tB->month)
        return 1;

    if (tA->date < tB->date)
        return -1;
    if (tA->date > tB->date)
        return 1;

    if (tA->hour < tB->hour)
        return -1;
    if (tA->hour > tB->hour)
        return 1;

    if (tA->min < tB->min)
        return -1;
    if (tA->min > tB->min)
        return 1;

    if (tA->sec < tB->sec)
        return -1;
    if (tA->sec > tB->sec)
        return 1;

    return 0;
}

void nextDate(TimeVal* time)
{
    // If this is the last day of the current month
    if (time->date >= daysInMonth(time->month, time->year))
    {
        // It is now the first of the following month
        time->date = 1;
        
        // If this is the last month of the year
        if (time->month == 12)
        {
            // It is now the first month of the next year
            time->month = 1;
            
            // Move to the next year
            time->year++;
        }
        else
        {
            // Simply move to the next month
            time->month++;
        }
    }
    else
    {
        // Simply move to the next day of the month
        time->date++;
    }
}

void setTime(TimeVal* timeVal)
{
    /*
    WRITE: 0xD0 ACK<< DS1307 write address
    WRITE: 0x00 ACK<< location to write
    WRITE: 0x00 ACK<< 0 seconds, reset CH bit to 0
    WRITE: 0x30 ACK<< 30 minutes
    WRITE: 0x09 ACK<< 9 hours, 24 hour clock mode
    WRITE: 0x02 ACK<< Monday (day 2)
    WRITE: 0x07 ACK<< 7th (date)
    WRITE: 0x09 ACK<< September (month)
    WRITE: 0x09 ACK<< '09 (year)
    WRITE: 0x00 ACK<< Set control register to 0'
    I2C STOP BIT
    */

    // Compute the BCD data
    uint8_t yearByte  = toBCD(timeVal->year - 2000);
    uint8_t monthByte = toBCD(timeVal->month);
    uint8_t dateByte  = toBCD(timeVal->date);
    uint8_t dayByte   = toBCD(getWeekday(timeVal) + 1);
    uint8_t hourByte  = toBCD(timeVal->hour);
    uint8_t minByte   = toBCD(timeVal->min);
    uint8_t secByte   = toBCD(timeVal->sec);

    // Set device address and write mode
    i2c_start_wait(DEV_DS1307 + I2C_WRITE);

    // Write at address 0
    i2c_write(0x00);

    i2c_write(secByte);     // Seconds, CH bit
    i2c_write(minByte);     // Minutes
    i2c_write(hourByte);    // Hours, 12/24h clock mode
    i2c_write(dayByte);     // Weekday
    i2c_write(dateByte);    // Date
    i2c_write(monthByte);   // Month
    i2c_write(yearByte);    // Year

    // Set control register to 0
    i2c_write(0x00);

    // Stop, release bus    
    i2c_stop();
}

void getTime(TimeVal* timeVal)
{
    /*
    I2C START BIT
    WRITE: 0xD0 ACK<< DS1307 write address
    WRITE: 0x00 ACK<< set write pointer
    I2C START BIT
    WRITE: 0xD1 ACK<< DS1307 read address
    READ: 0x30 ACK<< 30 seconds
    READ: 0x30 ACK<< 30 minutes
    READ: 0x09 ACK<< 9 hours, 24 hour clock mode
    READ: 0x02 ACK<< Monday (day 2)
    READ: 0x07 ACK<< 7th (date)
    READ: 0x09 ACK<< September (month)
    READ: 0x09 ACK<< '09 (year)
    READ: 0x00 NACK<< Control register
    I2C STOP BIT
    */

    unsigned char ret;

    // Set device address and write mode
    ret = i2c_start(DEV_DS1307 + I2C_WRITE);

    if (ret != 0)
        return;

    // Write read address
    ret = i2c_write(0x00);

    if (ret != 0)
        return;

    // Set device address and read mode
    ret = i2c_rep_start(DEV_DS1307 + I2C_READ);

    if (ret != 0)
        return;

    // Read the time data
    unsigned char secByte   = i2c_readAck();
    unsigned char minByte   = i2c_readAck();
    unsigned char hourByte  = i2c_readAck();
    /*unsigned char dayByte=*/i2c_readAck();
    unsigned char dateByte  = i2c_readAck();
    unsigned char monthByte = i2c_readAck();
    unsigned char yearByte  = i2c_readAck();

    // Read the control register
    i2c_readNak();

    // Release the bus
    i2c_stop();

    // Compute the year
    timeVal->year = 2000 + fromBCD(yearByte);

    // Compute the month index
    timeVal->month = fromBCD(monthByte);

    // Compute the date
    timeVal->date = fromBCD(dateByte);

    // Compute the hours, minutes and seconds
    timeVal->hour = fromBCD(hourByte);
    timeVal->min = fromBCD(minByte);
    timeVal->sec = fromBCD(secByte);
}


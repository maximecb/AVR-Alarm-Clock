// Header files
#include "config.h"
#include "lcd.h"
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>

//=============================================================================
// LCD interface code (HD44780-compatible)
//=============================================================================

#define LCD_LINE_0_START 0x00
#define LCD_LINE_1_START 0x40
#define LCD_LINE_2_START 0x14
#define LCD_LINE_3_START 0x54

void lcd_write4(uint8_t rs, uint8_t db)
{
    // Setup the data port for writing
    INIT_LCD_DATA();

    LCD_RW(0);

    LCD_RS(rs);

    LCD_DATA(db);

    LCD_E(1);
    _delay_us(2);
    LCD_E(0);
    _delay_us(2);
}

void lcd_write8(uint8_t rs, uint8_t db)
{
    lcd_write4(rs, db >> 4);
    lcd_write4(rs, db);
}

uint8_t lcd_read4(uint8_t rs)
{
    // Setup the data port for reading
    INIT_LCD_DATA_IN();

    LCD_RW(1);

    LCD_RS(rs);

    LCD_E(1);
    _delay_us(2);

    uint8_t data = LCD_DATA_IN();

    LCD_E(0);
    _delay_us(2);

    return data;
}

uint8_t lcd_read8(uint8_t rs)
{
    return (lcd_read4(rs) << 4) | lcd_read4(rs);
}

bool lcd_busy()
{
    uint8_t byte = lcd_read8(0);
    return (byte & 0b10000000)? true:false;
}

void lcd_command(uint8_t rs, uint8_t db)
{
    lcd_write8(rs, db);

    // Wait for the command to complete
    for (uint8_t i = 0; lcd_busy() == true && i != 0xFF; ++i);
}

void lcd_init()
{
    INIT_LCD_RS();
    INIT_LCD_RW();
    INIT_LCD_E();
    INIT_LCD_DATA();

    // Reset the LCD display
    _delay_us(50);
    lcd_write4(0, 0x3);
    _delay_us(5);
    lcd_write4(0, 0x3);
    _delay_us(5);
    lcd_write4(0, 0x3);
    _delay_us(5);

    // Entry mode set: 4-bit mode
    lcd_write4(0, 0b0010);
    _delay_us(50);

    // Function set: 2 line display and 5x8 font
    lcd_command(0, 0b00101000);

    // Display on/off control: display on, no cursor
    lcd_command(0, 0b00001100);

    // Entry mode set
    lcd_command(0, 0b00000110);

    // Clear the LCD
    lcd_clear();
}

void lcd_clear()
{
    // Clear the display
    lcd_command(0, 0b00000001);
}

int lcd_line_no = 0;

void lcd_set_line(int lineNo)
{
    // Display data ram is 80 x 8 bits
    // 20 chars per line, 4 lines

    uint8_t address;

    switch (lineNo)
    {
        case 0: address = LCD_LINE_0_START; break;
        case 1: address = LCD_LINE_1_START; break;
        case 2: address = LCD_LINE_2_START; break;
        case 3: address = LCD_LINE_3_START; break;
        default: address = LCD_LINE_0_START; break;
    }
    
    lcd_line_no = lineNo;

    // Set the cursor position (DDRAM address)
    lcd_command(0, 128 + address);
}

int lcd_putchar(char c, FILE* stream)
{
    if (c == '\n')
    {
        if (lcd_line_no >= LCD_NUM_LINES - 1)
            lcd_set_line(0);
        else
            lcd_set_line(lcd_line_no + 1);  
    }
    else
    {
        lcd_command(1, c);
    }

    return 0;
}

bool lcd_ping()
{
    // Read the address counter
    uint8_t ac1 = lcd_read8(0) & 0b01111111;

    // Move the cursor right
    lcd_command(0, 0x14);

    // Read the address counter
    uint8_t ac2 = lcd_read8(0) & 0b01111111;

    // Move the cursor left
    lcd_command(0, 0x10);

    return (ac2 == ac1 + 1);
}

FILE lcdFile = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
FILE* lcd = &lcdFile;


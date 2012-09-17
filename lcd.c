// Header files
#include "config.h"
#include "lcd.h"
#include <stdio.h>
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
    LCD_RW(0);

    LCD_RS(rs);

    LCD_DATA(db);

    LCD_E(1);
    _delay_us(100);
    LCD_E(0);
    _delay_us(100);
}

void lcd_write8(uint8_t rs, uint8_t db)
{
    lcd_write4(rs, db >> 4);
    lcd_write4(rs, db);
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
    _delay_us(1);
    lcd_write4(0, 0x3);
    _delay_us(1);

    // Entry mode set: 4-bit mode
    lcd_write4(0, 0b0010);
    _delay_ms(10);

    // Function set: 2 line display and 5x8 font
    lcd_write8(0, 0b00101000);
    _delay_ms(2);

    // Display on/off control: display on, no cursor
    lcd_write8(0, 0b00001100);
    _delay_us(50);

    // Entry mode set
    lcd_write4(0, 0b0000);
    lcd_write4(0, 0b0110);
    _delay_us(50);

    // Clear the LCD
    lcd_clear();
}

void lcd_clear()
{
    // Clear display
    lcd_write8(0, 0b00000001);
    _delay_ms(2);
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
    lcd_write8(0, 128 + address);
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
        lcd_write8(1, c);
    }

    return 0;
}

FILE lcdFile = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
FILE* lcd = &lcdFile;


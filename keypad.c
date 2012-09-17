// Header files
#include "config.h"
#include <stdio.h>
#include <util/delay.h>

//=============================================================================
// 4x4 matrix keypad interface code
//=============================================================================

void keypad_init()
{
    INIT_KEY_OUT();
    INIT_KEY_IN();
}

uint16_t keypad_read()
{
    uint16_t keys = 0;

    uint16_t key = 1;

    for (uint8_t row = 0; row < 4; ++row)
    {
        // Set this row high, others low
        KEY_OUT(1 << row);

        _delay_ms(10);

        uint8_t in = KEY_IN();

        for (uint8_t col = 0; col < 4; ++col)
        {
            // If this column is 0, the key is down
            if ((in & (1 << col)) == 0)
                keys |= key;

            key <<= 1;
        }
    }

    KEY_OUT(0);

    return keys;
}


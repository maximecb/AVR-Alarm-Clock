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


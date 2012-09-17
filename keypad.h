#ifndef _KEYPAD_H_
#define _KEYPAD_H_

// Key definitions
#define KEY_1       (1 << 0)
#define KEY_2       (1 << 1)
#define KEY_3       (1 << 2)
#define KEY_A       (1 << 3)
#define KEY_4       (1 << 4)
#define KEY_5       (1 << 5)
#define KEY_6       (1 << 6)
#define KEY_B       (1 << 7)
#define KEY_7       (1 << 8)
#define KEY_8       (1 << 9)
#define KEY_9       (1 << 10)
#define KEY_C       (1 << 11)
#define KEY_STAR    (1 << 12)
#define KEY_0       (1 << 13)
#define KEY_SHARP   (1 << 14)
#define KEY_D       (1 << 15)

extern void keypad_init();

extern uint16_t keypad_read();

#endif // #ifndef _KEYPAD_H_


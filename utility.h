#ifndef _UTILITY_H_
#define _UTILITY_H_

// Output pin handling macro
#define MAKE_OUTPUT(IO_NAME, P_LTR, PIN_NO, NUM_BITS)                   \
inline void IO_NAME(uint8_t val)                                        \
{                                                                       \
    uint8_t NMASK = (uint8_t)(                                          \
        ~((0xFF >> (8-NUM_BITS)) << PIN_NO)                             \
    );                                                                  \
                                                                        \
    PORT##P_LTR = (PORT##P_LTR & NMASK) | (val << PIN_NO);              \
}                                                                       \
inline void INIT_##IO_NAME()                                            \
{                                                                       \
    uint8_t MASK = (uint8_t)(                                           \
        ((0xFF >> (8-NUM_BITS)) << PIN_NO)                              \
    );                                                                  \
                                                                        \
    DDR##P_LTR = DDR##P_LTR | MASK;                                     \
    IO_NAME(0);                                                         \
}

// Input pin handling macro
#define MAKE_INPUT(IO_NAME, P_LTR, PIN_NO, NUM_BITS)                    \
inline uint8_t IO_NAME()                                                \
{                                                                       \
    uint8_t MASK = (uint8_t)(                                           \
        ((0xFF >> (8-NUM_BITS)) << PIN_NO)                              \
    );                                                                  \
                                                                        \
    uint8_t val = (PIN##P_LTR & MASK) >> PIN_NO;                        \
    return val;                                                         \
}                                                                       \
inline void INIT_##IO_NAME()                                            \
{                                                                       \
    uint8_t NMASK = (uint8_t)(                                          \
        ~((0xFF >> (8-NUM_BITS)) << PIN_NO)                             \
    );                                                                  \
                                                                        \
    DDR##P_LTR = DDR##P_LTR & NMASK;                                    \
}

#endif // #ifndef _UTILITY_H_

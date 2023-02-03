#ifndef _SHIFT_REGISTER_H_
#define _SHIFT_REGISTER_H_

#include <stdint.h>


// NO MALLOCS HERE
// All the new shift_registers are put in a (private) array with a fixed (at compilation) size
#define SHIFT_REGISTER_NB CONFIG_SHIFT_REGISTER_NB


struct shift_register;
typedef struct shift_register* shift_register_t;

extern shift_register_t shift_register__create(uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb);
extern void shift_register__send(shift_register_t s, uint64_t bits);

#endif

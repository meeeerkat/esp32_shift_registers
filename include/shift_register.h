#ifndef _SHIFT_REGISTER_H_
#define _SHIFT_REGISTER_H_

#include <stdint.h>



typedef struct {
  uint8_t ser, srclk, rclk;
  uint8_t bits_nb;
  uint64_t bits_sent;
} shift_register_t;

extern void shift_register__init(shift_register_t* sr, uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb);
extern void shift_register__send(shift_register_t* sr, uint64_t bits);
extern void shift_register__update(shift_register_t* sr, uint64_t mask, uint64_t bits);

#endif

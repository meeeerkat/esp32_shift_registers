
#include "shift_register.h"

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"





void shift_register__init(shift_register_t* sr, uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb) {
  // Fields setup
  sr->ser = ser;
  sr->srclk = srclk;
  sr->rclk = rclk;
  sr->bits_nb = bits_nb;
  sr->bits_sent = 0;

  // gpios setup
  gpio_set_direction(ser, GPIO_MODE_OUTPUT);
  gpio_set_direction(srclk, GPIO_MODE_OUTPUT);
  gpio_set_direction(rclk, GPIO_MODE_OUTPUT);
  gpio_set_level(ser, 0);
  gpio_set_level(srclk, 0);
  gpio_set_level(rclk, 0);

  // Reset register
  shift_register__send(sr, 0);
}

void shift_register__send(shift_register_t* sr, uint64_t bits) {
  // We just send a request to the task (async behaviour)
  // Handling of a shift_register__send request
  for (uint8_t b=0; b < sr->bits_nb; b++) {
    // Data bit set
    gpio_set_level(sr->ser, (bits >> b) & 1);
    // SRCLK rising edge (and back to 0)
    gpio_set_level(sr->srclk, 1);
    gpio_set_level(sr->srclk, 0);
  }
  // RCLK rising edge (and back to 0)
  gpio_set_level(sr->rclk, 1);
  gpio_set_level(sr->rclk, 0);

  sr->bits_sent = bits;
}

void shift_register__update(shift_register_t* sr, uint64_t mask, uint64_t bits) {
  // same as sr->bits_sent but with zeroes where the mask applies
  const uint64_t cleared_bits = sr->bits_sent & (~mask);
  // mask & bits -> we only update bits that are in the mask (the others aren't modified)
  const uint64_t new_bits = cleared_bits | (mask & bits);
  shift_register__send(sr, new_bits);
}

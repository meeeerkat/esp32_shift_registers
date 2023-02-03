
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

  // Maybe reset register ???
  // using a new reset pin or just setting it all at 0 ???
  // right now bits_sent isn't right before the first send
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


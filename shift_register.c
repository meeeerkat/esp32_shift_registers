
#include "shift_register.h"

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"





void shift_register__init(shift_register_t* sr, uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb) {

  sr->ser = ser;
  sr->srclk = srclk;
  sr->rclk = rclk;
  sr->bits_nb = bits_nb;

  gpio_set_direction(ser, GPIO_MODE_OUTPUT);
  gpio_set_direction(srclk, GPIO_MODE_OUTPUT);
  gpio_set_direction(rclk, GPIO_MODE_OUTPUT);

  spi_bus_config_t buscfg={
    .miso_io_num = -1,
    .mosi_io_num = sr->ser,
    .sclk_io_num = sr->srclk,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1
  };
  
  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED); 

  spi_device_interface_config_t devcfg={
   //.command_bits = 0,
   //.address_bits = 0,
   //.dummy_bits = 0,
   .mode = 0,
   .clock_speed_hz = 1,
//   .spics_io_num = -1,
   .queue_size = 1,
   .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_BIT_LSBFIRST,
  };
  spi_bus_add_device(SPI2_HOST, &devcfg, &sr->spi_handle);

  gpio_set_level(sr->rclk, 0);
  // Reset register
  shift_register__send(sr, 0);
}

void shift_register__send(shift_register_t* sr, uint64_t bits) {
  spi_transaction_t transaction = {
    .length = sr->bits_nb,
    .tx_buffer = &bits,
    .rx_buffer = NULL
  };

  esp_err_t err = spi_device_transmit(sr->spi_handle, &transaction);
  printf("HERE %u\n", err);

  // RCLK rising edge (and back to 0)
  gpio_set_level(sr->rclk, 1);
  vTaskDelay(pdMS_TO_TICKS(1000));
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

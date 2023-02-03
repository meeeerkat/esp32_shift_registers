
#include "shift_register.h"

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"




QueueHandle_t tosend_queue = NULL;





void shift_register__init(shift_register_t* sr, uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb) {
  // Fields setup
  sr->ser = ser;
  sr->srclk = srclk;
  sr->rclk = rclk;
  sr->bits_nb = bits_nb;
  sr->bits_to_send = 0;

  // gpios setup
  gpio_set_direction(ser, GPIO_MODE_OUTPUT);
  gpio_set_direction(srclk, GPIO_MODE_OUTPUT);
  gpio_set_direction(rclk, GPIO_MODE_OUTPUT);
  gpio_set_level(ser, 0);
  gpio_set_level(srclk, 0);
  gpio_set_level(rclk, 0);
}

void shift_register__send(shift_register_t* sr, uint64_t bits) {
  // We just send a request to the task (async behaviour)
  sr->bits_to_send = bits;
  xQueueSend(tosend_queue, &sr, 0);
}




void shift_registers_task(void *args)
{
  shift_register_t* sr;
  while (1) {
    if(xQueueReceive(tosend_queue, &sr, portMAX_DELAY)) {
      // Handling of a shift_register__send request
      for (uint8_t b=0; b < sr->bits_nb; b++) {
        // Data bit set
        gpio_set_level(sr->ser, (sr->bits_to_send >> b) & 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        // SRCLK rising edge (and back to 0)
        gpio_set_level(sr->srclk, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(sr->srclk, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      // RCLK rising edge (and back to 0)
      gpio_set_level(sr->rclk, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      gpio_set_level(sr->rclk, 0);
    }
  }
}


void init_shift_registers_handling() {
  tosend_queue = xQueueCreate(20, sizeof(shift_register_t));
  xTaskCreate(shift_registers_task, "shift_registers_task", 2048, NULL, 5, NULL);
}



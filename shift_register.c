
#include "shift_register.h"

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"



struct shift_register {
  uint8_t ser, srclk, rclk;
  uint8_t bits_nb;
  uint64_t bits_to_send;
};



struct shift_register components[SHIFT_REGISTERS_NB];
size_t components_nb;
QueueHandle_t tosend_queue = NULL;





shift_register_t shift_register__create(uint8_t ser, uint8_t srclk, uint8_t rclk, uint8_t bits_nb) {
  // check if we're not creating too many shift registers
  if (components_nb == SHIFT_REGISTERS_NB)
    return NULL;

  // Fields setup
  shift_register_t new_comp = &components[components_nb];
  new_comp->ser = ser;
  new_comp->srclk = srclk;
  new_comp->rclk = rclk;
  new_comp->bits_nb = bits_nb;
  new_comp->bits_to_send = 0;

  // gpios setup
  gpio_set_direction(ser, GPIO_MODE_OUTPUT);
  gpio_set_direction(srclk, GPIO_MODE_OUTPUT);
  gpio_set_direction(rclk, GPIO_MODE_OUTPUT);
  gpio_set_level(ser, 0);
  gpio_set_level(srclk, 0);
  gpio_set_level(rclk, 0);

  // components_nb update
  components_nb++;

  // return handle to call methods on
  return new_comp;
}

void shift_register__send(shift_register_t sr, uint64_t bits) {
  sr->bits_to_send = bits;
  xQueueSend(tosend_queue, &sr, 0);
}




void shift_registers_task(void *args)
{
  shift_register_t sr;
  while (1) {
    if(xQueueReceive(tosend_queue, &sr, portMAX_DELAY)) {
      for (uint8_t b=0; b < sr->bits_nb; b++) {
        gpio_set_level(sr->ser, (sr->bits_to_send >> b) & 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        printf("srclk, 1\n");
        gpio_set_level(sr->srclk, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        printf("srclk, 0\n");
        gpio_set_level(sr->srclk, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      printf("rclk, 1\n");
      gpio_set_level(sr->rclk, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      printf("rclk, 0\n");
      gpio_set_level(sr->rclk, 0);
    }
  }
}


void init_shift_registers_handling() {
  components_nb = 0;
  tosend_queue = xQueueCreate(20, sizeof(shift_register_t));
  xTaskCreate(shift_registers_task, "shift_registers_task", 2048, NULL, 5, NULL);
}



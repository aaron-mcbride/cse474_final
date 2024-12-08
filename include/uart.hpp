
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

struct read_uart_param_t {
  int32_t uart_num = 0;
  int32_t baud_rate = 9600;
  int32_t data_len;
  uint8_t data[1024];
  

}

void read_uart_task(void*);

#include "uart.hpp"
#include <HardwareSerial.h>

uart_output_t uart_output[uart_cnt]{};

static constexpr int32_t uart_task_delay{100};
static constexpr int32_t uart_num_array[uart_cnt]{1, 2, 3};
TaskHandle_t uart_task_array[uart_cnt]{nullptr};
HardwareSerial* const uart_inst_array[uart_cnt]{&Serial1, &Serial2, &Serial3};

static void uart_task(void* param) {
  const int32_t uart_num = *((int32_t*)param);
  HardwareSerial* uart_inst{uart_inst_array[uart_num]};
  for (;;) {
    if (uart_inst->available()) {
      xSemaphoreTake(uart_output[uart_num].data_sem, portMAX_DELAY);
      uart_output[uart_num].buffer.push(uart_inst->read());
      xSemaphoreGive(uart_output[uart_num].data_sem);
    }
    vTaskDelay(uart_task_delay / portTICK_PERIOD_MS);
  }
}

void init_uart() {
  static constexpr char* uart_name_array[uart_cnt]{"uart0", "uart1", "uart2"};
  for (int i = 0; i < uart_cnt; i++) {
    uart_output[i].data_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(uart_output[i].data_sem);
    xTaskCreate(&uart_task, uart_name_array[i], 128, 
        (void*)&uart_num_array[i], 10, &uart_task_array[i]);
    vTaskSuspend(uart_task_array[i]);
  }
}

bool set_uart_config(const int32_t num, const uart_config_t config) {
  if (num < 0 || num >= uart_cnt) {
    return false;
  }
  // Set enabled/disabled
  HardwareSerial* uart_inst{uart_inst_array[num]};
  if (config.enabled) {
    uart_inst->begin(config.baud_rate);
    vTaskResume(uart_task_array[num]);
  } else {
    uart_inst->end();
    vTaskSuspend(uart_task_array[num]);
  }
  return true;
}

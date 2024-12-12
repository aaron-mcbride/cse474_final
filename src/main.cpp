
#include <Arduino.h>
#include <FreeRTOSConfig.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include "lcd.hpp"
#include "adc.hpp"
#include "gpio.hpp"
#include "i2c.hpp"
#include "spi.hpp"
#include "uart.hpp"
#include "tc.hpp"

// ADC -> TESTED
// GPIO -> TESTED
// I2C -> NOT TESTED
// UART -> NOT TESTED
// SPI -> NOT TESTED
// TC -> NOT TESTED

// void setup() {
//   Serial.begin(9600);
//   while(!Serial);
//   Serial.println("--CONNECTED--");
//   delay(1000);
  
//   gpio_init();

//   adc_init();

//   lcd_init();

//   Serial.println("scheduler started");

//   vTaskStartScheduler();
// }


// I2C

// setup:
// init_i2c();
// set_i2c_config({true, 100000, 1});

// loop:
// const int32_t buff_size = i2c_output.buffer.size();
// for (int i = buff_size - 1; i >= 0; i--) {
//   Serial.print((char)i2c_output.buffer[i]);
// }
// Serial.println();
// delay(1000);


// SPI

// Setup:
// TaskHandle_t pt_handle = NULL;

// init_spi();
// set_spi_config({true, spi_order_t::msb_first});
// xTaskCreate(&print_task, "print_task", 128, NULL, 10, &pt_handle);
// vTaskStartScheduler();

// Task:
// void print_task(void*) {
//   while (1) {
//     xSemaphoreTake(spi_output.data_sem, portMAX_DELAY);
//     const int32_t buff_size = spi_output.buffer.size();
//     for (int i = buff_size - 1; i >= 0; i--) {
//       Serial.print((char)spi_output.buffer[i]);
//     }
//     xSemaphoreGive(spi_output.data_sem);
//     Serial.println("...");
//     vTaskDelay(5000 / portTICK_PERIOD_MS);
//   }
// }

void print_task(void*) {
  while (1) {
    xSemaphoreTake(uart_output[1].data_sem, portMAX_DELAY);
    const int32_t buff_size = uart_output[1].buffer.size();
    for (int i = buff_size - 1; i >= 0; i--) {
      Serial.print((int)uart_output[1].buffer[i]);
    }
    xSemaphoreGive(uart_output[1].data_sem);
    Serial.println("...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("--CONNECTED--");

  TaskHandle_t pt_handle = NULL;
  
  init_uart();
  set_uart_config(1, {true, 9600});
  xTaskCreate(&print_task, "print_task", 128, NULL, 10, &pt_handle);
  vTaskStartScheduler();
}

void loop() {}

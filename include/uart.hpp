
#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t uart_buffer_size{1024};
constexpr int32_t uart_cnt{3};

struct uart_output_t {
  buffer_t<uint8_t, uart_buffer_size> buffer{};
  SemaphoreHandle_t data_sem{nullptr};
};

struct uart_config_t {
  bool enabled;
  int32_t baud_rate;
};

extern uart_output_t uart_output[uart_cnt];

void init_uart();

bool set_uart_config(const int32_t num, const uart_config_t config);

#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t uart_buffer_size{1024};
constexpr int32_t uart_cnt{4};

enum class uart_parity_t {
  none,
  even,
  odd
};

enum class uart_mode_t {
  async,
  sync,
};

struct uart_output_t {
  buffer_t<uint8_t, uart_buffer_size> uart_data{};
  SemaphoreHandle_t data_sem{nullptr};
};

struct uart_config_t {
  bool enabled{false};
  int32_t baud_rate{9600};
  int32_t data_size{8};
  uart_mode_t mode{uart_mode_t::async};
  uart_parity_t parity{uart_parity_t::none};
};

uart_output_t uart_output[4]{};

void init_uart();

bool set_uart_config(const int32_t num, const uart_config_t config);

uart_config_t get_uart_config(const int32_t num);
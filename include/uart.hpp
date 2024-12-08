
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t uart_buffer_size{1024};

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
  SemaphoreHandle_t data_sem{NULL};
};

struct uart_config_t {
  bool enabled{false};
  int32_t baud_rate{9600};
  int32_t stop_bits{1};
  int32_t packet_size{8};
  uart_mode_t mode{uart_mode_t::async};
  uart_parity_t parity{uart_parity_t::none};
};

uart_output_t uart_output[4]{};

bool set_uart_config(const int32_t num, uart_config_t config);

uart_config_t get_uart_config(const int32_t num);
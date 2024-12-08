
#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t spi_buffer_size{1024};

enum class spi_order_t {
  msb_first,
  lsb_first
};

struct spi_output_t {
  buffer_t<uint8_t, spi_buffer_size> buffer{};
  SemaphoreHandle_t data_sem{nullptr};
};

struct spi_config_t {
  bool enabled{false};
  int32_t baud_rate{9600};
  spi_order_t data_order{spi_order_t::msb_first};
};

spi_output_t spi_output{};

bool set_spi_config(const spi_config_t config);

spi_config_t get_spi_config();
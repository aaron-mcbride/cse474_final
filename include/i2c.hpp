
#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t i2c_buffer_size{1024};

struct i2c_output_t {
  buffer_t<uint8_t, i2c_buffer_size> buffer{};
  SemaphoreHandle_t data_sem{nullptr};
};

struct i2c_config_t {
  bool enabled{false};
  int32_t baud_rate{9600};
  uint8_t bus_addr{0x00};  
};

i2c_output_t i2c_output[2]{};

bool set_i2c_config(const i2c_config_t config);

i2c_config_t get_i2c_config();
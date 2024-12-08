#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "buffer.hpp"

constexpr int32_t gpio_buffer_size{1024};
constexpr int32_t gpio_cnt{3};

struct gpio_output_t {
  buffer_t<uint8_t, gpio_buffer_size> gpio_data{};
  SemaphoreHandle_t data_sem{NULL};
};

typedef struct gpio_config {
  bool enabled{false}; 
  int32_t frequency;
};

gpio_output_t gpio_outputs[3];

void gpio_init();

bool set_gpio_config(const int32_t num, const gpio_config gc);

void gpio_task(void *pvParameters);

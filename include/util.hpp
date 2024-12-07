
#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

template<typename T>
T get_bit(const int32_t pos) {
  return ((T)1 << pos);
};

template<typename T>
struct semp_value_t {
  T value;
  SemaphoreHandle_t sem;
};
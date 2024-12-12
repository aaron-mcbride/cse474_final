#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "buffer.hpp"
#include <semphr.h>

constexpr uint32_t adc_buffer_size{1024};
constexpr uint32_t adc_cnt{3};

struct adc_output_t {
  uint32_t adc_data;
  // SemaphoreHandle_t data_sem{NULL};
};

extern adc_output_t adc_outputs[3];

extern SemaphoreHandle_t ADC_semaphore;

// bool set_adc_config(const int32_t num, const adc_config gc);

void adc_init(void);

void adc_task(void *pvParameters);

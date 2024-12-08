#pragma once
#include <Arduino.h>
#include "gpio.hpp"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

struct gpio_ref_t {
    uint8_t gpio_num;
    uint32_t pin_num;
    volatile uint8_t* port_reg;
    volatile uint8_t* ddr_reg;
    uint8_t portreg_bit;
};

static constexpr gpio_ref_t gpiopins[3] = {
    {uint8_t(0), uint32_t(23), &PORTA, &DDRA, uint8_t(1)},
    {uint8_t(1), uint32_t(25), &PORTA, &DDRA, uint8_t(3)},
    {uint8_t(2), uint32_t(27), &PORTA, &DDRA, uint8_t(5)}
};
int32_t gpio_freq[3] = {100, 100, 100};

void gpio_task(void *pvParameters){
    gpio_ref_t* curr = static_cast<gpio_ref_t*>(pvParameters);
    for(;;){
        xSemaphoreTake(gpio_outputs[curr->gpio_num].data_sem, 10);
        gpio_outputs[curr->gpio_num].gpio_data.push((*(curr->port_reg) & (1 << curr->portreg_bit)) ? true : false);
        xSemaphoreGive(gpio_outputs[curr->gpio_num].data_sem);
        vTaskDelay(100 / (gpio_freq[curr->gpio_num] * (portTICK_PERIOD_MS)));
    }    
}

static bool isValidPin(const int32_t num){
    return (num == 23 || num == 25 || num == 27);
}

void gpio_init() {
  for (int32_t i = 0; i < gpio_cnt; i++) {
    gpio_outputs[i].data_sem = xSemaphoreCreateBinary();
  }
}

// change freq of gpio pin
bool set_gpio_config(const int32_t num, const gpio_config gc){
    if(!isValidPin(num)){ return false; }

    uint32_t arr_idx = (num % 23) / 2;

    if(gc.enabled){
        // set pinmode to input
        *gpiopins[arr_idx].ddr_reg &= ~(bit(gpiopins[arr_idx].portreg_bit));
    } else {
        // set pinmode to output
       *gpiopins[arr_idx].ddr_reg |= bit(gpiopins[arr_idx].portreg_bit);
    }

    // set frequency
    gpio_freq[arr_idx] = gc.frequency;
}

// Get the current GPIO config
gpio_config get_gpio_config(const int32_t num){
  gpio_config config{};
  if(isValidPin(num)){  
    const int32_t arr_idx = (num % 23) / 2;
    config.enabled = *gpiopins[arr_idx].ddr_reg & bit(gpiopins[arr_idx].portreg_bit);
    config.frequency = gpio_freq[arr_idx];
  }
  return config;
}
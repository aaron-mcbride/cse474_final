#pragma once
#include <Arduino.h>
#include "gpio.hpp"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#define bit_(pos) ((uint8_t)1 << (pos))

typedef struct gpio_ref_t {
    uint8_t gpio_num;
    uint32_t pin_num;
    volatile uint8_t* port_reg;
    volatile uint8_t* ddr_reg;
    uint8_t portreg_bit;
    uint32_t freq;
} gpio_ref_t;

gpio_ref_t gpiopins[3] = {
    {uint8_t(0), uint32_t(23), &PORTA, &DDRA, uint8_t(1), uint32_t(100)},
    {uint8_t(1), uint32_t(25), &PORTA, &DDRA, uint8_t(3), uint32_t(100)},
    {uint8_t(2), uint32_t(27), &PORTA, &DDRA, uint8_t(5), uint32_t(100)}
};

void gpio_task(void *pvParameters){
    gpio_ref_t* curr = static_cast<gpio_ref_t*>(pvParameters);
    for(;;){
        xSemaphoreTake(gpio_outputs[curr->gpio_num].data_sem, 10);
        gpio_outputs[curr->gpio_num].gpio_data.push((*(curr->port_reg) & (1 << curr->portreg_bit)) ? true : false);
        xSemaphoreGive(gpio_outputs[curr->gpio_num].data_sem);
        vTaskDelay(100 / (curr->freq * (portTICK_PERIOD_MS)));
    }    
}

bool isValidPin(const int32_t num){
    return (num == 23 || num == 25 || num == 27);
}

// change freq of gpio pin
bool set_gpio_config(const int32_t num, const gpio_config gc){
    if(!isValidPin(num)){ return false; }

    uint32_t arr_idx = (num % 23) / 2;

    if(gc.enabled){
        // set pinmode to input
        *gpiopins[arr_idx].ddr_reg &= ~(bit_(gpiopins[arr_idx].portreg_bit));
    } else {
        // set pinmode to output
       *gpiopins[arr_idx].ddr_reg |= bit_(gpiopins[arr_idx].portreg_bit);
    }

    // set frequency
    gpiopins[arr_idx].freq = gc.frequency;
}
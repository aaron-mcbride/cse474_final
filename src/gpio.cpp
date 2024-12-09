
#include "gpio.hpp"

struct gpio_ref_t {
    uint8_t gpio_num;
    uint32_t pin_num;
    volatile uint8_t* port_reg;
    volatile uint8_t* ddr_reg;
    uint8_t portreg_bit;
};

gpio_ref_t gpiopins[3]{
    {uint8_t(0), uint32_t(23), &PORTA, &DDRA, uint8_t(1)},
    {uint8_t(1), uint32_t(25), &PORTA, &DDRA, uint8_t(3)},
    {uint8_t(2), uint32_t(27), &PORTA, &DDRA, uint8_t(5)}
};

int32_t gpio_freq[3]{0, 0, 0};

TaskHandle_t gpio_tasks[3]{nullptr, nullptr, nullptr};

static void gpio_task(void *pvParameters){
    gpio_ref_t* curr = static_cast<gpio_ref_t*>(pvParameters);
    for(;;){
        xSemaphoreTake(gpio_outputs[curr->gpio_num].data_sem, 10);
        gpio_outputs[curr->gpio_num].gpio_data.push((*(curr->port_reg) & (1 << curr->portreg_bit)) ? true : false);
        xSemaphoreGive(gpio_outputs[curr->gpio_num].data_sem);
        vTaskDelay(100 / (gpio_freq[curr->gpio_num] * (portTICK_PERIOD_MS)));
    }    
}

static bool isValidPin(const int32_t num){
    return (num < 3 && num >= 0);
}

void gpio_init(void) {
    for(int i = 0; i < 3; i++){
        xTaskCreate(
            gpio_task,
            "gpio_task",
            128,
            &gpiopins[i],
            2,
            &gpio_tasks[i]
        );
        gpio_outputs[i].data_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(gpio_outputs[i].data_sem);
    }
}

// change freq of gpio pin
bool set_gpio_config(const int32_t num, const gpio_config gc){
    if(!isValidPin(num)){ return false; }

    if(gc.enabled){
        // set pinmode to input
        *gpiopins[num].ddr_reg &= ~(bit(gpiopins[num].portreg_bit));
        vTaskResume(gpio_tasks[num]);
    } else {
        // set pinmode to output
        vTaskSuspend(gpio_tasks[num]);
       *gpiopins[num].ddr_reg |= bit(gpiopins[num].portreg_bit);
    }

    // set frequency
    gpio_freq[num] = gc.frequency;
    return true;
}

// Get the current GPIO config
gpio_config get_gpio_config(const int32_t num){
  gpio_config config{};
  if(isValidPin(num)){  
    config.enabled = *gpiopins[num].ddr_reg & bit(gpiopins[num].portreg_bit);
    config.frequency = gpio_freq[num];
  }
  return config;
}
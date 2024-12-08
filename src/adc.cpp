#include <Arduino.h>
#include "adc.hpp"
#include <semphr.h>

typedef struct adc_ref_t {
    uint8_t gpio_num;
    uint8_t pin_num;
    uint8_t adc_channel_bit;
} adc_ref_t;

struct adc_config {
    uint16_t adc_gain;
    uint16_t adc_prescaler;
};

adc_ref_t adcpins[3] = {
    {uint8_t(0), A0, PC0},
    {uint8_t(1), A1, PC1},
    {uint8_t(2), A2, PC2}
};

TaskHandle_t adc_tasks[3]{nullptr, nullptr, nullptr};

static bool isValidPin(const int32_t num){
    return (num < 3 && num >= 0);
}

static bool set_adc_config(const int32_t num, const adc_config gc) {
    if(!isValidPin(num)){ return false; }
    return true;
}

void adc_init(void){
    ADMUX = (1 << REFS0); // Set reference as AV_CC
    ADCSRA &= ~bit(ADLAR); // set ADC reg for result to 8 LSB in ADCL, 2 MSB im ADCH
    ADCSRA |= bit(ADIE); // Enable ADC interrupt

    for(int i = 0; i < 3; i++){
        xTaskCreate(
            adc_task,
            "adc_task",
            128,
            &adcpins[i],
            2,
            &adc_tasks[i]
        );
        adc_outputs[i].data_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(adc_outputs[i].data_sem);
    }
    ADCSRA |= bit(ADEN); // Enable ADC
}

// disable adc pin
bool adc_disable(const int32_t num){
    if(!isValidPin(num)){ return false; }
    vTaskSuspend(adc_tasks[num]);
    ADMUX &= ~bit(num);
    return true;
}

// enable adc pin
bool adc_enable(const int32_t num){
    if(!isValidPin(num)){ return false; }
    vTaskResume(adc_tasks[num]);
    ADMUX |= bit(num);
    return true;
}

// adc run task
void adc_task(void *pvParameters){
    adc_ref_t* curr = static_cast<adc_ref_t*>(pvParameters);
    for(;;){
        xSemaphoreTake(ADC_semaphore, 10);
        xSemaphoreTake(adc_outputs[curr->gpio_num].data_sem, 10);
        ADMUX &= 0xF0; // Clear bottom 4 bits
        ADMUX |= curr->gpio_num;

        while (ADCSRA & (1 << ADSC)); // wait for ADC reading availability

        // read from adc
        uint16_t ADC_code = 0;
        ADC_code = ADCL | ((uint16_t)ADCH << 8);

        adc_outputs[curr->gpio_num].adc_data = ADC_code;

        xSemaphoreGive(adc_outputs[curr->gpio_num].data_sem);
        xSemaphoreGive(ADC_semaphore);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// enable adc pin
bool get_adc_pin_enabled(const int32_t num){
    if(!isValidPin(num)){ return false; }
    return (eTaskGetState(adc_tasks[num]) == eRunning);
}
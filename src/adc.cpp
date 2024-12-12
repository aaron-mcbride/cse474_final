
#include "adc.hpp"
#include <avr/wdt.h>


typedef struct adc_ref_t {
    uint8_t gpio_num;
    uint8_t pin_num;
    uint8_t adc_channel_bit;
} adc_ref_t;

typedef struct adc_config {
    uint16_t adc_prescaler;
} adc_config;


adc_ref_t adcpins[3] = {
    {uint8_t(0), A0, 0},
    {uint8_t(1), A1, 1},
    {uint8_t(2), A2, 2}
};

TaskHandle_t adc_tasks[3]{nullptr, nullptr, nullptr};
adc_output_t adc_outputs[3] = {};
SemaphoreHandle_t ADC_semaphore = NULL;


static bool isValidPin(const int32_t num){
    return (num < 3 && num >= 0);
}

// static bool set_adc_config(const adc_config ac) {

//     // set prescaler
//     uint16_t scale = ac.adc_prescaler;
//     ADCSRA &= ~(0x07);  // clear prescaler
//     // set division factor in 3 LSB of ADCSRA reg
//     if(scale <= 2){
//         ADCSRA |= 0x01;
//     } else if(scale <= 4){
//         ADCSRA |= 0x02;
//     } else if(scale <= 8){
//         ADCSRA |= 0x03;
//     } else if(scale <= 16){
//         ADCSRA |= 0x04;
//     } else if(scale <= 32){
//         ADCSRA |= 0x05;
//     } else if(scale <= 64){
//         ADCSRA |= 0x06;
//     } else if(scale <= 128){
//         ADCSRA |= 0x07;
//     }

//     return true;
// }

void adc_init(void){
    ADMUX = (1 << REFS0); // Set reference as AV_CC
    ADCSRA &= ~bit(ADLAR); // set ADC reg for result to 8 LSB in ADCL, 2 MSB im ADCH
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // set prescaler to 128

    wdt_disable();
    ADC_semaphore = xSemaphoreCreateMutex();

    for(int i = 0; i < 3; i++){
        xTaskCreate(
            adc_task,
            "adc_task",
            256,
            &adcpins[i],
            2,
            &adc_tasks[i]
        );
    }
    xSemaphoreGive(ADC_semaphore);
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
    int i = 0;
    adc_ref_t* curr = static_cast<adc_ref_t*>(pvParameters);
    uint32_t ADC_code;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    
    for(;;){

        xSemaphoreTake(ADC_semaphore, 100 / portTICK_PERIOD_MS);
        vTaskDelay(15 / portTICK_PERIOD_MS);

        
        ADMUX = (ADMUX & 0xE0) | (curr->adc_channel_bit);
	    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((curr->adc_channel_bit >> 3) & 0x01) << MUX5);

        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC)); 

        // // read from adc
        ADC_code = ADCL | ((uint16_t)ADCH << 8);

        // ADC_code = ADC;
        adc_outputs[curr->gpio_num].adc_data = ADC_code;

        i++;
        xSemaphoreGive(ADC_semaphore);
    }
}

// enable adc pin
bool get_adc_pin_enabled(const int32_t num){
    if(!isValidPin(num)){ return false; }
    return (eTaskGetState(adc_tasks[num]) == eRunning);
}
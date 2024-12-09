
#include "lcd.hpp"

LiquidCrystal lcd(7, 8, 9, 10, 11, 12, 13);

// indecies/state: 
// 0: I2C
// 1: SPI
// 2: UART
// 3: GPIO
// 4: ADC
static void lcd_init(void){

    xTaskCreate(
        lcd_change_state_task,
        "lcd_change_state_task",
        128,
        nullptr,
        2,
        nullptr
    );

    xTaskCreate(
        lcd_task,
        "lcd_task",
        128,
        nullptr,
        2,
        nullptr
    );
}

static void lcd_task(void *pvParameters) {
    lcd.begin(16, 2);
    String s, tmp;
    char buf[4];

    for(;;){
        switch(lcd_state){
            case(0): // I2C
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("I2C:");
                // cursor to bottom row + write
                lcd.setCursor(0, 1);
                xSemaphoreTake(i2c_output.data_sem, 10);
                lcd.print("Data: " + i2c_output.buffer[i2c_output.buffer.size()]);
                xSemaphoreGive(i2c_output.data_sem);

            break;
            case(1): // SPI
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("SPI:");
                // cursor to bottom row + write
                lcd.setCursor(0, 1);
                xSemaphoreTake(spi_output.data_sem, 10);
                lcd.print("Data: " + spi_output.buffer[spi_output.buffer.size()]);
                xSemaphoreGive(spi_output.data_sem);
            break;
            case(2): // UART 
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("UART:");
                lcd.setCursor(0, 1);
                // cursor to bottom row + write
                //  TODO: figure out what to do for multiple UART inputs
                xSemaphoreTake(uart_output[0].data_sem, 10);
                lcd.print("Data: " + uart_output[0].uart_data[uart_output[0].uart_data.size()]);
                xSemaphoreTake(uart_output[0].data_sem, 10);
            break;
            case(3): // GPIO
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("GPIO:");
                lcd.setCursor(0, 1);
                // cursor to bottom row + write
                xSemaphoreTake(gpio_outputs[0].data_sem, 10);
                s += "1: " + gpio_outputs[0].gpio_data[gpio_outputs[0].gpio_data.size()];
                xSemaphoreGive(gpio_outputs[0].data_sem);

                xSemaphoreTake(gpio_outputs[1].data_sem, 10);
                s += " 2: " + gpio_outputs[1].gpio_data[gpio_outputs[1].gpio_data.size()];
                xSemaphoreGive(gpio_outputs[1].data_sem);

                xSemaphoreTake(gpio_outputs[2].data_sem, 10);
                s +=  " 3: " + gpio_outputs[2].gpio_data[gpio_outputs[2].gpio_data.size()];
                xSemaphoreGive(gpio_outputs[2].data_sem);
                lcd.print(s);
                s = "";
            break;
            case(4): // ADC
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("ADC:");
                lcd.setCursor(0, 1);
                // cursor to bottom row + write
                xSemaphoreTake(adc_outputs[0].data_sem, 10);
                sprintf(buf, "%.2f", (adc_outputs[0].adc_data / (5 * 256.0)));
                xSemaphoreGive(adc_outputs[0].data_sem);
                tmp = buf;
                s += "1: " + (tmp);
                xSemaphoreTake(adc_outputs[1].data_sem, 10);
                sprintf(buf, "%.2f", (adc_outputs[1].adc_data / (5 * 256.0)));
                xSemaphoreGive(adc_outputs[1].data_sem);
                tmp = buf;
                s += " 2: " + (tmp);
                xSemaphoreTake(adc_outputs[2].data_sem, 10);
                sprintf(buf, "%.2f", (adc_outputs[2].adc_data / (5 * 256.0)));
                xSemaphoreGive(adc_outputs[2].data_sem);
                tmp = buf;
                s +=  " 3: " + (tmp);
                lcd.print(s);
                s = "";
            break;
        }
    }
}

static void lcd_change_state_task(void *pvParameters) {
    uint8_t joystick_x, joystick_y;
    for(;;){
        // read from ADCS
        xSemaphoreTake(adc_outputs[0].data_sem, 10);
        joystick_x = adc_outputs[0].adc_data;
        xSemaphoreGive(adc_outputs[0].data_sem);
        xSemaphoreTake(adc_outputs[1].data_sem, 10);
        joystick_y = adc_outputs[1].adc_data;
        xSemaphoreGive(adc_outputs[1].data_sem);
        if(joystick_x > 64){
            lcd_state++;
            vTaskDelay(1000 / portTICK_PERIOD_MS); // delay between changing
        } else if(joystick_x < 192){
            lcd_state--;
            vTaskDelay(1000 / portTICK_PERIOD_MS); // delay between changing  
        }
    }
}
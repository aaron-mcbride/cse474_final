
#include "lcd.hpp"

LiquidCrystal lcd(7, 8, 9, 10, 11, 12, 13);
static int8_t lcd_state = 0;

// indecies/state: 
// 0: I2C
// 1: SPI
// 2: UART
// 3: GPIO
// 4: ADC
void lcd_init(){

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
    String s1, s2, prevs;
    int adc_decimal;

    for(;;){
        switch(lcd_state){
            case(0): // I2C
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("I2C:");
                // cursor to bottom row + write
                // lcd.setCursor(0, 1);
                // xSemaphoreTake(i2c_output.data_sem, 10);
                // lcd.print("Data: " + i2c_output.buffer[i2c_output.buffer.size()]);
                // xSemaphoreGive(i2c_output.data_sem);

            break;
            case(1): // SPI
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("SPI:");
                // cursor to bottom row + write
                lcd.setCursor(0, 1);
                // xSemaphoreTake(spi_output.data_sem, 10);
                // lcd.print("Data: " + spi_output.buffer[spi_output.buffer.size()]);
                // xSemaphoreGive(spi_output.data_sem);
            break;
            case(2): // UART0
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("UART0:");
                lcd.setCursor(0, 1);
                // cursor to bottom row + write
                // xSemaphoreTake(uart_output[0].data_sem, 10);
                // lcd.print("Data: " + uart_output[0].uart_data[uart_output[0].uart_data.size()]);
                // xSemaphoreTake(uart_output[0].data_sem, 10);
            break;
                case(3): // UART1 
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("UART1:");
                // lcd.setCursor(0, 1);
                // // cursor to bottom row + write
                // xSemaphoreTake(uart_output[0].data_sem, 10);
                // lcd.print("Data: " + uart_output[0].uart_data[uart_output[0].uart_data.size()]);
                // xSemaphoreTake(uart_output[0].data_sem, 10);
            break;
                case(4): // UART2 
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("UART2:");
                // lcd.setCursor(0, 1);
                // // cursor to bottom row + write
                // xSemaphoreTake(uart_output[0].data_sem, 10);
                // lcd.print("Data: " + uart_output[0].uart_data[uart_output[0].uart_data.size()]);
                // xSemaphoreTake(uart_output[0].data_sem, 10);
            break;
                case(5): // UART3 
                lcd.clear();
                // cursor to top row + write
                lcd.setCursor(0, 0);
                lcd.print("UART3:");
                // lcd.setCursor(0, 1);
                // // cursor to bottom row + write
                // xSemaphoreTake(uart_output[0].data_sem, 10);
                // lcd.print("Data: " + uart_output[0].uart_data[uart_output[0].uart_data.size()]);
                // xSemaphoreTake(uart_output[0].data_sem, 10);
            break;
            case(6): // GPIO
                s2 = "";
                // // cursor to bottom row + write
                // xSemaphoreTake(gpio_outputs[0].data_sem, 10);
                s2 += "1: ";
                s2 += gpio_outputs[0].gpio_data[gpio_outputs[0].gpio_data.size()] == false ? "0" : "1";
                // xSemaphoreGive(gpio_outputs[0].data_sem);

                // xSemaphoreTake(gpio_outputs[1].data_sem, 10);
                s2 += " 2: ";
                s2 += gpio_outputs[1].gpio_data[gpio_outputs[1].gpio_data.size()] == false ? "0" : "1";
                // xSemaphoreGive(gpio_outputs[1].data_sem);


                // xSemaphoreTake(gpio_outputs[2].data_sem, 10);
                s2 +=  " 3: "; 
                s2 += gpio_outputs[2].gpio_data[gpio_outputs[2].gpio_data.size()] == false ? "0" : "1";
                // xSemaphoreGive(gpio_outputs[2].data_sem);
                if(prevs != s2){
                    lcd.clear();
                    // cursor to top row + write
                    lcd.setCursor(0, 0);
                    lcd.print("GPIO:");
                    lcd.setCursor(0, 1);
                    lcd.print(s2);
                }
                prevs = s2;
            break;
            case(7): // ADC
                s1 = "ADC: ";
                s2 = "";
                s1 += " 1: ";
                s1 += myitoa(adc_outputs[0].adc_data * 5  / 1024);
                s1 += ".";
                adc_decimal = ((adc_outputs[0].adc_data * 5 * 100)  / 1024) % 100;
                s1 += adc_decimal;
                s2 += "2: ";
                s2 += myitoa(adc_outputs[1].adc_data * 5  / 1024);
                s2 += ".";
                adc_decimal = ((adc_outputs[1].adc_data * 5 * 100)  / 1024) % 100;
                s2 += adc_decimal;
                s2 += " 3: ";
                s2 += myitoa(adc_outputs[2].adc_data * 5  / 1024);
                s2 += ".";
                adc_decimal = ((adc_outputs[2].adc_data * 5 * 100)  / 1024) % 100;
                s2 += myitoa(adc_decimal);
                if(prevs != (s1+s2)){
                    lcd.clear();
                    // cursor to top row + write
                    lcd.setCursor(0, 0);
                    lcd.print(s1);
                    lcd.setCursor(0, 1);
                    lcd.print(s2);
                }
                prevs = s1 + s2;
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // delay between changing
    }
}

static void lcd_change_state_task(void *pvParameters) {
    uint32_t joystick_x;
    for(;;){
        // read from ADCS
        xSemaphoreTake(ADC_semaphore, 10);
        joystick_x = adc_outputs[0].adc_data;
        // joystick_y = adc_outputs[1].adc_data;
        xSemaphoreGive(ADC_semaphore);
        if(joystick_x > 768){
            if(lcd_state == 7){
                lcd_state = 0;
            } else {
                lcd_state++;
            }
            vTaskDelay(200 / portTICK_PERIOD_MS); // delay between changing
        } else if(joystick_x < 256){
            if(lcd_state == 0){
                lcd_state = 7;
            } else {
                lcd_state--;
            }
            vTaskDelay(200 / portTICK_PERIOD_MS); // delay between changing  
        }
    }
}

static String myitoa(int input) {
    String s = "";
    if(input == 0){ return "0";}
    while(input != 0){
        int rightmost = input % 10;
        if(rightmost == 0){
            s = '0' + s;
        } else if(rightmost == 1){
            s = '1' + s;
        } else if(rightmost == 2){
            s = '2' + s;
        } else if(rightmost == 3){
            s = '3' + s;
        } else if(rightmost == 4){
            s = '4' + s;
        } else if(rightmost == 5){
            s = '5' + s;
        } else if(rightmost == 6){
            s = '6' + s;
        } else if(rightmost == 7){
            s = '7' + s;
        } else if(rightmost == 8){
            s = '8' + s;
        } else if(rightmost == 9){
            s = '9' + s;
        }
        input /= 10;
    }
    return s;
}
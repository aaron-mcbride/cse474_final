
#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <LiquidCrystal.h>
#include <string.h>
#include <semphr.h>
#include "adc.hpp"
#include "gpio.hpp"
#include "i2c.hpp"
#include "spi.hpp"
#include "uart.hpp"
#include "tc.hpp"

// indecies/state: 
// 0: I2C
// 1: SPI
// 2: UART0
// 3: UART1
// 4: UART2
// 5: UART3
// 6: GPIO
// 7: ADC

void lcd_init();

void lcd_task(void *pvParameters);

void lcd_change_state_task(void *pvParameters);

static String myitoa(int input);


#pragma once
#include <Arduino.h>
#include "adc.hpp"
#include "spi.hpp"
#include "uart.hpp"
#include "gpio.hpp"
#include "i2c.hpp"
#include <LiquidCrystal.h>
#include <string.h>
#include <semphr.h>

static void lcd_init(void);

static void lcd_task(void *pvParameters);

static void lcd_change_state_task(void *pvParameters);

// indecies/state: 
// 0: I2C
// 1: SPI
// 2: UART
// 3: GPIO
// 4: ADC
static int8_t lcd_state = 0;

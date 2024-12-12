
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>

#include <FreeRTOSConfig.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include "adc.hpp"
#include "gpio.hpp"
#include "lcd.hpp"

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("--CONNECTED--");
  delay(1000);
  
  gpio_init();

  adc_init();

  lcd_init();

  Serial.println("scheduler started");

  vTaskStartScheduler();
}

void loop() {
  
}


#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>

#include <FreeRTOSConfig.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("--CONNECTED--");
}

void loop() {

}

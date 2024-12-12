
#include "i2c.hpp"
#include "utility/twi.h"
#include <Wire.h>
#include <string.h>

i2c_output_t i2c_output{};

void init_i2c() {
  i2c_output.data_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(i2c_output.data_sem);
}

static void i2c_isr(int num_bytes) {
  xSemaphoreTakeFromISR(i2c_output.data_sem, nullptr);
  for (int i = 0; i < num_bytes; i++) {
    i2c_output.buffer.push(Wire.read());
  }
  xSemaphoreGiveFromISR(i2c_output.data_sem, nullptr);
}

bool set_i2c_config(const i2c_config_t config) {
  if (config.enabled) {
    Wire.begin(config.bus_addr);
    Wire.setClock(config.baud_rate);
    Wire.onReceive(&i2c_isr);
  } else {
    Wire.end();
  }
  return true;
}
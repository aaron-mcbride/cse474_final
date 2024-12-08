
#include "i2c.hpp"

struct baud_ref_t {
  int32_t baud_rate;
  bool twps0;
  bool twps1;
  int32_t twbr;
};

static constexpr uint8_t twa_mask{0b11111110};
static constexpr int32_t baud_rate_cnt{9};
static constexpr baud_ref_t baud_ref[9]{
  {4800, 0, 1, 7}, 
  {9600, 0, 0, 207}, 
  {19200, 0, 0, 103}, 
  {38400, 0, 0, 51}, 
  {57600, 0, 0, 34}, 
  {115200, 0, 0, 16}, 
  {230400, 0, 0, 8}, 
  {460800, 0, 0, 3}, 
  {921600, 0, 0, 1}
};

// Initialize I2C peripheral
void init_i2c() {
  i2c_output.data_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(i2c_output.data_sem);
}

// Set current i2c peripheral configuration
bool set_i2c_config(const i2c_config_t config) {

  // Disable peripheral
  TWCR &= ~bit(TWEN);
  TWCR &= ~bit(TWIE);

  // Set baud rate
  for (int32_t i = 0; i < baud_rate_cnt; i++) {
    if (config.baud_rate == baud_ref[i].baud_rate) {
      TWBR = baud_ref[i].twbr;
      if (baud_ref[i].twps0) {
        TWSR |= bit(TWPS0);
      } else {
        TWSR &= ~bit(TWPS0);
      }
      if (baud_ref[i].twps1) {
        TWSR |= bit(TWPS1);
      } else {
        TWSR &= ~bit(TWPS1);
      }
    } else if (i >= baud_rate_cnt - 1) {
      return false;
    }
  }
  // Set address
  if (config.bus_addr & ~twa_mask) { return false; }
  TWAR = (TWAR & ~twa_mask) | (config.bus_addr << 1);

  // Set enabled
  if (config.enabled) {
    TWCR |= bit(TWEN);
    TWCR |= bit(TWIE);
    TWAR |= bit(TWGCE);
    TWCR |= bit(TWEA);
    TWCR &= ~bit(TWSTA);
    TWCR &= ~bit(TWSTO);
  } else {
    TWCR &= ~bit(TWEN);
    TWCR &= ~bit(TWIE);
  }
  return true;
}

// Get current I2C peripheral configuration
i2c_config_t get_i2c_config() {
  i2c_config_t config{};

  // Get enabled
  config.enabled = TWCR & bit(TWEN);

  // Get baud rate
  for (int32_t i = 0; i < baud_rate_cnt; i++) {
    if (TWBR == baud_ref[i].twbr &&
        ((TWSR & bit(TWPS0)) == baud_ref[i].twps0) &&
        ((TWSR & bit(TWPS1)) == baud_ref[i].twps1)) {
      config.baud_rate = baud_ref[i].baud_rate;
      break;
    }
  }
  // Get address
  config.bus_addr = (TWAR & twa_mask) >> 1;
  return config;
}

// I2C interrupt for reading data
ISR(TWI_vect) {
  xSemaphoreTakeFromISR(i2c_output.data_sem, nullptr);
  const uint8_t output_value = TWDR;
  i2c_output.buffer.push(output_value);
  xSemaphoreGiveFromISR(i2c_output.data_sem, nullptr);
}
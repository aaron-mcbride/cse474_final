
#include "spi.hpp"

struct baud_ref_t {
  int32_t baud_rate;
  int32_t spr0_value;
  int32_t spr1_value;
  int32_t spr2_value;
};

static constexpr int32_t baud_rate_cnt = 8;
static constexpr baud_ref_t baud_ref[8] = {
  {2000000, 0, 0, 0},
  {1000000, 0, 0, 1},
  {500000, 0, 1, 0},
  {250000, 0, 1, 1},
  {125000, 1, 0, 0},
  {62500, 1, 0, 1},
  {31250, 1, 1, 0},
  {15625, 1, 1, 1}
};

// Update the spi peripheral's configuration
bool set_spi_config(const spi_config_t config) {

  // disable spi peripheral
  SPCR &= ~bit(SPE);
  SPCR &= ~bit(SPIE);

  // baud rate
  for (int32_t i = 0; i < baud_rate_cnt; i++) {
    if (config.baud_rate == baud_ref[i].baud_rate) {
      if (baud_ref[i].spr0_value) {
        SPSR |= bit(SPI2X);
      } else {
        SPSR &= ~bit(SPI2X);
      }
      if (baud_ref[i].spr1_value) {
        SPCR |= bit(SPR0);
      } else {
        SPCR &= ~bit(SPR0);
      }
      if (baud_ref[i].spr2_value) {
        SPCR |= bit(SPR1);
      } else {
        SPCR &= ~bit(SPR1);
      }
      break;
    } else if (i == baud_rate_cnt - 1) {
      return false;
    }
  }
  // set data order
  if (config.data_order == spi_order_t::msb_first) {
    SPCR &= ~bit(DORD);
  } else {
    SPCR |= bit(DORD);
  }
  // Set spi enabled
  if (config.enabled) {
    DDRB |= bit(PB1);
    DDRB |= bit(PB2);
    DDRB |= bit(PB3);
    SPCR |= bit(SPE);
    SPCR |= bit(SPIE);
    SPCR &= ~bit(MSTR);
    SPCR &= ~bit(CPOL);
    SPCR &= ~bit(CPHA);
  } else {
    DDRB &= ~bit(PB1);
    DDRB &= ~bit(PB2);
    DDRB &= ~bit(PB3);
    SPCR &= ~bit(SPE);
    SPCR &= ~bit(SPIE);
  }
  return true;
}

// Get the spi peripheral's current config
spi_config_t get_spi_config() {
  spi_config_t config{};
  
  // Get enabled
  if (SPCR & bit(SPE)) {
    config.enabled = true;
  } else {
    config.enabled = false;
  }
  // Get baud rate
  for (int32_t i = 0; i < baud_rate_cnt; i++) {
    if (baud_ref[i].spr0_value == (SPSR & bit(SPR0)) &&
        baud_ref[i].spr1_value == (SPCR & bit(SPR1)) &&
        baud_ref[i].spr2_value == (SPCR & bit(SPI2X))) {
      config.baud_rate = baud_ref[i].baud_rate;
      break;
    }
  }
  // Get data order
  if (SPCR & bit(DORD)) {
    config.data_order = spi_order_t::lsb_first;
  } else {
    config.data_order = spi_order_t::msb_first;
  }
  return config;
}

// SPI interrupts
ISR(SPI_STC_vect) {
  xSemaphoreTakeFromISR(spi_output.data_sem, nullptr);
  const uint8_t data = SPDR;
  spi_output.buffer.push(data);
  xSemaphoreGiveFromISR(spi_output.data_sem, nullptr);
}

#include "spi.hpp"
#include <SPI.h>

spi_output_t spi_output{};

constexpr int32_t default_spi_mode = SPI_MODE0;

void init_spi() {
  spi_output.data_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(spi_output.data_sem);
}

void set_spi_config(const spi_config_t config) {
  if (config.enabled) {
    pinMode(MISO, OUTPUT);
    SPCR |= bit(SPE);
    SPCR |= _BV(SPIE);
    if (config.data_order == spi_order_t::lsb_first) {
      SPCR |= bit(DORD);
    } else {
      SPCR &= ~bit(DORD);
    }
  } else {
    SPCR &= ~bit(SPE);
  }
}

ISR(SPI_STC_vect) {
  xSemaphoreTakeFromISR(spi_output.data_sem, nullptr);
  const uint8_t spi_data = SPDR;
  spi_output.buffer.push(spi_data);
  xSemaphoreGiveFromISR(spi_output.data_sem, nullptr);
}
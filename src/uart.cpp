
#include "uart.hpp"

struct uart_ref_t {
  volatile uint8_t* ctrl_reg_a;
  volatile uint8_t* ctrl_reg_b;
  volatile uint8_t* ctrl_reg_c;
  volatile uint8_t* rx_reg;
  volatile uint16_t* baud_reg; 
};

struct baud_ref_t {
  int32_t baud_rate;
  int32_t ubrr_value;
};

static constexpr uart_ref_t uart_ref[4] = {
  {&UCSR0A, &UCSR0B, &UCSR0C, &UDR0, &UBRR0},
  {&UCSR1A, &UCSR1B, &UCSR1C, &UDR1, &UBRR1},
  {&UCSR2A, &UCSR2B, &UCSR2C, &UDR2, &UBRR2},
  {&UCSR3A, &UCSR3B, &UCSR3C, &UDR3, &UBRR3},
};

static constexpr int32_t baud_rate_cnt{10};
static constexpr baud_ref_t baud_ref[10] = {
  {2400, 416},
  {4800, 207},
  {9600, 103},
  {14400, 68},
  {19200, 51},
  {28800, 34},
  {38400, 25},
  {57600, 16},
  {76800, 12},
  {115200, 8},
};

// Check if a uart number is valid
static bool valid_uart(const int32_t num) {
  return num >= 0 && num < uart_cnt;
}

// Init uart peripherals/freeRTOS stuff
void init_uart() {
  for (int32_t i = 0; i < uart_cnt; i++) {
    uart_output[i].data_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(uart_output[i].data_sem);
  }
}

// update uart peripheral configuration
bool set_uart_config(const int32_t num, const uart_config_t config) {
  if (!valid_uart(num)) { return false; }
  auto &ref{uart_ref[num]};

  // Disable uart peripheral/interrupt
  *ref.ctrl_reg_b &= ~bit(RXEN0);
  *ref.ctrl_reg_b &= ~bit(RXCIE0);

  // Set baud rate
  for (int32_t i = 0; i < baud_rate_cnt; i++) {
    if (baud_ref[i].baud_rate == config.baud_rate) {
      *ref.baud_reg = baud_ref[i].ubrr_value;
      break;
    } else if (i == baud_rate_cnt - 1) {
      return false;
    }
  }
  // Set packet size
  if (config.data_size == 5) {
    *ref.ctrl_reg_c &= ~bit(UCSZ00);
    *ref.ctrl_reg_c &= ~bit(UCSZ01);
    *ref.ctrl_reg_c &= ~bit(UCSZ02);
  } else if (config.data_size == 6) {
    *ref.ctrl_reg_c |= bit(UCSZ00);
    *ref.ctrl_reg_c &= ~bit(UCSZ01);
    *ref.ctrl_reg_c &= ~bit(UCSZ02);
  } else if (config.data_size == 7) {
    *ref.ctrl_reg_c &= ~bit(UCSZ00);
    *ref.ctrl_reg_c |= bit(UCSZ01);
    *ref.ctrl_reg_c &= ~bit(UCSZ02);
  } else if (config.data_size == 8) {
    *ref.ctrl_reg_c |= bit(UCSZ00);
    *ref.ctrl_reg_c |= bit(UCSZ01);
    *ref.ctrl_reg_c &= ~bit(UCSZ02);
  } else {
    return false;
  }
  // Set mode
  if (config.mode == uart_mode_t::async) {
    *ref.ctrl_reg_c &= ~bit(UMSEL00);
    *ref.ctrl_reg_c &= ~bit(UMSEL01);
  } else if (config.mode == uart_mode_t::sync) {
    *ref.ctrl_reg_c |= bit(UMSEL00);
    *ref.ctrl_reg_c &= ~bit(UMSEL01);
  } else {
    return false;
  }
  // Set parity
  if (config.parity == uart_parity_t::none) {
    *ref.ctrl_reg_c &= ~bit(UPM00);
    *ref.ctrl_reg_c &= ~bit(UPM01);
  } else if (config.parity == uart_parity_t::even) {
    *ref.ctrl_reg_c |= bit(UPM00);
    *ref.ctrl_reg_c &= ~bit(UPM01);
  } else if (config.parity == uart_parity_t::odd) {
    *ref.ctrl_reg_c |= bit(UPM00);
    *ref.ctrl_reg_c |= bit(UPM01);
  } else {
    return false;
  }
  // set enabled
  if (config.enabled) {    
    *ref.ctrl_reg_b |= bit(RXEN0);
    *ref.ctrl_reg_b |= bit(RXCIE0);
    *ref.ctrl_reg_b &= ~bit(TXEN0);
    *ref.ctrl_reg_c &= ~bit(USBS0);
    *ref.ctrl_reg_c &= ~bit(UCPOL0);
  } else {
    *ref.ctrl_reg_b &= ~bit(RXEN0);
    *ref.ctrl_reg_b &= ~bit(RXCIE0);
  }
  return true;
}

// Get a uart peripheral's configuration
uart_config_t get_uart_config(const int32_t num) {
  uart_config_t config{};
  if (valid_uart(num)) { 
    auto& ref{uart_ref[num]};

    // Get enabled
    if (*ref.ctrl_reg_b & bit(RXEN0)) {
      config.enabled = true;
    } else {
      config.enabled = false;
    }
    // Get baud rate
    const uint32_t baud_value = *ref.baud_reg;
    for (int32_t i = 0; i < baud_rate_cnt; i++) {
      if (baud_value == baud_ref[i].ubrr_value) {
        config.baud_rate = baud_ref[i].baud_rate;
        break;
      }
    }
    // Get data size
    if ((*ref.ctrl_reg_c & bit(UCSZ00)) && 
        (*ref.ctrl_reg_c & bit(UCSZ01)) &&
        (*ref.ctrl_reg_c & bit(UCSZ02))) {
      config.data_size = 5;
    } else if ((*ref.ctrl_reg_c & bit(UCSZ00)) && 
        !(*ref.ctrl_reg_c & bit(UCSZ01)) &&
        (*ref.ctrl_reg_c & bit(UCSZ02))) {
      config.data_size = 6;
    } else if (!(*ref.ctrl_reg_c & bit(UCSZ00)) && 
        (*ref.ctrl_reg_c & bit(UCSZ01)) &&
        !(*ref.ctrl_reg_c & bit(UCSZ02))) {
      config.data_size = 7;
    } else if ((*ref.ctrl_reg_c & bit(UCSZ00)) && 
        (*ref.ctrl_reg_c & bit(UCSZ01)) &&
        !(*ref.ctrl_reg_c & bit(UCSZ02))) {
      config.data_size = 8;
    }
    // Get mode
    if (*ref.ctrl_reg_c & bit(UMSEL00)) {
      config.mode = uart_mode_t::sync;
    } else {
      config.mode = uart_mode_t::async;
    }
    // Get parity
    if ((*ref.ctrl_reg_c & bit(UPM00)) && 
        (*ref.ctrl_reg_c & bit(UPM01))) {
      config.parity = uart_parity_t::odd;
    } else if ((*ref.ctrl_reg_c & bit(UPM00)) && 
        !(*ref.ctrl_reg_c & bit(UPM01))) {
      config.parity = uart_parity_t::even;
    } else {
      config.parity = uart_parity_t::none;
    }
  }
  return config;
}

// Data collection ISRs
ISR(USART0_RX_vect) {
  xSemaphoreTakeFromISR(uart_output[0].data_sem, nullptr);
  const uint8_t rx_value = *uart_ref[0].rx_reg;
  uart_output[0].uart_data.push(rx_value);
  xSemaphoreGiveFromISR(uart_output[0].data_sem, nullptr);
}
ISR(USART1_RX_vect) {
  xSemaphoreTakeFromISR(uart_output[1].data_sem, nullptr);
  const uint8_t rx_value = *uart_ref[1].rx_reg;
  uart_output[1].uart_data.push(rx_value);
  xSemaphoreGiveFromISR(uart_output[1].data_sem, nullptr);
}
ISR(USART2_RX_vect) {
  xSemaphoreTakeFromISR(uart_output[2].data_sem, nullptr);
  const uint8_t rx_value = *uart_ref[2].rx_reg;
  uart_output[2].uart_data.push(rx_value);
  xSemaphoreGiveFromISR(uart_output[2].data_sem, nullptr);
}
ISR(USART3_RX_vect) {
  xSemaphoreTakeFromISR(uart_output[3].data_sem, nullptr);
  const uint8_t rx_value = *uart_ref[3].rx_reg;
  uart_output[3].uart_data.push(rx_value);
  xSemaphoreGiveFromISR(uart_output[3].data_sem, nullptr);
}


#include "tc.hpp"
#include "limits.h"

static constexpr int32_t max_cnt{UINT16_MAX};

enum class edge_t {
  rising,
  falling
};

struct tc_ref_t {
  int32_t icp_pin;
  volatile uint8_t* ddr_reg;
  volatile uint8_t* ctrl_reg_a;
  volatile uint8_t* ctrl_reg_b;
  volatile uint8_t* ctrl_reg_c;
  volatile uint16_t* tcn_reg;
  volatile uint16_t* icr_reg;
  volatile uint8_t* timsk_reg;
};

static constexpr tc_ref_t tc_ref[tc_cnt]{
  {4, &DDRD, &TCCR1A, &TCCR1B, &TCCR1C, &TCNT1, &ICR1, &TIMSK1},
  {0, &DDRL, &TCCR4A, &TCCR4B, &TCCR4C, &TCNT4, &ICR4, &TIMSK4},
  {1, &DDRL, &TCCR5A, &TCCR5B, &TCCR5C, &TCNT5, &ICR5, &TIMSK5}
};

volatile uint64_t ovf_cnt[tc_cnt]{0};
volatile uint64_t high_cnt[tc_cnt]{0};
volatile edge_t current_edge[tc_cnt]{edge_t::rising};

static bool valid_tc(const int32_t num) {
  return num >= 0 && num < tc_cnt;
}

void init_tc() {
  for (int32_t i = 0; i < tc_cnt; i++) {
    tc_output[i].data_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(tc_output[i].data_sem);
  }
}

// Updates the tc peripheral's configuration
bool set_tc_config(const int32_t num, const tc_config_t config) {
  static bool init_flag{false};
  if (!valid_tc(num)) { return false; }
  auto& ref{tc_ref[num]};

  // Reset current state
  current_edge[num] = edge_t::rising;
  ovf_cnt[num] = 0;

  // Enable/disable and init peripheral
  if (config.enabled) {
    if (!init_flag) {
      *ref.ddr_reg &= ~bit(ref.icp_pin);
      *ref.ctrl_reg_c &= ~(bit(CS10) | bit(CS11) | bit(CS12));
      *ref.ctrl_reg_a &= ~(bit(WGM10) | bit(WGM11));
      *ref.ctrl_reg_b &= ~(bit(WGM12) | bit(WGM13));
      init_flag = true;
    }
    *ref.ctrl_reg_c |= bit(ICES1);
    *ref.timsk_reg |= bit(ICIE1);
    *ref.timsk_reg |= bit(TOIE1);
  } else {
    *ref.timsk_reg &= ~bit(ICIE1);
    *ref.timsk_reg &= ~bit(TOIE1);
  }
  return true;
}

// Gets the tc peripheral's current configuration
tc_config_t get_tc_config(const int32_t num) {
  tc_config_t config{};
  if (valid_tc(num)) {
    auto& ref{tc_ref[num]};
    if (*ref.timsk_reg & bit(ICIE1)) {
      config.enabled = true;
    } else {
      config.enabled = false;
    }
  }
  return config;
}

// Overflow detection ISRs
ISR(TIMER1_OVF_vect) { ovf_cnt[0]++; }
ISR(TIMER4_OVF_vect) { ovf_cnt[1]++; }
ISR(TIMER5_OVF_vect) { ovf_cnt[2]++; }

// Input capture ISRs (duty cycle/frequency calc)
ISR(TIMER1_CAPT_vect) {
  const uint16_t icr_value{*tc_ref[0].icr_reg};
  const uint64_t current_cnt{(ovf_cnt[0] * max_cnt) + icr_value};
  if (current_edge[0] == edge_t::rising) {
    *tc_ref[0].ctrl_reg_c &= ~bit(ICES1);
    current_edge[0] = edge_t::falling;
    ovf_cnt[0] = 0;
    const uint64_t total_cnt = high_cnt[0] + current_cnt;
    xSemaphoreGiveFromISR(tc_output[0].data_sem, nullptr);
    tc_output[0].freq = (double)F_CPU / (double)total_cnt;
    tc_output[0].duty_cycle = (double)high_cnt[0] / (double)total_cnt;
    xSemaphoreGiveFromISR(tc_output[0].data_sem, nullptr);
  } else {
    *tc_ref[0].ctrl_reg_c |= bit(ICES1);
    current_edge[0] = edge_t::rising;
    ovf_cnt[0] = 0;
    high_cnt[0] = current_cnt;
  }
}
ISR(TIMER4_CAPT_vect) {
  const uint16_t icr_value{*tc_ref[1].icr_reg};
  const uint64_t current_cnt{(ovf_cnt[1] * max_cnt) + icr_value};
  if (current_edge[1] == edge_t::rising) {
    *tc_ref[1].ctrl_reg_c &= ~bit(ICES1);
    current_edge[1] = edge_t::falling;
    ovf_cnt[1] = 0;
    const uint64_t total_cnt = high_cnt[1] + current_cnt;
    xSemaphoreGiveFromISR(tc_output[1].data_sem, nullptr);
    tc_output[1].freq = (double)F_CPU / (double)total_cnt;
    tc_output[1].duty_cycle = (double)high_cnt[1] / (double)total_cnt;
    xSemaphoreGiveFromISR(tc_output[1].data_sem, nullptr);
  } else {
    *tc_ref[1].ctrl_reg_c |= bit(ICES1);
    current_edge[1] = edge_t::rising;
    ovf_cnt[1] = 0;
    high_cnt[1] = current_cnt;
  }
}
ISR(TIMER5_CAPT_vect) {
  const uint16_t icr_value{*tc_ref[2].icr_reg};
  const uint64_t current_cnt{(ovf_cnt[2] * max_cnt) + icr_value};
  if (current_edge[2] == edge_t::rising) {
    *tc_ref[2].ctrl_reg_c &= ~bit(ICES1);
    current_edge[2] = edge_t::falling;
    ovf_cnt[2] = 0;
    const uint64_t total_cnt = high_cnt[2] + current_cnt;
    xSemaphoreGiveFromISR(tc_output[2].data_sem, nullptr);
    tc_output[2].freq = (double)F_CPU / (double)total_cnt;
    tc_output[2].duty_cycle = (double)high_cnt[2] / (double)total_cnt;
    xSemaphoreGiveFromISR(tc_output[2].data_sem, nullptr);
  } else {
    *tc_ref[2].ctrl_reg_c |= bit(ICES1);
    current_edge[2] = edge_t::rising;
    ovf_cnt[2] = 0;
    high_cnt[2] = current_cnt;
  }
}
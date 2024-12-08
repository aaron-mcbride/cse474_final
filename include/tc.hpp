
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

constexpr int32_t tc_cnt{3};
constexpr int64_t invalid_freq_v{-1};

struct tc_output_t {
  double freq{NAN};
  double duty_cycle{NAN};
  SemaphoreHandle_t data_sem{nullptr};
};

struct tc_config_t {
  bool enabled{false};
};

tc_output_t tc_output[tc_cnt]{};

void init_tc();

bool set_tc_config(const int32_t num, const tc_config_t config);

tc_config_t get_tc_config(const int32_t num);
// hal/battery_handler.h
#ifndef KANITO_TALLY_HAL_BATTERY_HANDLER_H_
#define KANITO_TALLY_HAL_BATTERY_HANDLER_H_

#include "config.h"

class BatteryHandler {
 public:
  explicit BatteryHandler(uint8_t pin);
  void Update();
  float GetVoltageMv();
  float GetPercentage();
  float GetEstimatedTimeRemaining();
  bool IsCharging();

 private:
  static constexpr float kVoltageDividerRatio = 2.0f;
  static constexpr float kAdcReferenceVoltage = 3.3f;
  static constexpr uint16_t kAdcResolution = 4095;
  static constexpr float kAlpha = 0.02f;

  uint8_t pin_;

  // Filter & State Variables
  float filtered_voltage_ = 0.0f;
  float current_percentage_ = 0.0f;

  // Tracking
  uint32_t last_update_time_ = 0;
  float last_percentage_ = 0.0f;
  float rate_of_change_ = 0.0f;            // % per ms
  float estimated_time_remaining_ = 0.0f;  // min

  float CalculatePercentage(float voltage);
};

#endif  // KANITO_TALLY_HAL_BATTERY_HANDLER_H_
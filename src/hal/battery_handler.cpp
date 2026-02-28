// hal/battery_handler.cpp

#include "battery_handler.h"

BatteryHandler::BatteryHandler(uint8_t pin) : pin_(pin) {
  pinMode(pin_, INPUT);
  analogReadResolution(12);
}

void BatteryHandler::Update() {
  // Read raw ADC value and convert to voltage
  uint16_t raw = analogRead(pin_);
  filtered_voltage_ = ((float)raw / kAdcResolution) * kAdcReferenceVoltage *
                      kVoltageDividerRatio;

  current_percentage_ = CalculatePercentage(filtered_voltage_);
  // Update rate of change every minute
  uint32_t current_time = millis();
  if (current_time - last_update_time_ > 60000) {
    float delta_percentage = current_percentage_ - last_percentage_;
    uint32_t delta_time = current_time - last_update_time_;
    rate_of_change_ = delta_percentage / delta_time;
    last_percentage_ = current_percentage_;
    last_update_time_ = current_time;
  }
  // Estimate time remaining based on rate of change
  if (rate_of_change_ == 0.0f) {
    estimated_time_remaining_ = -1.0f;
  } else if (IsCharging()) {
    estimated_time_remaining_ =
        (100.0f - current_percentage_) / (rate_of_change_ * 60000.0f);
  } else {
    estimated_time_remaining_ =
        (current_percentage_) / (-rate_of_change_ * 60000.0f);
  }
}
// Simple linear interpolation based on a voltage-percentage LUT
float BatteryHandler::CalculatePercentage(float voltage) {
  if (voltage >= 4.20f) return 100.0f;
  if (voltage <= 3.20f) return 0.0f;

  // Voltage | Percentage relation LUT
  static const float v_curve[11] = {3.20f, 3.65f, 3.70f, 3.75f, 3.79f, 3.83f,
                                    3.89f, 3.97f, 4.06f, 4.13f, 4.20f};
  for (uint8_t i = 0; i < 10; i++) {
    if (voltage >= v_curve[i] && voltage <= v_curve[i + 1]) {
      return (i * 10.0f) +
             10.0f * ((voltage - v_curve[i]) / (v_curve[i + 1] - v_curve[i]));
    }
  }
  return 0.0f;
}

// A simple threshold-based charging detection. If the battery percentage is
// rising
bool BatteryHandler::IsCharging() {
  return (rate_of_change_ > (0.5f / 6000.0f));
}

// Returns voltage in mV.
float BatteryHandler::GetVoltageMv() { return filtered_voltage_; }

// Returns percentage from 0 to 100.
float BatteryHandler::GetPercentage() { return current_percentage_; }

// Returns estimated time remaining in minutes. -1 if unknown (e.g. no change).
float BatteryHandler::GetEstimatedTimeRemaining() {
  return estimated_time_remaining_;
}
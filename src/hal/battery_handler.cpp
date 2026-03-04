// hal/battery_handler.cpp

#include "battery_handler.h"

BatteryHandler::BatteryHandler(uint8_t pin) : pin_(pin) {
  pinMode(pin_, INPUT);
  analogReadResolution(12);
}

void BatteryHandler::Update() {
  uint32_t current_time = millis();

  if (current_time - last_adc_time_ >= kAdcIntervalMs) {
    last_adc_time_ = current_time;

    float current_voltage_v =
        (analogReadMilliVolts(pin_) * kVoltageDividerRatio / 1000.0f);
    if (is_first_read_) {
      filtered_voltage_ = current_voltage_v;
      last_checked_voltage_ = current_voltage_v;
      last_percentage_ = CalculatePercentage(filtered_voltage_);
      is_first_read_ = false;
    } else {
      filtered_voltage_ =
          (current_voltage_v * kAlpha) + (filtered_voltage_ * (1.0f - kAlpha));
    }
    current_percentage_ = CalculatePercentage(filtered_voltage_);
  }

 
  if (current_time - last_charge_check_time_ > 5000) {
    float delta_voltage = filtered_voltage_ - last_checked_voltage_;

    if (delta_voltage > 0.01f) {
      charge_confidence_++;
    }
    // Wenn die Spannung sinkt (z.B. > 5mV), entlädt er sicher
    else if (delta_voltage < -0.005f) {
      if (charge_confidence_ > 0) charge_confidence_--;
    }

    // Hysterese: Status erst ändern, wenn das Signal stabil ist
    if (charge_confidence_ >= 3) {
      is_charging_ = true;
      charge_confidence_ = 3;  // Deckelung nach oben
    } else if (charge_confidence_ == 0) {
      is_charging_ = false;
    }

    last_checked_voltage_ = filtered_voltage_;
    last_charge_check_time_ = current_time;
  }

  // 3. Restzeit-Berechnung (Langsam: Nur alle 60 Sekunden updaten für ruhige
  // Anzeige)
  if (current_time - last_update_time_ > 60000) {
    float delta_percentage = current_percentage_ - last_percentage_;
    uint32_t delta_time = current_time - last_update_time_;

    rate_of_change_ =
        delta_percentage / delta_time;  // Prozent pro Millisekunde
    last_percentage_ = current_percentage_;
    last_update_time_ = current_time;

    // Estimate time remaining based on rate of change
    if (rate_of_change_ == 0.0f) {
      estimated_time_remaining_ = -1.0f;
    } else if (is_charging_) {
      estimated_time_remaining_ =
          (100.0f - current_percentage_) / (rate_of_change_ * 60000.0f);
    } else {
      estimated_time_remaining_ =
          (current_percentage_) / (-rate_of_change_ * 60000.0f);
    }
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
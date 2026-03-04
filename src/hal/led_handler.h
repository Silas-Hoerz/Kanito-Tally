// hal/led_handler.h
#ifndef KANITO_TALLY_HAL_LED_HANDLER_H_
#define KANITO_TALLY_HAL_LED_HANDLER_H_

#include <Arduino.h>

enum class LedMode { kOff, kOn, kBlink, kBreath, kPWM };

class LedHandler {
 public:
  explicit LedHandler(uint8_t pin);
  void Begin();
  void Update();
  void SetMode(LedMode mode);
  void SetIntervalMs(uint32_t interval);

  void SetMasterBrightness(uint8_t interval);

  void SetRelativeIntensity(float intensity);

 private:
  uint8_t pin_;
  LedMode current_mode_;
  uint32_t last_update_ms_;
  uint32_t interval_ms_;
  bool state_;

  uint8_t master_brightness_;
  float relative_intensity_;

  static constexpr uint8_t kPwmResolutionBits = 12;
  static constexpr uint32_t kPwmMaxVal = 4095;

  uint32_t CalculateGamma(float normalized_value);
};

#endif  // KANITO_TALLY_HAL_LED_HANDLER_H_
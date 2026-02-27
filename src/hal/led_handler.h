// hal/led_handler.h
#ifndef KANITO_TALLY_HAL_LED_HANDLER_H_
#define KANITO_TALLY_HAL_LED_HANDLER_H_

#include <Arduino.h>

enum class LedMode { kOff, kOn, kBlink, kBreath };

class LedHandler {
 public:
  explicit LedHandler(uint8_t pin);
  void Begin();
  void Update();
  void SetMode(LedMode mode);
  void SetIntervalMs(uint32_t interval);

 private:
  uint8_t pin_;
  LedMode current_mode_;
  uint32_t last_update_ms_;
  uint32_t interval_ms_;
  bool state_;
};

#endif  // KANITO_TALLY_HAL_LED_HANDLER_H_
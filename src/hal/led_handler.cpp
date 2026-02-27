// hal/led_handler.cpp
#include "led_handler.h"

#include <cmath>

LedHandler::LedHandler(uint8_t pin)
    : pin_(pin),
      current_mode_(LedMode::kOff),
      last_update_ms_(0),
      interval_ms_(500),
      state_(false) {}

void LedHandler::Begin() {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

void LedHandler::Update() {
  switch (current_mode_) {
    case LedMode::kOn:
      digitalWrite(pin_, HIGH);
      break;

    case LedMode::kBlink:
      if (millis() - last_update_ms_ >= interval_ms_) {
        last_update_ms_ = millis();
        state_ = !state_;
        digitalWrite(pin_, state_ ? HIGH : LOW);
      }
      break;

    case LedMode::kBreath: {
      float phase = (float)(millis() % interval_ms_) / (float)interval_ms_;
      uint8_t intensity =
          (uint8_t)((sin(phase * 2.0 * PI - PI / 2.0) + 1.0) * 127.5);
      analogWrite(pin_, intensity);
      break;
    }

    // LedMode::kOff
    default:
      digitalWrite(pin_, LOW);
      break;
  }
}

void LedHandler::SetMode(LedMode mode) { current_mode_ = mode; }

void LedHandler::SetIntervalMs(uint32_t interval) { interval_ms_ = interval; }
// core/tally_core.cpp
#include "tally_core.h"

#include <Arduino.h>

TallyCore::TallyCore(LedHandler& led, ButtonHandler& button)
    : current_state_(SystemState::kBoot),
      led_(led),
      button_(button),
      boot_time_(0) {}

void TallyCore::Begin() { OnStateEntry(current_state_); }

void TallyCore::SetState(SystemState new_state) {
  if (current_state_ == new_state) {
    return;  // No state change
  }
  // 1. Execute exit actions for the current state
  OnStateExit(current_state_);
  // 2. Update the current state
  current_state_ = new_state;
  // 3. Execute entry actions for the new state
  OnStateEntry(current_state_);
}

// --- Entry ---

void TallyCore::OnStateEntry(SystemState state) {
  switch (state) {
    case SystemState::kBoot: {
      boot_time_ = millis();
      break;
    }
    case SystemState::kStandby: {
      break;
    }
    case SystemState::kLive: {
      break;
    }
    case SystemState::kConfig: {
      break;
    }
    case SystemState::kShutdown: {
      break;
    }
    default:
      break;
  }
}

// --- Do ---

void TallyCore::Update() {
  switch (current_state_) {
    case SystemState::kBoot: {
      HandleBoot();
      break;
    }
    case SystemState::kStandby: {
      HandleStandby();
      break;
    }
    case SystemState::kLive: {
      HandleLive();
      break;
    }
    case SystemState::kConfig: {
      HandleConfig();
      break;
    }
    case SystemState::kShutdown: {
      HandleShutdown();
      break;
    }
    default:
      break;
  }
}

// --- Exit ---

void TallyCore::OnStateExit(SystemState state) {
  switch (state) {
    case SystemState::kBoot: {
      break;
    }
    case SystemState::kStandby: {
      break;
    }
    case SystemState::kLive: {
      break;
    }
    case SystemState::kConfig: {
      break;
    }
    case SystemState::kShutdown: {
      break;
    }
    default:
      break;
  }
}

// --- Specific State Implementations ---

void TallyCore::HandleBoot() {
  ButtonEvent button_event = button_.GetEvent();
  if (button_event == ButtonEvent::kLongPress) {
    SetState(SystemState::kStandby);
  } else if (millis() - boot_time_ >= 1000) {
    esp_deep_sleep_enable_gpio_wakeup((1ULL << kPinButton),
                                      ESP_GPIO_WAKEUP_GPIO_LOW);
    esp_deep_sleep_start();
  }
}

void TallyCore::HandleStandby() {
  // Check for Button Event
  ButtonEvent button_event = button_.GetEvent();
  if (button_event == ButtonEvent::kLongPress) {
    SetState(SystemState::kConfig);
  } else if (button_event == ButtonEvent::kShortAndLongPress) {
    SetState(SystemState::kShutdown);
  }
}

void TallyCore::HandleLive() {}
void TallyCore::HandleConfig() {}
void TallyCore::HandleShutdown() {
  Serial.println("Shutthing down.");
  esp_deep_sleep_enable_gpio_wakeup((1ULL << kPinButton),
                                    ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_start();
}
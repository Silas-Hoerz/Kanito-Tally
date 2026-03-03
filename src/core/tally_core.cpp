// core/tally_core.cpp
#include "tally_core.h"

#include <Arduino.h>

#include "secrets.h"

TallyCore::TallyCore(LedHandler& led, ButtonHandler& button,
                     NetworkHandler& network)
    : current_state_(SystemState::kBoot),
      led_(led),
      button_(button),
      network_(network),
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
      Serial.println("[CORE] State: Boot - Waiting for Long Press...");
      led_.SetMode(LedMode::kBlink);
      led_.SetIntervalMs(100);

      boot_time_ = millis();
      break;
    }
    case SystemState::kStandby: {
      Serial.println("[CORE] State: Standby - Connecting to Network...");
      led_.SetMode(LedMode::kBreath);
      led_.SetIntervalMs(2000);

      network_.Connect(kWifiSsid, kWifiPassword);
      break;
    }
    case SystemState::kLive: {
      Serial.println("[CORE] State: Live - ON AIR");
      led_.SetMode(LedMode::kOn);
      break;
    }
    case SystemState::kConfig: {
      Serial.println("[CORE] State: Config - Starting AP...");
      led_.SetMode(LedMode::kBlink);
      led_.SetIntervalMs(500);
      break;
    }
    case SystemState::kShutdown: {
      Serial.println("[CORE] State: Shutdown - Going to Deep Sleep...");
      led_.SetMode(LedMode::kOff);
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
  } else if (millis() - boot_time_ >= 5000) {
    //SetState(SystemState::kShutdown);
  }
}

void TallyCore::HandleStandby() {
  // Check for Button Event
  ButtonEvent button_event = button_.GetEvent();
  if (button_event == ButtonEvent::kLongPress) {
    SetState(SystemState::kConfig);
  } else if (button_event == ButtonEvent::kShortAndLongPress) {
    //SetState(SystemState::kShutdown);
  }
}

void TallyCore::HandleLive() {}

void TallyCore::HandleConfig() {}

void TallyCore::HandleShutdown() {
  delay(100);
 
  //esp_deep_sleep_enable_gpio_wakeup((1ULL << kPinButton),
  //                                  ESP_GPIO_WAKEUP_GPIO_LOW);
 // esp_deep_sleep_start();
}
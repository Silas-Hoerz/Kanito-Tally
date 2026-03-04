// core/tally_core.cpp
#include "tally_core.h"

#include <Arduino.h>

#include "secrets.h"

TallyCore::TallyCore(LedHandler& led, ButtonHandler& button,
                     NetworkHandler& network)
    : current_state_(SystemState::kBoot),
      previous_state_(SystemState::kBoot),
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
  previous_state_ = current_state_;
  current_state_ = new_state;

  led_.SetRelativeIntensity(1.0f);
  led_.SetMode(LedMode::kOn);
  led_.Update();
  delay(40);
  // 3. Execute entry actions for the new state
  OnStateEntry(current_state_);
}

// --- Entry ---

void TallyCore::OnStateEntry(SystemState state) {
  switch (state) {
    case SystemState::kBoot: {
      Serial.println("[CORE] State: Boot - Waiting for Long Press...");
      led_.SetRelativeIntensity(0.5f);
      led_.SetMode(LedMode::kOff);
      led_.SetIntervalMs(50);

      boot_time_ = millis();
      break;
    }
    case SystemState::kStandby: {
      Serial.println("[CORE] State: Standby - Connecting to Network...");
      led_.SetRelativeIntensity(0.3f);
      led_.SetMode(LedMode::kBreath);
      led_.SetIntervalMs(10000);

      network_.Connect(kWifiSsid, kWifiPassword);
      break;
    }
    case SystemState::kPreview: {
      led_.SetRelativeIntensity(0.5f);
      Serial.println("[CORE] State: Preview - Preview");
      led_.SetMode(LedMode::kOn);
      break;
    }
    case SystemState::kLive: {
      led_.SetRelativeIntensity(1.0f);
      Serial.println("[CORE] State: Live - ON AIR");
      led_.SetMode(LedMode::kOn);
      break;
    }
    case SystemState::kConfig: {
      Serial.println("[CORE] State: Config - Starting AP...");
      led_.SetRelativeIntensity(0.5f);
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
  ProcessPayload();
  ButtonEvent button_event = button_.GetEvent();

  bool event_consumed = false;
  if (button_event != ButtonEvent::kNone) {
    event_consumed = HandleGlobalButton(button_event);
    if (event_consumed) return;
  }

  switch (current_state_) {
    case SystemState::kBoot: {
      HandleBoot(button_event);
      break;
    }
    case SystemState::kStandby: {
      HandleStandby(button_event);
      break;
    }
    case SystemState::kPreview: {
      HandlePreview(button_event);
      break;
    }
    case SystemState::kLive: {
      HandleLive(button_event);
      break;
    }
    case SystemState::kConfig: {
      HandleConfig(button_event);
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
    case SystemState::kPreview: {
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

void TallyCore::HandleBoot(ButtonEvent button_event) {
  if (button_event == ButtonEvent::kLongPressStart) {
    SetState(SystemState::kStandby);
  } else if (millis() - boot_time_ >= 3000) {
    SetState(SystemState::kShutdown);
  }
}

void TallyCore::HandleStandby(ButtonEvent button_event) {}
void TallyCore::HandlePreview(ButtonEvent button_event) {}
void TallyCore::HandleLive(ButtonEvent button_event) {}
void TallyCore::HandleConfig(ButtonEvent button_event) {
  static uint32_t click_time = 0;
  static bool click_pending = false;

  if (button_event == ButtonEvent::kShortClicked) {
    click_time = millis();
    click_pending = true;
  }

  if (click_pending &&
      (millis() - click_time > button_.GetMaxSequencePause())) {
    click_pending = false;

    if (!button_.IsPressed()) {
      SetState(previous_state_);
    }
  }
}
void TallyCore::HandleShutdown() {
  delay(150);

  esp_deep_sleep_enable_gpio_wakeup((1ULL << kPinButton),
                                    ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_start();
}

void TallyCore::ProcessPayload() {
  // Payload incomming_data;

  // if (network_.GetLatestPayload(incomming_data)) {
  //   CommandType command = incomming_data.command;
  //   switch (command) {
  //     case CommandType::kHeartbeat:
  //       /* code */
  //       break;
  //     case CommandType::kSetState:
  //       switch (incomming_data.state) {
  //         case DeviceState::kLive:
  //           SetState(SystemState::kLive);
  //           break;
  //         case DeviceState::kPreview:
  //           SetState(SystemState::kPreview);
  //           break;
  //         case DeviceState::kOff:
  //           SetState(SystemState::kStandby);
  //           break;
  //       }
  //       break;
  //     case CommandType::kSetConfig:
  //       /* code */
  //       break;
  //     case CommandType::kDiscoveryPing:
  //       /* code */
  //       break;
  //     case CommandType::kDiscoveryPong:
  //       /* code */
  //       break;

  //     default:
  //       break;
  //   }
  // }
}

bool TallyCore::HandleGlobalButton(ButtonEvent button_event) {
  if (current_state_ == SystemState::kBoot ||
      current_state_ == SystemState::kShutdown) {
    return false;
  }

  static uint32_t last_short_press_time = 0;
  static bool is_in_sequence = false;
  uint32_t now = millis();

  if (button_event == ButtonEvent::kShortClicked) {
    last_short_press_time = now;
    return false;
  }

  if (button_event == ButtonEvent::kPressed) {
    if (last_short_press_time != 0 &&
        (now - last_short_press_time <= button_.GetMaxSequencePause())) {
      is_in_sequence = true;
    } else {
      is_in_sequence = false;
    }
    return false;
  }

  if (button_event == ButtonEvent::kLongPressStart) {
    if (is_in_sequence) {
      SetState(SystemState::kShutdown);
    } else {
      SetState(SystemState::kConfig);
    }

    last_short_press_time = 0;
    is_in_sequence = false;

    return true;
  }

  return false;
}
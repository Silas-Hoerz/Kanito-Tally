// core/tally_core.cpp
#include "tally_core.h"

// static wrapper-functions for callbacks
static TallyCore* g_core_ptr = nullptr;

void HandleClickWrapper() {
  if (g_core_ptr) g_core_ptr->HandleButtonPress();
}

void HandleLongPressStartWrapper() {
  if (g_core_ptr) g_core_ptr->HandleButtonLongPress();
}

TallyCore::TallyCore(LedHandler& led, ButtonHandler& button)
    : led_(led), button_(button), current_state_(SystemState::kBoot) {
  g_core_ptr = this;
}

void TallyCore::Begin() {
  button_.AttachClick(HandleClickWrapper);
  button_.AttachLongPressStart(HandleLongPressStartWrapper);
  SetState(SystemState::kBoot);
}

void TallyCore::SetState(SystemState new_state) {
  current_state_ = new_state;
  switch (current_state_) {
    case SystemState::kBoot: {
      led_.SetMode(LedMode::kBlink);
      led_.SetIntervalMs(100);
      break;
    }
    case SystemState::kStandby: {
      led_.SetMode(LedMode::kBreath);
      led_.SetIntervalMs(2000);
      break;
    }
    case SystemState::kLive: {
      led_.SetMode(LedMode::kOn);
      break;
    }
    case SystemState::kConfig: {
      led_.SetMode(LedMode::kBlink);
      led_.SetIntervalMs(500);
      break;
    }
    default:
      break;
  }
}

void TallyCore::HandleButtonPress() {
  if (current_state_ == SystemState::kStandby) {
    SetState(SystemState::kLive);
  } else if (current_state_ == SystemState::kLive) {
    SetState(SystemState::kStandby);
  }
}

void TallyCore::HandleButtonLongPress() {
  if (current_state_ == SystemState::kStandby) {
    SetState(SystemState::kLive);
  } else if (current_state_ == SystemState::kLive) {
    SetState(SystemState::kStandby);
  }
}

void TallyCore::Update() {}
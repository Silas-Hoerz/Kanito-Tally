// hal/button_handler.cpp
#include "button_handler.h"

ButtonHandler::ButtonHandler(uint8_t pin)
    : pin_(pin),
      button_(pin, true, true),  // pin, activeLow=true, pullupActive=true
      current_event_(ButtonEvent::kNone),
      sequence_length_(0),
      last_action_time_(0) {
  // Failsafe hardware setup
  pinMode(pin_, INPUT_PULLUP);

  // Link the callbacks
  button_.attachClick(OnClick, this);
  button_.attachDoubleClick(OnDoubleClick, this);
  button_.attachLongPressStart(OnLongPressStart, this);
  button_.attachLongPressStop(OnLongPressStop, this);

  // Optional tuning of OneButton internals (can be tweaked later)
  // button_.setClickMs(250);
  // button_.setPressMs(800);
}

void ButtonHandler::Update() {
  button_.tick();

  if (sequence_length_ > 0) {
    // Check if combo timeout has passed AND the button is currently NOT
    // pressed. Since we use INPUT_PULLUP, HIGH means the button is released.
    if ((millis() - last_action_time_ > kComboTimeoutMs) &&
        (digitalRead(pin_) == HIGH)) {
      EvaluateSequence();
    }
  }
}

ButtonEvent ButtonHandler::GetEvent() {
  ButtonEvent event_to_return = current_event_;
  current_event_ = ButtonEvent::kNone;
  return event_to_return;
}

void ButtonHandler::AddAction(ActionType action) {
  // Prevent buffer overflow. We only support combos up to 2 actions.
  if (sequence_length_ < 2) {
    action_sequence_[sequence_length_] = action;
    sequence_length_++;
  }
  // Reset the timer every time a new action is added
  last_action_time_ = millis();
}

void ButtonHandler::EvaluateSequence() {
  if (sequence_length_ == 1) {
    if (action_sequence_[0] == ActionType::kShort) {
      current_event_ = ButtonEvent::kShortPress;
    } else if (action_sequence_[0] == ActionType::kLong) {
      current_event_ = ButtonEvent::kLongPress;
    }
  } else if (sequence_length_ == 2) {
    if (action_sequence_[0] == ActionType::kShort &&
        action_sequence_[1] == ActionType::kShort) {
      current_event_ = ButtonEvent::kDoubleShortPress;
    } else if (action_sequence_[0] == ActionType::kLong &&
               action_sequence_[1] == ActionType::kLong) {
      current_event_ = ButtonEvent::kDoubleLongPress;
    } else if (action_sequence_[0] == ActionType::kShort &&
               action_sequence_[1] == ActionType::kLong) {
      current_event_ = ButtonEvent::kShortAndLongPress;
    } else if (action_sequence_[0] == ActionType::kLong &&
               action_sequence_[1] == ActionType::kShort) {
      current_event_ = ButtonEvent::kLongAndShortPress;
    }
  }

  // Clear the sequence after evaluation to be ready for the next input
  sequence_length_ = 0;
}

// --- Static Callback Wrappers ---

void ButtonHandler::OnClick(void* ptr) {
  ButtonHandler* instance = static_cast<ButtonHandler*>(ptr);
  // OneButton detected a single short press that was completed
  instance->AddAction(ActionType::kShort);
}

void ButtonHandler::OnDoubleClick(void* ptr) {
  ButtonHandler* instance = static_cast<ButtonHandler*>(ptr);
  // OneButton natively caught two short presses within its own clickTicks
  instance->AddAction(ActionType::kShort);
  instance->AddAction(ActionType::kShort);
}

void ButtonHandler::OnLongPressStart(void* ptr) {
  ButtonHandler* instance = static_cast<ButtonHandler*>(ptr);
  instance->AddAction(ActionType::kLong);
}

void ButtonHandler::OnLongPressStop(void* ptr) {
  ButtonHandler* instance = static_cast<ButtonHandler*>(ptr);
  // When the long press is released, we reset the timer so the user
  // has kComboTimeoutMs time to press another button before evaluation.
  instance->last_action_time_ = millis();
}
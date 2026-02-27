// hal/button_handler.h
#ifndef KANITO_TALLY_HAL_BUTTON_HANDLER_H_
#define KANITO_TALLY_HAL_BUTTON_HANDLER_H_

#include <OneButton.h>

#include "config.h"

// Strict enum class fr all logical button events
enum class ButtonEvent {
  kNone,
  kShortPress,
  kDoubleShortPress,
  kLongPress,
  kDoubleLongPress,
  kShortAndLongPress,
  kLongAndShortPress
};

class ButtonHandler {
 public:
  explicit ButtonHandler(uint8_t pin);

  void Update();

  ButtonEvent GetEvent();

 private:
  enum class ActionType { kNone, kShort, kLong };

  OneButton button_;
  uint8_t pin_;
  ButtonEvent current_event_;

  ActionType action_sequence_[2];
  uint8_t sequence_length_;
  uint32_t last_action_time_;

  static constexpr uint32_t kComboTimeoutMs = 300;

  void AddAction(ActionType action);
  void EvaluateSequence();

  // Callback methods required by OneButton.
  static void OnClick(void* ptr);
  static void OnDoubleClick(void* ptr);
  static void OnLongPressStart(void* ptr);
  static void OnLongPressStop(void* ptr);
};
#endif  // KANITO_TALLY_HAL_BUTTON_HANDLER_H_
// core/tally_core.h
#ifndef KANITO_TALLY_CORE_TALLY_CORE_H_
#define KANITO_TALLY_CORE_TALLY_CORE_H_

#include "hal/button_handler.h"
#include "hal/led_handler.h"

enum class SystemState { kBoot, kStandby, kLive, kConfig, kShutdown };

class TallyCore {
 public:
  explicit TallyCore(LedHandler& led, ButtonHandler& button);
  void Begin();
  void Update();
  // Triggers a state transition
  void SetState(SystemState new_state);

 private:
  void HandleBoot();
  void HandleStandby();
  void HandleLive();
  void HandleConfig();
  void HandleShutdown();

  // Transition helpers
  void OnStateEntry(SystemState state);
  void OnStateExit(SystemState state);

  SystemState current_state_;
  LedHandler& led_;
  ButtonHandler& button_;

  uint32_t boot_time_;
};

#endif  // KANITO_TALLY_CORE_TALLY_CORE_H_
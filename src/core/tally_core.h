// core/tally_core.h
#ifndef KANITO_TALLY_CORE_TALLY_CORE_H_
#define KANITO_TALLY_CORE_TALLY_CORE_H_

#include "hal/button_handler.h"
#include "hal/led_handler.h"

enum class SystemState { kBoot, kStandby, kLive, kConfig };

class TallyCore {
 public:
  explicit TallyCore(LedHandler& led, ButtonHandler& button);
  void Begin();
  void Update();
  void SetState(SystemState state);

 private:
  void HandleBoot();
  void HandleStandby();
  void HandleLive();
  void HandleConfig();

  void HandleButtonPress();
  void HandleButtonLongPress();

  SystemState current_state_;
  LedHandler& led_;
  ButtonHandler& button_;
};

#endif  // KANITO_TALLY_CORE_TALLY_CORE_H_
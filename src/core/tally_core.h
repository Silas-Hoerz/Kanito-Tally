// core/tally_core.h
#ifndef KANITO_TALLY_CORE_TALLY_CORE_H_
#define KANITO_TALLY_CORE_TALLY_CORE_H_

#include "hal/button_handler.h"
#include "hal/led_handler.h"
#include "network/network_handler.h"

enum class SystemState { kBoot, kStandby, kPreview, kLive, kConfig, kShutdown };

class TallyCore {
 public:
  explicit TallyCore(LedHandler& led, ButtonHandler& button,
                     NetworkHandler& network);
  void Begin();
  void Update();
  // Triggers a state transition
  void SetState(SystemState new_state);

 private:
  void HandleBoot(ButtonEvent event);
  void HandleStandby(ButtonEvent event);
  void HandlePreview(ButtonEvent event);
  void HandleLive(ButtonEvent event);
  void HandleConfig(ButtonEvent event);
  void HandleShutdown();

  void ProcessPayload();

  // Transition helpers
  void OnStateEntry(SystemState state);
  void OnStateExit(SystemState state);

  SystemState current_state_;
  SystemState previous_state_;
  LedHandler& led_;
  ButtonHandler& button_;
  NetworkHandler& network_;

  uint32_t boot_time_;
};

#endif  // KANITO_TALLY_CORE_TALLY_CORE_H_
// main.cpp
#include <Arduino.h>

#include "config.h"
#include "core/tally_core.h"
#include "hal/button_handler.h"
#include "hal/led_handler.h"

// Global objects
LedHandler status_led(kPinLedRed);
ButtonHandler button(kPinButton);
TallyCore core(status_led, button);

void setup() {
  Serial.begin(kSerialBaud);
  status_led.Begin();
  status_led.SetMode(LedMode::kBlink);
  status_led.SetIntervalMs(500);

  core.Begin();

  Serial.println("Kanito Tally initialized.");
}

void loop() {
  status_led.Update();
  button.Update();
  core.Update();
}
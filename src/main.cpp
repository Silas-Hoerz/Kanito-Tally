// main.cpp
#include <Arduino.h>

#include "config.h"
#include "core/tally_core.h"
#include "hal/button_handler.h"
#include "hal/led_handler.h"
#include "network/network_handler.h"

// Global objects
LedHandler status_led(kPinLedRed);
ButtonHandler button(kPinButton);
NetworkHandler network;
TallyCore core(status_led, button, network);

void setup() {
  Serial.begin(kSerialBaud);
  delay(3000);
  status_led.Begin();
  status_led.SetMode(LedMode::kBlink);
  status_led.SetIntervalMs(500);

  core.Begin();

  Serial.println("Kanito Tally initialized.");
}

void loop() {
  status_led.Update();
  button.Update();
  network.Update();
  core.Update();
}

// network/network_handler.h
#ifndef KANITO_TALLY_NETWORK_NETWORK_HANDLER_H_
#define KANITO_TALLY_NETWORK_NETWORK_HANDLER_H_

#include <AsyncUDP.h>
#include <WiFi.h>

#include "Arduino.h"
#include "config.h"
#include "esp_wifi.h"

enum class NetworkState { kDisconnected, kConnecting, kConnected };

enum class SignalQuality { kExcellent, kGood, kPoor, kCritical };

class NetworkHandler {
 public:
  explicit NetworkHandler();

  void Connect(const char* ssid, const char* password);
  void Disconnect();
  void Update();

  NetworkState GetState();
  bool IsConnected();
  int8_t GetRssi();

 private:
  void EvaluateSignalStrength();

  NetworkState current_state_ = NetworkState::kDisconnected;
  uint32_t connection_start_time_ = 0;
  uint32_t last_reconnect_attempt_ = 0;
  uint32_t last_rssi_check_time_ = 0;

  SignalQuality current_signal_quality_ = SignalQuality::kExcellent;

  char ssid_[33];
  char password_[64];

  IPAddress local_ip_;
  int8_t rssi_;

  bool is_power_save_active_ = false;
  bool udp_initialized_ = false;

  AsyncUDP udp_;

  static constexpr uint32_t kConnectionTimeoutMs = 10000;
  static constexpr uint32_t kReconnectIntervallMs = 5000;
  // static constexpr uint32_t kEvaluationIntervallMs = 2000;
};

#endif  // KANITO_TALLY_NETWORK_NETWORK_HANDLER_H_
// network/network_handler.cpp

#include "network_handler.h"

NetworkHandler::NetworkHandler() {}

void NetworkHandler::Connect(const char* ssid, const char* password) {
  strlcpy(ssid_, ssid, sizeof(ssid_));
  strlcpy(password_, password, sizeof(password_));
  WiFi.mode(WiFiMode_t::WIFI_MODE_STA);

  // sleep between DTIM Router-Beacons
  esp_wifi_set_ps(wifi_ps_type_t::WIFI_PS_NONE);

  WiFi.begin(ssid_, password_);
  current_state_ = NetworkState::kConnecting;
  connection_start_time_ = millis();
}

void NetworkHandler::Disconnect() {
  WiFi.disconnect();
  last_reconnect_attempt_ = millis();
  current_state_ = NetworkState::kDisconnected;
}

void NetworkHandler::Update() {
  switch (current_state_) {
    case NetworkState::kDisconnected:
      if (strlen(ssid_) > 0) {
        if (millis() - last_reconnect_attempt_ > kReconnectIntervallMs) {
          last_reconnect_attempt_ = millis();
          WiFi.begin(ssid_, password_);
          current_state_ = NetworkState::kConnecting;
          connection_start_time_ = millis();
        }
      }
      break;

    case NetworkState::kConnecting:
      if (WiFi.status() == wl_status_t::WL_CONNECTED) {
        current_state_ = NetworkState::kConnected;

        local_ip_ = WiFi.localIP();
        Serial.print("WiFi connected. IP: ");
        Serial.println(local_ip_);

        // UDP Setup
        if (!udp_initialized_) {
          if (udp_.listen(4444)) {
            udp_.onPacket([](AsyncUDPPacket packet) {
              String message = packet.readString();
              Serial.printf("UDP Paket von %s empfangen; %s\n",
                            packet.remoteIP().toString().c_str(),
                            message.c_str());
            });
          }
        }

      } else {
        if (millis() - connection_start_time_ > kConnectionTimeoutMs) {
          current_state_ = NetworkState::kDisconnected;
          last_reconnect_attempt_ = millis();
        }
      }
      break;

    case NetworkState::kConnected:
      if (WiFi.status() != wl_status_t::WL_CONNECTED) {
        current_state_ = NetworkState::kDisconnected;
        last_reconnect_attempt_ = millis();
        break;
      }

      EvaluateSignalStrength();

      break;
  }
}

NetworkState NetworkHandler::GetState() { return current_state_; }

bool NetworkHandler::IsConnected() {
  return (current_state_ == NetworkState::kConnected);
}

int8_t NetworkHandler::GetRssi() { return rssi_; }

void NetworkHandler::EvaluateSignalStrength() {
  if (millis() - last_rssi_check_time_ < 2000) return;
  last_rssi_check_time_ = millis();

  rssi_ = WiFi.RSSI();

  if (rssi_ < -80)
    current_signal_quality_ = SignalQuality::kCritical;
  else if (rssi_ < -70)
    current_signal_quality_ = SignalQuality::kPoor;
  else if (rssi_ < -60)
    current_signal_quality_ = SignalQuality::kGood;
  else
    current_signal_quality_ = SignalQuality::kExcellent;

  if (current_signal_quality_ == SignalQuality::kCritical &&
      is_power_save_active_) {
    esp_wifi_set_ps(wifi_ps_type_t::WIFI_PS_NONE);
    is_power_save_active_ = false;
  } else if (current_signal_quality_ <= SignalQuality::kPoor &&
             !is_power_save_active_) {
    esp_wifi_set_ps(wifi_ps_type_t::WIFI_PS_MIN_MODEM);
    is_power_save_active_ = true;
  }
}
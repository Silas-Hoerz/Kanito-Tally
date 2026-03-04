// network/protocol.h
#ifndef KANITO_TALLY_NETWORK_PROTOCOL_H_
#define KANITO_TALLY_NETWORK_PROTOCOL_H_

#include <cstdint>

static constexpr uint16_t kMagicWord = 0x4B54;  // 'K' 'T' (Kanito-Tally)
static constexpr uint8_t kProtocolVersion = 1;

enum class CommandType : uint8_t {
  kHeartbeat = 0x00,      // Tally -> Hub
  kSetState = 0x01,       // Hub -> Tally
  kSetConfig = 0x02,      // Hub -> Tally
  kDiscoveryPing = 0x03,  // Hub -> Tally
  kDiscoveryPong = 0x04   // Tally -> Hub
};

enum class DeviceState : uint8_t {
  kOff = 0,
  kPreview = 1,
  kLive = 2,
};

// Specific Payloads

struct StatePayload{
    DeviceState state;
};

struct ConfigPayload{
    char wifi_ssid[32];
    char wifi_password[64];
};

struct __attribute__((packed)) Payload {
  uint16_t magic_word;
  uint8_t version;
  CommandType command;

  DeviceState state;
  uint8_t brightness;
  uint16_t reserved;
};

#endif  // KANITO_TALLY_NETWORK_PROTOCOL_H_
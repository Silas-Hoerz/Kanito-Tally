import socket
import time
import struct

# TRAGE HIER WIEDER DIE IP DEINES ESP32 EIN
ESP_IP = "192.168.178.169"
UDP_PORT = 4444

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print(f"Sende binäre UDP Pakete an {ESP_IP}:{UDP_PORT}...")

# C++ Struct Layout:
# uint16_t magic_word  (2 Bytes)
# uint8_t  version     (1 Byte)
# uint8_t  command     (1 Byte) - CommandType::kSetState = 1
# uint8_t  state       (1 Byte) - DeviceState::kLive = 2, kPreview = 1, kOff = 0
# uint8_t  brightness  (1 Byte) - 0-255
# uint16_t reserved    (2 Bytes)
# = 8 Bytes gesamt

# '<' bedeutet Little-Endian (ESP32 Standard)
# 'H B B B B H' sind die Datentypen (H=uint16, B=uint8)
STRUCT_FORMAT = '<HBBBBH'

def send_payload(target_state, brightness):
    # Packe die Variablen exakt wie das C++ Struct in ein Byte-Array
    binary_data = struct.pack(
        STRUCT_FORMAT,
        0x4B54,      # Magic Word
        1,           # Version
        1,           # Command (kSetState)
        target_state,# State
        brightness,  # Brightness
        0            # Reserved
    )
    sock.sendto(binary_data, (ESP_IP, UDP_PORT))

try:
    while True:
        print("Sende LIVE (State 2, 100% Brightness)")
        send_payload(2, 255)
        time.sleep(2)
        
        print("Sende PREVIEW (State 1, 50% Brightness)")
        send_payload(1, 128)
        time.sleep(2)

except KeyboardInterrupt:
    print("\nBeendet.")
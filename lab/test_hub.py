import socket
import time

# TRAGE HIER DIE IP EIN, DIE DEIN ESP32 IM SERIELLEN MONITOR AUSGIBT
ESP_IP = "192.168.178.169" 
UDP_PORT = 4444

print(f"Sende UDP Pakete an {ESP_IP}:{UDP_PORT}...")

# UDP Socket erstellen
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:
        # Wir simulieren den Live-Zustand (Rot)
        message = "STATE:LIVE"
        sock.sendto(message.encode(), (ESP_IP, UDP_PORT))
        print(f"Gesendet: {message}")
        time.sleep(2)
        
        # Wir simulieren den Standby-Zustand
        message = "STATE:STANDBY"
        sock.sendto(message.encode(), (ESP_IP, UDP_PORT))
        print(f"Gesendet: {message}")
        time.sleep(2)
        
except KeyboardInterrupt:
    print("\nBeendet.")
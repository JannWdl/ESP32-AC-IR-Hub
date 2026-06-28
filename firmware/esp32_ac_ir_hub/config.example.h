#pragma once

// Kopiere diese Datei nach config.h und trage dein WLAN ein.
// config.h wird absichtlich nicht nach GitHub gepusht.

#define WIFI_SSID "DEIN_WLAN"
#define WIFI_PASSWORD "DEIN_PASSWORT"

// KY-005 IR-Sender
#define IR_LED_PIN 4

// KY-022 IR-Empfänger
#define IR_RECV_PIN 15

#define HOSTNAME "esp32-klima"

#define ENABLE_AP_FALLBACK 1
#define AP_SSID "ESP32-Klima-Setup"
#define AP_PASSWORD "12345678"

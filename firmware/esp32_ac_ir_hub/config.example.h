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

// Optionales OLED-Display, z. B. 0,96" SSD1306 128x64 I2C.
// 0 = Display deaktiviert, 1 = Display aktiv.
#define OLED_ENABLED 1
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET_PIN -1

// Optionaler DHT11 Raumtemperatur-/Luftfeuchtigkeitssensor.
#define DHT_ENABLED 1
#define DHT_PIN 26
#define DHT_TYPE DHT11

// Optionaler MQ-135 Luftqualitätssensor.
// MQ135_PIN muss ein ADC-fähiger ESP32-Pin sein. GPIO34 ist Input-only und ideal.
#define MQ135_ENABLED 1
#define MQ135_PIN 34
#define MQ135_ADC_REF_VOLTAGE 3.3f

// Grobe Rohwert-Schwellen für die Web-/OLED-Anzeige.
// Nicht als kalibrierter ppm-Wert verstehen.
#define MQ135_GOOD_MAX_ADC 1200
#define MQ135_OK_MAX_ADC 2200
#define MQ135_HIGH_MAX_ADC 3200

#define SENSOR_READ_INTERVAL_MS 5000UL

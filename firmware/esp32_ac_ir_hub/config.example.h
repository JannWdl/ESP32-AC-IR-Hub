#pragma once

#define WIFI_SSID "DEIN_WLAN"
#define WIFI_PASSWORD "DEIN_PASSWORT"

#define IR_LED_PIN 4
#define IR_RECV_PIN 15

#define HOSTNAME "esp32-klima"

#define ENABLE_AP_FALLBACK 1
#define AP_SSID "ESP32-Klima-Setup"
#define AP_PASSWORD "12345678"

#define OLED_ENABLED 1
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET_PIN -1

#define DHT_ENABLED 0
#define DHT_PIN 26
#define DHT_TYPE DHT11

#define MQ135_ENABLED 0
#define MQ135_PIN 34
#define MQ135_ADC_REF_VOLTAGE 3.3f

#define SENSOR_READ_INTERVAL_MS 5000UL

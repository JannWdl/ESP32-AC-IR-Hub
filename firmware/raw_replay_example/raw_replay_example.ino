#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#include "config.h"

// Universal-Fallback:
// Wenn ein Protokoll nicht nativ sauber läuft, kannst du den rawData[]-Block
// aus dem IR-Reader hier einfügen und per sendRaw() senden.

IRsend irsend(IR_LED_PIN);

// Beispiel-Platzhalter. Ersetze den Inhalt mit deinem kompletten rawData[]-Block.
uint16_t RAW_EXAMPLE[] = {
  9000, 4500, 560, 560, 560, 1690, 560
};

void setup() {
  Serial.begin(115200);
  delay(500);

  irsend.begin();

  Serial.println("Raw Replay Beispiel gestartet.");
  Serial.println("Sendet alle 5 Sekunden den Beispielcode.");
}

void loop() {
  irsend.sendRaw(RAW_EXAMPLE, sizeof(RAW_EXAMPLE) / sizeof(RAW_EXAMPLE[0]), 38);
  Serial.println("Raw IR gesendet.");
  delay(5000);
}

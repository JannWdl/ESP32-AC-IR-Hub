#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// KY-022:
// -  -> GND
// +  -> 3V3
// S  -> GPIO 15
const uint16_t IR_RECV_PIN = 15;

const uint16_t CAPTURE_BUFFER_SIZE = 1024;
const uint8_t TIMEOUT = 50;

IRrecv irrecv(IR_RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);
decode_results results;

void setup() {
  Serial.begin(115200);
  delay(1000);

  irrecv.enableIRIn();

  Serial.println();
  Serial.println("ESP32 IR Reader gestartet");
  Serial.println("KY-022 an GPIO 15 anschliessen.");
  Serial.println("Fernbedienung auf den Empfaenger richten und Taste druecken.");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println();
    Serial.println("========== IR SIGNAL ==========");
    Serial.println(resultToHumanReadableBasic(&results));
    Serial.println();
    Serial.println("Als Source-Code:");
    Serial.println(resultToSourceCode(&results));
    Serial.println("===============================");
    Serial.println();

    irrecv.resume();
  }
}

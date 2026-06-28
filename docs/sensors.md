# Sensoren: DHT11 und MQ-135

Die Firmware kann zusätzlich Raumdaten erfassen und sinnvoll in Webinterface, API und OLED anzeigen.

## DHT11

Der DHT11 liefert:

- Raumtemperatur in °C
- relative Luftfeuchtigkeit in %

Standardpin:

```text
GPIO 26
```

Anzeige:

- Webinterface: Karte `Raum & Sensoren`
- API: `sensors.dht11.temperatureC` und `sensors.dht11.humidityPercent`
- OLED: `R:23C 45%`

## MQ-135

Der MQ-135 wird erstmal nur als einfacher Analog-Sensor eingebunden.

Die Firmware zeigt bewusst keine ppm-Werte und keine Qualitätsbewertung an. Ohne Kalibrierung, bekannten Lastwiderstand, Temperatur-/Feuchtekompensation und Einbrennzeit wären solche Werte irreführend.

Angezeigt werden nur:

- ADC-Rohwert 0 bis 4095
- Spannung am ADC

Standardpin:

```text
GPIO 34
```

API:

```json
{
  "sensors": {
    "mq135": {
      "enabled": true,
      "pin": 34,
      "raw": 1234,
      "voltage": 0.994
    }
  }
}
```

## Flash-Skript

Beim Flashen fragt `scripts/flash.ps1` ab:

```text
OLED Display angeschlossen? [J/n]
MQ-135 Sensor angeschlossen? [J/n]
DHT11 Sensor angeschlossen? [J/n]
```

Damit werden diese Werte in `config.h` gesetzt:

```cpp
#define OLED_ENABLED 1
#define MQ135_ENABLED 1
#define DHT_ENABLED 1
```

Ohne Abfrage, z. B. für Automationen:

```powershell
.\scripts\flash.ps1 COM7 -NoHardwarePrompt
```

## Pins ändern

In `firmware/esp32_ac_ir_hub/config.h`:

```cpp
#define DHT_PIN 26
#define MQ135_PIN 34
```

## Hinweise

- DHT11 ist langsam und wird nur alle 5 Sekunden abgefragt.
- MQ-135 braucht Aufwärmzeit.
- MQ-135-Werte sind Rohwerte/Trends, keine kalibrierten Messwerte.
- GPIO34 ist nur Eingang und daher sehr gut für den analogen MQ-135-Wert geeignet.

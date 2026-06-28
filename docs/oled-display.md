# OLED-Display

Diese Version unterstützt optional ein kleines I2C-OLED mit SSD1306-Controller, typischerweise 0,96 Zoll mit 128x64 Pixeln.

## Anzeigeinhalt

Das Display zeigt:

- Power: AN / AUS
- Temperatur
- Modus, z. B. Kuehlen oder Heizen
- Lüfterstufe
- IP-Adresse
- Quelle der letzten Änderung: Web, Remote, Speicher oder Fallback

Die Anzeige aktualisiert sich automatisch, wenn:

- du im Webinterface etwas änderst
- der ESP32 die Original-Fernbedienung über KY-022 mithört
- der gespeicherte Zustand beim Booten geladen wird

## Anschluss

```text
OLED VCC -> ESP32 3V3
OLED GND -> ESP32 GND
OLED SDA -> ESP32 GPIO 21
OLED SCL -> ESP32 GPIO 22
```

## Aktivieren / deaktivieren

In `firmware/esp32_ac_ir_hub/config.h`:

```cpp
#define OLED_ENABLED 1
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET_PIN -1
```

Ohne OLED:

```cpp
#define OLED_ENABLED 0
```

## Installation der Libraries

Das PowerShell-Installationsskript installiert automatisch:

```powershell
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "Adafruit GFX Library"
```

## Fehlerbehebung

### Serieller Monitor: OLED nicht gefunden

Prüfen:

1. VCC wirklich an 3V3
2. GND verbunden
3. SDA an GPIO 21
4. SCL an GPIO 22
5. Adresse testweise von `0x3C` auf `0x3D` ändern

### Display bleibt schwarz, Webinterface funktioniert

Dann ist meist die I2C-Adresse falsch oder das Display braucht eine andere Versorgung. Viele Module laufen an 3,3 V, manche sind mit 5 V stabiler. Beim ESP32 bleiben SDA/SCL aber 3,3-V-Logik.

### Umlaute fehlen auf dem Display

Die Firmware nutzt für das OLED bewusst ASCII-Texte wie `Kuehlen` und `Luefter`, weil die Standard-Schrift der Adafruit-GFX-Library keine saubere Umlautdarstellung bietet.

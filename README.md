# ESP32 AC IR Hub

ESP32-Webinterface zur Steuerung von Klimaanlagen per Infrarot.

Der aktuelle Stand ist auf deine Anlage ausgelegt:

- ESP32
- Mitsubishi Heavy Industries
- erkanntes Protokoll: `MITSUBISHI_HEAVY_152`
- IR-Sender: `KY-005`
- IR-Empfänger: `KY-022`
- optionales OLED-Display SSD1306 I2C 128x64
- optionaler DHT11 für Raumtemperatur und Luftfeuchtigkeit
- optionaler MQ-135 als Luftqualitätsindikator
- Webinterface + JSON-API
- Original-Fernbedienung wird mitgehört, damit Weboberfläche und OLED den Zustand aktualisieren können

> Wichtig: IR ist normalerweise Einbahnstraße. Der ESP32 weiß sicher, was er selbst gesendet hat und was der KY-022 gesehen hat. Er weiß nicht garantiert, ob die Klimaanlage den Befehl wirklich empfangen hat.

## Funktionen

- Weboberfläche auf dem ESP32
- Statusanzeige: Power, Temperatur, Modus, Lüfter und Quelle der letzten Änderung
- OLED-Anzeige direkt am Gerät
- Raumdaten mit DHT11
- Luftqualitäts-Trend mit MQ-135
- API für Home Assistant / eigene Automationen
- IR-Sync: Original-Fernbedienung mithören
- letzter Zustand wird im ESP32-Speicher gesichert
- Reader-Sketch zum Erkennen fremder Fernbedienungen
- Raw-Replay-Beispiel für unbekannte oder noch nicht sauber unterstützte Protokolle

## Standard-Pins

```text
KY-005 S      -> GPIO 4
KY-022 S      -> GPIO 15
OLED SDA      -> GPIO 21
OLED SCL      -> GPIO 22
DHT11 OUT     -> GPIO 26
MQ-135 AO     -> GPIO 34
```

Mehr dazu:

- [`docs/wiring.md`](docs/wiring.md)
- [`docs/oled-display.md`](docs/oled-display.md)
- [`docs/sensors.md`](docs/sensors.md)
- [`docs/supported-ac-protocols.md`](docs/supported-ac-protocols.md)

## Schnellstart unter Windows

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\install.ps1
.\scripts\flash.ps1 COM7
.\scripts\monitor.ps1 COM7
```

Beim Flashen fragt das Skript:

```text
OLED Display angeschlossen? [J/n]
MQ-135 Sensor angeschlossen? [J/n]
DHT11 Sensor angeschlossen? [J/n]
```

Ohne Abfrage:

```powershell
.\scripts\flash.ps1 COM7 -NoHardwarePrompt
```

## Optionale Hardware beim Installieren deaktivieren

```powershell
.\scripts\install.ps1 -DisableOled
.\scripts\install.ps1 -DisableDht11
.\scripts\install.ps1 -DisableMq135
```

OLED mit anderer Adresse:

```powershell
.\scripts\install.ps1 -OledAddress 0x3D
```

## Webinterface

Im seriellen Monitor steht die IP-Adresse:

```text
IP: 192.168.178.xxx
```

Dann öffnen:

```text
http://192.168.178.xxx
```

oder:

```text
http://esp32-klima.local
```

## API

```text
GET /api/status
GET /api/set?power=1
GET /api/set?power=0
GET /api/set?temp=25
GET /api/set?mode=cool
GET /api/set?mode=heat
GET /api/set?mode=dry
GET /api/set?fan=auto
GET /api/send
```

`/api/status` liefert auch OLED- und Sensordaten zurück.

## Aktueller gelernter Zustand

```text
Protocol  : MITSUBISHI_HEAVY_152
Zustand   : AN, 25 °C
```

```cpp
uint8_t state[19] = {
  0xAD, 0x51, 0x3C, 0xE5, 0x1A, 0x09, 0xF6, 0x08, 0xF7,
  0x01, 0xFE, 0x72, 0x8D, 0x65, 0x9A, 0x00, 0xFF, 0x80, 0x7F
};
```

## Lizenz

MIT für dieses Projekt. Die eingebundenen Libraries haben eigene Lizenzen und werden per Arduino CLI oder PlatformIO installiert.

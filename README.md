# ESP32 AC IR Hub

ESP32-Webinterface zur Steuerung von Klimaanlagen per Infrarot.

Der aktuelle Stand ist auf deine Anlage ausgelegt:

- **ESP32**
- **Mitsubishi Heavy Industries**
- erkanntes Protokoll: `MITSUBISHI_HEAVY_152`
- IR-Sender: `KY-005`
- IR-Empfänger: `KY-022`
- Webinterface + JSON-API
- Original-Fernbedienung wird mitgehört, damit die Weboberfläche den Zustand aktualisieren kann

> Wichtig: IR ist normalerweise Einbahnstraße. Der ESP32 weiß sicher, was er selbst gesendet hat und was der KY-022 gesehen hat. Er weiß nicht garantiert, ob die Klimaanlage den Befehl wirklich empfangen hat.

## Funktionen

- Weboberfläche auf dem ESP32
- Statusanzeige: Power, Temperatur, Modus, Lüfter, Quelle der letzten Änderung
- API für Home Assistant / eigene Automationen
- IR-Sync: Original-Fernbedienung mithören
- letzter Zustand wird im ESP32-Speicher gesichert
- Reader-Sketch zum Erkennen fremder Fernbedienungen
- Raw-Replay-Beispiel für unbekannte oder noch nicht sauber unterstützte Protokolle
- Dokumentation für gängige A/C-Protokolle

## Hardware

### KY-005 IR-Sender

```text
KY-005 S  -> ESP32 GPIO 4
KY-005 +  -> ESP32 3V3
KY-005 -  -> ESP32 GND
```

### KY-022 IR-Empfänger

```text
KY-022 S  -> ESP32 GPIO 15
KY-022 +  -> ESP32 3V3
KY-022 -  -> ESP32 GND
```

Mehr dazu: [`docs/wiring.md`](docs/wiring.md)

## Schnellstart unter Windows

PowerShell im Projektordner öffnen:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\install.ps1
.\scripts\flash.ps1 COM7
.\scripts\monitor.ps1 COM7
```

Im seriellen Monitor steht danach die IP-Adresse:

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

## Projektstruktur

```text
firmware/
  esp32_ac_ir_hub/       Hauptfirmware: Webinterface + Mitsubishi-Heavy-152 + IR-Sync
  ir_reader/             Fernbedienungen auslesen
  raw_replay_example/    Universal-Fallback: aufgezeichnete Raw-Codes senden

docs/
  wiring.md
  supported-ac-protocols.md
  home-assistant.md
  github-setup.md

scripts/
  install.ps1
  flash.ps1
  monitor.ps1
  publish_to_github.ps1
```

## API

Status:

```text
GET /api/status
```

Setzen:

```text
GET /api/set?power=1
GET /api/set?power=0
GET /api/set?temp=25
GET /api/set?mode=cool
GET /api/set?mode=heat
GET /api/set?mode=dry
GET /api/set?fan=auto
GET /api/send
```

Beispiel:

```text
/api/set?power=1&mode=cool&temp=25&fan=auto
```

## Unterstützte Klimaanlagen

Das Projekt nutzt **IRremoteESP8266** als zentrale IR-Library. Die Library unterstützt ESP8266/ESP32 und viele A/C-Protokolle. Für unbekannte Geräte gibt es zusätzlich den Raw-Copy-Fallback.

Siehe: [`docs/supported-ac-protocols.md`](docs/supported-ac-protocols.md)

## GitHub-Upload

Wenn dieses Paket lokal entpackt ist und du auf GitHub ein leeres Repo angelegt hast:

```powershell
.\scripts\publish_to_github.ps1 -RepoUrl "https://github.com/JannWdl/ESP32-AC-IR-Hub.git"
```

Mehr dazu: [`docs/github-setup.md`](docs/github-setup.md)

## Aktueller gelernter Zustand

Ausgelesen von deiner Fernbedienung:

```text
Protocol  : MITSUBISHI_HEAVY_152
Zustand   : AN, 25 °C
```

Raw-State:

```cpp
uint8_t state[19] = {
  0xAD, 0x51, 0x3C, 0xE5, 0x1A, 0x09, 0xF6, 0x08, 0xF7,
  0x01, 0xFE, 0x72, 0x8D, 0x65, 0x9A, 0x00, 0xFF, 0x80, 0x7F
};
```

## Lizenz

MIT für dieses Projekt.  
Achtung: Die eingebundene IRremoteESP8266-Library hat eine eigene Lizenz. Diese wird nicht in dieses Repo kopiert, sondern per Arduino CLI / PlatformIO installiert.

# Unterstützte A/C-Protokolle

Dieses Projekt nutzt standardmäßig **IRremoteESP8266**.

Die Library unterstützt ESP8266/ESP32, IR-Senden/Empfangen und viele Klimaanlagen-Protokolle. Dieses Repo nutzt:

- native Klasse für deine Anlage: `IRMitsubishiHeavy152Ac`
- Reader-Sketch zum Erkennen des Protokolls
- Raw-Replay als Fallback für Geräte, die nicht sauber nativ funktionieren

## Dein aktuelles Gerät

```text
Protocol  : MITSUBISHI_HEAVY_152
Hersteller: Mitsubishi Heavy Industries
Klasse    : IRMitsubishiHeavy152Ac
Header    : ir_MitsubishiHeavy.h
```

## Gängige Klimaanlagen und passende Header

| Hersteller / Familie | Typische Header in IRremoteESP8266 | Hinweis |
|---|---|---|
| Mitsubishi Heavy Industries | `ir_MitsubishiHeavy.h` | 88-bit und 152-bit Varianten |
| Mitsubishi Electric | `ir_Mitsubishi.h` | andere Protokolle als Mitsubishi Heavy |
| Daikin | `ir_Daikin.h` | viele Daikin-Varianten |
| Panasonic | `ir_Panasonic.h` | mehrere A/C-Modelle |
| Samsung | `ir_Samsung.h` | `SAMSUNG_AC` |
| LG | `ir_LG.h` | mehrere LG-A/C-Varianten |
| Fujitsu / Fujitsu General / OGeneral | `ir_Fujitsu.h` | mehrere Remote-Modelle |
| Midea / Comfee / Trotec / MrCool / Pioneer System | `ir_Midea.h`, teils `ir_Coolix.h` | viele OEM-Geräte nutzen Midea/Coolix |
| Gree / Cooper & Hunter / Kelvinator | `ir_Gree.h`, `ir_Kelvinator.h` | mehrere Gree-Modelle |
| Haier / Mabe | `ir_Haier.h` | mehrere Haier-Varianten |
| Hitachi | `ir_Hitachi.h` | verschiedene Frame-Längen |
| Carrier / Surrey | `ir_Carrier.h` | mehrere Carrier-Varianten |
| Sharp | `ir_Sharp.h` | mehrere A/C-Modelle |
| Sanyo | `ir_Sanyo.h` | mehrere A/C-Modelle |
| TCL / Daewoo / Electrolux-OEM | `ir_Tcl.h` | einige Geräte sind OEM-Varianten |
| Bosch / Durastar | `ir_Bosch.h` | RG10-Fernbedienungen |
| Electra / AEG / AUX / Frigidaire | `ir_Electra.h` | mehrere OEMs |

## Vorgehen für neue Klimaanlage

1. `ir_reader` flashen.
2. Original-Fernbedienung auf KY-022 richten.
3. Taste drücken.
4. Ausgabe notieren:

```text
Protocol  : ...
Code      : ...
uint8_t state[...] = {...};
```

5. Wenn das Protokoll nativ unterstützt wird: passende Klasse verwenden.
6. Wenn nicht: `rawData[]` in `raw_replay_example` übernehmen.

## Warum Raw-Fallback?

Klimaanlagen senden oft den kompletten Zustand:

```text
Power + Modus + Temperatur + Lüfter + Swing + Timer + ...
```

Wenn die native Klasse falsch interpretiert oder ein Sondermodell genutzt wird, ist Raw-Replay der sichere Fallback.

# Verkabelung

## Minimalaufbau

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

## Platzierung

Der KY-022 muss die Original-Fernbedienung sehen.

```text
Fernbedienung -> Klimaanlage
        \
         \-> KY-022 am ESP32 sieht das Signal mit
```

## Wenn die Klimaanlage nicht reagiert

Das KY-005-Modul ist oft schwach. Dann IR-LED mit Transistor treiben:

```text
ESP32 GPIO 4 -- 220Ω -- Basis NPN
Emitter NPN ---------- GND

5V -- 100Ω -- IR-LED Anode
IR-LED Kathode ------- Collector NPN

GND vom 5V-Netzteil und ESP32-GND verbinden
```

Geeignete Transistoren: BC337, 2N2222, S8050.

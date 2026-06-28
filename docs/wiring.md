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

### OLED-Display SSD1306 I2C, optional

Standard in der Firmware:

```text
OLED VCC -> ESP32 3V3
OLED GND -> ESP32 GND
OLED SDA -> ESP32 GPIO 21
OLED SCL -> ESP32 GPIO 22
```

Typische I2C-Adresse:

```text
0x3C
```

Falls im seriellen Monitor `OLED nicht gefunden` steht, ist oft die Adresse `0x3D` statt `0x3C` oder SDA/SCL sind vertauscht.

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

# Verkabelung

## IR

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

## OLED SSD1306 I2C, optional

```text
OLED VCC -> ESP32 3V3
OLED GND -> ESP32 GND
OLED SDA -> ESP32 GPIO 21
OLED SCL -> ESP32 GPIO 22
```

Standardadresse: `0x3C`. Falls das Display schwarz bleibt, testweise `0x3D` setzen.

## DHT11, optional

```text
DHT11 +   -> ESP32 3V3
DHT11 -   -> ESP32 GND
DHT11 OUT -> ESP32 GPIO 26
```

Viele DHT11-Module haben bereits einen Pullup-Widerstand. Beim nackten Sensor einen 4,7kΩ bis 10kΩ Pullup zwischen DATA und 3V3 verwenden.

## MQ-135, optional

Empfohlen für den Analogwert:

```text
MQ-135 VCC -> ESP32 3V3
MQ-135 GND -> ESP32 GND
MQ-135 AO  -> ESP32 GPIO 34
```

GPIO34 ist ADC-fähig und Input-only, also gut für Sensoren geeignet.

Wichtig: Viele MQ-135-Module sind eigentlich für 5V ausgelegt. Wenn du das Modul mit 5V betreibst, darf der AO-Ausgang nicht direkt an den ESP32, weil der ESP32 nur 3,3V am ADC verträgt. Dann brauchst du einen Spannungsteiler.

Beispiel Spannungsteiler für 5V-AO:

```text
MQ AO -- 10kΩ -- ESP32 GPIO34 -- 20kΩ -- GND
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

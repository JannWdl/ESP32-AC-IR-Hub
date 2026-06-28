# Home Assistant Integration

Die Firmware stellt eine einfache HTTP-API bereit.

## REST-Sensor

```yaml
sensor:
  - platform: rest
    name: ESP32 Klima Status
    resource: http://esp32-klima.local/api/status
    value_template: "{{ value_json.power }}"
    json_attributes:
      - temp
      - mode
      - modeText
      - fan
      - fanText
      - source
      - ageMs
```

## REST-Commands

```yaml
rest_command:
  klima_an:
    url: "http://esp32-klima.local/api/set?power=1"

  klima_aus:
    url: "http://esp32-klima.local/api/set?power=0"

  klima_temp:
    url: "http://esp32-klima.local/api/set?temp={{ temp }}"

  klima_kuehlen:
    url: "http://esp32-klima.local/api/set?mode=cool"

  klima_heizen:
    url: "http://esp32-klima.local/api/set?mode=heat"
```

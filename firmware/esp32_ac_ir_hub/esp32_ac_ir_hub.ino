#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <math.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_MitsubishiHeavy.h>

#include "config.h"
#include "learned_state.h"

#ifndef HOSTNAME
#define HOSTNAME "esp32-klima"
#endif

#ifndef ENABLE_AP_FALLBACK
#define ENABLE_AP_FALLBACK 1
#endif

#ifndef AP_SSID
#define AP_SSID "ESP32-Klima-Setup"
#endif

#ifndef AP_PASSWORD
#define AP_PASSWORD "12345678"
#endif

#ifndef OLED_ENABLED
#define OLED_ENABLED 0
#endif

#ifndef DHT_ENABLED
#define DHT_ENABLED 0
#endif

#ifndef DHT_PIN
#define DHT_PIN 26
#endif

#ifndef MQ135_ENABLED
#define MQ135_ENABLED 0
#endif

#ifndef MQ135_PIN
#define MQ135_PIN 34
#endif

#ifndef MQ135_ADC_REF_VOLTAGE
#define MQ135_ADC_REF_VOLTAGE 3.3f
#endif

#ifndef SENSOR_READ_INTERVAL_MS
#define SENSOR_READ_INTERVAL_MS 5000UL
#endif

#if DHT_ENABLED
#include <DHT.h>
#ifndef DHT_TYPE
#define DHT_TYPE DHT11
#endif
DHT dht(DHT_PIN, DHT_TYPE);
#endif

#if OLED_ENABLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifndef OLED_SDA_PIN
#define OLED_SDA_PIN 21
#endif

#ifndef OLED_SCL_PIN
#define OLED_SCL_PIN 22
#endif

#ifndef OLED_ADDRESS
#define OLED_ADDRESS 0x3C
#endif

#ifndef OLED_WIDTH
#define OLED_WIDTH 128
#endif

#ifndef OLED_HEIGHT
#define OLED_HEIGHT 64
#endif

#ifndef OLED_RESET_PIN
#define OLED_RESET_PIN -1
#endif

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN);
bool oledReady = false;
unsigned long lastOledRefreshMs = 0;
#endif

const uint16_t CAPTURE_BUFFER_SIZE = 1024;
const uint8_t IR_TIMEOUT = 50;

WebServer server(80);
Preferences prefs;

IRMitsubishiHeavy152Ac ac(IR_LED_PIN);
IRsend rawIrsend(IR_LED_PIN);
IRrecv irrecv(IR_RECV_PIN, CAPTURE_BUFFER_SIZE, IR_TIMEOUT, true);
decode_results results;

bool hasValidState = false;
bool loadedFromMemory = false;
String lastSource = "Noch kein gültiger IR-Code";
unsigned long lastUpdateMs = 0;
unsigned long ignoreIrUntilMs = 0;

float roomTemperatureC = NAN;
float roomHumidityPercent = NAN;
int mq135Raw = -1;
float mq135Voltage = NAN;
unsigned long lastSensorReadMs = 0;

String jsonEscape(String input) {
  input.replace("\\", "\\\\");
  input.replace("\"", "\\\"");
  input.replace("\n", "\\n");
  input.replace("\r", "\\r");
  return input;
}

String jsonFloat(float value, uint8_t decimals = 1) {
  if (isnan(value)) return "null";
  return String(value, (unsigned int)decimals);
}

String jsonIntOrNull(int value) {
  if (value < 0) return "null";
  return String(value);
}

String modeText(uint8_t mode) {
  switch (mode) {
    case kMitsubishiHeavyAuto: return "Auto";
    case kMitsubishiHeavyCool: return "Kühlen";
    case kMitsubishiHeavyDry:  return "Entfeuchten";
    case kMitsubishiHeavyFan:  return "Lüfter";
    case kMitsubishiHeavyHeat: return "Heizen";
    default: return "Unbekannt";
  }
}

String modeTextAscii(uint8_t mode) {
  switch (mode) {
    case kMitsubishiHeavyAuto: return "Auto";
    case kMitsubishiHeavyCool: return "Kuehlen";
    case kMitsubishiHeavyDry:  return "Entfeucht.";
    case kMitsubishiHeavyFan:  return "Luefter";
    case kMitsubishiHeavyHeat: return "Heizen";
    default: return "Unbekannt";
  }
}

String modeId(uint8_t mode) {
  switch (mode) {
    case kMitsubishiHeavyAuto: return "auto";
    case kMitsubishiHeavyCool: return "cool";
    case kMitsubishiHeavyDry:  return "dry";
    case kMitsubishiHeavyFan:  return "fan";
    case kMitsubishiHeavyHeat: return "heat";
    default: return "unknown";
  }
}

uint8_t modeFromId(String mode) {
  mode.toLowerCase();
  if (mode == "auto") return kMitsubishiHeavyAuto;
  if (mode == "cool") return kMitsubishiHeavyCool;
  if (mode == "dry")  return kMitsubishiHeavyDry;
  if (mode == "fan")  return kMitsubishiHeavyFan;
  if (mode == "heat") return kMitsubishiHeavyHeat;
  return ac.getMode();
}

String fanText(uint8_t fan) {
  switch (fan) {
    case kMitsubishiHeavy152FanAuto:  return "Auto";
    case kMitsubishiHeavy152FanLow:   return "Low";
    case kMitsubishiHeavy152FanMed:   return "Mittel";
    case kMitsubishiHeavy152FanHigh:  return "Hoch";
    case kMitsubishiHeavy152FanMax:   return "Max";
    case kMitsubishiHeavy152FanEcono: return "Eco";
    case kMitsubishiHeavy152FanTurbo: return "Turbo";
    default: return "Unbekannt";
  }
}

String fanId(uint8_t fan) {
  switch (fan) {
    case kMitsubishiHeavy152FanAuto:  return "auto";
    case kMitsubishiHeavy152FanLow:   return "low";
    case kMitsubishiHeavy152FanMed:   return "med";
    case kMitsubishiHeavy152FanHigh:  return "high";
    case kMitsubishiHeavy152FanMax:   return "max";
    case kMitsubishiHeavy152FanEcono: return "eco";
    case kMitsubishiHeavy152FanTurbo: return "turbo";
    default: return "unknown";
  }
}

uint8_t fanFromId(String fan) {
  fan.toLowerCase();
  if (fan == "auto") return kMitsubishiHeavy152FanAuto;
  if (fan == "low" || fan == "1") return kMitsubishiHeavy152FanLow;
  if (fan == "med" || fan == "2") return kMitsubishiHeavy152FanMed;
  if (fan == "high" || fan == "3") return kMitsubishiHeavy152FanHigh;
  if (fan == "max" || fan == "4") return kMitsubishiHeavy152FanMax;
  if (fan == "eco") return kMitsubishiHeavy152FanEcono;
  if (fan == "turbo") return kMitsubishiHeavy152FanTurbo;
  return ac.getFan();
}

String rawStateHex() {
  uint8_t* raw = ac.getRaw();
  String out = "";
  for (uint8_t i = 0; i < kMitsubishiHeavy152StateLength; i++) {
    if (raw[i] < 0x10) out += "0";
    out += String(raw[i], HEX);
    if (i < kMitsubishiHeavy152StateLength - 1) out += " ";
  }
  out.toUpperCase();
  return out;
}

String currentIpText() {
  if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
#if ENABLE_AP_FALLBACK
  return WiFi.softAPIP().toString();
#else
  return "keine IP";
#endif
}

void readSensors(bool force = false) {
  unsigned long now = millis();
  if (!force && now - lastSensorReadMs < SENSOR_READ_INTERVAL_MS) return;
  lastSensorReadMs = now;

#if DHT_ENABLED
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h)) roomHumidityPercent = h;
  if (!isnan(t)) roomTemperatureC = t;
#endif

#if MQ135_ENABLED
  mq135Raw = analogRead(MQ135_PIN);
  mq135Voltage = ((float)mq135Raw / 4095.0f) * MQ135_ADC_REF_VOLTAGE;
#endif
}

String sensorsJson() {
  String json = "{";
  json += "\"dht11\":{";
  json += "\"enabled\":" + String(DHT_ENABLED ? "true" : "false") + ",";
  json += "\"pin\":" + String(DHT_PIN) + ",";
  json += "\"temperatureC\":" + jsonFloat(roomTemperatureC, 1) + ",";
  json += "\"humidityPercent\":" + jsonFloat(roomHumidityPercent, 1);
  json += "},";

  json += "\"mq135\":{";
  json += "\"enabled\":" + String(MQ135_ENABLED ? "true" : "false") + ",";
  json += "\"pin\":" + String(MQ135_PIN) + ",";
  json += "\"raw\":" + jsonIntOrNull(mq135Raw) + ",";
  json += "\"voltage\":" + jsonFloat(mq135Voltage, 3);
  json += "}";
  json += "}";
  return json;
}

#if OLED_ENABLED
void displayBootText(const String& line1, const String& line2 = "") {
  if (!oledReady) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32 AC IR Hub");
  display.println(line1);
  if (line2.length() > 0) display.println(line2);
  display.display();
}

String shortSourceText() {
  if (lastSource.indexOf("Fernbedienung") >= 0) return "Remote";
  if (lastSource.indexOf("Webinterface") >= 0) return "Web";
  if (lastSource.indexOf("Speicher") >= 0) return "Speicher";
  if (lastSource.indexOf("Fallback") >= 0) return "Fallback";
  return "Unbekannt";
}

void updateOled(bool force = false) {
  if (!oledReady) return;
  if (!force && millis() - lastOledRefreshMs < 1000) return;
  lastOledRefreshMs = millis();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Klima ");
  display.print(ac.getPower() ? "AN" : "AUS");
  display.setCursor(72, 0);
  display.print(currentIpText());

  display.setTextSize(3);
  display.setCursor(0, 14);
  display.print(ac.getTemp());
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(modeTextAscii(ac.getMode()));
  display.print(" F:");
  display.print(fanText(ac.getFan()));

  display.setCursor(0, 50);
#if DHT_ENABLED
  display.print("R:");
  if (isnan(roomTemperatureC)) display.print("--"); else display.print(roomTemperatureC, 0);
  display.print("C ");
  if (isnan(roomHumidityPercent)) display.print("--"); else display.print(roomHumidityPercent, 0);
  display.print("% ");
#else
  display.print("DHT:aus ");
#endif

#if MQ135_ENABLED
  display.print("MQ:");
  if (mq135Raw < 0) display.print("--"); else display.print(mq135Raw);
#else
  display.print("MQ:aus");
#endif

  display.setCursor(0, 58);
  display.print("Quelle: ");
  display.print(shortSourceText());

  display.display();
}

void initOled() {
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  oledReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);

  if (!oledReady) {
    Serial.println("OLED nicht gefunden. Prüfe Adresse 0x3C/0x3D und SDA/SCL.");
    return;
  }

  displayBootText("Display gestartet", "Boot...");
  Serial.println("OLED Display aktiv.");
}
#else
void initOled() {}
void updateOled(bool force = false) { (void)force; }
void displayBootText(const String& line1, const String& line2 = "") { (void)line1; (void)line2; }
#endif

bool isValidMhi152State(uint8_t* state) {
  return IRMitsubishiHeavy152Ac::checkZmsSig(state) &&
         IRMitsubishiHeavy152Ac::validChecksum(state);
}

void saveState() {
  prefs.begin("klima", false);
  prefs.putBytes("state", ac.getRaw(), kMitsubishiHeavy152StateLength);
  prefs.putString("source", lastSource);
  prefs.end();
}

bool loadState() {
  uint8_t saved[kMitsubishiHeavy152StateLength];
  prefs.begin("klima", true);
  size_t len = prefs.getBytesLength("state");
  if (len != kMitsubishiHeavy152StateLength) {
    prefs.end();
    return false;
  }
  prefs.getBytes("state", saved, kMitsubishiHeavy152StateLength);
  prefs.end();
  if (!isValidMhi152State(saved)) return false;
  ac.setRaw(saved);
  hasValidState = true;
  loadedFromMemory = true;
  lastSource = "Aus Speicher geladen";
  lastUpdateMs = millis();
  return true;
}

String statusJson() {
  readSensors(false);

  String json = "{";
  json += "\"ok\":true,";
  json += "\"protocol\":\"MITSUBISHI_HEAVY_152\",";
  json += "\"hasValidState\":" + String(hasValidState ? "true" : "false") + ",";
  json += "\"loadedFromMemory\":" + String(loadedFromMemory ? "true" : "false") + ",";
  json += "\"power\":" + String(ac.getPower() ? "true" : "false") + ",";
  json += "\"temp\":" + String(ac.getTemp()) + ",";
  json += "\"mode\":\"" + modeId(ac.getMode()) + "\",";
  json += "\"modeText\":\"" + jsonEscape(modeText(ac.getMode())) + "\",";
  json += "\"fan\":\"" + fanId(ac.getFan()) + "\",";
  json += "\"fanText\":\"" + jsonEscape(fanText(ac.getFan())) + "\",";
  json += "\"source\":\"" + jsonEscape(lastSource) + "\",";
  json += "\"ageMs\":" + String(lastUpdateMs == 0 ? 0 : millis() - lastUpdateMs) + ",";
  json += "\"raw\":\"" + rawStateHex() + "\",";
  json += "\"oledEnabled\":" + String(OLED_ENABLED ? "true" : "false") + ",";
#if OLED_ENABLED
  json += "\"oledReady\":" + String(oledReady ? "true" : "false") + ",";
#else
  json += "\"oledReady\":false,";
#endif
  json += "\"sensors\":" + sensorsJson() + ",";
  json += "\"ip\":\"" + currentIpText() + "\"";
  json += "}";
  return json;
}

void markState(String source) {
  hasValidState = true;
  loadedFromMemory = false;
  lastSource = source;
  lastUpdateMs = millis();
  saveState();
  updateOled(true);

  Serial.println();
  Serial.println("========== KLIMA STATE ==========");
  Serial.print("Quelle: "); Serial.println(lastSource);
  Serial.println(ac.toString());
  Serial.println("Raw: " + rawStateHex());
  Serial.println("================================");
}

void sendCurrentState(String reason) {
  ignoreIrUntilMs = millis() + 1500;
  Serial.println();
  Serial.println("========== IR RAW SEND ==========");
  Serial.println("Quelle: " + reason);
  Serial.println(ac.toString());
  Serial.println("Raw: " + rawStateHex());
  rawIrsend.sendMitsubishiHeavy152(ac.getRaw(), kMitsubishiHeavy152StateLength);
  markState(reason);
  Serial.println("Raw-State gesendet.");
  Serial.println("=================================");
}

void handleIrReceiver() {
  if (!irrecv.decode(&results)) return;
  if (millis() < ignoreIrUntilMs) {
    irrecv.resume();
    return;
  }
  if (results.overflow) {
    Serial.println("IR Empfang: Buffer overflow. CAPTURE_BUFFER_SIZE erhöhen.");
    irrecv.resume();
    return;
  }
  if (results.decode_type == MITSUBISHI_HEAVY_152 && results.bits == kMitsubishiHeavy152Bits) {
    if (isValidMhi152State(results.state)) {
      ac.setRaw(results.state);
      markState("Original-Fernbedienung");
    } else {
      Serial.println("MHI152 empfangen, aber Signatur/Checksumme ungültig.");
    }
  } else {
    Serial.print("IR ignoriert. Typ: ");
    Serial.print(typeToString(results.decode_type));
    Serial.print(" Bits: ");
    Serial.println(results.bits);
  }
  irrecv.resume();
}

void handleApiStatus() { server.send(200, "application/json", statusJson()); }

void handleApiSet() {
  if (server.hasArg("power")) {
    String p = server.arg("power");
    p.toLowerCase();
    if (p == "1" || p == "on" || p == "true" || p == "an") ac.on();
    if (p == "0" || p == "off" || p == "false" || p == "aus") ac.off();
  }
  if (server.hasArg("temp")) {
    int temp = server.arg("temp").toInt();
    temp = constrain(temp, kMitsubishiHeavyMinTemp, kMitsubishiHeavyMaxTemp);
    ac.setTemp((uint8_t)temp);
  }
  if (server.hasArg("mode")) ac.setMode(modeFromId(server.arg("mode")));
  if (server.hasArg("fan")) ac.setFan(fanFromId(server.arg("fan")));
  sendCurrentState("Webinterface");
  server.send(200, "application/json", statusJson());
}

void handleApiSendOnly() {
  sendCurrentState("Webinterface - erneut gesendet");
  server.send(200, "application/json", statusJson());
}

void handleRoot() {
  String html;
  html.reserve(15000);
  html += "<!DOCTYPE html><html lang='de'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Klima Sync</title>";
  html += "<style>:root{color-scheme:dark}body{font-family:Arial,Helvetica,sans-serif;background:#101114;color:#f5f5f5;margin:0;padding:18px}.box{max-width:620px;margin:auto;background:#1b1d23;padding:20px;border-radius:20px;box-shadow:0 0 26px rgba(0,0,0,.45)}h1{margin:0 0 8px 0;font-size:26px}.sub{color:#aeb3c2;margin-bottom:16px;line-height:1.4}.status,.sensor{background:#272a33;border-radius:16px;padding:16px;margin:14px 0 18px 0;line-height:1.65}.temp{font-size:52px;font-weight:800;line-height:1}.grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}.grid3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px}.sgrid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px}.sbox{background:#1b1d23;border-radius:12px;padding:10px}.label{color:#aeb3c2;font-size:13px}.value{font-size:19px;font-weight:700}button{box-sizing:border-box;width:100%;padding:15px;margin-top:10px;border:0;border-radius:14px;font-size:17px;font-weight:700;cursor:pointer}.on{background:#2d9cff;color:#07111f}.off{background:#e74c3c;color:white}.ok{background:#2ecc71;color:#05140b}.small{background:#3a3e4a;color:white}.hint{font-size:13px;color:#aeb3c2;margin-top:18px;line-height:1.5}code{background:#111319;padding:2px 5px;border-radius:6px}@media(max-width:520px){.sgrid{grid-template-columns:1fr}.grid3{grid-template-columns:1fr}}</style>";
  html += "</head><body><div class='box'>";
  html += "<h1>ESP32 Klima Sync</h1><div class='sub'>Mitsubishi Heavy 152 · Webinterface + Fernbedienung mithören · OLED + Sensoren</div>";
  html += "<div class='status'><div id='power'>Status: ...</div><div class='temp'><span id='temp'>--</span>°C</div><div><span id='mode'>...</span> · Lüfter <span id='fan'>...</span></div><div style='margin-top:10px;color:#aeb3c2'>Letzte Änderung: <span id='source'>...</span></div><div style='color:#aeb3c2'>Alter: <span id='age'>...</span></div><div style='color:#aeb3c2'>OLED: <span id='oled'>...</span></div></div>";
  html += "<div class='sensor'><b>Raum & Sensoren</b><div class='sgrid'><div class='sbox'><div class='label'>DHT11 Temperatur</div><div class='value' id='roomTemp'>...</div></div><div class='sbox'><div class='label'>DHT11 Feuchte</div><div class='value' id='roomHum'>...</div></div><div class='sbox'><div class='label'>MQ-135 Rohwert</div><div class='value' id='mq135'>...</div><div class='label' id='mq135v'></div></div></div></div>";
  html += "<div class='grid'><button class='on' onclick=\"setAc('power=1')\">AN</button><button class='off' onclick=\"setAc('power=0')\">AUS</button></div>";
  html += "<div class='grid'><button class='small' onclick='tempDown()'>- 1 °C</button><button class='small' onclick='tempUp()'>+ 1 °C</button></div>";
  html += "<div class='grid3'><button class='small' onclick=\"setAc('mode=cool')\">Kühlen</button><button class='small' onclick=\"setAc('mode=heat')\">Heizen</button><button class='small' onclick=\"setAc('mode=dry')\">Entfeuchten</button></div>";
  html += "<div class='grid3'><button class='small' onclick=\"setAc('fan=auto')\">Fan Auto</button><button class='small' onclick=\"setAc('fan=low')\">Fan Low</button><button class='small' onclick=\"setAc('fan=high')\">Fan High</button></div>";
  html += "<button class='ok' onclick='sendAgain()'>Aktuellen Zustand erneut senden</button>";
  html += "<div class='hint'>API: <code>/api/status</code> · <code>/api/set?power=1&temp=25&mode=cool&fan=auto</code><br>Gesendet wird der aktuelle 19-Byte-MHI152-Raw-State.</div>";
  html += "</div><script>let state={temp:25};function fmt(v,s){return v===null||v===undefined?'--':(Number(v).toFixed(1)+s)}async function refresh(){try{const r=await fetch('/api/status',{cache:'no-store'});state=await r.json();document.getElementById('power').textContent='Status: '+(state.power?'AN':'AUS')+(state.hasValidState?'':' (noch unbekannt)');document.getElementById('temp').textContent=state.temp;document.getElementById('mode').textContent=state.modeText;document.getElementById('fan').textContent=state.fanText;document.getElementById('source').textContent=state.source;document.getElementById('age').textContent=Math.round((state.ageMs||0)/1000)+' s';document.getElementById('oled').textContent=state.oledEnabled?(state.oledReady?'aktiv':'aktiviert, nicht gefunden'):'deaktiviert';const d=state.sensors.dht11;document.getElementById('roomTemp').textContent=d.enabled?fmt(d.temperatureC,' °C'):'deaktiviert';document.getElementById('roomHum').textContent=d.enabled?fmt(d.humidityPercent,' %'):'deaktiviert';const m=state.sensors.mq135;document.getElementById('mq135').textContent=m.enabled?(m.raw===null?'--':m.raw):'deaktiviert';document.getElementById('mq135v').textContent=m.enabled?((m.voltage===null?'--':Number(m.voltage).toFixed(3))+' V'):' ';}catch(e){console.log(e)}}async function setAc(q){await fetch('/api/set?'+q,{cache:'no-store'});await refresh();}async function sendAgain(){await fetch('/api/send',{cache:'no-store'});await refresh();}function tempUp(){let t=(state.temp||25)+1;if(t>31)t=31;setAc('temp='+t);}function tempDown(){let t=(state.temp||25)-1;if(t<17)t=17;setAc('temp='+t);}refresh();setInterval(refresh,1000);</script></body></html>";
  server.send(200, "text/html", html);
}

void startWifi() {
  displayBootText("WLAN verbindet...", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Verbinde mit WLAN");
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WLAN verbunden");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    if (MDNS.begin(HOSTNAME)) {
      Serial.print("mDNS aktiv: http://"); Serial.print(HOSTNAME); Serial.println(".local");
    }
    updateOled(true);
    return;
  }
#if ENABLE_AP_FALLBACK
  Serial.println("WLAN fehlgeschlagen. Starte Access Point.");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP Name: "); Serial.println(AP_SSID);
  Serial.print("AP Passwort: "); Serial.println(AP_PASSWORD);
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
  displayBootText("AP aktiv", WiFi.softAPIP().toString());
#endif
}

void setup() {
  Serial.begin(115200);
  delay(700);
  Serial.println();
  Serial.println("Starte ESP32 AC IR Hub...");
  Serial.println("Protokoll: MITSUBISHI_HEAVY_152");

  initOled();

#if DHT_ENABLED
  dht.begin();
  Serial.print("DHT11 aktiv an GPIO ");
  Serial.println(DHT_PIN);
#else
  Serial.println("DHT11 deaktiviert.");
#endif

#if MQ135_ENABLED
  analogReadResolution(12);
  pinMode(MQ135_PIN, INPUT);
  Serial.print("MQ-135 aktiv an ADC GPIO ");
  Serial.println(MQ135_PIN);
#else
  Serial.println("MQ-135 deaktiviert.");
#endif

  ac.begin();
  rawIrsend.begin();
  ac.stateReset();
  if (!loadState()) {
    ac.setRaw(MHI152_ON_25C);
    hasValidState = true;
    lastSource = "Fallback: gelernter Zustand AN 25 °C";
    lastUpdateMs = millis();
    saveState();
    Serial.println("Fallback-State geladen: AN 25 °C");
  }

  readSensors(true);
  updateOled(true);

  irrecv.enableIRIn();
  Serial.println("IR-Empfänger aktiv.");
  startWifi();
  server.on("/", handleRoot);
  server.on("/api/status", handleApiStatus);
  server.on("/api/set", handleApiSet);
  server.on("/api/send", handleApiSendOnly);
  server.begin();
  Serial.println("Webserver gestartet");
  updateOled(true);
}

void loop() {
  server.handleClient();
  handleIrReceiver();
  readSensors(false);
  updateOled(false);
}

param(
    [string]$Ssid = "",
    [string]$Password = "",
    [int]$IrPin = 4,
    [int]$RecvPin = 15,
    [string]$Hostname = "esp32-klima",
    [switch]$DisableOled,
    [int]$OledSdaPin = 21,
    [int]$OledSclPin = 22,
    [string]$OledAddress = "0x3C"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

function Test-CommandExists {
    param([string]$Name)
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
}

function Escape-CString {
    param([string]$Text)
    if ($null -eq $Text) { return "" }
    return $Text.Replace("\", "\\").Replace('"', '\"')
}

Write-Host ""
Write-Host "=== ESP32 AC IR Hub Installer ===" -ForegroundColor Cyan
Write-Host ""

if (-not (Test-CommandExists "arduino-cli")) {
    Write-Host "arduino-cli nicht gefunden. Versuche Installation ueber winget..." -ForegroundColor Yellow

    if (-not (Test-CommandExists "winget")) {
        throw "winget wurde nicht gefunden. Installiere Arduino CLI manuell oder installiere den Windows App Installer."
    }

    winget install -e --id ArduinoSA.CLI --accept-package-agreements --accept-source-agreements

    if (-not (Test-CommandExists "arduino-cli")) {
        Write-Host ""
        Write-Host "arduino-cli wurde installiert, ist aber in diesem Terminal noch nicht im PATH." -ForegroundColor Yellow
        Write-Host "PowerShell neu oeffnen und nochmal starten: .\scripts\install.ps1"
        exit 1
    }
}

Write-Host "arduino-cli gefunden:" -ForegroundColor Green
arduino-cli version

try { arduino-cli config dump *> $null } catch { arduino-cli config init }

$EspUrl = "https://espressif.github.io/arduino-esp32/package_esp32_index.json"
$ConfigDump = arduino-cli config dump 2>$null

if ($ConfigDump -notmatch [regex]::Escape($EspUrl)) {
    Write-Host "Fuege ESP32 Board Manager URL hinzu..." -ForegroundColor Cyan
    arduino-cli config add board_manager.additional_urls $EspUrl
}

Write-Host "Board-Index aktualisieren..." -ForegroundColor Cyan
arduino-cli core update-index

Write-Host "ESP32 Core installieren/aktualisieren..." -ForegroundColor Cyan
arduino-cli core install esp32:esp32

Write-Host "Library-Index aktualisieren..." -ForegroundColor Cyan
arduino-cli lib update-index

Write-Host "IRremoteESP8266 installieren..." -ForegroundColor Cyan
arduino-cli lib install IRremoteESP8266

Write-Host "OLED-Libraries installieren..." -ForegroundColor Cyan
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "Adafruit GFX Library"

Write-Host ""
Write-Host "WLAN-Konfiguration erstellen..." -ForegroundColor Cyan

if ([string]::IsNullOrWhiteSpace($Ssid)) { $Ssid = Read-Host "WLAN-Name / SSID" }

if ([string]::IsNullOrWhiteSpace($Password)) {
    $SecurePassword = Read-Host "WLAN-Passwort" -AsSecureString
    $BSTR = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($SecurePassword)
    try { $Password = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($BSTR) }
    finally { [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($BSTR) }
}

$EscSsid = Escape-CString $Ssid
$EscPassword = Escape-CString $Password
$EscHostname = Escape-CString $Hostname
$OledEnabled = if ($DisableOled) { 0 } else { 1 }

$ConfigContent = @"
#pragma once

#define WIFI_SSID "$EscSsid"
#define WIFI_PASSWORD "$EscPassword"

#define IR_LED_PIN $IrPin
#define IR_RECV_PIN $RecvPin

#define HOSTNAME "$EscHostname"

#define ENABLE_AP_FALLBACK 1
#define AP_SSID "ESP32-Klima-Setup"
#define AP_PASSWORD "12345678"

#define OLED_ENABLED $OledEnabled
#define OLED_SDA_PIN $OledSdaPin
#define OLED_SCL_PIN $OledSclPin
#define OLED_ADDRESS $OledAddress
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET_PIN -1
"@

$HubConfigPath = Join-Path $Root "firmware\esp32_ac_ir_hub\config.h"
$RawConfigPath = Join-Path $Root "firmware\raw_replay_example\config.h"
Set-Content -Path $HubConfigPath -Value $ConfigContent -Encoding UTF8
Set-Content -Path $RawConfigPath -Value $ConfigContent -Encoding UTF8

Write-Host ""
Write-Host "config.h wurde erstellt:" -ForegroundColor Green
Write-Host "  $HubConfigPath"
Write-Host "  $RawConfigPath"

if ($DisableOled) {
    Write-Host "OLED: deaktiviert" -ForegroundColor Yellow
} else {
    Write-Host "OLED: aktiviert, SDA=$OledSdaPin, SCL=$OledSclPin, Adresse=$OledAddress" -ForegroundColor Green
}

Write-Host ""
Write-Host "Angeschlossene Boards/Ports:" -ForegroundColor Cyan
arduino-cli board list

Write-Host ""
Write-Host "Fertig. Danach:"
Write-Host "  .\scripts\flash.ps1 COM7"
Write-Host ""

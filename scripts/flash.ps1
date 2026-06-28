param(
    [Parameter(Position=0)]
    [string]$Port = "",

    [ValidateSet("hub", "reader", "raw")]
    [string]$Target = "hub",

    [string]$Board = "esp32:esp32:esp32",

    [switch]$NoHardwarePrompt
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

function Test-CommandExists {
    param([string]$Name)
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
}

function Read-YesNo {
    param(
        [string]$Question,
        [bool]$Default = $true
    )

    $suffix = if ($Default) { "[J/n]" } else { "[j/N]" }
    $answer = Read-Host "$Question $suffix"

    if ([string]::IsNullOrWhiteSpace($answer)) { return $Default }
    $answer = $answer.Trim().ToLower()
    return ($answer -eq "j" -or $answer -eq "ja" -or $answer -eq "y" -or $answer -eq "yes")
}

function Set-ConfigDefine {
    param(
        [string]$Path,
        [string]$Name,
        [string]$Value
    )

    $content = Get-Content -Path $Path -Raw
    $line = "#define $Name $Value"

    if ($content -match "(?m)^#define\s+$Name\s+.*$") {
        $content = [regex]::Replace($content, "(?m)^#define\s+$Name\s+.*$", $line)
    } else {
        $content = $content.TrimEnd() + "`r`n" + $line + "`r`n"
    }

    Set-Content -Path $Path -Value $content -Encoding UTF8
}

function Configure-Hardware {
    param([string]$ConfigPath)

    Write-Host ""
    Write-Host "Hardware-Konfiguration fuer diesen Flash:" -ForegroundColor Cyan

    $hasOled = Read-YesNo "OLED Display angeschlossen?" $true
    $hasMq135 = Read-YesNo "MQ-135 Sensor angeschlossen?" $true
    $hasDht11 = Read-YesNo "DHT11 Sensor angeschlossen?" $true

    Set-ConfigDefine $ConfigPath "OLED_ENABLED" ($(if ($hasOled) { "1" } else { "0" }))
    Set-ConfigDefine $ConfigPath "MQ135_ENABLED" ($(if ($hasMq135) { "1" } else { "0" }))
    Set-ConfigDefine $ConfigPath "DHT_ENABLED" ($(if ($hasDht11) { "1" } else { "0" }))

    Write-Host ""
    Write-Host "Gespeichert in config.h:" -ForegroundColor Green
    Write-Host "  OLED_ENABLED  = $(if ($hasOled) { 1 } else { 0 })"
    Write-Host "  MQ135_ENABLED = $(if ($hasMq135) { 1 } else { 0 })"
    Write-Host "  DHT_ENABLED   = $(if ($hasDht11) { 1 } else { 0 })"
}

if (-not (Test-CommandExists "arduino-cli")) {
    throw "arduino-cli wurde nicht gefunden. Starte zuerst .\scripts\install.ps1"
}

switch ($Target) {
    "hub" {
        $SketchPath = Join-Path $Root "firmware\esp32_ac_ir_hub"
        $ConfigPath = Join-Path $SketchPath "config.h"
        if (-not (Test-Path $ConfigPath)) { throw "config.h fehlt. Starte zuerst .\scripts\install.ps1" }
        Write-Host "Flashe ESP32 AC IR Hub..." -ForegroundColor Cyan

        if (-not $NoHardwarePrompt) {
            Configure-Hardware $ConfigPath
        }
    }
    "reader" {
        $SketchPath = Join-Path $Root "firmware\ir_reader"
        Write-Host "Flashe IR-Reader..." -ForegroundColor Cyan
    }
    "raw" {
        $SketchPath = Join-Path $Root "firmware\raw_replay_example"
        $ConfigPath = Join-Path $SketchPath "config.h"
        if (-not (Test-Path $ConfigPath)) { throw "config.h fehlt. Starte zuerst .\scripts\install.ps1" }
        Write-Host "Flashe Raw-Replay-Beispiel..." -ForegroundColor Cyan
    }
}

if ([string]::IsNullOrWhiteSpace($Port)) {
    Write-Host ""
    Write-Host "Keine COM-Schnittstelle angegeben. Gefundene Ports:" -ForegroundColor Yellow
    arduino-cli board list
    Write-Host ""
    $Port = Read-Host "COM-Port eingeben, z.B. COM7"
}

Write-Host ""
Write-Host "Board : $Board"
Write-Host "Port  : $Port"
Write-Host "Target: $Target"
Write-Host "Sketch: $SketchPath"
Write-Host ""

Write-Host "Kompiliere..." -ForegroundColor Cyan
arduino-cli compile --fqbn $Board "$SketchPath"

Write-Host ""
Write-Host "Uploade auf $Port..." -ForegroundColor Cyan
arduino-cli upload -p $Port --fqbn $Board "$SketchPath"

Write-Host ""
Write-Host "Flash fertig." -ForegroundColor Green
Write-Host "Seriellen Monitor starten:"
Write-Host "  .\scripts\monitor.ps1 $Port"
Write-Host ""

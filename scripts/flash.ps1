param(
    [Parameter(Position=0)]
    [string]$Port = "",

    [ValidateSet("hub", "reader", "raw")]
    [string]$Target = "hub",

    [string]$Board = "esp32:esp32:esp32"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

function Test-CommandExists {
    param([string]$Name)
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
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

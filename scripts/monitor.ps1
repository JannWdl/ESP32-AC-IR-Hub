param(
    [Parameter(Position=0)]
    [string]$Port = "",

    [int]$Baud = 115200
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Port)) {
    Write-Host "Gefundene Ports:" -ForegroundColor Cyan
    arduino-cli board list
    Write-Host ""
    $Port = Read-Host "COM-Port eingeben, z.B. COM7"
}

Write-Host ""
Write-Host "Starte seriellen Monitor auf $Port mit $Baud Baud." -ForegroundColor Cyan
Write-Host "Beenden: STRG+C"
Write-Host ""

arduino-cli monitor -p $Port -c baudrate=$Baud

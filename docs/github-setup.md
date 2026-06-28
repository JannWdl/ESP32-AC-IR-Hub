# GitHub Setup

## Option A: mit Push-Skript

1. Auf GitHub ein neues, leeres Repository anlegen, z.B.:

```text
ESP32-AC-IR-Hub
```

2. Dieses Projekt entpacken.

3. PowerShell im Projektordner öffnen:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\publish_to_github.ps1 -RepoUrl "https://github.com/JannWdl/ESP32-AC-IR-Hub.git"
```

## Option B: manuell

```powershell
git init
git add .
git commit -m "Initial ESP32 AC IR Hub"
git branch -M main
git remote add origin https://github.com/JannWdl/ESP32-AC-IR-Hub.git
git push -u origin main
```

## Wichtig

`config.h` enthält dein WLAN-Passwort und ist in `.gitignore`.
Nur `config.example.h` wird hochgeladen.

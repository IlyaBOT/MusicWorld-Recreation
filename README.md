# MusicWorld Recreation

## Table of Contents

<details>

   <summary>Contents</summary>

1. [Project progress](#project-progress)
1. [Controls](#controls)
1. [Build on Windows (Visual Studio)](#build-on-windows-visual-studio)
   1. [Requirements:](#requirements)
   1. [Build and run in PowerShell:](#build-and-run-in-powershell)
1. [Build on macOS](#build-on-macos)
   1. [Requirements:](#requirements)
   1. [Build:](#build)
   1. [Run:](#run)
1. [Build on Linux](#build-on-linux)
   1. [Requirements](#requirements)
      1. [Ubuntu / Debian](#ubuntu--debian)
      1. [Fedora](#fedora)
      1. [Arch Linux](#arch-linux)
   1. [Build](#build)
   1. [Run](#run)

</details>

Music World recreation for Android, iOS and PC, using C++17 + raylib + CMake.
Runs on macOS, Linux and Windows only right now. Mobile support will be added later.

## Project progress
- Virtual resolution **240x400** (scales with letterboxing).
- Screens:
  - Splash (LG Electronics and Com2us logos) 
  - Title (Music World)
  - Main Menu
  - Mode select + Difficulty select
  - Music Player (track carousel and track playing + swipe support + fake (temporary) EQ)
  - Help
  - Settings (vibration toggle + 2 timing sliders. Language selection will be added later.)
  - Records
- Locks:
  - Party mode locked until `storyCompleted=true`
  - Hard difficulty locked until `storyCompleted=true`
- Game profile save: `save/profile.cfg`

## Controls
- Mouse: click; drag-release = swipe left/right.
- Keyboard:
  - `Esc`/`Backspace` = back
  - `F1` = debug overlay

---

## Build on Windows (Visual Studio)
### Requirements:
- Visual Studio 2022 (Desktop development with C++)
- CMake (comes with VS)

### Build and run in PowerShell:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\build\Release\mw_recreation.exe
```

---

## Build on macOS
### Requirements:
- Xcode Command Line Tools: `xcode-select --install`
- CMake: `brew install cmake`

### Build:
```bash
cmake --fresh -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Run:
```bash
./build/mw_recreation
```

---

## Build on Linux
### Requirements
Install a C/C++ toolchain + CMake + Git, and system libraries needed by raylib (audio + OpenGL/Mesa + windowing).
#### Ubuntu / Debian
```bash
sudo apt update
sudo apt install build-essential git cmake libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

#### Fedora
```bash
sudo dnf install gcc gcc-c++ make git cmake alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel libatomic
```

#### Arch Linux
```bash
sudo pacman -S --needed base-devel git cmake alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
```

### Build
```bash
cmake --fresh -S . -B build -DCMAKE_BUILD_TYPE=Release +cmake --build build -j
```

### Run
```bash
./build/mw_recreation
```

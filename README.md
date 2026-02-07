# MusicWorld Recreation

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
Requirements:
- Visual Studio 2022 (Desktop development with C++)
- CMake (comes with VS)

Build and run in PowerShell:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\build\Release\mw_recreation.exe
```

---

## Build on macOS
Requirements:
- Xcode Command Line Tools: `xcode-select --install`
- CMake: `brew install cmake`

Build:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Run:
```bash
./build/mw_recreation
```

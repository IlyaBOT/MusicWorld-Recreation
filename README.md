# Music World Recreation

Music World recreation for Android, iOS and PC, using C++17 + raylib + CMake.
Desktop build is fully supported. Android project is in `android/` directory.

## Project progress
- Virtual resolution **240x400** (scales with letterboxing).
- Screens:
  - Splash (LG Electronics and Com2us logos) 
  - Title (Music World)
  - Main Menu
  - Mode select + Difficulty select
  - Music Player (track carousel with swipe support + fake (temporary) EQ)
  - Help [X]
  - Settings (vibration toggle + music toggle + music remix toggle + 2 timing sliders. Language selection will be added later.)
  - Records [X]
- Locks:
  - Party mode locked until `storyCompleted=true`
  - Hard difficulty locked until `storyCompleted=true`
- Game profile save path:
  - Desktop: `save/profile.cfg`
  - Android/iOS: app internal storage

## Controls
- Mouse: click; drag-release = swipe left/right.
- Keyboard:
  - `Esc`/`Backspace` = back
  - `F1` = debug overlay

---

## Build on Windows (Visual Studio)
### Requirements:
- Visual Studio 2022 (Desktop development with C++)
- CMake

### Build and run in PowerShell:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
build\Release\mw_recreation.exe
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
cmake --fresh -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Run
```bash
./build/mw_recreation
```

---

## Build on Android
### Requirements
- Android Studio (with SDK + NDK + CMake installed)

### Notes
- Native C++ is built from the root `CMakeLists.txt`.
- APK assets are taken directly from repo folder `assets/` via `sourceSets`; no manual copy step.

### Build in Android Studio
1. Open `android/` as project.
2. Wait for Gradle sync.
3. Build `app` (`Build > Make Project`) or run on device.

### Build from CLI (inside `android/`)
```bash
./gradlew assembleDebug
```

APK output:
`android/app/build/outputs/apk/debug/app-debug.apk`

---

## Runtime arguments (desktop)
- `--no-splash`: skip splash and start from title.
- `--no-title`: skip splash/title and open main menu directly.

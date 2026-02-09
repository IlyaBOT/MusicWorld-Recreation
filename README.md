# Music World Recreation

Music World recreation for Android, iOS, PC and Mac, using C++17 with raylib and CMake. Desktop project is in `build/` directory, Android project is in `android/` directory, *iOS version WIP*.

## Screenshots
<p>
  <img src="screenshots/2026-02-09%2020.58.03.jpg" width="240" height="400" alt="Screenshot 1">
  <img src="screenshots/2026-02-09%2020.58.48.jpg" width="240" height="400" alt="Screenshot 2">
  <img src="screenshots/2026-02-09%2020.56.59.jpg" width="180" height="500" alt="Screenshot 3">
</p>

## Supported platforms
- Android: **7.0+** (API 24, `minSdk 24`)
- Linux: **x86_64**, Ubuntu 20.04+ (or equivalent modern distro with OpenGL + ALSA)
- macOS X: **10.15+** (Intel, x64)
- Windows: **10+** (x64)
- iOS: **13.0+** (WIP)

## Project progress
- Virtual resolution **240x400** (scales with letterboxing).
- Screens:
  - Splash Screen (LG Electronics and Com2us logos) [✓]
  - Title Screen (Music World logo) [✓]
  - Main Menu [✓]
  - Game mode select + Difficulty select scerens [✓]
  - Music Player (Fully functional track carousel with swipe support + Working 10-band EQ) [✓]
  - Settings (vibration toggle + music toggle + music remix toggle + 2 timing sliders. Language selection will be added later.) [✓]
  - Records [✓]
  - Tutorial [X]
  - Levels:
  - - Rythm game logic (Timer with accuracy assessment system and rewards) [X]
  - - Game progress logic ("Track" map with the ability to unlock locations as you progress through the game) [X]
  - - Animation and “throwing” Tuneys to the rhythm of the tap point [X]
  - - Boss Fights [X]
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

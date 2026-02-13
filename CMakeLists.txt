cmake_minimum_required(VERSION 3.18)
set(MW_BUILD_VERSION "0.2")
if(EXISTS "${CMAKE_SOURCE_DIR}/VERSION")
  file(READ "${CMAKE_SOURCE_DIR}/VERSION" MW_BUILD_VERSION_RAW)
  string(STRIP "${MW_BUILD_VERSION_RAW}" MW_BUILD_VERSION)
  if(MW_BUILD_VERSION STREQUAL "")
    set(MW_BUILD_VERSION "0.2")
  endif()
endif()
project(mw_recreation VERSION ${MW_BUILD_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MW_USE_SYSTEM_RAYLIB "Use system-installed raylib instead of FetchContent" OFF)

if(MW_USE_SYSTEM_RAYLIB)
  find_package(raylib 5.5 REQUIRED)
else()
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
  if (ANDROID)
    # raylib expects this cache variable to configure Android backend/libs.
    set(PLATFORM Android CACHE STRING "raylib target platform" FORCE)
  endif()
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
  )
  FetchContent_MakeAvailable(raylib)
endif()

set(MW_RECREATION_SOURCES
  src/main.cpp

  src/core/App.cpp
  src/core/Assets.cpp
  src/core/Input.cpp
  src/core/Profile.cpp
  src/core/VirtualScreen.cpp
  src/game/HealthSystem.cpp
  src/game/ScoreComboSystem.cpp
  src/game/RhythmSystem.cpp
  src/game/TuneySystem.cpp

  src/ui/Widgets.cpp

  src/screens/splash/SplashScreen.cpp
  src/screens/title/TitleScreen.cpp
  src/screens/debug/DebugWorldScreen.cpp

  src/screens/menu/main/MenuMainScreen.cpp
  src/screens/menu/mode/ModeSelectScreen.cpp
  src/screens/menu/difficulty/DifficultyScreen.cpp
  src/screens/menu/player/PlayerScreen.cpp
  src/screens/menu/help/HelpScreen.cpp
  src/screens/menu/settings/SettingsScreen.cpp
  src/screens/menu/records/RecordsFlow.cpp

  src/screens/gameplay/GameMapScreen.cpp
  src/screens/gameplay/GameplayPlaceholderScreen.cpp
)

if (ANDROID)
  add_library(mw_recreation SHARED ${MW_RECREATION_SOURCES})
else()
  add_executable(mw_recreation ${MW_RECREATION_SOURCES})
endif()

if (WIN32 AND NOT ANDROID)
  set(MW_APP_ICON_ICO "${CMAKE_SOURCE_DIR}/assets/sprites/icons/icon.ico")
  if (EXISTS "${MW_APP_ICON_ICO}")
    file(TO_NATIVE_PATH "${MW_APP_ICON_ICO}" MW_APP_ICON_ICO_NATIVE)
    string(REPLACE "\\" "\\\\" MW_APP_ICON_ICO_NATIVE_ESCAPED "${MW_APP_ICON_ICO_NATIVE}")
    configure_file(
      "${CMAKE_SOURCE_DIR}/tools/windows_app_icon.rc.in"
      "${CMAKE_BINARY_DIR}/mw_app_icon.rc"
      @ONLY
    )
    target_sources(mw_recreation PRIVATE "${CMAKE_BINARY_DIR}/mw_app_icon.rc")
  endif()
endif()

target_include_directories(mw_recreation PRIVATE src)
target_link_libraries(mw_recreation PRIVATE raylib)
target_compile_definitions(mw_recreation PRIVATE MW_BUILD_VERSION="${MW_BUILD_VERSION}")

if (ANDROID)
  # Keep NativeActivity entrypoint from raylib/native_app_glue in the final .so.
  target_compile_definitions(mw_recreation PRIVATE PLATFORM_ANDROID)
  target_link_options(mw_recreation PRIVATE "LINKER:-u,ANativeActivity_onCreate")
endif()

if (APPLE)
  target_link_libraries(mw_recreation PRIVATE "-framework Cocoa" "-framework IOKit" "-framework OpenGL")
endif()

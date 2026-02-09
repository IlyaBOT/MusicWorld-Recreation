#include "raylib.h"
#include "core/App.h"
#include "core/BuildVersion.h"
#include "screens/splash/SplashScreen.h"
#include "screens/title/TitleScreen.h"
#include "screens/menu/main/MenuMainScreen.h"
#include <memory>
#include <string>
#include <string_view>

int main(int argc, char** argv) {
    bool noSplash = false;
    bool noTitle = false;
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--no-splash") noSplash = true;
        else if (arg == "--no-title") noTitle = true;
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    const std::string windowTitle = std::string("Music World - Recreation v") + buildinfo::kVersion;
    InitWindow(432, 720, windowTitle.c_str());
    SetTargetFPS(60);

    App app;
    app.init();
    if (noTitle) app.push(std::make_unique<MenuMainScreen>());
    else if (noSplash) app.push(std::make_unique<TitleScreen>());
    else app.push(std::make_unique<SplashScreen>());

    while (!WindowShouldClose() && !app.quitRequested()) {
        app.runOneFrame();
    }

    app.shutdown();
    CloseWindow();
    return 0;
}

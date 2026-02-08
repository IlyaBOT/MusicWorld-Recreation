#include "raylib.h"
#include "core/App.h"
#include "screens/splash/SplashScreen.h"
#include <memory>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(432, 720, "Music World - Recreation");
    SetTargetFPS(60);

    App app;
    app.init();
    app.push(std::make_unique<SplashScreen>());

    while (!WindowShouldClose() && !app.quitRequested()) {
        app.runOneFrame();
    }

    app.shutdown();
    CloseWindow();
    return 0;
}

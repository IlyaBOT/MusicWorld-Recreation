#include "raylib.h"
#include "core/App.h"
#include "screens/splash/SplashScreen.h"
#include <memory>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(900, 600, "Music World - Recreation (skeleton)");
    SetTargetFPS(60);

    App app;
    app.init();
    app.push(std::make_unique<SplashScreen>());

    while (!WindowShouldClose()) {
        app.runOneFrame();
    }

    app.shutdown();
    CloseWindow();
    return 0;
}

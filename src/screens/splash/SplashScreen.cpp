#include "screens/splash/SplashScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/title/TitleScreen.h"
#include <memory>

void SplashScreen::update(const UpdateContext& ctx) {
    timer_ += ctx.dt;
    auto st = ctx.input->state();
    if ((st.pressed && st.inViewport) || timer_ > 1.4f) {
        ctx.app->replace(std::make_unique<TitleScreen>());
    }
}

void SplashScreen::draw(const DrawContext& ctx) {
    ClearBackground(RAYWHITE);
    auto logo = ctx.assets->tex("sprites/title/1276.png").tex;
    DrawCentered(logo, 120, 200);
    if (!logo.id || ctx.debug) DrawTextCentered("com2us (missing sprites/title/1276.png)", 120, 360, 12, GRAY);
}

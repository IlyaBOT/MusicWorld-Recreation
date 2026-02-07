#include "screens/splash/SplashScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/title/TitleScreen.h"
#include <memory>

namespace {
constexpr float kCom2UsDurationSec = 2.0f;
constexpr float kLgDurationSec = 2.0f;
constexpr float kSplashDurationSec = kCom2UsDurationSec + kLgDurationSec;
}

void SplashScreen::update(const UpdateContext& ctx) {
    timer_ += ctx.dt;
    auto st = ctx.input->state();
    if ((st.pressed && st.inViewport) || timer_ > kSplashDurationSec) {
        ctx.app->replace(std::make_unique<TitleScreen>());
    }
}

void SplashScreen::draw(const DrawContext& ctx) {
    ClearBackground(WHITE);
    bool showLg = timer_ >= kCom2UsDurationSec;
    auto logo = ctx.assets->tex(showLg ? "sprites/UI/Title/lg.png" : "sprites/UI/Title/com2us.png").tex;
    DrawCentered(logo, 120, 200);
    if (!logo.id || ctx.debug) {
        DrawTextCentered(showLg
            ? "LG Electronics (missing sprites/UI/Title/lg.png)"
            : "com2us (missing sprites/UI/Title/com2us.png)",
            120, 360, 12, GRAY);
    }
}

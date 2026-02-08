#include "screens/splash/SplashScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/title/TitleScreen.h"
#include <memory>

namespace {
constexpr float kLgDurationSec = 2.0f;
constexpr float kCom2UsDurationSec = 2.0f;
constexpr float kSplashDurationSec = kLgDurationSec + kCom2UsDurationSec;
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
    bool showCom2us = timer_ >= kLgDurationSec;
    auto logo = ctx.assets->tex(showCom2us ? "sprites/UI/Title/com2us.png" : "sprites/UI/Title/lg.png").tex;
    DrawCentered(logo, 120, 200);
    int textY = (ctx.vs ? ctx.vs->vh : 400) - 16;
    if (!logo.id || ctx.debug) {
        DrawTextCentered(showCom2us
            ? "com2us (missing sprites/UI/Title/com2us.png)"
            : "LG Electronics (missing sprites/UI/Title/lg.png)",
            120, textY, 12, GRAY);
    }
}

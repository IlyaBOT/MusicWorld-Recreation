#include "screens/title/TitleScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/main/MenuMainScreen.h"
#include <memory>
#include <string>

namespace {
constexpr const char* kBgFrames[] = {
    "sprites/LevelBackgrounds/0577.png",
    "sprites/LevelBackgrounds/0578.png",
    "sprites/LevelBackgrounds/0579.png",
};
constexpr int kBgFrameCount = (int)(sizeof(kBgFrames) / sizeof(kBgFrames[0]));
constexpr float kBgAnimFps = 8.0f;

constexpr const char* kTapIdlePrimary = "sprites/UI/Menu/Buttons/tap-btn.png";
constexpr const char* kTapIdleFallback = "sprites/UI/Menu/Russian/tap-btn.png";
constexpr const char* kTapActivePrimary = "sprites/UI/Menu/Buttons/tap-btn_selected.png";
constexpr const char* kTapActiveFallback = "sprites/UI/Menu/Russian/tap-btn_selected.png";

std::string ResolvePath(const char* primary, const char* fallback) {
    if (FileExists(Assets::A(primary).c_str())) return primary;
    if (FileExists(Assets::A(fallback).c_str())) return fallback;
    return primary;
}

void DrawTiled(Texture2D tile, int vw, int vh) {
    if (!tile.id || tile.width <= 0 || tile.height <= 0) return;
    for (int y = 0; y < vh; y += tile.height) {
        for (int x = 0; x < vw; x += tile.width) {
            DrawTexture(tile, x, y, WHITE);
        }
    }
}
}

void TitleScreen::onEnter() {
    bgAnimTimer_ = 0.0f;

    tapButton_ = {};
    tapButton_.rect = {11.0f, 339.0f, 218.0f, 26.0f};
    tapButton_.bgRel = ResolvePath(kTapIdlePrimary, kTapIdleFallback);
    tapButton_.bgRelActive = ResolvePath(kTapActivePrimary, kTapActiveFallback);
}

void TitleScreen::update(const UpdateContext& ctx) {
    bgAnimTimer_ += ctx.dt;

    auto st = ctx.input->state();
    bool clickedTap = tapButton_.update(
        st.mouseV,
        st.down && st.inViewport,
        st.pressed && st.inViewport,
        st.released && st.inViewport
    );
    bool pressedAnywhere = st.pressed && st.inViewport;
    if (clickedTap || pressedAnywhere || st.keySelect) {
        ctx.app->replace(std::make_unique<MenuMainScreen>());
        ctx.app->playSfx("sounds/MenuSelect.wav");
    }
}

void TitleScreen::draw(const DrawContext& ctx) {
    int vw = ctx.vs ? ctx.vs->vw : 240;
    int vh = ctx.vs ? ctx.vs->vh : 400;
    int frame = ((int)(bgAnimTimer_ * kBgAnimFps)) % kBgFrameCount;

    ClearBackground(BLACK);
    DrawTiled(ctx.assets->tex(kBgFrames[frame]).tex, vw, vh);

    auto logo = ctx.assets->tex("sprites/UI/Title/logo.png").tex;
    DrawCentered(logo, vw * 0.5f, vh * 0.5f);

    Texture2D tap = ctx.assets->tex(tapButton_.pressed ? tapButton_.bgRelActive : tapButton_.bgRel).tex;
    float tapCx = tapButton_.rect.x + tapButton_.rect.width * 0.5f;
    float tapCy = tapButton_.rect.y + tapButton_.rect.height * 0.5f;
    DrawCentered(tap, tapCx, tapCy);

    if (ctx.debug) {
        if (!logo.id) DrawText("missing sprites/UI/Title/logo.png", 6, 6, 12, YELLOW);
        DrawText(TextFormat("title bg frame: %d", frame), 6, 20, 12, YELLOW);
    }
}

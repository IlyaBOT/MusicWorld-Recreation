#include "screens/debug/DebugWorldScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/main/MenuMainScreen.h"
#include <algorithm>
#include <cmath>
#include <memory>

namespace {
constexpr const char* kBgTile = "sprites/LevelBackgrounds/debugWorld_bg.png";

constexpr int kVirtualW = 240;
constexpr int kVirtualH = 400;

constexpr float kBarX = 8.0f;
constexpr float kBarY = 304.0f;
constexpr float kBarW = 232.0f;
constexpr float kBarH = 92.0f;
constexpr float kBarPadX = 8.0f;
constexpr float kBarPadY = 7.0f;
constexpr float kButtonW = 36.0f;
constexpr float kButtonH = 36.0f;
constexpr float kButtonGapY = 6.0f;
constexpr float kButtonGapX = 8.0f;
constexpr int kRows = 2;
constexpr int kCols = 5;
constexpr float kScrollSpeed = 20.0f;
constexpr float kHpBarY = 16.0f;
constexpr float kHpSegmentsStartX = 29.0f;
constexpr float kHpSegmentsTopPad = 1.0f;
constexpr float kHpSegmentGapX = 1.0f;
constexpr float kHpSegmentsOffsetY = 6.0f;
constexpr float kHpBlinkFrequencyHz = 2.0f;
constexpr float kGameOverGapY = 6.0f;

void drawTiled(Texture2D tile, int vw, int vh) {
    if (!tile.id || tile.width <= 0 || tile.height <= 0) return;
    for (int y = 0; y < vh; y += tile.height) {
        for (int x = 0; x < vw; x += tile.width) {
            DrawTexture(tile, x, y, WHITE);
        }
    }
}
}

void DebugWorldScreen::onEnter() {
    bgAnimTimer_ = 0.0f;
    health_.reset();
    tuneysLoaded_ = false;
    assetsRef_ = nullptr;
    barRect_ = {kBarX, kBarY, kBarW, kBarH};
    buttons_.clear();
    buttons_.resize(kRows * kCols);
    for (auto& b : buttons_) {
        b.bgRel = "sprites/UI/Menu/Buttons/1391.png";
        b.bgRelActive = "sprites/UI/Menu/Buttons/1395.png";
    }
    if (!buttons_.empty()) {
        buttons_.front().labelRel = "sprites/UI/Menu/Buttons/0915.png";
        buttons_.front().labelRelActive = "sprites/UI/Menu/Buttons/0915.png";
    }
    if (buttons_.size() > (size_t)kCols) {
        buttons_[(size_t)kCols + 0].labelRel = "sprites/UI/Fonts/1003.png";
        buttons_[(size_t)kCols + 1].labelRel = "sprites/UI/Fonts/1004.png";
    }
    scrollX_ = 0.0f;
    draggingScroll_ = false;
    dragStartMouseX_ = 0.0f;
    dragStartScrollX_ = 0.0f;
    rebuildButtons();
}

void DebugWorldScreen::onExit() {
    if (tuneysLoaded_ && assetsRef_) assetsRef_->unloadTuneySprites();
    tuneysLoaded_ = false;
    assetsRef_ = nullptr;
}

void DebugWorldScreen::rebuildButtons() {
    float contentX = barRect_.x + kBarPadX;
    float contentY = barRect_.y + kBarPadY;

    float contentW = kCols * kButtonW + (kCols - 1) * kButtonGapX;
    float visibleW = barRect_.width - kBarPadX * 2.0f;
    maxScrollX_ = std::max(0.0f, contentW - visibleW);
    scrollX_ = std::clamp(scrollX_, 0.0f, maxScrollX_);

    for (int row = 0; row < kRows; ++row) {
        for (int col = 0; col < kCols; ++col) {
            int idx = row * kCols + col;
            float x = contentX + col * (kButtonW + kButtonGapX) - scrollX_;
            float y = contentY + row * (kButtonH + kButtonGapY);
            buttons_[idx].rect = {x, y, kButtonW, kButtonH};
        }
    }
}

void DebugWorldScreen::update(const UpdateContext& ctx) {
    if (!tuneysLoaded_ && ctx.assets) {
        assetsRef_ = ctx.assets;
        assetsRef_->preloadTuneySprites();
        tuneysLoaded_ = true;
    }
    bgAnimTimer_ += ctx.dt;

    auto st = ctx.input->state();
    if (st.keyBack) {
        ctx.app->clearAndPush(std::make_unique<MenuMainScreen>());
        ctx.app->playSfx("sounds/MenuBack.wav");
        return;
    }

    bool inBar = st.inViewport && CheckCollisionPointRec(st.mouseV, barRect_);

    if (inBar) {
        float wheel = GetMouseWheelMove();
        if (std::fabs(wheel) > 0.001f) {
            scrollX_ = std::clamp(scrollX_ - wheel * kScrollSpeed, 0.0f, maxScrollX_);
            rebuildButtons();
        }
    }

    if (st.pressed && inBar) {
        draggingScroll_ = true;
        dragStartMouseX_ = st.mouseV.x;
        dragStartScrollX_ = scrollX_;
    }
    if (st.released) {
        draggingScroll_ = false;
    }
    if (draggingScroll_ && st.down && inBar) {
        float dx = st.mouseV.x - dragStartMouseX_;
        scrollX_ = std::clamp(dragStartScrollX_ - dx, 0.0f, maxScrollX_);
        rebuildButtons();
    }

    if (inBar && st.swipe == SwipeDir::Left) {
        scrollX_ = std::clamp(scrollX_ + (kButtonW + kButtonGapX), 0.0f, maxScrollX_);
        rebuildButtons();
    } else if (inBar && st.swipe == SwipeDir::Right) {
        scrollX_ = std::clamp(scrollX_ - (kButtonW + kButtonGapX), 0.0f, maxScrollX_);
        rebuildButtons();
    }

    for (size_t i = 0; i < buttons_.size(); ++i) {
        auto& b = buttons_[i];
        if (b.update(st.mouseV, st.down && inBar, st.pressed && inBar, st.released && inBar)) {
            if (i == (size_t)kCols + 0) health_.decrease(1);
            if (i == (size_t)kCols + 1) health_.increase(1);
            ctx.app->playSfx("sounds/MenuSelect.wav");
        }
    }
}

void DebugWorldScreen::draw(const DrawContext& ctx) {
    int vw = ctx.vs ? ctx.vs->vw : kVirtualW;
    int vh = ctx.vs ? ctx.vs->vh : kVirtualH;

    ClearBackground(BLACK);
    drawTiled(ctx.assets->tex(kBgTile).tex, vw, vh);

    Texture2D hpBg = ctx.assets->tex("sprites/UI/HealthBar/1194.png").tex;
    if (hpBg.id) {
        float hpBgX = std::floor((vw - hpBg.width) * 0.5f) - 3.0f;
        DrawTexture(hpBg, (int)hpBgX, (int)kHpBarY, WHITE);

        float segX = hpBgX + kHpSegmentsStartX;
        float segTop = kHpBarY + kHpSegmentsTopPad + kHpSegmentsOffsetY;
        int hpValue = health_.value();
        float blinkPhase = std::fmod(bgAnimTimer_ * kHpBlinkFrequencyHz, 1.0f);
        bool blinkOn = blinkPhase < 0.5f;
        for (int i = 0; i < hpValue; ++i) {
            const char* segRel = HealthSystem::segmentSpriteRel(i);
            if (!segRel) continue;
            Texture2D seg = ctx.assets->tex(segRel).tex;
            if (!seg.id) continue;
            float y = segTop;
            float x = segX + i * (seg.width + kHpSegmentGapX);
            Color tint = WHITE;
            if (i == hpValue - 1) tint = blinkOn ? WHITE : Fade(WHITE, 0.25f);
            DrawTexture(seg, (int)x, (int)y, tint);
        }
    }

    DrawRectangleRounded(barRect_, 0.20f, 10, Fade(BLACK, 0.50f));
    DrawRectangleRoundedLinesEx(barRect_, 0.20f, 10, 2.0f, Fade(RAYWHITE, 0.35f));

    BeginScissorMode((int)barRect_.x, (int)barRect_.y, (int)barRect_.width, (int)barRect_.height);
    for (const auto& b : buttons_) b.draw(*ctx.assets);
    EndScissorMode();

    if (health_.isDepleted()) {
        Texture2D gameTex = ctx.assets->tex("sprites/UI/GameOver/game.png").tex;
        Texture2D overTex = ctx.assets->tex("sprites/UI/GameOver/over.png").tex;
        float gameCx = vw * 0.5f;
        float gameCy = vh * (1.0f / 3.0f) - (gameTex.height + overTex.height + kGameOverGapY) * 0.5f + gameTex.height * 0.5f;
        DrawCentered(gameTex, gameCx, gameCy);
        DrawCentered(overTex, gameCx, gameCy + gameTex.height * 0.5f + kGameOverGapY + overTex.height * 0.5f);
    }

    if (ctx.debug) {
        DrawRectangle(0, 34, 240, 14, Fade(BLACK, 0.35f));
        DrawText(TextFormat("HP: %d", health_.value()), 4, 36, 12, YELLOW);
    }
}

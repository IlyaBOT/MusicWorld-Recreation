#include "screens/menu/settings/SettingsScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/SpritePath.h"
#include <algorithm>
#include <cmath>
#include <string>

namespace {
#if defined(PLATFORM_ANDROID) || defined(__ANDROID__) || defined(__EMSCRIPTEN__)
constexpr bool kShowVolumePanel = false;
#else
constexpr bool kShowVolumePanel = true;
#endif

constexpr float kPanelX = 6.0f;
constexpr float kPanelW = 224.0f;
constexpr float kPanelStartY = 54.0f;
constexpr float kPanelGap = 6.0f;
constexpr float kVibroPanelH = 86.0f;
constexpr float kRowPanelH = 82.0f;
constexpr float kNotesPanelH = 98.0f;
constexpr float kSliderTrackInset = 8.0f;
constexpr float kSliderTrackOffsetX = 1.0f;
constexpr float kSliderIndicatorOffsetX = -4.0f;
constexpr float kSliderSideSnapOffsetX = 8.0f;
constexpr float kPanelTitleInsetX = 6.0f;
constexpr float kPanelTitleInsetY = 8.0f;
constexpr float kSliderOffsetY = 36.0f;
constexpr float kNotesSliderOffsetY = 56.0f;
constexpr float kContentBottomPad = 38.0f;
constexpr float kScrollSpeed = 20.0f;
constexpr int kDefaultVirtualW = 240;
constexpr int kDefaultVirtualH = 400;
constexpr float kScrollbarRightInset = 3.0f;
constexpr float kScrollbarWidth = 5.0f;
constexpr float kScrollbarThumbWidth = 6.0f;
constexpr float kScrollbarRadius = 6.0f;
constexpr float kScrollbarThumbRadius = 8.0f;
constexpr float kScrollbarMinThumbH = 20.0f;

float sliderTrackStartX(Rectangle r) { return r.x + kSliderTrackInset + kSliderTrackOffsetX; }
float sliderTrackEndX(Rectangle r) { return r.x + r.width - kSliderTrackInset - 1.0f + kSliderTrackOffsetX; }

float sliderStepPositionX(Rectangle r, int idx) {
    idx = std::clamp(idx, 0, 4);
    float left = sliderTrackStartX(r);
    float right = sliderTrackEndX(r);
    float span = std::max(1.0f, right - left);
    float t = idx / 4.0f;
    float x = left + t * span + kSliderIndicatorOffsetX;
    if (idx < 2) x += kSliderSideSnapOffsetX;
    else if (idx > 2) x -= kSliderSideSnapOffsetX;
    return x;
}

struct ScrollbarGeom {
    Rectangle bar{};
    Rectangle thumb{};
};

static ScrollbarGeom makeScrollbarGeom(int vw, int vh, float scrollY, float maxScroll) {
    ScrollbarGeom g{};
    float top = kPanelStartY;
    float bottom = (float)vh - kContentBottomPad;
    float h = std::max(1.0f, bottom - top);

    g.bar = {
        (float)vw - kScrollbarRightInset - kScrollbarWidth,
        top,
        kScrollbarWidth,
        h
    };

    float totalContent = h + std::max(0.0f, maxScroll);
    float ratio = (totalContent > 0.0f) ? (h / totalContent) : 1.0f;
    float thumbH = std::clamp(h * ratio, kScrollbarMinThumbH, h);
    float travel = std::max(0.0f, h - thumbH);
    float t = (maxScroll > 0.0f) ? std::clamp(scrollY / maxScroll, 0.0f, 1.0f) : 0.0f;
    float thumbY = top + travel * t;

    g.thumb = {
        g.bar.x - (kScrollbarThumbWidth - g.bar.width) * 0.5f,
        thumbY,
        kScrollbarThumbWidth,
        thumbH
    };
    return g;
}

static void drawRoundedRectPx(Rectangle r, float radiusPx, Color color) {
    if (r.width <= 0.0f || r.height <= 0.0f) return;
    float minSide = std::min(r.width, r.height);
    float roundness = (minSide > 0.0f) ? std::min(1.0f, (radiusPx * 2.0f) / minSide) : 0.0f;
    DrawRectangleRounded(r, roundness, 8, color);
}
}

float SettingsScreen::calcMaxScroll(int viewportHeight) const {
    float total = kVibroPanelH + kPanelGap
        + (kShowVolumePanel ? (kRowPanelH + kPanelGap) : 0.0f)
        + kNotesPanelH + kPanelGap + kRowPanelH;
    float visible = (float)viewportHeight - kPanelStartY - kContentBottomPad;
    return std::max(0.0f, total - std::max(1.0f, visible));
}

void SettingsScreen::rebuildLayout() {
    float y = kPanelStartY;
    y -= scrollY_;
    vibroPanel_ = {kPanelX, y, kPanelW, kVibroPanelH};
    y += kVibroPanelH + kPanelGap;

    if (kShowVolumePanel) {
        volumePanel_ = {kPanelX, y, kPanelW, kRowPanelH};
        y += kRowPanelH + kPanelGap;
    } else {
        volumePanel_ = {0, 0, 0, 0};
    }

    notesPanel_ = {kPanelX, y, kPanelW, kNotesPanelH};
    y += kNotesPanelH + kPanelGap;
    soundPanel_ = {kPanelX, y, kPanelW, kRowPanelH};

    sliderVolume_ = {kPanelX, volumePanel_.y + kSliderOffsetY, kPanelW, 20};
    sliderNotes_ = {kPanelX, notesPanel_.y + kNotesSliderOffsetY, kPanelW, 20};
    sliderSound_ = {kPanelX, soundPanel_.y + kSliderOffsetY, kPanelW, 20};

    btnVibOn_.rect = {vibroPanel_.x + 42, vibroPanel_.y + 36, 60, 31};
    btnVibOff_.rect = {vibroPanel_.x + 128, vibroPanel_.y + 36, 60, 31};
}

void SettingsScreen::onEnter() {
    scrollY_ = 0.0f;
    maxScrollY_ = calcMaxScroll(kDefaultVirtualH);
    rebuildLayout();
    draggingVolume_ = draggingNotes_ = draggingSound_ = false;
    vibroActive_ = volumeActive_ = notesActive_ = soundActive_ = false;
    draggingScrollbar_ = false;
    scrollbarGrabOffsetY_ = 0.0f;
    SetupMenuBackButton(back_, 400);
}

static int clampi(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

static int valueFromX(Rectangle r, float x, int min, int max, int steps) {
    if (steps == 5) {
        int bestIdx = 0;
        float bestDist = std::fabs(x - sliderStepPositionX(r, 0));
        for (int i = 1; i < 5; ++i) {
            float d = std::fabs(x - sliderStepPositionX(r, i));
            if (d < bestDist) {
                bestDist = d;
                bestIdx = i;
            }
        }
        float t = bestIdx / 4.0f;
        return (int)std::round(min + t * (max - min));
    }

    float left = sliderTrackStartX(r);
    float right = sliderTrackEndX(r);
    float span = std::max(1.0f, right - left);
    float t = ((x - kSliderIndicatorOffsetX) - left) / span;
    t = std::max(0.0f, std::min(1.0f, t));
    if (steps > 1) {
        float s = (float)(steps - 1);
        int idx = (int)std::round(t * s);
        t = idx / s;
    }
    return (int)std::round(min + t * (max - min));
}

static void drawPanelFrame(Rectangle r, bool active) {
    constexpr float roundness = 0.12f;
    constexpr int segments = 10;

    Color outer = active ? Color{178, 146, 246, 255} : Color{121, 74, 208, 255};
    Color mid = active ? Color{147, 85, 230, 255} : Color{120, 66, 206, 255};
    Color inner = active ? Color{125, 58, 215, 255} : Color{110, 50, 196, 255};
    Color bottomShade = active ? Color{80, 28, 162, 255} : Color{68, 24, 146, 255};
    Color topLine = active ? Color{244, 233, 255, 235} : Color{180, 154, 228, 220};

    DrawRectangleRounded(r, roundness, segments, outer);
    Rectangle r1 = {r.x + 1, r.y + 1, r.width - 2, r.height - 2};
    DrawRectangleRounded(r1, roundness, segments, mid);
    Rectangle r2 = {r.x + 2, r.y + 2, r.width - 4, r.height - 4};
    DrawRectangleRounded(r2, roundness, segments, inner);

    // Internal horizontal strips for the old-school texture feel.
    int x0 = (int)(r2.x + 4);
    int x1 = (int)(r2.x + r2.width - 5);
    for (int y = (int)(r2.y + 2); y <= (int)(r2.y + r2.height - 3); y += 2) {
        DrawLine(x0, y, x1, y, Fade(WHITE, active ? 0.08f : 0.06f));
    }

    DrawLine((int)(r2.x + 5), (int)(r2.y + 1), (int)(r2.x + r2.width - 6), (int)(r2.y + 1), topLine);
    DrawLine((int)(r2.x + 5), (int)(r2.y + r2.height - 2), (int)(r2.x + r2.width - 6), (int)(r2.y + r2.height - 2), bottomShade);
    DrawLine((int)(r2.x + 1), (int)(r2.y + 5), (int)(r2.x + 1), (int)(r2.y + r2.height - 6), Fade(topLine, 0.8f));
    DrawLine((int)(r2.x + r2.width - 2), (int)(r2.y + 5), (int)(r2.x + r2.width - 2), (int)(r2.y + r2.height - 6), Fade(topLine, 0.8f));

    if (active) {
        DrawLine((int)(r.x + 6), (int)r.y, (int)(r.x + r.width - 7), (int)r.y, Fade(WHITE, 0.75f));
        DrawLine((int)(r.x + 6), (int)(r.y + r.height - 1), (int)(r.x + r.width - 7), (int)(r.y + r.height - 1), Fade(WHITE, 0.42f));
    }
}

static void drawAnimatedSliderKnob(const DrawContext& ctx, float cx, float cy) {
    float t = (float)GetTime();
    const char* frameRel = (((int)(t * 8.0f)) & 1) == 0
        ? "sprites/UI/Menu/Buttons/1396.png"
        : "sprites/UI/Menu/Buttons/1395.png";
    auto knob = ctx.assets->tex(frameRel).tex;
    if (!knob.id) return;

    // "Breathing" marker: only scales in/out around center.
    float breathe = 0.5f + 0.5f * std::sin(t * 6.0f);
    float scale = 0.86f + breathe * 0.26f;
    float w = 18.0f * scale;
    float h = 18.0f * scale;

    Rectangle src = {0.0f, 0.0f, (float)knob.width, (float)knob.height};
    Rectangle dst = {cx - w * 0.5f, cy - h * 0.5f, w, h};
    DrawTexturePro(knob, src, dst, {0, 0}, 0.0f, WHITE);
}

static void drawSlider(const DrawContext& ctx, Rectangle r, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    auto bar = ctx.assets->tex("sprites/UI/Menu/Buttons/scrollbar5.png").tex;
    if (bar.id) {
        DrawTexture(bar, (int)r.x, (int)r.y, WHITE);
    } else {
        DrawRectangleRounded(r, 0.35f, 8, Fade(RAYWHITE, 0.35f));
    }
    int idx = (int)std::round(t * 4.0f);
    float cx = sliderStepPositionX(r, idx);
    float cy = r.y + r.height * 0.5f;
    drawAnimatedSliderKnob(ctx, cx, cy);
}

static Texture2D sliderFontGlyph(const DrawContext& ctx, char ch) {
    switch (ch) {
    case '-': return ctx.assets->tex("sprites/UI/Fonts/1003.png").tex;
    case '+': return ctx.assets->tex("sprites/UI/Fonts/1004.png").tex;
    case '0': return ctx.assets->tex("sprites/UI/Fonts/0992.png").tex;
    case '1': return ctx.assets->tex("sprites/UI/Fonts/0993.png").tex;
    case '2': return ctx.assets->tex("sprites/UI/Fonts/0994.png").tex;
    case '4': return ctx.assets->tex("sprites/UI/Fonts/0996.png").tex;
    default: return {};
    }
}

static void drawScaleLabelSpriteCentered(const DrawContext& ctx, int value, int cx, int topY) {
    std::string text;
    if (value > 0) text.push_back('+');
    else if (value < 0) text.push_back('-');
    text += std::to_string(std::abs(value));

    int totalW = 0;
    int maxH = 0;
    for (char ch : text) {
        Texture2D g = sliderFontGlyph(ctx, ch);
        if (!g.id) continue;
        totalW += g.width;
        maxH = std::max(maxH, g.height);
    }
    if (totalW <= 0 || maxH <= 0) return;

    float x = cx - totalW * 0.5f;
    int baselineY = topY + maxH;
    for (char ch : text) {
        Texture2D g = sliderFontGlyph(ctx, ch);
        if (!g.id) continue;
        int y = baselineY - g.height;
        if (ch == '-') y -= 4;
        DrawTexture(g, (int)std::round(x), y, WHITE);
        x += g.width;
    }
}

static void drawScaleLabelsSprites(const DrawContext& ctx, Rectangle slider, const int values[5]) {
    float left = sliderTrackStartX(slider);
    float right = sliderTrackEndX(slider);
    float span = std::max(1.0f, right - left);
    for (int i = 0; i < 5; ++i) {
        int x = (int)std::round(left + span * (i / 4.0f)) - 2;
        int av = std::abs(values[i]);
        if (av == 200 || av == 400) x -= 4;
        drawScaleLabelSpriteCentered(ctx, values[i], x, (int)(slider.y + slider.height + 2));
    }
}

void SettingsScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    int vw = ctx.vs ? ctx.vs->vw : kDefaultVirtualW;
    int vh = ctx.vs ? ctx.vs->vh : kDefaultVirtualH;
    maxScrollY_ = calcMaxScroll(vh);

    ScrollbarGeom sb = makeScrollbarGeom(vw, vh, scrollY_, maxScrollY_);
    if (st.inViewport) {
        float wheel = GetMouseWheelMove();
        if (std::fabs(wheel) > 0.001f) {
            scrollY_ = std::clamp(scrollY_ - wheel * kScrollSpeed, 0.0f, maxScrollY_);
        }
    }

    if (st.pressed && st.inViewport && CheckCollisionPointRec(st.mouseV, sb.thumb)) {
        draggingScrollbar_ = true;
        scrollbarGrabOffsetY_ = st.mouseV.y - sb.thumb.y;
    }
    if (st.released) {
        draggingScrollbar_ = false;
    }
    if (draggingScrollbar_ && st.down && st.inViewport && maxScrollY_ > 0.0f) {
        float travel = std::max(0.0f, sb.bar.height - sb.thumb.height);
        if (travel > 0.0f) {
            float newThumbY = std::clamp(st.mouseV.y - scrollbarGrabOffsetY_, sb.bar.y, sb.bar.y + travel);
            float t = (newThumbY - sb.bar.y) / travel;
            scrollY_ = std::clamp(t * maxScrollY_, 0.0f, maxScrollY_);
        }
    }

    scrollY_ = std::clamp(scrollY_, 0.0f, maxScrollY_);
    rebuildLayout();

    auto clickBack = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (st.keyBack || clickBack(back_)) { ctx.app->saveProfile(); ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    float contentBottomY = (float)vh - kContentBottomPad;
    bool inContent = st.inViewport && st.mouseV.y >= kPanelStartY && st.mouseV.y <= contentBottomY;
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && inContent, st.pressed && inContent, st.released && inContent); };

    if (click(btnVibOn_) && !ctx.profile->vibration) {
        ctx.profile->vibration = true;
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->saveProfile();
    }
    if (click(btnVibOff_) && ctx.profile->vibration) {
        ctx.profile->vibration = false;
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->saveProfile();
    }

    auto beginDrag = [&](Rectangle r, bool& dragging){
        if (st.pressed && inContent && CheckCollisionPointRec(st.mouseV, {r.x-10, r.y-10, r.width+20, r.height+20})) dragging = true;
        if (st.released) dragging = false;
    };

    if (kShowVolumePanel) beginDrag(sliderVolume_, draggingVolume_);
    beginDrag(sliderNotes_, draggingNotes_);
    beginDrag(sliderSound_, draggingSound_);

    if (kShowVolumePanel && draggingVolume_ && st.down && inContent) {
        int v = valueFromX(sliderVolume_, st.mouseV.x, 0, 100, 5);
        if (ctx.profile->masterVolume != v) {
            ctx.profile->masterVolume = v;
            SetMasterVolume((float)v / 100.0f);
            ctx.app->playSfx("sounds/MenuSelect.wav");
            ctx.app->saveProfile();
        }
    }
    if (draggingNotes_ && st.down && inContent) {
        int v = valueFromX(sliderNotes_, st.mouseV.x, -2, 2, 5);
        v = clampi(v, -2, 2);
        if (ctx.profile->syncNotes != v) {
            ctx.profile->syncNotes = v;
            ctx.app->saveProfile();
        }
    }
    if (draggingSound_ && st.down && inContent) {
        int v = valueFromX(sliderSound_, st.mouseV.x, -400, 400, 5);
        int snapped = (int)std::round(v / 200.0f) * 200;
        snapped = clampi(snapped, -400, 400);
        if (ctx.profile->syncSound != snapped) {
            ctx.profile->syncSound = snapped;
            ctx.app->saveProfile();
        }
    }

    vibroActive_ = btnVibOn_.hovered || btnVibOn_.pressed || btnVibOff_.hovered || btnVibOff_.pressed;
    volumeActive_ = kShowVolumePanel && (draggingVolume_ || (inContent && CheckCollisionPointRec(st.mouseV, volumePanel_)));
    notesActive_ = draggingNotes_ || (inContent && CheckCollisionPointRec(st.mouseV, notesPanel_));
    soundActive_ = draggingSound_ || (inContent && CheckCollisionPointRec(st.mouseV, soundPanel_));
}

void SettingsScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);

    auto title = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/settings-title.png", "sprites/UI/Menu/Russian/0052.png")).tex;
    DrawTexture(title, 4, 4, WHITE);

    int vw = ctx.vs ? ctx.vs->vw : kDefaultVirtualW;
    int vh = ctx.vs ? ctx.vs->vh : kDefaultVirtualH;
    int clipY = (int)kPanelStartY;
    int clipH = std::max(1, (int)(vh - kContentBottomPad) - clipY);
    BeginScissorMode(0, clipY, vw, clipH);

    drawPanelFrame(vibroPanel_, vibroActive_);
    if (kShowVolumePanel) drawPanelFrame(volumePanel_, volumeActive_);
    drawPanelFrame(notesPanel_, notesActive_);
    drawPanelFrame(soundPanel_, soundActive_);

    auto vibTitle = ctx.assets->tex(vibroActive_
        ? "sprites/UI/Menu/Russian/vibro-title_selected.png"
        : "sprites/UI/Menu/Russian/vibro-title.png").tex;
    DrawAt(vibTitle, vibroPanel_.x + kPanelTitleInsetX, vibroPanel_.y + kPanelTitleInsetY);

    auto on = ctx.assets->tex(ctx.profile->vibration
        ? "sprites/UI/Menu/Russian/on-btn_enabled.png"
        : "sprites/UI/Menu/Russian/on-btn_disabled.png").tex;
    auto off = ctx.assets->tex(ctx.profile->vibration
        ? "sprites/UI/Menu/Russian/off-btn_disabled.png"
        : "sprites/UI/Menu/Russian/off-btn_enabled.png").tex;
    DrawCentered(on, btnVibOn_.rect.x + btnVibOn_.rect.width * 0.5f, btnVibOn_.rect.y + btnVibOn_.rect.height * 0.5f);
    DrawCentered(off, btnVibOff_.rect.x + btnVibOff_.rect.width * 0.5f, btnVibOff_.rect.y + btnVibOff_.rect.height * 0.5f);

    if (kShowVolumePanel) {
        auto volTitle = ctx.assets->tex(volumeActive_
            ? "sprites/UI/Menu/Russian/volume-title_selected.png"
            : "sprites/UI/Menu/Russian/volume-title.png").tex;
        DrawAt(volTitle, volumePanel_.x + kPanelTitleInsetX, volumePanel_.y + kPanelTitleInsetY);
        int volStep = (int)std::round((ctx.profile->masterVolume / 100.0f) * 4.0f);
        float volumeT = std::max(0.0f, std::min(4.0f, (float)volStep)) / 4.0f;
        drawSlider(ctx, sliderVolume_, volumeT);
    }

    auto notesTitle = ctx.assets->tex(notesActive_
        ? "sprites/UI/Menu/Russian/sync-notes_selected.png"
        : "sprites/UI/Menu/Russian/sync-notes-title.png").tex;
    DrawAt(notesTitle, notesPanel_.x + kPanelTitleInsetX, notesPanel_.y + kPanelTitleInsetY);
    auto notesDesc = ctx.assets->tex("sprites/UI/Menu/Russian/sync-notes-description.png").tex;
    DrawAt(notesDesc, notesPanel_.x + 4, notesPanel_.y + 26);
    float notesT = (float)(ctx.profile->syncNotes + 2) / 4.0f;
    drawSlider(ctx, sliderNotes_, notesT);
    const int notesValues[5] = {-2, -1, 0, 1, 2};
    drawScaleLabelsSprites(ctx, sliderNotes_, notesValues);

    auto soundTitle = ctx.assets->tex(soundActive_
        ? "sprites/UI/Menu/Russian/sync-sound_selected.png"
        : "sprites/UI/Menu/Russian/sync-sound-title.png").tex;
    DrawAt(soundTitle, soundPanel_.x + kPanelTitleInsetX, soundPanel_.y + kPanelTitleInsetY);
    float soundT = (float)(ctx.profile->syncSound + 400) / 800.0f;
    drawSlider(ctx, sliderSound_, soundT);
    const int soundValues[5] = {-400, -200, 0, 200, 400};
    drawScaleLabelsSprites(ctx, sliderSound_, soundValues);

    EndScissorMode();

    ScrollbarGeom sb = makeScrollbarGeom(vw, vh, scrollY_, maxScrollY_);
    Color barColor = Color{79, 39, 150, 220};
    Color thumbShadowColor = Color{0, 0, 0, 180};
    Color thumbColor = Color{196, 169, 255, 245};
    drawRoundedRectPx(sb.bar, kScrollbarRadius, barColor);
    Rectangle thumbShadow = {sb.thumb.x + 2.0f, sb.thumb.y + 2.0f, sb.thumb.width, sb.thumb.height};
    drawRoundedRectPx(thumbShadow, kScrollbarThumbRadius, thumbShadowColor);
    drawRoundedRectPx(sb.thumb, kScrollbarThumbRadius, thumbColor);

    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText("SettingsScreen", 6, (ctx.vs ? ctx.vs->vh : 400) - 12, 12, YELLOW);
}

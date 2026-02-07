#include "screens/menu/settings/SettingsScreen.h"
#include "core/App.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include <algorithm>

void SettingsScreen::onEnter() {
    vibToggle_.rect = { 30, 120, 180, 46 };
    vibToggle_.bgRel = "sprites/UI/Menu/Buttons/1359.png";
    vibToggle_.bgRelActive = "sprites/UI/Menu/Buttons/1360.png";
    SetupMenuBackButton(back_);
    sliderNotes_ = { 30, 205, 180, 18 };
    sliderSound_ = { 30, 275, 180, 18 };
}

static int clampi(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

static int valueFromX(Rectangle r, float x, int min, int max, int steps) {
    float t = (x - r.x) / r.width;
    t = std::max(0.0f, std::min(1.0f, t));
    if (steps > 1) {
        float s = (float)(steps - 1);
        int idx = (int)std::round(t * s);
        t = idx / s;
    }
    return (int)std::round(min + t * (max - min));
}

static void drawSlider(const DrawContext& ctx, Rectangle r, int value, int min, int max) {
    auto bar = ctx.assets->tex("sprites/UI/Menu/Buttons/1338.png").tex;
    auto knob = ctx.assets->tex("sprites/UI/Menu/Buttons/1358.png").tex;

    if (bar.id) DrawTexture(bar, (int)r.x, (int)r.y-2, WHITE);
    else DrawRectangle((int)r.x, (int)r.y, (int)r.width, (int)r.height, Fade(RAYWHITE, 0.25f));

    float t = (float)(value - min) / (float)(max - min);
    float x = r.x + t * r.width;

    if (knob.id) DrawTexture(knob, (int)(x - knob.width/2), (int)(r.y - knob.height/2 + 7), WHITE);
    else DrawCircle((int)x, (int)(r.y + r.height/2), 6, YELLOW);
}

void SettingsScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (st.keyBack || click(back_)) { ctx.app->saveProfile(); ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    if (click(vibToggle_)) {
        ctx.profile->vibration = !ctx.profile->vibration;
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->saveProfile();
    }

    auto beginDrag = [&](Rectangle r, bool& dragging){
        if (st.pressed && st.inViewport && CheckCollisionPointRec(st.mouseV, {r.x-10, r.y-10, r.width+20, r.height+20})) dragging = true;
        if (st.released) dragging = false;
    };

    beginDrag(sliderNotes_, draggingNotes_);
    beginDrag(sliderSound_, draggingSound_);

    if (draggingNotes_ && st.down && st.inViewport) {
        int v = valueFromX(sliderNotes_, st.mouseV.x, -2, 2, 5);
        ctx.profile->syncNotes = clampi(v, -2, 2);
    }
    if (draggingSound_ && st.down && st.inViewport) {
        int v = valueFromX(sliderSound_, st.mouseV.x, -400, 400, 5);
        int snapped = (int)std::round(v / 200.0f) * 200;
        ctx.profile->syncSound = clampi(snapped, -400, 400);
    }
}

void SettingsScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 360);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/0052.png").tex;
    DrawTexture(title, 10, 18, WHITE);

    vibToggle_.draw(*ctx.assets);
    DrawText("VIBRATION", 38, 132, 14, RAYWHITE);
    auto on = ctx.assets->tex("sprites/UI/Menu/Buttons/0077.png").tex;
    auto off = ctx.assets->tex("sprites/UI/Menu/Buttons/0078.png").tex;
    if (ctx.profile->vibration) {
        if (on.id) DrawTexture(on, 140, 128, WHITE);
        else DrawText("ON", 160, 132, 14, YELLOW);
    } else {
        if (off.id) DrawTexture(off, 140, 128, WHITE);
        else DrawText("OFF", 160, 132, 14, YELLOW);
    }

    DrawText("Sync notes", 30, 185, 14, RAYWHITE);
    drawSlider(ctx, sliderNotes_, ctx.profile->syncNotes, -2, 2);
    DrawText(TextFormat("%d", ctx.profile->syncNotes), 215, 200, 14, RAYWHITE);

    DrawText("Sync sound", 30, 255, 14, RAYWHITE);
    drawSlider(ctx, sliderSound_, ctx.profile->syncSound, -400, 400);
    DrawText(TextFormat("%d", ctx.profile->syncSound), 200, 270, 14, RAYWHITE);
    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText("SettingsScreen", 6, (ctx.vs ? ctx.vs->vh : 360) - 12, 12, YELLOW);
}

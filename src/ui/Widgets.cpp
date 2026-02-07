#include "ui/Widgets.h"
#include "core/Assets.h"
#include <algorithm>

namespace ui {

void Toast::show(const std::string& t, float seconds) {
    text = t;
    timer = seconds;
}

void Toast::update(float dt) {
    if (timer > 0.0f) timer = std::max(0.0f, timer - dt);
}

void Toast::draw(int vw, int vh, bool debug) const {
    if (timer <= 0.0f) return;
    float alpha = (timer < 0.2f) ? (timer / 0.2f) : 1.0f;

    int pad = 10;
    int boxW = vw - 2*pad;
    int boxH = 46;
    int x = pad;
    int y = vh - boxH - 20;

    DrawRectangleRounded({(float)x,(float)y,(float)boxW,(float)boxH}, 0.25f, 8, Fade(BLACK, 0.7f * alpha));
    DrawRectangleRoundedLinesEx({(float)x,(float)y,(float)boxW,(float)boxH}, 0.25f, 8, 2.0f, Fade(RAYWHITE, 0.35f * alpha));
    DrawText(text.c_str(), x+12, y+14, 16, Fade(RAYWHITE, alpha));

    if (debug) DrawText("TOAST", x+boxW-60, y+4, 12, Fade(YELLOW, alpha));
}

static void drawCentered(Texture2D t, Rectangle r, Color tint) {
    if (t.id == 0) return;
    float x = r.x + (r.width - t.width)*0.5f;
    float y = r.y + (r.height - t.height)*0.5f;
    DrawTexture(t, (int)x, (int)y, tint);
}

bool SpriteButton::update(Vector2 p, bool down, bool pressedEvt, bool releasedEvt) {
    if (!enabled) { hovered = pressed = false; return false; }
    hovered = CheckCollisionPointRec(p, rect);

    if (pressedEvt && hovered) pressed = true;
    bool clicked = false;
    if (releasedEvt) {
        clicked = pressed && hovered;
        pressed = false;
    }
    if (!down && !hovered) pressed = false;
    return clicked;
}

void SpriteButton::draw(Assets& assets) const {
    Color tint = enabled ? WHITE : Fade(WHITE, 0.45f);
    bool active = (hovered || pressed);

    if (!bgRel.empty()) {
        Texture2D t = assets.tex(bgRel).tex;
        if (!bgRelActive.empty() && active) t = assets.tex(bgRelActive).tex;
        drawCentered(t, rect, tint);
    } else {
        DrawRectangleRounded(rect, 0.2f, 6, Fade(DARKPURPLE, enabled ? 0.9f : 0.4f));
        DrawRectangleRoundedLinesEx(rect, 0.2f, 6, 2.0f, Fade(RAYWHITE, 0.35f));
    }

    if (!labelRel.empty()) {
        Texture2D t = assets.tex(labelRel).tex;
        if (!labelRelActive.empty() && active) t = assets.tex(labelRelActive).tex;
        drawCentered(t, rect, tint);
    }
}

void Carousel::start(int direction) {
    if (direction == 0) return;
    if (dir != 0) return;
    dir = direction;
    anim = 0.0f;
}

void Carousel::update(float dt) {
    if (dir == 0) return;
    anim += dt * speed;
    if (anim >= 1.0f) { anim = 1.0f; dir = 0; }
}

float Carousel::offsetPx(float cardW) const {
    if (dir == 0) return 0.0f;
    float t = anim;
    float e = 1.0f - (1.0f - t)*(1.0f - t);
    return -dir * e * cardW;
}

} // namespace ui

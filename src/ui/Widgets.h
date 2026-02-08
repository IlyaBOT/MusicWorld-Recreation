#pragma once
#include "raylib.h"
#include <string>

class Assets;

namespace ui {

struct Toast {
    float timer = 0.0f;
    std::string text;

    void show(const std::string& t, float seconds = 1.2f);
    void update(float dt);
    void draw(int vw, int vh, bool debug = false) const;
};

struct SpriteButton {
    Rectangle rect{};
    std::string bgRel;
    std::string bgRelActive;
    std::string labelRel;
    std::string labelRelActive;
    bool enabled = true;

    bool hovered = false;
    bool pressed = false;

    bool update(Vector2 p, bool down, bool pressedEvt, bool releasedEvt);
    void draw(Assets& assets) const;
};

struct Carousel {
    float anim = 0.0f;
    int dir = 0;
    float speed = 10.0f;

    bool animating() const { return dir != 0; }
    void start(int direction);
    void update(float dt);
    float offsetPx(float cardW) const;
};

} // namespace ui

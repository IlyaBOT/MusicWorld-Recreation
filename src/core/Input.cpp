#include "core/Input.h"
#include "core/VirtualScreen.h"
#include <cmath>

void InputState::resetFrame() {
    pressed = released = false;
    swipe = SwipeDir::None;
    swipeDist = 0.0f;
    keyBack = keySelect = false;
}

void Input::update(const VirtualScreen& vs) {
    s_.resetFrame();
    s_.mouseWin = GetMousePosition();
    s_.inViewport = vs.containsWindowPoint(s_.mouseWin);
    s_.mouseV = vs.windowToVirtual(s_.mouseWin);

    s_.pressed  = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    s_.down     = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    s_.released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    s_.keyBack = IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE);
    s_.keySelect = IsKeyPressed(KEY_ENTER);

    const float MIN_DIST = 28.0f;
    const float MAX_TIME = 0.6f;

    if (s_.pressed && s_.inViewport) {
        dragging_ = true;
        dragStartV_ = s_.mouseV;
        dragStartTime_ = (float)GetTime();
    }

    if (dragging_ && s_.released) {
        dragging_ = false;
        float dt = (float)GetTime() - dragStartTime_;
        Vector2 d = { s_.mouseV.x - dragStartV_.x, s_.mouseV.y - dragStartV_.y };
        float adx = std::fabs(d.x);
        float ady = std::fabs(d.y);
        if (dt <= MAX_TIME && adx >= MIN_DIST && adx > ady * 1.2f) {
            s_.swipe = (d.x < 0) ? SwipeDir::Left : SwipeDir::Right;
            s_.swipeDist = adx;
        }
    }
}

#pragma once
#include "raylib.h"

enum class SwipeDir { None, Left, Right };

struct InputState {
    Vector2 mouseWin{};
    bool pressed = false;
    bool down = false;
    bool released = false;

    Vector2 mouseV{};
    bool inViewport = false;

    SwipeDir swipe = SwipeDir::None;
    float swipeDist = 0.0f;

    bool keyBack = false;
    bool keySelect = false;

    void resetFrame();
};

struct VirtualScreen;

class Input {
public:
    void update(const VirtualScreen& vs);
    const InputState& state() const { return s_; }

private:
    InputState s_{};
    bool dragging_ = false;
    Vector2 dragStartV_{};
    float dragStartTime_ = 0.0f;
};

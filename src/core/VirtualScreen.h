#pragma once
#include "raylib.h"

struct VirtualScreen {
    int vw = 240;
    int vh = 400;

    RenderTexture2D target{};
    Rectangle dstRect{};
    bool initialized = false;

    void init(int virtualW, int virtualH);
    void shutdown();

    void begin();
    void end();

    Vector2 windowToVirtual(Vector2 p) const;
    bool containsWindowPoint(Vector2 p) const;
};

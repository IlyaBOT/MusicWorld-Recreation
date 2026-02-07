#pragma once
#include "raylib.h"
#include "core/Assets.h"

inline void DrawMenuBackground(Assets& assets) {
    auto top = assets.tex("sprites/menu/1414.png").tex;
    auto half = assets.tex("sprites/menu/1415.png").tex;

    if (top.id && half.id) {
        DrawTexture(top, 0, 0, WHITE);
        int yBottom = 41;
        DrawTexture(half, 0, yBottom, WHITE);

        Rectangle src = { (float)half.width, 0, -(float)half.width, (float)half.height };
        Rectangle dst = { (float)(240 - half.width), (float)yBottom, (float)half.width, (float)half.height };
        DrawTexturePro(half, src, dst, {0,0}, 0.0f, WHITE);
    } else {
        ClearBackground({85, 15, 110, 255});
        DrawRectangle(0, 0, 240, 120, {150, 30, 160, 255});
        DrawText("Missing menu bg (sprites/menu/1414.png, 1415.png)", 10, 10, 12, RAYWHITE);
    }
}

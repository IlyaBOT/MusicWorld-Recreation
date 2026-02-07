#pragma once
#include "raylib.h"
#include "core/Assets.h"

inline void DrawMenuBackground(Assets& assets, int vw = 240, int vh = 360) {
    auto top = assets.tex("sprites/UI/top.png").tex;
    auto half = assets.tex("sprites/UI/background_half.png").tex;

    if (top.id && half.id) {
        constexpr Color kTopFill = {255, 32, 190, 255};
        int yBottom = vh - half.height;
        if (yBottom < 0) yBottom = 0;
        if (yBottom > 0) DrawRectangle(0, 0, vw, yBottom, kTopFill);

        DrawTexture(half, 0, yBottom, WHITE);
        Rectangle src = { (float)half.width, 0, -(float)half.width, (float)half.height };
        Rectangle dst = { (float)(vw - half.width), (float)yBottom, (float)half.width, (float)half.height };
        DrawTexturePro(half, src, dst, {0,0}, 0.0f, WHITE);

        Rectangle topSrc = {0.0f, 0.0f, (float)top.width, (float)top.height};
        Rectangle topDst = {0.0f, 0.0f, (float)vw, (float)(top.height + yBottom)};
        DrawTexturePro(top, topSrc, topDst, {0,0}, 0.0f, WHITE);
    } else {
        ClearBackground({255, 32, 190, 255});
        DrawRectangle(0, 0, vw, 120, {150, 30, 160, 255});
        DrawText("Missing menu bg (sprites/UI/top.png, sprites/UI/background_half.png)", 10, 10, 12, RAYWHITE);
    }
}

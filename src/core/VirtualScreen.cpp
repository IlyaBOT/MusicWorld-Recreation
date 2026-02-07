#include "core/VirtualScreen.h"
#include <algorithm>

void VirtualScreen::init(int virtualW, int virtualH) {
    vw = virtualW; vh = virtualH;
    if (initialized) shutdown();
    target = LoadRenderTexture(vw, vh);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    initialized = true;
}

void VirtualScreen::shutdown() {
    if (!initialized) return;
    UnloadRenderTexture(target);
    initialized = false;
}

void VirtualScreen::begin() {
    BeginTextureMode(target);
    ClearBackground(BLACK);
}

void VirtualScreen::end() {
    EndTextureMode();

    int ww = GetScreenWidth();
    int wh = GetScreenHeight();

    float sx = (float)ww / (float)vw;
    float sy = (float)wh / (float)vh;
    float s = std::min(sx, sy);

    float w = vw * s;
    float h = vh * s;
    float x = (ww - w) * 0.5f;
    float y = (wh - h) * 0.5f;

    dstRect = { x, y, w, h };

    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle src = { 0, 0, (float)vw, -(float)vh };
    DrawTexturePro(target.texture, src, dstRect, {0,0}, 0.0f, WHITE);

    EndDrawing();
}

Vector2 VirtualScreen::windowToVirtual(Vector2 p) const {
    if (dstRect.width <= 0 || dstRect.height <= 0) return {0,0};
    float nx = (p.x - dstRect.x) / dstRect.width;
    float ny = (p.y - dstRect.y) / dstRect.height;
    return { nx * vw, ny * vh };
}

bool VirtualScreen::containsWindowPoint(Vector2 p) const {
    return CheckCollisionPointRec(p, dstRect);
}

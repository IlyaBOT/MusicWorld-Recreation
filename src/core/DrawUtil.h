#pragma once
#include "raylib.h"

inline void DrawCentered(Texture2D t, float cx, float cy, Color tint = WHITE) {
    if (t.id == 0) return;
    DrawTexture(t, (int)(cx - t.width * 0.5f), (int)(cy - t.height * 0.5f), tint);
}
inline void DrawAt(Texture2D t, float x, float y, Color tint = WHITE) {
    if (t.id == 0) return;
    DrawTexture(t, (int)x, (int)y, tint);
}
inline void DrawTextCentered(const char* txt, int cx, int y, int size, Color col) {
    int w = MeasureText(txt, size);
    DrawText(txt, cx - w/2, y, size, col);
}

#include "screens/title/TitleScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/main/MenuMainScreen.h"
#include <memory>

void TitleScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if ((st.pressed && st.inViewport) || st.keySelect) {
        ctx.app->replace(std::make_unique<MenuMainScreen>());
        ctx.app->playSfx("sounds/MenuSelect.wav");
    }
}

void TitleScreen::draw(const DrawContext& ctx) {
    // reuse menu bg for now
    auto top = ctx.assets->tex("sprites/menu/1414.png").tex;
    auto half = ctx.assets->tex("sprites/menu/1415.png").tex;

    if (top.id && half.id) {
        DrawTexture(top, 0, 0, WHITE);
        int yBottom = 41;
        DrawTexture(half, 0, yBottom, WHITE);
        Rectangle src = { (float)half.width, 0, -(float)half.width, (float)half.height };
        Rectangle dst = { (float)(240 - half.width), (float)yBottom, (float)half.width, (float)half.height };
        DrawTexturePro(half, src, dst, {0,0}, 0.0f, WHITE);
    } else {
        ClearBackground({70, 10, 95, 255});
    }

    auto logo = ctx.assets->tex("sprites/title/1192.png").tex;
    auto lg   = ctx.assets->tex("sprites/title/1193.png").tex;
    auto tap  = ctx.assets->tex("sprites/menu/russian/0035.png").tex;

    DrawCentered(logo, 120, 190);
    DrawCentered(lg, 120, 315);
    DrawCentered(tap, 120, 352);

    if (ctx.debug) {
        if (!logo.id) DrawText("missing sprites/title/1192.png", 6, 6, 12, YELLOW);
        if (!lg.id)   DrawText("missing sprites/title/1193.png", 6, 20, 12, YELLOW);
        if (!tap.id)  DrawText("missing sprites/menu/russian/0035.png", 6, 34, 12, YELLOW);
    }
}

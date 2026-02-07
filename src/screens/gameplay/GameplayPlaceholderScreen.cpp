#include "screens/gameplay/GameplayPlaceholderScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/BackButton.h"

void GameplayPlaceholderScreen::onEnter() {
    SetupMenuBackButton(back_);
}

void GameplayPlaceholderScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    bool clickedBack = back_.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport);
    if (st.keyBack || clickedBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    // dev shortcut: U unlocks storyCompleted
    if (IsKeyPressed(KEY_U)) {
        ctx.profile->storyCompleted = true;
        ctx.toast->show("Unlocked: storyCompleted=true");
        ctx.app->saveProfile();
    }
}

void GameplayPlaceholderScreen::draw(const DrawContext& ctx) {
    ClearBackground({15, 15, 25, 255});
    DrawTextCentered("GAMEPLAY PLACEHOLDER", 120, 90, 18, RAYWHITE);
    DrawText(TextFormat("mode: %s", mode_.c_str()), 30, 150, 16, RAYWHITE);
    DrawText(TextFormat("difficulty: %s", diff_.c_str()), 30, 175, 16, RAYWHITE);
    DrawText("Esc/Backspace = back", 30, 230, 14, GRAY);
    DrawText("U = unlock storyCompleted", 30, 252, 14, GRAY);
    back_.draw(*ctx.assets);
}

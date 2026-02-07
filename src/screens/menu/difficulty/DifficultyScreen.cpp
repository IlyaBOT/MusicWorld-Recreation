#include "screens/menu/difficulty/DifficultyScreen.h"
#include "core/App.h"
#include "screens/menu/MenuBg.h"
#include "screens/gameplay/GameplayPlaceholderScreen.h"
#include <memory>

void DifficultyScreen::onEnter() {
    easy_.rect = { 20, 115, 200, 70 };
    med_.rect  = { 20, 195, 200, 70 };
    hard_.rect = { 20, 275, 200, 70 };
    easy_.bgRel = med_.bgRel = hard_.bgRel = "sprites/UI/Menu/Buttons/1384.png";
    easy_.bgRelActive = med_.bgRelActive = hard_.bgRelActive = "sprites/UI/Menu/Buttons/1384.png";
}

static void drawDiffRow(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& iconRel, const std::string& textRel, bool locked) {
    b.draw(*ctx.assets);
    auto icon = ctx.assets->tex(iconRel).tex;
    if (icon.id) DrawTexture(icon, (int)(b.rect.x + 28), (int)(b.rect.y + (b.rect.height-icon.height)/2), WHITE);
    auto txt = ctx.assets->tex(textRel).tex;
    if (txt.id) DrawTexture(txt, (int)(b.rect.x + 90), (int)(b.rect.y + (b.rect.height-txt.height)/2 + 8), WHITE);
    if (locked) {
        auto lock = ctx.assets->tex("sprites/UI/Menu/Buttons/1353.png").tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width - lock.width - 18), (int)(b.rect.y + (b.rect.height-lock.height)/2), WHITE);
    }
}

void DifficultyScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    if (click(easy_)) { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<GameplayPlaceholderScreen>(mode_, "easy")); return; }
    if (click(med_))  { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<GameplayPlaceholderScreen>(mode_, "med")); return; }
    if (click(hard_)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (!ctx.profile->storyCompleted) { ctx.toast->show("Locked (finish Story)"); return; }
        ctx.app->push(std::make_unique<GameplayPlaceholderScreen>(mode_, "hard"));
        return;
    }
}

void DifficultyScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 360);

    if (mode_ == "free") {
        auto t = ctx.assets->tex("sprites/UI/Menu/Russian/0055.png").tex;
        DrawTexture(t, 10, 24, WHITE);
    } else {
        auto t = ctx.assets->tex("sprites/UI/Menu/Russian/0017.png").tex;
        DrawTexture(t, 10, 24, WHITE);
    }

    drawDiffRow(ctx, easy_, "sprites/UI/Menu/Buttons/1234.png", "sprites/UI/Menu/Russian/0024.png", false);
    drawDiffRow(ctx, med_,  "sprites/UI/Menu/Buttons/1235.png", "sprites/UI/Menu/Russian/0025.png", false);
    drawDiffRow(ctx, hard_, "sprites/UI/Menu/Buttons/1236.png", "sprites/UI/Menu/Russian/0026.png", !ctx.profile->storyCompleted);

    if (ctx.debug) DrawText("DifficultyScreen", 6, 384, 12, YELLOW);
}

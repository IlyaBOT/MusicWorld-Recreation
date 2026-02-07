#include "screens/menu/mode/ModeSelectScreen.h"
#include "core/App.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/difficulty/DifficultyScreen.h"
#include <memory>

void ModeSelectScreen::onEnter() {
    dj_.rect = { 20, 105, 100, 100 };
    story_.rect = { 120, 105, 100, 100 };
    free_.rect = { 20, 210, 100, 100 };
    party_.rect = { 120, 210, 100, 100 };

    dj_.bgRel = story_.bgRel = free_.bgRel = party_.bgRel = "sprites/menu/buttons/1384.png";
    dj_.bgRelActive = story_.bgRelActive = free_.bgRelActive = party_.bgRelActive = "sprites/menu/buttons/1384.png";
}

void ModeSelectScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    if (click(dj_)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.toast->show("Under Construction");
        return;
    }
    if (click(story_)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->push(std::make_unique<DifficultyScreen>("story"));
        return;
    }
    if (click(free_)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->push(std::make_unique<DifficultyScreen>("free"));
        return;
    }
    if (click(party_)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (!ctx.profile->storyCompleted) ctx.toast->show("Locked (finish Story)");
        else ctx.toast->show("Party mode WIP");
        return;
    }
}

static void drawBubble(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& iconRel, const std::string& textRel, bool locked) {
    b.draw(*ctx.assets);

    auto icon = ctx.assets->tex(iconRel).tex;
    if (icon.id) {
        float cx = b.rect.x + b.rect.width/2;
        float cy = b.rect.y + b.rect.height/2 - 6;
        DrawTexture(icon, (int)(cx - icon.width/2), (int)(cy - icon.height/2), WHITE);
    }
    auto txt = ctx.assets->tex(textRel).tex;
    if (txt.id) {
        float cx = b.rect.x + b.rect.width/2;
        float y = b.rect.y + b.rect.height - 12;
        DrawTexture(txt, (int)(cx - txt.width/2), (int)(y), WHITE);
    }
    if (locked) {
        auto lock = ctx.assets->tex("sprites/menu/buttons/1353.png").tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width/2 - lock.width/2), (int)(b.rect.y + b.rect.height/2 - lock.height/2), WHITE);
        else DrawText("LOCK", (int)b.rect.x + 34, (int)b.rect.y + 44, 16, RED);
    }
}

void ModeSelectScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);
    auto title = ctx.assets->tex("sprites/menu/russian/0058.png").tex;
    DrawTexture(title, 10, 18, WHITE);

    drawBubble(ctx, dj_,    "sprites/menu/buttons/1224.png", "sprites/menu/russian/0053.png", false);
    drawBubble(ctx, story_, "sprites/menu/buttons/1225.png", "sprites/menu/russian/0017.png", false);
    drawBubble(ctx, free_,  "sprites/menu/buttons/1226.png", "sprites/menu/russian/0055.png", false);
    drawBubble(ctx, party_, "sprites/menu/buttons/1227.png", "sprites/menu/russian/0054.png", !ctx.profile->storyCompleted);

    if (ctx.debug) DrawText("ModeSelectScreen", 6, 384, 12, YELLOW);
}

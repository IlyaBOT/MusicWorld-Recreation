#include "screens/menu/difficulty/DifficultyScreen.h"
#include "core/App.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/SpritePath.h"
#include "screens/gameplay/GameplayPlaceholderScreen.h"
#include "screens/menu/records/RecordsFlow.h"
#include <algorithm>
#include <memory>

void DifficultyScreen::onEnter() {
    easy_.rect = { 16, 95, 100, 100 };
    med_.rect  = { 124, 95, 100, 100 };
    hard_.rect = { 70, 208, 100, 100 };

    easy_.bgRel = med_.bgRel = hard_.bgRel = "sprites/UI/Menu/Buttons/1385.png";
    easy_.bgRelActive = med_.bgRelActive = hard_.bgRelActive = "sprites/UI/Menu/Buttons/1387.png";
    SetupMenuBackButton(back_, 400);
}

static void drawDiffBubble(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& iconRel, const std::string& textRel, const std::string& textRelActive, bool locked) {
    b.draw(*ctx.assets);

    auto icon = ctx.assets->tex(iconRel).tex;
    if (!locked && icon.id) {
        float cx = b.rect.x + b.rect.width * 0.5f;
        float cy = b.rect.y + b.rect.height * 0.5f - 8.0f;
        DrawTexture(icon, (int)(cx - icon.width * 0.5f), (int)(cy - icon.height * 0.5f), WHITE);
    }

    auto txt = ctx.assets->tex((b.hovered || b.pressed) ? textRelActive : textRel).tex;
    if (txt.id) {
        float cx = b.rect.x + b.rect.width * 0.5f;
        float x = cx - txt.width * 0.5f;
        float y = b.rect.y + b.rect.height - 20.0f;
        int vw = ctx.vs ? ctx.vs->vw : 240;
        int vh = ctx.vs ? ctx.vs->vh : 400;
        x = std::max(0.0f, std::min(x, (float)(vw - txt.width)));
        y = std::max(0.0f, std::min(y, (float)(vh - txt.height)));
        DrawTexture(txt, (int)x, (int)y, WHITE);
    }

    if (locked) {
        auto lock = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Buttons/lock.png", "sprites/UI/Menu/Buttons/1353.png")).tex;
        if (lock.id) {
            float cx = b.rect.x + b.rect.width * 0.5f;
            float cy = b.rect.y + b.rect.height * 0.5f;
            DrawTexture(lock, (int)(cx - lock.width * 0.5f), (int)(cy - lock.height * 0.5f), WHITE);
        }
    }
}

void DifficultyScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    bool backClicked = click(back_);
    bool easyClicked = click(easy_);
    bool medClicked = click(med_);
    bool hardClicked = click(hard_);
    bool hardLocked = !ctx.profile->storyCompleted;
    if (hardLocked) {
        hard_.hovered = false;
        hard_.pressed = false;
    }

    if (st.keyBack || backClicked) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    auto openSelected = [&](const char* diff) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (target_ == Target::Gameplay) ctx.app->push(std::make_unique<GameplayPlaceholderScreen>(mode_, diff));
        else ctx.app->push(std::make_unique<RecordsFlow::RecordsScreen>(mode_, diff));
    };

    if (easyClicked) { openSelected("easy"); return; }
    if (medClicked)  { openSelected("med"); return; }
    if (hardClicked) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (hardLocked) { ctx.toast->show("Locked (finish Story)"); return; }
        if (target_ == Target::Gameplay) ctx.app->push(std::make_unique<GameplayPlaceholderScreen>(mode_, "hard"));
        else ctx.app->push(std::make_unique<RecordsFlow::RecordsScreen>(mode_, "hard"));
        return;
    }
}

void DifficultyScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);

    if (mode_ == "free") {
        auto t = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/freemode-btn.png", "sprites/UI/Menu/Russian/0055.png")).tex;
        DrawTexture(t, 4, 4, WHITE);
    } else if (mode_ == "story") {
        auto t = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/freegame-title.png", "sprites/UI/Menu/Russian/start-btn.png")).tex;
        DrawTexture(t, 4, 4, WHITE);
    } else if (mode_ == "dj") {
        auto t = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/djmode-title.png", "sprites/UI/Menu/Russian/start-btn.png")).tex;
        DrawTexture(t, 4, 4, WHITE);
    } else {
        auto t = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/start-btn.png", "sprites/UI/Menu/Russian/0017.png")).tex;
        DrawTexture(t, 4, 4, WHITE);
    }

    drawDiffBubble(ctx,
        easy_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/easy.png", "sprites/UI/Menu/Buttons/1234.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/easy-btn.png", "sprites/UI/Menu/Russian/0024.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/easy-btn.png", "sprites/UI/Menu/Russian/0024.png")),
        false);
    drawDiffBubble(ctx,
        med_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/medium.png", "sprites/UI/Menu/Buttons/1235.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/medium-btn.png", "sprites/UI/Menu/Russian/0025.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/medium-btn.png", "sprites/UI/Menu/Russian/0025.png")),
        false);
    drawDiffBubble(ctx,
        hard_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/hard.png", "sprites/UI/Menu/Buttons/1236.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/hard-btn.png", "sprites/UI/Menu/Russian/0026.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/hard-btn.png", "sprites/UI/Menu/Russian/0026.png")),
        !ctx.profile->storyCompleted);
    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText("DifficultyScreen", 6, (ctx.vs ? ctx.vs->vh : 400) - 12, 12, YELLOW);
}

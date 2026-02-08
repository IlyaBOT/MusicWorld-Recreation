#include "screens/menu/mode/ModeSelectScreen.h"
#include "core/App.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/SpritePath.h"
#include "screens/menu/difficulty/DifficultyScreen.h"
#include <algorithm>
#include <memory>

void ModeSelectScreen::onEnter() {
    dj_.rect = { 11, 101, 100, 100 };
    story_.rect = { 132, 101, 100, 100 };
    free_.rect = { 11, 230, 100, 100 };
    party_.rect = { 132, 230, 100, 100 };

    dj_.bgRel = story_.bgRel = free_.bgRel = party_.bgRel = "sprites/UI/Menu/Buttons/1385.png";
    dj_.bgRelActive = story_.bgRelActive = free_.bgRelActive = party_.bgRelActive = "sprites/UI/Menu/Buttons/1387.png";
    SetupMenuBackButton(back_, 400);
}

void ModeSelectScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    Vector2 hitPoint = st.mouseV;
    hitPoint.y += 4.0f;
    auto click = [&](ui::SpriteButton& b){ return b.update(hitPoint, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    bool backClicked = click(back_);
    bool djClicked = click(dj_);
    bool storyClicked = click(story_);
    bool freeClicked = click(free_);
    bool partyClicked = click(party_);
    bool partyLocked = !ctx.profile->storyCompleted;
    if (partyLocked) {
        party_.hovered = false;
        party_.pressed = false;
    }

    if (st.keyBack || backClicked) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    if (djClicked) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->push(std::make_unique<DifficultyScreen>("dj"));
        return;
    }
    if (storyClicked) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->push(std::make_unique<DifficultyScreen>("story"));
        return;
    }
    if (freeClicked) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        ctx.app->push(std::make_unique<DifficultyScreen>("free"));
        return;
    }
    if (partyClicked) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (partyLocked) ctx.toast->show("Locked (finish Story)");
        else ctx.toast->show("Party mode WIP");
        return;
    }
}

static void drawBubble(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& iconRel, const std::string& textRel, const std::string& textRelActive, bool locked, float textYOffset = 0.0f) {
    b.draw(*ctx.assets);

    auto icon = ctx.assets->tex(iconRel).tex;
    if (!locked && icon.id) {
        float cx = b.rect.x + b.rect.width/2;
        float cy = b.rect.y + b.rect.height/2 - 6;
        DrawTexture(icon, (int)(cx - icon.width/2), (int)(cy - icon.height/2), WHITE);
    }
    auto txt = ctx.assets->tex((b.hovered || b.pressed) ? textRelActive : textRel).tex;
    if (txt.id) {
        float cx = b.rect.x + b.rect.width/2;
        float x = cx - txt.width/2;
        float y = b.rect.y + b.rect.height - 28 + textYOffset;
        int vw = ctx.vs ? ctx.vs->vw : 240;
        int vh = ctx.vs ? ctx.vs->vh : 400;
        x = std::max(0.0f, std::min(x, (float)(vw - txt.width)));
        y = std::max(0.0f, std::min(y, (float)(vh - txt.height)));
        DrawTexture(txt, (int)x, (int)y, WHITE);
    }
    if (locked) {
        auto lock = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Buttons/lock.png", "sprites/UI/Menu/Buttons/1353.png")).tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width/2 - lock.width/2), (int)(b.rect.y + b.rect.height/2 - lock.height/2), WHITE);
        else DrawText("LOCK", (int)b.rect.x + 34, (int)b.rect.y + 44, 16, RED);
    }
}

void ModeSelectScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);
    auto title = ctx.assets->tex(ResolveSpritePath("sprites/UI/Menu/Russian/gamemode-title.png", "sprites/UI/Menu/Russian/0058.png")).tex;
    DrawTexture(title, 4, 4, WHITE);

    drawBubble(ctx,
        dj_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/djmode.png", "sprites/UI/Menu/Buttons/1224.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/djmode-btn.png", "sprites/UI/Menu/Russian/0053.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/djmode-btn.png", "sprites/UI/Menu/Russian/0053.png")),
        false);
    drawBubble(ctx,
        story_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/history.png", "sprites/UI/Menu/Buttons/1225.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/history-btn.png", "sprites/UI/Menu/Russian/0017.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/history-btn.png", "sprites/UI/Menu/Russian/0017.png")),
        false);
    drawBubble(ctx,
        free_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/freemode.png", "sprites/UI/Menu/Buttons/1226.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/freemode-btn.png", "sprites/UI/Menu/Russian/0055.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/freemode-btn.png", "sprites/UI/Menu/Russian/0055.png")),
        false,
        -8.0f);
    drawBubble(ctx,
        party_,
        ResolveSpritePath("sprites/UI/Menu/Buttons/crazymode.png", "sprites/UI/Menu/Buttons/1227.png"),
        ResolveSpritePath("sprites/UI/Menu/Russian/crazymode-btn.png", "sprites/UI/Menu/Russian/0054.png"),
        ResolveSelectedSpritePath(ResolveSpritePath("sprites/UI/Menu/Russian/crazymode-btn.png", "sprites/UI/Menu/Russian/0054.png")),
        !ctx.profile->storyCompleted);
    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText("ModeSelectScreen", 6, (ctx.vs ? ctx.vs->vh : 400) - 12, 12, YELLOW);
}

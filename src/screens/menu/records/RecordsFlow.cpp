#include "screens/menu/records/RecordsFlow.h"
#include "core/App.h"
#include "screens/menu/MenuBg.h"
#include "core/DrawUtil.h"
#include <memory>

namespace RecordsFlow {

// ModeScreen
void ModeScreen::onEnter() {
    story.rect = { 20, 135, 200, 60 };
    free.rect  = { 20, 205, 200, 60 };
    party.rect = { 20, 275, 200, 60 };
    story.bgRel = free.bgRel = party.bgRel = "sprites/menu/buttons/1384.png";
    story.bgRelActive = free.bgRelActive = party.bgRelActive = "sprites/menu/buttons/1384.png";
}

void ModeScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    if (click(story)) { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<DifficultyScreen>("story")); return; }
    if (click(free))  { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<DifficultyScreen>("free")); return; }
    if (click(party)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (!ctx.profile->storyCompleted) ctx.toast->show("Locked (finish Story)");
        else ctx.app->push(std::make_unique<DifficultyScreen>("party"));
        return;
    }
}

static void drawModeRow(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& textRel, bool locked) {
    b.draw(*ctx.assets);
    auto t = ctx.assets->tex(textRel).tex;
    if (t.id) DrawCentered(t, b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2 + 6);
    if (locked) {
        auto lock = ctx.assets->tex("sprites/menu/buttons/1353.png").tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width - lock.width - 18), (int)(b.rect.y + 10), WHITE);
    }
}

void ModeScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);
    auto title = ctx.assets->tex("sprites/menu/russian/0050.png").tex;
    DrawTexture(title, 10, 18, WHITE);
    drawModeRow(ctx, story, "sprites/menu/russian/0017.png", false);
    drawModeRow(ctx, free,  "sprites/menu/russian/0055.png", false);
    drawModeRow(ctx, party, "sprites/menu/russian/0054.png", !ctx.profile->storyCompleted);
    DrawTextCentered("Choose mode", 120, 110, 12, Fade(RAYWHITE, 0.8f));
}

// DifficultyScreen
void DifficultyScreen::onEnter() {
    easy.rect = { 20, 145, 200, 60 };
    med.rect  = { 20, 215, 200, 60 };
    hard.rect = { 20, 285, 200, 60 };
    easy.bgRel = med.bgRel = hard.bgRel = "sprites/menu/buttons/1384.png";
    easy.bgRelActive = med.bgRelActive = hard.bgRelActive = "sprites/menu/buttons/1384.png";
}

void DifficultyScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };

    if (click(easy)) { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<RecordsScreen>(mode, "easy")); return; }
    if (click(med))  { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<RecordsScreen>(mode, "med")); return; }
    if (click(hard)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (!ctx.profile->storyCompleted) { ctx.toast->show("Locked (finish Story)"); return; }
        ctx.app->push(std::make_unique<RecordsScreen>(mode, "hard"));
        return;
    }
}

static void drawDiffRow(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& textRel, bool locked) {
    b.draw(*ctx.assets);
    auto t = ctx.assets->tex(textRel).tex;
    if (t.id) DrawCentered(t, b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2 + 6);
    if (locked) {
        auto lock = ctx.assets->tex("sprites/menu/buttons/1353.png").tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width - lock.width - 18), (int)(b.rect.y + 10), WHITE);
    }
}

void DifficultyScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);
    auto title = ctx.assets->tex("sprites/menu/russian/0050.png").tex;
    DrawTexture(title, 10, 18, WHITE);
    DrawTextCentered("Difficulty", 120, 118, 14, RAYWHITE);
    drawDiffRow(ctx, easy, "sprites/menu/russian/0024.png", false);
    drawDiffRow(ctx, med,  "sprites/menu/russian/0025.png", false);
    drawDiffRow(ctx, hard, "sprites/menu/russian/0026.png", !ctx.profile->storyCompleted);
}

// RecordsScreen
void RecordsScreen::onEnter() {
    btnPrev.rect = { 30, 310, 40, 40 };
    btnNext.rect = { 170, 310, 40, 40 };
    btnPrev.bgRel = "sprites/menu/buttons/1404.png";
    btnNext.bgRel = "sprites/menu/buttons/1411.png";
    carousel.speed = 8.5f;
    level = 1;
}

void RecordsScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }
    carousel.update(ctx.dt);

    auto step = [&](int dir){
        if (carousel.animating()) return;
        int nl = level + dir;
        if (nl < 1) nl = 10;
        if (nl > 10) nl = 1;
        level = nl;
        carousel.start(dir);
        ctx.app->playSfx("sounds/MenuSwitch.wav");
    };

    if (st.swipe == SwipeDir::Left) step(+1);
    if (st.swipe == SwipeDir::Right) step(-1);

    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (click(btnPrev)) step(-1);
    if (click(btnNext)) step(+1);
}

static const char* gradeFor(int score) {
    if (score >= 9000) return "S";
    if (score >= 7000) return "A";
    if (score >= 5000) return "B";
    if (score >= 3000) return "C";
    if (score >= 1000) return "D";
    return "E";
}

void RecordsScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);
    auto title = ctx.assets->tex("sprites/menu/russian/0050.png").tex;
    DrawTexture(title, 10, 18, WHITE);

    Rectangle card = { 30, 90, 180, 190 };
    DrawRectangleRounded(card, 0.12f, 8, Fade(BLACK, 0.25f));
    DrawRectangleRoundedLinesEx(card, 0.12f, 8, 2.0f, Fade(RAYWHITE, 0.35f));

    std::string key = Profile::makeScoreKey(mode, diff, level);
    int score = ctx.profile->getScore(key, 0);

    DrawTextCentered(TextFormat("Level %d/10", level), (int)(card.x + card.width/2), (int)(card.y + 30), 16, RAYWHITE);
    DrawTextCentered(TextFormat("Score: %d", score), (int)(card.x + card.width/2), (int)(card.y + 85), 16, RAYWHITE);
    DrawTextCentered(TextFormat("Grade: %s", gradeFor(score)), (int)(card.x + card.width/2), (int)(card.y + 120), 20, YELLOW);
    DrawTextCentered(TextFormat("%s / %s", mode.c_str(), diff.c_str()), (int)(card.x + card.width/2), (int)(card.y + 160), 12, Fade(RAYWHITE, 0.75f));

    btnPrev.draw(*ctx.assets);
    btnNext.draw(*ctx.assets);
}
} // namespace RecordsFlow

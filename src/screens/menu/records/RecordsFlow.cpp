#include "screens/menu/records/RecordsFlow.h"
#include "core/App.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/difficulty/DifficultyScreen.h"
#include "core/DrawUtil.h"
#include <cctype>
#include <memory>

namespace RecordsFlow {

// ModeScreen
void ModeScreen::onEnter() {
    story.rect = { 20, 135, 200, 60 };
    free.rect  = { 20, 205, 200, 60 };
    party.rect = { 20, 275, 200, 60 };
    story.bgRel = free.bgRel = party.bgRel = "sprites/UI/Menu/Buttons/1384.png";
    story.bgRelActive = free.bgRelActive = party.bgRelActive = "sprites/UI/Menu/Buttons/1384.png";
    SetupMenuBackButton(back, 400);
}

void ModeScreen::update(const UpdateContext& ctx) {
    if (ctx.app) ctx.app->requestMenuMusic();

    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (st.keyBack || click(back)) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    if (click(story)) { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<::DifficultyScreen>("story", ::DifficultyScreen::Target::Records)); return; }
    if (click(free))  { ctx.app->playSfx("sounds/MenuSelect.wav"); ctx.app->push(std::make_unique<::DifficultyScreen>("free", ::DifficultyScreen::Target::Records)); return; }
    if (click(party)) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        if (!ctx.profile->storyCompleted) ctx.toast->show("Locked (finish Story)");
        else ctx.app->push(std::make_unique<::DifficultyScreen>("party", ::DifficultyScreen::Target::Records));
        return;
    }
}

static void drawModeRow(const DrawContext& ctx, const ui::SpriteButton& b, const std::string& textRel, bool locked) {
    b.draw(*ctx.assets);
    auto t = ctx.assets->tex(textRel).tex;
    if (t.id) DrawCentered(t, b.rect.x + b.rect.width/2, b.rect.y + b.rect.height/2 + 6);
    if (locked) {
        auto lock = ctx.assets->tex("sprites/UI/Menu/Buttons/1353.png").tex;
        if (lock.id) DrawTexture(lock, (int)(b.rect.x + b.rect.width - lock.width - 18), (int)(b.rect.y + 10), WHITE);
    }
}

void ModeScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);
    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/0050.png").tex;
    DrawTexture(title, 4, 4, WHITE);
    drawModeRow(ctx, story, "sprites/UI/Menu/Russian/0017.png", false);
    drawModeRow(ctx, free,  "sprites/UI/Menu/Russian/0055.png", false);
    drawModeRow(ctx, party, "sprites/UI/Menu/Russian/0054.png", !ctx.profile->storyCompleted);
    back.draw(*ctx.assets);
    DrawTextCentered("Choose mode", 120, 110, 12, Fade(RAYWHITE, 0.8f));
}

// RecordsScreen
void RecordsScreen::onEnter() {
    btnPrev.rect = { 8, 186, 34, 34 };
    btnNext.rect = { 198, 186, 34, 34 };
    btnPrev.bgRel = btnNext.bgRel = "sprites/UI/Menu/Buttons/1393.png";
    btnPrev.bgRelActive = btnNext.bgRelActive = "sprites/UI/Menu/Buttons/1395.png";
    btnPrev.labelRel = "sprites/UI/Menu/Buttons/backward-btn.png";
    btnPrev.labelRelActive = "sprites/UI/Menu/Buttons/backward-btn_selected.png";
    btnNext.labelRel = "sprites/UI/Menu/Buttons/forward-btn.png";
    btnNext.labelRelActive = "sprites/UI/Menu/Buttons/forward-btn_selected.png";
    SetupMenuBackButton(back, 400);
    carousel.speed = 8.5f;
    level = 1;
}

void RecordsScreen::update(const UpdateContext& ctx) {
    if (ctx.app) ctx.app->requestMenuMusic();

    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (st.keyBack || click(back)) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }
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

    if (click(btnPrev)) step(-1);
    if (click(btnNext)) step(+1);
}

static int wrapLevel(int v) {
    while (v < 1) v += 10;
    while (v > 10) v -= 10;
    return v;
}

static const char* gradeSpriteRel(char grade) {
    switch (std::toupper((unsigned char)grade)) {
    case 'S': return "sprites/UI/Scores/S-score-letter.png";
    case 'A': return "sprites/UI/Scores/A-score-letter.png";
    case 'B': return "sprites/UI/Scores/B-score-letter.png";
    case 'C': return "sprites/UI/Scores/C-score-letter.png";
    case 'D': return "sprites/UI/Scores/D-score-letter.png";
    case 'E': return "sprites/UI/Scores/E-score-letter.png";
    default: return "sprites/UI/Scores/F-score-letter.png";
    }
}

static const char* diffIconRel(const std::string& diff) {
    if (diff == "easy") return "sprites/UI/Menu/Buttons/easy.png";
    if (diff == "med") return "sprites/UI/Menu/Buttons/medium.png";
    return "sprites/UI/Menu/Buttons/hard.png";
}

static const char* diffLabelRel(const std::string& diff) {
    if (diff == "easy") return "sprites/UI/Menu/Russian/easy-btn.png";
    if (diff == "med") return "sprites/UI/Menu/Russian/medium-btn.png";
    return "sprites/UI/Menu/Russian/hard-btn.png";
}

static const char* levelNameRel(int level) {
    static constexpr const char* kNames[] = {
        "sprites/UI/Menu/Russian/tracks/morning-dew.png",
        "sprites/UI/Menu/Russian/tracks/picnic.png",
        "sprites/UI/Menu/Russian/tracks/kiss-of-flowercula.png",
        "sprites/UI/Menu/Russian/tracks/jungle-drum.png",
        "sprites/UI/Menu/Russian/tracks/worship-the-wild.png",
        "sprites/UI/Menu/Russian/tracks/konggas-dance.png",
        "sprites/UI/Menu/Russian/tracks/future-city.png",
        "sprites/UI/Menu/Russian/tracks/electrical-parade.png",
        "sprites/UI/Menu/Russian/tracks/space-invader.png",
        "sprites/UI/Menu/Russian/tracks/prince-shutter.png",
    };
    return kNames[wrapLevel(level) - 1];
}

static const char* levelPreviewRel(int level) {
    static constexpr const char* kPreview[] = {
        "sprites/LevelBackgrounds/0585.png",
        "sprites/LevelBackgrounds/0580.png",
        "sprites/LevelBackgrounds/0580.png",
        "sprites/LevelBackgrounds/0581.png",
        "sprites/LevelBackgrounds/0582.png",
        "sprites/LevelBackgrounds/0582.png",
        "sprites/LevelBackgrounds/0583.png",
        "sprites/LevelBackgrounds/0584.png",
        "sprites/LevelBackgrounds/0584.png",
        "sprites/LevelBackgrounds/0586.png",
    };
    return kPreview[wrapLevel(level) - 1];
}

static Texture2D statDigit(const DrawContext& ctx, char ch) {
    if (ch >= '0' && ch <= '9') {
        char rel[64];
        snprintf(rel, sizeof(rel), "sprites/UI/Fonts/%04d.png", 850 + (ch - '0'));
        return ctx.assets->tex(rel).tex;
    }
    if (ch == '/') return ctx.assets->tex("sprites/UI/Fonts/1002.png").tex;
    return {};
}

static int measureDigits(const DrawContext& ctx, const std::string& s) {
    int w = 0;
    for (char ch : s) {
        Texture2D g = statDigit(ctx, ch);
        if (!g.id) continue;
        w += g.width + 1;
    }
    if (w > 0) --w;
    return w;
}

static void drawDigits(const DrawContext& ctx, const std::string& s, float x, float y) {
    float px = x;
    for (char ch : s) {
        Texture2D g = statDigit(ctx, ch);
        if (!g.id) continue;
        DrawTexture(g, (int)px, (int)y, WHITE);
        px += g.width + 1.0f;
    }
}

static void drawDigitsRight(const DrawContext& ctx, int value, float rightX, float y) {
    std::string s = std::to_string(std::max(0, value));
    int w = measureDigits(ctx, s);
    drawDigits(ctx, s, rightX - w, y);
}

static void drawPreview(Texture2D tex, Rectangle dst) {
    DrawRectangleRounded(dst, 0.08f, 6, Fade(BLACK, 0.2f));
    DrawRectangleRoundedLinesEx(dst, 0.08f, 6, 1.0f, Fade(RAYWHITE, 0.45f));
    if (!tex.id || tex.width <= 0 || tex.height <= 0) return;

    float srcSize = (float)std::min(tex.width, tex.height);
    float srcX = ((float)tex.width - srcSize) * 0.5f;
    float srcY = ((float)tex.height - srcSize) * 0.5f;
    Rectangle src = {srcX, srcY, srcSize, srcSize};
    Rectangle inner = {dst.x + 2.0f, dst.y + 2.0f, dst.width - 4.0f, dst.height - 4.0f};
    DrawTexturePro(tex, src, inner, {0, 0}, 0.0f, WHITE);
}

static void drawRecordsCard(const DrawContext& ctx, const std::string& mode, const std::string& diff, int level, float xOffset) {
    level = wrapLevel(level);
    Rectangle card = { 20.0f + xOffset, 72.0f, 200.0f, 286.0f };

    DrawRectangleRounded(card, 0.08f, 10, Color{129, 66, 206, 255});
    DrawRectangleRoundedLinesEx(card, 0.08f, 10, 3.0f, Color{236, 226, 255, 245});
    Rectangle inner = { card.x + 3.0f, card.y + 3.0f, card.width - 6.0f, card.height - 6.0f };
    DrawRectangleRounded(inner, 0.08f, 10, Color{122, 58, 200, 255});
    DrawLine((int)(inner.x + 8), (int)(inner.y + inner.height - 8), (int)(inner.x + inner.width - 8), (int)(inner.y + inner.height - 8), Color{89, 40, 158, 230});

    Rectangle preview = { card.x + 58.0f, card.y + 20.0f, 84.0f, 84.0f };
    drawPreview(ctx.assets->tex(levelPreviewRel(level)).tex, preview);

    std::string lvlText = std::to_string(level) + "/10";
    drawDigits(ctx, lvlText, preview.x + preview.width - measureDigits(ctx, lvlText), preview.y + preview.height - 16.0f);

    auto name = ctx.assets->tex(levelNameRel(level)).tex;
    if (name.id) DrawCentered(name, card.x + card.width * 0.5f, preview.y + preview.height + 16.0f);

    std::string key = Profile::makeScoreKey(mode, diff, level);
    Profile::Record rec = ctx.profile->getRecord(key);
    int score = rec.score;

    float leftX = card.x + 30.0f;
    float rightX = card.x + card.width - 18.0f;
    float yAssessment = card.y + 132.0f;
    float yScore = card.y + 160.0f;
    float yGood = card.y + 188.0f;
    float yPerfect = card.y + 216.0f;
    float yCombo = card.y + 244.0f;
    float yMaxCombo = card.y + 268.0f;

    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/assessment-title.png").tex, leftX, yAssessment);
    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/score-title.png").tex, leftX, yScore + 2.0f);
    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/good-title.png").tex, leftX, yGood);
    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/perfect-title.png").tex, leftX, yPerfect + 2.0f);
    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/combo-title.png").tex, leftX, yCombo + 2.0f);
    DrawAt(ctx.assets->tex("sprites/UI/Menu/Russian/max-combo-title.png").tex, leftX, yMaxCombo);

    auto gradeTex = ctx.assets->tex(gradeSpriteRel(rec.grade)).tex;
    if (gradeTex.id) {
        DrawTexture(gradeTex, (int)(rightX - gradeTex.width), (int)(yAssessment - 2), WHITE);
    } else {
        char gradeText[2] = {rec.grade, '\0'};
        DrawText(gradeText, (int)(rightX - 28), (int)(yAssessment), 36, WHITE);
    }

    drawDigitsRight(ctx, score, rightX, yScore + 4.0f);
    drawDigitsRight(ctx, rec.good, rightX, yGood + 4.0f);
    drawDigitsRight(ctx, rec.perfect, rightX, yPerfect + 4.0f);
    drawDigitsRight(ctx, rec.combo, rightX, yCombo + 4.0f);
    drawDigitsRight(ctx, rec.maxCombo, rightX, yMaxCombo + 8.0f);
}

void RecordsScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);
    int vw = ctx.vs ? ctx.vs->vw : 240;

    auto recordsTitle = ctx.assets->tex("sprites/UI/Menu/Russian/records-title.png").tex;
    if (recordsTitle.id) DrawTexture(recordsTitle, vw - recordsTitle.width - 4, 6, WHITE);

    Vector2 diffBubbleCenter = {18.0f, 24.0f};
    DrawCircleV(diffBubbleCenter, 24.0f, Color{171, 113, 237, 255});
    DrawCircleLines((int)diffBubbleCenter.x, (int)diffBubbleCenter.y, 24.0f, Fade(RAYWHITE, 0.7f));
    auto diffIcon = ctx.assets->tex(diffIconRel(diff)).tex;
    if (diffIcon.id) DrawTexture(diffIcon, (int)(diffBubbleCenter.x - diffIcon.width * 0.5f), (int)(diffBubbleCenter.y - diffIcon.height * 0.5f), WHITE);
    auto diffLabel = ctx.assets->tex(diffLabelRel(diff)).tex;
    if (diffLabel.id) DrawTexture(diffLabel, 2, 52, WHITE);

    if (carousel.animating()) {
        float span = 212.0f;
        int prevLevel = wrapLevel(level - carousel.dir);
        float off = carousel.offsetPx(span);
        drawRecordsCard(ctx, mode, diff, prevLevel, off);
        drawRecordsCard(ctx, mode, diff, level, off + carousel.dir * span);
    } else {
        drawRecordsCard(ctx, mode, diff, level, 0.0f);
    }

    btnPrev.draw(*ctx.assets);
    btnNext.draw(*ctx.assets);
    back.draw(*ctx.assets);
}
} // namespace RecordsFlow

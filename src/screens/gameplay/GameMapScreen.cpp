#include "screens/gameplay/GameMapScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/BackButton.h"
#include <algorithm>

namespace {
constexpr int kLevelsCount = 10;
constexpr const char* kMapBgRel = "sprites/LevelBackgrounds/1429.png";
}

void GameMapScreen::onEnter() {
    SetupMenuBackButton(back_, 400);
    storyCompleted_ = false;
    bestReachedLevel_ = 1;
    unlockedLevels_ = 1;
}

void GameMapScreen::refreshProgress(const Profile& profile) {
    storyCompleted_ = profile.storyCompleted;

    int bestScoredLevel = 0;
    for (int level = 1; level <= kLevelsCount; ++level) {
        std::string key = Profile::makeScoreKey(mode_, diff_, level);
        auto rec = profile.getRecord(key);
        if (rec.score > 0) bestScoredLevel = level;
    }

    bestReachedLevel_ = std::max(1, bestScoredLevel);
    unlockedLevels_ = std::clamp(bestScoredLevel + 1, 1, kLevelsCount);
}

void GameMapScreen::update(const UpdateContext& ctx) {
    refreshProgress(*ctx.profile);

    auto st = ctx.input->state();
    bool clickedBack = back_.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport);
    if (st.keyBack || clickedBack) {
        ctx.app->pop();
        ctx.app->playSfx("sounds/MenuBack.wav");
        return;
    }
}

void GameMapScreen::draw(const DrawContext& ctx) {
    int vw = ctx.vs ? ctx.vs->vw : 240;
    int vh = ctx.vs ? ctx.vs->vh : 400;

    auto bg = ctx.assets->tex(kMapBgRel).tex;
    if (bg.id) {
        DrawTexturePro(
            bg,
            {0.0f, 0.0f, (float)bg.width, (float)bg.height},
            {0.0f, 0.0f, (float)vw, (float)vh},
            {0.0f, 0.0f},
            0.0f,
            WHITE);
    } else {
        ClearBackground(BLACK);
    }

    back_.draw(*ctx.assets);

    if (ctx.debug) {
        DrawRectangle(0, vh - 26, vw, 26, Fade(BLACK, 0.45f));
        DrawText(TextFormat("GameMap mode=%s diff=%s unlock=%d/%d best=%d story=%d",
                            mode_.c_str(), diff_.c_str(),
                            unlockedLevels_, kLevelsCount, bestReachedLevel_, storyCompleted_ ? 1 : 0),
                 4, vh - 18, 10, YELLOW);
    }
}


#include "screens/menu/main/MenuMainScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/mode/ModeSelectScreen.h"
#include "screens/menu/player/PlayerScreen.h"
#include "screens/menu/help/HelpScreen.h"
#include "screens/menu/settings/SettingsScreen.h"
#include "screens/menu/records/RecordsFlow.h"
#include <memory>

static ui::SpriteButton makeItem(float y, const std::string& labelRel) {
    ui::SpriteButton b;
    b.rect = { 22, y, 196, 36 };
    b.bgRel = "sprites/UI/Menu/Buttons/1359.png";
    b.bgRelActive = "sprites/UI/Menu/Buttons/1360.png";
    b.labelRel = labelRel;
    return b;
}

void MenuMainScreen::onEnter() {
    items_.clear();
    items_.push_back(makeItem(122, "sprites/UI/Menu/Russian/0004.png")); // Game
    items_.push_back(makeItem(162, "sprites/UI/Menu/Russian/0005.png")); // Player
    items_.push_back(makeItem(202, "sprites/UI/Menu/Russian/0006.png")); // Help
    items_.push_back(makeItem(242, "sprites/UI/Menu/Russian/0007.png")); // Settings
    items_.push_back(makeItem(282, "sprites/UI/Menu/Russian/0008.png")); // Records
    items_.push_back(makeItem(322, "sprites/UI/Menu/Russian/0036.png")); // Exit
    focus_ = 0;
}

void MenuMainScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();

    for (int i = 0; i < (int)items_.size(); ++i) {
        if (items_[i].update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport)) {
            focus_ = i;
            ctx.app->playSfx("sounds/MenuSelect.wav");

            if (i == 0) ctx.app->push(std::make_unique<ModeSelectScreen>());
            if (i == 1) ctx.app->push(std::make_unique<PlayerScreen>());
            if (i == 2) ctx.app->push(std::make_unique<HelpScreen>());
            if (i == 3) ctx.app->push(std::make_unique<SettingsScreen>());
            if (i == 4) ctx.app->push(std::make_unique<RecordsFlow::ModeScreen>());
            if (i == 5) CloseWindow();
            return;
        }
    }

    if (st.keyBack) CloseWindow();
}

void MenuMainScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/0046.png").tex; // "МЕНЮ"
    DrawAt(title, 10, 18);

    for (int i = 0; i < (int)items_.size(); ++i) {
        auto& b = items_[i];
        b.hovered = b.hovered || (i == focus_);
        b.draw(*ctx.assets);
    }
    if (ctx.debug) DrawText("MenuMainScreen", 6, 384, 12, YELLOW);
}

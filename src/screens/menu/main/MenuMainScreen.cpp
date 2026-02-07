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
#include <string>

static ui::SpriteButton makeItem(float y, const std::string& labelRel) {
    auto labelSelected = [&]() {
        constexpr const char* kExt = ".png";
        if (labelRel.size() <= 4 || labelRel.rfind(kExt) != labelRel.size() - 4) return labelRel;
        std::string candidate = labelRel.substr(0, labelRel.size() - 4) + "_selected.png";
        return FileExists(Assets::A(candidate).c_str()) ? candidate : labelRel;
    };

    ui::SpriteButton b;
    b.rect = { 22, y, 196, 36 };
    b.bgRel = "sprites/UI/Menu/Buttons/1359.png";
    b.bgRelActive = "sprites/UI/Menu/Buttons/1363.png";
    b.labelRel = labelRel;
    b.labelRelActive = labelSelected();
    return b;
}

void MenuMainScreen::onEnter() {
    items_.clear();
    items_.push_back(makeItem(122, "sprites/UI/Menu/Russian/game-btn.png")); // Game
    items_.push_back(makeItem(162, "sprites/UI/Menu/Russian/player-btn.png")); // Player
    items_.push_back(makeItem(202, "sprites/UI/Menu/Russian/help-btn.png")); // Help
    items_.push_back(makeItem(242, "sprites/UI/Menu/Russian/settings-btn.png")); // Settings
    items_.push_back(makeItem(282, "sprites/UI/Menu/Russian/records-btn.png")); // Records
    items_.push_back(makeItem(322, "sprites/UI/Menu/Russian/exit-btn.png")); // Exit
}

void MenuMainScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();

    for (int i = 0; i < (int)items_.size(); ++i) {
        if (items_[i].update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport)) {
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
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 360);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/menu-title.png").tex; // "МЕНЮ"
    DrawAt(title, 4, 4);

    for (int i = 0; i < (int)items_.size(); ++i) {
        items_[i].draw(*ctx.assets);
    }
    if (ctx.debug) DrawText("MenuMainScreen", 6, (ctx.vs ? ctx.vs->vh : 360) - 12, 12, YELLOW);
}

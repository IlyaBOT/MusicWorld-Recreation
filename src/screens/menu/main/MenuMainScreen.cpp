#include "screens/menu/main/MenuMainScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/MenuBg.h"
#include "screens/menu/mode/ModeSelectScreen.h"
#include "screens/menu/player/PlayerScreen.h"
#include "screens/menu/help/HelpScreen.h"
#include "screens/menu/settings/SettingsScreen.h"
#include <memory>
#include <string>

static bool canLoadMenuTexture(const std::string& rel) {
    Image img = LoadImage(Assets::A(rel).c_str());
    bool ok = (img.data != nullptr);
    if (ok) UnloadImage(img);
    return ok;
}

static ui::SpriteButton makeItem(float y, const std::string& labelRel) {
    auto labelSelected = [&]() {
        constexpr const char* kExt = ".png";
        if (labelRel.size() <= 4 || labelRel.rfind(kExt) != labelRel.size() - 4) return labelRel;
        std::string candidate = labelRel.substr(0, labelRel.size() - 4) + "_selected.png";
        return canLoadMenuTexture(candidate) ? candidate : labelRel;
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
    constexpr float kMenuButtonYOffset = 8.0f;
    items_.clear();
    items_.push_back(makeItem(86 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/game-btn.png")); // Game
    items_.push_back(makeItem(126 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/player-btn.png")); // Player
    items_.push_back(makeItem(166 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/help-btn.png")); // Help
    items_.push_back(makeItem(206 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/settings-btn.png")); // Settings
    items_.push_back(makeItem(246 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/records-btn.png")); // Records
    items_.push_back(makeItem(286 + kMenuButtonYOffset, "sprites/UI/Menu/Russian/exit-btn.png")); // Exit
}

void MenuMainScreen::update(const UpdateContext& ctx) {
    if (ctx.app) ctx.app->requestMenuMusic();

    auto st = ctx.input->state();
    Vector2 hitPoint = st.mouseV;
    hitPoint.y += 4.0f;

    for (int i = 0; i < (int)items_.size(); ++i) {
        if (items_[i].update(hitPoint, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport)) {
            ctx.app->playSfx("sounds/MenuSelect.wav");

            if (i == 0) ctx.app->push(std::make_unique<ModeSelectScreen>());
            if (i == 1) ctx.app->push(std::make_unique<PlayerScreen>());
            if (i == 2) ctx.app->push(std::make_unique<HelpScreen>());
            if (i == 3) ctx.app->push(std::make_unique<SettingsScreen>());
            if (i == 4) ctx.app->push(std::make_unique<ModeSelectScreen>(ModeSelectScreen::Target::Records));
            if (i == 5) ctx.app->requestQuit();
            return;
        }
    }

    if (st.keyBack) ctx.app->requestQuit();
}

void MenuMainScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/menu-title.png").tex; // "МЕНЮ"
    DrawAt(title, 4, 4);

    for (int i = 0; i < (int)items_.size(); ++i) {
        items_[i].draw(*ctx.assets);
    }
    if (ctx.debug) DrawText("MenuMainScreen", 6, (ctx.vs ? ctx.vs->vh : 400) - 12, 12, YELLOW);
}

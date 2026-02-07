#include "screens/menu/help/HelpScreen.h"
#include "core/App.h"
#include "screens/menu/MenuBg.h"
#include "core/DrawUtil.h"
#include <filesystem>
#include <algorithm>

static std::vector<std::string> listPng(const std::string& dir) {
    std::vector<std::string> out;
    namespace fs = std::filesystem;
    if (!fs::exists(dir)) return out;
    for (auto& e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        auto p = e.path();
        auto ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png") out.push_back(p.filename().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

void HelpScreen::scanPages() {
    pages_.clear();
    auto files = listPng("assets/sprites/help");
    for (auto& fn : files) pages_.push_back(std::string("sprites/help/") + fn);
    if (pages_.empty()) pages_.push_back("sprites/help/page_01.png");
}

void HelpScreen::onEnter() {
    scanPages();
    idx_ = 0;
    btnPrev_.rect = { 30, 310, 40, 40 };
    btnNext_.rect = { 170, 310, 40, 40 };
    btnPrev_.bgRel = "sprites/UI/Menu/Buttons/1404.png";
    btnNext_.bgRel = "sprites/UI/Menu/Buttons/1411.png";
    carousel_.speed = 8.5f;
}

void HelpScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    if (st.keyBack) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    carousel_.update(ctx.dt);

    auto step = [&](int dir){
        if (carousel_.animating()) return;
        idx_ = (idx_ + dir + (int)pages_.size()) % (int)pages_.size();
        carousel_.start(dir);
        ctx.app->playSfx("sounds/MenuSwitch.wav");
    };

    if (st.swipe == SwipeDir::Left) step(+1);
    if (st.swipe == SwipeDir::Right) step(-1);

    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (click(btnPrev_)) step(-1);
    if (click(btnNext_)) step(+1);
}

void HelpScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/0051.png").tex;
    DrawTexture(title, 10, 18, WHITE);

    Rectangle card = { 20, 90, 200, 200 };
    DrawRectangleRounded(card, 0.10f, 8, Fade(BLACK, 0.22f));
    DrawRectangleRoundedLinesEx(card, 0.10f, 8, 2.0f, Fade(RAYWHITE, 0.35f));

    auto page = ctx.assets->tex(pages_[idx_]).tex;
    if (page.id) DrawCentered(page, card.x + card.width/2, card.y + card.height/2);
    else {
        DrawTextCentered("Help page missing", (int)(card.x+card.width/2), (int)(card.y+80), 16, RAYWHITE);
        DrawText("Put PNGs into assets/sprites/help/", 28, 180, 12, GRAY);
    }

    btnPrev_.draw(*ctx.assets);
    btnNext_.draw(*ctx.assets);

    DrawText(TextFormat("%d/%d", idx_+1, (int)pages_.size()), 110, 292, 14, RAYWHITE);
    if (ctx.debug) DrawText("HelpScreen", 6, 384, 12, YELLOW);
}

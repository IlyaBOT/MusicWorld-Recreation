#include "screens/menu/player/PlayerScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include <filesystem>
#include <algorithm>

static std::vector<std::string> listAudio(const std::string& dir) {
    std::vector<std::string> out;
    namespace fs = std::filesystem;
    if (!fs::exists(dir)) return out;
    for (auto& e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        auto p = e.path();
        auto ext = p.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".mp3" || ext == ".ogg" || ext == ".wav") out.push_back(p.filename().string());
    }
    std::sort(out.begin(), out.end());
    return out;
}

void PlayerScreen::scanTracks() {
    tracks_.clear();
    auto files = listAudio("assets/music/levels");
    int n = 0;
    for (auto& fn : files) {
        n++;
        Track t;
        t.fileRel = std::string("music/levels/") + fn;

        int nameId = 100 + (n - 1);
        char buf[64];
        snprintf(buf, sizeof(buf), "sprites/UI/Menu/Russian/%04d.png", nameId);
        t.namePngRel = buf;

        char ibuf[64];
        snprintf(ibuf, sizeof(ibuf), "sprites/player/trackicon_%02d.png", n);
        t.iconRel = ibuf;

        t.titleText = fn;
        tracks_.push_back(std::move(t));
    }
    if (tracks_.empty()) {
        Track t;
        t.fileRel = "music/levels/01-Example.mp3";
        t.namePngRel = "sprites/UI/Menu/Russian/0100.png";
        t.iconRel = "sprites/player/trackicon_01.png";
        t.titleText = "No tracks in assets/music/levels/";
        tracks_.push_back(t);
    }
}

void PlayerScreen::loadTrack(int i) {
    if (i < 0 || i >= (int)tracks_.size()) return;

    if (musicOk_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        musicOk_ = false;
    }

    std::string path = Assets::A(tracks_[i].fileRel);
    if (FileExists(path.c_str())) {
        music_ = LoadMusicStream(path.c_str());
        musicOk_ = (music_.ctxData != nullptr);
        if (musicOk_) SetMusicVolume(music_, 0.9f);
    }
    playing_ = false;
}

void PlayerScreen::onEnter() {
    scanTracks();
    btnPrev_.rect = { 30, 310, 40, 40 };
    btnNext_.rect = { 170, 310, 40, 40 };
    btnPlay_.rect = { 95, 300, 50, 50 };

    btnPrev_.bgRel = "sprites/UI/Menu/Buttons/1404.png";
    btnNext_.bgRel = "sprites/UI/Menu/Buttons/1411.png";
    btnPlay_.bgRel = "sprites/UI/Menu/Buttons/1319.png";
    SetupMenuBackButton(back_);

    carousel_.speed = 8.5f;

    idx_ = 0;
    loadTrack(idx_);
}

void PlayerScreen::onExit() {
    if (musicOk_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        musicOk_ = false;
    }
}

void PlayerScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (st.keyBack || click(back_)) { ctx.app->pop(); ctx.app->playSfx("sounds/MenuBack.wav"); return; }

    if (musicOk_) UpdateMusicStream(music_);
    carousel_.update(ctx.dt);

    auto step = [&](int dir){
        if (carousel_.animating()) return;
        idx_ = (idx_ + dir + (int)tracks_.size()) % (int)tracks_.size();
        carousel_.start(dir);
        loadTrack(idx_);
        ctx.app->playSfx("sounds/MenuSwitch.wav");
    };

    if (st.swipe == SwipeDir::Left) step(+1);
    if (st.swipe == SwipeDir::Right) step(-1);

    if (click(btnPrev_)) step(-1);
    if (click(btnNext_)) step(+1);

    if (click(btnPlay_)) {
        if (!musicOk_) { ctx.toast->show("Missing audio file"); return; }
        if (!playing_) { PlayMusicStream(music_); playing_ = true; }
        else { PauseMusicStream(music_); playing_ = false; }
        ctx.app->playSfx("sounds/MenuSelect.wav");
    }
}

static void drawEq(const DrawContext& ctx, float x, float y) {
    auto seg = ctx.assets->tex("sprites/UI/Menu/Buttons/1323.png").tex;
    if (!seg.id) { DrawText("EQ missing 1323.png", (int)x, (int)y, 12, YELLOW); return; }
    for (int i=0;i<10;i++) {
        float dy = (float)((i*37 + (int)(GetTime()*60)) % 12);
        DrawTexture(seg, (int)(x + i*(seg.width+1)), (int)(y + dy), WHITE);
    }
}

void PlayerScreen::draw(const DrawContext& ctx) {
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 360);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/0049.png").tex;
    DrawTexture(title, 4, 4, WHITE);

    Rectangle card = { 30, 90, 180, 190 };
    DrawRectangleRounded(card, 0.12f, 8, Fade(BLACK, 0.25f));
    DrawRectangleRoundedLinesEx(card, 0.12f, 8, 2.0f, Fade(RAYWHITE, 0.35f));

    const auto& t = tracks_[idx_];

    auto icon = ctx.assets->tex(t.iconRel).tex;
    if (icon.id) DrawCentered(icon, card.x + card.width/2, card.y + 60);
    else DrawTextCentered("track icon", (int)(card.x + card.width/2), (int)(card.y + 54), 12, GRAY);

    auto name = ctx.assets->tex(t.namePngRel).tex;
    if (name.id) DrawCentered(name, card.x + card.width/2, card.y + 110);
    else DrawTextCentered(t.titleText.c_str(), (int)(card.x + card.width/2), (int)(card.y + 105), 12, RAYWHITE);

    drawEq(ctx, card.x + 20, card.y + 140);

    btnPrev_.draw(*ctx.assets);
    btnNext_.draw(*ctx.assets);
    btnPlay_.bgRel = playing_ ? "sprites/UI/Menu/Buttons/1320.png" : "sprites/UI/Menu/Buttons/1319.png";
    btnPlay_.draw(*ctx.assets);
    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText(TextFormat("tracks=%d idx=%d", (int)tracks_.size(), idx_), 6, (ctx.vs ? ctx.vs->vh : 360) - 12, 12, YELLOW);
}

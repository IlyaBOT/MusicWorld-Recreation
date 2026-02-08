#include "screens/menu/player/PlayerScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/BackButton.h"
#include "screens/menu/MenuBg.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <limits>

namespace {
constexpr float kTrackCoverSize = 80.0f;
constexpr float kTrackCoverFrameSize = 104.0f;
constexpr float kTrackCoverTopInset = 14.0f;
constexpr float kPlayerCardX = 24.0f;
constexpr int kTrackCount = 10;
constexpr float kDebugNudgeStep = 1.0f;

struct TrackOrderEntry {
    const char* key;
    int rank;
    const char* nameRel;
};

static Vector2 gDebugTrackNudge[kTrackCount] = {};

static int wrapTrackIndex(int index) {
    int i = index % kTrackCount;
    if (i < 0) i += kTrackCount;
    return i;
}

static constexpr TrackOrderEntry kTrackOrder[] = {
    {"MorningDew", 0, "sprites/UI/Menu/Russian/tracks/morning-dew.png"},
    {"Picnic", 1, "sprites/UI/Menu/Russian/tracks/picnic.png"},
    {"KissOfFlowercula", 2, "sprites/UI/Menu/Russian/tracks/kiss-of-flowercula.png"},
    {"JungleDrum", 3, "sprites/UI/Menu/Russian/tracks/jungle-drum.png"},
    {"WorshipTheWild", 4, "sprites/UI/Menu/Russian/tracks/worship-the-wild.png"},
    {"Kongga_sDance", 5, "sprites/UI/Menu/Russian/tracks/konggas-dance.png"},
    {"FutureCity", 6, "sprites/UI/Menu/Russian/tracks/future-city.png"},
    {"ElectricalParade", 7, "sprites/UI/Menu/Russian/tracks/electrical-parade.png"},
    {"SpaceInvader", 8, "sprites/UI/Menu/Russian/tracks/space-invader.png"},
    {"PrinceShutter", 9, "sprites/UI/Menu/Russian/tracks/prince-shutter.png"},
};

int extractTrackNumberPrefix(const std::string& s) {
    int value = 0;
    bool any = false;
    for (char ch : s) {
        if (!std::isdigit((unsigned char)ch)) break;
        any = true;
        value = value * 10 + (ch - '0');
    }
    return any ? value : std::numeric_limits<int>::max();
}

int desiredTrackRank(const std::string& filename) {
    for (const auto& entry : kTrackOrder) {
        if (filename.find(entry.key) != std::string::npos) return entry.rank;
    }
    return std::numeric_limits<int>::max();
}

const char* desiredTrackNameRel(const std::string& filename) {
    for (const auto& entry : kTrackOrder) {
        if (filename.find(entry.key) != std::string::npos) return entry.nameRel;
    }
    return nullptr;
}
}

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
    std::sort(out.begin(), out.end(), [](const std::string& a, const std::string& b) {
        int ra = desiredTrackRank(a);
        int rb = desiredTrackRank(b);
        if (ra != rb) return ra < rb;

        int na = extractTrackNumberPrefix(a);
        int nb = extractTrackNumberPrefix(b);
        if (na != nb) return na < nb;
        return a < b;
    });
    return out;
}

struct TrackBgLayout {
    const char* rel;
    float offsetX;
    float offsetY;
    float zoom;
};

static TrackBgLayout trackBackgroundLayout(int trackIndex) {
    constexpr float kZoomDefault = 1.0f;
    constexpr float kZoomTight = 1.0f;
    switch (trackIndex) {
    case 0: return {"sprites/LevelBackgrounds/0580.png", 0.0f, -20.0f, kZoomTight};
    case 1: return {"sprites/LevelBackgrounds/0581.png", 81.0f, 62.0f, kZoomTight};
    case 2: return {"sprites/LevelBackgrounds/0581.png", 32.0f, 3.0f, kZoomTight};
    case 3: return {"sprites/LevelBackgrounds/0582.png", 41.0f, -112.0f, 1.0f};
    case 4: return {"sprites/LevelBackgrounds/0583.png", 17.0f, 59.0f, 1.0f};
    case 5: return {"sprites/LevelBackgrounds/0583.png", 3.0f, -73.0f, 1.0f};
    case 6: return {"sprites/LevelBackgrounds/0584.png", 1.0f, -85.0f, 1.0f};
    case 7: return {"sprites/LevelBackgrounds/0585.png", -42.0f, -34.0f, kZoomTight};
    case 8: return {"sprites/LevelBackgrounds/0585.png", 7.0f, -111.0f, 1.0f};
    case 9: return {"sprites/LevelBackgrounds/0586.png", 33.0f, -22.0f, 1.0f};
    default: return {nullptr, 0.0f, 0.0f, kZoomDefault};
    }
}

static const char* track10AnimatedBackgroundRel() {
    static constexpr const char* kFrames[] = {
        "sprites/LevelBackgrounds/0586.png",
        "sprites/LevelBackgrounds/0587.png",
        "sprites/LevelBackgrounds/0588.png",
        "sprites/LevelBackgrounds/0589.png",
        "sprites/LevelBackgrounds/0590.png",
        "sprites/LevelBackgrounds/0591.png",
        "sprites/LevelBackgrounds/0592.png",
    };
    constexpr int kFrameCount = (int)(sizeof(kFrames) / sizeof(kFrames[0]));
    int frame = ((int)(GetTime() * 8.0f)) % kFrameCount;
    return kFrames[frame];
}

static TrackBgLayout adjustedTrackBackgroundLayout(int trackIndex) {
    int i = wrapTrackIndex(trackIndex);
    TrackBgLayout layout = trackBackgroundLayout(i);
    layout.offsetX += gDebugTrackNudge[i].x;
    layout.offsetY += gDebugTrackNudge[i].y;
    return layout;
}

static void printTrackBackgroundDebugLine(const UpdateContext& ctx, int trackIndex, const std::string& trackFileRel) {
    int i = wrapTrackIndex(trackIndex);
    TrackBgLayout layout = adjustedTrackBackgroundLayout(i);
    if (!layout.rel) return;

    const char* previewRel = (i == 9) ? track10AnimatedBackgroundRel() : layout.rel;
    Texture2D bg = ctx.assets->tex(previewRel).tex;

    float absX = 0.0f;
    float absY = 0.0f;
    bool haveAbs = (bg.id && bg.width > 0 && bg.height > 0);
    if (haveAbs) {
        absX = (float)bg.width * 0.5f + layout.offsetX;
        absY = (float)bg.height * 0.5f + layout.offsetY;
    }

    if (haveAbs) {
        std::printf(
            "[cover-debug] track=%d file=%s abs=(%.1f, %.1f) offset=(%.1f, %.1f) nudge=(%.1f, %.1f) -> case %d: return {\"%s\", %.1ff, %.1ff, %.1ff};\n",
            i + 1,
            trackFileRel.c_str(),
            absX,
            absY,
            layout.offsetX,
            layout.offsetY,
            gDebugTrackNudge[i].x,
            gDebugTrackNudge[i].y,
            i,
            layout.rel,
            layout.offsetX,
            layout.offsetY,
            layout.zoom);
    } else {
        std::printf(
            "[cover-debug] track=%d file=%s abs=(n/a) offset=(%.1f, %.1f) nudge=(%.1f, %.1f) -> case %d: return {\"%s\", %.1ff, %.1ff, %.1ff};\n",
            i + 1,
            trackFileRel.c_str(),
            layout.offsetX,
            layout.offsetY,
            gDebugTrackNudge[i].x,
            gDebugTrackNudge[i].y,
            i,
            layout.rel,
            layout.offsetX,
            layout.offsetY,
            layout.zoom);
    }
    std::fflush(stdout);
}

static bool drawTrackBackground(const DrawContext& ctx, Rectangle view, int trackIndex) {
    int i = wrapTrackIndex(trackIndex);
    TrackBgLayout layout = adjustedTrackBackgroundLayout(i);
    const char* rel = (i == 9) ? track10AnimatedBackgroundRel() : layout.rel;
    if (!rel || view.width <= 0 || view.height <= 0) return false;

    auto bg = ctx.assets->tex(rel).tex;
    if (!bg.id || bg.width <= 0 || bg.height <= 0) return false;

    float zoom = std::max(0.1f, layout.zoom);
    float srcW = std::min((float)bg.width, view.width / zoom);
    float srcH = std::min((float)bg.height, view.height / zoom);

    float centerX = (float)bg.width * 0.5f + layout.offsetX;
    float centerY = (float)bg.height * 0.5f + layout.offsetY;
    float srcX = std::clamp(centerX - srcW * 0.5f, 0.0f, (float)bg.width - srcW);
    float srcY = std::clamp(centerY - srcH * 0.5f, 0.0f, (float)bg.height - srcH);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    DrawTexturePro(
        bg,
        {srcX, srcY, srcW, srcH},
        view,
        {0.0f, 0.0f},
        0.0f,
        WHITE);
    EndScissorMode();
    return true;
}

static void drawDebugArrowButton(const DrawContext& ctx, const ui::SpriteButton& b, float rotationDeg) {
    const bool active = b.hovered || b.pressed;
    Texture2D tex = ctx.assets->tex(active
        ? "sprites/UI/Menu/Buttons/right-btn_selected.png"
        : "sprites/UI/Menu/Buttons/right-btn.png").tex;
    if (!tex.id) return;

    Rectangle src = {0.0f, 0.0f, (float)tex.width, (float)tex.height};
    Rectangle dst = {b.rect.x + b.rect.width * 0.5f, b.rect.y + b.rect.height * 0.5f, b.rect.width, b.rect.height};
    Vector2 origin = {dst.width * 0.5f, dst.height * 0.5f};
    DrawTexturePro(tex, src, dst, origin, rotationDeg, WHITE);
}

static Texture2D playerCounterCurrentGlyph(const DrawContext& ctx, char ch) {
    if (ch >= '0' && ch <= '9') {
        char rel[64];
        snprintf(rel, sizeof(rel), "sprites/UI/Fonts/%04d.png", 850 + (ch - '0'));
        return ctx.assets->tex(rel).tex;
    }
    return {};
}

static Texture2D playerCounterLegacyGlyph(const DrawContext& ctx, char ch) {
    switch (ch) {
    case '/': return ctx.assets->tex("sprites/UI/Fonts/1002.png").tex;
    case '0': return ctx.assets->tex("sprites/UI/Fonts/0992.png").tex;
    case '1': return ctx.assets->tex("sprites/UI/Fonts/0993.png").tex;
    case '2': return ctx.assets->tex("sprites/UI/Fonts/0994.png").tex;
    case '3': return ctx.assets->tex("sprites/UI/Fonts/0995.png").tex;
    case '4': return ctx.assets->tex("sprites/UI/Fonts/0996.png").tex;
    case '5': return ctx.assets->tex("sprites/UI/Fonts/0997.png").tex;
    case '6': return ctx.assets->tex("sprites/UI/Fonts/0998.png").tex;
    case '7': return ctx.assets->tex("sprites/UI/Fonts/0999.png").tex;
    case '8': return ctx.assets->tex("sprites/UI/Fonts/1000.png").tex;
    case '9': return ctx.assets->tex("sprites/UI/Fonts/1001.png").tex;
    default: return {};
    }
}

static int measureCounterSprites(const DrawContext& ctx, const std::string& text, Texture2D (*glyphFn)(const DrawContext&, char)) {
    int width = 0;
    for (char ch : text) {
        Texture2D g = glyphFn(ctx, ch);
        if (!g.id) continue;
        width += g.width + 1;
    }
    if (width > 0) width -= 1;
    return width;
}

static void drawCounterSprites(const DrawContext& ctx, const std::string& text, float x, float y, Texture2D (*glyphFn)(const DrawContext&, char)) {
    float drawX = x;
    for (char ch : text) {
        Texture2D g = glyphFn(ctx, ch);
        if (!g.id) continue;
        DrawTexture(g, (int)drawX, (int)y, WHITE);
        drawX += g.width + 1.0f;
    }
}

void PlayerScreen::scanTracks() {
    tracks_.clear();
    auto files = listAudio("assets/music/levels");
    int n = 0;
    for (auto& fn : files) {
        n++;
        Track t;
        t.fileRel = std::string("music/levels/") + fn;

        if (const char* mappedName = desiredTrackNameRel(fn)) {
            t.namePngRel = mappedName;
        } else {
            t.namePngRel.clear();
        }

        char ibuf[64];
        snprintf(ibuf, sizeof(ibuf), "sprites/player/trackicon_%02d.png", n);
        t.iconRel = ibuf;

        t.titleText = fn;
        tracks_.push_back(std::move(t));
    }
    if (tracks_.empty()) {
        Track t;
        t.fileRel = "music/levels/01-Example.mp3";
        t.namePngRel = "sprites/UI/Menu/Russian/tracks/morning-dew.png";
        t.iconRel = "sprites/player/trackicon_01.png";
        t.titleText = "No tracks in assets/music/levels/";
        tracks_.push_back(t);
    }
}

void PlayerScreen::loadTrack(const UpdateContext& ctx, int i) {
    if (i < 0 || i >= (int)tracks_.size()) return;

    if (musicOk_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        musicOk_ = false;
    }

    std::string rel = tracks_[i].fileRel;
    if (ctx.app) rel = ctx.app->resolveMusicRel(rel);
    std::string path = Assets::A(rel);
    if (FileExists(path.c_str())) {
        music_ = LoadMusicStream(path.c_str());
        musicOk_ = (music_.ctxData != nullptr);
        if (musicOk_) SetMusicVolume(music_, 0.9f);
    }
    playing_ = false;
}

void PlayerScreen::ensureTrackLoaded(const UpdateContext& ctx) {
    bool musicEnabled = !ctx.profile || ctx.profile->musicEnabled;
    bool wantRemix = ctx.profile && ctx.profile->musicRemix;
    if (!musicEnabled) {
        if (musicOk_) {
            StopMusicStream(music_);
            UnloadMusicStream(music_);
            musicOk_ = false;
        }
        playing_ = false;
        trackDirty_ = true;
        remixState_ = wantRemix;
        return;
    }

    if (!trackDirty_ && wantRemix == remixState_) return;
    trackDirty_ = false;
    remixState_ = wantRemix;
    loadTrack(ctx, idx_);
}

void PlayerScreen::onEnter() {
    scanTracks();
    btnPrev_.rect = { 58, 316, 36, 30 };
    btnNext_.rect = { 146, 316, 36, 30 };
    btnPlay_.rect = { 94, 297, 52, 58 };

    btnPrev_.bgRel = "sprites/UI/Menu/Buttons/backward-btn.png";
    btnPrev_.bgRelActive = "sprites/UI/Menu/Buttons/backward-btn_selected.png";
    btnNext_.bgRel = "sprites/UI/Menu/Buttons/forward-btn.png";
    btnNext_.bgRelActive = "sprites/UI/Menu/Buttons/forward-btn_selected.png";
    btnPlay_.bgRel = "sprites/UI/Menu/Buttons/play-big-btn.png";
    btnPlay_.bgRelActive = "sprites/UI/Menu/Buttons/play-big-btn_selected.png";

    // Cover-offset debug arrows (left/up/down/right)
    dbgLeft_.rect  = {170, 248, 18, 18};
    dbgUp_.rect    = {188, 230, 18, 18};
    dbgDown_.rect  = {188, 266, 18, 18};
    dbgRight_.rect = {206, 248, 18, 18};

    SetupMenuBackButton(back_, 400);

    carousel_.speed = 8.5f;

    idx_ = 0;
    trackDirty_ = true;
    remixState_ = false;
    playing_ = false;
    app_ = nullptr;
}

void PlayerScreen::onExit() {
    if (app_) app_->resumeMenuMusic();
    if (musicOk_) {
        StopMusicStream(music_);
        UnloadMusicStream(music_);
        musicOk_ = false;
    }
}

void PlayerScreen::update(const UpdateContext& ctx) {
    auto st = ctx.input->state();
    auto click = [&](ui::SpriteButton& b){ return b.update(st.mouseV, st.down && st.inViewport, st.pressed && st.inViewport, st.released && st.inViewport); };
    if (ctx.app) {
        app_ = ctx.app;
        app_->pauseMenuMusic();
    }

    if (st.keyBack || click(back_)) {
        if (ctx.app) {
            ctx.app->resumeMenuMusic();
            ctx.app->requestMenuMusic();
            ctx.app->pop();
            ctx.app->playSfx("sounds/MenuBack.wav");
        }
        return;
    }

    ensureTrackLoaded(ctx);
    if (musicOk_) UpdateMusicStream(music_);
    carousel_.update(ctx.dt);

    auto step = [&](int dir){
        if (carousel_.animating()) return;
        idx_ = (idx_ + dir + (int)tracks_.size()) % (int)tracks_.size();
        carousel_.start(dir);
        trackDirty_ = true;
        ctx.app->playSfx("sounds/MenuSwitch.wav");
    };

    if (st.swipe == SwipeDir::Left) step(+1);
    if (st.swipe == SwipeDir::Right) step(-1);

    if (click(btnPrev_)) step(-1);
    if (click(btnNext_)) step(+1);

    if (click(btnPlay_)) {
        if (ctx.profile && !ctx.profile->musicEnabled) { ctx.toast->show("Music disabled in Settings"); return; }
        if (!musicOk_) { ctx.toast->show("Missing audio file"); return; }
        if (!playing_) { PlayMusicStream(music_); playing_ = true; }
        else { PauseMusicStream(music_); playing_ = false; }
        ctx.app->playSfx("sounds/MenuSelect.wav");
    }

    bool dbgChanged = false;
    int ti = wrapTrackIndex(idx_);
    if (click(dbgLeft_))  { gDebugTrackNudge[ti].x -= kDebugNudgeStep; dbgChanged = true; }
    if (click(dbgRight_)) { gDebugTrackNudge[ti].x += kDebugNudgeStep; dbgChanged = true; }
    if (click(dbgUp_))    { gDebugTrackNudge[ti].y -= kDebugNudgeStep; dbgChanged = true; }
    if (click(dbgDown_))  { gDebugTrackNudge[ti].y += kDebugNudgeStep; dbgChanged = true; }
    if (dbgChanged) {
        ctx.app->playSfx("sounds/MenuSelect.wav");
        const std::string trackFileRel = (idx_ >= 0 && idx_ < (int)tracks_.size()) ? tracks_[idx_].fileRel : std::string("<unknown>");
        printTrackBackgroundDebugLine(ctx, idx_, trackFileRel);
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
    DrawMenuBackground(*ctx.assets, ctx.vs ? ctx.vs->vw : 240, ctx.vs ? ctx.vs->vh : 400);

    auto title = ctx.assets->tex("sprites/UI/Menu/Russian/player-title.png").tex;
    DrawTexture(title, 4, 4, WHITE);

    Rectangle card = { kPlayerCardX, 82, 180, 190 };
    DrawRectangleRounded(card, 0.12f, 8, Fade(BLACK, 0.25f));
    DrawRectangleRoundedLinesEx(card, 0.12f, 8, 2.0f, Fade(RAYWHITE, 0.35f));

    const auto& t = tracks_[idx_];

    Rectangle coverFrame = {
        card.x + (card.width - kTrackCoverFrameSize) * 0.5f,
        card.y + kTrackCoverTopInset - 6.0f,
        kTrackCoverFrameSize,
        kTrackCoverFrameSize
    };
    Rectangle playerWindow = {
        coverFrame.x + (coverFrame.width - kTrackCoverSize) * 0.5f,
        coverFrame.y + (coverFrame.height - kTrackCoverSize) * 0.5f,
        kTrackCoverSize,
        kTrackCoverSize
    };
    DrawRectangleRounded(coverFrame, 0.14f, 6, Fade(BLACK, 0.2f));
    DrawRectangleRoundedLinesEx(coverFrame, 0.14f, 6, 1.0f, Fade(RAYWHITE, 0.35f));

    bool drewBg = drawTrackBackground(ctx, playerWindow, idx_);
    if (!drewBg) {
        auto icon = ctx.assets->tex(t.iconRel).tex;
        if (icon.id) DrawCentered(icon, playerWindow.x + playerWindow.width * 0.5f, playerWindow.y + playerWindow.height * 0.5f);
        else DrawTextCentered("track icon", (int)(playerWindow.x + playerWindow.width * 0.5f), (int)(playerWindow.y + playerWindow.height * 0.5f - 6), 12, GRAY);
    }

    std::string currentTrackText = std::to_string(idx_ + 1);
    std::string tailTrackText = "/" + std::to_string((int)tracks_.size());
    int currentW = measureCounterSprites(ctx, currentTrackText, playerCounterCurrentGlyph);
    int tailW = measureCounterSprites(ctx, tailTrackText, playerCounterLegacyGlyph);
    int betweenGap = (currentW > 0 && tailW > 0) ? 1 : 0;
    int counterW = currentW + betweenGap + tailW;
    float counterX = playerWindow.x + playerWindow.width - (float)counterW - 1.0f + 4.0f;
    float counterY = playerWindow.y + playerWindow.height - 13.0f + 2.0f;
    drawCounterSprites(ctx, currentTrackText, counterX, counterY, playerCounterCurrentGlyph);
    drawCounterSprites(ctx, tailTrackText, counterX + currentW + betweenGap, counterY, playerCounterLegacyGlyph);

    auto name = ctx.assets->tex(t.namePngRel).tex;
    float nameY = playerWindow.y + playerWindow.height + 22.0f;
    if (name.id) DrawCentered(name, card.x + card.width / 2, nameY);
    else DrawTextCentered(t.titleText.c_str(), (int)(card.x + card.width / 2), (int)(nameY - 5), 12, RAYWHITE);

    const float eqX = card.x + 20.0f;
    const float eqY = card.y + 146.0f;
    auto eqSeg = ctx.assets->tex("sprites/UI/Menu/Buttons/1323.png").tex;
    if (eqSeg.id) {
        const float eqW = 10.0f * (float)eqSeg.width + 9.0f;
        const float eqH = (float)eqSeg.height + 12.0f;
        Rectangle eqBg = {eqX - 6.0f, eqY - 4.0f, eqW + 12.0f, eqH + 8.0f};
        const float eqRoundness = std::min(1.0f, 12.0f / std::min(eqBg.width, eqBg.height));
        DrawRectangleRounded(eqBg, eqRoundness, 8, Fade(BLACK, 0.28f));
        DrawRectangleRoundedLinesEx(eqBg, eqRoundness, 8, 1.0f, Fade(RAYWHITE, 0.22f));
    }
    drawEq(ctx, eqX, eqY);

    btnPrev_.draw(*ctx.assets);
    btnNext_.draw(*ctx.assets);
    btnPlay_.bgRel = playing_ ? "sprites/UI/Menu/Buttons/pause-big-btn.png" : "sprites/UI/Menu/Buttons/play-big-btn.png";
    btnPlay_.bgRelActive = playing_ ? "sprites/UI/Menu/Buttons/pause-big-btn_selected.png" : "sprites/UI/Menu/Buttons/play-big-btn_selected.png";
    btnPlay_.draw(*ctx.assets);
    drawDebugArrowButton(ctx, dbgLeft_, 180.0f);
    drawDebugArrowButton(ctx, dbgUp_, -90.0f);
    drawDebugArrowButton(ctx, dbgDown_, 90.0f);
    drawDebugArrowButton(ctx, dbgRight_, 0.0f);
    back_.draw(*ctx.assets);

    if (ctx.debug) DrawText(TextFormat("tracks=%d idx=%d", (int)tracks_.size(), idx_), 6, (ctx.vs ? ctx.vs->vh : 400) - 12, 12, YELLOW);
}

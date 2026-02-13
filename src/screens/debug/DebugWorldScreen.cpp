#include "screens/debug/DebugWorldScreen.h"
#include "core/App.h"
#include "core/DrawUtil.h"
#include "screens/menu/main/MenuMainScreen.h"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <memory>
#include <unordered_map>

namespace {
constexpr const char* kBgTile = "sprites/LevelBackgrounds/debugWorld_bg.png";

constexpr int kVirtualW = 240;
constexpr int kVirtualH = 400;

constexpr float kBarX = 8.0f;
constexpr float kBarY = 304.0f;
constexpr float kBarW = 232.0f;
constexpr float kBarH = 92.0f;
constexpr float kBarPadX = 8.0f;
constexpr float kBarPadY = 7.0f;
constexpr float kButtonW = 36.0f;
constexpr float kButtonH = 36.0f;
constexpr float kButtonGapY = 6.0f;
constexpr float kButtonGapX = 8.0f;
constexpr int kRows = 2;
constexpr int kCols = 5;
constexpr float kScrollSpeed = 20.0f;
constexpr float kHpBarY = 16.0f;
constexpr float kHpSegmentsStartX = 29.0f;
constexpr float kHpSegmentsTopPad = 1.0f;
constexpr float kHpSegmentGapX = 1.0f;
constexpr float kHpSegmentsOffsetY = 6.0f;
constexpr float kHpBlinkFrequencyHz = 2.0f;
constexpr float kGameOverGapY = 6.0f;
constexpr float kTripleSpawnDelaySec = 0.3f;
constexpr float kTripleSpawnStepX = 26.0f;
constexpr float kTripleSpawnEdgeExtraX = 32.0f;
constexpr float kTuneyTargetYRatio = 0.50f;
constexpr float kTuneySpawnBottomInset = 1.0f;
constexpr float kFallingSpeedPxPerSec = 130.0f;
constexpr float kFallingOutMargin = 40.0f;
constexpr float kPressFxRisePx = 12.0f;
constexpr float kPressFxDurationSec = 0.5f;
constexpr int kDefaultTargetFps = 60;
constexpr int kTargetFpsStep = 5;
constexpr int kTargetFpsMin = 1;
constexpr int kDebugOverlayYOffset = 16;

constexpr int kBtnSpawnSingle = 0;
constexpr int kBtnSpawnTriple = 1;
constexpr int kBtnStartDelayMinus = 2;
constexpr int kBtnStartDelayPlus = 3;
constexpr int kBtnFpsMinus = 6;
constexpr int kBtnFpsPlus = 7;
constexpr int kBtnHpMinus = 8;
constexpr int kBtnHpPlus = 9;

constexpr int kRhythmStartDelayStepMs = 5;
constexpr int kRhythmCenterShiftStepMs = 5;
constexpr int kRhythmStartDelayMinMs = -500;
constexpr int kRhythmStartDelayMaxMs = 500;
constexpr int kRhythmCenterShiftMinMs = -300;
constexpr int kRhythmCenterShiftMaxMs = 300;

void drawTiled(Texture2D tile, int vw, int vh) {
    if (!tile.id || tile.width <= 0 || tile.height <= 0) return;
    for (int y = 0; y < vh; y += tile.height) {
        for (int x = 0; x < vw; x += tile.width) {
            DrawTexture(tile, x, y, WHITE);
        }
    }
}

void drawTripleSpawnIcon(Assets& assets, Rectangle rect) {
    Texture2D tex = assets.tex("sprites/Tuneys/Normal/0734.png").tex;
    if (!tex.id) return;

    const float scale = 0.60f;
    const float w = tex.width * scale;
    const float h = tex.height * scale;
    const float cx = rect.x + rect.width * 0.5f;
    const float cy = rect.y + rect.height * 0.56f;
    const float sideOffset = w * 0.62f;
    const float topOffset = h * 0.54f;

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, {cx - sideOffset - w * 0.5f, cy - h * 0.5f, w, h}, {0, 0}, 0.0f, WHITE);
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, {cx + sideOffset - w * 0.5f, cy - h * 0.5f, w, h}, {0, 0}, 0.0f, WHITE);
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, {cx - w * 0.5f, cy - topOffset - h * 0.5f, w, h}, {0, 0}, 0.0f, WHITE);
}

int detectVsyncRefreshFps() {
    int fps = GetMonitorRefreshRate(GetCurrentMonitor());
    if (fps <= 0) fps = kDefaultTargetFps;
    return fps;
}

int stepTargetFps(int current, int delta, int maxFps) {
    current = std::clamp(current, kTargetFpsMin, maxFps);
    if (delta < 0) {
        if (current <= kTargetFpsMin) return kTargetFpsMin;
        if (current == kTargetFpsStep) return kTargetFpsMin;
        return std::max(kTargetFpsMin, current - kTargetFpsStep);
    }
    if (delta > 0) {
        if (current >= maxFps) return maxFps;
        if (current == kTargetFpsMin) return std::min(maxFps, kTargetFpsStep);
        return std::min(maxFps, current + kTargetFpsStep);
    }
    return current;
}

const char* timerWindowStatusForNote(
    const RhythmSystem::NoteSnapshot* note,
    const RhythmSystem::TimingConfig& cfg,
    int* outDeltaMs) {
    if (!note) {
        if (outDeltaMs) *outDeltaMs = 0;
        return "None";
    }

    const int deltaMs = (int)std::lround(note->elapsedMs - note->centerMs);
    if (outDeltaMs) *outDeltaMs = deltaMs;
    const int absDeltaMs = std::abs(deltaMs);
    const bool insideEarlyLate = (deltaMs >= -cfg.earlyTapMs) && (deltaMs <= cfg.lateTapMs);

    if (insideEarlyLate && absDeltaMs <= cfg.perfectTapMs) return "Perfect";
    if (insideEarlyLate && absDeltaMs <= cfg.goodTapMs) return "Good";
    if (absDeltaMs <= cfg.missTapMs) return "Miss";
    return (deltaMs < 0) ? "Early" : "Late";
}
}

void DebugWorldScreen::onEnter() {
    bgAnimTimer_ = 0.0f;
    health_.reset();
    score_.reset();
    rhythm_.clear();
    tuneySystem_.clear();
    spawnQueue_.clear();
    rhythmStartQueue_.clear();
    pressFx_.clear();
    tuneyToNote_.clear();
    debugPerfectCount_ = 0;
    debugGoodCount_ = 0;
    debugMissCount_ = 0;
    debugEarlyCount_ = 0;
    debugLateCount_ = 0;
    debugLastStatus_ = "None";
    debugLastDeltaMs_ = 0;
    tuneysLoaded_ = false;
    assetsRef_ = nullptr;
    rhythmTimerMs_ = 1000;
    rhythmStartDelayMs_ = 0;
    rhythmCenterShiftMs_ = 100;
    maxVsyncFps_ = detectVsyncRefreshFps();
    targetFps_ = std::clamp(kDefaultTargetFps, kTargetFpsMin, maxVsyncFps_);
    SetTargetFPS(targetFps_);
    syncRhythmConfig();

    barRect_ = {kBarX, kBarY, kBarW, kBarH};
    buttons_.clear();
    buttons_.resize(kRows * kCols);
    for (auto& b : buttons_) {
        b.bgRel = "sprites/UI/Menu/Buttons/1391.png";
        b.bgRelActive = "sprites/UI/Menu/Buttons/1395.png";
    }

    if (!buttons_.empty()) {
        buttons_[kBtnSpawnSingle].labelRel = "sprites/Tuneys/Normal/0915.png";
        buttons_[kBtnSpawnSingle].labelRelActive = "sprites/Tuneys/Normal/0915.png";
    }
    if (buttons_.size() > (size_t)kBtnSpawnTriple) {
        buttons_[kBtnSpawnTriple].labelRel.clear();
        buttons_[kBtnSpawnTriple].labelRelActive.clear();
    }

    auto setPlusMinus = [&](int minusIdx, int plusIdx) {
        if (minusIdx >= 0 && minusIdx < (int)buttons_.size()) buttons_[(size_t)minusIdx].labelRel = "sprites/UI/Fonts/1003.png";
        if (plusIdx >= 0 && plusIdx < (int)buttons_.size()) buttons_[(size_t)plusIdx].labelRel = "sprites/UI/Fonts/1004.png";
    };
    setPlusMinus(kBtnStartDelayMinus, kBtnStartDelayPlus);
    setPlusMinus(kBtnFpsMinus, kBtnFpsPlus);
    setPlusMinus(kBtnHpMinus, kBtnHpPlus);

    scrollX_ = 0.0f;
    draggingScroll_ = false;
    dragStartMouseX_ = 0.0f;
    dragStartScrollX_ = 0.0f;
    rebuildButtons();
}

void DebugWorldScreen::onExit() {
    rhythm_.clear();
    tuneySystem_.clear();
    score_.reset();
    spawnQueue_.clear();
    rhythmStartQueue_.clear();
    pressFx_.clear();
    tuneyToNote_.clear();
    if (tuneysLoaded_ && assetsRef_) assetsRef_->unloadTuneySprites();
    tuneysLoaded_ = false;
    assetsRef_ = nullptr;
    SetTargetFPS(kDefaultTargetFps);
}

void DebugWorldScreen::syncRhythmConfig() {
    auto cfg = rhythm_.timingConfig();
    cfg.centerOffsetMs = rhythmCenterShiftMs_;
    rhythm_.setTimingConfig(cfg);
}

Vector2 DebugWorldScreen::screenCenterTarget(const UpdateContext& ctx) const {
    int vw = ctx.vs ? ctx.vs->vw : kVirtualW;
    int vh = ctx.vs ? ctx.vs->vh : kVirtualH;
    return {(float)vw * 0.5f, (float)vh * kTuneyTargetYRatio};
}

void DebugWorldScreen::createTuneyAt(const UpdateContext& ctx, Vector2 target, TuneySystem::Type type) {
    int vh = ctx.vs ? ctx.vs->vh : kVirtualH;
    TuneySystem::SpawnParams spawn{};
    spawn.type = type;
    spawn.movement = TuneySystem::Movement::Linear;
    spawn.targetPoint = target;
    spawn.spawnPoint = {target.x, (float)vh - kTuneySpawnBottomInset};
    spawn.preTouchDistancePx = TuneySystem::kDefaultPreTouchDistancePx;
    spawn.touchHoldSeconds = TuneySystem::kDefaultTouchHoldSeconds;
    spawn.preTouchAnimation = TuneySystem::makePreTouchAnimation(type);
    tuneySystem_.create(spawn);
}

void DebugWorldScreen::queueTripleTuneySpawn(const UpdateContext& ctx) {
    const Vector2 center = screenCenterTarget(ctx);
    spawnQueue_.push_back({0.0f, {center.x - kTripleSpawnStepX - kTripleSpawnEdgeExtraX, center.y}, TuneySystem::Type::Normal});
    spawnQueue_.push_back({kTripleSpawnDelaySec, {center.x, center.y}, TuneySystem::Type::Normal});
    spawnQueue_.push_back({kTripleSpawnDelaySec * 2.0f, {center.x + kTripleSpawnStepX + kTripleSpawnEdgeExtraX, center.y}, TuneySystem::Type::Normal});
}

void DebugWorldScreen::processTuneySpawnQueue(const UpdateContext& ctx) {
    for (size_t i = 0; i < spawnQueue_.size();) {
        auto& pending = spawnQueue_[i];
        pending.delay -= ctx.dt;
        if (pending.delay > 0.0f) {
            ++i;
            continue;
        }
        createTuneyAt(ctx, pending.target, pending.type);
        spawnQueue_.erase(spawnQueue_.begin() + (std::ptrdiff_t)i);
    }
}

void DebugWorldScreen::processTuneyEvents() {
    auto events = tuneySystem_.consumeEvents();
    for (const auto& ev : events) {
        if (ev.typeEvent == TuneySystem::EventType::EnteredPreTouch) {
            rhythmStartQueue_.push_back({rhythmStartDelayMs_ / 1000.0f, ev.tuneyId});
        } else if (ev.typeEvent == TuneySystem::EventType::Destroyed) {
            auto it = tuneyToNote_.find(ev.tuneyId);
            if (it != tuneyToNote_.end()) {
                rhythm_.destroyNote(it->second);
                tuneyToNote_.erase(it);
            }
        }
    }
}

void DebugWorldScreen::processRhythmStartQueue(float dtSec) {
    for (size_t i = 0; i < rhythmStartQueue_.size();) {
        auto& pending = rhythmStartQueue_[i];
        pending.delaySec -= dtSec;
        if (pending.delaySec > 0.0f) {
            ++i;
            continue;
        }
        if (tuneyToNote_.find(pending.tuneyId) == tuneyToNote_.end()) {
            const int initialElapsedMs = (pending.delaySec < 0.0f)
                ? (int)std::lround(-pending.delaySec * 1000.0f)
                : 0;
            uint32_t noteId = rhythm_.createNote(pending.tuneyId, rhythmTimerMs_, initialElapsedMs);
            tuneyToNote_[pending.tuneyId] = noteId;
        }
        rhythmStartQueue_.erase(rhythmStartQueue_.begin() + (std::ptrdiff_t)i);
    }
}

void DebugWorldScreen::processRhythmEvents() {
    auto events = rhythm_.consumeJudgementEvents();
    for (const auto& ev : events) {
        score_.applyJudgement(ev.judgement);
        debugLastDeltaMs_ = ev.deltaMs;
        switch (ev.judgement) {
            case RhythmJudgement::Perfect:
                ++debugPerfectCount_;
                debugLastStatus_ = "Perfect";
                break;
            case RhythmJudgement::Good:
                ++debugGoodCount_;
                debugLastStatus_ = "Good";
                break;
            case RhythmJudgement::Miss:
                ++debugMissCount_;
                debugLastStatus_ = "Miss";
                break;
            case RhythmJudgement::Skip:
                debugLastStatus_ = "Skip";
                break;
            case RhythmJudgement::None:
                break;
        }
        if (ev.judgement != RhythmJudgement::Skip && ev.judgement != RhythmJudgement::None) {
            if (ev.deltaMs < 0) ++debugEarlyCount_;
            else if (ev.deltaMs > 0) ++debugLateCount_;
        }
    }
}

void DebugWorldScreen::handleTuneyPress(const UpdateContext& ctx, Vector2 point) {
    auto snaps = tuneySystem_.snapshot();
    for (auto it = snaps.rbegin(); it != snaps.rend(); ++it) {
        if (it->spriteRel.empty()) continue;
        Texture2D tex = ctx.assets->tex(it->spriteRel).tex;
        if (!tex.id) continue;
        Rectangle r = {it->pos.x - tex.width * 0.5f, it->pos.y - tex.height * 0.5f, (float)tex.width, (float)tex.height};
        if (!CheckCollisionPointRec(point, r)) continue;

        auto noteIt = tuneyToNote_.find(it->tuneyId);
        if (noteIt == tuneyToNote_.end()) continue;

        auto result = rhythm_.press(noteIt->second);
        if (result.consumed) {
            if (result.judgement == RhythmJudgement::Perfect || result.judgement == RhythmJudgement::Good) {
                spawnPressFx(it->pos);
                tuneySystem_.destroy(it->tuneyId, TuneySystem::Animation{});
                tuneyToNote_.erase(noteIt);
            }
            if (ctx.app) ctx.app->playSfx("sounds/MenuSelect.wav");
        }
        return;
    }
}

void DebugWorldScreen::spawnPressFx(Vector2 pos) {
    pressFx_.push_back({pos, 0.0f});
}

void DebugWorldScreen::updatePressFx(float dtSec) {
    for (size_t i = 0; i < pressFx_.size();) {
        pressFx_[i].timer += dtSec;
        if (pressFx_[i].timer < kPressFxDurationSec) {
            ++i;
            continue;
        }
        pressFx_.erase(pressFx_.begin() + (std::ptrdiff_t)i);
    }
}

void DebugWorldScreen::drawPressFx(const DrawContext& ctx) const {
    Texture2D tex = ctx.assets->tex("sprites/Tuneys/Normal/0923.png").tex;
    if (!tex.id) return;

    for (const auto& fx : pressFx_) {
        float t = std::clamp(fx.timer / kPressFxDurationSec, 0.0f, 1.0f);
        float smooth = t * t * (3.0f - 2.0f * t);
        float y = fx.startPos.y - smooth * kPressFxRisePx;
        DrawTexture(tex, (int)(fx.startPos.x - tex.width * 0.5f), (int)(y - tex.height * 0.5f), WHITE);
    }
}

void DebugWorldScreen::syncRhythmToTuneys(const UpdateContext& ctx) {
    const int vh = ctx.vs ? ctx.vs->vh : kVirtualH;
    std::unordered_map<uint32_t, RhythmSystem::NoteSnapshot> noteMap;
    for (const auto& n : rhythm_.snapshot()) noteMap[n.noteId] = n;

    for (auto it = tuneyToNote_.begin(); it != tuneyToNote_.end();) {
        const uint32_t tuneyId = it->first;
        const uint32_t noteId = it->second;
        auto noteIt = noteMap.find(noteId);
        if (noteIt == noteMap.end()) {
            tuneySystem_.clearExternalSprite(tuneyId);
            tuneySystem_.destroy(tuneyId);
            it = tuneyToNote_.erase(it);
            continue;
        }

        if (const char* animSprite = rhythm_.currentAnimationSprite(noteId)) {
            tuneySystem_.setExternalSprite(tuneyId, animSprite);
        } else {
            tuneySystem_.clearExternalSprite(tuneyId);
        }

        if (noteIt->second.phase == RhythmSystem::NotePhase::FallingAnimation) {
            Vector2 pos{};
            if (tuneySystem_.getPosition(tuneyId, pos)) {
                pos.y += kFallingSpeedPxPerSec * ctx.dt;
                tuneySystem_.setPosition(tuneyId, pos);
                if (pos.y > vh + kFallingOutMargin) {
                    rhythm_.markFallenOut(noteId);
                    tuneySystem_.destroy(tuneyId);
                }
            }
        }

        ++it;
    }
}

void DebugWorldScreen::rebuildButtons() {
    float contentX = barRect_.x + kBarPadX;
    float contentY = barRect_.y + kBarPadY;

    float contentW = kCols * kButtonW + (kCols - 1) * kButtonGapX;
    float visibleW = barRect_.width - kBarPadX * 2.0f;
    maxScrollX_ = std::max(0.0f, contentW - visibleW);
    scrollX_ = std::clamp(scrollX_, 0.0f, maxScrollX_);

    for (int row = 0; row < kRows; ++row) {
        for (int col = 0; col < kCols; ++col) {
            int idx = row * kCols + col;
            float x = contentX + col * (kButtonW + kButtonGapX) - scrollX_;
            float y = contentY + row * (kButtonH + kButtonGapY);
            buttons_[(size_t)idx].rect = {x, y, kButtonW, kButtonH};
        }
    }
}

void DebugWorldScreen::update(const UpdateContext& ctx) {
    if (!tuneysLoaded_ && ctx.assets) {
        assetsRef_ = ctx.assets;
        assetsRef_->preloadTuneySprites();
        tuneysLoaded_ = true;
    }
    bgAnimTimer_ += ctx.dt;

    auto st = ctx.input->state();
    if (st.keyBack) {
        ctx.app->clearAndPush(std::make_unique<MenuMainScreen>());
        ctx.app->playSfx("sounds/MenuBack.wav");
        return;
    }

    processTuneySpawnQueue(ctx);
    updatePressFx(ctx.dt);
    tuneySystem_.update(ctx.dt);
    processTuneyEvents();
    processRhythmStartQueue(ctx.dt);
    rhythm_.update(ctx.dt);

    bool inBar = st.inViewport && CheckCollisionPointRec(st.mouseV, barRect_);

    if (inBar) {
        float wheel = GetMouseWheelMove();
        if (std::fabs(wheel) > 0.001f) {
            scrollX_ = std::clamp(scrollX_ - wheel * kScrollSpeed, 0.0f, maxScrollX_);
            rebuildButtons();
        }
    }

    if (st.pressed && inBar) {
        draggingScroll_ = true;
        dragStartMouseX_ = st.mouseV.x;
        dragStartScrollX_ = scrollX_;
    }
    if (st.released) {
        draggingScroll_ = false;
    }
    if (draggingScroll_ && st.down && inBar) {
        float dx = st.mouseV.x - dragStartMouseX_;
        scrollX_ = std::clamp(dragStartScrollX_ - dx, 0.0f, maxScrollX_);
        rebuildButtons();
    }

    if (inBar && st.swipe == SwipeDir::Left) {
        scrollX_ = std::clamp(scrollX_ + (kButtonW + kButtonGapX), 0.0f, maxScrollX_);
        rebuildButtons();
    } else if (inBar && st.swipe == SwipeDir::Right) {
        scrollX_ = std::clamp(scrollX_ - (kButtonW + kButtonGapX), 0.0f, maxScrollX_);
        rebuildButtons();
    }

    if (st.pressed && st.inViewport && !inBar) {
        handleTuneyPress(ctx, st.mouseV);
    }

    for (size_t i = 0; i < buttons_.size(); ++i) {
        auto& b = buttons_[i];
        if (!b.update(st.mouseV, st.down && inBar, st.pressed && inBar, st.released && inBar)) continue;

        bool changedTiming = false;
        if ((int)i == kBtnSpawnSingle) {
            createTuneyAt(ctx, screenCenterTarget(ctx), TuneySystem::Type::Normal);
        } else if ((int)i == kBtnSpawnTriple) {
            queueTripleTuneySpawn(ctx);
            processTuneySpawnQueue(ctx);
        } else if ((int)i == kBtnStartDelayMinus) {
            rhythmStartDelayMs_ = std::clamp(rhythmStartDelayMs_ - kRhythmStartDelayStepMs, kRhythmStartDelayMinMs, kRhythmStartDelayMaxMs);
            changedTiming = true;
        } else if ((int)i == kBtnStartDelayPlus) {
            rhythmStartDelayMs_ = std::clamp(rhythmStartDelayMs_ + kRhythmStartDelayStepMs, kRhythmStartDelayMinMs, kRhythmStartDelayMaxMs);
            changedTiming = true;
        } else if ((int)i == kBtnFpsMinus) {
            targetFps_ = stepTargetFps(targetFps_, -kTargetFpsStep, maxVsyncFps_);
            SetTargetFPS(targetFps_);
        } else if ((int)i == kBtnFpsPlus) {
            targetFps_ = stepTargetFps(targetFps_, +kTargetFpsStep, maxVsyncFps_);
            SetTargetFPS(targetFps_);
        } else if ((int)i == kBtnHpMinus) {
            health_.decrease(1);
        } else if ((int)i == kBtnHpPlus) {
            health_.increase(1);
        }

        if (changedTiming) syncRhythmConfig();
        ctx.app->playSfx("sounds/MenuSelect.wav");
    }

    syncRhythmToTuneys(ctx);
    processRhythmEvents();
}

void DebugWorldScreen::draw(const DrawContext& ctx) {
    int vw = ctx.vs ? ctx.vs->vw : kVirtualW;
    int vh = ctx.vs ? ctx.vs->vh : kVirtualH;

    ClearBackground(BLACK);
    drawTiled(ctx.assets->tex(kBgTile).tex, vw, vh);
    tuneySystem_.draw(*ctx.assets);
    drawPressFx(ctx);

    Texture2D hpBg = ctx.assets->tex("sprites/UI/HealthBar/1194.png").tex;
    if (hpBg.id) {
        float hpBgX = std::floor((vw - hpBg.width) * 0.5f) - 3.0f;
        DrawTexture(hpBg, (int)hpBgX, (int)kHpBarY, WHITE);

        float segX = hpBgX + kHpSegmentsStartX;
        float segTop = kHpBarY + kHpSegmentsTopPad + kHpSegmentsOffsetY;
        int hpValue = health_.value();
        float blinkPhase = std::fmod(bgAnimTimer_ * kHpBlinkFrequencyHz, 1.0f);
        bool blinkOn = blinkPhase < 0.5f;
        for (int i = 0; i < hpValue; ++i) {
            const char* segRel = HealthSystem::segmentSpriteRel(i);
            if (!segRel) continue;
            Texture2D seg = ctx.assets->tex(segRel).tex;
            if (!seg.id) continue;
            float y = segTop;
            float x = segX + i * (seg.width + kHpSegmentGapX);
            Color tint = WHITE;
            if (i == hpValue - 1) tint = blinkOn ? WHITE : Fade(WHITE, 0.25f);
            DrawTexture(seg, (int)x, (int)y, tint);
        }
    }

    DrawRectangleRounded(barRect_, 0.20f, 10, Fade(BLACK, 0.50f));
    DrawRectangleRoundedLinesEx(barRect_, 0.20f, 10, 2.0f, Fade(RAYWHITE, 0.35f));

    BeginScissorMode((int)barRect_.x, (int)barRect_.y, (int)barRect_.width, (int)barRect_.height);
    for (size_t i = 0; i < buttons_.size(); ++i) {
        buttons_[i].draw(*ctx.assets);
        if ((int)i == kBtnSpawnTriple) {
            drawTripleSpawnIcon(*ctx.assets, buttons_[i].rect);
        }
    }
    EndScissorMode();

    if (health_.isDepleted()) {
        Texture2D gameTex = ctx.assets->tex("sprites/UI/GameOver/game.png").tex;
        Texture2D overTex = ctx.assets->tex("sprites/UI/GameOver/over.png").tex;
        float gameCx = vw * 0.5f;
        float gameCy = vh * (1.0f / 3.0f) - (gameTex.height + overTex.height + kGameOverGapY) * 0.5f + gameTex.height * 0.5f;
        DrawCentered(gameTex, gameCx, gameCy);
        DrawCentered(overTex, gameCx, gameCy + gameTex.height * 0.5f + kGameOverGapY + overTex.height * 0.5f);
    }

    if (ctx.debug) {
        DrawRectangle(0, 34 + kDebugOverlayYOffset, 240, 100, Fade(BLACK, 0.85f));
        DrawText(TextFormat("HP:%d Tuneys:%d Score:%d Combo:%d Max:%d",
                            health_.value(), (int)tuneySystem_.count(),
                            score_.score(), score_.combo(), score_.maxCombo()),
                 4, 36 + kDebugOverlayYOffset, 10, YELLOW);

        int timerNowMs = -1;
        int timerPeriodMs = rhythmTimerMs_;
        int timerCenterMs = rhythmTimerMs_ / 2 + rhythmCenterShiftMs_;
        int timerWindowDeltaMs = 0;
        const RhythmSystem::NoteSnapshot* activeNote = nullptr;
        auto notes = rhythm_.snapshot();
        if (!notes.empty()) {
            timerNowMs = (int)std::lround(notes.front().elapsedMs);
            timerPeriodMs = notes.front().timerPeriodMs;
            timerCenterMs = (int)std::lround(notes.front().centerMs);
            activeNote = &notes.front();
        }
        auto cfg = rhythm_.timingConfig();
        const char* timerStatus = timerWindowStatusForNote(activeNote, cfg, &timerWindowDeltaMs);
        DrawText(TextFormat("Timer:%d/%d center:%d last:%s(%dms)",
                            timerNowMs, timerPeriodMs, timerCenterMs,
                            debugLastStatus_.c_str(), debugLastDeltaMs_),
                 4, 48 + kDebugOverlayYOffset, 10, ORANGE);
        DrawText(TextFormat("Status(now): %s (%dms)",
                            timerStatus, timerWindowDeltaMs),
                 4, 60 + kDebugOverlayYOffset, 10, Fade(SKYBLUE, 0.95f));
        DrawText(TextFormat("Rhythm: period=%d start=%dms\n center=(T/2)+%dms",
                            rhythmTimerMs_, rhythmStartDelayMs_, rhythmCenterShiftMs_),
                 4, 72 + kDebugOverlayYOffset, 10, ORANGE);
        DrawText(TextFormat("Perfect<=%d good<=%d miss<=%d\n [early=%d late=%d]",
                            cfg.perfectTapMs, cfg.goodTapMs, cfg.missTapMs,
                            cfg.earlyTapMs, cfg.lateTapMs),
                 4, 84 + (kDebugOverlayYOffset*2), 10, Fade(RAYWHITE, 0.9f));
    }

    int fps = GetFPS();
    const char* fpsText = TextFormat("%d FPS", fps);
    constexpr int kFpsFontSize = 18;
    int textW = MeasureText(fpsText, kFpsFontSize);
    int x = vw - textW - 4;
    int y = 4;
    DrawText(fpsText, x + 2, y + 2, kFpsFontSize, Fade(BLACK, 0.75f));
    DrawText(fpsText, x, y, kFpsFontSize, YELLOW);
    
}

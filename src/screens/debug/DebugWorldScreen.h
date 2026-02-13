#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include "game/HealthSystem.h"
#include "game/RhythmSystem.h"
#include "game/ScoreComboSystem.h"
#include "game/TuneySystem.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class DebugWorldScreen : public IScreen {
public:
    void onEnter() override;
    void onExit() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;

private:
    struct PendingTuneySpawn {
        float delay = 0.0f;
        Vector2 target{};
        TuneySystem::Type type = TuneySystem::Type::Normal;
    };

    struct PendingRhythmStart {
        float delaySec = 0.0f;
        uint32_t tuneyId = 0;
    };

    struct PressFx {
        Vector2 startPos{};
        float timer = 0.0f;
    };

    void rebuildButtons();
    Vector2 screenCenterTarget(const UpdateContext& ctx) const;
    void createTuneyAt(const UpdateContext& ctx, Vector2 target, TuneySystem::Type type = TuneySystem::Type::Normal);
    void queueTripleTuneySpawn(const UpdateContext& ctx);
    void processTuneySpawnQueue(const UpdateContext& ctx);
    void processRhythmStartQueue(float dtSec);
    void processTuneyEvents();
    void processRhythmEvents();
    void handleTuneyPress(const UpdateContext& ctx, Vector2 point);
    void spawnPressFx(Vector2 pos);
    void updatePressFx(float dtSec);
    void drawPressFx(const DrawContext& ctx) const;
    void syncRhythmToTuneys(const UpdateContext& ctx);
    void syncRhythmConfig();

    std::vector<ui::SpriteButton> buttons_{};
    std::vector<PendingTuneySpawn> spawnQueue_{};
    std::vector<PendingRhythmStart> rhythmStartQueue_{};
    std::vector<PressFx> pressFx_{};
    Rectangle barRect_{};
    float bgAnimTimer_ = 0.0f;
    float scrollX_ = 0.0f;
    float maxScrollX_ = 0.0f;
    bool draggingScroll_ = false;
    float dragStartMouseX_ = 0.0f;
    float dragStartScrollX_ = 0.0f;
    HealthSystem health_{};
    RhythmSystem rhythm_{};
    ScoreComboSystem score_{};
    TuneySystem tuneySystem_{};
    std::unordered_map<uint32_t, uint32_t> tuneyToNote_{};
    int debugPerfectCount_ = 0;
    int debugGoodCount_ = 0;
    int debugMissCount_ = 0;
    int debugEarlyCount_ = 0;
    int debugLateCount_ = 0;
    std::string debugLastStatus_ = "None";
    int debugLastDeltaMs_ = 0;
    int rhythmTimerMs_ = 1000;
    int rhythmStartDelayMs_ = 0;
    int rhythmCenterShiftMs_ = 100;
    int targetFps_ = 60;
    int maxVsyncFps_ = 60;
    bool tuneysLoaded_ = false;
    class Assets* assetsRef_ = nullptr;
};

#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <vector>
#include <string>

class PlayerScreen : public IScreen {
public:
    void onEnter() override;
    void onExit() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;

private:
    struct Track {
        std::string fileRel;     // music/levels/xx-*.mp3 (relative to assets/)
        std::string namePngRel;  // sprites/UI/Menu/Russian/tracks/*.png
        std::string iconRel;     // sprites/player/trackicon_01.png
        std::string titleText;   // fallback
    };

    std::vector<Track> tracks_;
    int idx_ = 0;

    ui::SpriteButton btnPrev_{}, btnNext_{}, btnPlay_{}, back_{};
    ui::SpriteButton dbgLeft_{}, dbgRight_{}, dbgUp_{}, dbgDown_{};
    ui::Carousel carousel_{};

    ::Music music_{};
    bool musicOk_ = false;
    bool eqProcessorAttached_ = false;
    bool playing_ = false;
    bool trackDirty_ = true;
    bool remixState_ = false;
    class App* app_ = nullptr;

    void scanTracks();
    void loadTrack(const UpdateContext& ctx, int i);
    void ensureTrackLoaded(const UpdateContext& ctx);
};

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
        std::string namePngRel;  // sprites/UI/Menu/Russian/0100.png etc
        std::string iconRel;     // sprites/player/trackicon_01.png
        std::string titleText;   // fallback
    };

    std::vector<Track> tracks_;
    int idx_ = 0;

    ui::SpriteButton btnPrev_{}, btnNext_{}, btnPlay_{}, back_{};
    ui::Carousel carousel_{};

    ::Music music_{};
    bool musicOk_ = false;
    bool playing_ = false;

    void scanTracks();
    void loadTrack(int i);
};

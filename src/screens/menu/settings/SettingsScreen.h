#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"

class SettingsScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    void rebuildLayout();
    float calcMaxScroll(int viewportHeight) const;

    ui::SpriteButton btnVibOn_{}, btnVibOff_{}, btnMusicOn_{}, btnMusicOff_{}, btnRemixOn_{}, btnRemixOff_{}, back_{};
    Rectangle vibroPanel_{};
    Rectangle musicPanel_{};
    Rectangle remixPanel_{};
    Rectangle volumePanel_{};
    Rectangle notesPanel_{};
    Rectangle soundPanel_{};
    Rectangle sliderVolume_{};
    Rectangle sliderNotes_{};
    Rectangle sliderSound_{};
    bool draggingVolume_ = false;
    bool draggingNotes_ = false;
    bool draggingSound_ = false;
    bool vibroActive_ = false;
    bool musicActive_ = false;
    bool remixActive_ = false;
    bool volumeActive_ = false;
    bool notesActive_ = false;
    bool soundActive_ = false;
    float scrollY_ = 0.0f;
    float maxScrollY_ = 0.0f;
    bool draggingScrollbar_ = false;
    float scrollbarGrabOffsetY_ = 0.0f;
};

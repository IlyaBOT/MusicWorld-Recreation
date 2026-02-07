#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"

class SettingsScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    ui::SpriteButton vibToggle_{};
    Rectangle sliderNotes_{};
    Rectangle sliderSound_{};
    bool draggingNotes_ = false;
    bool draggingSound_ = false;
};

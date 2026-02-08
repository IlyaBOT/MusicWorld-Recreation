#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"

class ModeSelectScreen : public IScreen {
public:
    enum class Target { Gameplay, Records };

    explicit ModeSelectScreen(Target target = Target::Gameplay) : target_(target) {}
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    Target target_ = Target::Gameplay;
    ui::SpriteButton dj_{}, story_{}, free_{}, party_{}, back_{};
};

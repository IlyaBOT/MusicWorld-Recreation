#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"

class TitleScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;

private:
    ui::SpriteButton tapButton_{};
    float bgAnimTimer_ = 0.0f;
};

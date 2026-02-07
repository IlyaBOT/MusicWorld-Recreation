#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"

class ModeSelectScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    ui::SpriteButton dj_{}, story_{}, free_{}, party_{};
};

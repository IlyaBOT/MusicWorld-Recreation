#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <vector>

class MenuMainScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::vector<ui::SpriteButton> items_;
};

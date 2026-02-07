#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <vector>
#include <string>

class HelpScreen : public IScreen {
public:
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::vector<std::string> pages_;
    int idx_ = 0;
    ui::SpriteButton btnPrev_{}, btnNext_{};
    ui::Carousel carousel_{};

    void scanPages();
};

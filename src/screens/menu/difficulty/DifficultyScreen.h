#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <string>

class DifficultyScreen : public IScreen {
public:
    explicit DifficultyScreen(std::string mode) : mode_(std::move(mode)) {}
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::string mode_;
    ui::SpriteButton easy_{}, med_{}, hard_{};
};

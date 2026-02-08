#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <string>

class DifficultyScreen : public IScreen {
public:
    enum class Target { Gameplay, Records };

    explicit DifficultyScreen(std::string mode, Target target = Target::Gameplay)
        : mode_(std::move(mode)), target_(target) {}
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::string mode_;
    Target target_ = Target::Gameplay;
    ui::SpriteButton easy_{}, med_{}, hard_{}, back_{};
};

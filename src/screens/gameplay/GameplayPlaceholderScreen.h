#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <string>

class GameplayPlaceholderScreen : public IScreen {
public:
    GameplayPlaceholderScreen(std::string mode, std::string diff) : mode_(std::move(mode)), diff_(std::move(diff)) {}
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::string mode_;
    std::string diff_;
    ui::SpriteButton back_{};
};

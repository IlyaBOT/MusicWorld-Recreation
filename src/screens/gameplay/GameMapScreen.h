#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <string>

class GameMapScreen : public IScreen {
public:
    GameMapScreen(std::string mode, std::string diff) : mode_(std::move(mode)), diff_(std::move(diff)) {}

    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;

private:
    void refreshProgress(const struct Profile& profile);

    std::string mode_;
    std::string diff_;
    ui::SpriteButton back_{};
    bool storyCompleted_ = false;
    int bestReachedLevel_ = 1;
    int unlockedLevels_ = 1;
};


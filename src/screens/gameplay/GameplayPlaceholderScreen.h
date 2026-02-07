#pragma once
#include "screens/IScreen.h"
#include <string>

class GameplayPlaceholderScreen : public IScreen {
public:
    GameplayPlaceholderScreen(std::string mode, std::string diff) : mode_(std::move(mode)), diff_(std::move(diff)) {}
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    std::string mode_;
    std::string diff_;
};

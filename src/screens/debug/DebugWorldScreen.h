#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <vector>

class DebugWorldScreen : public IScreen {
public:
    void onEnter() override;
    void onExit() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;

private:
    void rebuildButtons();

    std::vector<ui::SpriteButton> buttons_{};
    Rectangle barRect_{};
    float bgAnimTimer_ = 0.0f;
    float scrollX_ = 0.0f;
    float maxScrollX_ = 0.0f;
    bool draggingScroll_ = false;
    float dragStartMouseX_ = 0.0f;
    float dragStartScrollX_ = 0.0f;
    int hp_ = 12;
    bool tuneysLoaded_ = false;
    class Assets* assetsRef_ = nullptr;
};

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
    void rebuildItems(bool includeDebugWorldButton);

    std::vector<ui::SpriteButton> items_;
    bool includeDebugWorldButton_ = false;
    int menuTitleTapCount_ = 0;
    bool mobileDebugArm_ = false;
};

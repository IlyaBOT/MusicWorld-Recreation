#pragma once
#include "screens/IScreen.h"

class TitleScreen : public IScreen {
public:
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
};

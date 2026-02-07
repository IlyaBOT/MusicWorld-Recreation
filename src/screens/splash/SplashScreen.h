#pragma once
#include "screens/IScreen.h"

class SplashScreen : public IScreen {
public:
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
private:
    float timer_ = 0.0f;
};

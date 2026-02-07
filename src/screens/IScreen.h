#pragma once
#include <memory>

struct UpdateContext;
struct DrawContext;

class IScreen {
public:
    virtual ~IScreen() = default;
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void update(const UpdateContext& ctx) = 0;
    virtual void draw(const DrawContext& ctx) = 0;
};
using ScreenPtr = std::unique_ptr<IScreen>;

#pragma once
#include "screens/IScreen.h"
#include "ui/Widgets.h"
#include <string>

namespace RecordsFlow {

struct ModeScreen : public IScreen {
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
    ui::SpriteButton story{}, free{}, party{}, back{};
};

struct RecordsScreen : public IScreen {
    RecordsScreen(std::string mode, std::string diff) : mode(std::move(mode)), diff(std::move(diff)) {}
    void onEnter() override;
    void update(const UpdateContext& ctx) override;
    void draw(const DrawContext& ctx) override;
    std::string mode;
    std::string diff;
    int level = 1;
    ui::SpriteButton btnPrev{}, btnNext{}, back{};
    ui::Carousel carousel{};
};

} // namespace RecordsFlow

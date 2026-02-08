#pragma once
#include "ui/Widgets.h"

inline void SetupMenuBackButton(ui::SpriteButton& b, int vh = 400) {
    b = {};
    b.rect = {1.0f, (float)(vh - 1 - 38), 38.0f, 38.0f};
    b.bgRel = "sprites/UI/Menu/Buttons/1393.png";
    b.bgRelActive = "sprites/UI/Menu/Buttons/1395.png";
    b.labelRel = "sprites/UI/Menu/Buttons/back-btn.png";
    b.labelRelActive = "sprites/UI/Menu/Buttons/back-btn_selected.png";
}

#pragma once
#include "core/Assets.h"
#include <string>

inline bool CanLoadSpritePath(const std::string& rel) {
    Image img = LoadImage(Assets::A(rel).c_str());
    bool ok = (img.data != nullptr);
    if (ok) UnloadImage(img);
    return ok;
}

inline const char* ResolveSpritePath(const char* primary, const char* fallback) {
    return CanLoadSpritePath(primary) ? primary : fallback;
}

inline std::string ResolveSelectedSpritePath(const std::string& baseRel) {
    if (baseRel.size() <= 4 || baseRel.rfind(".png") != baseRel.size() - 4) return baseRel;
    std::string selected = baseRel.substr(0, baseRel.size() - 4) + "_selected.png";
    return CanLoadSpritePath(selected) ? selected : baseRel;
}

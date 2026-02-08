#pragma once
#include "core/Assets.h"
#include <string>

inline const char* ResolveSpritePath(const char* primary, const char* fallback) {
    return FileExists(Assets::A(primary).c_str()) ? primary : fallback;
}

inline std::string ResolveSelectedSpritePath(const std::string& baseRel) {
    if (baseRel.size() <= 4 || baseRel.rfind(".png") != baseRel.size() - 4) return baseRel;
    std::string selected = baseRel.substr(0, baseRel.size() - 4) + "_selected.png";
    return FileExists(Assets::A(selected).c_str()) ? selected : baseRel;
}

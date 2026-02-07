#pragma once
#include "core/Assets.h"

inline const char* ResolveSpritePath(const char* primary, const char* fallback) {
    return FileExists(Assets::A(primary).c_str()) ? primary : fallback;
}

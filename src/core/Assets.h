#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

struct TextureHandle {
    Texture2D tex{};
    bool ok = false;
};

struct SoundHandle {
    Sound snd{};
    bool ok = false;
};

class Assets {
public:
    void init();
    void shutdown();

    TextureHandle tex(const std::string& relPath);
    SoundHandle sfx(const std::string& relPath);
    void preloadTuneySprites();
    void unloadTuneySprites();

    static std::string A(const std::string& rel);

private:
    std::unordered_map<std::string, TextureHandle> tex_;
    std::unordered_map<std::string, SoundHandle> sfx_;
    std::vector<std::string> tuneySpriteKeys_;
    TextureHandle missingTex_{};
};

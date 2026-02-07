#include "core/Assets.h"

void Assets::init() {
    Image img = GenImageColor(2, 2, MAGENTA);
    missingTex_.tex = LoadTextureFromImage(img);
    SetTextureFilter(missingTex_.tex, TEXTURE_FILTER_POINT);
    UnloadImage(img);
    missingTex_.ok = true;
}

void Assets::shutdown() {
    for (auto& kv : tex_) {
        if (kv.second.ok) UnloadTexture(kv.second.tex);
    }
    tex_.clear();

    for (auto& kv : sfx_) {
        if (kv.second.ok) UnloadSound(kv.second.snd);
    }
    sfx_.clear();

    if (missingTex_.ok) UnloadTexture(missingTex_.tex);
    missingTex_ = {};
}

std::string Assets::A(const std::string& rel) {
    return std::string("assets/") + rel;
}

TextureHandle Assets::tex(const std::string& relPath) {
    auto it = tex_.find(relPath);
    if (it != tex_.end()) return it->second;

    TextureHandle h{};
    std::string path = A(relPath);
    if (FileExists(path.c_str())) {
        h.tex = LoadTexture(path.c_str());
        SetTextureFilter(h.tex, TEXTURE_FILTER_POINT);
        h.ok = (h.tex.id != 0);
    }
    if (!h.ok) h = missingTex_;
    tex_[relPath] = h;
    return h;
}

SoundHandle Assets::sfx(const std::string& relPath) {
    auto it = sfx_.find(relPath);
    if (it != sfx_.end()) return it->second;

    SoundHandle h{};
    std::string path = A(relPath);
    if (FileExists(path.c_str())) {
        h.snd = LoadSound(path.c_str());
        h.ok = (h.snd.frameCount > 0);
    }
    sfx_[relPath] = h;
    return h;
}

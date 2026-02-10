#include "core/Assets.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string_view>

namespace {
constexpr Color kChromaGreen = {0x1f, 0x8f, 0x20, 0xff};
constexpr int kChromaTolerance = 40;

bool IsChromaGreen(Color px, Color key, int tol2) {
    int dr = (int)px.r - (int)key.r;
    int dg = (int)px.g - (int)key.g;
    int db = (int)px.b - (int)key.b;
    if ((dr * dr + dg * dg + db * db) > tol2) return false;

    // Keep keying limited to green-dominant shades to avoid deleting unrelated colors.
    int pr = (int)px.r;
    int pg = (int)px.g;
    int pb = (int)px.b;
    return pg >= pr + 8 && pg >= pb + 8;
}

Image ApplyChromaKeyWithTolerance(const Image& source, Color key, int tolerance) {
    Image out = ImageCopy(source);
    if (!out.data) return {};

    ImageFormat(&out, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    auto* pixels = reinterpret_cast<Color*>(out.data);
    if (!pixels) {
        UnloadImage(out);
        return {};
    }

    const int count = out.width * out.height;
    const int tol2 = tolerance * tolerance;
    for (int i = 0; i < count; ++i) {
        if (IsChromaGreen(pixels[i], key, tol2)) {
            pixels[i].a = 0;
        }
    }

    return out;
}

Texture2D LoadTextureWithChromaKey(const std::string& path) {
    Texture2D t{};
    Image img = LoadImage(path.c_str());
    if (!img.data) return t;

    Image keyed = ApplyChromaKeyWithTolerance(img, kChromaGreen, kChromaTolerance);
    if (keyed.data) {
        t = LoadTextureFromImage(keyed);
        UnloadImage(keyed);
    } else {
        t = LoadTextureFromImage(img);
    }
    UnloadImage(img);
    return t;
}

bool IsBackgroundTexturePath(std::string_view relPath) {
    if (relPath.rfind("sprites/LevelBackgrounds/", 0) == 0) return true;
    if (relPath == "sprites/UI/background_half.png") return true;
    return false;
}
}

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
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS) || defined(ANDROID) || defined(__ANDROID__)
    return rel;
#else
    return std::string("assets/") + rel;
#endif
}

TextureHandle Assets::tex(const std::string& relPath) {
    auto it = tex_.find(relPath);
    if (it != tex_.end()) return it->second;

    TextureHandle h{};
    std::string path = A(relPath);
    if (IsBackgroundTexturePath(relPath)) {
        h.tex = LoadTexture(path.c_str());
    } else {
        h.tex = LoadTextureWithChromaKey(path);
        if (h.tex.id == 0) h.tex = LoadTexture(path.c_str());
    }
    h.ok = (h.tex.id != 0);
    if (h.ok) SetTextureFilter(h.tex, TEXTURE_FILTER_POINT);
    if (!h.ok) h = missingTex_;
    tex_[relPath] = h;
    return h;
}

SoundHandle Assets::sfx(const std::string& relPath) {
    auto it = sfx_.find(relPath);
    if (it != sfx_.end()) return it->second;

    SoundHandle h{};
    std::string path = A(relPath);
    h.snd = LoadSound(path.c_str());
    h.ok = (h.snd.frameCount > 0);
    sfx_[relPath] = h;
    return h;
}

void Assets::preloadTuneySprites() {
    if (!tuneySpriteKeys_.empty()) return;

    namespace fs = std::filesystem;
    const fs::path root = fs::path(A("sprites/Tuneys"));
    std::error_code ec;
    if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) return;

    std::vector<std::string> keys;
    for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) continue;
        const fs::directory_entry& entry = *it;
        if (!entry.is_regular_file(ec)) continue;

        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return (char)std::tolower(c); });
        if (ext != ".png") continue;

        const std::string fullPath = entry.path().generic_string();
        const std::string marker = "sprites/Tuneys/";
        size_t pos = fullPath.find(marker);
        if (pos == std::string::npos) continue;
        std::string rel = fullPath.substr(pos);

        auto h = tex(rel);
        if (h.ok) keys.push_back(rel);
    }

    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    tuneySpriteKeys_ = std::move(keys);
}

void Assets::unloadTuneySprites() {
    for (auto it = tex_.begin(); it != tex_.end();) {
        if (it->first.rfind("sprites/Tuneys/", 0) != 0) {
            ++it;
            continue;
        }
        if (it->second.ok && (!missingTex_.ok || it->second.tex.id != missingTex_.tex.id)) {
            UnloadTexture(it->second.tex);
        }
        it = tex_.erase(it);
    }
    tuneySpriteKeys_.clear();
}

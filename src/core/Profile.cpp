#include "core/Profile.h"
#include <algorithm>
#include <fstream>
#include <sstream>

static bool toBool(const std::string& v) {
    return v == "1" || v == "true" || v == "yes" || v == "on";
}

static int clampi(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

void Profile::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq);
        std::string v = line.substr(eq + 1);

        if (k == "storyCompleted") storyCompleted = toBool(v);
        else if (k == "vibration") vibration = toBool(v);
        else if (k == "musicEnabled") musicEnabled = toBool(v);
        else if (k == "musicRemix") musicRemix = toBool(v);
        else if (k == "masterVolume") masterVolume = clampi(std::stoi(v), 0, 100);
        else if (k == "syncNotes") syncNotes = std::stoi(v);
        else if (k == "syncSound") syncSound = std::stoi(v);
        else if (k.rfind("score.", 0) == 0) {
            scores[k.substr(6)] = std::stoi(v);
        }
    }
}

void Profile::save(const std::string& path) const {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return;
    f << "# MusicWorld-Recreation profile\n";
    f << "storyCompleted=" << (storyCompleted ? "1" : "0") << "\n";
    f << "vibration=" << (vibration ? "1" : "0") << "\n";
    f << "musicEnabled=" << (musicEnabled ? "1" : "0") << "\n";
    f << "musicRemix=" << (musicRemix ? "1" : "0") << "\n";
    f << "masterVolume=" << clampi(masterVolume, 0, 100) << "\n";
    f << "syncNotes=" << syncNotes << "\n";
    f << "syncSound=" << syncSound << "\n";
    for (auto& kv : scores) {
        f << "score." << kv.first << "=" << kv.second << "\n";
    }
}

int Profile::getScore(const std::string& key, int def) const {
    auto it = scores.find(key);
    return it == scores.end() ? def : it->second;
}

void Profile::setScore(const std::string& key, int value) {
    scores[key] = value;
}

std::string Profile::makeScoreKey(const std::string& mode, const std::string& diff, int level) {
    std::ostringstream ss;
    ss << mode << "." << diff << "." << level;
    return ss.str();
}

#pragma once
#include <string>
#include <unordered_map>

struct Profile {
    bool storyCompleted = false;

    bool vibration = true;
    int syncNotes = 0;   // -2..+2
    int syncSound = 0;   // -400..+400

    std::unordered_map<std::string, int> scores;

    void load(const std::string& path);
    void save(const std::string& path) const;

    int getScore(const std::string& key, int def = 0) const;
    void setScore(const std::string& key, int value);

    static std::string makeScoreKey(const std::string& mode, const std::string& diff, int level);
};

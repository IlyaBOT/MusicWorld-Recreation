#pragma once
#include <string>
#include <unordered_map>

struct Profile {
    struct Record {
        int score = 0;
        int good = 0;
        int perfect = 0;
        int combo = 0;
        int maxCombo = 0;
        char grade = 'F';
    };

    bool storyCompleted = false;

    bool vibration = true;
    bool musicEnabled = true;
    bool musicRemix = false;
    int masterVolume = 100; // 0..100
    int syncNotes = 0;   // -2..+2
    int syncSound = 0;   // -400..+400

    std::unordered_map<std::string, int> scores;
    std::unordered_map<std::string, Record> records;

    void load(const std::string& path);
    void save(const std::string& path) const;

    int getScore(const std::string& key, int def = 0) const;
    void setScore(const std::string& key, int value);
    Record getRecord(const std::string& key) const;
    void setRecord(const std::string& key, const Record& record);

    static std::string makeScoreKey(const std::string& mode, const std::string& diff, int level);
    static char gradeFromScore(int score);
};

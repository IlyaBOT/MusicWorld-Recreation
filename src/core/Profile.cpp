#include "core/Profile.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>

static bool toBool(const std::string& v) {
    return v == "1" || v == "true" || v == "yes" || v == "on";
}

static int clampi(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

static int parseIntSafe(const std::string& s, int fallback = 0) {
    try {
        size_t pos = 0;
        int value = std::stoi(s, &pos);
        if (pos != s.size()) return fallback;
        return value;
    } catch (...) {
        return fallback;
    }
}

static char normalizeGradeChar(char c) {
    switch (std::toupper((unsigned char)c)) {
    case 'S': return 'S';
    case 'A': return 'A';
    case 'B': return 'B';
    case 'C': return 'C';
    case 'D': return 'D';
    case 'E': return 'E';
    case 'F': return 'F';
    default: return '\0';
    }
}

static Profile::Record makeLegacyRecordFromScore(int score) {
    Profile::Record rec;
    rec.score = std::max(0, score);
    rec.grade = Profile::gradeFromScore(rec.score);
    rec.good = rec.score > 0 ? rec.score / 67 : 0;
    rec.perfect = rec.score > 0 ? rec.score / 84 : 0;
    rec.combo = rec.score > 0 ? rec.score / 28 : 0;
    rec.maxCombo = rec.score > 0 ? rec.score / 43 : 0;
    return rec;
}

static bool parseRecordCsv(const std::string& value, Profile::Record& out) {
    std::array<std::string, 6> fields{};
    std::stringstream ss(value);
    for (int i = 0; i < (int)fields.size(); ++i) {
        if (!std::getline(ss, fields[i], ',')) return false;
    }

    Profile::Record rec;
    rec.score = std::max(0, parseIntSafe(fields[0], 0));
    rec.good = std::max(0, parseIntSafe(fields[2], 0));
    rec.perfect = std::max(0, parseIntSafe(fields[3], 0));
    rec.combo = std::max(0, parseIntSafe(fields[4], 0));
    rec.maxCombo = std::max(0, parseIntSafe(fields[5], 0));

    char gradeRaw = '\0';
    for (char c : fields[1]) {
        if (!std::isspace((unsigned char)c)) {
            gradeRaw = c;
            break;
        }
    }
    char grade = normalizeGradeChar(gradeRaw);
    rec.grade = grade ? grade : Profile::gradeFromScore(rec.score);

    out = rec;
    return true;
}

void Profile::load(const std::string& path) {
    scores.clear();
    records.clear();

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
        else if (k == "masterVolume") masterVolume = clampi(parseIntSafe(v, 100), 0, 100);
        else if (k == "syncNotes") syncNotes = clampi(parseIntSafe(v, 0), -2, 2);
        else if (k == "syncSound") syncSound = clampi(parseIntSafe(v, 0), -400, 400);
        else if (k.rfind("record.", 0) == 0) {
            Record rec;
            if (parseRecordCsv(v, rec)) setRecord(k.substr(7), rec);
        }
        else if (k.rfind("score.", 0) == 0) {
            setScore(k.substr(6), parseIntSafe(v, 0));
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

    std::set<std::string> keys;
    for (const auto& kv : scores) keys.insert(kv.first);
    for (const auto& kv : records) keys.insert(kv.first);
    for (const auto& key : keys) {
        Record rec = getRecord(key);
        f << "record." << key << "="
          << rec.score << ","
          << rec.grade << ","
          << rec.good << ","
          << rec.perfect << ","
          << rec.combo << ","
          << rec.maxCombo << "\n";
    }
}

int Profile::getScore(const std::string& key, int def) const {
    auto rec = records.find(key);
    if (rec != records.end()) return rec->second.score;
    auto it = scores.find(key);
    return it == scores.end() ? def : it->second;
}

void Profile::setScore(const std::string& key, int value) {
    int clean = std::max(0, value);
    scores[key] = clean;
    records[key] = makeLegacyRecordFromScore(clean);
}

Profile::Record Profile::getRecord(const std::string& key) const {
    auto rec = records.find(key);
    if (rec != records.end()) return rec->second;

    auto it = scores.find(key);
    if (it != scores.end()) return makeLegacyRecordFromScore(it->second);

    return {};
}

void Profile::setRecord(const std::string& key, const Profile::Record& record) {
    Record rec = record;
    rec.score = std::max(0, rec.score);
    rec.good = std::max(0, rec.good);
    rec.perfect = std::max(0, rec.perfect);
    rec.combo = std::max(0, rec.combo);
    rec.maxCombo = std::max(0, rec.maxCombo);

    char grade = normalizeGradeChar(rec.grade);
    rec.grade = grade ? grade : gradeFromScore(rec.score);

    records[key] = rec;
    scores[key] = rec.score;
}

std::string Profile::makeScoreKey(const std::string& mode, const std::string& diff, int level) {
    std::ostringstream ss;
    ss << mode << "." << diff << "." << level;
    return ss.str();
}

char Profile::gradeFromScore(int score) {
    if (score >= 9000) return 'S';
    if (score >= 7000) return 'A';
    if (score >= 5000) return 'B';
    if (score >= 3000) return 'C';
    if (score >= 1000) return 'D';
    if (score > 0) return 'E';
    return 'F';
}

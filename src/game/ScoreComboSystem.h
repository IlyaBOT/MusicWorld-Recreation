#pragma once

#include "game/RhythmTypes.h"

class ScoreComboSystem {
public:
    struct Snapshot {
        int score = 0;
        int combo = 0;
        int maxCombo = 0;
        int perfect = 0;
        int good = 0;
        int miss = 0;
        int skip = 0;
    };

    void reset();
    void applyJudgement(RhythmJudgement judgement);

    const Snapshot& snapshot() const { return state_; }

    int score() const { return state_.score; }
    int combo() const { return state_.combo; }
    int maxCombo() const { return state_.maxCombo; }
    bool comboActive() const { return state_.combo > 2; }

private:
    Snapshot state_{};
};


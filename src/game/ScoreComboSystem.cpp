#include "game/ScoreComboSystem.h"

#include <algorithm>

void ScoreComboSystem::reset() {
    state_ = {};
}

void ScoreComboSystem::applyJudgement(RhythmJudgement judgement) {
    if (judgement == RhythmJudgement::None) return;

    state_.score += ScoreDeltaForJudgement(judgement);

    switch (judgement) {
        case RhythmJudgement::Perfect: ++state_.perfect; break;
        case RhythmJudgement::Good: ++state_.good; break;
        case RhythmJudgement::Miss: ++state_.miss; break;
        case RhythmJudgement::Skip: ++state_.skip; break;
        case RhythmJudgement::None: break;
    }

    if (JudgementKeepsCombo(judgement)) {
        ++state_.combo;
        state_.maxCombo = std::max(state_.maxCombo, state_.combo);
    } else if (JudgementBreaksCombo(judgement)) {
        state_.combo = 0;
    }
}


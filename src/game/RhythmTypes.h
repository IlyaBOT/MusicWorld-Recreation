#pragma once

enum class RhythmJudgement {
    None = 0,
    Perfect,
    Good,
    Miss,
    Skip
};

inline int ScoreDeltaForJudgement(RhythmJudgement judgement) {
    switch (judgement) {
        case RhythmJudgement::Perfect: return 15;
        case RhythmJudgement::Good: return 10;
        case RhythmJudgement::Miss: return 0;
        case RhythmJudgement::Skip: return 0;
        case RhythmJudgement::None: return 0;
    }
    return 0;
}

inline bool JudgementKeepsCombo(RhythmJudgement judgement) {
    return judgement == RhythmJudgement::Perfect || judgement == RhythmJudgement::Good;
}

inline bool JudgementBreaksCombo(RhythmJudgement judgement) {
    return judgement == RhythmJudgement::Miss || judgement == RhythmJudgement::Skip;
}


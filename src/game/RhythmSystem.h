#pragma once

#include "game/RhythmTypes.h"

#include <cstdint>
#include <string>
#include <vector>

class RhythmSystem {
public:
    struct TimingConfig {
        int earlyTapMs = 150;   // Lower bound for early hits (before center).
        int lateTapMs = 299;    // Upper bound for late hits (after center).
        int perfectTapMs = 150; // |delta| <= 150 -> perfect.
        int goodTapMs = 299;    // 176..299 -> good.
        int missTapMs = 350;    // 300..350 -> miss.
        int centerOffsetMs = 100; // Center shift in formula: (timerMs/2) + centerOffsetMs.

        float missAnimationFps = 10.0f;
        int missAnimationLoops = 4;
        float lastChanceAnimationFps = 5.0f;
        float fallAnimationFps = 8.0f;
    };

    enum class NotePhase {
        WaitingPress,
        MissAnimation,
        LastChanceAnimation,
        FallingAnimation,
        Completed
    };

    struct AnimationFrame {
        std::string spriteRel;
        float durationSec = 0.2f;
    };

    struct AnimationClip {
        std::vector<AnimationFrame> frames;
        bool loop = false;
    };

    struct PressResult {
        bool consumed = false;
        uint32_t noteId = 0;
        uint32_t tuneyId = 0;
        RhythmJudgement judgement = RhythmJudgement::None;
        int deltaMs = 0;
        int absDeltaMs = 0;
        int scoreDelta = 0;
    };

    struct JudgementEvent {
        uint32_t noteId = 0;
        uint32_t tuneyId = 0;
        RhythmJudgement judgement = RhythmJudgement::None;
        int scoreDelta = 0;
        int deltaMs = 0;
        int absDeltaMs = 0;
    };

    struct NoteSnapshot {
        uint32_t noteId = 0;
        uint32_t tuneyId = 0;
        int timerPeriodMs = 0;
        float elapsedMs = 0.0f;
        float centerMs = 0.0f;
        NotePhase phase = NotePhase::WaitingPress;
        RhythmJudgement lastJudgement = RhythmJudgement::None;
    };

    RhythmSystem();
    explicit RhythmSystem(const TimingConfig& config);

    void setTimingConfig(const TimingConfig& config);
    const TimingConfig& timingConfig() const { return config_; }

    uint32_t createNote(uint32_t tuneyId, int timerPeriodMs, int initialElapsedMs = 0);
    bool destroyNote(uint32_t noteId);
    bool markFallenOut(uint32_t noteId);
    void clear();

    void update(float dtSec);
    PressResult press(uint32_t noteId);

    std::vector<JudgementEvent> consumeJudgementEvents();
    std::vector<NoteSnapshot> snapshot() const;

    const char* currentAnimationSprite(uint32_t noteId) const;
    bool isFalling(uint32_t noteId) const;
    size_t activeCount() const { return notes_.size(); }

    static AnimationClip makeMissAnimation(const TimingConfig& config);
    static AnimationClip makeLastChanceAnimation(const TimingConfig& config);
    static AnimationClip makeFallAnimation(const TimingConfig& config);

private:
    struct NoteTimer {
        int periodMs = 1000;
        float elapsedMs = 0.0f;
        float centerMs = 500.0f;

        void configure(int timerPeriodMs, int centerOffsetMs, int initialElapsedMs);
        void setCenterOffset(int centerOffsetMs);
        void advance(float dtMs);
    };

    struct AnimationPlayer {
        AnimationClip clip{};
        size_t frameIndex = 0;
        float frameTimer = 0.0f;
        bool finished = true;

        void start(const AnimationClip& animation);
        void update(float dtSec);
        const char* spriteRel() const;
    };

    struct NoteRuntime {
        uint32_t noteId = 0;
        uint32_t tuneyId = 0;
        NoteTimer timer{};
        NotePhase phase = NotePhase::WaitingPress;
        RhythmJudgement lastJudgement = RhythmJudgement::None;
        AnimationPlayer animation{};
    };

    TimingConfig config_{};
    uint32_t nextNoteId_ = 1;
    std::vector<NoteRuntime> notes_{};
    std::vector<JudgementEvent> events_{};

    NoteRuntime* find(uint32_t noteId);
    const NoteRuntime* find(uint32_t noteId) const;

    void pushJudgementEvent(const NoteRuntime& note, RhythmJudgement judgement, int deltaMs, int absDeltaMs);
    PressResult makeResult(const NoteRuntime& note, RhythmJudgement judgement, int deltaMs, int absDeltaMs, bool consumed) const;
};

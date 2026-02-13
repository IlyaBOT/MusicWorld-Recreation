#include "game/RhythmSystem.h"

#include <algorithm>
#include <cmath>

namespace {

float safeFps(float fps) {
    return (fps > 0.001f) ? fps : 1.0f;
}

float frameDurationFromFps(float fps) {
    return 1.0f / safeFps(fps);
}

void pushFrames(RhythmSystem::AnimationClip& clip, const char* a, const char* b, int loops, float frameDuration) {
    const int count = std::max(1, loops);
    clip.frames.reserve(clip.frames.size() + (size_t)(count * 2));
    for (int i = 0; i < count; ++i) {
        clip.frames.push_back({a, frameDuration});
        clip.frames.push_back({b, frameDuration});
    }
}

} // namespace

RhythmSystem::RhythmSystem()
    : config_(TimingConfig{}) {}

RhythmSystem::RhythmSystem(const TimingConfig& config)
    : config_(config) {}

void RhythmSystem::NoteTimer::configure(int timerPeriodMs, int centerOffsetMs, int initialElapsedMs) {
    periodMs = std::max(1, timerPeriodMs);
    elapsedMs = (float)std::max(0, initialElapsedMs);
    centerMs = periodMs * 0.5f + (float)centerOffsetMs;
}

void RhythmSystem::NoteTimer::setCenterOffset(int centerOffsetMs) {
    centerMs = periodMs * 0.5f + (float)centerOffsetMs;
}

void RhythmSystem::NoteTimer::advance(float dtMs) {
    elapsedMs += std::max(0.0f, dtMs);
}

void RhythmSystem::setTimingConfig(const TimingConfig& config) {
    config_ = config;
    for (auto& note : notes_) {
        note.timer.setCenterOffset(config_.centerOffsetMs);
    }
}

RhythmSystem::AnimationClip RhythmSystem::makeMissAnimation(const TimingConfig& config) {
    AnimationClip clip{};
    const float frameDuration = frameDurationFromFps(config.missAnimationFps);
    pushFrames(
        clip,
        "sprites/Tuneys/Normal/0941.png",
        "sprites/Tuneys/Normal/0942.png",
        config.missAnimationLoops,
        frameDuration
    );
    clip.loop = false;
    return clip;
}

RhythmSystem::AnimationClip RhythmSystem::makeLastChanceAnimation(const TimingConfig& config) {
    AnimationClip clip{};
    const float frameDuration = frameDurationFromFps(config.lastChanceAnimationFps);
    clip.frames.push_back({"sprites/Tuneys/Normal/0943.png", frameDuration});
    clip.frames.push_back({"sprites/Tuneys/Normal/0944.png", frameDuration});
    clip.loop = false;
    return clip;
}

RhythmSystem::AnimationClip RhythmSystem::makeFallAnimation(const TimingConfig& config) {
    AnimationClip clip{};
    const float frameDuration = frameDurationFromFps(config.fallAnimationFps);
    clip.frames.push_back({"sprites/Tuneys/Normal/0950.png", frameDuration});
    clip.frames.push_back({"sprites/Tuneys/Normal/0951.png", frameDuration});
    clip.frames.push_back({"sprites/Tuneys/Normal/0952.png", frameDuration});
    clip.frames.push_back({"sprites/Tuneys/Normal/0953.png", frameDuration});
    clip.loop = true;
    return clip;
}

void RhythmSystem::AnimationPlayer::start(const AnimationClip& animation) {
    clip = animation;
    frameIndex = 0;
    frameTimer = 0.0f;
    finished = clip.frames.empty();
}

void RhythmSystem::AnimationPlayer::update(float dtSec) {
    if (finished || clip.frames.empty()) return;

    frameTimer += std::max(0.0f, dtSec);
    while (frameIndex < clip.frames.size()) {
        const float frameDuration = std::max(0.001f, clip.frames[frameIndex].durationSec);
        if (frameTimer < frameDuration) break;

        frameTimer -= frameDuration;
        ++frameIndex;
        if (frameIndex < clip.frames.size()) continue;

        if (clip.loop && !clip.frames.empty()) {
            frameIndex = 0;
        } else {
            frameIndex = clip.frames.empty() ? 0 : (clip.frames.size() - 1);
            finished = true;
            break;
        }
    }
}

const char* RhythmSystem::AnimationPlayer::spriteRel() const {
    if (clip.frames.empty() || frameIndex >= clip.frames.size()) return nullptr;
    return clip.frames[frameIndex].spriteRel.c_str();
}

uint32_t RhythmSystem::createNote(uint32_t tuneyId, int timerPeriodMs, int initialElapsedMs) {
    NoteRuntime note{};
    note.noteId = nextNoteId_++;
    note.tuneyId = tuneyId;
    note.timer.configure(timerPeriodMs, config_.centerOffsetMs, initialElapsedMs);
    note.phase = NotePhase::WaitingPress;
    note.lastJudgement = RhythmJudgement::None;

    notes_.push_back(note);
    return note.noteId;
}

bool RhythmSystem::destroyNote(uint32_t noteId) {
    auto it = std::find_if(notes_.begin(), notes_.end(), [noteId](const NoteRuntime& n) {
        return n.noteId == noteId;
    });
    if (it == notes_.end()) return false;
    notes_.erase(it);
    return true;
}

bool RhythmSystem::markFallenOut(uint32_t noteId) {
    NoteRuntime* note = find(noteId);
    if (!note) return false;
    note->phase = NotePhase::Completed;
    return true;
}

void RhythmSystem::clear() {
    notes_.clear();
    events_.clear();
}

void RhythmSystem::pushJudgementEvent(const NoteRuntime& note, RhythmJudgement judgement, int deltaMs, int absDeltaMs) {
    events_.push_back({
        note.noteId,
        note.tuneyId,
        judgement,
        ScoreDeltaForJudgement(judgement),
        deltaMs,
        absDeltaMs
    });
}

RhythmSystem::PressResult RhythmSystem::makeResult(
    const NoteRuntime& note,
    RhythmJudgement judgement,
    int deltaMs,
    int absDeltaMs,
    bool consumed) const {
    PressResult result{};
    result.consumed = consumed;
    result.noteId = note.noteId;
    result.tuneyId = note.tuneyId;
    result.judgement = judgement;
    result.deltaMs = deltaMs;
    result.absDeltaMs = absDeltaMs;
    result.scoreDelta = ScoreDeltaForJudgement(judgement);
    return result;
}

RhythmSystem::PressResult RhythmSystem::press(uint32_t noteId) {
    NoteRuntime* note = find(noteId);
    if (!note) return {};
    if (note->phase != NotePhase::WaitingPress) return makeResult(*note, RhythmJudgement::None, 0, 0, false);

    const int deltaMs = (int)std::lround(note->timer.elapsedMs - note->timer.centerMs);
    const int absDeltaMs = std::abs(deltaMs);
    const bool insideEarlyLate = (deltaMs >= -config_.earlyTapMs) && (deltaMs <= config_.lateTapMs);

    RhythmJudgement judgement = RhythmJudgement::None;
    if (insideEarlyLate && absDeltaMs <= config_.perfectTapMs) {
        judgement = RhythmJudgement::Perfect;
    } else if (insideEarlyLate && absDeltaMs <= config_.goodTapMs) {
        judgement = RhythmJudgement::Good;
    } else if (absDeltaMs <= config_.missTapMs) {
        judgement = RhythmJudgement::Miss;
    } else {
        return makeResult(*note, RhythmJudgement::None, deltaMs, absDeltaMs, false);
    }

    note->lastJudgement = judgement;
    if (judgement == RhythmJudgement::Miss) {
        note->phase = NotePhase::MissAnimation;
        note->animation.start(makeMissAnimation(config_));
    } else {
        note->phase = NotePhase::Completed;
    }

    pushJudgementEvent(*note, judgement, deltaMs, absDeltaMs);
    return makeResult(*note, judgement, deltaMs, absDeltaMs, true);
}

void RhythmSystem::update(float dtSec) {
    const float dtMs = std::max(0.0f, dtSec) * 1000.0f;

    for (auto& note : notes_) {
        note.timer.advance(dtMs);

        switch (note.phase) {
            case NotePhase::WaitingPress: {
                const float skipThreshold = note.timer.centerMs + (float)config_.missTapMs;
                if (note.timer.elapsedMs > skipThreshold) {
                    note.lastJudgement = RhythmJudgement::Skip;
                    note.phase = NotePhase::LastChanceAnimation;
                    note.animation.start(makeLastChanceAnimation(config_));
                    const int deltaMs = (int)std::lround(note.timer.elapsedMs - note.timer.centerMs);
                    pushJudgementEvent(note, RhythmJudgement::Skip, deltaMs, std::abs(deltaMs));
                }
            } break;
            case NotePhase::MissAnimation:
                note.animation.update(dtSec);
                if (note.animation.finished) {
                    note.phase = NotePhase::Completed;
                }
                break;
            case NotePhase::LastChanceAnimation:
                note.animation.update(dtSec);
                if (note.animation.finished) {
                    note.phase = NotePhase::FallingAnimation;
                    note.animation.start(makeFallAnimation(config_));
                }
                break;
            case NotePhase::FallingAnimation:
                note.animation.update(dtSec);
                break;
            case NotePhase::Completed:
                break;
        }
    }

    notes_.erase(
        std::remove_if(notes_.begin(), notes_.end(), [](const NoteRuntime& note) {
            return note.phase == NotePhase::Completed;
        }),
        notes_.end()
    );
}

std::vector<RhythmSystem::JudgementEvent> RhythmSystem::consumeJudgementEvents() {
    std::vector<JudgementEvent> out;
    out.swap(events_);
    return out;
}

std::vector<RhythmSystem::NoteSnapshot> RhythmSystem::snapshot() const {
    std::vector<NoteSnapshot> result;
    result.reserve(notes_.size());
    for (const auto& note : notes_) {
        result.push_back({
            note.noteId,
            note.tuneyId,
            note.timer.periodMs,
            note.timer.elapsedMs,
            note.timer.centerMs,
            note.phase,
            note.lastJudgement
        });
    }
    return result;
}

const char* RhythmSystem::currentAnimationSprite(uint32_t noteId) const {
    const NoteRuntime* note = find(noteId);
    if (!note) return nullptr;
    switch (note->phase) {
        case NotePhase::MissAnimation:
        case NotePhase::LastChanceAnimation:
        case NotePhase::FallingAnimation:
            return note->animation.spriteRel();
        case NotePhase::WaitingPress:
        case NotePhase::Completed:
            return nullptr;
    }
    return nullptr;
}

bool RhythmSystem::isFalling(uint32_t noteId) const {
    const NoteRuntime* note = find(noteId);
    return note && note->phase == NotePhase::FallingAnimation;
}

RhythmSystem::NoteRuntime* RhythmSystem::find(uint32_t noteId) {
    for (auto& note : notes_) {
        if (note.noteId == noteId) return &note;
    }
    return nullptr;
}

const RhythmSystem::NoteRuntime* RhythmSystem::find(uint32_t noteId) const {
    for (const auto& note : notes_) {
        if (note.noteId == noteId) return &note;
    }
    return nullptr;
}

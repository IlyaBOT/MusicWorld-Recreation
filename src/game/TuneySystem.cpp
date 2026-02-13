#include "game/TuneySystem.h"
#include "core/Assets.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kMinSegmentLength = 0.001f;

std::string makeSpriteRel(const char* typeFolder, int frameNumber) {
    return "sprites/Tuneys/" + std::string(typeFolder) + "/" + TextFormat("%04i", frameNumber) + ".png";
}

const char* typeFolder(TuneySystem::Type type) {
    switch (type) {
        case TuneySystem::Type::Normal: return "Normal";
        case TuneySystem::Type::Fat: return "Fat";
        case TuneySystem::Type::Scratch: return "Scratch";
    }
    return "Normal";
}

void appendFrameRange(TuneySystem::Animation& animation, const char* folder, int first, int last, float frameDuration) {
    if (last < first) return;
    animation.frames.reserve(animation.frames.size() + (size_t)(last - first + 1));
    for (int frame = first; frame <= last; ++frame) {
        animation.frames.push_back({makeSpriteRel(folder, frame), frameDuration});
    }
}

Vector2 subtract(Vector2 a, Vector2 b) {
    return {a.x - b.x, a.y - b.y};
}

float length(Vector2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vector2 normalize(Vector2 v) {
    float len = length(v);
    if (len <= kMinSegmentLength) return {0.0f, 0.0f};
    return {v.x / len, v.y / len};
}

} // namespace

void TuneySystem::AnimRuntime::start(const Animation& animation) {
    clip = animation;
    frameIndex = 0;
    frameTimer = 0.0f;
    finished = clip.frames.empty();
}

void TuneySystem::AnimRuntime::update(float dt) {
    if (finished || clip.frames.empty()) return;

    frameTimer += dt;
    while (frameIndex < clip.frames.size()) {
        const float frameDuration = std::max(0.001f, clip.frames[frameIndex].duration);
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

const char* TuneySystem::AnimRuntime::currentSpriteRel() const {
    if (clip.frames.empty() || frameIndex >= clip.frames.size()) return nullptr;
    return clip.frames[frameIndex].spriteRel.c_str();
}

const char* TuneySystem::spawnSprite(Type type) {
    switch (type) {
        case Type::Normal: return "sprites/Tuneys/Normal/0734.png";
        case Type::Fat: return "sprites/Tuneys/Fat/0761.png";
        case Type::Scratch: return "sprites/Tuneys/Scratch/0532.png";
    }
    return "sprites/Tuneys/Normal/0734.png";
}

const char* TuneySystem::touchHoldSprite(Type type) {
    switch (type) {
        case Type::Normal: return "sprites/Tuneys/Normal/0742.png";
        case Type::Fat: return "sprites/Tuneys/Fat/0769.png";
        case Type::Scratch: return "sprites/Tuneys/Scratch/0540.png";
    }
    return "sprites/Tuneys/Normal/0742.png";
}

TuneySystem::Animation TuneySystem::makePreTouchAnimation(Type type, float frameDuration) {
    Animation anim{};
    const char* folder = typeFolder(type);
    switch (type) {
        case Type::Normal: appendFrameRange(anim, folder, 735, 741, frameDuration); break;
        case Type::Fat: appendFrameRange(anim, folder, 762, 768, frameDuration); break;
        case Type::Scratch: appendFrameRange(anim, folder, 533, 539, frameDuration); break;
    }
    return anim;
}

TuneySystem::Animation TuneySystem::makeDestroyAnimation(Type type, float frameDuration) {
    Animation anim{};
    const char* folder = typeFolder(type);
    switch (type) {
        case Type::Normal:
            for (int frame = 741; frame >= 735; --frame) anim.frames.push_back({makeSpriteRel(folder, frame), frameDuration});
            break;
        case Type::Fat:
            for (int frame = 768; frame >= 762; --frame) anim.frames.push_back({makeSpriteRel(folder, frame), frameDuration});
            break;
        case Type::Scratch:
            for (int frame = 539; frame >= 533; --frame) anim.frames.push_back({makeSpriteRel(folder, frame), frameDuration});
            break;
    }
    return anim;
}

void TuneySystem::buildPath(Tuney& tuney, const SpawnParams& params) {
    tuney.path.clear();
    tuney.path.push_back(params.spawnPoint);
    if (params.movement == Movement::Curve) {
        for (const auto& p : params.curvePoints) tuney.path.push_back(p);
    }
    tuney.path.push_back(params.targetPoint);
    tuney.pathSegment = 0;
}

float TuneySystem::distanceToTarget(const Tuney& tuney) {
    Vector2 v = subtract(tuney.target, tuney.pos);
    return length(v);
}

void TuneySystem::advanceMovement(Tuney& tuney, float dt) {
    if (tuney.path.empty()) return;
    float remaining = std::max(0.0f, tuney.speed * dt);
    while (remaining > 0.0f && tuney.pathSegment + 1 < tuney.path.size()) {
        const Vector2 toNext = subtract(tuney.path[tuney.pathSegment + 1], tuney.pos);
        const float toNextLen = length(toNext);
        if (toNextLen <= kMinSegmentLength) {
            tuney.pos = tuney.path[tuney.pathSegment + 1];
            ++tuney.pathSegment;
            continue;
        }

        if (remaining >= toNextLen) {
            tuney.pos = tuney.path[tuney.pathSegment + 1];
            ++tuney.pathSegment;
            remaining -= toNextLen;
            continue;
        }

        Vector2 dir = normalize(toNext);
        tuney.pos = {tuney.pos.x + dir.x * remaining, tuney.pos.y + dir.y * remaining};
        remaining = 0.0f;
    }
}

const char* TuneySystem::currentSpriteRel(const Tuney& tuney) {
    if (!tuney.externalSpriteRel.empty()) return tuney.externalSpriteRel.c_str();

    switch (tuney.state) {
        case State::FlyingIn: return spawnSprite(tuney.type);
        case State::PreTouch: return tuney.anim.currentSpriteRel();
        case State::TouchHold: return touchHoldSprite(tuney.type);
        case State::Destroying: return tuney.anim.currentSpriteRel();
        case State::Dead: break;
    }
    return nullptr;
}

void TuneySystem::pushEvent(const Tuney& tuney, EventType typeEvent) {
    events_.push_back({tuney.id, tuney.type, typeEvent});
}

uint32_t TuneySystem::create(const SpawnParams& params) {
    Tuney tuney{};
    tuney.id = nextId_++;
    tuney.type = params.type;
    tuney.movement = params.movement;
    tuney.pos = params.spawnPoint;
    tuney.target = params.targetPoint;
    tuney.speed = std::max(1.0f, params.speed);
    tuney.preTouchDistance = std::max(1.0f, params.preTouchDistancePx);
    tuney.touchHoldSeconds = std::max(0.0f, params.touchHoldSeconds);
    tuney.touchHoldTimer = tuney.touchHoldSeconds;
    tuney.preTouchAnimation = params.preTouchAnimation.frames.empty()
        ? makePreTouchAnimation(params.type)
        : params.preTouchAnimation;
    tuney.destroyAnimation = makeDestroyAnimation(params.type);
    buildPath(tuney, params);
    tuneys_.push_back(std::move(tuney));
    return tuneys_.back().id;
}

bool TuneySystem::destroy(uint32_t id) {
    Tuney* tuney = find(id);
    if (!tuney) return false;
    return destroy(id, tuney->destroyAnimation);
}

bool TuneySystem::destroy(uint32_t id, const Animation& destroyAnimation) {
    Tuney* tuney = find(id);
    if (!tuney || tuney->state == State::Dead) return false;
    tuney->state = State::Destroying;
    tuney->anim.start(destroyAnimation);
    tuney->externalSpriteRel.clear();
    if (destroyAnimation.frames.empty()) {
        tuney->state = State::Dead;
        pushEvent(*tuney, EventType::Destroyed);
    }
    return true;
}

void TuneySystem::clear() {
    tuneys_.clear();
    events_.clear();
}

void TuneySystem::update(float dt) {
    for (auto& tuney : tuneys_) {
        switch (tuney.state) {
            case State::FlyingIn: {
                advanceMovement(tuney, dt);
                if (distanceToTarget(tuney) <= tuney.preTouchDistance) {
                    tuney.state = State::PreTouch;
                    tuney.anim.start(tuney.preTouchAnimation);
                    pushEvent(tuney, EventType::EnteredPreTouch);
                }
            } break;
            case State::PreTouch:
                tuney.anim.update(dt);
                if (tuney.anim.finished) {
                    tuney.state = State::TouchHold;
                    tuney.touchHoldTimer = tuney.touchHoldSeconds;
                    pushEvent(tuney, EventType::EnteredTouchHold);
                }
                break;
            case State::TouchHold:
                if (tuney.touchHoldTimer > 0.0f) {
                    tuney.touchHoldTimer -= dt;
                    if (tuney.touchHoldTimer < 0.0f) tuney.touchHoldTimer = 0.0f;
                }
                break;
            case State::Destroying:
                tuney.anim.update(dt);
                if (tuney.anim.finished) {
                    tuney.state = State::Dead;
                    pushEvent(tuney, EventType::Destroyed);
                }
                break;
            case State::Dead:
                break;
        }
    }

    tuneys_.erase(
        std::remove_if(tuneys_.begin(), tuneys_.end(), [](const Tuney& t) { return t.state == State::Dead; }),
        tuneys_.end()
    );
}

void TuneySystem::draw(Assets& assets) const {
    for (const auto& tuney : tuneys_) {
        const char* spriteRel = currentSpriteRel(tuney);
        if (!spriteRel) continue;
        Texture2D tex = assets.tex(spriteRel).tex;
        if (!tex.id) continue;
        DrawTexture(tex, (int)(tuney.pos.x - tex.width * 0.5f), (int)(tuney.pos.y - tex.height * 0.5f), WHITE);
    }
}

bool TuneySystem::getPosition(uint32_t id, Vector2& outPosition) const {
    const Tuney* tuney = find(id);
    if (!tuney) return false;
    outPosition = tuney->pos;
    return true;
}

bool TuneySystem::setPosition(uint32_t id, Vector2 position) {
    Tuney* tuney = find(id);
    if (!tuney) return false;
    tuney->pos = position;
    return true;
}

bool TuneySystem::isAtOrBelowY(uint32_t id, float y) const {
    const Tuney* tuney = find(id);
    if (!tuney) return false;
    return tuney->pos.y >= y;
}

bool TuneySystem::anyAtOrBelowY(float y) const {
    for (const auto& tuney : tuneys_) {
        if (tuney.pos.y >= y) return true;
    }
    return false;
}

std::vector<TuneySystem::Event> TuneySystem::consumeEvents() {
    std::vector<Event> out;
    out.swap(events_);
    return out;
}

std::vector<TuneySystem::Snapshot> TuneySystem::snapshot() const {
    std::vector<Snapshot> out;
    out.reserve(tuneys_.size());
    for (const auto& tuney : tuneys_) {
        const char* sprite = currentSpriteRel(tuney);
        out.push_back({tuney.id, tuney.type, tuney.state, tuney.pos, sprite ? std::string(sprite) : std::string{}});
    }
    return out;
}

bool TuneySystem::setExternalSprite(uint32_t id, const std::string& spriteRel) {
    Tuney* tuney = find(id);
    if (!tuney) return false;
    tuney->externalSpriteRel = spriteRel;
    return true;
}

bool TuneySystem::clearExternalSprite(uint32_t id) {
    Tuney* tuney = find(id);
    if (!tuney) return false;
    tuney->externalSpriteRel.clear();
    return true;
}

TuneySystem::Tuney* TuneySystem::find(uint32_t id) {
    for (auto& tuney : tuneys_) {
        if (tuney.id == id) return &tuney;
    }
    return nullptr;
}

const TuneySystem::Tuney* TuneySystem::find(uint32_t id) const {
    for (const auto& tuney : tuneys_) {
        if (tuney.id == id) return &tuney;
    }
    return nullptr;
}


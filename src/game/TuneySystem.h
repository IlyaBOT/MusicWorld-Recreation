#pragma once

#include "raylib.h"
#include <cstdint>
#include <string>
#include <vector>

class Assets;

class TuneySystem {
public:
    enum class Type {
        Normal,
        Fat,
        Scratch
    };

    enum class Movement {
        Linear,
        Curve
    };

    enum class State {
        FlyingIn,
        PreTouch,
        TouchHold,
        Destroying,
        Dead
    };

    enum class EventType {
        EnteredPreTouch,
        EnteredTouchHold,
        Destroyed
    };

    struct Event {
        uint32_t tuneyId = 0;
        Type type = Type::Normal;
        EventType typeEvent = EventType::EnteredPreTouch;
    };

    struct Snapshot {
        uint32_t tuneyId = 0;
        Type type = Type::Normal;
        State state = State::FlyingIn;
        Vector2 pos{};
        std::string spriteRel;
    };

    struct Frame {
        std::string spriteRel;
        float duration = 0.06f;
    };

    struct Animation {
        std::vector<Frame> frames;
        bool loop = false;
    };

    struct SpawnParams {
        Type type = Type::Normal;
        Movement movement = Movement::Linear;
        Vector2 spawnPoint{};
        Vector2 targetPoint{};
        std::vector<Vector2> curvePoints{};
        float speed = 180.0f;
        float preTouchDistancePx = kDefaultPreTouchDistancePx;
        float touchHoldSeconds = kDefaultTouchHoldSeconds;
        Animation preTouchAnimation{};
    };

    static constexpr float kDefaultPreTouchDistancePx = 42.0f;
    static constexpr float kDefaultTouchHoldSeconds = 0.2f;
    static constexpr float kDefaultFrameSeconds = 0.06f;

    uint32_t create(const SpawnParams& params);
    bool destroy(uint32_t id);
    bool destroy(uint32_t id, const Animation& destroyAnimation);
    void clear();

    void update(float dt);
    void draw(Assets& assets) const;

    bool getPosition(uint32_t id, Vector2& outPosition) const;
    bool setPosition(uint32_t id, Vector2 position);
    bool isAtOrBelowY(uint32_t id, float y) const;
    bool anyAtOrBelowY(float y) const;
    size_t count() const { return tuneys_.size(); }

    std::vector<Event> consumeEvents();
    std::vector<Snapshot> snapshot() const;

    bool setExternalSprite(uint32_t id, const std::string& spriteRel);
    bool clearExternalSprite(uint32_t id);

    static Animation makePreTouchAnimation(Type type, float frameDuration = kDefaultFrameSeconds);
    static Animation makeDestroyAnimation(Type type, float frameDuration = kDefaultFrameSeconds);
    static const char* spawnSprite(Type type);
    static const char* touchHoldSprite(Type type);

private:
    struct AnimRuntime {
        Animation clip{};
        size_t frameIndex = 0;
        float frameTimer = 0.0f;
        bool finished = true;

        void start(const Animation& animation);
        void update(float dt);
        const char* currentSpriteRel() const;
    };

    struct Tuney {
        uint32_t id = 0;
        Type type = Type::Normal;
        Movement movement = Movement::Linear;
        State state = State::FlyingIn;
        Vector2 pos{};
        Vector2 target{};
        float speed = 180.0f;
        float preTouchDistance = kDefaultPreTouchDistancePx;
        float touchHoldSeconds = kDefaultTouchHoldSeconds;
        float touchHoldTimer = kDefaultTouchHoldSeconds;
        std::vector<Vector2> path{};
        size_t pathSegment = 0;
        Animation preTouchAnimation{};
        Animation destroyAnimation{};
        AnimRuntime anim{};
        std::string externalSpriteRel{};
    };

    uint32_t nextId_ = 1;
    std::vector<Tuney> tuneys_{};
    std::vector<Event> events_{};

    static void buildPath(Tuney& tuney, const SpawnParams& params);
    static void advanceMovement(Tuney& tuney, float dt);
    static float distanceToTarget(const Tuney& tuney);
    static const char* currentSpriteRel(const Tuney& tuney);

    void pushEvent(const Tuney& tuney, EventType typeEvent);

    Tuney* find(uint32_t id);
    const Tuney* find(uint32_t id) const;
};


#pragma once

class HealthSystem {
public:
    static constexpr int kMinHp = 0;
    static constexpr int kMaxHp = 25;
    static constexpr int kDefaultHp = 12;
    static constexpr int kSegmentCount = 25;

    HealthSystem() = default;
    explicit HealthSystem(int hp) { set(hp); }

    void reset(int hp = kDefaultHp);
    void set(int hp);
    void increase(int amount = 1);
    void decrease(int amount = 1);

    int value() const { return hp_; }
    bool isDepleted() const { return hp_ <= kMinHp; }

    static const char* segmentSpriteRel(int slot);

private:
    int hp_ = kDefaultHp;
};


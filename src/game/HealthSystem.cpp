#include "game/HealthSystem.h"
#include <algorithm>

namespace {
constexpr const char* kSegments[HealthSystem::kSegmentCount] = {
    "sprites/UI/HealthBar/1195.png", "sprites/UI/HealthBar/1195.png", "sprites/UI/HealthBar/1195.png",
    "sprites/UI/HealthBar/1196.png", "sprites/UI/HealthBar/1196.png", "sprites/UI/HealthBar/1196.png",
    "sprites/UI/HealthBar/1197.png", "sprites/UI/HealthBar/1197.png", "sprites/UI/HealthBar/1197.png",
    "sprites/UI/HealthBar/1198.png", "sprites/UI/HealthBar/1198.png", "sprites/UI/HealthBar/1198.png",
    "sprites/UI/HealthBar/1199.png", "sprites/UI/HealthBar/1199.png", "sprites/UI/HealthBar/1203.png",
    "sprites/UI/HealthBar/1200.png", "sprites/UI/HealthBar/1200.png", "sprites/UI/HealthBar/1200.png",
    "sprites/UI/HealthBar/1201.png", "sprites/UI/HealthBar/1201.png", "sprites/UI/HealthBar/1201.png",
    "sprites/UI/HealthBar/1202.png", "sprites/UI/HealthBar/1202.png", "sprites/UI/HealthBar/1202.png",
    "sprites/UI/HealthBar/1202.png"
};
}

void HealthSystem::reset(int hp) {
    set(hp);
}

void HealthSystem::set(int hp) {
    hp_ = std::clamp(hp, kMinHp, kMaxHp);
}

void HealthSystem::increase(int amount) {
    if (amount <= 0) return;
    set(hp_ + amount);
}

void HealthSystem::decrease(int amount) {
    if (amount <= 0) return;
    set(hp_ - amount);
}

const char* HealthSystem::segmentSpriteRel(int slot) {
    if (slot < 0 || slot >= kSegmentCount) return nullptr;
    return kSegments[slot];
}


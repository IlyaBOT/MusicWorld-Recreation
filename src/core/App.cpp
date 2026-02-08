#include "core/App.h"
#include <algorithm>
#include <filesystem>

IScreen* App::top() { return stack_.empty() ? nullptr : stack_.back().get(); }

void App::init() {
    std::filesystem::create_directories("save");
    vs.init(240, 400);
    assets.init();
    InitAudioDevice();

    profile.load(profilePath_);
    SetMasterVolume(std::max(0.0f, std::min(1.0f, profile.masterVolume / 100.0f)));
    lastTime_ = (float)GetTime();
    quitRequested_ = false;
}

void App::shutdown() {
    saveProfile();
    while (!stack_.empty()) pop();
    CloseAudioDevice();
    assets.shutdown();
    vs.shutdown();
}

void App::push(ScreenPtr s) {
    if (!s) return;
    stack_.push_back(std::move(s));
    stack_.back()->onEnter();
}

void App::pop() {
    if (stack_.empty()) return;
    stack_.back()->onExit();
    stack_.pop_back();
}

void App::replace(ScreenPtr s) { pop(); push(std::move(s)); }

void App::clearAndPush(ScreenPtr s) { while (!stack_.empty()) pop(); push(std::move(s)); }

void App::playSfx(const std::string& rel) {
    auto h = assets.sfx(rel);
    if (h.ok) PlaySound(h.snd);
}

void App::saveProfile() { profile.save(profilePath_); }

void App::requestQuit() { quitRequested_ = true; }

bool App::quitRequested() const { return quitRequested_; }

void App::runOneFrame() {
    float now = (float)GetTime();
    float dt = now - lastTime_;
    lastTime_ = now;
    if (dt > 0.1f) dt = 0.1f;

    input.update(vs);
    if (IsKeyPressed(KEY_F1)) debug = !debug;

    UpdateContext u{dt, &vs, &input, &assets, &profile, &toast, &debug, this};
    DrawContext d{&vs, &assets, &profile, &toast, debug};

    toast.update(dt);
    if (top()) top()->update(u);

    vs.begin();
    if (top()) top()->draw(d);

    if (debug) {
        DrawRectangle(0, 0, 240, 16, Fade(BLACK, 0.35f));
        DrawText(TextFormat("debug | stack=%d", (int)stack_.size()), 4, 3, 12, YELLOW);
        auto st = input.state();
        DrawText(TextFormat("x=%.1f y=%.1f swipe=%d", st.mouseV.x, st.mouseV.y, (int)st.swipe), 4, 18, 12, Fade(RAYWHITE, 0.85f));
    }

    toast.draw(vs.vw, vs.vh, debug);
    vs.end();
}

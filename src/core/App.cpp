#include "core/App.h"
#include <algorithm>
#include <filesystem>
#include <string_view>

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
    unloadMenuMusic();
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
    menuMusicKeepAlive_ = false;
    if (top()) top()->update(u);
    updateMenuMusic();

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

void App::requestMenuMusic() {
    menuMusicKeepAlive_ = true;
}

void App::pauseMenuMusic() {
    menuMusicPaused_ = true;
}

void App::resumeMenuMusic() {
    menuMusicPaused_ = false;
}

std::string App::resolveMusicRel(const std::string& rel) const {
    constexpr std::string_view kRemixPrefix = "music/remix/";
    constexpr std::string_view kBasePrefix = "music/";
    if (!profile.musicRemix) return rel;
    if (rel.rfind(kRemixPrefix.data(), 0) == 0) return rel;
    if (rel.rfind(kBasePrefix.data(), 0) != 0) return rel;

    std::string candidate = std::string(kRemixPrefix) + rel.substr(kBasePrefix.size());
    if (FileExists(Assets::A(candidate).c_str())) return candidate;
    return rel;
}

void App::updateMenuMusic() {
    if (!profile.musicEnabled) {
        unloadMenuMusic();
        menuMusicPaused_ = false;
        return;
    }

    bool wantRemix = profile.musicRemix;
    if (menuMusicPaused_) {
        if (menuMusicOk_ && menuMusicPlaying_) {
            PauseMusicStream(menuMusic_);
            menuMusicPlaying_ = false;
        }
        return;
    }

    if (!menuMusicKeepAlive_) {
        unloadMenuMusic();
        return;
    }

    if (!menuMusicOk_ || wantRemix != menuMusicRemixState_) {
        unloadMenuMusic();
        std::string rel = resolveMusicRel("music/menu.mp3");
        std::string path = Assets::A(rel);
        if (FileExists(path.c_str())) {
            menuMusic_ = LoadMusicStream(path.c_str());
            menuMusicOk_ = (menuMusic_.ctxData != nullptr);
            if (menuMusicOk_) {
                menuMusic_.looping = true;
                SetMusicVolume(menuMusic_, 0.9f);
                menuMusicStarted_ = false;
                menuMusicRemixState_ = wantRemix;
            } else {
                menuMusic_ = {};
            }
        }
    }

    if (menuMusicOk_) {
        UpdateMusicStream(menuMusic_);
        if (!menuMusicPlaying_) {
            if (menuMusicStarted_) ResumeMusicStream(menuMusic_);
            else {
                PlayMusicStream(menuMusic_);
                menuMusicStarted_ = true;
            }
            menuMusicPlaying_ = true;
        }
    }
}

void App::unloadMenuMusic() {
    if (menuMusicOk_) {
        StopMusicStream(menuMusic_);
        UnloadMusicStream(menuMusic_);
    }
    menuMusic_ = {};
    menuMusicOk_ = false;
    menuMusicPlaying_ = false;
    menuMusicStarted_ = false;
}

#pragma once
#include "core/VirtualScreen.h"
#include "core/Input.h"
#include "core/Assets.h"
#include "core/Profile.h"
#include "ui/Widgets.h"
#include "screens/IScreen.h"
#include <vector>
#include <string>

struct UpdateContext {
    float dt = 0.0f;
    VirtualScreen* vs = nullptr;
    Input* input = nullptr;
    Assets* assets = nullptr;
    Profile* profile = nullptr;
    ui::Toast* toast = nullptr;
    bool* debug = nullptr;
    class App* app = nullptr;
};

struct DrawContext {
    VirtualScreen* vs = nullptr;
    Assets* assets = nullptr;
    Profile* profile = nullptr;
    ui::Toast* toast = nullptr;
    bool debug = false;
};

class App {
public:
    void init();
    void shutdown();

    void push(ScreenPtr s);
    void pop();
    void replace(ScreenPtr s);
    void clearAndPush(ScreenPtr s);

    void runOneFrame();
    void requestQuit();
    bool quitRequested() const;

    void playSfx(const std::string& rel);
    void saveProfile();

    VirtualScreen vs;
    Input input;
    Assets assets;
    Profile profile;
    ui::Toast toast;
    bool debug = false;

private:
    std::vector<ScreenPtr> stack_;
    float lastTime_ = 0.0f;
    std::string profilePath_ = "save/profile.cfg";
    bool quitRequested_ = false;

    IScreen* top();
};

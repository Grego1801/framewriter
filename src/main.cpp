#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fstream>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"

static int g_frame = 0;
static bool g_playing = false;

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);
        if (g_playing) {
            g_frame++;
            if (g_frame % 4 == 0) {
                std::ofstream f(FRAME_FILE, std::ios::trunc);
                if (f.is_open()) f << g_frame;
            }
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_frame = 0;
        g_playing = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_playing = true;
        std::ofstream f(FRAME_FILE, std::ios::trunc);
        if (f.is_open()) f << 0;
        return true;
    }

    void onQuit() {
        g_playing = false;
        g_frame = 0;
        std::ofstream f(FRAME_FILE, std::ios::trunc);
        if (f.is_open()) f << -1;
        PlayLayer::onQuit();
    }
};

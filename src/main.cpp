#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fstream>
#include <eclipse.hpp>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"
static bool g_playing = false;

void writeFrame(int f) {
    std::ofstream file(FRAME_FILE, std::ios::trunc);
    if (file.is_open()) file << f;
}

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);
        if (g_playing) {
            auto result = eclipse::label::getVariable<int64_t>("frameReal");
            if (result) {
                writeFrame((int)result.unwrap());
            }
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_playing = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_playing = true;
        writeFrame(0);
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        writeFrame(0);
    }

    void onQuit() {
        g_playing = false;
        writeFrame(-1);
        PlayLayer::onQuit();
    }
};

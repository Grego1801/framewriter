#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <cstdio>

using namespace geode::prelude;

#define FRAME_FILE "/tmp/gd_frame"
static bool g_playing = false;

void writeFrame(int f) {
    FILE* file = fopen(FRAME_FILE, "w");
    if (file) { fprintf(file, "%d", f); fclose(file); }
}

class $modify(GJBaseGameLayer) {
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        if (!g_playing) return;
        auto pl = PlayLayer::get();
        if (!pl) return;
        int frame = (int)round(pl->m_attemptTime * 480.0);
        writeFrame(frame);
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

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fstream>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"
static bool g_playing = false;
static int g_lastFrame = -1;

void writeFrame(int f) {
    FILE* file = fopen(FRAME_FILE, "w");
    if (file) { fprintf(file, "%d", f); fclose(file); }
}

class $modify(GJBaseGameLayer) {
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        if (!g_playing || !isLastTick || isHalfTick) return;
        
        auto pl = PlayLayer::get();
        if (!pl) return;
        
        // m_currentTime is elapsed seconds, multiply by TPS to get frame
        // Use m_gameState.m_unk1 or find correct field
        // For now derive from attempt time
        double t = pl->m_attemptTime;
        int frame = (int)round(t * 480.0);
        
        if (frame != g_lastFrame) {
            g_lastFrame = frame;
            writeFrame(frame);
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_playing = false;
        g_lastFrame = -1;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_playing = true;
        writeFrame(0);
        return true;
    }

    void resetLevel() {
        g_lastFrame = -1;
        PlayLayer::resetLevel();
        writeFrame(0);
    }

    void onQuit() {
        g_playing = false;
        writeFrame(-1);
        PlayLayer::onQuit();
    }
};

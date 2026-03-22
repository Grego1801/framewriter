#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fstream>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"

static int g_frame = 0;
static bool g_playing = false;

void writeFrame(int f) {
    FILE* file = fopen(FRAME_FILE, "w");
    if (file) {
        fprintf(file, "%d", f);
        fclose(file);
    }
}

class $modify(GJBaseGameLayer) {
    void update(float dt) {
        GJBaseGameLayer::update(dt);
        if (g_playing) {
            g_frame++;
            writeFrame(g_frame);
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
        writeFrame(0);
        return true;
    }

    void resetLevel() {
        g_frame = 0;
        PlayLayer::resetLevel();
        writeFrame(0);
    }

    void onQuit() {
        g_playing = false;
        g_frame = 0;
        writeFrame(-1);
        PlayLayer::onQuit();
    }
};

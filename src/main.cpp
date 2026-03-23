#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fcntl.h>
#include <unistd.h>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.bin"

static bool g_playing = false;
static int g_fd = -1;
static int g_frame = 0;

void openFile() {
    g_fd = open(FRAME_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
}

void writeFrame(int f) {
    if (g_fd >= 0) {
        pwrite(g_fd, &f, sizeof(int), 0);
    }
}

class $modify(GJBaseGameLayer) {
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        if (!g_playing || isHalfTick) return;
        g_frame++;
        writeFrame(g_frame);
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_playing = false;
        g_frame = 0;
        if (g_fd < 0) openFile();
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

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"

static bool g_dead = false;
static bool g_playing = false;

void writeState(int status, float percent) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d:%.2f", status, percent);
    int fd = open(STATE_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) {
        write(fd, buf, strlen(buf));
        close(fd);
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_dead = false;
        g_playing = true;
        writeState(0, 0.0f);
        return true;
    }

    void resetLevel() {
        g_dead = false;
        PlayLayer::resetLevel();
        writeState(0, 0.0f);
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        // Only count death if it's the MAIN player not p2
        // and only if we're actually playing
        if (g_playing && !g_dead && p == m_player1) {
            g_dead = true;
            float pct = this->getCurrentPercent();
            writeState(1, pct);
        }
        PlayLayer::destroyPlayer(p, o);
    }

    void onQuit() {
        g_playing = false;
        g_dead = false;
        writeState(2, 0.0f);
        PlayLayer::onQuit();
    }
};

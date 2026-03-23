#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"

static bool g_dead = false;
static bool g_started = false;
static bool g_playing = false;
static int g_fd = -1;

void writeState(int status, float percent) {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%d:%.2f", status, percent);
    if (g_fd < 0)
        g_fd = open(STATE_FILE, O_WRONLY | O_CREAT, 0666);
    if (g_fd >= 0) {
        lseek(g_fd, 0, SEEK_SET);
        write(g_fd, buf, len);
        ftruncate(g_fd, len);
        fsync(g_fd);
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_dead = false;
        g_started = false;
        g_playing = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_playing = true;
        writeState(0, 0.0f);
        return true;
    }

    void startGame() {
        PlayLayer::startGame();
        g_started = true;
        g_dead = false;
        writeState(0, 0.0f);
    }

    void resetLevel() {
        g_dead = false;
        g_started = false;
        PlayLayer::resetLevel();
        writeState(0, 0.0f);
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        if (g_started && !g_dead && p == m_player1) {
            g_dead = true;
            float pct = this->getCurrentPercent();
            writeState(1, pct);
        }
    }

    void onQuit() {
        g_playing = false;
        g_started = false;
        g_dead = false;
        writeState(2, 0.0f);
        PlayLayer::onQuit();
    }
};

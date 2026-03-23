#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"

static bool g_dead = false;
static bool g_started = false;

void writeState(int status, float percent) {
    FILE* f = fopen(STATE_FILE, "w");
    if (f) {
        fprintf(f, "%d:%.2f", status, percent);
        fflush(f);
        fclose(f);
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_dead = false;
        g_started = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
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
        // m_isDead is set by GD after destroyPlayer
        // Check if this is p1 dying for the first time this attempt
        if (g_started && !g_dead && p == m_player1) {
            g_dead = true;
            writeState(1, this->getCurrentPercent());
        }
    }

    void onQuit() {
        g_started = false;
        g_dead = false;
        writeState(2, 0.0f);
        PlayLayer::onQuit();
    }
};

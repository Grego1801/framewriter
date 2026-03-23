#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <fcntl.h>
#include <unistd.h>
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

class $modify(GJBaseGameLayer) {
    // playerTookDamage fires when player actually takes damage
    // More reliable than destroyPlayer
    void playerTookDamage(PlayerObject* player) {
        GJBaseGameLayer::playerTookDamage(player);
        if (g_started && !g_dead && player == m_player1) {
            g_dead = true;
            auto pl = PlayLayer::get();
            float pct = pl ? pl->getCurrentPercent() : 0.0f;
            writeState(1, pct);
        }
    }
};

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

    void onQuit() {
        g_started = false;
        g_dead = false;
        writeState(2, 0.0f);
        PlayLayer::onQuit();
    }
};

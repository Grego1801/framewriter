#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"

static bool g_started = false;
static bool g_wroteDeathThisAttempt = false;

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
        g_started = false;
        g_wroteDeathThisAttempt = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        writeState(0, 0.0f);
        return true;
    }

    void startGame() {
        PlayLayer::startGame();
        g_started = true;
        g_wroteDeathThisAttempt = false;
        writeState(0, 0.0f);
    }

    void resetLevel() {
        g_wroteDeathThisAttempt = false;
        g_started = false;
        PlayLayer::resetLevel();
        writeState(0, 0.0f);
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        // Use m_isDead which GD sets AFTER destroyPlayer
        // So check our own flag instead
        if (g_started && !g_wroteDeathThisAttempt && p == m_player1) {
            // Verify GD also thinks player is dead
            if (m_player1->m_isDead) {
                g_wroteDeathThisAttempt = true;
                writeState(1, this->getCurrentPercent());
            }
        }
    }

    void onQuit() {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        writeState(2, 0.0f);
        PlayLayer::onQuit();
    }
};

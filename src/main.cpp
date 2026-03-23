#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"
#define LOG_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_log.txt"

static bool g_started = false;
static bool g_wroteDeathThisAttempt = false;

void writeState(int status, float percent) {
    FILE* f = fopen(STATE_FILE, "w");
    if (f) { fprintf(f, "%d:%.2f", status, percent); fflush(f); fclose(f); }
}

void log(const char* msg) {
    FILE* f = fopen(LOG_FILE, "a");
    if (f) { fprintf(f, "%s\n", msg); fclose(f); }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        writeState(0, 0.0f);
        log("init");
        return true;
    }

    void startGame() {
        PlayLayer::startGame();
        g_started = true;
        g_wroteDeathThisAttempt = false;
        writeState(0, 0.0f);
        log("startGame -> g_wrote=false");
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_wroteDeathThisAttempt = false;
        g_started = false;
        writeState(0, 0.0f);
        log("resetLevel -> g_wrote=false");
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        char buf[128];
        snprintf(buf, sizeof(buf), "destroyPlayer started=%d wrote=%d isP1=%d isDead=%d",
            g_started, g_wroteDeathThisAttempt,
            (int)(p == m_player1),
            (int)(m_player1 && m_player1->m_isDead));
        log(buf);
        if (g_started && !g_wroteDeathThisAttempt && p == m_player1 && m_player1->m_isDead) {
            g_wroteDeathThisAttempt = true;
            writeState(1, this->getCurrentPercent());
            log("DEATH WRITTEN");
        }
    }

    void onQuit() {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        writeState(2, 0.0f);
        log("onQuit");
        PlayLayer::onQuit();
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <fstream>
#include <string>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"
#define LOG_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_log.txt"

static bool g_started = false;
static bool g_wroteDeathThisAttempt = false;

// Renamed to avoid ambiguity with math.h and geode::log
void writeState(int status, float percent) {
    std::ofstream f(STATE_FILE);
    if (f.is_open()) {
        f << status << ":" << std::fixed << std::setprecision(2) << percent;
        f.close();
    }
}

void aiLog(const std::string& msg) {
    std::ofstream f(LOG_FILE, std::ios::app);
    if (f.is_open()) {
        f << msg << "\n";
        f.close();
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        writeState(0, 0.0f);
        aiLog("init");
        return true;
    }

    void startGame() {
        PlayLayer::startGame();
        g_started = true;
        g_wroteDeathThisAttempt = false;
        writeState(0, 0.0f);
        aiLog("startGame -> g_wrote=false");
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_wroteDeathThisAttempt = false;
        g_started = false;
        writeState(0, 0.0f);
        aiLog("resetLevel -> g_wrote=false");
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        
        // Safety check for null pointers
        if (!p || !m_player1) return;

        std::string debugMsg = "destroyPlayer started=" + std::to_string(g_started) + 
                               " wrote=" + std::to_string(g_wroteDeathThisAttempt) + 
                               " isP1=" + std::to_string(p == m_player1) + 
                               " isDead=" + std::to_string(m_player1->m_isDead);
        aiLog(debugMsg);

        if (g_started && !g_wroteDeathThisAttempt && p == m_player1 && m_player1->m_isDead) {
            g_wroteDeathThisAttempt = true;
            writeState(1, this->getCurrentPercent());
            aiLog("DEATH WRITTEN");
        }
    }

    void onQuit() {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        writeState(2, 0.0f);
        aiLog("onQuit");
        PlayLayer::onQuit();
    }
};

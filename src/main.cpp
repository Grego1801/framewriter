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
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        g_started = true; // We are in the level
        g_wroteDeathThisAttempt = false;
        writeState(0, 0.0f);
        aiLog("init -> ready to play");
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        
        g_started = true; // Still in the level, physics are resetting
        g_wroteDeathThisAttempt = false; // Reset the death toggle
        writeState(0, 0.0f);
        aiLog("resetLevel -> state: ALIVE");
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        
        if (!p || !m_player1) return;

        // Condition 1: Is it the main player?
        // Condition 2: Have we NOT written death yet for this attempt?
        // Condition 3: Is the player actually flagged as dead?
        if (p == m_player1 && !g_wroteDeathThisAttempt && m_player1->m_isDead) {
            g_wroteDeathThisAttempt = true;
            float pct = this->getCurrentPercent();
            writeState(1, pct);
            aiLog("DEATH WRITTEN: " + std::to_string(pct) + "%");
        }
    }

    void onQuit() {
        g_started = false;
        g_wroteDeathThisAttempt = false;
        writeState(2, 0.0f);
        aiLog("onQuit -> state: MENU");
        PlayLayer::onQuit();
    }
};


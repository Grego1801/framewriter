#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fstream>
#include <string>

using namespace geode::prelude;

#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"

static int g_frame = 0;

class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        g_frame++;
        
        if (g_frame % 4 == 0) {
            std::ofstream f(FRAME_FILE, std::ios::trunc);
            if (f.is_open()) {
                f << g_frame;
            }
        }
    }
    
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        g_frame = 0;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        
        // Write 0 to signal level started
        std::ofstream f(FRAME_FILE, std::ios::trunc);
        if (f.is_open()) f << 0;
        
        return true;
    }
    
    void onQuit() {
        g_frame = 0;
        std::ofstream f(FRAME_FILE, std::ios::trunc);
        if (f.is_open()) f << -1;
        PlayLayer::onQuit();
    }
};

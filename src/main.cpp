#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fstream>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        
        // Try different field names for game time
        // In newer GD versions it might be m_gameState.m_levelTime
        // or accessed via getCurrentTime()
        float time = this->m_gameState.m_levelTime;
        int frame = (int)(time * 240.0f);
        
        if (frame % 4 == 0) {
            std::ofstream f("/sdcard/gd_frame.txt",
                           std::ios::trunc);
            if (f.is_open()) {
                f << frame;
            }
        }
    }
    
    void onQuit() {
        std::ofstream f("/sdcard/gd_frame.txt",
                       std::ios::trunc);
        if (f.is_open()) f << -1;
        PlayLayer::onQuit();
    }
};

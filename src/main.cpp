#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fstream>

using namespace geode::prelude;

// Write to Geode's own directory which GD has write access to
#define FRAME_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_frame.txt"

class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        
        float time = this->m_gameState.m_levelTime;
        int frame = (int)(time * 240.0f);
        
        if (frame % 4 == 0) {
            std::ofstream f(FRAME_FILE, std::ios::trunc);
            if (f.is_open()) {
                f << frame;
            }
        }
    }
    
    void onQuit() {
        std::ofstream f(FRAME_FILE, std::ios::trunc);
        if (f.is_open()) f << -1;
        PlayLayer::onQuit();
    }
};

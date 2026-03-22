#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fstream>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        
        // Access m_time through the PlayLayer instance
        auto pl = static_cast<PlayLayer*>(this);
        int frame = (int)(pl->m_time * 240.0f);
        
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

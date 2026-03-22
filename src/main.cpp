#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fstream>

using namespace geode::prelude;

class $modify(PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);
        
        // Write current frame number to file
        int frame = (int)(m_time * 240.0f);
        
        // Only write every 4 frames to reduce I/O
        if (frame % 4 == 0) {
            std::ofstream f("/sdcard/gd_frame.txt",
                           std::ios::trunc);
            if (f.is_open()) {
                f << frame;
            }
        }
    }
    
    void onQuit() {
        // Write -1 to signal level ended
        std::ofstream f("/sdcard/gd_frame.txt",
                       std::ios::trunc);
        if (f.is_open()) f << -1;
        PlayLayer::onQuit();
    }
};

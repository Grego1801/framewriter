#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace geode::prelude;

struct SharedData {
    int frame;
    bool isDead;
};

static SharedData* g_shared = nullptr;

class $modify(MyPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (!g_shared) {
            // Using Geode's internal folder to avoid permission issues
            int fd = open("/data/data/com.geode.launcher/files/gd_bot.bin", O_RDWR | O_CREAT, 0666);
            ftruncate(fd, sizeof(SharedData));
            g_shared = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            chmod("/data/data/com.geode.launcher/files/gd_bot.bin", 0666);
        }
        
        g_shared->frame = 0;
        g_shared->isDead = false;
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (g_shared && !m_isPaused) {
            // Read the ACTUAL physics step from the game state
            // This matches Eclipse and ensures 240Hz sync
            g_shared->frame = m_gameState.m_currentStep;
            g_shared->isDead = m_player1->m_isDead;
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (g_shared) {
            g_shared->frame = 0;
            g_shared->isDead = false;
        }
    }
};

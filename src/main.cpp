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
static int g_fd = -1;

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (!g_shared) {
            // Use /data/local/tmp for cross-process access on Android
            g_fd = open("/data/local/tmp/gd_bot.bin", O_RDWR | O_CREAT, 0666);
            ftruncate(g_fd, sizeof(SharedData));
            g_shared = (SharedData*)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, g_fd, 0);
            chmod("/data/local/tmp/gd_bot.bin", 0666); // Root access fix
        }
        
        g_shared->frame = 0;
        g_shared->isDead = false;
        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (g_shared && !m_isPaused) {
            g_shared->frame++;
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


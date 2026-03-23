#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <unistd.h>

using namespace geode::prelude;

#define STATE_FILE "/sdcard/Android/media/com.geode.launcher/game/gd_state.txt"

static bool g_dead = false;

void writeState(const char* state) {
    int fd = open(STATE_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) {
        write(fd, state, 1);
        close(fd);
    }
}

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_dead = false;
        writeState("0");
        return true;
    }

    void resetLevel() {
        g_dead = false;
        PlayLayer::resetLevel();
        writeState("0");
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        if (!g_dead) {
            g_dead = true;
            writeState("1");
        }
    }

    void onQuit() {
        g_dead = false;
        writeState("2");
        PlayLayer::onQuit();
    }
};

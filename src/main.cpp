#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <fcntl.h>
#include <unistd.h>

using namespace geode::prelude;

#define STATE_FILE "/data/local/tmp/gd_state.txt"

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
        writeState("0"); // alive
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        writeState("0"); // alive after reset
    }

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        PlayLayer::destroyPlayer(p, o);
        writeState("1"); // dead
    }

    void onQuit() {
        writeState("2"); // quit
        PlayLayer::onQuit();
    }
};

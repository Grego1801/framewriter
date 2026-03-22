#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace geode::prelude;

#define SHM_NAME "/gd_frame"

static bool g_playing = false;
static int* g_frame_ptr = nullptr;

void initShm() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) return;
    ftruncate(fd, sizeof(int));
    g_frame_ptr = (int*)mmap(NULL, sizeof(int),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (g_frame_ptr) *g_frame_ptr = -1;
}

void writeFrame(int f) {
    if (g_frame_ptr) *g_frame_ptr = f;
}

class $modify(GJBaseGameLayer) {
    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        if (!g_playing) return;
        auto pl = PlayLayer::get();
        if (!pl) return;
        int frame = (int)round(pl->m_attemptTime * 480.0);
        writeFrame(frame);
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!g_frame_ptr) initShm();
        g_playing = false;
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        g_playing = true;
        writeFrame(0);
        return true;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        writeFrame(0);
    }

    void onQuit() {
        g_playing = false;
        writeFrame(-1);
        PlayLayer::onQuit();
    }
};

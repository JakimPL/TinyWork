/*
 * TinyWork Framework - Default main.c
 * Standard entry point for 320x200 demos
 *
 * Override by setting MAIN_SRC in your Makefile if custom logic needed
 */

#include "runtime/runtime.h"
#include "video/video.h"

int main(int, char **) {
    if (video_init() != 0) {
        return 1;
    }

    initialize();
    run();
    video_cleanup();
    return 0;
}

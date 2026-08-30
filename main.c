/*
 * TinyWork Framework - Default main.c
 * Standard entry point for 320x200 demos
 *
 * Override by setting MAIN_C in your Makefile if custom logic needed
 */

#include "runtime/runtime.h"
#include "video/video.h"
#if defined(PCM_PLAYBACK) && !defined(__DJGPP__)
#include <stdio.h>
#include "audio/pcm.h"
#endif

int main(int, char **) {
    if (video_init() != 0) {
        return 1;
    }

#if defined(PCM_PLAYBACK) && !defined(__DJGPP__)
    if (pcm_init() != 0) {
        fprintf(stderr, "Audio device unavailable, running silently\n");
    }
#endif

    initialize();
    run();

#if defined(PCM_PLAYBACK) && !defined(__DJGPP__)
    pcm_cleanup();
#endif
    video_cleanup();
    return 0;
}

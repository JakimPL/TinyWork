#include "runtime.h"

#ifdef __DJGPP__
#include <conio.h>
#include <time.h>
#else
#include <SDL2/SDL.h>
#include "../video/video.h"
#endif

#ifndef __DJGPP__
static void render_frame(void) {
    frame();
    video_update_from_buffer(image);
    video_present();
}

static void redraw_current_frame(void) {
    video_handle_resize();
    render_frame();
}

static int handle_escape_key(void) {
    if (video_is_fullscreen()) {
        video_toggle_fullscreen();
        return 1;
    }
    return 0;
}

static void handle_fullscreen_toggle_key(SDL_Keycode key) {
    if (key == SDLK_F11 || key == SDLK_f) {
        video_toggle_fullscreen();
    }
}

static void handle_window_event(SDL_WindowEvent *window_event) {
    if (window_event->event == SDL_WINDOWEVENT_SIZE_CHANGED || window_event->event == SDL_WINDOWEVENT_EXPOSED) {
        redraw_current_frame();
    }
}

static int process_event(SDL_Event *event) {
    if (event->type == SDL_QUIT) {
        return 0;
    }
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_ESCAPE) {
            return handle_escape_key();
        }
        handle_fullscreen_toggle_key(event->key.keysym.sym);
    }
    if (event->type == SDL_WINDOWEVENT) {
        handle_window_event(&event->window);
    }
    return 1;
}
#endif

void run(void) {
#ifdef __DJGPP__
    uclock_t frame_start, frame_time;
    const uclock_t frame_delay = UCLOCKS_PER_SEC / 60;

    while (!kbhit()) {
        frame_start = uclock();
        frame();
        frame_time = uclock() - frame_start;
        if (frame_time < frame_delay) {
            uclock_t delay_end = uclock() + (frame_delay - frame_time);
            while (uclock() < delay_end && !kbhit()) {
                /* busy wait */
            }
        }
    }
    getch();
#else
    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            running = process_event(&event);
        }

        render_frame();
        SDL_Delay(16);
    }
#endif
}
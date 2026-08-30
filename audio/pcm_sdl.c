#include <SDL2/SDL.h>
#include "pcm.h"

#define PCM_LATENCY_FRAMES 3
#define PCM_QUEUE_LIMIT (PCM_LATENCY_FRAMES * PCM_SAMPLES_PER_FRAME)

static SDL_AudioDeviceID device = 0;

int pcm_init(void) {
    SDL_AudioSpec desired;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return -1;
    }

    SDL_zero(desired);
    desired.freq = PCM_SAMPLE_RATE;
    desired.format = AUDIO_U8;
    desired.channels = 1;
    desired.samples = PCM_SAMPLES_PER_FRAME;
    desired.callback = NULL;

    device = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
    if (device == 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return -1;
    }

    SDL_PauseAudioDevice(device, 0);
    return 0;
}

void pcm_submit(unsigned char *buffer) {
    if (device == 0) {
        return;
    }

    SDL_QueueAudio(device, buffer, PCM_SAMPLES_PER_FRAME);
}

void pcm_sync(void) {
    if (device == 0) {
        SDL_Delay(1000 / PCM_FRAME_RATE);
        return;
    }

    while (SDL_GetQueuedAudioSize(device) > PCM_QUEUE_LIMIT) {
        SDL_Delay(1);
    }
}

void pcm_cleanup(void) {
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

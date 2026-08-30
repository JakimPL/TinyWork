    %ifndef TINYWORK_PCM
    %define TINYWORK_PCM

    %ifndef COM
    %ifdef PCM_PLAYBACK
    global pcm_buffer

    section .bss
pcm_buffer:
    resb PCM_SAMPLES_PER_FRAME
    %endif
    %endif

    %endif

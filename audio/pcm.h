#ifndef PCM_H
#define PCM_H

#define PCM_FRAME_RATE 60
#define PCM_SAMPLES_PER_FRAME 160
#define PCM_SAMPLE_RATE (PCM_SAMPLES_PER_FRAME * PCM_FRAME_RATE)

/* Assembly must provide */
extern unsigned char pcm_buffer[PCM_SAMPLES_PER_FRAME];

int pcm_init(void);
void pcm_submit(unsigned char *buffer);
void pcm_sync(void);
void pcm_cleanup(void);

#endif

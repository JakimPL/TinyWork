#ifndef VIDEO_H
#define VIDEO_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#define SDL_SCALER 4
#define BUFFER_SIZE 65536

extern void set_palette(void);
extern void draw();
extern unsigned char image[BUFFER_SIZE];

int video_init(void);
void video_update_from_buffer(unsigned char *buffer);
void video_present(void);
void video_handle_resize(void);
void video_toggle_fullscreen(void);
int video_is_fullscreen(void);
void video_cleanup(void);

#endif

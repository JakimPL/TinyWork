/*
 * Minimal example using TinyWork framework
 * Demonstrates the basic structure for a 320x200 demo
 */

#include "runtime/runtime.h"
#include "runtime/render.h"
#include "video/video.h"

/*
 * frame() - Called every frame by the runtime loop
 * This is where your rendering logic goes
 */
void frame() {
    // Call your drawing function (implemented in assembly or C)
    draw();

    // Update video buffer from image array
    video_update_from_buffer(image);

    // Present the frame to screen
    video_present();
}

/*
 * main() - Entry point
 * Standard pattern: init video, run event loop, cleanup
 */
int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    // Initialize video subsystem (SDL2 or DOS VGA)
    if (video_init() != 0) {
        return 1;
    }

    // Run the main loop with our frame callback
    // Handles input, timing, and platform differences
    run(frame);

    // Clean up resources
    video_cleanup();

    return 0;
}

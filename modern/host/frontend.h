// Host build: the frontend's interface (window, input, sound output). The game publishes each frame into an AcwwShm
// block and reads the input back from it; the backend shows the frame and fills in the input:
// - frontend_sdl.c (HOST_FRONTEND_SDL, the 64-bit builds): SDL2 in the game's process, the block plain memory;
// - frontend_viewer.c (otherwise: the i386 build, which can't load the system's 64-bit SDL): the block is shared
//   memory and the 64-bit viewer program (modern/host/viewer) shows it;
// - frontend_none.c (HOST_FRONTEND_NONE: a 64-bit build without SDL2, e.g. aarch64): none, always headless.
// - frontend_3ds.c (HOST_FRONTEND_3DS): the Nintendo 3DS's screens, buttons and sound.
// Plain fixed-size types: the viewer uses this header too.
#ifndef HOST_FRONTEND_H
#define HOST_FRONTEND_H

#include <stdint.h>

#define ACWW_SHM_MAGIC 0x41435757u  // "ACWW"
#define ACWW_SHM_NAME_DEFAULT "/acww-host"
#define ACWW_AUDIO_RATE 32728     // the DS mixer: one stereo sample every 1024 cycles of 33.51 MHz
#define ACWW_AUDIO_FRAMES 16384   // power of two

typedef struct {
    uint32_t magic;
    uint32_t frame;        // bumped by the game after each published frame
    uint32_t keys;         // pressed: bit 0 A, 1 B, 2 Select, 3 Start, 4 Right, 5 Left, 6 Up, 7 Down, 8 R, 9 L,
                           // 10 X, 11 Y
    int32_t touch_x, touch_y;
    uint32_t touch_down;
    uint32_t quit;         // set by the frontend when its window closes
    int16_t stick_x, stick_y;  // the 3DS's circle pad (-156..156, y up; 0 0 elsewhere)
    uint32_t pad[8];
    uint32_t pixels[256 * 384];  // top screen then bottom screen, 0x00RRGGBB
    // sound: a ring of stereo frames (left, right) at ACWW_AUDIO_RATE; the game advances audio_write, the frontend
    // audio_read (both count frames, the ring index is the count mod ACWW_AUDIO_FRAMES)
    uint32_t audio_write;
    uint32_t audio_read;
    int16_t audio[ACWW_AUDIO_FRAMES * 2];
} AcwwShm;

// the backend (frontend.c calls it; not used with ACWW_HEADLESS)
AcwwShm *frontend_backend_open(const char *argv0);  // the block, or 0: no window (the run goes on headless)
void frontend_backend_frame(AcwwShm *shm);           // after each published frame: show it, take the input
void frontend_backend_close(void);                   // at exit (also from the crash and SIGINT handlers)

#endif

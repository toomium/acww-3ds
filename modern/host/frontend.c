// Host build: the game's side of the frontend (frontend.h). At each VBlank the frame goes to the AcwwShm block and the
// input comes from it, through the backend: frontend_sdl.c (in-process SDL2, the 64-bit builds) or frontend_viewer.c
// (the 64-bit viewer process, the i386 build). With ACWW_HEADLESS there is no backend at all: no window, no sound
// device, input only from ACWW_KEYS / ACWW_TOUCH.

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "frontend.h"
#include "host.h"
#include "host_mem.h"
#include "options_3ds.h"

static AcwwShm *sShm;

#include "render.h"
#include "wgpu_display.h"

void frontend_init(const char *argv0) {
    if (getenv("ACWW_HEADLESS")) return;
    sShm = frontend_backend_open(argv0);
}

AcwwShm *frontend_shm(void) { return sShm; }

// the circle pad for the game (include/host_input.h): its angle and tilt while it holds a direction down
volatile short gHostStickAngle;
volatile unsigned short gHostStickTilt;
static void stick(const AcwwShm *shm, u32 keys) {
    static int on = -1;
    if (on < 0) on = !getenv("ACWW_ANALOG") || atoi(getenv("ACWW_ANALOG"));
    float x = shm->stick_x, y = shm->stick_y, r = sqrtf(x * x + y * y);
    if (!on || !(keys & 0xf0) || r < 20) { gHostStickTilt = 0; return; }
    gHostStickAngle = (short)lrintf(atan2f(x, -y) * (32768.0f / 3.14159265f));  // (0 down, 0x4000 right)
    gHostStickTilt = (unsigned short)(r >= 150 ? 0x1000 : r * 0x1000 / 150);
}

// scripted keys for headless runs: ACWW_KEYS="frame:frames:mask;..." (mask: the bits of AcwwShm.keys, hex)
// (ACWW_KEYS read once: an input script is several KB, and reading it again every frame took much of a run's time)
static u32 scripted_keys(void) {
    static struct { unsigned frame, frames, mask; } *keys;
    static int n = -1;
    if (n < 0) {
        const char *spec = getenv("ACWW_KEYS") ? getenv("ACWW_KEYS") : "";
        int cap = 1;
        for (const char *p = spec; *p; p++) cap += *p == ';';
        keys = malloc(cap * sizeof *keys);
        n = 0;
        for (const char *p = spec; *p && keys;) {
            unsigned fr, len, m;
            if (sscanf(p, "%u:%u:%x", &fr, &len, &m) == 3) keys[n].frame = fr, keys[n].frames = len, keys[n++].mask = m;
            p = strchr(p, ';');
            if (!p) break;
            p++;
        }
    }
    u32 f = (u32)host_frame(), k = 0;
    for (int i = 0; i < n; i++)
        if (f >= keys[i].frame && f < keys[i].frame + keys[i].frames) k |= keys[i].mask;
    return k;
}

// at each VBlank: publish the frame, take the input
void frontend_frame(void) {
#ifdef N3DS_DIRECT
    opt_frame();
#endif
    u16 keyinput = 0x3ff;  // KEYINPUT: released bits are 1
    // the ARM7's button word (HW_BUTTON_XY_BUF, the system work area), OR-ed into KEYINPUT by PAD_Read: X bit 10, Y bit 11, debug bit 13
    // (released = 1); bit 15 clear = not folded
    u16 xy = 0x2c00;
    if (sShm) {
        // (not when the GPU shows them itself: they would be read back from it)
        if (!render_presenting())
            for (int s = 0; s < 2; s++) memcpy(sShm->pixels + s * 256 * 192, render_screen(s), 256 * 192 * 4);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        sShm->frame++;
        {
            HOST_PROF(PROF_FRONT);
            frontend_backend_frame(sShm);
        }
        if (sShm->quit) host_quit();
        u32 k = sShm->keys;
        keyinput = (u16)(~k & 0x3ff);
        stick(sShm, k);
        xy = (u16)(0x2000 | (~k & 0x0c00));
    }
    u32 sk = scripted_keys();
    keyinput &= (u16)~(sk & 0x3ff);
    xy &= (u16)~(sk & 0x0c00);
#ifdef N3DS_DIRECT
    {
        u32 p = (~keyinput & 0x3ff) | (~xy & 0x0c00), hidden = p & ~opt_keys(p);
        keyinput |= (u16)(hidden & 0x3ff);
        xy |= (u16)(hidden & 0x0c00);
    }
#endif
    *(u16 *)host_io(0x04000130) = keyinput;
    *(volatile u16 *)&gHostSysWork.buttonXY = xy;
}

// for the touch panel service (pxi7.c): the mouse or a finger on the bottom screen
int frontend_touch(int *x, int *y) {
    if (!sShm || !sShm->touch_down) return 0;
    *x = sShm->touch_x;
    *y = sShm->touch_y;
    return 1;
}

void frontend_shutdown(void) {
    if (sShm) frontend_backend_close();
}

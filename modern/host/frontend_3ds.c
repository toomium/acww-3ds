// Host build: the Nintendo 3DS frontend backend (frontend.h; HOST_FRONTEND_3DS): both DS
// screens 1:1 in the middle of the 3DS's screens (the CPU renderers' pictures), the buttons (the 3DS's are the DS's,
// the circle pad as the D-pad), the touch screen, the sound through the DSP (ndsp; it needs the DSP firmware,
// sdmc:/3ds/dspfirm.cdc, which the user dumps), and the home menu's close.
#if HOST_FRONTEND_3DS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "frontend.h"
#include "pica_display.h"

int render_presenting(void);  // (render.h: not includable beside libctru, whose u32 differs from host.h's)
void pxi7_save_flush(void);   // (pxi7.c)

static AcwwShm sShm;

// ---- sound: wave buffers in the DSP's memory, refilled at each frame from the AcwwShm ring as it plays them
enum { WAVE_BUFS = 4, WAVE_FRAMES = 1024 };
static ndspWaveBuf sWave[WAVE_BUFS];
static int16_t *sWaveData;

static void open_audio(void) {
    if (getenv("ACWW_MUTE")) return;
    if (R_FAILED(ndspInit())) {
        fprintf(stderr, "frontend: no sound: the DSP firmware (sdmc:/3ds/dspfirm.cdc) is missing\n");
        return;
    }
    sWaveData = linearAlloc(WAVE_BUFS * WAVE_FRAMES * 4);
    if (!sWaveData) { ndspExit(); return; }
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspSetClippingMode(NDSP_CLIP_NORMAL);  // (libctru's default, soft, squeezes the DS mixer's loud peaks: distortion)
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, ACWW_AUDIO_RATE);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
    // ACWW_VOLUME=<percent> (default 100): the speakers may distort loud deep notes that headphones play cleanly
    float mix[12] = {0}, vol = getenv("ACWW_VOLUME") ? atoi(getenv("ACWW_VOLUME")) / 100.0f : 1.0f;
    mix[0] = mix[1] = vol;
    ndspChnSetMix(0, mix);
    for (int i = 0; i < WAVE_BUFS; i++) {
        sWave[i].data_pcm16 = sWaveData + i * WAVE_FRAMES * 2;
        sWave[i].status = NDSP_WBUF_DONE;  // (free to be filled)
    }
}

// a played buffer is filled when a whole buffer's sound is waiting: the ring holds the rest meanwhile. The level (the
// DSP's queue and the ring) is held near AUDIO_TARGET by playing up to 0.5 % faster or slower; when the DSP's queue
// ran dry, nothing is queued until three buffers are waiting (else every buffer arrives just in time: crackles).
// (As frontend_sdl.c's audio_callback.)
#define AUDIO_TARGET (2.5 * WAVE_FRAMES)
static unsigned sDry;  // (ACWW_FPS: how often the DSP's queue ran dry, every 120 frames: frontend_backend_frame)
static void feed_audio(AcwwShm *shm) {
    if (!sWaveData) return;
    static int primed;
    uint32_t r = shm->audio_read, w = __atomic_load_n(&shm->audio_write, __ATOMIC_ACQUIRE);
    if (w - r > ACWW_AUDIO_FRAMES / 2) r = w - 3 * WAVE_FRAMES;  // far behind (a stall): skip ahead
    int queued = 0;
    for (int i = 0; i < WAVE_BUFS; i++) queued += sWave[i].status == NDSP_WBUF_QUEUED || sWave[i].status == NDSP_WBUF_PLAYING;
    if (!queued && primed) sDry++;
    if (!queued) primed = 0;
    if (!primed && w - r < 3 * WAVE_FRAMES) goto done;
    primed = 1;
    double err = ((double)(w - r) + queued * WAVE_FRAMES - (queued ? ndspChnGetSamplePos(0) : 0) - AUDIO_TARGET) / AUDIO_TARGET;
    ndspChnSetRate(0, ACWW_AUDIO_RATE * (1.0 + 0.005 * (err < -1 ? -1 : err > 1 ? 1 : err)));
    for (int i = 0; i < WAVE_BUFS && w - r >= WAVE_FRAMES; i++) {
        ndspWaveBuf *b = &sWave[i];
        if (b->status != NDSP_WBUF_DONE && b->status != NDSP_WBUF_FREE) continue;
        for (int k = 0; k < WAVE_FRAMES; k++, r++) {
            b->data_pcm16[k * 2] = shm->audio[(r % ACWW_AUDIO_FRAMES) * 2];
            b->data_pcm16[k * 2 + 1] = shm->audio[(r % ACWW_AUDIO_FRAMES) * 2 + 1];
        }
        b->nsamples = WAVE_FRAMES;
        DSP_FlushDataCache(b->data_pcm16, WAVE_FRAMES * 4);
        ndspChnWaveBufAdd(0, b);
    }
done:
    __atomic_store_n(&shm->audio_read, r, __ATOMIC_RELEASE);
}

#ifdef N3DS_DIRECT
// The enhanced port: the sound's thread (snd7/driver.c) feeds the DSP itself as it mixes,
// so that the sound goes on while the game's thread is held up; the frames waiting to be played (the ring and the
// DSP's queue). Without the DSP (no firmware, ACWW_MUTE) the ring is played away in real time.
int frontend_audio_pump(void) {
    AcwwShm *shm = &sShm;
    if (!sWaveData) {
        static u64 last;
        u64 now = svcGetSystemTick();
        u32 w = __atomic_load_n(&shm->audio_write, __ATOMIC_ACQUIRE), r = shm->audio_read;
        u64 played = last ? (now - last) * ACWW_AUDIO_RATE / SYSCLOCK_ARM11 : 0;
        if (played) last = now;
        else if (!last) last = now;
        r = played >= w - r ? w : r + (u32)played;
        __atomic_store_n(&shm->audio_read, r, __ATOMIC_RELEASE);
        return (int)(w - r);
    }
    feed_audio(shm);
    int queued = 0;
    for (int i = 0; i < WAVE_BUFS; i++) queued += sWave[i].status == NDSP_WBUF_QUEUED || sWave[i].status == NDSP_WBUF_PLAYING;
    u32 w = __atomic_load_n(&shm->audio_write, __ATOMIC_ACQUIRE);
    return (int)(w - shm->audio_read) + queued * WAVE_FRAMES - (queued ? (int)ndspChnGetSamplePos(0) : 0);
}
#endif

// ---- picture: the framebuffers are BGR8 and turned by 90 degrees (column-major, 240 pixels a column, bottom up)
static void show(gfxScreen_t screen, const uint32_t *px) {
    u16 h, w;  // (the framebuffer's own width and height: 240 and the screen's width)
    u8 *fb = gfxGetFramebuffer(screen, GFX_LEFT, &h, &w);
    int x0 = (w - 256) / 2, y0 = (240 - 192) / 2;
    for (int y = 0; y < 192; y++) {
        for (int x = 0; x < 256; x++) {
            uint32_t c = px[y * 256 + x];
            u8 *d = fb + ((x0 + x) * 240 + (239 - (y0 + y))) * 3;
            d[0] = (u8)c, d[1] = (u8)(c >> 8), d[2] = (u8)(c >> 16);
        }
    }
}

// the home menu and sleep: the GPU thread's work done first (pica_display.c). (libctru calls the hooks in the
// thread of aptMainLoop, the game's; this one is set after citro3d's, render_init, so it runs before it)
static aptHookCookie sAptHook;
static void apt_hook(APT_HookType hook, void *param) {
    (void)param;
    if (hook == APTHOOK_ONSUSPEND || hook == APTHOOK_ONSLEEP || hook == APTHOOK_ONEXIT) {
        pdisp_idle();
        pxi7_save_flush();  // (the console may be switched off from there)
    }
}

AcwwShm *frontend_backend_open(const char *argv0) {
    (void)argv0;
    gfxInitDefault();
    aptHook(&sAptHook, apt_hook, 0);
    pdisp_attach_screens();  // (when the GPU composites, it shows the screens)
    sShm.magic = ACWW_SHM_MAGIC;
    open_audio();
    return &sShm;
}

void frontend_backend_frame(AcwwShm *shm) {
    if (!aptMainLoop()) shm->quit = 1;  // (the home menu's close; it also handles the home button and sleep)
    hidScanInput();
    u32 k = hidKeysHeld();
    // frontend.h's bits are the 3DS's (and the DS's) first 12: A B Select Start Right Left Up Down R L X Y
    uint32_t keys = k & 0xfff;
    if (k & KEY_CPAD_RIGHT) keys |= KEY_DRIGHT;
    if (k & KEY_CPAD_LEFT) keys |= KEY_DLEFT;
    if (k & KEY_CPAD_UP) keys |= KEY_DUP;
    if (k & KEY_CPAD_DOWN) keys |= KEY_DDOWN;
    shm->keys = keys;
    circlePosition cp;
    hidCircleRead(&cp);
    shm->stick_x = cp.dx, shm->stick_y = cp.dy;
    // the bottom screen's 320x240 to the DS's 256x192: all of it when the GPU shows it scaled, else its middle (a
    // touch outside it is none)
    touchPosition t;
    hidTouchRead(&t);
    int scaled = render_presenting() && pdisp_scaled();
    int tx = scaled ? t.px * 4 / 5 : t.px - 32, ty = scaled ? t.py * 4 / 5 : t.py - 24;
    if (!scaled) pdisp_touch_map(&tx, &ty);  // (the UI moved to the wide screen's edges: the direct build)
    shm->touch_down = (k & KEY_TOUCH) && tx >= 0 && tx < 256 && ty >= 0 && ty < 192;
    if (shm->touch_down) shm->touch_x = tx, shm->touch_y = ty;
    if (!render_presenting()) {  // (else the GPU has shown them: pica_display.c)
        show(GFX_TOP, shm->pixels);
        show(GFX_BOTTOM, shm->pixels + 256 * 192);
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
#ifndef N3DS_DIRECT  // (the enhanced port: the sound's thread feeds the DSP, frontend_audio_pump)
    feed_audio(shm);
#endif
    static unsigned frames;
    if (sWaveData && ++frames % 120 == 0 && getenv("ACWW_FPS")) {  // (and the frames played a second: the DS's 32728)
        static u32 lastRead;
        static u64 lastTick;
        u32 r = __atomic_load_n(&shm->audio_read, __ATOMIC_ACQUIRE);
        u64 now = svcGetSystemTick();
        fprintf(stderr, "sound: the DSP's queue ran dry %u times; %.0f frames a second played\n", sDry,
                lastTick ? (double)(r - lastRead) * SYSCLOCK_ARM11 / (double)(now - lastTick) : 0.0);
        lastRead = r, lastTick = now;
    }
}

void frontend_backend_close(void) {
    pdisp_idle();
    if (sWaveData) { ndspExit(); linearFree(sWaveData); sWaveData = 0; }
    pdisp_quit();
    gfxExit();
}

#endif

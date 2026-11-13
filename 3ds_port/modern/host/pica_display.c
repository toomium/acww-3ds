// Host build: the screens composited and shown by the 3DS's GPU (pica_display.h). The 2D
// renderer gives each line's state and the layers it draws itself (gpu2d.h Gpu2dLayers); the GPU draws the others
// from VRAM and puts everything together per run of lines with the same state, as gpu2d.c compose_line does:
//   - the BGs the GPU draws (Gpu2dLineState.gpuBg: text BGs in the standard palette, affine BGs) are textures of the
//     whole BG, whose 8x8 tiles are the GPU's 8x8 texture tiles: a tile is decoded again only when its map entry,
//     its tile data or the palette changed; drawn scrolled (text) or through the affine transformation;
//   - the other BGs and the OBJs come as pixels (the windows applied), uploaded each frame; the OBJs in one texture,
//     their priority in the alpha (an alpha test per priority);
//   - the backdrop, then the layers from the back (priority 3) to the front, the OBJs of a priority in front of its
//     BGs; the stencil buffer says per pixel whether the top layer so far is a 2nd target of the colour effects (bit
//     0) or a 1st target (bit 1), whether the windows disable the effects there (bit 2), which window the pixel is in
//     (bits 3-4, from Gpu2dLayers.region: each layer drawn where its window shows it), and marks a blended pixel
//     while a layer is drawn (bit 5);
//   - alpha blending (BLDCNT effect 1: a 1st target over a 2nd one, eva and evb as the constant blend factors), the
//     3D layer over a 2nd target by its own alpha, then brightness up or down (effect 2, 3) over the 1st targets on
//     top, and master brightness.
// Then each engine's picture is drawn on its screen. Differences from the DS: the colours in the GPU's 8 bits and
// rounding; a 2nd target that was itself blended counts with its blended colour (the DS takes the layer's own);
// semi-transparent and bitmap OBJs (the game uses neither) are drawn opaque; a BG's VRAM and palette as at the
// frame's end (an H-blank change of them in the frame shows in the whole frame).

#include "pica_display.h"
#include "prof.h"

#ifndef __3DS__

int pdisp_init(void) { return 0; }
void pdisp_attach_screens(void) {}
int pdisp_scaled(void) { return 0; }
struct Gpu2dLayers *pdisp_layers(void) { return 0; }
void pdisp_frame(void *texture3d, const uint32_t *picture3d) { (void)texture3d, (void)picture3d; }
const uint32_t *pdisp_screen(int screen) { (void)screen; return 0; }
void pdisp_submit(int kind, void (*job)(void *), void *arg) { (void)kind, job(arg); }
int pdisp_threaded(void) { return 1; }
void pdisp_frame_open(int kind) { (void)kind; }
void pdisp_frame_run(void) {}
void pdisp_wait(int kind, int older) { (void)kind, (void)older; }
void pdisp_idle(void) {}
void pdisp_gsp_init(void) {}
void pdisp_quit(void) {}
void pdisp_touch_map(int *x, int *y) { (void)x, (void)y; }

#else

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>
#include <citro3d.h>

#include "gpu2d/gpu2d.h"
#include "gpu3d/gpu3d.h"

unsigned long long host_frame(void);  // (host.h, whose u32 isn't libctru's)
void host_plat_log_write(void);       // (plat.h: the port's log written out now)

extern const u8 pica_display_shbin[];  // (the vertex shader, pica_display.v.pica: picasso and bin2s)
extern const u32 pica_display_shbin_size;

enum { W = 256, H = 192, TEX = 256, UP = 512, MAX_VERTS = 0x8000 };

typedef struct {
    float pos[3];  // pixels
    float col[4];
    float tc[2];
} DVertex;

static DVLB_s *sDvlb;
static shaderProgram_s sProgram;
static int sUProjection;
static DVertex *sV;
static int sNV;

static Gpu2dLayers sLayers;            // its pixel buffers in linear memory (the GPU's transfers read them)
static C3D_Tex sBg[2][4], sObj[2], sRegion[2], s3d;
#ifdef N3DS_PORT
static C3D_Tex sOpt;                   // the options page's picture (gpu2d_options), and its linear copy for the upload
static uint32_t *sOptStage, sOptSerial;
#endif
static C3D_Tex sSprites;               // the OBJs as sprites (Gpu2dLayers.sprite: the port), their atlas
// The port: each engine's backdrop colours, a texel row per line (8 x 256, the lines after the last as the
// last), so that the backdrop is a quad per group and the sky's colour changing every line (its H-blank) doesn't cut
// the layers into lines: thin quads are slow on the GPU (each 8x8 tile of the target is read and written per line)
#ifdef N3DS_PORT
static C3D_Tex sBackdrop[2];
enum { kLineBackdrop = 1 };
#else
enum { kLineBackdrop = 0 };
#endif
static u32 *s3dStage;                  // the 3D picture as RGBA8, row by row
static C3D_Tex *s3dTex;                // this frame's 3D picture: the GPU's (premultiplied) or s3d
static int s3dPremultiplied;
static int s3dWide;                    // this frame's 3D picture: the wide view's (gpu3d.h)
static C3D_Tex sComp[2];               // each engine's picture
static C3D_RenderTarget *sCompTarget[2], *sScreen[2];
static int sScreenStencil;             // the screens' targets have a stencil buffer (the wide view's)
static C3D_Tex sUp;                    // a picture doubled (the screens' sharp scaling; RGB8, VRAM)
static C3D_RenderTarget *sUpTarget;
static u32 *sReadback;
static uint32_t sScreenPx[2][W * H];
static int sScreenRead[2];

static const u32 kLinearToTiled = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |
                                  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
static const u32 kTiledToLinear = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
                                  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO) |
                                  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                                  GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8);

// ---- the GPU thread: the 3D pictures (gpu3d_pica.c) and the compositing on the New 3DS's core 2,
// while the game's thread goes on with the next frame. The jobs in turn, from a short queue: the game's thread waits
// for the last one of a kind before it changes what that reads (render.c: the 3D engine's list and textures for the
// 3D picture, the 2D renderer's layers for the compositing; here and gpu3d_pica.c: the jobs' own inputs), not for
// the others (the game's frames are uneven: it works every third one, the 3D engine swaps then). The 2D layers are
// drawn here too, first: the 2D renderer records each line's state and copies VRAM's 2D banks, the palettes and
// OAM, which the game changes when it likes (Gpu2dFrameInput.deferLayers); the compositing reads those copies.
// citro3d is the GPU thread's then: the game's thread uses it only when that is idle.
// Without core 2 (an Old 3DS; ACWW_PICA_THREAD=0) the jobs are done at once. For the check that a job sees what it
// would at once: ACWW_PICA_DELAY=<ms>, each job waits that long first; ACWW_PICA_HASH=1, a hash of each composited
// picture in the log.
volatile int *gPicaProf = &gHostProf;
enum { QUEUE = 4 };
static Thread sWorker;
static LightLock sLock;  // (over the queue and the counts)
static CondVar sChanged;
static struct { void (*fn)(void *); void *arg; } sQueue[QUEUE];
static u32 sSubmitted, sDone, sLastOf[2][2];  // jobs submitted, done; the numbers of the last two of each kind
static int sDelayMs;

static void worker(void *arg) {
    (void)arg;
    LightLock_Lock(&sLock);
    for (;;) {
        while (sDone == sSubmitted) CondVar_Wait(&sChanged, &sLock);
        void (*fn)(void *) = sQueue[sDone % QUEUE].fn;
        void *a = sQueue[sDone % QUEUE].arg;
        LightLock_Unlock(&sLock);
        if (sDelayMs) svcSleepThread((s64)sDelayMs * 1000000);
        fn(a);
        LightLock_Lock(&sLock);
        sDone++;
        CondVar_WakeUp(&sChanged, ARBITRATION_SIGNAL_ALL);
    }
}
static void start_worker(void) {
    static int tried;
    if (tried++) return;
    const char *e = getenv("ACWW_PICA_THREAD");
    if (e && !atoi(e)) return;
    sDelayMs = getenv("ACWW_PICA_DELAY") ? atoi(getenv("ACWW_PICA_DELAY")) : 0;
    LightLock_Init(&sLock);
    CondVar_Init(&sChanged);
    s32 prio = 0x30;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    sWorker = threadCreate(worker, 0, 64 * 1024, prio, 2, true);
    if (sWorker) gPicaProf = &gHostProfWorker;
    fprintf(stderr, "3ds: the GPU work %s\n", sWorker ? "on core 2" : "on the game's core (no core 2)");
}
static void wait_done(int last, int kind, int older) {
    if (!sWorker) return;
    LightLock_Lock(&sLock);
    u32 n = last ? sSubmitted : sLastOf[kind][older];
    if (sDone < n) {
        HOST_PROF(PROF_WAIT);
        while (sDone < n) CondVar_Wait(&sChanged, &sLock);
    }
    LightLock_Unlock(&sLock);
}
void pdisp_wait(int kind, int older) { wait_done(0, kind, older); }
static void frame_close(void);
// (citro3d is the game's thread's after this: a frame a 3D job left open ended)
void pdisp_idle(void) {
    wait_done(1, 0, 0);
    frame_close();
}
int pdisp_threaded(void) {
    start_worker();
    return sWorker != 0;
}
void pdisp_submit(int kind, void (*job)(void *), void *arg) {
    start_worker();
    if (!sWorker) { job(arg); return; }
    LightLock_Lock(&sLock);
    while (sSubmitted - sDone >= QUEUE) CondVar_Wait(&sChanged, &sLock);
    sQueue[sSubmitted % QUEUE].fn = job, sQueue[sSubmitted % QUEUE].arg = arg;
    sLastOf[kind][1] = sLastOf[kind][0];
    sLastOf[kind][0] = ++sSubmitted;
    CondVar_WakeUp(&sChanged, ARBITRATION_SIGNAL_ALL);
    LightLock_Unlock(&sLock);
}

// ACWW_PICA_PROBE=1 (with ACWW_FPS, for the GPU's time on hardware): every 300 composited frames the next of these
// left out, each report saying which (the pictures are wrong meanwhile): 0 nothing, 1 the sprites' upload, 2 the
// blended passes (the 3D layer), 3 the backdrop, 4 the BGs and OBJs, 5 every draw, 6 every draw, the screens'
// clears and the sprites' upload
enum { PROBES = 7 };
static int sProbe = -1, sProbeMode;
static u32 sFrame;
static int probe_skips(int what) {
    static const u8 kSkips[PROBES] = {0, 1, 2, 4, 8, 16 | 2 | 4 | 8, 32 | 16 | 2 | 4 | 8 | 1};
    return sProbe > 0 && (kSkips[sProbeMode] & what);
}

// ACWW_FPS: the GPU's drawing time (citro3d's, from a frame's end to its work's end) of the 3D pictures, of the
// compositing and of frames with both (pdisp_frame_run: then from the 3D's commands' start, but C3D_GetDrawingTime
// counts from the frame's end: the time the GPU drew while the compositing was made is left out), every 300 frames
// composited; C3D_GetDrawingTime is the frame's before C3D_FrameBegin's
static int sFrameKinds;  // (the kinds of job in the citro3d frame begun last: bit 0 the 3D, bit 1 the compositing)
static void frame_time(int begun) {
    static int on = -1;
    static float ms[3];
    static u32 composited;
    if (on < 0) on = getenv("ACWW_FPS") != 0;
    if (!on) return;
    if (begun) {  // (the frame before's time)
        if (sFrameKinds) ms[sFrameKinds - 1] += C3D_GetDrawingTime();
        return;
    }
    if (++composited % 300 == 0) {
        fprintf(stderr, "pica: the GPU's drawing, ms a composited frame: 3D %.1f, compositing %.1f, both %.1f",
                ms[0] / 300, ms[1] / 300, ms[2] / 300);
        if (sProbe > 0) fprintf(stderr, " (probe %d)", sProbeMode);
        fprintf(stderr, "\n");
        ms[0] = ms[1] = ms[2] = 0;
    }
}

// The citro3d frames. C3D_FrameBegin waits until the GPU has done all the work before (libcitro3d.a disassembled,
// 2026-11-02: gxCmdQueueWait(-1), then the queue stopped), and the GPU starts on a frame's commands only at its
// C3D_FrameEnd (gxCmdQueueRun): C3D_FrameSplit only queues them. So with a frame for the 3D picture and one for the
// compositing, the GPU thread and the GPU took turns: the compositing waited for the 3D's GPU work, the next 3D
// picture for the compositing's. The port: one frame for both. The 3D job begins it and has the GPU start on
// its commands at once (pdisp_frame_run), the compositing is made in the same frame meanwhile and ends it.
// ACWW_PICA_MERGE=0: a frame each, as before.
// citro3d's GX queue, the first member of its context (libcitro3d.a: C3D_FrameBegin / C3D_FrameEnd pass
// __C3D_Context + 0 to gxCmdQueueWait / gxCmdQueueRun)
extern u8 __C3D_Context[];
static int sOpen, sRunning;  // a frame begun by a job and not ended; the GPU started on it
static int sMerged;  // (frames so far with the 3D and the compositing in one: the stall's report)
static void frame_begin(void) {
    {
        HOST_PROF_ON(gPicaProf, PROF_GPU);
#ifdef N3DS_PORT
        // the GPU's work not done after a second (a stall: one frame for the 3D and the compositing stopped the
        // game on hardware, 2026-11-02): the GX queue's state to the log, written out at once (the log's thread
        // starts only at frame 600)
        gxCmdQueue_s *q = (gxCmdQueue_s *)__C3D_Context;
        if (!gxCmdQueueWait(q, SYSCLOCK_ARM11)) {  // (in system ticks, libctru.a disassembled: not ns as gx.h says)
            fprintf(stderr, "pica: the GPU's work not done after 1 s (frame %llu, one frame: %d): %u entries, %u "
                    "submitted, %u done; types:", host_frame(), sMerged, q->numEntries, q->curEntry, q->lastEntry);
            for (u32 i = 0; i < q->numEntries; i++) fprintf(stderr, " %u", (unsigned)q->entries[i].type);
            fprintf(stderr, "\n");
            host_plat_log_write();
        }
#endif
        C3D_FrameBegin(0);  // (waits for the GPU's work before)
    }
    frame_time(1);
    sOpen = 1, sRunning = 0, sFrameKinds = 0;
}
void pdisp_frame_open(int kind) {
    if (sOpen && kind == PDISP_3D) frame_close();  // (a 3D job after a 3D job: what it changes the GPU may still read)
    if (!sOpen) frame_begin();
    sFrameKinds |= 1 << kind;
    if (kind == PDISP_COMP) frame_time(0);
}
void pdisp_frame_run(void) {
    static int merge = -1;
    if (merge < 0) {
        const gxCmdQueue_s *q = (const gxCmdQueue_s *)__C3D_Context;  // (as citro3d's: entries, room for some)
        merge = (!getenv("ACWW_PICA_MERGE") || atoi(getenv("ACWW_PICA_MERGE"))) && q->entries && q->maxEntries >= 4;
#ifndef N3DS_PORT
        merge = 0;  // (the 3DS build as it was)
#endif
#ifdef N3DS_PORT
        fprintf(stderr, "pica: the 3D picture and the compositing %s\n", merge ? "in one frame" : "in a frame each");
#endif
    }
    if (!merge) {
        C3D_FrameEnd(0);
        sOpen = 0;
        return;
    }
    // the 3D's commands queued (the queue stopped since C3D_FrameBegin), flushed from the data cache here, as
    // C3D_FrameEnd(0) does before it runs the queue, then run. (GX_CMDLIST_FLUSH, the flush left to GSP, wasn't
    // enough: on hardware the GPU stopped on such a list after a few frames, the queue's report: its two memory
    // fills done, the command list submitted and never done.) GPUCMD_Split ends the list and moves gpuCmdBuf past
    // it (libctru.a disassembled). From now on what is queued starts at once: only transfers and fills, whose
    // sources are flushed where they are written; commands go in only after the queue is stopped again
    // (frame_flush_commands, frame_close)
    u32 *start = gpuCmdBuf;
    C3D_FrameSplit(0);
    GSPGPU_FlushDataCache(start, (u32)(gpuCmdBuf - start) * 4);
    gxCmdQueueRun((gxCmdQueue_s *)__C3D_Context);
    sRunning = 1;
    sMerged++;
}
// a running frame's queue stopped once the GPU has done what it holds (the 3D, the transfers so far): what is queued
// next waits for C3D_FrameEnd(0), which flushes the data cache before it runs the queue
static void frame_stop(void) {
    if (!sRunning) return;
    gxCmdQueue_s *q = (gxCmdQueue_s *)__C3D_Context;
    {
        HOST_PROF_ON(gPicaProf, PROF_GPU);
        gxCmdQueueWait(q, -1);
    }
    gxCmdQueueStop(q);  // (not running: the queue let go at once)
    sRunning = 0;
}
static void frame_close(void) {
    if (!sOpen) return;
    frame_stop();
    {
        HOST_PROF_ON(gPicaProf, PROF_FEND);
        C3D_FrameEnd(0);
    }
    sOpen = 0;
    // ACWW_PICA_STATS: the most GX queue entries a frame took, every 300 frames ended (the queue is cleared only at
    // C3D_FrameBegin, and full it breaks; one frame for the 3D and the compositing puts both's entries in it; they
    // stay counted until the next frame begins)
    static int stats = -1;
    static u32 frames, most;
    if (stats < 0) stats = getenv("ACWW_PICA_STATS") && atoi(getenv("ACWW_PICA_STATS"));
    if (stats) {
        const gxCmdQueue_s *q = (const gxCmdQueue_s *)__C3D_Context;
        if (q->numEntries > most) most = q->numEntries;
        if (++frames % 300 == 0) {
            fprintf(stderr, "pica: the GX queue: at most %lu of %u entries a frame\n", (unsigned long)most,
                    (unsigned)q->maxEntries);
            most = 0;
        }
    }
}
// (before a C3D_SyncDisplayTransfer in a running frame: it would queue the commands before it unflushed, to start at
// once: with any, the queue stopped first). The GX queue nearly full (the most seen in a frame: 14 of 32,
// ACWW_PICA_STATS): the frame ended, the compositing goes on in one of its own, as with ACWW_PICA_MERGE=0
static void frame_flush_commands(void) {
    if (!sRunning) return;
    const gxCmdQueue_s *q = (const gxCmdQueue_s *)__C3D_Context;
    if (q->numEntries + 12u > q->maxEntries) {  // (12: what may follow the last upload, with room to spare)
        frame_close();
        frame_begin();
        sFrameKinds = 1 << PDISP_COMP;
        return;
    }
    if (gpuCmdBufOffset) frame_stop();
}

// (vram: in VRAM when it fits, the layers: the GPU reads its own memory much faster than the main memory, and the
// compositing reads them over and over; else in linear memory)
static int tex(C3D_Tex *t, GPU_TEXCOLOR fmt, int vram) {
    if (!(vram && C3D_TexInitVRAM(t, TEX, TEX, fmt)) && !C3D_TexInit(t, TEX, TEX, fmt)) return 0;
    C3D_TexSetFilter(t, GPU_NEAREST, GPU_NEAREST);
    C3D_TexSetWrap(t, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    return 1;
}

int pdisp_init(void) {
    if (sV) return 1;
    pdisp_gsp_init();  // (as gpu3d_pica.c: C3D_Init there, or here first)
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 4);
    sV = linearAlloc(MAX_VERTS * sizeof(DVertex));
    sReadback = linearAlloc(TEX * TEX * 4);
    s3dStage = linearAlloc(W * H * 4);
    sDvlb = DVLB_ParseFile((u32 *)pica_display_shbin, pica_display_shbin_size);
    if (!sV || !sReadback || !s3dStage || !sDvlb || !tex(&s3d, GPU_RGBA8, 0)) return 0;
    shaderProgramInit(&sProgram);
    shaderProgramSetVsh(&sProgram, &sDvlb->DVLE[0]);
    sUProjection = shaderInstanceGetUniformLocation(sProgram.vertexShader, "projection");
    for (int e = 0; e < 2; e++) {
        int vram = !gpu3d_wide_wanted();  // (the wide view's 3D pictures take the VRAM: gpu3d_pica.c)
        for (int n = 0; n < 4; n++)
            if (!(sLayers.bg[e][n] = linearAlloc(W * H * 2)) || !tex(&sBg[e][n], GPU_RGBA5551, vram)) return 0;
        if (!(sLayers.obj[e] = linearAlloc(W * H * 4)) || !tex(&sObj[e], GPU_RGBA8, vram)) return 0;
        if (!(sLayers.region[e] = linearAlloc(W * H * 4)) || !tex(&sRegion[e], GPU_RGBA8, vram)) return 0;
        if (!C3D_TexInitVRAM(&sComp[e], TEX, TEX, GPU_RGBA8)) return 0;
        C3D_TexSetFilter(&sComp[e], GPU_NEAREST, GPU_NEAREST);
        sCompTarget[e] = C3D_RenderTargetCreateFromTex(&sComp[e], GPU_TEXFACE_2D, 0, GPU_RB_DEPTH24_STENCIL8);
        if (!sCompTarget[e]) return 0;
    }
#ifdef N3DS_PORT  // (the OBJs as sprites)
    if (!(sLayers.sprite = linearAlloc(GPU2D_ATLAS_W * GPU2D_ATLAS_H * 4))) return 0;
    if (!(!gpu3d_wide_wanted() && C3D_TexInitVRAM(&sSprites, GPU2D_ATLAS_W, GPU2D_ATLAS_H, GPU_RGBA8)) &&
        !C3D_TexInit(&sSprites, GPU2D_ATLAS_W, GPU2D_ATLAS_H, GPU_RGBA8))
        return 0;
    C3D_TexSetFilter(&sSprites, GPU_NEAREST, GPU_NEAREST);
    C3D_TexSetWrap(&sSprites, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    if (!(sOptStage = linearAlloc(W * H * 4)) || !tex(&sOpt, GPU_RGBA8, 0)) return 0;
    for (int e = 0; e < 2; e++) {
        if (!C3D_TexInit(&sBackdrop[e], 8, 256, GPU_RGBA8)) return 0;
        C3D_TexSetFilter(&sBackdrop[e], GPU_NEAREST, GPU_NEAREST);
        C3D_TexSetWrap(&sBackdrop[e], GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    }
#endif
    return 1;
}

// (the event thread GSP starts runs until the last gspExit: on a stack in the heap, which is freed before the process
// ends - a GSP event in between faults)
static int sGspRefs;
void pdisp_gsp_init(void) {
    gspInit();
    sGspRefs++;
}
void pdisp_quit(void) {
    pdisp_idle();
    if (sGspRefs) C3D_Fini();
    for (; sGspRefs; sGspRefs--) gspExit();
}

// the screens 1:1 (256x192, pixel for pixel); ACWW_3DS_SCALE=1.25: 1.25 times (320x240)
int pdisp_scaled(void) {
    static int scaled = -1;
    if (scaled < 0) scaled = getenv("ACWW_3DS_SCALE") && atof(getenv("ACWW_3DS_SCALE")) == 1.25 && !gpu3d_wide_wanted();
    return scaled;
}

void pdisp_attach_screens(void) {
    if (!sV || sScreen[0]) return;
    const u32 out = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
                    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
    if (pdisp_scaled() && C3D_TexInitVRAM(&sUp, UP, UP, GPU_RGB8)) {  // (render targets are in VRAM: 768 KB)
        C3D_TexSetFilter(&sUp, GPU_LINEAR, GPU_LINEAR);
        sUpTarget = C3D_RenderTargetCreateFromTex(&sUp, GPU_TEXFACE_2D, 0, -1);
    }
    sScreenStencil = gpu3d_wide_wanted();  // (margins2d)
    sScreen[0] = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, sScreenStencil ? GPU_RB_DEPTH24_STENCIL8 : -1);
    sScreen[1] = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, sScreenStencil ? GPU_RB_DEPTH24_STENCIL8 : -1);
    if (sScreen[0]) C3D_RenderTargetSetOutput(sScreen[0], GFX_TOP, GFX_LEFT, out);
    if (sScreen[1]) C3D_RenderTargetSetOutput(sScreen[1], GFX_BOTTOM, GFX_LEFT, out);
    fprintf(stderr, "pica: %lu KB of VRAM free%s%s\n", (unsigned long)(vramSpaceFree() / 1024),
            sScreen[0] && sScreen[1] ? "" : " (no screen targets)", pdisp_scaled() && !sUpTarget ? " (not scaled: no VRAM)" : "");
}

struct Gpu2dLayers *pdisp_layers(void) { return sV ? &sLayers : 0; }

// ---- the BGs drawn from VRAM: a texture of the whole BG each (GBATEK "DS Video BG Modes / Control")
typedef struct {
    int kind, w, h, color256;  // (kind 0: no texture yet)
    u32 frame;                 // the frame it was brought up to date in
    u32 charBase, mapBase;
    C3D_Tex tex;               // what the GPU reads: in VRAM where it fits, else in linear memory
    C3D_Tex stage;             // (tex in VRAM: the copy the CPU decodes into, which the CPU can't write)
    int staged;
    int period, opaque;        // a text BG's map repeated every 1, 2, 4 or 8 tiles both ways (0: none; the same
                               // pattern wherever it is repeated); and, at 1 (one entry everywhere), its tile without
                               // a transparent pixel
    u16 map[64 * 64];          // the map entries the texture shows (affine: 8-bit ones)
    u8 chars[0x10000];         // the tile data it was decoded from
    u8 pal[0x200];             // and the BG palette
} BgTex;
static BgTex sBgTex[2][4];
static u16 sTmpMap[64 * 64];
static u8 sMorton[8][8];       // a texel's place in its 8x8 tile: [y][x]

// len bytes of an engine's BG VRAM from addr (through its 16 KB granules)
static void vram_copy(u8 *dst, const u8 *const *gran, u32 mask, u32 addr, u32 len) {
    while (len) {
        u32 a = addr & mask, in = 0x4000 - (a & 0x3fff), n = in < len ? in : len;
        memcpy(dst, gran[a >> 14] + (a & 0x3fff), n);
        dst += n, addr += n, len -= n;
    }
}
static inline u16 to5551(u16 c) { return (u16)((c & 31) << 11 | (c >> 5 & 31) << 6 | (c >> 10 & 31) << 1 | 1); }
static inline u16 pal(const u8 *p, u32 i) { return (u16)(p[i * 2] | p[i * 2 + 1] << 8); }

// one 8x8 tile of the BG texture from map entry `entry`
static void bg_tile(BgTex *t, int tx, int ty, u16 entry) {
    u16 *dst = (u16 *)(t->staged ? t->stage.data : t->tex.data) + (ty * (t->w / 8) + tx) * 64;
    if (t->kind == 2) {  // affine: 256 colours, no flips
        const u8 *c = t->chars + (entry & 0xff) * 64u;
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++) {
                u8 i = c[y * 8 + x];
                dst[sMorton[y][x]] = i ? to5551(pal(t->pal, i)) : 0;
            }
        return;
    }
    u32 tile = entry & 0x3ff;
    int fx = entry & 0x400 ? 7 : 0, fy = entry & 0x800 ? 7 : 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            int sx = x ^ fx, sy = y ^ fy;
            u32 i;
            if (t->color256) i = t->chars[tile * 64 + (u32)(sy * 8 + sx)];
            else {
                u8 b = t->chars[tile * 32 + (u32)(sy * 4 + sx / 2)];
                i = sx & 1 ? b >> 4 : b & 15;
                if (i) i += (entry >> 12) * 16u;
            }
            dst[sMorton[y][x]] = i ? to5551(pal(t->pal, i)) : 0;
        }
}

// where BG n's tile data (cb) and map (mb) are, its size and colours
static void bg_layout(int e, int n, const Gpu2dLineState *s, int *w, int *h, int *color256, u32 *cb, u32 *mb) {
    u16 cnt = s->bgcnt[n];
    *cb = (cnt >> 2 & 15) * 0x4000u, *mb = (cnt >> 8 & 31) * 0x800u;
    if (e == 0) *cb += (s->dispcnt >> 24 & 7) * 0x10000u, *mb += (s->dispcnt >> 27 & 7) * 0x10000u;
    if (s->bgKind[n] == 1) *w = cnt & 0x4000 ? 512 : 256, *h = cnt & 0x8000 ? 512 : 256, *color256 = (cnt & 0x80) != 0;
    else *w = *h = 128 << (cnt >> 14), *color256 = 1;
}
// the highest tile a map uses (kind 2: 8-bit entries)
static u32 max_tile(const u16 *map, int count, int kind) {
    u32 m = 0;
    for (int i = 0; i < count; i++) {
        u32 tile = kind == 2 ? map[i] : map[i] & 0x3ffu;
        if (tile > m) m = tile;
    }
    return m;
}
// the map's entries into out, as the texture lays them out (32x32-tile blocks of 2 KB, left to right, then down)
static void read_map(u16 *out, const u8 *const *gran, u32 mask, u32 mb, int tw, int th, int kind) {
    if (kind == 2) {
        static u8 m8[128 * 128];
        vram_copy(m8, gran, mask, mb, (u32)(tw * th));
        for (int i = 0; i < tw * th; i++) out[i] = m8[i];
        return;
    }
    for (int by = 0; by < th / 32; by++)
        for (int bx = 0; bx < tw / 32; bx++) {
            static u16 block[32 * 32];
            vram_copy((u8 *)block, gran, mask, mb + (u32)(by * (tw / 32) + bx) * 0x800u, 0x800);
            for (int y = 0; y < 32; y++) memcpy(&out[(by * 32 + y) * tw + bx * 32], &block[y * 32], 64);
        }
}

// BG n of an engine as its texture, brought up to date with VRAM: 0 if it can't be
static u32 sBgDrawn;  // (the BG textures made again so far: screen_unchanged)
static BgTex *bg_texture(int e, int n, const Gpu2dLineState *s) {
    HOST_PROF_ON(gPicaProf, PROF_TILES);
    BgTex *t = &sBgTex[e][n];
    if (t->kind && t->frame == sFrame) return t;  // (once a frame: VRAM as at its end)
    t->frame = sFrame;
    int kind = s->bgKind[n], w, h, color256;
    u32 cb, mb;
    bg_layout(e, n, s, &w, &h, &color256, &cb, &mb);
    int full = 0;
    if (t->kind != kind || t->w != w || t->h != h) {
        if (t->kind) C3D_TexDelete(&t->tex);
        if (t->kind && t->staged) C3D_TexDelete(&t->stage);
        t->kind = 0;
        // (in VRAM where it fits, which the GPU reads faster)
        t->staged = C3D_TexInitVRAM(&t->tex, (u16)w, (u16)h, GPU_RGBA5551);
        if (t->staged && !C3D_TexInit(&t->stage, (u16)w, (u16)h, GPU_RGBA5551)) C3D_TexDelete(&t->tex), t->staged = 0;
        if (!t->staged && !C3D_TexInit(&t->tex, (u16)w, (u16)h, GPU_RGBA5551)) return 0;
        C3D_TexSetFilter(&t->tex, GPU_NEAREST, GPU_NEAREST);
        t->tex.border = 0;  // (an affine BG without wrap: transparent outside)
        t->kind = kind, t->w = w, t->h = h;
        full = 1;
    }
    if (t->color256 != color256 || t->charBase != cb || t->mapBase != mb) full = 1;
    t->color256 = color256, t->charBase = cb, t->mapBase = mb;
    const u8 *const *gran = sLayers.bgGran[e];
    u32 mask = sLayers.bgMask[e];
    if (memcmp(t->pal, sLayers.palette[e], 0x200)) full = 1, memcpy(t->pal, sLayers.palette[e], 0x200);
    // the map first: the tiles it uses (up to the highest one) are those to compare
    int tw = w / 8, th = h / 8, drawn = 0;
    u32 tileBytes = color256 ? 64 : 32;
    read_map(sTmpMap, gran, mask, mb, tw, th, kind);
    u32 maxTile = max_tile(sTmpMap, tw * th, kind);
    // the tile data, compared where it is in 1 KB pieces (in one 16 KB granule each: the character base is in 16 KB
    // steps), then the tiles of a piece that differs
    static u8 dirty[1024];
    u32 len = ((maxTile + 1) * tileBytes + 1023) & ~1023u;
    memset(dirty, full, sizeof dirty);
    for (u32 o = 0; o < len; o += 1024) {
        u32 a = (cb + o) & mask;
        const u8 *src = gran[a >> 14] + (a & 0x3fff);
        if (!full && !memcmp(src, t->chars + o, 1024)) continue;
        if (!full)
            for (u32 k = o / tileBytes; k < (o + 1024) / tileBytes && k < 1024; k++)
                dirty[k] = memcmp(src + (k * tileBytes - o), t->chars + k * tileBytes, tileBytes) != 0;
        memcpy(t->chars + o, src, 1024);
    }
    for (int ty = 0; ty < th; ty++)
        for (int tx = 0; tx < tw; tx++) {
            u16 entry = sTmpMap[ty * tw + tx];
            u32 tile = kind == 2 ? entry : entry & 0x3ffu;
            if (full || entry != t->map[ty * tw + tx] || dirty[tile]) {
                bg_tile(t, tx, ty, entry);
                drawn = 1;
            }
            t->map[ty * tw + tx] = entry;
        }
    t->period = 0;
    for (int p = 1; p <= 8 && kind == 1 && !t->period; p *= 2) {  // (tw and th are 32 or 64: p divides them)
        int i = 0;
        while (i < tw * th && t->map[i] == t->map[i / tw % p * tw + i % tw % p]) i++;
        if (i == tw * th) t->period = p;
    }
    t->opaque = 0;
    if (t->period == 1 && (t->map[0] & 0x3ffu) * tileBytes < sizeof t->chars) {
        const u8 *c = t->chars + (t->map[0] & 0x3ffu) * tileBytes;
        t->opaque = 1;
        for (u32 k = 0; k < tileBytes && t->opaque; k++) t->opaque = color256 ? c[k] != 0 : (c[k] & 0xf) && (c[k] >> 4);
    }
    sBgDrawn += (u32)drawn;
    if (drawn && t->staged) {  // (the GPU copies it to VRAM: C3D_SyncTextureCopy)
        C3D_TexFlush(&t->stage);
        if (sRunning && gpuCmdBufOffset) frame_stop();  // (it queues the commands before it unflushed: frame_stop)
        C3D_TexLoadImage(&t->tex, t->stage.data, GPU_TEXFACE_2D, 0);
    } else if (drawn) C3D_TexFlush(&t->tex);
    return t;
}

// ---- drawing
static void projection(float w, float h, int tilt) {
    C3D_Mtx m;
    if (tilt) Mtx_OrthoTilt(&m, 0.0f, w, h, 0.0f, 0.0f, 1.0f, true);  // (pixels, y down, on a turned 3DS screen)
    else Mtx_Ortho(&m, 0.0f, w, h, 0.0f, 0.0f, 1.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sUProjection, &m);
}
// the fragment colour: the texture's or the vertex colour
static void source(C3D_Tex *t) {
    C3D_TexEnv *e = C3D_GetTexEnv(0);
    C3D_TexEnvInit(e);
    C3D_TexEnvSrc(e, C3D_Both, t ? GPU_TEXTURE0 : GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(e, C3D_Both, GPU_REPLACE);
    if (t) C3D_TexBind(0, t);
}
static void opaque(void) { C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO); }
static void colour_writes(int on) { C3D_DepthTest(false, GPU_ALWAYS, on ? GPU_WRITE_COLOR : (GPU_WRITEMASK)0); }
static void stencil(GPU_TESTFUNC f, u32 ref, u32 mask, u32 write) {
    C3D_StencilTest(true, f, (int)ref, (int)mask, (int)write);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_REPLACE);
}

// ---- a pass: quads put together, then drawn at once (draws are what costs on the CPU: the GPU's state is set up for
// each one)
static int sMark;
// ACWW_PICA_STATS=1: the pixels the GPU fills and the draws, per kind of pass, every 300 composited frames
enum { ST_BACKDROP, ST_REGION, ST_OBJ, ST_LAYER, ST_BLEND, ST_ASIS, ST_MARK, ST_EFFECT, ST_UP, ST_SCREEN, ST_N };
static int sStats = -1, sTag;
static double sStatPx[ST_N], sStatDraws[ST_N], sStatQuads[ST_N], sStatObj, sStatBg;
static void begin(void) { sMark = sNV; }
static void end(void) {
    if (sNV <= sMark) return;
    int skip = probe_skips(16) || (sTag == ST_BLEND && probe_skips(2)) || (sTag == ST_BACKDROP && probe_skips(4)) ||
               ((sTag == ST_LAYER || sTag == ST_OBJ) && probe_skips(8));
    if (!skip) C3D_DrawArrays(GPU_TRIANGLES, sMark, sNV - sMark);
    if (sStats) {
        sStatDraws[sTag]++;
        sStatQuads[sTag] += (sNV - sMark) / 6;
        for (int i = sMark; i < sNV; i += 3) {  // (each triangle's area)
            const float *a = sV[i].pos, *b = sV[i + 1].pos, *c = sV[i + 2].pos;
            float t = ((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])) / 2;
            sStatPx[sTag] += t < 0 ? -t : t;
        }
    }
}
static void stats_report(void) {
    static const char *const kName[ST_N] = {"backdrop", "window regions", "OBJ passes", "BG layers", "blended", "as it is", "marks",
                                            "effects", "doubling", "screens"};
    static u32 frames;
    if (++frames % 300) return;
    fprintf(stderr, "pica: kpixels (draws, quads) a composited frame:");
    for (int i = 0; i < ST_N; i++) {
        fprintf(stderr, " %s %.0f (%.1f, %.0f)", kName[i], sStatPx[i] / 300 / 1000, sStatDraws[i] / 300, sStatQuads[i] / 300);
        sStatPx[i] = sStatDraws[i] = sStatQuads[i] = 0;
    }
    fprintf(stderr, " | OBJ texels shown %.0f, BG texels shown (CPU BGs) %.0f\n", sStatObj / 300 / 1000,
            sStatBg / 300 / 1000);
    sStatObj = sStatBg = 0;
}
// a quad: its corners (pixels: top left, top right, bottom right, bottom left) and their texture coordinates
static void quad(const float xy[4][2], const float uv[4][2], const float col[4]) {
    if (sNV + 6 > MAX_VERTS) return;
    static const float none[4] = {0, 0, 0, 0};
    if (!col) col = none;
    DVertex q[4];
    for (int i = 0; i < 4; i++) {
        q[i].pos[0] = xy[i][0], q[i].pos[1] = xy[i][1], q[i].pos[2] = 0.5f;
        memcpy(q[i].col, col, sizeof q[i].col);
        q[i].tc[0] = uv[i][0], q[i].tc[1] = uv[i][1];
    }
    DVertex *d = sV + sNV;
    d[0] = q[0], d[1] = q[1], d[2] = q[2], d[3] = q[0], d[4] = q[2], d[5] = q[3];
    sNV += 6;
}
// a rectangle [x0, x1) x [y0, y1) of a texture's pixels from (u0 * size, row ty0) on, the texture `rows` tall (the
// GPU's texture v runs from the bottom: v = 1 is the first row)
static void rect(float x0, float y0, float x1, float y1, float u0, float u1, float ty0, float rows, const float col[4]) {
    if (x1 <= x0 || y1 <= y0) return;
    float v0 = 1.0f - ty0 / rows, v1 = 1.0f - (ty0 + y1 - y0) / rows;
    const float xy[4][2] = {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}, uv[4][2] = {{u0, v0}, {u1, v0}, {u1, v1}, {u0, v1}};
    quad(xy, uv, col);
}
static void bgr555(u16 c, float a, float out[4]) {
    out[0] = (float)(c & 31) / 31.0f, out[1] = (float)(c >> 5 & 31) / 31.0f, out[2] = (float)(c >> 10 & 31) / 31.0f;
    out[3] = a;
}

// a group of lines with the same state for the GPU (the layers, their order, textures and blending, the windows'
// masks) - one draw per pass; in pieces of lines that differ only in what is vertex data (scroll, the affine
// parameters, the backdrop and brightness): a quad each
enum { MAX_PIECES = H };
typedef struct {
    int engine, n;
    const Gpu2dLineState *s;      // (the group's first line's: the GPU state)
    int y0[MAX_PIECES], y1[MAX_PIECES];
} Group;
static const Gpu2dLineState *piece_state(const Group *g, int i) { return &sLayers.line[g->engine][g->y0[i]]; }
// The wide view's 2D margins (margins2d): the groups drawn again over [sX0, sX1), the first and last lines sY lines
// further out, with only what goes on beyond the DS's screen (the backdrop, the affine BGs that wrap, the effects)
static int sMargin;
static float sX0 = 0, sX1 = W, sY = 0;
// The port's wide view (screen_compose): an engine composed straight onto its screen, the DS's picture in its
// middle; over [sX0, sX1) and sY lines beyond the first and last what goes on beyond the DS's screen (the backdrop,
// the wide 3D picture, the BGs margin_bg names, the effects), everything else in the DS's 256x192
#ifdef N3DS_PORT
static int sScreenMode;
#else
enum { sScreenMode = 0 };
#endif
#ifdef N3DS_PORT
// R5.3, the UI the game anchors (gpu2d.h: Gpu2dSprite.anchor, Gpu2dBgAnchor) at the wide screen's edges: the same
// quads moved by the margins' width (sAx, sAy: the DS's picture's place on the screen in screen_compose, else 0). An
// element is clipped to the DS's 256x192 before it is moved, as on a DS: sliding out of that it leaves the screen.
static float sAx, sAy;
static u8 sNear[8];  // (GPU2D_NEAR | id << 5: the edges element id is near, this engine and frame: anchors_setup)
static void anchor_shift(int a, float *dx, float *dy) {
    if (a & GPU2D_NEAR) a = sNear[a >> 5 & 7];
    *dx = a & GPU2D_LEFT ? -sAx : a & GPU2D_RIGHT ? sAx : 0;
    *dy = a & GPU2D_TOP ? -sAy : a & GPU2D_BOTTOM ? sAy : 0;
}
// a line where engine e's anchored BGs and windows are moved: the BGs still as the game gave them (Gpu2dBgAnchor.bgcnt)
static int bg_anchored(int e, const Gpu2dLineState *s) {
    const Gpu2dBgAnchor *a = &sLayers.bgAnchor[e];
    for (int n = 0; n < 4; n++)
        if ((a->bgs >> n & 1) && s->bgcnt[n] != a->bgcnt[n]) return 0;
    return a->bgs != 0;
}
#endif
static float piece_y0(const Group *g, int i) { return g->y0[i] == 0 ? 0.0f - sY : (float)g->y0[i]; }
static float piece_y1(const Group *g, int i) { return g->y1[i] == H ? H + sY : (float)g->y1[i]; }
#ifdef N3DS_PORT
// the menus' wipe (gpu2d_menu_wipe) as window 1's shape over the group's lines of the whole screen: as the game's
// H-blank (MenuScreen_WipeHBlank) sets it, the left edge pos - a zigzag (0-23-0 over 48 lines) clamped to 0-0xfe,
// to the line's end; the DS's 0-0xfe stretched over [sX0, sX1), so the edge crosses the wide screen as the DS's
static void menu_wipe(const Group *g) {
    for (int i = 0; i < g->n; i++)
        for (int y = (int)piece_y0(g, i); y < (int)piece_y1(g, i); y++) {
            int p = (y % 48 + 48) % 48, e = sLayers.menuWipe - (p < 24 ? p : 47 - p);
            if (e >= 0xfe) continue;
            float x = e <= 0 ? sX0 : sX0 + (float)e * (sX1 - sX0) / 0xfe;
            rect(x, (float)y, sX1, (float)(y + 1), 0, 0, 0, TEX, 0);
        }
}
#endif
// (group: the passes a margin layer can leave out - everything under it is a 2nd target the effects are allowed
// on, so none of it shows "as it is"; nothing after it reads the stencil)
static int sAllSecond, sNoMark;
static int affine_wraps(const Gpu2dLineState *s, int l) {
    return l >= 2 && l < 4 && (s->gpuBg >> l & 1) && s->bgKind[l] == 2 && (s->bgcnt[l] & 0x2000);
}
static BgTex *bg_texture(int e, int n, const Gpu2dLineState *s);
// a BG the margins draw: the affine ones that wrap; the text BGs of one tile everywhere (the same wherever they are
// repeated): what the screen transitions show (an iris: a BG of black outside its window, then over everything); in
// the port also those of a short repeated pattern (the menus' scrolling dots behind their panel)
static int margin_bg(int e, const Gpu2dLineState *s, int l, int is3d, int windows) {
    if (affine_wraps(s, l)) return 1;
    if (l >= 4 || (l == 0 && is3d) || !(s->gpuBg >> l & 1) || s->bgKind[l] != 1) return 0;
    const BgTex *b = bg_texture(e, l, s);
#ifdef N3DS_PORT
    return b && b->period;
#else
    return b && b->period == 1;
#endif
}

// a layer's texture and its wrapping for the group (0: none)
static BgTex *layer_bind(const Group *g, int l, int is3d) {
    const Gpu2dLineState *s = g->s;
    if (l == 4) { source(sLayers.sprite ? &sSprites : &sObj[g->engine]); return 0; }
    if (l == 0 && is3d) { source(s3dTex); return 0; }
    if (!(s->gpuBg >> l & 1)) { source(&sBg[g->engine][l]); return 0; }
    BgTex *b = bg_texture(g->engine, l, s);
    if (!b) return 0;
    int wrap = b->kind == 1 || (s->bgcnt[l] & 0x2000);  // (text BGs repeat; affine ones by BGxCNT bit 13)
    C3D_TexSetWrap(&b->tex, wrap ? GPU_REPEAT : GPU_CLAMP_TO_BORDER, wrap ? GPU_REPEAT : GPU_CLAMP_TO_BORDER);
    source(&b->tex);
    return b;
}
// an affine BG's quad over [x0, x1) x [y0, y1): at pixel (x, y) the texel ((X + PA x + PB (y - yr)) >> 8,
// (Y + PC x + PD (y - yr)) >> 8), p = PA, PB, PC, PD, X, Y (line yr's reference point); the corners half a pixel out,
// so that the pixels' centres sample those
static void affine_quad(const BgTex *b, float x0, float x1, float y0, float y1, float yr, const float p[6]) {
    const float xy[4][2] = {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}};
    float uv[4][2];
    for (int k = 0; k < 4; k++) {
        float px = xy[k][0] - 0.5f, py = xy[k][1] - 0.5f - yr;
        float tx = (p[4] + p[0] * px + p[1] * py) / 256.0f, ty = (p[5] + p[2] * px + p[3] * py) / 256.0f;
        uv[k][0] = tx / b->w, uv[k][1] = 1.0f - ty / b->h;
    }
    quad(xy, uv, 0);
}
// a layer's quad for piece i
static void layer_quad(const Group *g, int i, int l, int is3d, const BgTex *b) {
    const Gpu2dLineState *s = piece_state(g, i);
    float y0 = (float)g->y0[i], y1 = (float)g->y1[i];
    if (l == 0 && is3d) {  // (the 3D layer scrolled by BG0HOFS; nothing where it is scrolled away)
        float x0 = s->hofs3d < 0 ? (float)-s->hofs3d : 0, x1 = s->hofs3d > 0 ? (float)(W - s->hofs3d) : W;
        float ox = s3dWide ? GPU3D_WIDE_X : 0, oy = s3dWide ? GPU3D_WIDE_Y : 0, tw = s3dWide ? 512 : TEX;
        if (sScreenMode && s3dWide) {  // (as far as the screen and the wide picture go)
            float a = (float)(-s->hofs3d - GPU3D_WIDE_X), b = (float)(W + GPU3D_WIDE_X - s->hofs3d);
            x0 = sX0 > a ? sX0 : a, x1 = sX1 < b ? sX1 : b, y0 = piece_y0(g, i), y1 = piece_y1(g, i);
        }
        rect(x0, y0, x1, y1, (x0 + s->hofs3d + ox) / tw, (x1 + s->hofs3d + ox) / tw, y0 + oy, TEX, 0);
        return;
    }
    if (!b) {  // (the OBJs, a BG the CPU drew)
        if (l == 4 || !(g->s->gpuBg >> l & 1)) rect(0, y0, W, y1, 0, (float)W / TEX, y0, TEX, 0);
        return;
    }
    // (screen_compose: beyond the DS's screen only the BGs the margins draw)
    int out = sMargin || (sScreenMode && margin_bg(g->engine, s, l, is3d, 0));
    if (b->kind == 1) {  // text: scrolled (the margins: on beyond the screen's edges, as it repeats)
        float x0 = out ? sX0 : 0, x1 = out ? sX1 : W, dx = 0, dy = 0;
        float u0 = (s->bghofs[l] + x0) / b->w, u1 = (s->bghofs[l] + x1) / b->w;
        if (out) y0 = piece_y0(g, i), y1 = piece_y1(g, i);
#ifdef N3DS_PORT
        else if ((sLayers.bgAnchor[g->engine].bgs >> l & 1) && bg_anchored(g->engine, s)) anchor_shift(sLayers.bgAnchor[g->engine].anchor, &dx, &dy);
#endif
        rect(x0 + dx, y0 + dy, x1 + dx, y1 + dy, u0, u1, (float)s->bgvofs[l] + y0, (float)b->h, 0);
        return;
    }
    const int16_t *a = s->affine[l - 2];
    const int32_t *rf = &sLayers.ref[g->engine][g->y0[i]][2 * (l - 2)];
    float d0 = out ? piece_y0(g, i) : y0, d1 = out ? piece_y1(g, i) : y1, e0 = out ? sX0 : 0, e1 = out ? sX1 : W;
#ifdef N3DS_PORT
    // (screen_compose's margins above and below: a BG whose parameters the game changes line by line, with those it
    // gives for the lines there - Gpu2dMarginAffine; a line at a time)
    const Gpu2dMarginAffine *m = &sLayers.margin[g->engine];
    for (int k = 0; sScreenMode && out && m->bg == l && k < 2 * GPU2D_MARGIN; k++) {
        int line = k < GPU2D_MARGIN ? k - GPU2D_MARGIN : H + k - GPU2D_MARGIN;
        if (k < GPU2D_MARGIN ? !(i == 0 && g->y0[0] == 0 && line >= -sY) : !(i == g->n - 1 && g->y1[i] == H && line < H + sY))
            continue;
        const float p[6] = {m->pa[k], a[1], a[2], a[3], (float)m->x[k], (float)m->y[k]};
        affine_quad(b, e0, e1, (float)line, (float)line + 1, (float)line, p);
        if (k < GPU2D_MARGIN) d0 = y0;
        else d1 = y1;
    }
#endif
    const float p[6] = {a[0], a[1], a[2], a[3], (float)rf[0], (float)rf[1]};
    affine_quad(b, e0, e1, d0, d1, y0, p);
}
// (the OBJs of a priority only on the lines that have some: the OBJ layer is mostly empty)
static int sObjPrio;
// a sprite's lines [y0, y1), within the DS's screen (screen_compose: the target goes on beyond it); there an OBJ over
// the DS's left or right edge goes on into the margin (an item's cursor at the left edge), but an anchored one is cut
// at the edge before it is moved (sliding out it leaves the 3DS's screen there)
static void sprite_rect(const Gpu2dSprite *s, int y0, int y1) {
    int x0 = s->x, x1 = s->x + s->w;
    float dx = 0, dy = 0;
#ifdef N3DS_PORT
    if (sScreenMode && s->anchor) x0 = x0 < 0 ? 0 : x0, x1 = x1 > W ? W : x1;
    anchor_shift(s->anchor, &dx, &dy);
#endif
    if (x1 > x0)
        rect((float)x0 + dx, (float)y0 + dy, (float)x1 + dx, (float)y1 + dy, (float)(s->u + x0 - s->x) / GPU2D_ATLAS_W,
             (float)(s->u + x1 - s->x) / GPU2D_ATLAS_W, (float)(s->v + y0 - s->y), GPU2D_ATLAS_H, 0);
}
// the sprites of priority sObjPrio in the group's pieces: the later in OAM first, so that the earlier is in front
static void sprite_quads(const Group *g) {
    const Gpu2dSprite *sp = sLayers.sprites[g->engine];
    for (int k = sLayers.nsprites[g->engine] - 1; k >= 0; k--) {
        const Gpu2dSprite *s = &sp[k];
        if (s->prio != sObjPrio || s->window) continue;
        for (int i = 0; i < g->n; i++) {
            int y0 = s->y > g->y0[i] ? s->y : g->y0[i], y1 = s->y + s->h < g->y1[i] ? s->y + s->h : g->y1[i];
            if (y1 > y0) sprite_rect(s, y0, y1);
        }
    }
}
static void layer_pass(const Group *g, int l, int is3d, const BgTex *b) {
    begin();
    if (l == 4 && sLayers.sprite) sprite_quads(g);
    for (int i = 0; i < g->n && !(l == 4 && sLayers.sprite); i++) {
        if (l != 4) { layer_quad(g, i, l, is3d, b); continue; }
        const u8 *has = sLayers.objPrio[g->engine];
        for (int y0 = g->y0[i], y1; y0 < g->y1[i]; y0 = y1) {
            for (y1 = y0; y1 < g->y1[i] && (has[y1] >> sObjPrio & 1); y1++) {}
            if (y1 > y0) rect(0, (float)y0, W, (float)y1, 0, (float)W / TEX, (float)y0, TEX, 0);
            else y1 = y0 + 1;
        }
    }
    end();
}
static int obj_lines(const Group *g, int prio) {
    for (int k = 0; sLayers.sprite && k < sLayers.nsprites[g->engine]; k++)
        if (sLayers.sprites[g->engine][k].prio == prio && !sLayers.sprites[g->engine][k].window) return 1;
    for (int i = 0; i < g->n && !sLayers.sprite; i++)
        for (int y = g->y0[i]; y < g->y1[i]; y++)
            if (sLayers.objPrio[g->engine][y] >> prio & 1) return 1;
    return 0;
}

// one layer of a group: l 0-3 BG (BG0 the 3D layer when is3d), 4 the OBJs of priority prio; where the window regions
// allow it (rref / rmask: the stencil's region bits, 0 / 0 everywhere)
static void layer(const Group *g, int l, int prio, int is3d, u32 rref, u32 rmask) {
    const Gpu2dLineState *s = g->s;
    u32 bld = s->bldcnt, effect = bld >> 6 & 3, first = bld >> l & 1, second = bld >> (8 + l) & 1;
    u32 eva = s->bldalpha & 31, evb = s->bldalpha >> 8 & 31;
    eva = eva > 16 ? 16 : eva, evb = evb > 16 ? 16 : evb;
    if (l == 4 && !obj_lines(g, prio)) return;  // (nothing of it to draw)
    sObjPrio = prio;
    if (l == 4 && !sLayers.sprite) C3D_AlphaTest(true, GPU_EQUAL, (prio + 1) * 32);
    else C3D_AlphaTest(true, GPU_GREATER, 0);
    u32 bits = second | first << 1;
    int blend3d = l == 0 && is3d, alpha = !blend3d && first && effect == 1;
    BgTex *b = layer_bind(g, l, is3d);
    if (!(blend3d || alpha)) {
        stencil(GPU_EQUAL, rref | bits, rmask, 3);
        opaque();
        sTag = l == 4 ? ST_OBJ : ST_LAYER;
        layer_pass(g, l, is3d, b);
        return;
    }
    if (blend3d && s3dPremultiplied && !sMargin) {
        // the GPU's 3D picture is blended the same way over a 2nd target and elsewhere: one pass, which also says
        // what this layer is where it is (the three passes below give the same)
        stencil(GPU_EQUAL, rref | bits, rmask, 3);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
        sTag = ST_BLEND;
        layer_pass(g, l, is3d, b);
        return;
    }
    // over a 2nd target (stencil bit 0), where the windows allow effects (bit 2 clear; the 3D layer: always):
    // blended, and marked (bit 5); elsewhere as it is; then what this layer is, where it is
    u32 cond = blend3d ? 1 : 5;
    stencil(GPU_EQUAL, rref | 1 | 0x20, rmask | cond, 0x20);
    if (blend3d) {
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, s3dPremultiplied ? GPU_ONE : GPU_SRC_ALPHA,
                       GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
    } else {
        // (the constant colour's red, green and blue are eva / 16, its alpha evb / 16: R in the low byte)
        u32 ca = eva * 255 / 16, cb = evb * 255 / 16;
        C3D_BlendingColor(ca | ca << 8 | ca << 16 | cb << 24);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_CONSTANT_COLOR, GPU_CONSTANT_ALPHA, GPU_ONE, GPU_ZERO);
    }
    sTag = ST_BLEND;
    layer_pass(g, l, is3d, b);
    if (!sAllSecond) {
        stencil(GPU_EQUAL, rref, rmask | 0x20, 0);
        // (the GPU's premultiplied 3D picture over what is below elsewhere too: its colour alone is darkened where its
        // alpha isn't full)
        if (!(blend3d && s3dPremultiplied)) opaque();
        sTag = ST_ASIS;
        layer_pass(g, l, is3d, b);
    }
    if (sNoMark) return;
    colour_writes(0);
    stencil(GPU_EQUAL, rref | bits, rmask, 0x23);
    sTag = ST_MARK;
    layer_pass(g, l, is3d, b);
    colour_writes(1);
}

static void group(const Group *g) {
    const Gpu2dLineState *s = g->s;
    float white[4] = {1, 1, 1, 1};
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    source(0);
    opaque();
    if (s->white) {
        C3D_StencilTest(false, GPU_ALWAYS, 0, 0, 0);
        begin();
        for (int i = 0; i < g->n; i++) rect(sX0, piece_y0(g, i), sX1, piece_y1(g, i), 0, 0, 0, TEX, white);
        end();
        return;
    }
    u32 bld = s->bldcnt, effect = bld >> 6 & 3, dispcnt = s->dispcnt;
    int is3d = g->engine == 0 && (dispcnt & 8), windows = (dispcnt & 0xe000) != 0;
    // the windows: each pixel's region (bits 3-4) and whether it allows the effects (bit 2); the layers each region
    // shows (outside, window 0, window 1, the OBJ window: WINOUT, WININ, WININ high, WINOUT high)
    u8 shows[4] = {(u8)(s->winout & 0x3f), (u8)(s->winin & 0x3f), (u8)(s->winin >> 8 & 0x3f), (u8)(s->winout >> 8 & 0x3f)};
    // the backdrop: 1st / 2nd target as BLDCNT bits 5 and 13; the effects allowed, no window, nothing marked (the
    // margins: outside the windows)
    u32 outside = sMargin && windows && !(shows[0] & 0x20) ? 4 : 0;
    stencil(GPU_ALWAYS, (bld >> 13 & 1) | (bld >> 5 & 1) << 1 | outside, 0, 0xff);
    sTag = ST_BACKDROP;
    begin();
#ifdef N3DS_PORT  // (a quad over the group's lines, texel row y the colour of line y)
    source(&sBackdrop[g->engine]);
    rect(sX0, piece_y0(g, 0), sX1, piece_y1(g, g->n - 1), 0, 1, piece_y0(g, 0), 256, 0);
    end();
    source(0);
#else
    float col[4];
    for (int i = 0; i < g->n; i++) {
        bgr555(piece_state(g, i)->backdrop, 1, col);
        rect(sX0, piece_y0(g, i), sX1, piece_y1(g, i), 0, 0, 0, TEX, col);
    }
    end();
#endif
    if (windows && !sMargin && sLayers.sprite) {
        // the regions from the windows' shapes (with sprites): outside, then the OBJ window's OBJs, window 1, window 0
        // over them, each writing its region (bits 3-4) and whether it allows the effects (bit 2)
        colour_writes(0);
        sTag = ST_REGION;
        stencil(GPU_ALWAYS, shows[0] & 0x20 ? 0 : 4, 0, 0x1c);
        begin();
        for (int i = 0; i < g->n; i++)
            if (sScreenMode) rect(sX0, piece_y0(g, i), sX1, piece_y1(g, i), 0, 0, 0, TEX, 0);
            else rect(0, (float)g->y0[i], W, (float)g->y1[i], 0, 0, 0, TEX, 0);
        end();
        if (dispcnt & 0x8000) {
            source(&sSprites);
            C3D_AlphaTest(true, GPU_GREATER, 0);
            stencil(GPU_ALWAYS, 3 << 3 | (shows[3] & 0x20 ? 0 : 4), 0, 0x1c);
            begin();
            const Gpu2dSprite *sp = sLayers.sprites[g->engine];
            for (int k = 0; k < sLayers.nsprites[g->engine]; k++) {
                const Gpu2dSprite *t = &sp[k];
                for (int i = 0; t->window && i < g->n; i++) {
                    int y0 = t->y > g->y0[i] ? t->y : g->y0[i], y1 = t->y + t->h < g->y1[i] ? t->y + t->h : g->y1[i];
                    if (y1 > y0) sprite_rect(t, y0, y1);
                }
            }
            end();
            C3D_AlphaTest(false, GPU_ALWAYS, 0);
            source(0);
        }
        for (int w = 1; w >= 0; w--) {  // (a window's X1 after its X2: it wraps round the line's end)
            if (!(dispcnt & 0x2000u << w)) continue;
            stencil(GPU_ALWAYS, (u32)(w + 1) << 3 | (shows[w + 1] & 0x20 ? 0 : 4), 0, 0x1c);
            begin();
#ifdef N3DS_PORT
            if (w == 1 && g->engine == 0 && sScreenMode && sLayers.menuWipe >= 0) {
                menu_wipe(g);
                end();
                continue;
            }
#endif
            for (int i = 0; i < g->n; i++) {
                const Gpu2dLineState *ps = piece_state(g, i);
                float x1 = (float)(ps->winh[w] >> 8), x2 = (float)(ps->winh[w] & 0xff), y0 = (float)g->y0[i], y1 = (float)g->y1[i];
                if (!(ps->winV >> w & 1)) continue;
                float dx = 0, dy = 0;
#ifdef N3DS_PORT
                if ((sLayers.bgAnchor[g->engine].windows >> w & 1) && bg_anchored(g->engine, ps)) anchor_shift(sLayers.bgAnchor[g->engine].anchor, &dx, &dy);
#endif
                x1 += dx, x2 += dx, y0 += dy, y1 += dy;
                if (x1 < x2) rect(x1, y0, x2, y1, 0, 0, 0, TEX, 0);
                else if (x1 > x2) rect(dx, y0, x2, y1, 0, 0, 0, TEX, 0), rect(x1, y0, W + dx, y1, 0, 0, 0, TEX, 0);
            }
            end();
        }
        colour_writes(1);
    } else if (windows && !sMargin) {
        colour_writes(0);
        sTag = ST_REGION;
        source(&sRegion[g->engine]);
        for (u32 k = 0; k < 4; k++) {
            C3D_AlphaTest(true, GPU_EQUAL, (int)(63 + 64 * k));
            stencil(GPU_ALWAYS, k << 3 | (shows[k] & 0x20 ? 0 : 4), 0, 0x1c);
            begin();
            for (int i = 0; i < g->n; i++)
                rect(0, (float)g->y0[i], W, (float)g->y1[i], 0, (float)W / TEX, (float)g->y0[i], TEX, 0);
            end();
        }
        colour_writes(1);
    }
    int margin = 0, margins = 0;  // (the margins' layers drawn, of how many)
    for (int l = 0; sMargin && l < 4; l++)
        margins += (dispcnt & 0x100u << l) && margin_bg(g->engine, s, l, is3d, windows) && (!windows || shows[0] >> l & 1);
    // (screen_compose without windows: the first layer and the last one drawn, for the passes that would draw nothing -
    // as the margins' sAllSecond and sNoMark)
    int first = -1, last = -1;
    for (int p = 3; sScreenMode && !windows && p >= 0; p--)
        for (int n = 0; n <= 4; n++) {
            int l = n == 4 ? 4 : 3 - n;
            if (l < 4 ? (dispcnt & 0x100u << l) && s->prio[l] == p : (dispcnt & 0x1000) && obj_lines(g, p)) {
                if (first < 0) first = p * 8 + l;
                last = p * 8 + l;
            }
        }
    for (int p = 3; p >= 0; p--) {
        for (int n = 0; n <= 4; n++) {  // (BG3 to BG0, then the OBJs of this priority in front of them)
            int l = n == 4 ? 4 : 3 - n;
            if (l < 4 && (!(dispcnt & 0x100u << l) || s->prio[l] != p)) continue;
            if (l == 4 && !(dispcnt & 0x1000)) continue;
            if (first >= 0) {
                sAllSecond = p * 8 + l == first && (bld >> 13 & 1);
                sNoMark = p * 8 + l == last && effect != 2 && effect != 3;
                layer(g, l, p, is3d, 0, 0);
                sAllSecond = sNoMark = 0;
                continue;
            }
            if (sMargin) {  // (under the first: the backdrop alone; after the last: only the master brightness)
                if (l == 4 || !margin_bg(g->engine, s, l, is3d, windows) || (windows && !(shows[0] >> l & 1))) continue;
                sAllSecond = margin == 0 && (bld >> 13 & 1) && !outside;
                sNoMark = ++margin == margins && effect != 2 && effect != 3;
                layer(g, l, p, is3d, 0, 0);
                sAllSecond = sNoMark = 0;
                continue;
            }
            // (the layers the CPU drew have the windows applied; the GPU's BGs and the 3D layer are drawn per region)
            // (the sprites too: the windows aren't applied to them)
            int perRegion = windows && ((l < 4 && ((s->gpuBg >> l & 1) || (l == 0 && is3d))) || (l == 4 && sLayers.sprite));
            if (!perRegion) { layer(g, l, p, is3d, 0, 0); continue; }
            for (u32 k = 0; k < 4; k++)
                if (shows[k] >> l & 1) layer(g, l, p, is3d, k << 3, 0x18);
        }
    }
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    source(0);
    sTag = ST_EFFECT;
    if (effect == 2 || effect == 3) {  // brightness up / down where a 1st target is on top (BLDY per piece)
        stencil(GPU_EQUAL, 2, 6, 0);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
        begin();
        for (int i = 0; i < g->n; i++) {
            u32 evy = piece_state(g, i)->bldy & 31;
            float c = effect == 2 ? 1.0f : 0.0f, f[4] = {c, c, c, (float)(evy > 16 ? 16 : evy) / 16.0f};
            if (evy) rect(sX0, piece_y0(g, i), sX1, piece_y1(g, i), 0, 0, 0, TEX, f);
        }
        end();
    }
    C3D_StencilTest(false, GPU_ALWAYS, 0, 0, 0);  // master brightness (per piece)
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
    begin();
    for (int i = 0; i < g->n; i++) {
        u32 mb = piece_state(g, i)->masterBright, mode = mb >> 14, k = mb & 31;
        float c = mode == 1 ? 1.0f : 0.0f, f[4] = {c, c, c, (float)(k > 16 ? 16 : k) / 16.0f};
        if ((mode == 1 || mode == 2) && k) rect(sX0, piece_y0(g, i), sX1, piece_y1(g, i), 0, 0, 0, TEX, f);
    }
    end();
}

static void upload(void *src, C3D_Tex *t, u32 bytes, GX_TRANSFER_FORMAT fmt) {
    GSPGPU_FlushDataCache(src, bytes);
    frame_flush_commands();
    HOST_PROF_ON(gPicaProf, PROF_UPLOAD);
    C3D_SyncDisplayTransfer(src, GX_BUFFER_DIM(W, H), t->data, GX_BUFFER_DIM(W, H),
                            kLinearToTiled | GX_TRANSFER_IN_FORMAT(fmt) | GX_TRANSFER_OUT_FORMAT(fmt));
}

// lines y and y + 1 of an engine in one run: the same state, and the affine BGs the GPU draws stepping on as without
// a reload of their reference points (an H-blank write)
static int same_run(int e, int y) {
    const Gpu2dLineState *a = &sLayers.line[e][y], *b = a + 1;
    Gpu2dLineState t;
    if (kLineBackdrop) t = *b, t.backdrop = a->backdrop, b = &t;  // (sBackdrop has it)
    if (memcmp(a, b, sizeof *a)) return 0;
    for (int i = 0; i < 2; i++)
        if ((a->gpuBg >> (2 + i) & 1) && a->bgKind[2 + i] == 2 &&
            (sLayers.ref[e][y + 1][2 * i] != sLayers.ref[e][y][2 * i] + a->affine[i][1] ||
             sLayers.ref[e][y + 1][2 * i + 1] != sLayers.ref[e][y][2 * i + 1] + a->affine[i][3]))
            return 0;
    return 1;
}

// the state the GPU is set up with for a line: what is not vertex data
static int same_gpu_state(const Gpu2dLineState *a, const Gpu2dLineState *b) {
    return a->dispcnt == b->dispcnt && a->gpuBg == b->gpuBg && !memcmp(a->bgKind, b->bgKind, sizeof a->bgKind) &&
           !memcmp(a->bgcnt, b->bgcnt, sizeof a->bgcnt) && !memcmp(a->prio, b->prio, sizeof a->prio) &&
           a->bldcnt == b->bldcnt && a->bldalpha == b->bldalpha && a->winin == b->winin && a->winout == b->winout &&
           a->white == b->white;
}

// (two frames' arguments in turn, as the 2D renderer's records: the compositing before the last may still run)
// The wide view (gpu3d.h): around the DS's picture at (x0, y0), its 3D world beyond the DS's screen: over the
// backdrop, under the master brightness, those of the middle line (the 2D layers, fog and the effects are the DS
// picture's only: a seam is expected)
static void margins(float x0, float y0) {
    const Gpu2dLineState *m = &sLayers.line[0][H / 2];
    float col[4], k = (float)GPU3D_WIDE_X, l = (float)GPU3D_WIDE_Y;
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    source(0);
    opaque();
    bgr555(m->backdrop, 1, col);
    begin();
    rect(x0 - k, y0 - l, x0 + W + k, y0 + H + l, 0, 0, 0, TEX, col);
    end();
    source(s3dTex);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
    begin();
    const float xy[4][2] = {{x0 - k, y0 - l}, {x0 + W + k, y0 - l}, {x0 + W + k, y0 + H + l}, {x0 - k, y0 + H + l}};
    const float uv[4][2] = {{0, 1}, {(W + 2 * k) / 512, 1}, {(W + 2 * k) / 512, 1 - (H + 2 * l) / TEX}, {0, 1 - (H + 2 * l) / TEX}};
    quad(xy, uv, 0);
    end();
    u32 mb = m->masterBright, mode = mb >> 14, f = mb & 31;
    if ((mode == 1 || mode == 2) && f) {
        float c = mode == 1 ? 1.0f : 0.0f, q[4] = {c, c, c, (float)(f > 16 ? 16 : f) / 16.0f};
        source(0);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
        begin();
        rect(x0 - k, y0 - l, x0 + W + k, y0 + H + l, 0, 0, 0, TEX, q);
        end();
    }
}

// whether the wide 3D picture goes in the margins: they show the 3D layer outside the windows, and no BG of one opaque
// tile everywhere is in front of it (the screen transitions' black); else the 2D layers (margins2d)
static int margins_3d(void) {
    const Gpu2dLineState *m = &sLayers.line[0][H / 2];
    if ((m->dispcnt & 0xe000) && !(m->winout & 1)) return 0;
    for (int l = 1; l < 4; l++) {
        if (!(m->dispcnt & 0x100u << l) || m->prio[l] >= m->prio[0] || !(m->gpuBg >> l & 1) || m->bgKind[l] != 1) continue;
        const BgTex *b = bg_texture(0, l, m);
        if (b && b->opaque) return 0;
    }
    return 1;
}

// an engine's picture, group by group of lines with the same GPU state, in pieces of lines with the same state
static void compose(int e) {
    static Group g;
    g.engine = e, g.n = 0, g.s = &sLayers.line[e][0];
    for (int y0 = 0, y1; y0 < H; y0 = y1) {
        for (y1 = y0 + 1; y1 < H && same_run(e, y1 - 1); y1++) {}
        if (g.n && !same_gpu_state(g.s, &sLayers.line[e][y0])) {
            group(&g);
            g.n = 0, g.s = &sLayers.line[e][y0];
        }
        g.y0[g.n] = y0, g.y1[g.n] = y1, g.n++;
    }
    group(&g);
}

// The wide view: around engine e's picture at (x0, y0) on a screen sw wide (one with a stencil buffer), the engine's
// lines drawn on beyond the DS's screen as far as they can be: the backdrop, the affine BGs that wrap (the town's
// sky: its clouds), the effects; the lines above and below as the first and last one
static void margins2d(int e, float x0, float y0, float sw) {
    C3D_Mtx m;
    Mtx_OrthoTilt(&m, -x0, sw - x0, 240.0f - y0, -y0, 0.0f, 1.0f, true);  // (the DS's pixels, as projection's)
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sUProjection, &m);
    sMargin = 1, sX0 = -x0, sX1 = sw - x0, sY = y0;
    // (not in the middle, where the DS's picture goes over it: the box in the turned target's pixels, a pixel inside
    // the picture's edges, which the picture covers; x0 and y0 centre it, so it is the same either way round)
    C3D_SetScissor(GPU_SCISSOR_INVERT, (u32)y0 + 1, (u32)x0 + 1, (u32)y0 + H - 1, (u32)x0 + W - 1);
    compose(e);
    C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
    sMargin = 0, sX0 = 0, sX1 = W, sY = 0;
    projection(sw, 240.0f, 1);
}

#ifdef N3DS_PORT
// R5.3: the anchored UI's place this frame on engine e's screen (the DS's picture at x0, y0): the edges each
// GPU2D_NEAR element is near (its OBJs' box's middle in the outer third of the DS's screen), and, for the touch
// (pdisp_touch_map), each moved element's box on the DS's screen with its move
typedef struct { int16_t x0, y0, x1, y1, dx, dy; } TouchMove;
enum { TOUCH_MOVES = 16 };
static TouchMove sMoves[TOUCH_MOVES], sTouch[TOUCH_MOVES];
static int sNMoves;
static volatile int sNTouch;
static void anchors_setup(int e, float x0, float y0) {
    static int box[256][4];  // (per anchor: its OBJs' box, x0 y0 x1 y1, on the DS's screen)
    for (int a = 0; a < 256; a++) box[a][0] = W, box[a][1] = H, box[a][2] = box[a][3] = 0;
    for (int k = 0; k < sLayers.nsprites[e]; k++) {
        const Gpu2dSprite *t = &sLayers.sprites[e][k];
        int *b = box[t->anchor], a0 = t->x < 0 ? 0 : t->x, a1 = t->x + t->w > W ? W : t->x + t->w;
        int b0 = t->y < 0 ? 0 : t->y, b1 = t->y + t->h > H ? H : t->y + t->h;
        if (!t->anchor || t->window || a1 <= a0 || b1 <= b0) continue;
        if (a0 < b[0]) b[0] = a0;
        if (b0 < b[1]) b[1] = b0;
        if (a1 > b[2]) b[2] = a1;
        if (b1 > b[3]) b[3] = b1;
    }
    for (int id = 0; id < 8; id++) {
        const int *b = box[GPU2D_NEAR | id << 5];
        int cx = b[0] + b[2], cy = b[1] + b[3];  // (twice the middle)
        sNear[id] = b[2] <= b[0] ? 0
                  : (u8)((cx < 2 * W / 3 ? GPU2D_LEFT : cx > 4 * W / 3 ? GPU2D_RIGHT : 0) |
                         (cy < 2 * H / 3 ? GPU2D_TOP : cy > 4 * H / 3 ? GPU2D_BOTTOM : 0));
    }
    sAx = x0, sAy = y0, sNMoves = 0;
    float dx, dy;
    for (int a = 1; a < 256 && sNMoves < TOUCH_MOVES; a++) {
        anchor_shift(a, &dx, &dy);
        if (box[a][2] > box[a][0] && (dx || dy))
            sMoves[sNMoves++] = (TouchMove){(int16_t)box[a][0], (int16_t)box[a][1], (int16_t)box[a][2], (int16_t)box[a][3], (int16_t)dx, (int16_t)dy};
    }
    // (an anchored BG: the box of its windows' lines)
    const Gpu2dBgAnchor *ba = &sLayers.bgAnchor[e];
    anchor_shift(ba->anchor, &dx, &dy);
    int bx0 = W, by0 = H, bx1 = 0, by1 = 0;
    for (int y = 0; ba->bgs && ba->windows && (dx || dy) && y < H; y++)
        for (int w = 0; w < 2; w++) {
            const Gpu2dLineState *ls = &sLayers.line[e][y];
            if (!(ba->windows >> w & 1) || !(ls->dispcnt & 0x2000u << w) || !(ls->winV >> w & 1) || !bg_anchored(e, ls)) continue;
            int a0 = ls->winh[w] >> 8, a1 = ls->winh[w] & 0xff;
            if (a1 <= a0) a0 = 0, a1 = W;
            bx0 = a0 < bx0 ? a0 : bx0, bx1 = a1 > bx1 ? a1 : bx1, by0 = y < by0 ? y : by0, by1 = y + 1;
        }
    if (bx1 > bx0 && sNMoves < TOUCH_MOVES)
        sMoves[sNMoves++] = (TouchMove){(int16_t)bx0, (int16_t)by0, (int16_t)bx1, (int16_t)by1, (int16_t)dx, (int16_t)dy};
}
// (the bottom screen's moves for the frontend's thread; shortcut: not locked - a touch while they change may go by
// the frame before's, which hardly differ)
static void touch_publish(int on) {
    sNTouch = 0;
    if (!on) return;
    memcpy(sTouch, sMoves, sizeof sMoves);
    sNTouch = sNMoves;
}
#endif
void pdisp_touch_map(int *x, int *y) {
#ifdef N3DS_PORT
    for (int i = 0, n = sNTouch; i < n; i++) {
        const TouchMove *m = &sTouch[i];
        int sx = *x - m->dx, sy = *y - m->dy;
        if (sx >= m->x0 && sx < m->x1 && sy >= m->y0 && sy < m->y1) { *x = sx, *y = sy; return; }
    }
#else
    (void)x, (void)y;
#endif
}

// The port's wide view: engine e straight onto the screen being drawn (sw wide, with a stencil buffer), the
// DS's picture at (x0, y0) - no picture of the engine first, no copy, no margins drawn apart (sScreenMode)
static void screen_compose(int e, float x0, float y0, float sw) {
    C3D_Mtx m;
    Mtx_OrthoTilt(&m, -x0, sw - x0, 240.0f - y0, -y0, 0.0f, 1.0f, true);  // (the DS's pixels, as projection's)
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sUProjection, &m);
#ifdef N3DS_PORT
    sScreenMode = 1, sX0 = -x0, sX1 = sw - x0, sY = y0;
    anchors_setup(e, x0, y0);
    colour_writes(1);
    compose(e);
    sScreenMode = 0, sX0 = 0, sX1 = W, sY = 0, sAx = sAy = 0;
#endif
    projection(sw, 240.0f, 1);
}

// The port: a screen drawn again only when what it shows changed. The game changes its 2D once a game frame
// (every third frame) and its 3D picture too without the pictures between, so in town the bottom screen stays the
// same in two frames of three (the top one, the sky, changes in two of three: its clouds move every frame). A hash
// of its engine's inputs (the lines' states, the sprites with the hash of what their pixels come from, the GPU's 3D
// picture: a new one is in the other texture, the fps text on the top, the options page on the bottom), and its BG
// textures brought up to date first (bg_texture, once a frame, compares the palette and the VRAM the BG uses:
// hashing all the engine's BG VRAM instead cost 1.1 ms a frame on hardware); always drawn with a layer the CPU drew
// on it (the 3D picture too), and while probing. ACWW_PICA_TOPSKIP=0: always drawn.
static char sFpsText[24];  // (fps_text: what show_fps draws, "" off)
static u32 hash32(u32 h, const void *p, size_t n) {
    const u8 *b = p;
    for (; n >= 4; n -= 4, b += 4) h = (h ^ *(const u32 *)b) * 16777619u, h ^= h >> 15;  // (FNV-1a by words)
    for (; n; n--) h = (h ^ *b++) * 16777619u;
    return h;
}
static int screen_unchanged(int s, int e, u32 used, u32 cpuBg, const void *texture3d) {
    HOST_PROF_ON(gPicaProf, PROF_HASH);
    static int on = -1;
    static u32 last[2];
    if (on < 0) on = !getenv("ACWW_PICA_TOPSKIP") || atoi(getenv("ACWW_PICA_TOPSKIP"));
    const Gpu2dLayers *L = &sLayers;
    // a new 3D picture in this frame and the one before (the pictures between: every frame) isn't worth the hash
    static const void *lastTex[2];
    static int new3d[2];  // (frames in a row with a new 3D picture)
    int with3d = e == 0 && (used & 8);
    new3d[s] = with3d && texture3d != lastTex[s] ? new3d[s] + 1 : 0;
    lastTex[s] = with3d ? texture3d : 0;
    int cpu = !on || (used & 0xf00u & cpuBg << 8) || ((used & 0x1000) && !L->sprite) ||
              ((used & 0xe000) && !L->sprite) || (with3d && !texture3d) || new3d[s] >= 2;  // (the CPU's layers, the 3D)
    u32 h = 2166136261u ^ (u32)e, drawnBefore = sBgDrawn;
    if (!cpu) {
        if (with3d) h = hash32(h, &texture3d, sizeof texture3d);
        h = hash32(h, L->line[e], sizeof L->line[e]);
        h = hash32(h, L->ref[e], sizeof L->ref[e]);
        h = hash32(h, L->objPrio[e], sizeof L->objPrio[e]);
#ifdef N3DS_PORT
        h = hash32(h, &L->margin[e], sizeof L->margin[e]);
        h = hash32(h, &L->iris, sizeof L->iris);
        h = hash32(h, &L->menuWipe, sizeof L->menuWipe);
        h = hash32(h, &L->bgAnchor[e], sizeof L->bgAnchor[e]);
#endif
        // (as layer_bind would at the first group showing it: the same state, so the same texture)
        u32 seen = 0;
        for (int y = 0; y < H; y++) {
            const Gpu2dLineState *ls = &L->line[e][y];
            for (int n = 0; n < 4; n++)
                if (!(seen >> n & 1) && (ls->dispcnt & 0x100u << n) && (ls->gpuBg >> n & 1) && !(n == 0 && e == 0 && (ls->dispcnt & 8)))
                    bg_texture(e, n, ls), seen |= 1u << n;
        }
        h = hash32(h, &L->nsprites[e], sizeof L->nsprites[e]);
        for (int k = 0; k < L->nsprites[e]; k++) {
            const Gpu2dSprite *t = &L->sprites[e][k];
            h = hash32(h, t, sizeof *t);  // (with Gpu2dSprite.hash: its pixels, not read again)
        }
        if (s == 0) h = hash32(h, sFpsText, sizeof sFpsText);
#ifdef N3DS_PORT
        if (s == 1) {
            h = hash32(h, &L->optSerial, sizeof L->optSerial);
            h = hash32(h, &L->optPageY, sizeof L->optPageY);
            h = hash32(h, &L->optBtnY, sizeof L->optBtnY);
        }
#endif
    }
    int same = !cpu && sProbe <= 0 && h == last[s] && sBgDrawn == drawnBefore;
    last[s] = cpu ? 0 : h;
    static u32 frames, skipped[2];  // (ACWW_PICA_STATS: how often, every 300 frames)
    skipped[s] += (u32)same;
    if (s == 1 && sStats > 0 && ++frames % 300 == 0) {
        fprintf(stderr, "pica: the top screen drawn again in %lu of 300 frames, the bottom one in %lu\n",
                (unsigned long)(300 - skipped[0]), (unsigned long)(300 - skipped[1]));
        skipped[0] = skipped[1] = 0;
    }
    return same;
}

// The port: the options page and the pause menu's button (gpu2d_options) over the DS's picture at (x0, y0), dw
// wide (W, or 320 scaled), on a screen sw wide, in the DS's pixels
static void options_page(float x0, float y0, float sw, float dw) {
#ifdef N3DS_PORT
    if (!sOptSerial || (sLayers.optPageY >= H && sLayers.optBtnY == GPU2D_NO_BTN)) return;
    C3D_Mtx m;
    float k = dw / W;
    Mtx_OrthoTilt(&m, -x0 / k, (sw - x0) / k, (240.0f - y0) / k, -y0 / k, 0.0f, 1.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sUProjection, &m);
    source(&sOpt);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
    sTag = ST_SCREEN;
    begin();
    rect(0, 17.0f + (float)sLayers.optPageY, W, H, 0, 1, 17, TEX, 0);  // (cut at the DS's bottom edge while it slides)
    if (sLayers.optBtnY != GPU2D_NO_BTN) {  // (cut at the DS's top edge as the tabs' OBJs while the tab bar slides)
        float by = (float)sLayers.optBtnY, top = by < 0 ? 0 : by;
        rect(26, top, 50, by + 16, 0, 24.0f / TEX, top - by, TEX, 0);
    }
    end();
    projection(sw, 240.0f, 1);
    opaque();
#else
    (void)x0, (void)y0, (void)sw, (void)dw;
#endif
}

// The port: the screen transitions' iris (gpu2d_iris) over a whole screen sw wide: black outside a circle round
// its middle, which opens from nothing to the screen's corners; a ring of quads out to beyond the screen's edges
static void iris(float sw) {
#ifdef N3DS_PORT
    if (sLayers.iris < 0) return;
    enum { N = 64 };  // (the circle's edge at most 0.3 pixels inside the true one)
    static float c[N + 1][2];
    if (!c[0][0])
        for (int i = 0; i <= N; i++) c[i][0] = cosf(6.2831853f * (float)i / N), c[i][1] = sinf(6.2831853f * (float)i / N);
    static const float black[4] = {0, 0, 0, 1}, uv[4][2];
    float x = sw / 2, y = 120, r = (float)(0x1000 - sLayers.iris) / 0x1000 * sqrtf(x * x + y * y), o = sw;
    source(0);
    opaque();
    sTag = ST_SCREEN;
    begin();
    for (int i = 0; i < N; i++) {
        const float *a = c[i], *b = c[i + 1];
        const float xy[4][2] = {{x + r * a[0], y + r * a[1]}, {x + o * a[0], y + o * a[1]}, {x + o * b[0], y + o * b[1]},
                                {x + r * b[0], y + r * b[1]}};
        quad(xy, uv, black);
    }
    end();
#else
    (void)sw;
#endif
}

// ACWW_SHOW_FPS=1: the game's frames and the pictures composited a second (over the last second, "60/30"), in the top
// screen's top left corner: characters of 3x5 pixels (bit 14 the top left one, row by row; 0-9, then '/'), each
// pixel 2x2, white on black
static void fps_text(u64 gameFrame) {  // (every composited frame, drawn or not)
    static int on = -1;
    static u32 pictures, fps, pps;
    static u64 since, sinceFrame;
    if (on < 0) on = getenv("ACWW_SHOW_FPS") && atoi(getenv("ACWW_SHOW_FPS"));
    if (!on) return;
    u64 now = osGetTime();
    if (!since) since = now, sinceFrame = gameFrame;
    pictures++;
    if (now - since >= 1000) {
        fps = (u32)((gameFrame - sinceFrame) * 1000 / (now - since)), pps = (u32)(pictures * 1000 / (now - since));
        pictures = 0, since = now, sinceFrame = gameFrame;
    }
    snprintf(sFpsText, sizeof sFpsText, "%lu/%lu", (unsigned long)fps, (unsigned long)pps);
}
static void show_fps(void) {
    static const u16 kChar[11] = {0x7b6f, 0x2c97, 0x73e7, 0x73cf, 0x5bc9, 0x79cf, 0x79ef, 0x7249, 0x7bef, 0x7bcf, 0x12a4};
    const char *text = sFpsText;
    int n = (int)strlen(text);
    if (!n) return;
    static const float black[4] = {0, 0, 0, 1}, white[4] = {1, 1, 1, 1};
    source(0);
    opaque();
    begin();
    rect(0, 0, 4 + 8 * (float)n, 14, 0, 0, 0, TEX, black);
    for (int i = 0; i < n; i++)
        for (int b = 0; b < 15; b++)
            if (kChar[text[i] == '/' ? 10 : text[i] - '0'] >> (14 - b) & 1) {
                float x = 2 + 8 * (float)i + 2 * (float)(b % 3), y = 2 + 2 * (float)(b / 3);
                rect(x, y, x + 2, y + 2, 0, 0, 0, TEX, white);
            }
    end();
}

// screen s's picture as drawn (its target read back: 240 x its width, a turned target's rows being the screen's
// columns from the left, its columns the lines from the bottom; RGBA8), 0 without one. Once the GPU has drawn it: an
// empty frame's C3D_FrameBegin waits for that (a read right after C3D_FrameEnd raced the GPU; one in a frame would
// only be queued)
static const u32 *screen_pixels(int s) {
    static u32 *px;
    u32 sw = s == 0 ? 400 : 320;
    if (!sScreen[s] || !(px || (px = linearAlloc(240 * 400 * 4)))) return 0;
    C3D_FrameBegin(0);
    C3D_FrameEnd(0);
    C3D_SyncDisplayTransfer((u32 *)sScreen[s]->frameBuf.colorBuf, GX_BUFFER_DIM(240, sw), px, GX_BUFFER_DIM(240, sw),
                            kTiledToLinear);
    GSPGPU_InvalidateDataCache(px, 240 * sw * 4);
    return px;
}
// ACWW_PICA_SHOT=<frame>,<frame>,...: both screens as drawn at those game frames, to sdmc:/3ds/acww/screen<0|1>-<frame>.ppm
static void screen_shots(const char *list, u64 gameFrame) {
    int want = 0;
    for (const char *p = list; *p && !want; p = strchr(p, ',') ? strchr(p, ',') + 1 : "")
        want = strtoull(p, 0, 10) == gameFrame;
    for (int s = 0; want && s < 2; s++) {
        u32 sw = s == 0 ? 400 : 320;
        const u32 *px = screen_pixels(s);
        char name[64];
        snprintf(name, sizeof name, "sdmc:/3ds/acww/screen%d-%llu.ppm", s, (unsigned long long)gameFrame);
        FILE *f = px ? fopen(name, "wb") : 0;
        if (!f) continue;
        fprintf(f, "P6\n%lu 240\n255\n", (unsigned long)sw);
        for (u32 y = 0; y < 240; y++)
            for (u32 x = 0; x < sw; x++) {
                u32 c = px[x * 240 + 239 - y];  // (r << 24 | g << 16 | b << 8 | a)
                u8 rgb[3] = {(u8)(c >> 24), (u8)(c >> 16), (u8)(c >> 8)};
                fwrite(rgb, 1, 3, f);
            }
        fclose(f);
    }
}

// the checks on the screens as drawn
static int sHash = -1;
static void screen_checks(u64 gameFrame) {
    static const char *shots = (const char *)-1;
    if (shots == (const char *)-1) shots = getenv("ACWW_PICA_SHOT");
    // ACWW_PICA_HASH and the wide view: the screens' DS pictures (lines 24-215 of the targets' columns, either way
    // round), as the engines' pictures before
    if (sHash > 0 && sScreenStencil && sScreen[0] && sScreen[1]) {
        u32 hash = 2166136261u;
        for (int s = 0; s < 2; s++) {
            u32 x0 = s == 0 ? (400 - W) / 2 : (320 - W) / 2;
            const u32 *px = screen_pixels(s);
            for (u32 x = x0; px && x < x0 + W; x++)
                for (u32 y = (240 - H) / 2; y < (240 + H) / 2; y++) hash = (hash ^ px[x * 240 + y]) * 16777619u;
        }
        fprintf(stderr, "pica: screens %u (game frame %llu) hash %08lx\n", (unsigned)sFrame, (unsigned long long)gameFrame,
                (unsigned long)hash);
    }
    if (shots) screen_shots(shots, gameFrame);
}

static void frame_job(void *arg);
static struct FrameArgs { void *texture3d; const uint32_t *picture3d; int record; u64 gameFrame; } sFrameArgs[2];
static int sDirectFrame;  // (the last composited frame went straight onto the screens: screen_compose)
void pdisp_frame(void *texture3d, const uint32_t *picture3d) {
    pdisp_wait(PDISP_COMP, 1);  // (the arguments of the compositing before the last)
    sScreenRead[0] = sScreenRead[1] = 0;
    int record = gpu2d_record();
    sFrameArgs[record] = (struct FrameArgs){texture3d, picture3d, record, host_frame()};
    pdisp_submit(PDISP_COMP, frame_job, &sFrameArgs[record]);
}

static void frame_job(void *arg) {
    const struct FrameArgs *a = arg;
    {  // the layers the 2D renderer recorded (render.c: Gpu2dFrameInput.deferLayers)
        HOST_PROF_ON(gPicaProf, PROF_LAYERS);
        gpu2d_draw_layers(a->record);
    }
    HOST_PROF_ON(gPicaProf, PROF_COMP);
    void *texture3d = a->texture3d;
    const uint32_t *picture3d = a->picture3d;
    sFrame++;
    if (sProbe < 0) sProbe = getenv("ACWW_PICA_PROBE") && atoi(getenv("ACWW_PICA_PROBE"));
    sProbeMode = (int)((sFrame - 1) / 300 % PROBES);
    s3dTex = texture3d ? (C3D_Tex *)texture3d : &s3d;
    s3dPremultiplied = texture3d != 0;
    s3dWide = texture3d && gpu3d_wide();
    if (!sMorton[0][1])
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                sMorton[y][x] = (u8)((x & 1) | (y & 1) << 1 | (x & 2) << 1 | (y & 2) << 2 | (x & 4) << 2 | (y & 4) << 3);
    // the layers the CPU drew this frame, to the GPU's textures; the 3D picture as RGBA8 (its colour straight)
    u32 used[2] = {0, 0}, cpuBg[2] = {0, 0};
    for (int e = 0; e < 2; e++)
        for (int y = 0; y < H; y++)
            if (!sLayers.line[e][y].white) used[e] |= sLayers.line[e][y].dispcnt, cpuBg[e] |= ~sLayers.line[e][y].gpuBg & 15u;
    pdisp_frame_open(PDISP_COMP);  // (begun by this frame's 3D job, or here: waits for the GPU's work before)
#ifdef N3DS_PORT
    for (int e = 0; e < 2; e++) {
        u32 *t = sBackdrop[e].data;
        for (int y = 0; y < 256; y++) {
            u16 c = sLayers.line[e][y < H ? y : H - 1].backdrop;
            u32 r = c & 31, g = c >> 5 & 31, b = c >> 10 & 31;
            u32 v = (r * 255 + 15) / 31 << 24 | (g * 255 + 15) / 31 << 16 | (b * 255 + 15) / 31 << 8 | 255;
            for (int x = 0; x < 8; x++) t[y / 8 * 64 + sMorton[y % 8][x]] = v;
        }
        C3D_TexFlush(&sBackdrop[e]);
    }
#endif
    for (int e = 0; e < 2; e++) {
        for (int n = 0; n < 4; n++)
            if ((used[e] & 0x100u << n) && (cpuBg[e] >> n & 1))
                upload(sLayers.bg[e][n], &sBg[e][n], W * H * 2, GX_TRANSFER_FMT_RGB5A1);
        if ((used[e] & 0x1000) && !sLayers.sprite) upload(sLayers.obj[e], &sObj[e], W * H * 4, GX_TRANSFER_FMT_RGBA8);
        if ((used[e] & 0xe000) && !sLayers.sprite) upload(sLayers.region[e], &sRegion[e], W * H * 4, GX_TRANSFER_FMT_RGBA8);
    }
    if (sLayers.sprite && (sLayers.nsprites[0] || sLayers.nsprites[1]) && !probe_skips(1)) {
        // (only the rows the sprites use, from the top, in whole 8-line tile rows: the transfer writes the texture's
        // rows from its start)
        int rows = 8;
        for (int e = 0; e < 2; e++)
            for (int k = 0; k < sLayers.nsprites[e]; k++)
                if (sLayers.sprites[e][k].v + sLayers.sprites[e][k].h > rows) rows = sLayers.sprites[e][k].v + sLayers.sprites[e][k].h;
        rows = (rows + 7) & ~7;
        u32 bytes = GPU2D_ATLAS_W * (u32)rows * 4, dim = GX_BUFFER_DIM(GPU2D_ATLAS_W, rows);
        GSPGPU_FlushDataCache(sLayers.sprite, bytes);
        frame_flush_commands();
        HOST_PROF_ON(gPicaProf, PROF_UPLOAD);
        C3D_SyncDisplayTransfer(sLayers.sprite, dim, sSprites.data, dim,
                                kLinearToTiled | GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                                    GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8));
    }
    if (!texture3d && picture3d && (used[0] & 8)) {
        static u32 a8[32];
        if (!a8[31])
            for (u32 a = 0; a < 32; a++) a8[a] = a * 255 / 31;
        for (int i = 0; i < W * H; i++) {
            u32 c = picture3d[i], r = c & 63, g = c >> 8 & 63, b = c >> 16 & 63;
            s3dStage[i] = (r << 2 | r >> 4) << 24 | (g << 2 | g >> 4) << 16 | (b << 2 | b >> 4) << 8 | a8[c >> 24 & 31];
        }
        upload(s3dStage, &s3d, W * H * 4, GX_TRANSFER_FMT_RGBA8);
    }
#ifdef N3DS_PORT
    if (sLayers.optPic && sLayers.optSerial != sOptSerial) {  // (a new page picture: copied at once, the game's thread may change it)
        sOptSerial = sLayers.optSerial;
        memcpy(sOptStage, sLayers.optPic, W * H * 4);
        upload(sOptStage, &sOpt, W * H * 4, GX_TRANSFER_FMT_RGBA8);
    }
#endif
    sNV = 0;
    if (sStats < 0) sStats = getenv("ACWW_PICA_STATS") && atoi(getenv("ACWW_PICA_STATS"));
    for (int e = 0; sStats && e < 2; e++) {  // (the pixels the layers the CPU drew actually have)
        for (int i = 0; (used[e] & 0x1000) && !sLayers.sprite && i < W * H; i++) sStatObj += sLayers.obj[e][i] != 0;
        for (int n = 0; n < 4; n++)
            for (int i = 0; (used[e] & 0x100u << n) && (cpuBg[e] >> n & 1) && i < W * H; i++)
                sStatBg += sLayers.bg[e][n][i] != 0;
    }
    C3D_BindProgram(&sProgram);
    C3D_AttrInfo *ai = C3D_GetAttrInfo();
    AttrInfo_Init(ai);
    AttrInfo_AddLoader(ai, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(ai, 1, GPU_FLOAT, 4);
    AttrInfo_AddLoader(ai, 2, GPU_FLOAT, 2);
    C3D_BufInfo *bi = C3D_GetBufInfo();
    BufInfo_Init(bi);
    BufInfo_Add(bi, sV, sizeof(DVertex), 3, 0x210);
    C3D_CullFace(GPU_CULL_NONE);
    for (int i = 1; i < 6; i++) C3D_TexEnvInit(C3D_GetTexEnv(i));
    C3D_DepthMap(true, -1.0f, 0.0f);
#ifdef N3DS_PORT
    static int screenMode = -1;  // (ACWW_PICA_SCREEN=0: the engines' pictures first, then copied, as before)
    if (screenMode < 0) screenMode = !getenv("ACWW_PICA_SCREEN") || atoi(getenv("ACWW_PICA_SCREEN"));
    int direct = screenMode && sScreenStencil && sScreen[0] && sScreen[1];  // (screen_compose)
#else
    const int direct = 0;
#endif
    sDirectFrame = direct;
#ifdef N3DS_PORT
    if (!direct) touch_publish(0);
#endif
    for (int e = 0; e < 2 && !direct; e++) {
        C3D_RenderTargetClear(sCompTarget[e], C3D_CLEAR_ALL, 0, 0);
        C3D_FrameDrawOn(sCompTarget[e]);
        colour_writes(1);
        projection(TEX, TEX, 0);
        compose(e);
    }
    // to the screens: 1.25 times, 320x240 (pdisp_scaled), "sharp bilinear": the picture doubled pixel by pixel, then
    // reduced with the linear filter (the DS's pixels stay square and sharp, only their edges are smoothed); else 1:1.
    // In their middle.
    C3D_StencilTest(false, GPU_ALWAYS, 0, 0, 0);
    C3D_AlphaTest(false, GPU_ALWAYS, 0);
    opaque();
    fps_text(a->gameFrame);
    int same[2] = {0, 0};
    for (int s = 0; s < 2 && direct; s++) {
        int e = s == 0 ? sLayers.topEngine : 1 - sLayers.topEngine;
        same[s] = screen_unchanged(s, e, used[e], cpuBg[e], texture3d);
    }
    for (int s = 0; s < 2 && sScreen[0]; s++) {
        int e = s == 0 ? sLayers.topEngine : 1 - sLayers.topEngine;
        if (same[s]) continue;  // (its target keeps the picture, shown again; the bottom's touch moves stay published)
        float sw = s == 0 ? 400.0f : 320.0f;
        C3D_Tex *pic = &sComp[e];
        float pw = W, ph = H, size = TEX;
        if (sUpTarget) {
            C3D_FrameDrawOn(sUpTarget);
            sTag = ST_UP;
            projection(UP, UP, 0);
            source(&sComp[e]);
            begin();
            const float xy[4][2] = {{0, 0}, {2 * W, 0}, {2 * W, 2 * H}, {0, 2 * H}};
            const float uv[4][2] = {{0, 1}, {(float)W / TEX, 1}, {(float)W / TEX, 1 - (float)H / TEX}, {0, 1 - (float)H / TEX}};
            quad(xy, uv, 0);
            end();
            pic = &sUp, pw = 2 * W, ph = 2 * H, size = UP;
        }
        float dw = sUpTarget ? 320.0f : W, dh = sUpTarget ? 240.0f : H, x0 = (sw - dw) / 2, y0 = (240 - dh) / 2;
        if (!probe_skips(32)) C3D_RenderTargetClear(sScreen[s], C3D_CLEAR_ALL, 0x000000ff, 0);
        C3D_FrameDrawOn(sScreen[s]);
        if (direct) {
            screen_compose(e, x0, y0, sw);
#ifdef N3DS_PORT
            if (s == 1) touch_publish(1);
#endif
            C3D_StencilTest(false, GPU_ALWAYS, 0, 0, 0);
            C3D_AlphaTest(false, GPU_ALWAYS, 0);
            opaque();
            if (s == 1) options_page(x0, y0, sw, dw);
            iris(sw);
            if (s == 0) show_fps();
            continue;
        }
        projection(sw, 240.0f, 1);
        if (s3dWide && e == 0 && (used[0] & 8) && margins_3d()) margins(x0, y0);
        else if (sScreenStencil) margins2d(e, x0, y0, sw);
        source(pic);
        opaque();
        sTag = ST_SCREEN;
        begin();
        const float xy[4][2] = {{x0, y0}, {x0 + dw, y0}, {x0 + dw, y0 + dh}, {x0, y0 + dh}};
        const float uv[4][2] = {{0, 1}, {pw / size, 1}, {pw / size, 1 - ph / size}, {0, 1 - ph / size}};
        quad(xy, uv, 0);
        end();
        if (s == 1) options_page(x0, y0, sw, dw);  // (the wide view off: the page there too, to switch it on again)
        iris(sw);
        if (s == 0) show_fps();
    }
    GSPGPU_FlushDataCache(sV, (u32)sNV * sizeof(DVertex));
    frame_close();
    if (sStats) stats_report();
    if (sHash < 0) sHash = getenv("ACWW_PICA_HASH") && atoi(getenv("ACWW_PICA_HASH"));
    if (sHash) {  // (both engines' pictures read back: FNV-1a)
        u32 hash = 2166136261u;
        for (int e = 0; e < 2 && !direct; e++) {
            C3D_SyncDisplayTransfer((u32 *)sComp[e].data, GX_BUFFER_DIM(TEX, TEX), sReadback, GX_BUFFER_DIM(TEX, TEX),
                                    kTiledToLinear);
            GSPGPU_InvalidateDataCache(sReadback, TEX * TEX * 4);
            for (int i = 0; i < W * H; i++) hash = (hash ^ sReadback[i]) * 16777619u;
        }
        if (!direct) fprintf(stderr, "pica: picture %u hash %08lx\n", (unsigned)sFrame, (unsigned long)hash);
    }
    screen_checks(a->gameFrame);
}

const uint32_t *pdisp_screen(int screen) {
    pdisp_idle();  // (citro3d and the pictures)
    int s = screen & 1, e = s == 0 ? sLayers.topEngine : 1 - sLayers.topEngine;
    if (!sV) return sScreenPx[s];
    if (!sScreenRead[s] && sDirectFrame) {  // (screen_compose: the DS's picture from the middle of the screen)
        const u32 *px = screen_pixels(s);
        u32 x0 = s == 0 ? (400 - W) / 2 : (320 - W) / 2, y0 = (240 - H) / 2;
        for (int i = 0; px && i < W * H; i++) sScreenPx[s][i] = px[(x0 + (u32)(i % W)) * 240 + 239 - y0 - (u32)(i / W)] >> 8;
        sScreenRead[s] = 1;
    }
    if (!sScreenRead[s]) {
        C3D_SyncDisplayTransfer((u32 *)sComp[e].data, GX_BUFFER_DIM(TEX, TEX), sReadback, GX_BUFFER_DIM(TEX, TEX),
                                kTiledToLinear);
        GSPGPU_InvalidateDataCache(sReadback, TEX * TEX * 4);
        for (int i = 0; i < W * H; i++) sScreenPx[s][i] = sReadback[i] >> 8;  // (RGBA8: r << 24 | g << 16 | b << 8)
        sScreenRead[s] = 1;
    }
    return sScreenPx[s];
}

#endif

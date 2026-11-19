// Host build: the screens composited and shown by the 3DS's GPU (pica_display.h): the backend of the console ports'
// compositing (comp2d.c, which has the logic: what is drawn, how the DS's layers, windows and effects are put
// together) on citro3d: its textures, uploads, draws and targets (comp2d.h cg_*), the GPU thread on the New 3DS's
// core 2, the citro3d frames shared with the 3D picture (gpu3d_pica.c), the 3DS's screens.
#include "pica_display.h"
#include "prof.h"
#include "comp2d.h"

#if defined(__SWITCH__)
// (the Switch: gl_display.c)
#elif !defined(__3DS__)

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

#include "gpu3d/gpu3d.h"

unsigned long long host_frame(void);  // (host.h, whose u32 isn't libctru's)
void host_plat_log_write(void);       // (plat.h: the port's log written out now)

extern const u8 pica_display_shbin[];  // (the vertex shader, pica_display.v.pica: picasso and bin2s)
extern const u32 pica_display_shbin_size;

enum { W = 256, H = 192, TEX = 256, UP = 512 };

// a texture (comp2d.h): the GPU's, and where it is in VRAM, the copy the CPU writes (CG_STAGED). The 3D renderer's
// pictures (gpu3d_pica.c sPic) are plain C3D_Tex: cg_tex_picture takes one as a CgTex, of which only `tex` is read.
struct CgTex {
    C3D_Tex tex;
    C3D_Tex stage;
    int staged;
};

static DVLB_s *sDvlb;
static shaderProgram_s sProgram;
static int sUProjection;
static CgVertex *sV;

static CgTex sComp[2];                 // each engine's picture
static C3D_RenderTarget *sCompTarget[2], *sScreen[2];
static int sScreenStencil;             // the screens' targets have a stencil buffer (the wide view's)
static CgTex sUp;                      // a picture doubled (the screens' sharp scaling; RGB8, VRAM)
static C3D_RenderTarget *sUpTarget;
static u32 *sReadback;

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
    int core = 2;
    sWorker = threadCreate(worker, 0, 64 * 1024, prio, core, true);
    // ACWW_PICA_THREAD=1 on an Old 3DS (a trial, not the default): core 1 at ACWW_CORE1_LIMIT percent (80 unless set;
    // the sound's thread shares it, plat_3ds.c sets the same share). shortcut: unmeasured; make it the Old 3DS's
    // default only if the console's log shows more fps and no sound running dry
    if (!sWorker && e && atoi(e) == 1) {
        if (!getenv("ACWW_CORE1_LIMIT")) setenv("ACWW_CORE1_LIMIT", "80", 0);
        int pct = atoi(getenv("ACWW_CORE1_LIMIT"));
        Result r = APT_SetAppCpuTimeLimit((u32)(pct < 5 ? 5 : pct > 89 ? 89 : pct));
        fprintf(stderr, "3ds: APT_SetAppCpuTimeLimit(%d) for the GPU work: %08lx\n", pct, (unsigned long)r);
        sWorker = threadCreate(worker, 0, 64 * 1024, prio, core = 1, true);
    }
    if (sWorker) gPicaProf = &gHostProfWorker;
    if (sWorker) fprintf(stderr, "3ds: the GPU work on core %d\n", core);
    else fprintf(stderr, "3ds: the GPU work on the game's core (no core 2)\n");
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
// The data cache written back so that the GPU reads what the CPU wrote: by the kernel on the calling core
// (svcStoreProcessDataCache), not by GSPGPU_FlushDataCache, a request to the system's GPU service on core 1, where
// the sound's thread is too (old3ds-test 78: the two backdrop textures' flushes, 8 KB each, 0.67 ms a composited
// frame, the vertices' 0.60). ACWW_PICA_FLUSH=0: the GSP's, as before; so too if the kernel refuses (logged once).
void pdisp_flush(const void *p, u32 size) {
    static int mode = -1;
    if (mode < 0) mode = !getenv("ACWW_PICA_FLUSH") || atoi(getenv("ACWW_PICA_FLUSH"));
    if (mode && size) {
        Result r = svcStoreProcessDataCache(CUR_PROCESS_HANDLE, (u32)p, size);
        if (R_SUCCEEDED(r)) return;
        fprintf(stderr, "pica: svcStoreProcessDataCache: %08lx; the GSP flushes the data cache\n", (unsigned long)r);
        mode = 0;
    }
    GSPGPU_FlushDataCache(p, size);
}
static void tex_flush(C3D_Tex *t) { pdisp_flush(t->data, C3D_TexCalcTotalSize(t->size, t->maxLevel)); }
// ACWW_PICA_TIMES=1: the wall-clock time of the compositing's calls that go through the GSP (the system's GPU service,
// on core 1 beside the sound), ms a composited frame and calls, every 120 composited frames. The ACWW_FPS profile's
// sampler runs on core 1 too, so its parts can't show this (old3ds-test 77: compose 0.76 ms less, the compositing's
// wall time the same)
enum { PT_BEGIN, PT_UPLOAD, PT_WRITTEN, PT_STOP, PT_FEND, PT_FLUSH, PT_JOB, PT_N };
static int sTimes = -1;
static u64 sTimeSum[PT_N], sTimeJob;
static u32 sTimeCalls[PT_N];
static u64 times_now(void) { return sTimes > 0 ? svcGetSystemTick() : 0; }
static void times_add(int k, u64 t0) {
    if (sTimes <= 0) return;
    sTimeSum[k] += svcGetSystemTick() - t0;
    sTimeCalls[k]++;
}
static void times_report(void) {
    static const char *const kName[PT_N] = {"frame begin", "uploads", "textures written", "waiting for the GPU",
                                            "C3D_FrameEnd", "vertices flushed", "begin to end"};
    if (sTimes <= 0 || sTimeCalls[PT_JOB] < 120) return;
    char line[400];
    int n = snprintf(line, sizeof line, "pica times, ms a composited frame (calls):");
    for (int k = 0; k < PT_N && n < (int)sizeof line; k++)
        n += snprintf(line + n, sizeof line - (size_t)n, " %s %.2f (%lu)", kName[k],
                      (double)sTimeSum[k] / SYSCLOCK_ARM11 * 1000 / sTimeCalls[PT_JOB], (unsigned long)sTimeCalls[k]);
    fprintf(stderr, "%s\n", line);
    memset(sTimeSum, 0, sizeof sTimeSum), memset(sTimeCalls, 0, sizeof sTimeCalls);
}
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
        if (comp2d_probe() >= 0) fprintf(stderr, " (probe %d)", comp2d_probe());
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
#ifdef ENHANCED_PORT
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
#ifndef ENHANCED_PORT
        merge = 0;  // (the 3DS build as it was)
#endif
#ifdef ENHANCED_PORT
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
    pdisp_flush(start, (u32)(gpuCmdBuf - start) * 4);
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
        u64 t0 = times_now();
        gxCmdQueueWait(q, -1);
        times_add(PT_STOP, t0);
    }
    gxCmdQueueStop(q);  // (not running: the queue let go at once)
    sRunning = 0;
}
static void frame_close(void) {
    if (!sOpen) return;
    frame_stop();
    {
        HOST_PROF_ON(gPicaProf, PROF_FEND);
        u64 t0 = times_now();
        C3D_FrameEnd(0);
        times_add(PT_FEND, t0);
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

int pdisp_init(void) {
    if (sV) return 1;
    pdisp_gsp_init();  // (as gpu3d_pica.c: C3D_Init there, or here first)
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 4);
    sReadback = linearAlloc(TEX * TEX * 4);
    sDvlb = DVLB_ParseFile((u32 *)pica_display_shbin, pica_display_shbin_size);
    if (!sReadback || !sDvlb) return 0;
    shaderProgramInit(&sProgram);
    shaderProgramSetVsh(&sProgram, &sDvlb->DVLE[0]);
    sUProjection = shaderInstanceGetUniformLocation(sProgram.vertexShader, "projection");
    return comp2d_init();  // (its textures: cg_tex_new below; the engines' pictures cg_target_tex)
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
    if (pdisp_scaled() && C3D_TexInitVRAM(&sUp.tex, UP, UP, GPU_RGB8)) {  // (render targets are in VRAM: 768 KB)
        C3D_TexSetFilter(&sUp.tex, GPU_LINEAR, GPU_LINEAR);
        sUpTarget = C3D_RenderTargetCreateFromTex(&sUp.tex, GPU_TEXFACE_2D, 0, -1);
    }
    sScreenStencil = gpu3d_wide_wanted();  // (margins2d)
    sScreen[0] = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, sScreenStencil ? GPU_RB_DEPTH24_STENCIL8 : -1);
    sScreen[1] = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, sScreenStencil ? GPU_RB_DEPTH24_STENCIL8 : -1);
    if (sScreen[0]) C3D_RenderTargetSetOutput(sScreen[0], GFX_TOP, GFX_LEFT, out);
    if (sScreen[1]) C3D_RenderTargetSetOutput(sScreen[1], GFX_BOTTOM, GFX_LEFT, out);
    fprintf(stderr, "pica: %lu KB of VRAM free%s%s\n", (unsigned long)(vramSpaceFree() / 1024),
            sScreen[0] && sScreen[1] ? "" : " (no screen targets)", pdisp_scaled() && !sUpTarget ? " (not scaled: no VRAM)" : "");
}



// ---- comp2d.h's backend: citro3d
CgTex *cg_tex_new(int w, int h, int fmt, int where) {
    CgTex *t = calloc(1, sizeof *t);
    if (!t) return 0;
    GPU_TEXCOLOR f = fmt == CG_RGBA5551 ? GPU_RGBA5551 : GPU_RGBA8;
    if (where == CG_STAGED) {  // (in VRAM, which the GPU reads faster, with a copy in linear memory the CPU writes)
        t->staged = C3D_TexInitVRAM(&t->tex, (u16)w, (u16)h, f);
        if (t->staged && !C3D_TexInit(&t->stage, (u16)w, (u16)h, f)) C3D_TexDelete(&t->tex), t->staged = 0;
        if (!t->staged && !C3D_TexInit(&t->tex, (u16)w, (u16)h, f)) { free(t); return 0; }
        C3D_TexSetFilter(&t->tex, GPU_NEAREST, GPU_NEAREST);
        t->tex.border = 0;  // (an affine BG without wrap: transparent outside)
        return t;
    }
    // (CG_VRAM: in VRAM when it fits, the layers: the GPU reads its own memory much faster than the main memory, and
    // the compositing reads them over and over; else in linear memory)
    if (!(where == CG_VRAM && C3D_TexInitVRAM(&t->tex, (u16)w, (u16)h, f)) && !C3D_TexInit(&t->tex, (u16)w, (u16)h, f)) {
        free(t);
        return 0;
    }
    C3D_TexSetFilter(&t->tex, GPU_NEAREST, GPU_NEAREST);
    C3D_TexSetWrap(&t->tex, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    return t;
}
void cg_tex_free(CgTex *t) {
    C3D_TexDelete(&t->tex);
    if (t->staged) C3D_TexDelete(&t->stage);
    free(t);
}
void *cg_tex_texels(CgTex *t) { return t->staged ? t->stage.data : t->tex.data; }
void cg_tex_written(CgTex *t) {
    if (!t->staged) {
        u64 t0 = times_now();
        tex_flush(&t->tex);
        times_add(PT_WRITTEN, t0);
        return;
    }
    tex_flush(&t->stage);  // (the GPU copies it to VRAM: C3D_SyncTextureCopy)
    if (sRunning && gpuCmdBufOffset) frame_stop();  // (it queues the commands before it unflushed: frame_stop)
    C3D_TexLoadImage(&t->tex, t->stage.data, GPU_TEXFACE_2D, 0);
}
void cg_tex_repeat(CgTex *t, int on) {
    C3D_TexSetWrap(&t->tex, on ? GPU_REPEAT : GPU_CLAMP_TO_BORDER, on ? GPU_REPEAT : GPU_CLAMP_TO_BORDER);
}
// (a transfer into the texture's tiles, by the GPU)
void cg_tex_upload(CgTex *t, const void *src, int w, int rows, int fmt) {
    GX_TRANSFER_FORMAT f = fmt == CG_RGBA5551 ? GX_TRANSFER_FMT_RGB5A1 : GX_TRANSFER_FMT_RGBA8;
    u32 dim = GX_BUFFER_DIM(w, rows);
    u64 t0 = times_now();
    pdisp_flush(src, (u32)(w * rows * (fmt == CG_RGBA5551 ? 2 : 4)));
    frame_flush_commands();
    HOST_PROF_ON(gPicaProf, PROF_UPLOAD);
    C3D_SyncDisplayTransfer((u32 *)src, dim, t->tex.data, dim, kLinearToTiled | GX_TRANSFER_IN_FORMAT(f) | GX_TRANSFER_OUT_FORMAT(f));
    times_add(PT_UPLOAD, t0);
}
CgTex *cg_tex_picture(void *texture3d) { return (CgTex *)texture3d; }  // (a C3D_Tex: its `tex`, the same pointer)
void *cg_alloc(uint32_t bytes) { return linearAlloc(bytes); }

// the fragment colour: the texture's or the vertex colour
void cg_source(CgTex *t) {
    C3D_TexEnv *e = C3D_GetTexEnv(0);
    C3D_TexEnvInit(e);
    C3D_TexEnvSrc(e, C3D_Both, t ? GPU_TEXTURE0 : GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(e, C3D_Both, GPU_REPLACE);
    if (t) C3D_TexBind(0, &t->tex);
}
void cg_blend(int mode, uint32_t eva, uint32_t evb) {
    if (mode == CG_CONSTANT) {  // (the constant colour's red, green and blue are eva / 16, its alpha evb / 16: R in the low byte)
        u32 ca = eva * 255 / 16, cb = evb * 255 / 16;
        C3D_BlendingColor(ca | ca << 8 | ca << 16 | cb << 24);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_CONSTANT_COLOR, GPU_CONSTANT_ALPHA, GPU_ONE, GPU_ZERO);
        return;
    }
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, mode == CG_ALPHA ? GPU_SRC_ALPHA : GPU_ONE,
                   mode == CG_OPAQUE ? GPU_ZERO : GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ZERO);
}
void cg_alpha_test(int func, int ref) {
    if (func == CG_OFF) C3D_AlphaTest(false, GPU_ALWAYS, 0);
    else C3D_AlphaTest(true, func == CG_EQUAL ? GPU_EQUAL : func == CG_GREATER ? GPU_GREATER : GPU_ALWAYS, ref);
}
void cg_stencil(int func, uint32_t ref, uint32_t mask, uint32_t write) {
    if (func == CG_OFF) {
        C3D_StencilTest(false, GPU_ALWAYS, 0, 0, 0);
        return;
    }
    C3D_StencilTest(true, func == CG_EQUAL ? GPU_EQUAL : GPU_ALWAYS, (int)ref, (int)mask, (int)write);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_REPLACE);
}
void cg_colour_writes(int on) { C3D_DepthTest(false, GPU_ALWAYS, on ? GPU_WRITE_COLOR : (GPU_WRITEMASK)0); }
void cg_ortho(float left, float right, float bottom, float top, int tilt) {
    C3D_Mtx m;
    if (tilt) Mtx_OrthoTilt(&m, left, right, bottom, top, 0.0f, 1.0f, true);  // (on a turned 3DS screen)
    else Mtx_Ortho(&m, left, right, bottom, top, 0.0f, 1.0f, true);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sUProjection, &m);
}
// (the box in the turned target's pixels: x and y swapped)
void cg_scissor_out(int on, int x0, int y0, int x1, int y1) {
    if (on) C3D_SetScissor(GPU_SCISSOR_INVERT, (u32)y0, (u32)x0, (u32)y1, (u32)x1);
    else C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, 0, 0);
}
CgVertex *cg_vertices(void) {
    if (!sV) sV = linearAlloc(CG_MAX_VERTS * sizeof(CgVertex));
    return sV;
}
void cg_draw(int first, int count) { C3D_DrawArrays(GPU_TRIANGLES, first, count); }

int cg_target(int which, int clear) {
    C3D_RenderTarget *t = which <= CG_COMP1 ? sCompTarget[which - CG_COMP0] : which == CG_UP ? sUpTarget
                        : sScreen[which - CG_SCREEN0];
    if (!t) return 0;
    if (clear) C3D_RenderTargetClear(t, C3D_CLEAR_ALL, which <= CG_COMP1 ? 0 : 0x000000ff, 0);
    C3D_FrameDrawOn(t);
    return 1;
}
// (an engine's picture made at the first ask, comp2d_init's: in VRAM, with a stencil buffer)
CgTex *cg_target_tex(int which) {
    if (which == CG_UP) return sUpTarget ? &sUp : 0;
    int e = which - CG_COMP0;
    if (!sCompTarget[e]) {
        if (!C3D_TexInitVRAM(&sComp[e].tex, TEX, TEX, GPU_RGBA8)) return 0;
        C3D_TexSetFilter(&sComp[e].tex, GPU_NEAREST, GPU_NEAREST);
        sCompTarget[e] = C3D_RenderTargetCreateFromTex(&sComp[e].tex, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH24_STENCIL8);
        if (!sCompTarget[e]) return 0;
    }
    return &sComp[e];
}
int cg_screens(void) { return sScreen[0] && sScreen[1]; }
int cg_screen_stencil(void) { return sScreenStencil; }

void cg_frame_begin(void) {  // (begun by this frame's 3D job, or here)
    if (sTimes < 0) sTimes = getenv("ACWW_PICA_TIMES") && atoi(getenv("ACWW_PICA_TIMES"));
    sTimeJob = times_now();
    u64 t0 = sTimeJob;
    pdisp_frame_open(PDISP_COMP);
    times_add(PT_BEGIN, t0);
}
void cg_draw_begin(void) {
    C3D_BindProgram(&sProgram);
    C3D_AttrInfo *ai = C3D_GetAttrInfo();
    AttrInfo_Init(ai);
    AttrInfo_AddLoader(ai, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(ai, 1, GPU_FLOAT, 4);
    AttrInfo_AddLoader(ai, 2, GPU_FLOAT, 2);
    C3D_BufInfo *bi = C3D_GetBufInfo();
    BufInfo_Init(bi);
    BufInfo_Add(bi, sV, sizeof(CgVertex), 3, 0x210);
    C3D_CullFace(GPU_CULL_NONE);
    for (int i = 1; i < 6; i++) C3D_TexEnvInit(C3D_GetTexEnv(i));
    C3D_DepthMap(true, -1.0f, 0.0f);
}
void cg_frame_end(int nverts) {
    u64 t0 = times_now();
    pdisp_flush(sV, (u32)nverts * sizeof(CgVertex));
    times_add(PT_FLUSH, t0);
    frame_close();
    times_add(PT_JOB, sTimeJob);
    times_report();
}

// screen s's picture as drawn (its target read back: 240 x its width, a turned target's rows being the screen's
// columns from the left, its columns the lines from the bottom; RGBA8), turned back to rows. Once the GPU has drawn
// it: an empty frame's C3D_FrameBegin waits for that (a read right after C3D_FrameEnd raced the GPU; one in a frame
// would only be queued)
const uint32_t *cg_screen_read(int s) {
    static u32 *px, rows[400 * 240];
    u32 sw = s == 0 ? 400 : 320;
    if (!sScreen[s] || !(px || (px = linearAlloc(240 * 400 * 4)))) return 0;
    C3D_FrameBegin(0);
    C3D_FrameEnd(0);
    C3D_SyncDisplayTransfer((u32 *)sScreen[s]->frameBuf.colorBuf, GX_BUFFER_DIM(240, sw), px, GX_BUFFER_DIM(240, sw),
                            kTiledToLinear);
    GSPGPU_InvalidateDataCache(px, 240 * sw * 4);
    for (u32 y = 0; y < 240; y++)
        for (u32 x = 0; x < sw; x++) rows[y * sw + x] = px[x * 240 + 239 - y];
    return rows;
}
const uint32_t *cg_comp_read(int e) {
    C3D_SyncDisplayTransfer((u32 *)sComp[e].tex.data, GX_BUFFER_DIM(TEX, TEX), sReadback, GX_BUFFER_DIM(TEX, TEX),
                            kTiledToLinear);
    GSPGPU_InvalidateDataCache(sReadback, TEX * TEX * 4);
    return sReadback;
}
uint64_t cg_time_ms(void) { return osGetTime(); }

#endif

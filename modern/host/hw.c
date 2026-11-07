// Host build: the DS memory map and the ARM9 I/O registers.
//
// Nothing is mapped at a DS address: the game's variables are ordinary data and bss, and the DS memory the game
// reaches by address is host objects (include/host_mem.h: the system work area, DTCM, ITCM, palettes, OAM, VRAM
// through vram.c's windows, the main RAM arena). The I/O registers are plain host arrays (host_io). The game reaches them only through HostIo_Read / HostIo_Write
// (include/nitro/io_access.h), which carry out the access and its side effects. Nothing is mapped at the DS's I/O
// addresses: a stray access there crashes (debug_posix.c reports where).
//
// Time is virtual: a cycle counter (ARM9 bus clock) that advances on what the game can observe of time - reads of the
// display status and the timers, polling loops, busy waits (io_read_cost) - and jumps to the next event when the game
// halts or waits; other register accesses take no time (docs/host.md, "Virtual time"). Interrupts are taken only at safe points:
// after a register access, and when the game enables interrupts, halts or waits (os.c).

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__x86_64__) && defined(__linux__)
#include <fcntl.h>     // (ACWW_HIGHMEM: /proc/self/maps, the reservations)
#include <sys/mman.h>
#include <unistd.h>
#endif

#include "host.h"
#include "host_mem.h"
#include "plat.h"
#include "guards.h"
#include "render.h"

// 3D command port writes since the last ACWW_STATS line (register writes and GX FIFO DMA)
static unsigned long long sGxWrites;
static u64 sIoCount;  // register accesses

#define IO0 0x04000000u
#define IOB 0x04001000u  // engine B's display registers
#define IO1 0x04100000u
#define PAGE 0x1000u

// the register pages: 0x04000000 (engine A's display registers and the rest), 0x04001000 (engine B's display
// registers), 0x04100000 (the IPC receive FIFO, card data)
static u8 sIo0[PAGE] __attribute__((aligned(8)));
static u8 sIoB[PAGE] __attribute__((aligned(8)));
static u8 sIo1[PAGE] __attribute__((aligned(8)));
// (a pointer the compiler can't see through: it would otherwise fold `page + (a - IO0)` into the constant
// `page - IO0` plus a, an address outside the program, which a 3DS program's relocations (.3dsx) can't express)
static u8 *opaque(u8 *p) {
    __asm__("" : "+r"(p));
    return p;
}
u8 *host_io(u32 a) {
    return a >= IO1 ? opaque(sIo1) + (a - IO1) : a >= IOB ? opaque(sIoB) + (a - IOB) : opaque(sIo0) + (a - IO0);
}
static int is_io(u32 a) { return (a >= IO0 && a < IOB + PAGE) || (a >= IO1 && a < IO1 + PAGE); }

// vram.c
void host_vram_control(u32 addr, u8 value);
#define R16(a) (*(u16 *)host_io(a))
#define R32(a) (*(u32 *)host_io(a))

// ---- time
#define LINE_CYCLES 2130u
#define FRAME_LINES 263u
#define FRAME_CYCLES (LINE_CYCLES * FRAME_LINES)
static u64 sCycles;
static u32 sTimeless;  // register accesses since time last moved (a spin without time: see io_spin_check)
u64 host_frame(void) { return sCycles / FRAME_CYCLES; }
u64 host_cycles(void) { return sCycles; }
u64 pxi7_next_due(void);
void pxi7_run_due(void);

// ---- interrupts
static u32 sIF;
void host_irq_raise(u32 m) { sIF |= m; }
void host_irq_ack(u32 m) { sIF &= ~m; }
u32 host_irq_pending(void) {
    // (asked after every register access: the GX FIFO line, level-triggered, only when it is enabled)
    u32 enabled = (R32(0x04000208) & 1) ? R32(0x04000210) : 0;
    if (!enabled) return 0;
    u32 lines = sIF | ((enabled & 1u << 21) && render_gx_fifo_irq_level() ? 1u << 21 : 0);
    return enabled & lines;
}

// ---- timers (0x04000100 + 4n: counter/reload u16, control u16)
typedef struct { u64 start; u16 reload; int shift; int on; int irq; int cascade; } Timer;
static Timer sTm[4];
static const int kTmShift[4] = {0, 6, 8, 10};
static u32 tm_period(Timer *t) { return (0x10000u - t->reload) << t->shift; }
static u16 tm_counter(Timer *t) {
    if (!t->on) return t->reload;
    u64 ticks = (sCycles - t->start) >> t->shift;
    return (u16)(t->reload + ticks % (0x10000u - t->reload));
}
static u64 tm_next_overflow(Timer *t) {
    u64 p = tm_period(t), done = (sCycles - t->start) / p;
    return t->start + (done + 1) * p;
}

// ---- ACWW_FPS's profile (host.h): the game's thread marks the part it is in, a sampler thread counts them every
// millisecond; with the fps line, each part's share of the real time as milliseconds a frame
// The background threads (the watchdog, ACWW_FPS's sampler) stop before the program ends (host_quit): the 3DS frees
// the heap, their stacks too, before it ends the process, and a thread waking in between faults
static int sThreadsQuit, sThreadsRunning;  // (atomics)
static int threads_quitting(void) { return __atomic_load_n(&sThreadsQuit, __ATOMIC_ACQUIRE); }
static void thread_stopped(void) { __atomic_sub_fetch(&sThreadsRunning, 1, __ATOMIC_RELEASE); }
static int thread_start(void (*fn)(void *), void *arg, int sampler) {
    __atomic_add_fetch(&sThreadsRunning, 1, __ATOMIC_RELAXED);
    if (sampler ? host_plat_sampler_start(fn, arg, sampler == 2) : host_plat_thread_start(fn, arg)) return 1;
    thread_stopped();
    return 0;
}
int host_thread_start(void (*fn)(void *), void *arg, int otherCore) { return thread_start(fn, arg, otherCore); }
int host_threads_quitting(void) { return threads_quitting(); }
void host_thread_stopped(void) { thread_stopped(); }
static void threads_stop(void) {
    __atomic_store_n(&sThreadsQuit, 1, __ATOMIC_RELEASE);
    for (int i = 0; i < 50 && __atomic_load_n(&sThreadsRunning, __ATOMIC_ACQUIRE); i++) host_plat_sleep_ms(5);
}

volatile int gHostProf, gHostProfWorker = PROF_IDLE;
static u32 sProfSamples[2][PROF_N];  // the game's thread, the GPU thread (written by the sampler only)
static void prof_sampler(void *arg) {
    (void)arg;
    while (!threads_quitting()) {
        host_plat_sleep_ms(1);
        int p = gHostProf, w = gHostProfWorker;
        __atomic_store_n(&sProfSamples[0][p], sProfSamples[0][p] + 1, __ATOMIC_RELAXED);
        __atomic_store_n(&sProfSamples[1][w], sProfSamples[1][w] + 1, __ATOMIC_RELAXED);
    }
    thread_stopped();
}
static const char *const kProfNames[PROF_N] = {"game", "registers", "DMA", "geometry", "2D", "3D", "compositing",
                                               "sound", "frontend", "pacing", "GPU wait", "lighting",
                                               "vertices", "waiting for the GPU thread", "idle", "ROM",
                                               "ARM7", "uploads", "VRAM", "BG tiles", "top hash", "frame end"};
// the slowest frame of the 120: the parts the sampler saw in it (milliseconds), the game's thread and the GPU thread
static u32 sProfAt[2][PROF_N], sProfSlowest[2][PROF_N];
static void prof_frame(int slowest) {
    for (int t = 0; t < 2; t++)
        for (int i = 0; i < PROF_N; i++) {
            u32 v = __atomic_load_n(&sProfSamples[t][i], __ATOMIC_RELAXED);
            if (slowest) sProfSlowest[t][i] = v - sProfAt[t][i];
            sProfAt[t][i] = v;
        }
}
// (a line is put together first and written at once: the GPU thread's lines would come between the pieces)
static void prof_slowest_report(void) {
    char line[512];
    int n = 0;
    for (int t = 0; t < 2; t++) {
        n += snprintf(line + n, sizeof line - n, t ? "; the GPU thread:" : "the slowest frame's parts (ms):");
        for (int i = 0; i < PROF_N; i++)
            if (sProfSlowest[t][i] >= 2) n += snprintf(line + n, sizeof line - n, " %s %u", kProfNames[i], sProfSlowest[t][i]);
    }
    fprintf(stderr, "%s\n", line);
}
static void prof_report(double frameMs) {
    const char *const *kNames = kProfNames;
    static u32 seen[2][PROF_N];
    for (int t = 0; t < 2; t++) {
        u32 n[PROF_N], total = 0;
        for (int i = 0; i < PROF_N; i++) {
            n[i] = __atomic_load_n(&sProfSamples[t][i], __ATOMIC_RELAXED) - seen[t][i];
            seen[t][i] += n[i];
            total += n[i];
        }
        if (!total || (t == 1 && n[PROF_IDLE] == total)) continue;  // (no GPU thread: always idle)
        char line[512];
        int k = snprintf(line, sizeof line, "profile%s: %u samples, ms a frame:", t ? ", the GPU thread" : "", total);
        for (int i = 0; i < PROF_N; i++)
            if (t == 0 ? i != PROF_IDLE && i < PROF_TILES
                       : (i == PROF_2D || i == PROF_3D || i == PROF_COMP || i == PROF_GPU || i == PROF_UPLOAD || i == PROF_IDLE ||
                          i == PROF_VERTEX || i == PROF_VRAM || i == PROF_LIGHT || i >= PROF_TILES))  // (the 3DS's jobs' parts)
                k += snprintf(line + k, sizeof line - k, " %s %.1f", kNames[i], frameMs * n[i] / total);
        fprintf(stderr, "%s\n", line);
    }
}

// ---- real time: one frame per 1/60 s unless ACWW_FAST is set
static int sFast = -1;
static u64 sNextFrame;  // host_plat_time_ns
static void host_pace_frame(void) {
    HOST_PROF(PROF_PACE);
    // ACWW_FAST=1: never paced; ACWW_FAST=<frame> (> 1): not paced until that frame (a test gets somewhere quickly,
    // then runs in real time - the wireless tests: two copies keep each other's pace)
    if (sFast < 0) sFast = getenv("ACWW_FAST") ? (atoi(getenv("ACWW_FAST")) > 1 ? atoi(getenv("ACWW_FAST")) : 1) : 0;
    if (sFast == 1 || host_frame() < (u64)sFast) return;
    u64 now = host_plat_time_ns();
    if (sNextFrame == 0) sNextFrame = now;
    sNextFrame += 16715000;                                  // 59.83 Hz
    if (now > sNextFrame + 1000000000u) sNextFrame = now;  // far behind: don't try to catch up
    host_plat_sleep_until_ns(sNextFrame);
}

// ---- watchdog: a background thread that reports (only reports) when virtual time stops moving. The game waiting
// for an interrupt handler by spinning on memory, without a register access, a halt or a wait, would hang here (on
// the DS the interrupt arrives anyway). No such loop is known; a reported one wants an explicit wait at the site
// (OS_Halt, or a call that advances time and takes the interrupts). Nothing is done to the game's thread.
// ACWW_WATCHDOG=<ms>: the threshold in real time (default 2000), 0 turns it off.
static u32 sHeartbeat, sHeartbeatFrame;  // the low word of the cycle counter and the frame (relaxed atomics)
static int sPaused;  // host_set_paused: the frontend holds the game (its key binding screen)
void host_set_paused(int paused) {
    // (the pause's real time isn't owed: the frames go on at their pace from now, not faster to catch up)
    if (!paused && __atomic_load_n(&sPaused, __ATOMIC_RELAXED)) sNextFrame = 0;
    __atomic_store_n(&sPaused, paused, __ATOMIC_RELAXED);
}
static void watchdog(void *arg) {
    u32 limit = (u32)(uintptr_t)arg, still = 0, last = 0;
    int reported = 0;
    while (!threads_quitting()) {
        host_plat_sleep_ms(100);
        u32 hb = __atomic_load_n(&sHeartbeat, __ATOMIC_RELAXED);
        if (hb != last || __atomic_load_n(&sPaused, __ATOMIC_RELAXED)) { last = hb; still = 0; reported = 0; continue; }
        still += 100;
        if (still >= limit && !reported) {
            reported = 1;
            fprintf(stderr, "host: watchdog: virtual time hasn't moved for %u ms (frame %u): the game spins without a "
                    "register access or a wait (break in with a debugger; SIGINT lists the threads)\n", still,
                    __atomic_load_n(&sHeartbeatFrame, __ATOMIC_RELAXED));
        }
    }
    thread_stopped();
}
#ifdef N3DS_DIRECT
// The enhanced port's log: stderr goes into memory (plat_3ds.c), and this thread writes it out
// four times a second: a write to the 3DS's SD card took 20-140 ms, now and then 10 s. What a crash leaves unwritten
// is lost.
static void log_writer(void *arg) {
    (void)arg;
    while (!threads_quitting()) {
        for (int i = 0; i < 25 && !threads_quitting(); i++) host_plat_sleep_ms(10);  // (threads_stop waits 250 ms)
        host_plat_log_write();
    }
    host_plat_log_write();
    thread_stopped();
}
#endif
static void watchdog_start(void) {
    const char *e = getenv("ACWW_WATCHDOG");
    long ms = e ? strtol(e, 0, 10) : 2000;
    if (ms <= 0) return;
    if (!thread_start(watchdog, (void *)(uintptr_t)ms, 0)) fprintf(stderr, "host: no watchdog thread\n");
}

// ---- screenshots: ACWW_SHOT="frame:path.ppm;..." (both screens, 256x384)
static void host_screenshot(void) {
    static const char *spec;
    if (!spec) spec = getenv("ACWW_SHOT") ? getenv("ACWW_SHOT") : "";
    u32 f = (u32)host_frame();
    for (const char *p = spec; *p;) {
        unsigned fr; char path[512];
        if (sscanf(p, "%u:%511[^;]", &fr, path) == 2 && fr == f) {
            FILE *o = fopen(path, "wb");
            if (o) {
                fprintf(o, "P6\n256 384\n255\n");
                for (int s = 0; s < 2; s++) {
                    const uint32_t *px = render_screen(s);
                    for (int i = 0; i < 256 * 192; i++) { u8 rgb[3] = {px[i] >> 16, px[i] >> 8, px[i]}; fwrite(rgb, 1, 3, o); }
                }
                fclose(o);
            }
        }
        p = strchr(p, ';');
        if (!p) break;
        p++;
    }
}

// ---- the main RAM image for the debug dumps (ACWW_RAMHASH, ACWW_RAMDUMP): 4 MB in the shape of the DS's main RAM
// from 0x02000000 - the game's data from offset 0x1000 and its bss from 0x40000 (the places the DS-address link of
// gdata/gbss gave them until docs/portability.md 3.3; the bss further up if the data is larger), the arena block
// (gHostMainRam, 0x02200000-0x023fffff) and the system work area as main RAM's last 4 KB (its first 4 KB on 64-bit
// hosts, whose layout differs). What doesn't fit below the arena is left out (reported once).
static void ram_image_put(u8 *img, u32 *at, const char *lo, const char *end, u32 limit) {
    size_t n = (size_t)(end - lo);
    static int warned;
    if (*at + n > limit) {
        if (!warned++) fprintf(stderr, "host: RAM image: the game's data and bss don't fit, %zu bytes cut\n", *at + n - limit);
        n = *at < limit ? limit - *at : 0;
    }
    memcpy(img + *at, lo, n);
    *at += (u32)n;
}
static const u8 *host_ram_image(void) {
    static u8 *img;
    if (!img && !(img = malloc(0x400000))) host_fatal("no memory for the RAM image");
    memset(img, 0, 0x400000);
    const u32 arena = HOST_MAIN_RAM_LO - 0x02000000;
    u32 at = 0x1000;
    ram_image_put(img, &at, gHostGameDataBegin, gHostGameDataEnd + 1, arena);
    at = at <= 0x40000 ? 0x40000 : (at + 0xfff) & ~0xfffu;
    ram_image_put(img, &at, gHostGameBssBegin, gHostGameBssEnd + 1, arena);
    memcpy(img + arena, gHostMainRam, sizeof gHostMainRam);
    memcpy(img + 0x3ff000, &gHostSysWork, 0x1000);
    return img;
}

u32 host_irq_state(void);
// ---- events
static u64 sNextVblank = 192u * LINE_CYCLES;
static void run_events(void);
static void io_trace_frame(void);
static u32 sStatWaits;
void host_advance_to_next_event(void) {
    sStatWaits++;
    u64 next = sNextVblank;
    if (pxi7_next_due() < next) next = pxi7_next_due();
    extern u64 wm7_next_due(void);
    if (wm7_next_due() < next) next = wm7_next_due();
    extern u64 snd7_next_due(void);
    if (snd7_next_due() < next) next = snd7_next_due();
    for (int i = 0; i < 4; i++) {
        if (sTm[i].on && sTm[i].irq && !sTm[i].cascade) {
            u64 o = tm_next_overflow(&sTm[i]);
            if (o < next) next = o;
        }
    }
    if (next > sCycles) sCycles = next;
    run_events();
}
static u64 sLastTimerCheck[4];
static void dma_on_vblank(void);
static int sInEvents;
static void run_events_body(void);
static void run_events(void) {
    if (sInEvents) return;  // (a register access from inside the event handling)
    sInEvents = 1;
    sTimeless = 0;
    run_events_body();
    __atomic_store_n(&sHeartbeat, (u32)sCycles, __ATOMIC_RELAXED);
    sInEvents = 0;
}
void snd7_run(u64 now);
static void run_events_body(void) {
    snd7_run(sCycles);
    pxi7_run_due();
    extern void wm7_run(u64 now);
    wm7_run(sCycles);
    while (sCycles >= sNextVblank) {
        static int watchdogOn;
        if (!watchdogOn) {
            watchdogOn = 1;
            watchdog_start();
        }
#ifdef N3DS_DIRECT
        // (from frame 600 on: the start's ROM reads took 10 s on hardware once the log's thread wrote to the SD card
        // beside them, unverified as the cause; until then the buffer holds the lines)
        static int logThread;
        if (!logThread && host_frame() >= 600) {
            logThread = 1;
            if (!thread_start(log_writer, 0, 0)) fprintf(stderr, "host: no thread for the log\n");
        }
#endif
        __atomic_store_n(&sHeartbeatFrame, (u32)host_frame(), __ATOMIC_RELAXED);
        u16 st = R16(0x04000004);
        if (st & 8) host_irq_raise(1);    // VBlank
        // HBlank: once per frame here, as delivered interrupts wake the threads waiting for one (its handler finds
        // VCOUNT past the visible lines and does nothing); the per-line effects run while the frame is drawn
        // (host_hblank)
        if (st & 16) host_irq_raise(2);
        dma_on_vblank();
        render_frame();
        extern void frontend_frame(void);
        frontend_frame();
        extern void host_heap_check(void);
        host_heap_check();
        host_guards_frame();  // ACWW_GUARDS (the guard variant)
        host_screenshot();
        io_trace_frame();
        {   // ACWW_POSLOG=<n>: the local player's position (fx32 x y z, in units of 1/4096) every n frames - for
            // writing input scripts that walk somewhere
            static long every = -2;
            if (every == -2) every = getenv("ACWW_POSLOG") ? strtol(getenv("ACWW_POSLOG"), 0, 10) : 0;
            if (every > 0 && host_frame() % (u64)every == 0) {
                extern struct VecFx32 *PlayerActor_GetBodyPos(u32 idx);
                const s32 *p = (const s32 *)PlayerActor_GetBodyPos(4);
                if (p) fprintf(stderr, "pos %llu %.2f %.2f %.2f\n", (unsigned long long)host_frame(), p[0] / 4096.0,
                               p[1] / 4096.0, p[2] / 4096.0);
            }
        }
        {   // ACWW_TEST_JOB_DONE=1: the current player's "Nook's part-time job" flag (PlayerData flag 1) kept clear, so
            // that a scripted test from the save script's save can use the gate (local wireless) without playing the
            // job. Test runs only: the flag is written into the game's memory every frame.
            static int jobDone = -1;
            if (jobDone < 0) jobDone = getenv("ACWW_TEST_JOB_DONE") != 0;
            extern void *PlayerData_GetCurrent(void);
            u32 *pd = jobDone ? PlayerData_GetCurrent() : 0;
            if (pd) pd[0x21fc / 4] &= ~2u;  // (PlayerData.flags[0], include/player/PlayerData.h)
        }
        {   // ACWW_EXIT_FRAME=<n>: end the run after frame n (sound file closed as on an interrupt) - test runs
            // stop once they have what they compare instead of running out a timeout
            static long exitAt = -2;
            if (exitAt == -2) exitAt = getenv("ACWW_EXIT_FRAME") ? strtol(getenv("ACWW_EXIT_FRAME"), 0, 10) : -1;
            if (exitAt >= 0 && (long)host_frame() >= exitAt) {
                fprintf(stderr, "host: exit at frame %llu (ACWW_EXIT_FRAME)\n", (unsigned long long)host_frame());
                host_quit();
            }
        }
        {   // ACWW_RAMHASH=<file>: "frame hash" of main RAM (host_ram_image: the game's data and heaps on 32-bit
            // hosts; then DTCM and the whole system work area) every frame - two runs' first differing frame;
            // ACWW_RAMDUMP=<frame>:<file>: the main RAM image at a frame (ACWW_RAMDUMP="f1,f2,...:prefix":
            // prefix-<frame>.bin at each frame)
            static FILE *hf; static int hinit; static const char *ramList, *ramTo;
            if (!hinit) {
                hinit = 1;
                if (getenv("ACWW_RAMHASH")) hf = fopen(getenv("ACWW_RAMHASH"), "wb");
                ramList = getenv("ACWW_RAMDUMP");
                ramTo = ramList && strchr(ramList, ':') ? strchr(ramList, ':') + 1 : 0;
            }
            if (hf) {
                const u64 *p = (const u64 *)host_ram_image();
                u64 h = 0x9e3779b97f4a7c15ull;
                for (u32 i = 0; i < 0x400000 / 8; i++) h = (h ^ p[i]) * 0x100000001b3ull;
                for (u32 i = 0; i < sizeof gHostDtcm / 8; i++) h = (h ^ ((const u64 *)gHostDtcm)[i]) * 0x100000001b3ull;
                for (u32 i = 0; i < sizeof gHostSysWork / 8; i++) h = (h ^ ((const u64 *)&gHostSysWork)[i]) * 0x100000001b3ull;
                fprintf(hf, "%llu %016llx\n", (unsigned long long)host_frame(), (unsigned long long)h);
                fflush(hf);
            }
            if (ramTo) {
                for (const char *q = ramList; q && q < ramTo; ) {
                    char *end;
                    long at = strtol(q, &end, 10);
                    if (end == q) break;
                    if ((long)host_frame() == at) {
                        char name[512];
                        snprintf(name, sizeof name, "%s-%ld.bin", ramTo, at);
                        FILE *df = fopen(name, "wb");
                        if (df) { fwrite(host_ram_image(), 1, 0x400000, df); fclose(df); }
                    }
                    q = end + (*end == ',');
                }
            }
        }
        {   // ACWW_VRAMDUMP=<frame>:<file>: the 9 VRAM banks, palettes (0x800), OAM (0x800), I/O 0x04000000-0x04001100
            static long dumpAt = -2; static const char *dumpTo;
            if (dumpAt == -2) {
                const char *e = getenv("ACWW_VRAMDUMP");
                dumpAt = e ? strtol(e, 0, 10) : -1;
                dumpTo = e && strchr(e, ':') ? strchr(e, ':') + 1 : 0;
            }
            if (dumpAt >= 0 && dumpTo && (long)host_frame() == dumpAt) {
                extern u8 *host_vram_bank(int i);
                extern u32 host_vram_bank_size(int i);
                extern void host_vram_flush(void);
                host_vram_flush();
                FILE *df = fopen(dumpTo, "wb");
                if (df) {
                    for (int b = 0; b < 9; b++) fwrite(host_vram_bank(b), 1, host_vram_bank_size(b), df);
                    fwrite(gHostPltt, 1, sizeof gHostPltt, df);
                    fwrite(gHostOam, 1, sizeof gHostOam, df);
                    for (u32 w = 0x04000000; w < 0x04001100; w += 4) { u32 v = R32(w); fwrite(&v, 4, 1, df); }  // display I/O
                    fclose(df);
                }
            }
        }
        static int statsEvery = -1;  // ACWW_STATS=<frames> (1: every 300)
        if (statsEvery < 0) statsEvery = getenv("ACWW_STATS") ? (atoi(getenv("ACWW_STATS")) > 1 ? atoi(getenv("ACWW_STATS")) : 300) : 0;
        if (statsEvery && host_frame() % statsEvery == 0) {
            extern u8 *host_vram_bank(int i);
            extern u32 host_vram_bank_size(int i);
            extern void host_vram_flush(void);
            host_vram_flush();
            int pal = 0; for (int i = 0; i < 0x800; i++) pal += gHostPltt[i] != 0;
            fprintf(stderr, "stats: frame %llu dispcnt %08x/%08x bright %04x/%04x pal %d banks", (unsigned long long)host_frame(),
                    R32(0x04000000), R32(0x04001000), R16(0x0400006c), R16(0x0400106c), pal);
            for (int b = 0; b < 9; b++) { int nz = 0; u8 *p = host_vram_bank(b); for (u32 i = 0; i < host_vram_bank_size(b); i++) nz += p[i] != 0; fprintf(stderr, " %d", nz); }
            fprintf(stderr, " game bld %08x %08x/%08x %08x bg %08x %08x", R32(0x04000050), R32(0x04000054), R32(0x04001050), R32(0x04001054), R32(0x04000008), R32(0x0400000c));
            extern u32 gScreenTransition[];
            fprintf(stderr, " transition");
            for (int i = 0; i < 12; i++) fprintf(stderr, " %x", gScreenTransition[i]);
            render_debug();
            fprintf(stderr, " gx %llu", sGxWrites);
            sGxWrites = 0;
            { static unsigned long long lastIo; fprintf(stderr, " io %llu", (unsigned long long)sIoCount - lastIo); lastIo = sIoCount; }
            fprintf(stderr, " vramcnt %08x %08x ime %x ie %08x if %08x cpu %x\n", R32(0x04000240), R32(0x04000244), R32(0x04000208), R32(0x04000210), sIF, host_irq_state());
        }
        static u64 prevFrame, slowest, slowestAt;  // (ACWW_FPS: the longest frame of the 120, real time, and which)
        if (getenv("ACWW_FPS")) {
            u64 t = host_plat_time_ns();
            int slow = prevFrame && t - prevFrame > slowest;
            if (slow) slowest = t - prevFrame, slowestAt = host_frame();
            prevFrame = t;
            prof_frame(slow);
        }
        if (getenv("ACWW_FPS") && host_frame() % 120 == 0) {  // frames per second of real time, over the last 120
            static u64 last;
            u64 now = host_plat_time_ns();
            if (last) {
                fprintf(stderr, "fps: frame %llu %.1f, the slowest frame %.1f ms (frame %llu)\n", (unsigned long long)host_frame(),
                        120 / ((double)(now - last) * 1e-9), slowest * 1e-6, (unsigned long long)slowestAt);
                slowest = 0;
                prof_slowest_report();
                prof_report((double)(now - last) / 120e6);
                double ms = (double)(host_plat_time_ns() - now) * 1e-6;  // (the report's writes: the 3DS's SD card)
                if (ms > 100) fprintf(stderr, "host: the report above took %.0f ms to write\n", ms);
            } else if (!thread_start(prof_sampler, 0, 1)) {
                fprintf(stderr, "host: no thread for ACWW_FPS's profile\n");
            }
            last = now;
        }
        host_pace_frame();
        extern void pxi7_vblank(void);
        pxi7_vblank();
        sNextVblank += FRAME_CYCLES;
    }
    for (int i = 0; i < 4; i++) {
        Timer *t = &sTm[i];
        if (!t->on || t->cascade) continue;
        u64 p = tm_period(t);
        u64 before = (sLastTimerCheck[i] > t->start ? sLastTimerCheck[i] - t->start : 0) / p;
        u64 now = (sCycles - t->start) / p;
        if (now > before && t->irq) host_irq_raise(1u << (3 + i));
        sLastTimerCheck[i] = sCycles;
    }
}

// ---- display status
static void update_display(void) {
    u32 line = (u32)(sCycles % FRAME_CYCLES) / LINE_CYCLES, dot = (u32)(sCycles % LINE_CYCLES);
    u16 st = R16(0x04000004) & ~7u;
    if (line >= 192) st |= 1;
    if (dot >= 1606) st |= 2;
    if (line == ((R16(0x04000004) >> 8) | ((R16(0x04000004) & 0x80) << 1))) st |= 4;
    R16(0x04000004) = st;
    R16(0x04000006) = (u16)line;
}

// ---- divider and square root
static void divide(void) {
    u16 cnt = R16(0x04000280);
    int64_t n = (cnt & 3) == 0 ? (int32_t)R32(0x04000290) : *(int64_t *)host_io(0x04000290);
    int64_t d = (cnt & 3) == 2 ? *(int64_t *)host_io(0x04000298) : (int32_t)R32(0x04000298);
    int64_t q, r;
    if (d == 0) { q = n < 0 ? 1 : -1; r = n; R16(0x04000280) = cnt | 0x4000; }
    else { q = n / d; r = n % d; R16(0x04000280) = cnt & ~0x4000; }
    *(int64_t *)host_io(0x040002a0) = q;
    *(int64_t *)host_io(0x040002a8) = r;
}
static void square_root(void) {
    u64 v = (R16(0x040002b0) & 1) ? *(u64 *)host_io(0x040002b8) : R32(0x040002b8);
    u64 r = 0, bit = 1ull << 62;
    while (bit > v) bit >>= 2;
    while (bit) { if (v >= r + bit) { v -= r + bit; r = (r >> 1) + bit; } else r >>= 1; bit >>= 2; }
    R32(0x040002b4) = (u32)r;
}

static void io_write(u32 a, u32 old);

// ---- DMA (0x040000b0 + 12n: source, destination, control)
// The registers are 32 bits; the functions that set them (MIi_DmaSetParams ...: include/nitro/dma_host.h) give the
// whole addresses first. They are used while the registers hold their low words (a register written otherwise - a
// register address, a DS constant - is taken as it is).
static uintptr_t sDmaSrc[4], sDmaDst[4];
void HostDma_Addresses(unsigned int ch, uintptr_t src, uintptr_t dst) {
    if (ch < 4) { sDmaSrc[ch] = src; sDmaDst[ch] = dst; }
}
static uintptr_t dma_address(u32 reg, uintptr_t full) { return (u32)full == reg ? full : reg; }
static int dma_is_io(uintptr_t a) { return a <= 0xffffffffu && is_io((u32)a); }
void gx_fifo_write(u32 addr, u32 value);
static int sTimingLegacy; static u32 sPollAddr; static long sIoLog;  // (defined below)
static void dma_run(int ch) {
    HOST_PROF(PROF_DMA);
    u32 base = 0x040000b0 + ch * 12, cnt = R32(base + 8);
    uintptr_t src = dma_address(R32(base), sDmaSrc[ch]), dst = dma_address(R32(base + 4), sDmaDst[ch]);
    u32 n = cnt & 0x1fffff, w = (cnt >> 26 & 1) ? 4 : 2;
    int dc = cnt >> 21 & 3, sc = cnt >> 23 & 3;
    if (n == 0) n = 0x200000;
    if (src < 0x10000 || dst < 0x10000) {  // (a null pointer, a small number)
        fprintf(stderr, "host: DMA%d from %08llx to %08llx, control %08x: bad address\n", ch, (unsigned long long)src,
                (unsigned long long)dst, cnt);
        R32(base + 8) = cnt & ~0x80000000u;
        return;
    }
    // memory to memory, both ascending and not overlapping: one copy (the words the loop below would copy)
    int incDst = dc == 0 || dc == 3;
    if (sc == 0 && incDst && !dma_is_io(src) && !dma_is_io(dst) && (dst + (uintptr_t)n * w <= src || src + (uintptr_t)n * w <= dst)) {
        memcpy((void *)dst, (const void *)src, (size_t)n * w);
        src += (uintptr_t)n * w;
        dst += (uintptr_t)n * w;
        n = 0;
    }
    // memory to a 3D command port (the GX FIFO's DMA): the words the loop below would write, without the register
    // layer per word (io_write does nothing else for them: no interrupt is checked between a DMA's words)
    u32 d32 = (u32)dst;
    if (w == 4 && sc == 0 && dc == 2 && n && !dma_is_io(src) && dst == d32 && d32 >= 0x04000400 && d32 < 0x04000600 &&
        sTimingLegacy == 0 && sTimeless + n < 1000000 && sIoLog == -1) {
        R32(d32) = ((const u32 *)src)[n - 1];
        sPollAddr = ~0u;
        sTimeless += n;
        sGxWrites += n;
        render_write32_n(d32, (const u32 *)src, n);
        src += (uintptr_t)n * 4;
        n = 0;
    }
    for (u32 i = 0; i < n; i++) {
        u32 v;
        if (src == IO1 + 0x10) v = card_read_data();
        else if (dma_is_io(src)) v = w == 4 ? R32((u32)src) : R16((u32)src);
        else v = w == 4 ? *(volatile u32 *)src : *(volatile u16 *)src;
        if (dma_is_io(dst)) {
            // as a CPU write would be (the display registers go to the renderer)
            u32 old = R32((u32)dst & ~3u);
            if (w == 4) R32((u32)dst) = v; else R16((u32)dst) = (u16)v;
            io_write((u32)dst, old);
        } else if (w == 4) *(volatile u32 *)dst = v;
        else *(volatile u16 *)dst = (u16)v;
        if (sc == 0) src += w; else if (sc == 1) src -= w;
        if (dc == 0 || dc == 3) dst += w; else if (dc == 1) dst -= w;
    }
    if (dc != 3) { R32(base + 4) = (u32)dst; sDmaDst[ch] = dst; }
    R32(base) = (u32)src;
    sDmaSrc[ch] = src;
    if (!(cnt >> 25 & 1) || ((cnt >> 27 & 7) == 0)) R32(base + 8) = cnt & ~0x80000000u;
    if (cnt >> 30 & 1) host_irq_raise(1u << (8 + ch));
}
static void dma_control_written(int ch) {
    u32 cnt = R32(0x040000b0 + ch * 12 + 8);
    if (!(cnt >> 31)) return;
    u32 mode = cnt >> 27 & 7;
    if (mode == 0 || mode == 7) dma_run(ch);  // immediate; GX FIFO: the FIFO never fills here
}
static void dma_on_vblank(void) {
    for (int ch = 0; ch < 4; ch++) {
        u32 cnt = R32(0x040000b0 + ch * 12 + 8);
        if (cnt >> 31 && ((cnt >> 27 & 7) == 1 || (cnt >> 27 & 7) == 2)) dma_run(ch);
    }
}
void host_card_dma(void) {
    for (int ch = 0; ch < 4; ch++) {
        u32 cnt = R32(0x040000b0 + ch * 12 + 8);
        if (cnt >> 31 && (cnt >> 27 & 7) == 5) dma_run(ch);
    }
}

// ---- the 3D engine: for now a sink; GXSTAT says idle with an empty FIFO
void gx_fifo_write(u32 addr, u32 value) { sGxWrites++; render_write32(addr, value); }

// ---- register reads and writes
// How much virtual time a register read takes (docs/host.md, "Virtual time"). Only what the game can observe of time
// moves it: a read of the display status / VCOUNT or of a timer's counter (64 cycles: one OS tick), and a polling loop
// (the same register read again with no other register access in between: from the fifth read on, 4096 cycles a
// read). Every other access takes no time, so how many accesses a stretch of code makes (the code's shape: an extra
// interrupt-mask save, a copy done by DMA or by the CPU, the build's architecture) doesn't move the clock.
// ACWW_TIMING=legacy: the earlier model (64 cycles for every read, 4096 for a read of the register read last).
static int sTimingLegacy = -1;
static u32 sPollAddr = ~0u, sPollRepeat;  // the address of the last access if it was a read; how often in a row
static u32 sStatPolls, sStatClockReads;  // per frame, for ACWW_IOTRACE
static void io_spin_check(void);
static uintptr_t io_last_caller(void);
static u32 io_read_cost(u32 a) {
    u32 w = a & ~3u;
    if (sTimingLegacy) {
        static u32 last, repeat;
        repeat = a == last ? repeat + 1 : 0;
        last = a;
        return repeat > 4 ? 4096 : 64;
    }
    sPollRepeat = a == sPollAddr ? sPollRepeat + 1 : 0;
    sPollAddr = a;
    if (sPollRepeat > 4) { sStatPolls++; return 4096; }
    if (w == 0x04000004 || (w >= 0x04000100 && w < 0x04000110)) { sStatClockReads++; return 64; }
    return 0;
}
// a stretch of game code that waits a given time without reading a clock (OS_SpinWait, SVC_WaitByLoop)
void host_spend_cycles(u64 n) {
    if (sTimingLegacy) return;
    sCycles += n;
    run_events();
}
// the H-blank after visible line `line` (render.c, while the 2D renderer draws the frame): the game's H-blank interrupt
// handler, run at once as the interrupt would, when DISPSTAT, IE and IME enable it. The DS raises it on every line;
// the game's handler acts on the visible ones, after which the effects' register writes show. Meanwhile DISPSTAT and
// VCOUNT read as that line's H-blank, without moving the virtual time, and the handler's register accesses leave the
// interrupted code's polling state (a polling loop, the spin check) as it was: they are the renderer's, not the game
// code's in which the frame happens to be drawn.
static int sHBlankLine = -1;
static u32 sPollAddr, sPollRepeat;  // (defined below, io_read_cost)
// returns 0 if no handler ran (nothing in the game changed)
int host_hblank(int line) {
    if (!(R16(0x04000004) & 0x10) || !(R32(0x04000208) & 1) || !(R32(0x04000210) & 2)) return 0;
    u16 st = R16(0x04000004), vcount = R16(0x04000006);
    u32 pollAddr = sPollAddr, pollRepeat = sPollRepeat, timeless = sTimeless;
    sHBlankLine = line;
    host_irq_call(1);
    sHBlankLine = -1;
    sPollAddr = pollAddr, sPollRepeat = pollRepeat, sTimeless = timeless;
    R16(0x04000004) = st;
    R16(0x04000006) = vcount;
    return 1;
}
static void io_read(u32 a) {
    if (sHBlankLine >= 0 && (a & ~3u) == 0x04000004) {
        R16(0x04000004) = (u16)((R16(0x04000004) & ~7u) | 2);
        R16(0x04000006) = (u16)sHBlankLine;
        return;
    }
    u32 cost = io_read_cost(a);
    if (cost) {
        sCycles += cost;
        run_events();
    } else io_spin_check();
    u32 w = a & ~3u;
    if (w == 0x04000004) update_display();
    else if (w >= 0x04000100 && w < 0x04000110) {
        int i = (w - 0x04000100) / 4;
        R16(w) = tm_counter(&sTm[i]);
    } else if (w == 0x04000214) R32(w) = sIF;
    else if ((w >= 0x04000600 && w < 0x040006a4) || w == 0x04000320) R32(w) = render_read32(w);  // 3D status, results
    else if (w == 0x04000184 || w == 0x04000180 || w == IO1) {
        extern void pxi7_prepare_read(u32 a);
        pxi7_prepare_read(w);
    } else if (w == 0x040001a4) {
        // ROMCTRL: the transfer state is kept up to date by card.c
    } else if (w == IO1 + 0x10) R32(w) = card_read_data();
}

// display registers for the renderer: the 3D command ports in full on every write, the others as changed halves (and
// the affine reference points on every write)
static void forward_display(u32 w, u32 old) {
    u32 v = R32(w);
    if ((w >= 0x04000400 && w < 0x04000600) || w == 0x04000600) { sGxWrites++; render_write32(w, v); return; }
    if (w >= 0x04000240 && w < 0x0400024c) {
        for (int i = 0; i < 4; i++) {
            u8 b = v >> (8 * i), o = old >> (8 * i);
            if (b != o) { host_vram_control(w + i, b); render_write8(w + i, b); }
        }
        return;
    }
    // (BGxX / BGxY of either engine: a write reloads the affine BG's internal reference point even with the same value)
    int reload = (w & ~0x1014u) == 0x04000028;
    for (int h = 0; h < 2; h++) {
        u16 n = v >> (16 * h), o = old >> (16 * h);
        if (n != o || reload) render_write16(w + 2 * h, n);
    }
}
static int is_display(u32 w) {
    return (w < 0x04000070 && w != 0x04000004) || (w >= IOB && w < IOB + 0x70) || (w >= 0x04000240 && w < 0x0400024c) ||
           w == 0x04000304 || (w >= 0x04000320 && w < 0x040006a4);
}

// The game spinning on memory with register accesses that take no time (an interrupt-mask save and restore around a
// check of a flag that an interrupt handler sets): after a million accesses without time passing, time moves to the
// next event (and that is reported once, with the caller: the site wants an explicit wait). None is known.
static void io_spin_check(void) {
    if (++sTimeless < 1000000) return;
    static int reported;
    if (!reported++)
        fprintf(stderr, "host: %u register accesses without time passing (frame %llu, last from %p): the game spins; "
                "virtual time moves to the next event\n", sTimeless, (unsigned long long)host_frame(),
                (void *)io_last_caller());
    sTimeless = 0;
    host_advance_to_next_event();
}

static long sIoLog = -2;  // ACWW_IOLOG
static void io_write(u32 a, u32 old) {
    u32 w = a & ~3u;
    if (!sTimingLegacy) {
        sPollAddr = ~0u;  // (a write ends a polling loop)
        io_spin_check();
    }
    if (sIoLog == -2) sIoLog = getenv("ACWW_IOLOG") ? strtol(getenv("ACWW_IOLOG"), 0, 16) : -1;
    if ((long)w == sIoLog) fprintf(stderr, "io: frame %llu [%08x] %08x -> %08x\n", (unsigned long long)host_frame(), w, old, R32(w));
    if (is_display(w)) { forward_display(w, old); return; }
    switch (w) {
    case 0x04000214: { u32 v = R32(w); sIF &= ~v; R32(w) = sIF; break; }
    case 0x040000b8: dma_control_written(0); break;
    case 0x040000c4: dma_control_written(1); break;
    case 0x040000d0: dma_control_written(2); break;
    case 0x040000dc: dma_control_written(3); break;
    case 0x04000100: case 0x04000104: case 0x04000108: case 0x0400010c: {
        // low half: reload value (reads give the counter); high half: control
        Timer *tm = &sTm[(w - 0x04000100) / 4];
        int i = (int)(tm - sTm);
        if ((a & 3) < 2) tm->reload = R16(w);
        u16 c = R16(w + 2);
        int on = c >> 7 & 1;
        if (on && !tm->on) { tm->start = sCycles; sLastTimerCheck[i] = sCycles; }
        tm->on = on; tm->irq = c >> 6 & 1; tm->shift = kTmShift[c & 3]; tm->cascade = i && (c >> 2 & 1);
        break;
    }
    case 0x04000180: pxi7_sync_written(R32(w)); break;
    case 0x04000184: break;  // FIFO control: pxi7.c reads it
    case 0x04000188: pxi7_receive(R32(w)); break;
    case 0x040001a4: card_control_written(R32(w)); break;
    case 0x04000280: case 0x04000290: case 0x04000294: case 0x04000298: case 0x0400029c: divide(); break;
    case 0x040002b0: case 0x040002b8: case 0x040002bc: square_root(); break;
    default: break;
    }
}

// ---- the game's register accesses (include/nitro/io_access.h)
// ACWW_IOHIST: which registers are accessed how often, and from where (acww_iohist.txt, written at exit, on SIGINT
// and at ACWW_EXIT_FRAME). The last 16 accesses are listed on SIGINT.
static int sIoHist = -1;
static struct { u32 addr, write, n; uintptr_t pc; } sHist[4096];
struct IoLog { u32 addr, write; uintptr_t pc; };
static struct IoLog sRing[16];
static u32 sRingPos;
static void io_hist(u32 a, uintptr_t pc, int write) {
    u32 h = (a * 2654435761u ^ (u32)pc * 40503u ^ (u32)(pc >> 16 >> 16) ^ (u32)write) & 4095;
    for (u32 i = 0; i < 4096; i++, h = (h + 1) & 4095) {
        if (sHist[h].n && (sHist[h].addr != a || sHist[h].pc != pc || sHist[h].write != (u32)write)) continue;
        sHist[h].addr = a; sHist[h].pc = pc; sHist[h].write = (u32)write; sHist[h].n++;
        return;
    }
}
static void io_hist_dump(void) {
    static int done;
    if (sIoHist <= 0 || done) return;
    done = 1;
    FILE *f = fopen("acww_iohist.txt", "wb");
    if (!f) return;
    for (int i = 0; i < 4096; i++)
        if (sHist[i].n) fprintf(f, "%u %08x %08llx %c\n", sHist[i].n, sHist[i].addr, (unsigned long long)sHist[i].pc, sHist[i].write ? 'w' : 'r');
    fclose(f);
}
void host_io_ring_dump(void) {
    fprintf(stderr, "host: %llu I/O accesses; last ones:\n", (unsigned long long)sIoCount);
    for (int i = 0; i < 16; i++) {
        struct IoLog *l = &sRing[(sRingPos + i) & 15];
        fprintf(stderr, "  %s %08x from %08llx\n", l->write ? "write" : "read ", l->addr, (unsigned long long)l->pc);
    }
}
static uintptr_t io_last_caller(void) { return sRing[(sRingPos - 1) & 15].pc; }

// the end of a run (the window closed, ACWW_EXIT_FRAME, the game's exit): the sound file closed, the frontend shut
// down, the reports written, then _Exit - not exit, whose handlers would run the game's static destructors, which
// free into heaps that aren't there any more outside the game's threads
void host_quit(void) {
    threads_stop();
    extern void pxi7_save_flush(void);
    pxi7_save_flush();
    extern void snd7_output_close(void);
    snd7_output_close();
    extern void frontend_shutdown(void);
    frontend_shutdown();
    host_exit_dumps();
    fflush(NULL);
    _Exit(0);
}

// The DS's system reset (OS_ResetSystem: the game starts again from its title, e.g. after a broken wireless
// connection): the program starts again in place, its window closed and opened again; headless runs end there (a
// test script can't go on).
void host_reset_system(void) {
    fprintf(stderr, "host: the game reset the system (frame %llu)%s\n", (unsigned long long)host_frame(),
            getenv("ACWW_HEADLESS") ? ": the run ends" : ": starting again");
    if (getenv("ACWW_HEADLESS")) host_quit();
    extern void pxi7_save_flush(void);
    pxi7_save_flush();
    extern void snd7_output_close(void);
    snd7_output_close();
    extern void frontend_shutdown(void);
    frontend_shutdown();
    host_exit_dumps();
    fflush(NULL);
    extern char **gHostArgv;
    host_plat_restart(gHostArgv);
    host_fatal("the program couldn't start again");
}

// the reports written at exit: atexit, ACWW_EXIT_FRAME (_Exit), SIGINT (debug_posix.c)
void host_exit_dumps(void) {
    io_hist_dump();
    render_exit_report();
    host_guards_exit();
    host_debug_exit_dumps();
}

static u32 io_load(u32 a, u32 size) {
    io_read(a);
    return size == 1 ? *host_io(a) : size == 2 ? *(u16 *)host_io(a) : *(u32 *)host_io(a);
}
static void io_store(u32 a, u32 size, u32 v) {
    u32 old = R32(a & ~3u);
    if (size == 1) *host_io(a) = (u8)v;
    else if (size == 2) *(u16 *)host_io(a) = (u16)v;
    else *(u32 *)host_io(a) = v;
    io_write(a, old);
}
// after each access: the interrupts that became due are taken
int host_irq_ready(void);
static inline void io_access_done(void) {
    if (host_irq_ready()) host_deliver_irqs();
}
// ACWW_IOTRACE=<file>: per frame, the number of register reads and writes and a hash of their sequence (addresses
// and values) - two builds' first frame whose register traffic differs; ACWW_IOTRACE_FRAMES=<from>-<to>: every
// access in those frames too ("r|w address value cycles caller", the caller less host_frame's address: add
// host_frame's address from `nm` for addr2line). (The text files here are written in binary mode: the same
// bytes, LF line ends, on Windows too, so that two builds' files compare.)
static int sIoTrace = -1;
static FILE *sIoTraceFile;
static u64 sTrHash = 0xcbf29ce484222325ull, sTrReads, sTrWrites;
static long sTrFrom = -1, sTrTo = -1;
static uintptr_t sTrCaller;
static void io_trace_init(void) {
    const char *e = getenv("ACWW_IOTRACE");
    sIoTrace = e && *e;
    if (!sIoTrace) return;
    if (!(sIoTraceFile = fopen(e, "wb"))) { sIoTrace = 0; return; }
    const char *r = getenv("ACWW_IOTRACE_FRAMES");
    if (r) sscanf(r, "%ld-%ld", &sTrFrom, &sTrTo);
}
static void io_trace_mix(u64 v) { sTrHash = (sTrHash ^ v) * 0x100000001b3ull; }
static void io_trace(u32 a, int write, void *caller) {
    io_trace_mix(a | (u64)write << 32);
    if (write) sTrWrites++; else sTrReads++;
    sTrCaller = (uintptr_t)caller;
}
static void io_trace_value(u32 a, int write, unsigned long long v) {
    // (every value read and written, but the written DMA source / destination addresses and PXI FIFO words: they
    // can be host addresses)
    int addrReg = (a >= 0x040000b0 && a < 0x040000e0 && (a - 0x040000b0) % 12 < 8) || (a & ~3u) == 0x04000188;
    if (!write || !addrReg) io_trace_mix(v);
    long f = (long)host_frame();
    if (f >= sTrFrom && f <= sTrTo)
        fprintf(sIoTraceFile, "%c %08x %llx %llu %llx\n", write ? 'w' : 'r', a, v, (unsigned long long)sCycles,
                (unsigned long long)(sTrCaller - (uintptr_t)&host_frame));
}
static void io_trace_frame(void) {
    if (sIoTrace < 0) io_trace_init();
    if (!sIoTrace) return;
    fprintf(sIoTraceFile, "frame %llu reads %llu writes %llu hash %016llx cycles %llu clock %u polls %u waits %u\n",
            (unsigned long long)host_frame(), (unsigned long long)sTrReads, (unsigned long long)sTrWrites,
            (unsigned long long)sTrHash, (unsigned long long)sCycles, sStatClockReads, sStatPolls, sStatWaits);
    fflush(sIoTraceFile);
    sTrReads = sTrWrites = 0;
    sStatClockReads = sStatPolls = sStatWaits = 0;
}

// (the whole pointer-sized address: a host pointer above 4 GB whose low word looks like a register isn't one)
static void io_note(uintptr_t a, int write, void *caller) {
    if (a > 0xffffffffu || !is_io((u32)a)) {
        fprintf(stderr, "host: register access to %08llx (not an I/O register) from %p\n", (unsigned long long)a, caller);
        host_fatal("bad register address");
    }
    sIoCount++;
    if (sIoTrace > 0) io_trace((u32)a, write, caller);
    sRing[sRingPos++ & 15] = (struct IoLog){(u32)a, (u32)write, (uintptr_t)caller};
    if (sIoHist < 0) sIoHist = getenv("ACWW_IOHIST") != NULL;
    if (sIoHist) io_hist((u32)a, (uintptr_t)caller, write);
}
unsigned long long HostIo_Read(uintptr_t addr, unsigned int size) {
    HOST_PROF(PROF_REGS);
    u32 a = (u32)addr;
    if (sHBlankLine >= 0)  // (the H-blank handler's accesses: not the game code's traffic, host_hblank)
        return size == 8 ? io_load(a, 4) | (unsigned long long)io_load(a + 4, 4) << 32 : io_load(a, size);
    io_note(addr, 0, __builtin_return_address(0));
    unsigned long long v = size == 8 ? io_load(a, 4) | (unsigned long long)io_load(a + 4, 4) << 32 : io_load(a, size);
    if (sIoTrace > 0) io_trace_value(a, 0, v);
    io_access_done();
    return v;
}
unsigned long long HostIo_Write(uintptr_t addr, unsigned int size, unsigned long long v) {
    HOST_PROF(PROF_REGS);
    u32 a = (u32)addr;
    int traced = sHBlankLine < 0;  // (as for reads)
    if (traced) io_note(addr, 1, __builtin_return_address(0));
    if (traced && sIoTrace > 0) io_trace_value(a, 1, v);
    if (size == 8) {
        io_store(a, 4, (u32)v);
        io_store(a + 4, 4, (u32)(v >> 32));
    } else {
        io_store(a, size, (u32)v);
    }
    if (traced) io_access_done();
    return v;
}

// MIi_CpuSend32 to a 3D command port (sdk.c), most of the game's register writes: each word as HostIo_Write takes
// it, without the dispatch that the port decides once (a display register: the 3D engine, forward_display). Through
// HostIo_Write while a trace or the histogram wants every access, in an H-blank handler, or for ACWW_IOLOG.
void HostIo_Send32(uintptr_t addr, const u32 *src, u32 n) {
    u32 a = (u32)addr;
    if (sIoTrace != 0 || sIoHist != 0 || sHBlankLine >= 0 || sIoLog != -1 || a < 0x04000400 || a >= 0x04000600) {
        for (u32 i = 0; i < n; i++) HostIo_Write(addr, 4, src[i]);
        return;
    }
    HOST_PROF(PROF_REGS);
    void *caller = __builtin_return_address(0);
    for (u32 i = 0; i < n; i++) {
        // after the first word, with the GX FIFO interrupt off and no time to move (the spin check far from its
        // limit), nothing between the words can make an interrupt due: the rest at once, checked after the last
        if (i == 1 && sTimingLegacy == 0 && sTimeless + n < 1000000 &&
            !((R32(0x04000208) & 1) && (R32(0x04000210) & 1u << 21))) {
            u32 m = n - 1;
            sIoCount += m;
            for (u32 k = 0; k < m && k < 16; k++) sRing[sRingPos++ & 15] = (struct IoLog){a, 1, (uintptr_t)caller};
            sRingPos += m > 16 ? m - 16 : 0;
            R32(a) = src[n - 1];
            sPollAddr = ~0u;
            sTimeless += m;
            sGxWrites += m;
            render_write32_n(a, src + 1, m);
            io_access_done();
            break;
        }
        sIoCount++;
        sRing[sRingPos++ & 15] = (struct IoLog){a, 1, (uintptr_t)caller};
        R32(a) = src[i];
        if (!sTimingLegacy) {
            sPollAddr = ~0u;
            io_spin_check();
        }
        sGxWrites++;
        render_write32(a, src[i]);
        io_access_done();
    }
}

// ---- ACWW_HIGHMEM=1 (x86-64 Linux): the low-4 GB poisoning harness (docs/portability.md 3.0, 3.3). The whole low
// 4 GB (from vm.mmap_min_addr's 64 KB up) is reserved without access, as macOS's __PAGEZERO: the 64-bit build is a
// PIE, so the program image, the C library's heap, the root heap and the threads' stacks are all above it (any
// range already mapped below 4 GB is left alone and reported). A pointer cut to 32 bits, or a DS address used as a
// pointer, then faults at a low address (or, sign-extended, at a non-canonical one), and the crash report
// (debug_posix.c) says where. (ACWW_HIGHMEM=2, which left out main RAM's mapping before that went, is the same.)
int host_highmem(void) {
    static int on = -1;
    if (on < 0) {
#if defined(__x86_64__) && defined(__linux__)
        const char *e = getenv("ACWW_HIGHMEM");
        on = e && *e && strcmp(e, "0") ? 1 : 0;
#else
        on = 0;
#endif
    }
    return on;
}

static void highmem_poison(void) {
#if defined(__x86_64__) && defined(__linux__)
    static char maps[1 << 16];
    int fd = open("/proc/self/maps", O_RDONLY);
    if (fd < 0) host_fatal("ACWW_HIGHMEM: /proc/self/maps");
    size_t len = 0;
    for (ssize_t r; len < sizeof maps - 1 && (r = read(fd, maps + len, sizeof maps - 1 - len)) > 0;) len += (size_t)r;
    close(fd);
    maps[len] = 0;
    // the mappings in address order (the file lists them so); reserve every gap below 4 GB
    uintptr_t at = 0x10000;  // (vm.mmap_min_addr: nothing can be mapped lower anyway)
    const uintptr_t top = 0x100000000ull;
    u64 reserved = 0;
    int ranges = 0;
    for (char *l = maps; at < top;) {
        unsigned long lo, hi;
        uintptr_t end = top;
        int last = sscanf(l, "%lx-%lx", &lo, &hi) != 2;
        if (!last) end = lo < top ? lo : top;
        if (end > at) {
            if (mmap((void *)at, end - at, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_FIXED_NOREPLACE,
                     -1, 0) != (void *)at)
                fprintf(stderr, "host: ACWW_HIGHMEM: can't reserve %08lx-%08lx\n", (unsigned long)at, (unsigned long)end);
            else { reserved += end - at; ranges++; }
        }
        if (last) break;
        if (lo < top) fprintf(stderr, "host: ACWW_HIGHMEM: mapped below 4 GB: %lx-%lx\n", lo, hi);
        if (hi > at) at = hi;
        char *nl = strchr(l, '\n');
        l = nl ? nl + 1 : l + strlen(l);
    }
    fprintf(stderr, "host: ACWW_HIGHMEM: %d ranges below 4 GB reserved (%llu MB of %llu MB)\n", ranges,
            (unsigned long long)(reserved >> 20), (unsigned long long)((top - 0x10000) >> 20));
#endif
}

// at start-up (main.c's early_init)
void host_highmem_init(void) {
    if (host_highmem()) highmem_poison();
}

void host_map_memory(void) {
    {
        const char *e = getenv("ACWW_TIMING");
        sTimingLegacy = e && !strcmp(e, "legacy");
        if (sTimingLegacy) fprintf(stderr, "host: ACWW_TIMING=legacy: every register read takes time\n");
    }
    HostMem_Check();  // (mem.c: the game's variables and the host blocks where they should be)
    host_debug_init();
    R16(0x04000130) = 0x3ff;
    if (getenv("ACWW_IOHIST")) atexit(io_hist_dump);
}

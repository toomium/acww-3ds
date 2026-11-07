// Host build runtime: platform services for the Nintendo 3DS (libctru, newlib). See plat.h.
// The program's directory is sdmc:/3ds/acww/: the options (acww.cfg), the log (log.txt), by default the ROM
// (game.nds) and so the save beside it.
#ifdef __3DS__

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <3ds.h>
#include <sys/iosupport.h>

#include "plat.h"

#define DIR "sdmc:/3ds/acww/"

#ifdef N3DS_DIRECT
// The enhanced port's log: stdout and stderr only copy their lines into memory, and hw.c's log thread writes them to
// log.txt (host_plat_log_write): a write to the SD card took up to 10 s now and then on the user's New 3DS, and
// stderr's lock, held through it, stopped the game's thread at its next line. A full buffer drops what doesn't fit.
static char sLog[64 * 1024];
static size_t sLogLen, sLogLost;
static LightLock sLogLock, sLogFileLock;  // (the buffer; the file: the log's thread and the program's end)
static FILE *sLogFile;
static ssize_t log_put(struct _reent *r, void *fd, const char *p, size_t n) {
    (void)r, (void)fd;
    LightLock_Lock(&sLogLock);
    size_t k = n <= sizeof sLog - sLogLen ? n : 0;
    memcpy(sLog + sLogLen, p, k);
    sLogLen += k;
    sLogLost += n - k;
    LightLock_Unlock(&sLogLock);
    return (ssize_t)n;
}
static const devoptab_t sLogDevice = {.name = "log", .write_r = log_put};

void host_plat_log_write(void) {
    static char out[sizeof sLog];
    LightLock_Lock(&sLogFileLock);
    LightLock_Lock(&sLogLock);
    size_t n = sLogLen, lost = sLogLost;
    memcpy(out, sLog, n);
    sLogLen = sLogLost = 0;
    LightLock_Unlock(&sLogLock);
    if (sLogFile) {
        fwrite(out, 1, n, sLogFile);
        if (lost) fprintf(sLogFile, "3ds: the log's buffer was full: %zu bytes lost\n", lost);
        fflush(sLogFile);
    }
    LightLock_Unlock(&sLogFileLock);
}
#endif

// The options: there is no environment on the 3DS, so acww.cfg's `NAME=value` lines (`#` comments) become one, and
// every ACWW_* option of docs/host.md works unchanged. stdout and stderr go to log.txt.
void host_plat_init(void) {
#ifdef N3DS_DIRECT  // (the enhanced port: written out by a thread of its own, hw.c log_writer)
    LightLock_Init(&sLogLock);
    LightLock_Init(&sLogFileLock);
    sLogFile = fopen(DIR "log.txt", "w");
    devoptab_list[STD_OUT] = devoptab_list[STD_ERR] = &sLogDevice;
    setvbuf(stdout, 0, _IONBF, 0);  // (stderr is unbuffered already: nothing waits in either at the program's end)
    atexit(host_plat_log_write);
#else
    freopen(DIR "log.txt", "w", stdout);
    freopen(DIR "log.txt", "a", stderr);
    setvbuf(stderr, 0, _IOLBF, 1024);  // (a line at a time: unbuffered, every piece of a line was a write to the SD card)
#endif
#ifdef N3DS_DIRECT  // (a save of acww.cfg cut short, options_3ds.c opt_save)
    remove(DIR "acww.cfg.tmp");
    if (access(DIR "acww.cfg", F_OK)) rename(DIR "acww.cfg.bak", DIR "acww.cfg");
#endif
    FILE *f = fopen(DIR "acww.cfg", "r");
    static char line[16384];  // (input scripts: ACWW_KEYS is several KB)
    while (f && fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        char *eq = strchr(line, '=');
        if (line[0] == '#' || !eq) continue;
        *eq = 0;
        setenv(line, eq + 1, 1);
    }
    if (f) fclose(f);
    setenv("ACWW_ROM", DIR "game.nds", 0);
#ifdef N3DS_DIRECT
    setenv("ACWW_LANG_ROM", DIR "lang.nds", 0);  // (the EUR ROM, for ACWW_LANG: card.c)
#endif
    bool n3ds = false;
    APT_CheckNew3DS(&n3ds);
    osSetSpeedupEnable(true);  // (the New 3DS: 804 MHz and its L2 cache instead of 268 MHz; nothing on the others)
    fprintf(stderr, "3ds: %s, application memory %lu KB (%lu KB free), heap %lu KB, linear heap %lu KB\n",
            n3ds ? "New 3DS" : "Old 3DS", (unsigned long)osGetMemRegionSize(MEMREGION_APPLICATION) / 1024,
            (unsigned long)osGetMemRegionFree(MEMREGION_APPLICATION) / 1024, (unsigned long)envGetHeapSize() / 1024,
            (unsigned long)envGetLinearHeapSize() / 1024);
}

// svcGetSystemTick counts at the ARM11's base clock (SYSCLOCK_ARM11), whatever the New 3DS's speed
uint64_t host_plat_time_ns(void) {
    uint64_t t = svcGetSystemTick();
    return t / SYSCLOCK_ARM11 * 1000000000u + t % SYSCLOCK_ARM11 * 1000000000u / SYSCLOCK_ARM11;
}

void host_plat_sleep_until_ns(uint64_t t) {
    uint64_t now = host_plat_time_ns();
    if (now < t) svcSleepThread((s64)(t - now));
}

void host_plat_sleep_ms(unsigned ms) { svcSleepThread((s64)ms * 1000000); }

int host_plat_localtime(struct tm *out) {
    time_t now = time(0);
    return localtime_r(&now, out) != 0;
}

// shortcut: no restart in place on the 3DS; the caller reports it and the program ends (back to the launcher)
void host_plat_restart(char **argv) { (void)argv; }

struct Start { void (*fn)(void *); void *arg; };
static void thread_main(void *p) {
    struct Start s = *(struct Start *)p;
    free(p);
    s.fn(s.arg);
}
// at a lower priority than the game's thread (a higher number), on the application core: the 3DS doesn't share a
// core between threads by time slices that we know of, so it runs while the game's thread waits
int host_plat_thread_start(void (*fn)(void *), void *arg) {
    struct Start *s = malloc(sizeof *s);
    if (!s) return 0;
    s->fn = fn;
    s->arg = arg;
    s32 prio = 0x30;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    if (!threadCreate(thread_main, s, 16 * 1024, prio < 0x3f ? prio + 1 : prio, -2, true)) {
        free(s);
        return 0;
    }
    return 1;
}

// on another core if there is one for us: the New 3DS's core 2, else core 1 (an application gets it only after
// APT_SetAppCpuTimeLimit); else on the game's core at a higher priority (the kernel switches to it when its sleep
// ends). The core is logged, to show whether the program can use one beside the game's.
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1) {
    Result limit = APT_SetAppCpuTimeLimit(30);
    s32 prio = 0x30;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    static const int kCores[2][3] = {{2, 1, -2}, {1, 2, -2}};
    for (int k = 0; k < 3; k++) {
        int c = kCores[core1 != 0][k];
        struct Start *s = malloc(sizeof *s);
        if (!s) return 0;
        s->fn = fn;
        s->arg = arg;
        if (threadCreate(thread_main, s, 16 * 1024, c < 0 ? prio - 1 : 0x18, c, true)) {
            fprintf(stderr, "3ds: a thread on core %d (the profile's sampler, the sound; APT_SetAppCpuTimeLimit(30): %08lx)\n",
                    c < 0 ? 0 : c, (unsigned long)limit);
            return 1;
        }
        free(s);
    }
    return 0;
}

// (no guard page: a 3DS application can't change its memory's protection)
void *host_plat_stack_alloc(size_t n, int flags) {
    (void)flags;
    return memalign(0x1000, (n + 0xfff) & ~(size_t)0xfff);
}

void host_plat_stack_free(void *stack, size_t n) {
    (void)n;
    free(stack);
}

void *host_plat_pages_alloc(size_t n) {
    void *p = memalign(0x1000, n);
    if (p) memset(p, 0, n);
    return p;
}

int host_plat_image_bounds(uintptr_t *lo, uintptr_t *hi) {
    (void)lo; (void)hi;
    return 0;
}

int host_plat_backtrace(void **pcs, int n) {
    (void)pcs; (void)n;
    return 0;
}

void host_plat_print_backtrace(void) {}

#endif

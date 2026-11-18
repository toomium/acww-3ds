// Host build runtime: platform services for the Nintendo Switch (libnx, newlib). See plat.h.
// The program's directory is sdmc:/switch/acww/: the options (acww.cfg), the log (log.txt), by default the data pack
// (game.pak) or the ROM (game.nds), and so the save beside it.
#ifdef __SWITCH__

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <switch.h>
#include <sys/iosupport.h>

#include "plat.h"

// The port's version, the log's first line: the Makefile writes port_version.h beside this file in build/upstream.
#if __has_include("port_version.h")
#include "port_version.h"
#else
#define PORT_VERSION "dev"
#endif

#define DIR "sdmc:/switch/acww/"

// The log, as the 3DS's enhanced port keeps it (plat_3ds.c): stdout and stderr only copy their lines into memory,
// and hw.c's log thread writes them to log.txt (host_plat_log_write), so a slow write to the SD card holds up no one.
// (freopen of stdout jumped to address 0 in Eden; not looked into further.) A full buffer drops what doesn't fit.
static char sLog[64 * 1024];
static size_t sLogLen, sLogLost;
static Mutex sLogLock, sLogFileLock;  // (the buffer; the file: the log's thread and the program's end)
static FILE *sLogFile;
static ssize_t log_put(struct _reent *r, void *fd, const char *p, size_t n) {
    (void)r, (void)fd;
    mutexLock(&sLogLock);
    size_t k = n <= sizeof sLog - sLogLen ? n : 0;
    memcpy(sLog + sLogLen, p, k);
    sLogLen += k;
    sLogLost += n - k;
    mutexUnlock(&sLogLock);
    return (ssize_t)n;
}
static const devoptab_t sLogDevice = {.name = "log", .write_r = log_put};

void host_plat_log_write(void) {
    static char out[sizeof sLog];
    mutexLock(&sLogFileLock);
    mutexLock(&sLogLock);
    size_t n = sLogLen, lost = sLogLost;
    memcpy(out, sLog, n);
    sLogLen = sLogLost = 0;
    mutexUnlock(&sLogLock);
    if (sLogFile) {
        fwrite(out, 1, n, sLogFile);
        if (lost) fprintf(sLogFile, "switch: the log's buffer was full: %zu bytes lost\n", lost);
        fflush(sLogFile);
    }
    mutexUnlock(&sLogFileLock);
}

// The options: there is no environment for a homebrew program, so acww.cfg's `NAME=value` lines (`#` comments)
// become one, as on the 3DS (plat_3ds.c), and every ACWW_* option of docs/host.md works unchanged.
void host_plat_init(void) {
    mutexInit(&sLogLock);
    mutexInit(&sLogFileLock);
    sLogFile = fopen(DIR "log.txt", "w");
    devoptab_list[STD_OUT] = devoptab_list[STD_ERR] = &sLogDevice;
    setvbuf(stdout, 0, _IONBF, 0);
    atexit(host_plat_log_write);
    printf("acww-switch %s\n", PORT_VERSION);
#ifdef ENHANCED_PORT  // (a save of acww.cfg cut short, options_cfg.c opt_save)
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
    // the game's files: the web builder's data pack, or the ROM image itself
    f = fopen(DIR "game.pak", "rb");
    if (f) fclose(f);
    setenv("ACWW_ROM", f ? DIR "game.pak" : DIR "game.nds", 0);
#ifdef ENHANCED_PORT
    setenv("ACWW_LANG_ROM", DIR "lang.nds", 0);  // (the EUR ROM, for ACWW_LANG: card.c)
#endif
}

uint64_t host_plat_time_ns(void) { return armTicksToNs(armGetSystemTick()); }

// (the 3DS's system tick and CPU counters: not on the Switch; the system tick at its own 19.2 MHz serves the
// geometry engine's timings, which only compare ticks with ticks)
uint64_t host_plat_ticks(void) { return armGetSystemTick(); }
int host_plat_pmu_start(int event0, int event1) {
    (void)event0, (void)event1;
    return 0;
}
void host_plat_pmu_stop(uint32_t count[3]) { count[0] = count[1] = count[2] = 0; }

void host_plat_sleep_until_ns(uint64_t t) {
    uint64_t now = host_plat_time_ns();
    if (now < t) svcSleepThread((s64)(t - now));
}

void host_plat_sleep_ms(unsigned ms) { svcSleepThread((s64)ms * 1000000); }

int host_plat_localtime(struct tm *out) {
    time_t now = time(0);
    return localtime_r(&now, out) != 0;
}

// shortcut: no restart in place on the Switch yet; the caller reports it and the program ends (back to the menu)
void host_plat_restart(char **argv) { (void)argv; }

struct Start { void (*fn)(void *); void *arg; Thread t; };
static void thread_main(void *p) {
    struct Start *s = p;
    s->fn(s->arg);  // (the Start stays: the thread's own memory is in it, and these threads run to the program's end)
}
static int start_on(void (*fn)(void *), void *arg, int prio, int core) {
    struct Start *s = calloc(1, sizeof *s);
    if (!s) return 0;
    s->fn = fn;
    s->arg = arg;
    if (R_FAILED(threadCreate(&s->t, thread_main, s, NULL, 64 * 1024, prio, core))) {
        free(s);
        return 0;
    }
    if (R_FAILED(threadStart(&s->t))) {
        threadClose(&s->t);
        free(s);
        return 0;
    }
    return 1;
}

// at a lower priority than the game's thread (a higher number), on the game's core
int host_plat_thread_start(void (*fn)(void *), void *arg) {
    s32 prio = 0x2c;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    return start_on(fn, arg, prio < 0x3f ? prio + 1 : prio, -2);
}

// on another core (an application has cores 0 to 2; the game's thread runs on core 0), at the game's priority; core1
// (the 3DS's core choice) is ignored
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1) {
    (void)core1;
    s32 prio = 0x2c;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    for (int core = 1; core <= 2; core++)
        if (start_on(fn, arg, prio, core)) {
            fprintf(stderr, "switch: a thread on core %d (the profile's sampler, the sound)\n", core);
            return 1;
        }
    return start_on(fn, arg, prio > 0 ? prio - 1 : prio, -2);
}

// (no guard page: memalign'd memory)
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

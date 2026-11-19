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

#include <arpa/inet.h>

#include <3ds.h>
#include <sys/iosupport.h>

#include "plat.h"

// The port's version, the log's first line: the Makefile writes port_version.h beside this file in build/upstream
// (a release's tag, else dev- and the commit); a build without the Makefile says "dev".
#if __has_include("port_version.h")
#include "port_version.h"
#else
#define PORT_VERSION "dev"
#endif

#define DIR "sdmc:/3ds/acww/"

#ifdef ENHANCED_PORT
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
    remove(DIR "log.prev.txt");  // (the last run's log kept: a test's own is often the one before a new start)
    rename(DIR "log.txt", DIR "log.prev.txt");
#ifdef ENHANCED_PORT  // (the enhanced port: written out by a thread of its own, hw.c log_writer)
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
    printf("acww-3ds %s\n", PORT_VERSION);
    // Opened by the HOME Menu version (cia/forwarder.c in the port's repository), which made its own title
    // Luma3DS's homebrew title so that the game runs in its place: this title is then both the homebrew title and
    // the one "selected" to be it next (Luma3DS's shared page). The selected one is put back to Luma3DS's own
    // (its kernel extension says which), so the forwarder's next start runs the forwarder again. Started from the
    // Homebrew Launcher this writes the value that is there. Not passed as an argument: main takes any as the ROM.
    {
        volatile u64 *hbldr = (volatile u64 *)(OS_SHAREDCFG_VADDR + 0x800);  // (the homebrew title, the selected one)
        u64 self = 0;
        s64 luma = 0, title = 0;
        if (R_SUCCEEDED(svcGetSystemInfo(&luma, 0x10000, 0)) && R_SUCCEEDED(svcGetSystemInfo(&title, 0x10000, 0x100)) &&
            title && R_SUCCEEDED(APT_GetProgramID(&self)) && hbldr[0] == self && hbldr[1] == self && self != (u64)title) {
            hbldr[1] = (u64)title;
            printf("3ds: opened by the HOME Menu version, in its own title (%016llX)\n", (unsigned long long)self);
        }
    }
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
    // local wireless (wm7.c): the game tells the players apart by their MAC address, which the PC build makes from
    // the machine's name and the save file's path - the same on every console. A console's own one instead, made
    // from its unique ID (CFGU_GenHashConsoleUnique: the same on every start, a different one on each console);
    // ACWW_MAC=xx:xx:xx:xx:xx:xx in acww.cfg sets it (two Azahar windows with the same console ID need that)
    if (!getenv("ACWW_MAC") && R_SUCCEEDED(cfguInit())) {
        u64 id = 0;
        if (R_SUCCEEDED(CFGU_GenHashConsoleUnique(0xacc0, &id)) && id) {
            char mac[18];  // (Nintendo's prefix, as wm7.c's own)
            snprintf(mac, sizeof mac, "00:09:bf:%02x:%02x:%02x", (unsigned)(id >> 16) & 0xff,
                     (unsigned)(id >> 8) & 0xff, (unsigned)id & 0xff);
            setenv("ACWW_MAC", mac, 1);
        }
        cfguExit();
    }
    // the game's files: the web builder's data pack, or (as before it) the ROM image itself
    f = fopen(DIR "game.pak", "rb");
    if (f) fclose(f);
    setenv("ACWW_ROM", f ? DIR "game.pak" : DIR "game.nds", 0);
#ifdef ENHANCED_PORT
    setenv("ACWW_LANG_ROM", DIR "lang.nds", 0);  // (the EUR ROM, for ACWW_LANG: card.c)
#endif
    bool n3ds = false;
    APT_CheckNew3DS(&n3ds);
#ifdef ENHANCED_PORT  // (the pictures between cost an Old 3DS about 17 ms a frame: off unless acww.cfg says)
    if (!n3ds) setenv("ACWW_PICA_INTERP", "0", 0);
#endif
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

uint64_t host_plat_ticks(void) { return svcGetSystemTick(); }

// The CPU's own event counters (the ARM11 MPCore's performance monitor: CP15 c15, c12: 0 the control register, 1 the
// cycle counter, 2 and 3 two event counters), for ACWW_G3D_PMU. They are reached in supervisor mode only, so through
// svcBackdoor, which Luma3DS gives a homebrew program; elsewhere the call fails or ends the program. The register
// layout (control: bit 0 on, bits 1 and 2 reset the counters, bits 20-27 and 12-19 the two events) is from ARM's
// manual as remembered: **unverified** until a run's numbers make sense (the cycles against the system ticks).
// (The user's Old 3DS, 2026-11-11: svcBackdoor fails there. Luma3DS's own call instead: svcCustomBackdoor, SVC 0x80,
// the function in r0: Luma3DS's sysmodules/rosalina/source/csvc.s and include/csvc.h. Not in libctru.)
Result luma_backdoor(s32 (*callback)(void));
__asm__(".text\n.arm\n.align 2\n.global luma_backdoor\n.type luma_backdoor, %function\nluma_backdoor:\n    svc 0x80\n    bx lr\n");
static u32 sPmuControl, sPmuCount[3];
static int sPmuRan, sPmuLuma;
static s32 pmu_start(void) {
    sPmuRan = 1;
    __asm__ volatile("mcr p15, 0, %0, c15, c12, 0" : : "r"(sPmuControl));
    return 0;
}
static s32 pmu_stop(void) {
    u32 off = 0;
    __asm__ volatile("mcr p15, 0, %0, c15, c12, 0" : : "r"(off));
    __asm__ volatile("mrc p15, 0, %0, c15, c12, 1" : "=r"(sPmuCount[0]));
    __asm__ volatile("mrc p15, 0, %0, c15, c12, 2" : "=r"(sPmuCount[1]));
    __asm__ volatile("mrc p15, 0, %0, c15, c12, 3" : "=r"(sPmuCount[2]));
    return 0;
}
int host_plat_pmu_start(int event0, int event1) {
    sPmuControl = (u32)event0 << 20 | (u32)event1 << 12 | 7;
    sPmuRan = 0;
    if (!sPmuLuma) svcBackdoor(pmu_start);
    if (!sPmuRan) {
        luma_backdoor(pmu_start);
        sPmuLuma = sPmuRan;
    }
    return sPmuRan;
}
void host_plat_pmu_stop(uint32_t count[3]) {
    if (sPmuLuma) luma_backdoor(pmu_stop);
    else svcBackdoor(pmu_stop);
    for (int i = 0; i < 3; i++) count[i] = sPmuCount[i];
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

// Local wireless (wm7.c): the DS's radio is UDP on the console's own Wi-Fi connection (the one set in System
// Settings). The socket service starts when the game first turns its wireless on (the gate), not at start-up: it
// takes SOC_BUF of the heap for good. 0: it couldn't (the reason in the log), and the game finds nobody.
#define SOC_BUF 0x40000
int port_net_start(void) {
    static int state;  // 0 not tried, 1 on, -1 failed
    if (state) return state > 0;
    state = -1;
    u32 *buf = memalign(0x1000, SOC_BUF);
    Result r = buf ? socInit(buf, SOC_BUF) : -1;
    if (R_FAILED(r)) {
        fprintf(stderr, "3ds: no socket service (socInit %08lx): no wireless\n", (unsigned long)r);
        free(buf);
        return 0;
    }
    atexit((void (*)(void))socExit);
    state = 1;
    long ip = gethostid();  // (network order; 0: the console isn't connected to a Wi-Fi network)
    if (!ip) fprintf(stderr, "3ds: wireless on, but no Wi-Fi connection: nobody to find\n");
    else fprintf(stderr, "3ds: wireless on, this console is %s on its Wi-Fi network\n",
                 inet_ntoa((struct in_addr){.s_addr = (u32)ip}));
    return 1;
}

// The Wi-Fi network's own broadcast address (network order; 0: none), asked again at most once a second (the
// connection can change): wm7.c sends its beacons there as well as to 255.255.255.255, which not every network
// passes on from a 3DS.
u32 port_net_broadcast(void) {
    static u32 addr;
    static u64 asked;
    u64 now = host_plat_time_ns();
    if (!asked || now - asked > 1000000000ull) {
        struct in_addr ip = {0}, mask = {0}, bcast = {0};
        addr = SOCU_GetIPInfo(&ip, &mask, &bcast) == 0 && ip.s_addr ? bcast.s_addr : 0;
        asked = now;
    }
    return addr;
}

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
// ends). The core is logged, to show whether the program can use one beside the game's. ACWW_CORE1_LIMIT=<percent>:
// core 1's share for the application, 5 to 89 (3dbrew: APT:SetApplicationCpuTimeLimit; the maximum depends on the
// exheader), default 30.
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1) {
    int pct = getenv("ACWW_CORE1_LIMIT") ? atoi(getenv("ACWW_CORE1_LIMIT")) : 30;
    pct = pct < 5 ? 5 : pct > 89 ? 89 : pct;
    Result limit = APT_SetAppCpuTimeLimit((u32)pct);
    s32 prio = 0x30;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    static const int kCores[3][3] = {{2, 1, -2}, {1, 2, -2}, {-2, -2, -2}};  // (-2: the game's core)
    for (int k = 0; k < 3; k++) {
        int c = kCores[core1 < 0 || core1 > 2 ? 0 : core1][k];
        struct Start *s = malloc(sizeof *s);
        if (!s) return 0;
        s->fn = fn;
        s->arg = arg;
        if (threadCreate(thread_main, s, 16 * 1024, c < 0 ? prio - 1 : 0x18, c, true)) {
            fprintf(stderr, "3ds: a thread on core %d (the profile's sampler, the sound; APT_SetAppCpuTimeLimit(%d): %08lx)\n",
                    c < 0 ? 0 : c, pct, (unsigned long)limit);
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

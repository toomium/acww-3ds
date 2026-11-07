// Host build runtime: platform services for Windows. See plat.h. (Not built or run yet: there is no Windows build;
// docs/portability.md 4.2, 4.4. Written for MinGW-w64 and the Win32 API of Windows 10.)
#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

#include "plat.h"

uint64_t host_plat_time_ns(void) {
    static LARGE_INTEGER freq;
    LARGE_INTEGER c;
    if (!freq.QuadPart) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&c);
    return (uint64_t)(c.QuadPart / freq.QuadPart) * 1000000000u +
           (uint64_t)(c.QuadPart % freq.QuadPart) * 1000000000u / (uint64_t)freq.QuadPart;
}

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

void host_plat_sleep_until_ns(uint64_t t) {
    // a high-resolution waitable timer (Windows 10 1803 and later), else an ordinary one; relative due times are
    // negative, in 100 ns units
    static HANDLE timer;
    if (!timer) timer = CreateWaitableTimerExW(0, 0, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!timer) timer = CreateWaitableTimerW(0, TRUE, 0);
    for (;;) {
        uint64_t now = host_plat_time_ns();
        if (now >= t) return;
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)((t - now + 99) / 100);
        if (!timer || !SetWaitableTimer(timer, &due, 0, 0, 0, FALSE)) {
            Sleep((DWORD)((t - now) / 1000000u));
            continue;
        }
        WaitForSingleObject(timer, INFINITE);
    }
}

void host_plat_sleep_ms(unsigned ms) { Sleep(ms); }

int host_plat_localtime(struct tm *out) {
    time_t now = time(0);
    return localtime_s(out, &now) == 0;
}

struct Start { void (*fn)(void *); void *arg; };
static DWORD WINAPI thread_main(LPVOID p) {
    struct Start s = *(struct Start *)p;
    free(p);
    s.fn(s.arg);
    return 0;
}
void host_plat_init(void) {}

void host_plat_restart(char **argv) {
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, sizeof path)) return;
    _execv(path, (const char *const *)argv);
}

int host_plat_thread_start(void (*fn)(void *), void *arg) {
    struct Start *s = malloc(sizeof *s);
    if (!s) return 0;
    s->fn = fn;
    s->arg = arg;
    HANDLE h = CreateThread(0, 0, thread_main, s, 0, 0);
    if (!h) { free(s); return 0; }
    CloseHandle(h);
    return 1;
}

// (the system shares the cores between threads by itself)
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1) { (void)core1; return host_plat_thread_start(fn, arg); }

static size_t page_size(void) {
    static size_t page;
    if (!page) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        page = si.dwPageSize;
    }
    return page;
}

// committed pages (no stack probing needed: coro_switch.S points the thread's stack bounds at them) with a
// no-access page below. (flags: none yet)
void *host_plat_stack_alloc(size_t n, int flags) {
    (void)flags;
    size_t page = page_size();
    n = (n + page - 1) & ~(page - 1);
    unsigned char *p = VirtualAlloc(0, n + page, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!p) return 0;
    DWORD old;
    VirtualProtect(p, page, PAGE_NOACCESS, &old);
    return p + page;
}

void host_plat_stack_free(void *stack, size_t n) {
    (void)n;
    if (stack) VirtualFree((unsigned char *)stack - page_size(), 0, MEM_RELEASE);
}

void *host_plat_pages_alloc(size_t n) {
    return VirtualAlloc(0, n, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

int host_plat_image_bounds(uintptr_t *lo, uintptr_t *hi) {
    // the PE headers of the program's own module (its base is the module handle)
    const unsigned char *base = (const unsigned char *)GetModuleHandleW(0);
    const IMAGE_NT_HEADERS *nt = (const IMAGE_NT_HEADERS *)(base + ((const IMAGE_DOS_HEADER *)base)->e_lfanew);
    *lo = (uintptr_t)base;
    *hi = (uintptr_t)base + nt->OptionalHeader.SizeOfImage;
    return 1;
}

int host_plat_backtrace(void **pcs, int n) {
    if (n > 62) n = 62;  // (the limit of RtlCaptureStackBackTrace before Windows Vista; harmless now)
    return CaptureStackBackTrace(1, (DWORD)n, pcs, 0);
}

// addresses only, with the program's load address: `addr2line -e acww.exe <pc - base + image base>`
void host_plat_print_backtrace(void) {
    void *bt[32];
    int n = host_plat_backtrace(bt, 32);
    fprintf(stderr, "backtrace (program loaded at %p):\n", (void *)GetModuleHandleW(0));
    for (int i = 0; i < n; i++) fprintf(stderr, "  %p\n", bt[i]);
}

#endif

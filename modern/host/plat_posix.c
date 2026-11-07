// Host build runtime: platform services for POSIX systems (Linux, macOS). See plat.h.
#ifndef _WIN32

#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#if defined(__GLIBC__) || defined(__APPLE__)
#include <execinfo.h>
#define HAVE_EXECINFO 1
#endif

#include "plat.h"

void host_plat_init(void) {}

uint64_t host_plat_time_ns(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000000u + (uint64_t)t.tv_nsec;
}

void host_plat_sleep_until_ns(uint64_t t) {
#if defined(__linux__)
    // an absolute deadline: no drift from the time spent between computing it and sleeping
    struct timespec ts = {(time_t)(t / 1000000000u), (long)(t % 1000000000u)};
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, 0) == EINTR) {}
#else
    // macOS has no clock_nanosleep: relative sleeps until the deadline has passed
    for (;;) {
        uint64_t now = host_plat_time_ns();
        if (now >= t) return;
        uint64_t d = t - now;
        struct timespec ts = {(time_t)(d / 1000000000u), (long)(d % 1000000000u)};
        nanosleep(&ts, 0);
    }
#endif
}

void host_plat_sleep_ms(unsigned ms) { host_plat_sleep_until_ns(host_plat_time_ns() + (uint64_t)ms * 1000000u); }

int host_plat_localtime(struct tm *out) {
    time_t now = time(0);
    return localtime_r(&now, out) != 0;
}

struct Start { void (*fn)(void *); void *arg; };
static void *thread_main(void *p) {
    struct Start s = *(struct Start *)p;
    free(p);
    s.fn(s.arg);
    return 0;
}
void host_plat_restart(char **argv) {
#ifdef __linux__
    execv("/proc/self/exe", argv);
#endif
    execvp(argv[0], argv);
}

int host_plat_thread_start(void (*fn)(void *), void *arg) {
    struct Start *s = malloc(sizeof *s);
    if (!s) return 0;
    s->fn = fn;
    s->arg = arg;
    // no signals for the new thread (it inherits this mask): the debugging handlers (debug_posix.c) look at the
    // game's thread
    sigset_t all, old;
    sigfillset(&all);
    pthread_sigmask(SIG_SETMASK, &all, &old);
    pthread_t t;
    int err = pthread_create(&t, 0, thread_main, s);
    pthread_sigmask(SIG_SETMASK, &old, 0);
    if (err) { free(s); return 0; }
    pthread_detach(t);
    return 1;
}

// (the system shares the cores between threads by itself)
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1) { (void)core1; return host_plat_thread_start(fn, arg); }

static size_t page_size(void) {
    static size_t page;
    if (!page) page = (size_t)sysconf(_SC_PAGESIZE);
    return page;
}

// the stack's pages with one more below them that faults (an overflow is a crash, not a corruption)
void *host_plat_stack_alloc(size_t n, int flags) {
    size_t page = page_size();
    n = (n + page - 1) & ~(page - 1);
    (void)flags;
    unsigned char *p = mmap(0, n + page, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) return 0;
    mprotect(p, page, PROT_NONE);
    return p + page;
}

void host_plat_stack_free(void *stack, size_t n) {
    if (!stack) return;
    size_t page = page_size();
    n = (n + page - 1) & ~(page - 1);
    munmap((unsigned char *)stack - page, n + page);
}

void *host_plat_pages_alloc(size_t n) {
    void *p = mmap(0, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return p == MAP_FAILED ? 0 : p;
}

int host_plat_image_bounds(uintptr_t *lo, uintptr_t *hi) {
#if defined(__ELF__)
    extern char __executable_start[], _end[];  // (the linkers' symbols: GNU ld, lld, gold)
    *lo = (uintptr_t)__executable_start;
    *hi = (uintptr_t)_end;
    return 1;
#else
    (void)lo; (void)hi;
    return 0;
#endif
}

int host_plat_backtrace(void **pcs, int n) {
#if HAVE_EXECINFO
    return backtrace(pcs, n);
#else
    (void)pcs; (void)n;
    return 0;
#endif
}

void host_plat_print_backtrace(void) {
#if HAVE_EXECINFO
    void *bt[32];
    int n = backtrace(bt, 32);
    backtrace_symbols_fd(bt, n, 2);
#endif
}

#endif

// Host build runtime: the operating-system services the runtime needs (docs/portability.md 1.4, 4.2). One
// implementation per platform: plat_posix.c (Linux, macOS), plat_win32.c (Windows); the parts that are plain C
// are in plat_common.c. Nothing else in modern/host calls the operating system directly, except the POSIX-only
// debugging aids (debug_posix.c, guards.c), the i386 viewer link (frontend_viewer.c), the low-4 GB harness
// (hw.c, ACWW_HIGHMEM: x86-64 Linux only) and the wireless's UDP sockets (wm7.c, POSIX and Winsock).
#ifndef HOST_PLAT_H
#define HOST_PLAT_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

// ---- called first in main: what the platform sets up before anything else (the 3DS: the options from a file,
// the log); nothing elsewhere
void host_plat_init(void);

// ---- time
uint64_t host_plat_time_ns(void);           // a monotonic clock in nanoseconds (any epoch)
void host_plat_sleep_until_ns(uint64_t t);  // sleep until host_plat_time_ns() >= t
void host_plat_sleep_ms(unsigned ms);
void host_plat_log_write(void);  // (the 3DS's direct build: the log's lines buffered so far to the SD card)
// the local (wall-clock) time now, broken down as localtime_r would; 0 if it isn't known
int host_plat_localtime(struct tm *out);

// ---- the program started again in place (the same arguments and environment: the DS's system reset); returns only
// if it couldn't be
void host_plat_restart(char **argv);

// ---- threads: a detached background thread (the watchdog); 0 if it couldn't be started
int host_plat_thread_start(void (*fn)(void *), void *arg);
// a thread that runs while the game's thread is busy (ACWW_FPS's sampler): on another core, or where there is only
// one, at a higher priority; 0 if it couldn't be started. core1: the 3DS's system core (core 1) tried first, before
// the New 3DS's core 2 (elsewhere ignored)
int host_plat_sampler_start(void (*fn)(void *), void *arg, int core1);

// ---- memory
// A stack of n bytes (rounded up to whole pages) for a coroutine (coro.h), with an inaccessible guard page below it
// where the system can do that (flags: none yet, 0). 0 if there is no memory.
void *host_plat_stack_alloc(size_t n, int flags);
void host_plat_stack_free(void *stack, size_t n);
// n bytes of zeroed, page-aligned memory (anywhere), never freed; 0 if there is no memory.
void *host_plat_pages_alloc(size_t n);

// ---- the program image: [*lo, *hi) holds the program's code, data and bss; 0 where that isn't known
int host_plat_image_bounds(uintptr_t *lo, uintptr_t *hi);

// ---- files
// The whole file in memory (malloc'd, freed with free); *size its length. 0 (and errno / GetLastError set) if it
// can't be read.
void *host_plat_read_file(const char *path, size_t *size);

// ---- crash reports
// The calling thread's return addresses, innermost first: up to n into pcs; the count (0 where the platform can't).
int host_plat_backtrace(void **pcs, int n);
// The same, written to stderr (with symbol names where the platform has them).
void host_plat_print_backtrace(void);

#endif

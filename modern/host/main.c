// Host build: entry point. Maps the DS memory, puts what the DS boot leaves in memory (ROM header, system work
// area: include/host_mem.h) in place and calls the game's NitroMain. See docs/host.md.

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coro.h"
#include "host.h"
#include "render.h"
#include "host_mem.h"
#include "overlay.h"
#include "plat.h"

int gHostVerbose;
const char *gHostRomPath;

void host_fatal(const char *msg) {
    fprintf(stderr, "host: fatal: %s (frame %llu)\n", msg, (unsigned long long)host_frame());
    extern void frontend_shutdown(void);
    frontend_shutdown();
    exit(2);
}

void host_log_proc(u32 profile, void *create, void *obj) {
    static int on = -1;
    if (on < 0) on = getenv("ACWW_PROCLOG") != 0;
    if (on) fprintf(stderr, "proc: frame %llu profile %u create %p -> %p\n", (unsigned long long)host_frame(), profile, create, obj);
}

// is p inside the program's image (code, data, bss)? (1 where the platform doesn't say: plat.h)
int host_in_program_image(const void *p) {
    uintptr_t lo, hi;
    if (!host_plat_image_bounds(&lo, &hi)) return 1;
    return (uintptr_t)p >= lo && (uintptr_t)p < hi;
}

void host_bad_object(u32 profile, void *create, void *obj) {
    fprintf(stderr, "host: profile %u: create %p returned %p without a vtable\n", profile, create, obj);
}

// Fatal_PanicV (HOST): the game's panic message; args points at the format's arguments on the stack
void host_panic(const char *file, u32 line, const char *fmt, void *args) {
    char buf[512];
#if defined(__aarch64__) && !defined(__APPLE__)
    // (AAPCS64's va_list is a struct, not a pointer to the arguments: only the format can be shown)
    (void)args;
    snprintf(buf, sizeof buf, "%s", fmt);
#else
    vsnprintf(buf, sizeof buf, fmt, *(va_list *)&args);
#endif
    fprintf(stderr, "host: game panic: %s (%s:%u)\n", buf, file ? file : "", line);
    host_fatal("panic");
}

// Fatal_Panic (HOST): the arguments as a va_list
void host_panic_v(const char *file, u32 line, const char *fmt, va_list *args) {
    char buf[512];
    vsnprintf(buf, sizeof buf, fmt, *args);
    fprintf(stderr, "host: game panic: %s (%s:%u)\n", buf, file ? file : "", line);
    host_fatal("panic");
}

void host_unimplemented(const char *what) {
    fprintf(stderr, "host: unimplemented: %s\n", what);
}

const u8 *host_rom(u32 *size);
void NitroMain(void);
void NitroStartUp(void);

// before the game's static constructors (which may touch DS memory): map it first
void host_vram_init(void);
#include "guards.h"
__attribute__((constructor(101))) static void early_init(void) {
    HostMem_Init();
    host_map_memory();
    host_vram_init();
    host_guards_init();   // (the guard variant: configure_host.py --guards)
    host_highmem_init();  // ACWW_HIGHMEM: the low 4 GB reserved from here on
}

static void game_main(void *arg) {
    (void)arg;
    NitroStartUp();  // crt0 calls it before the static initialisers; they ran already (host .init_array)
    NitroMain();
    host_fatal("NitroMain returned");
}

char **gHostArgv;  // (host_reset_system: the program starts again with them)
int main(int argc, char **argv) {
    gHostArgv = argv;
    host_plat_init();
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) gHostVerbose = 1;
        else gHostRomPath = argv[i];
    }
    if (!gHostRomPath) gHostRomPath = getenv("ACWW_ROM");  // (the 3DS: no arguments, plat_3ds.c)
    if (!gHostRomPath) { fprintf(stderr, "usage: %s [-v] <rom.nds>\n", argv[0]); return 1; }
    HostOverlay_Init();  // the overlays' variables as they are now (overlay.c)
    card_init(gHostRomPath);
    u32 size;
    const u8 *rom = host_rom(&size);
    // the ROM header (HW_ROM_HEADER_BUF, 0x160 bytes, and the DS boot's next 0x10: the system work area's
    // isdReserved), a stand-in for the card id (HW_BOOT_CHECK_INFO_BUF), the boot type (HW_WM_BOOT_BUF): ROM
    _Static_assert(sizeof gHostSysWork.romHeader + sizeof gHostSysWork.isdReserved >= 0x170, "ROM header");
    memcpy(gHostSysWork.romHeader, rom, sizeof gHostSysWork.romHeader);
    memcpy(gHostSysWork.isdReserved, rom + sizeof gHostSysWork.romHeader, 0x170 - sizeof gHostSysWork.romHeader);
    memcpy(gHostSysWork.bootCheckInfo, rom + 0x0c, 4);
    *(volatile u16 *)gHostSysWork.wmBootBuf = 1;
    pxi7_init();
    render_init();
    host_debug_crash_handlers();  // (melonDS's JIT replaced the SIGSEGV handler)
    extern void frontend_init(const char *argv0);
    extern void frontend_shutdown(void);
    frontend_init(argv[0]);
    atexit(frontend_shutdown);
    // the game's main thread on a stack of its own (os.c: below 4 GB on 64-bit Linux)
    static void *host_sp;
    enum { MAIN_STACK = 1024 * 1024 };
    void *stack = host_stack_alloc(MAIN_STACK);
    if (!stack) host_fatal("no memory for the main thread's stack");
    host_coro_switch(&host_sp, host_coro_prepare(stack, MAIN_STACK, game_main, 0));
    host_fatal("the game's main thread returned");
}

// ACWW_HEAPCHECK=1: walk every NNS expanded heap in the game's heap memory (the main RAM block; on 64-bit hosts
// the root heap's block) each frame (hw.c) and report the first broken block. The NNS heap header and block header
// (src/autoload_2/unk_02100d44.c), pointer-sized as there
struct HeapCheckLink { void *prev, *next; };
struct HeapCheckBlock { u16 sig, attr; u32 size; struct HeapCheckBlock *prev, *next; };
struct HeapCheckHead {
    u32 sig;
    struct HeapCheckLink link;
    struct { void *head, *tail; u16 num, offset; } childList;
    uintptr_t start, end;
    u32 attr;
    struct HeapCheckBlock *freeHead, *freeTail, *usedHead, *usedTail;
};
static uintptr_t sRootHeapLo, sRootHeapHi;
static int heap_check_range(const u8 *lo, const u8 *hi, int *heaps) {
    for (const u8 *a = lo; a + sizeof(struct HeapCheckHead) <= hi; a += 4) {
        const struct HeapCheckHead *h = (const void *)a;
        if (h->sig != 0x45585048 || h->start < (uintptr_t)lo || h->start >= h->end || h->end > (uintptr_t)hi) continue;
        (*heaps)++;
        struct HeapCheckBlock *lists[2] = {h->freeHead, h->usedHead};
        for (int l = 0; l < 2; l++) {
            struct HeapCheckBlock *prev = 0;
            u32 n = 0;
            for (struct HeapCheckBlock *m = lists[l]; m && n < 100000; n++) {
                if ((uintptr_t)m < h->start || (uintptr_t)m >= h->end || m->sig != (l ? 0x5544 : 0x4652) || m->prev != prev) {
                    fprintf(stderr, "heapcheck: frame %llu heap %p %s block %p bad (sig %04x prev %p want %p)\n",
                            (unsigned long long)host_frame(), (const void *)h, l ? "used" : "free", (void *)m,
                            ((uintptr_t)m >= h->start && (uintptr_t)m < h->end) ? m->sig : 0,
                            ((uintptr_t)m >= h->start && (uintptr_t)m < h->end) ? (void *)m->prev : 0, (void *)prev);
                    return 1;
                }
                prev = m;
                m = m->next;
            }
        }
    }
    return 0;
}
void host_heap_check(void) {
    static int on = -1, reported;
    if (on < 0) on = getenv("ACWW_HEAPCHECK") != 0;
    if (!on || reported) return;
    int heaps = 0;
    reported = heap_check_range(gHostMainRam, gHostMainRam + sizeof gHostMainRam, &heaps) ||
               (sRootHeapHi > sRootHeapLo && heap_check_range((const u8 *)sRootHeapLo, (const u8 *)sRootHeapHi, &heaps));
    if (!reported && host_frame() % 600 == 0)
        fprintf(stderr, "heapcheck: frame %llu: %d heaps fine\n", (unsigned long long)host_frame(), heaps);
}

// The root heap's memory on 64-bit hosts (Heap_InitSystem): the game's objects are larger there (pointers), so the
// DS's main RAM arena (about 1.4 MB for the root heap) runs out; the heap gets a larger block of its own instead
// (anywhere: whole pages, host_plat_pages_alloc).

// ::operator delete (newdelete.cpp): is p a block of the game's heaps? They are in the game's memory blocks
// (include/host_mem.h: the main RAM block and the arenas), the game's data and bss (a heap in a static buffer) or,
// on 64-bit hosts, the root heap's block
int host_is_game_heap(const void *p) {
    return HostMem_IsGameRam(p, 1);
}
void host_root_heap_bounds(uintptr_t *lo, uintptr_t *hi) {
    *lo = sRootHeapLo;
    *hi = sRootHeapHi;
}

void *host_root_heap_region(unsigned int *size) {
    const unsigned int kSize = 8u << 20;
    void *p = host_plat_pages_alloc(kSize);
    if (!p) return 0;
    *size = kSize;
    sRootHeapLo = (uintptr_t)p;
    sRootHeapHi = (uintptr_t)p + kSize;
    return p;
}

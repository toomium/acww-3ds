// Host build: host_coro_prepare (coro.h): the frame host_coro_switch pops (coro_switch.S), on a new stack.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HOST_CORO_IMPL
#include "coro.h"

void host_coro_boot(void);

// x86-64 Windows: the 256-byte frame of coro_switch.S (32 words: xmm6-15, StackLimit, StackBase, DeallocationStack,
// r15 r14 r13 r12 rdi rsi rbx rbp, return address), with the stack's own bounds for the TIB.
// (HOST_CORO_WIN64_TEST: the same for the Linux test of that code, modern/host/tests/coro_win64_test.c.)
#if defined(__x86_64__) && (defined(_WIN32) || defined(HOST_CORO_WIN64_TEST))
static void *prepare_win64(void *stack, size_t size, void (*fn)(void *), void *arg, void (*boot)(void)) {
    uintptr_t top = ((uintptr_t)stack + size) & ~(uintptr_t)15;
    uint64_t *sp = (uint64_t *)top - 32;   // sp = top after the frame is popped
    memset(sp, 0, 32 * sizeof *sp);
    sp[20] = (uint64_t)(uintptr_t)stack;   // StackLimit: the lowest usable address (all of the stack is committed)
    sp[21] = (uint64_t)top;                // StackBase
    sp[22] = (uint64_t)(uintptr_t)stack;   // DeallocationStack
    sp[25] = (uint64_t)(uintptr_t)arg;     // r13
    sp[26] = (uint64_t)(uintptr_t)fn;      // r12
    sp[31] = (uint64_t)(uintptr_t)boot;
    return sp;
}
#endif
#if defined(__x86_64__) && defined(HOST_CORO_WIN64_TEST)
void host_coro_boot_w64(void);
void *host_coro_prepare_w64(void *stack, size_t size, void (*fn)(void *), void *arg) {
    return prepare_win64(stack, size, fn, arg, host_coro_boot_w64);
}
#endif

#ifdef HOST_ASAN
#include <sanitizer/common_interface_defs.h>

// The stacks host_coro_prepare was given (newest first), and the stack the process started on (learnt at the first
// switch away from it): ASan wants the bounds of the stack a switch goes to.
typedef struct AsanStack {
    struct AsanStack *next;
    const char *lo;
    size_t size;
    void (*fn)(void *);
    void *arg;
} AsanStack;
static AsanStack *sAsanStacks;
static const void *sAsanOuterLo;
static size_t sAsanOuterSize;

static void asan_stack_of(const void *sp, const void **lo, size_t *size) {
    for (AsanStack *s = sAsanStacks; s; s = s->next)
        if ((const char *)sp >= s->lo && (const char *)sp < s->lo + s->size) {
            *lo = s->lo;
            *size = s->size;
            return;
        }
    *lo = sAsanOuterLo;
    *size = sAsanOuterSize;
}

static void asan_note_from(const void *lo, size_t size) {
    if (!lo) return;
    for (AsanStack *s = sAsanStacks; s; s = s->next)
        if ((const char *)lo >= s->lo && (const char *)lo < s->lo + s->size) return;
    sAsanOuterLo = lo;
    sAsanOuterSize = size;
}

static void asan_boot(void *arg) {
    AsanStack *s = arg;
    const void *lo;
    size_t size;
    __sanitizer_finish_switch_fiber(0, &lo, &size);
    asan_note_from(lo, size);
    s->fn(s->arg);
}

void host_coro_switch_asan(void **save, void *to) {
    const void *lo, *from_lo;
    size_t size, from_size;
    void *fake = 0;
    asan_stack_of(to, &lo, &size);
    __sanitizer_start_switch_fiber(&fake, lo, size);
    host_coro_switch(save, to);
    __sanitizer_finish_switch_fiber(fake, &from_lo, &from_size);
    asan_note_from(from_lo, from_size);
}

static void *prepare_raw(void *stack, size_t size, void (*fn)(void *), void *arg);
void *host_coro_prepare(void *stack, size_t size, void (*fn)(void *), void *arg) {
    AsanStack *s = calloc(1, sizeof *s);
    if (!s) abort();
    s->lo = stack;
    s->size = size;
    s->fn = fn;
    s->arg = arg;
    s->next = sAsanStacks;
    sAsanStacks = s;
    return prepare_raw(stack, size, asan_boot, s);
}
#define host_coro_prepare prepare_raw
static
#endif
void *host_coro_prepare(void *stack, size_t size, void (*fn)(void *), void *arg) {
#if defined(__x86_64__) && defined(_WIN32)
    return prepare_win64(stack, size, fn, arg, host_coro_boot);
#else
    uintptr_t top = ((uintptr_t)stack + size) & ~(uintptr_t)15;
#if defined(__i386__)
    // edi esi ebx ebp ret, then host_coro_boot runs with esp = top (16-aligned)
    uint32_t *sp = (uint32_t *)top - 5;
    sp[0] = 0;                              // edi
    sp[1] = (uint32_t)(uintptr_t)arg;       // esi
    sp[2] = (uint32_t)(uintptr_t)fn;        // ebx
    sp[3] = 0;                              // ebp
    sp[4] = (uint32_t)(uintptr_t)host_coro_boot;
    return sp;
#elif defined(__x86_64__)
    // r15 r14 r13 r12 rbx rbp ret; after the pops and the ret rsp = top (16-aligned before the boot's call)
    uint64_t *sp = (uint64_t *)top - 7;
    memset(sp, 0, 7 * sizeof *sp);
    sp[2] = (uint64_t)(uintptr_t)arg;       // r13
    sp[3] = (uint64_t)(uintptr_t)fn;        // r12
    sp[6] = (uint64_t)(uintptr_t)host_coro_boot;
    return sp;
#elif defined(__aarch64__)
    uint64_t *sp = (uint64_t *)top - 22;    // the 176-byte frame; sp = top after it is popped
    memset(sp, 0, 22 * sizeof *sp);
    sp[0] = (uint64_t)(uintptr_t)fn;        // x19
    sp[1] = (uint64_t)(uintptr_t)arg;       // x20
    sp[11] = (uint64_t)(uintptr_t)host_coro_boot;  // x30
    return sp;
#elif defined(__arm__)
    uint32_t *sp = (uint32_t *)top - 26;    // d8-d15 (16 words), r4-r12, lr; sp = top after they are popped
    memset(sp, 0, 26 * sizeof *sp);
    sp[16] = (uint32_t)(uintptr_t)fn;       // r4
    sp[17] = (uint32_t)(uintptr_t)arg;      // r5
    sp[25] = (uint32_t)(uintptr_t)host_coro_boot;  // lr
    return sp;
#else
#error "coro.c: no stack switch for this CPU yet"
#endif
#endif
}

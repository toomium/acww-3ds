// Host build: the parts of the NitroSDK OS that are assembly on the DS - interrupt masking and dispatch, thread
// contexts, lock ids, BIOS waits. Interrupts are delivered at safe points (enabling interrupts, halting, waiting),
// never in the middle of game code.

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plat.h"
#include "coro.h"
#include "host.h"
#include "host_mem.h"
#include "sys/OSThread.h"  // the game's thread structures (their layout differs on 64-bit hosts: no fixed offsets)

// ---- interrupt masking (the CPSR I bit)
static u32 sIrqDisabled = 0x80;  // as at reset: disabled until OS_EnableInterrupts
static int sInIrq;

static void deliver_if_enabled(void) {
    if (!sIrqDisabled && !sInIrq) host_deliver_irqs();
}
u32 OS_DisableInterrupts(void) { u32 old = sIrqDisabled; sIrqDisabled = 0x80; return old; }
u32 OS_EnableInterrupts(void) { u32 old = sIrqDisabled; sIrqDisabled = 0; deliver_if_enabled(); return old; }
u32 OS_RestoreInterrupts(u32 state) { u32 old = sIrqDisabled; sIrqDisabled = state & 0x80; deliver_if_enabled(); return old; }
u32 OS_DisableInterrupts_IrqAndFiq(void) { return OS_DisableInterrupts(); }
u32 OS_RestoreInterrupts_IrqAndFiq(u32 state) { return OS_RestoreInterrupts(state); }
u32 OS_GetProcMode(void) { return sInIrq ? 0x12 : 0x1f; }
u32 host_irq_state(void) { return sIrqDisabled | sInIrq; }

// ---- dispatch (OS_IrqHandler + OS_IrqHandler_ThreadSwitch)
extern void (*data_027e0000[])(void);   // OS_IRQTable
extern OSThreadInfo data_021fcc2c;      // OSi_ThreadInfo
extern OSThreadQueue data_027e0450;     // OSi_IrqThreadQueue
void OSi_RescheduleThread(void);

void host_deliver_irqs(void) {
    if (sInIrq) return;
    HOST_PROF(PROF_GAME);  // (the handlers are the game's)
    for (;;) {
        u32 pending = host_irq_pending();
        if (!pending || sIrqDisabled) break;
        int bit = __builtin_ctz(pending);
        host_irq_ack(1u << bit);
        sInIrq = 1;
        sIrqDisabled = 0x80;
        if (data_027e0000[bit]) data_027e0000[bit]();
        sIrqDisabled = 0;
        sInIrq = 0;
        // wake the threads waiting for an interrupt (OS_WaitIrq)
        OSThread *t = data_027e0450.head;
        if (t) {
            while (t) {
                OSThread *next = t->linkNext;
                t->state = 1;  // ready
                t->queue = 0;
                t->linkPrev = 0;
                t->linkNext = 0;
                t = next;
            }
            data_027e0450.head = data_027e0450.tail = 0;
            data_021fcc2c.isNeedRescheduling = 1;
        }
    }
    if (data_021fcc2c.isNeedRescheduling) {
        data_021fcc2c.isNeedRescheduling = 0;
        OSi_RescheduleThread();
    }
}

void OS_IrqHandler(void) { host_deliver_irqs(); }

// one interrupt's handler (OS_IRQTable entry `bit`) at once, in interrupt mode, for an interrupt the host raises in the
// middle of its own work (the H-blank: hw.c host_hblank); the handler wakes no thread
void host_irq_call(int bit) {
    if (!data_027e0000[bit]) return;
    u32 disabled = sIrqDisabled;
    int inIrq = sInIrq;
    sInIrq = 1;
    sIrqDisabled = 0x80;
    data_027e0000[bit]();
    sIrqDisabled = disabled;
    sInIrq = inIrq;
}

int host_irq_ready(void) { return !sIrqDisabled && !sInIrq && host_irq_pending(); }
void OSi_ExceptionHandler(void) { host_fatal("OSi_ExceptionHandler"); }

// ---- thread contexts: a host stack per OSContext (coro.h), in a side table. The game's thread stacks (in DS
// memory) aren't used for the host code; each thread runs on its own host stack.
#define MAX_CTX 64
#define HOST_STACK (512 * 1024)
// sp: the suspended stack (host_coro_switch); irq: the thread's CPSR I bit while it isn't running (each thread
// has its own, as on the DS)
typedef struct { void *key; void *sp; u8 *stack; uptr pc; int fresh; u32 irq; } HostCtx;
static HostCtx sCtx[MAX_CTX];
static void *sSaving;  // the thread whose OS_SaveContext is pending

// A stack for the game's code (anywhere; host_plat_stack_alloc: with a guard page below it)
void *host_stack_alloc(size_t n) {
    return host_plat_stack_alloc(n, 0);
}

static HostCtx *ctx_of(void *key) {
    HostCtx *free_ = 0;
    for (int i = 0; i < MAX_CTX; i++) {
        if (sCtx[i].key == key) return &sCtx[i];
        if (!sCtx[i].key && !free_) free_ = &sCtx[i];
    }
    if (!free_) host_fatal("too many thread contexts");
    free_->key = key;
    return free_;
}

static void trampoline(void *key) {
    OSContext *c = key;
    HostCtx *h = ctx_of(key);
    void (*f)(void *) = (void (*)(void *))h->pc;
    f((void *)c->r[0]);
    ((void (*)(void))c->r[14])();  // lr: OS_ExitThread
    host_fatal("thread returned past its exit function");
}

void OS_InitContext(OSContext *c, uptr pc, uptr sp) {
    HostCtx *h = ctx_of(c);
    memset(c, 0, sizeof *c);
    c->r[13] = sp;
    h->pc = pc;
    h->fresh = 1;
    (void)sp;
}

int OS_SaveContext(void *t) { sSaving = t; return 0; }

void OS_LoadContext(void *t) {
    HostCtx *next = ctx_of(t);
    if (next->fresh) {
        next->fresh = 0;
        next->irq = ((OSContext *)t)->cpsr & 0x80;
        if (!next->stack) next->stack = host_stack_alloc(HOST_STACK);
        if (!next->stack) host_fatal("no memory for a thread stack");
        next->sp = host_coro_prepare(next->stack, HOST_STACK, trampoline, t);
    }
    if (sSaving) {
        HostCtx *cur = ctx_of(sSaving);
        sSaving = 0;
        cur->irq = sIrqDisabled;
        sIrqDisabled = next->irq;
        host_coro_switch(&cur->sp, next->sp);
        return;  // resumed (sIrqDisabled was set by whoever switched back): the caller returns as if OS_SaveContext
                 // had returned 1
    }
    // the current thread is gone (exited, destroyed): its stack isn't kept
    static void *discarded;
    sIrqDisabled = next->irq;
    host_coro_switch(&discarded, next->sp);
    host_fatal("a discarded thread was resumed");
}

// for the SIGINT report (debug_posix.c): where each suspended game thread is (a frame-pointer walk from the frame
// host_coro_switch left: the callee-saved registers, ebp / rbp among them)
void host_dump_threads(void (*backtrace)(uintptr_t *bp, uintptr_t sp)) {
    for (int i = 0; i < MAX_CTX; i++) {
        if (!sCtx[i].key || !sCtx[i].sp) continue;
        OSThread *t = sCtx[i].key;
        uintptr_t *sp = sCtx[i].sp;
#if defined(__x86_64__)
        uintptr_t bp = sp[5];  // r15 r14 r13 r12 rbx rbp
#elif defined(__i386__)
        uintptr_t bp = sp[3];  // edi esi ebx ebp
#elif defined(__arm__)
        uintptr_t bp = sp[23];  // d8-d15, r4-r10, r11
#else
        uintptr_t bp = sp[10];  // x19..x28, x29
#endif
        fprintf(stderr, "thread %p (id %u, state %u, priority %u, queue %p, sp %p bp %p):\n", (void *)t, (unsigned)t->id, (unsigned)t->state,
                (unsigned)t->priority, (void *)t->queue, (void *)sp, (void *)bp);
        backtrace((uintptr_t *)bp, (uintptr_t)sp);
    }
}

// ---- lock ids (ARM9: 0x40..0x7f)
static u64 sLockIds;
u32 OS_GetLockID(void) {
    for (int i = 0; i < 64; i++) if (!(sLockIds >> i & 1)) { sLockIds |= 1ull << i; return 0x40 + i; }
    return (u32)-3;
}
void OS_ReleaseLockID(u16 id) { if (id >= 0x40 && id < 0x80) sLockIds &= ~(1ull << (id - 0x40)); }
void OS_UnLockCartridge(u32 id) { (void)id; }

// ---- waits
void OS_Halt(void) { host_advance_to_next_event(); host_deliver_irqs(); }
void VBlankIntrWait(void) {
    volatile u32 *check = HW_INTR_CHECK_BUF;  // (DTCM + 0x3ff8)
    while (!(*check & 1)) {
        host_advance_to_next_event();
        u32 old = sIrqDisabled;
        sIrqDisabled = 0;
        host_deliver_irqs();
        sIrqDisabled = old;
    }
    *check &= ~1u;
}

// ---- misc
void Fatal_SaveRegisters(void) {}
void AutoloadCallback(void) {}
void OSi_DoBoot(void) { host_fatal("OSi_DoBoot (reset)"); }
void __TransferControl(void *a, void *b, char *c) { (void)a; (void)b; (void)c; host_fatal("C++ exception"); }
void sys_exit(void) { host_quit(); }
int sys_readc(void) { return getchar(); }
void sys_writec(const u8 *p) { fputc(*p, stderr); }

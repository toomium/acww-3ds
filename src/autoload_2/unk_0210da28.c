#include "ds_div.h"
#include "nnsys/snd_arcstrm.h"
#include "proto/autoload_2/unk_0210b448.h"
#include "proto/autoload_2/unk_0210c084.h"
#include "proto/autoload_2/unk_0210da28.h"
#include "proto/autoload_2/unk_0210ec0c.h"
#include "proto/autoload_2/unk_02100234.h"
#include "proto/autoload_2/unk_02113088.h"
#include "proto/autoload_2/unk_02114410.h"
#include "proto/autoload_2/unk_02119434.h"
// mwcc-flags: -nothumb -O4,p
// NitroSystem (NNS) sound library tail: sound-archive stream player (NNS_SndArcStrm*: thread, job queue,
// stream contexts), capture effects (NNS_SndCapture*) and the NNSiSndFader helpers.
// autoload_2 0x0210da28-0x0210ec0c (the rest of output_effect.c is unk_0210ec0c.c). ARM, mwcc 1.2/base, -O4,p.
#include "types64.h"

typedef struct EffCtl {
    s32 type;               // 0x00
    void (*fn)();           // 0x04
    void (*pre)();          // 0x08
    void *preArg;           // 0x0c
    void (*post)();         // 0x10
    void *postArg;          // 0x14
    union {
        struct { s32 a; s32 b; } e[24];
        s32 w[48];
    } u;                    // 0x18
} EffCtl;

// the stream library (unk_0210a478.c, unk_0210a9c4.c): its NNSSndStrm is the player's first member
typedef struct NNSSndStrm NNSSndStrm;
static inline NNSSndStrm *PlayerStrm(NNSSndStrmPlayer *c) { return (NNSSndStrm *)c->stream; }
typedef void (*NNSSndStrmBlockCallback)(s32 status, s32 numChannels, NNSSndStrmBufAddr *buffer, u32 len, s32 format,
                                        void *arg);
extern BOOL NNS_SndStrmSetup(NNSSndStrm *st, s32 fmt, uptr buf, u32 size, s32 timer, s32 interval,
                             NNSSndStrmBlockCallback cb, void *arg);
extern void NNS_SndStrmStart(NNSSndStrm *st);
extern void NNS_SndStrmStop(NNSSndStrm *st);
extern void NNS_SndStrmSetVolume(NNSSndStrm *st, u32 vol);
extern void NNS_SndStrmSetChannelPan(NNSSndStrm *st, s32 ch, u32 pan);
extern void NNS_SndStrmInit(NNSSndStrm *st);
extern BOOL NNS_SndStrmAllocChannel(NNSSndStrm *st, s32 n, u8 *ch);
extern void NNS_SndStrmFreeChannel(NNSSndStrm *st);

// the sound archive (unk_0210b448.c)
extern s32 NNS_SndArcReadFile(u32 idx, void *buf, u32 len, u32 offset);
extern u32 NNS_SndArcGetFileOffset(u32 idx);
extern void *NNS_SndArcGetStrmPlayerInfo(s32 idx);
extern void *NNS_SndArcGetStrmInfo(s32 idx);
extern void *NNS_SndHeapAlloc(void *heap, u32 size, void (*cb)(void *, u32, void *, u32), void *a, u32 b);

// externs: data (this file's bss, defined at its end; the C prototype headers don't declare uninitialized variables)
extern s32 data_021fbda8;
extern u8 *data_021fbdb0;
extern NNSSndStrmThread *data_021fbdac;
extern NNSFndList data_021fbdb4;
extern OSMutex data_021fbdc0;
extern NNSSndStrmLoadCommand data_021fbdd8[8];
extern u8 data_021fbf60[0x200];
extern NNSSndStrmThread data_021fc160;
extern NNSSndStrmPlayer data_021fc650[4];
extern s16 data_02139fb4[];

// externs: functions
extern u32 OS_DisableInterrupts(void);
extern void OS_RestoreInterrupts(u32);
extern void MI_CpuFill8(void *dst, u32 v, u32 n);
extern void MIi_CpuCopyFast(const void *src, void *dst, u32 n);
// (these headers declare them for C++ only so far: proto/autoload_2/unk_02100234.h, unk_02113088.h, unk_02119434.h)
extern void NNS_FndInitList(NNSFndList *list, u16 offset);
extern void NNS_FndAppendListObject(NNSFndList *list, void *obj);
extern void NNS_FndRemoveListObject(NNSFndList *list, void *obj);
extern void *NNS_FndGetNextListObject(NNSFndList *list, void *obj);
extern void OS_CreateThread(OSThread *thread, void (*func)(void *), void *arg, void *stack, u32 stackSize, u32 prio);
extern void OS_WakeupThreadDirect(OSThread *t);
extern BOOL FS_OpenFileFast(FSFile *file, FSFileID id);
extern BOOL FS_CloseFile(FSFile *file);
extern BOOL NNSi_SndFaderIsFinished(NNSSndFader *);
extern void NNSi_SndFaderInit(NNSSndFader *);
extern s32 NNSi_SndFaderGet(NNSSndFader *);
extern void NNSi_SndFaderSet(NNSSndFader *, s32, s32);
extern void NNSi_SndFaderUpdate(NNSSndFader *);

// in-unit prototypes
void RequestNextStrm(NNSSndStrmPlayer *);
void StrmDataCallback(s32, s32, NNSSndStrmBufAddr *, u32, s32, void *);
void StrmBufDisposeCallback(void *, u32, void *, u32);
void FreeCommandBuffer(NNSSndStrmLoadCommand *);
NNSSndStrmLoadCommand *AllocCommandBuffer(void);
NNSSndStrmLoadCommand *PopCommandBuffer(NNSFndList *);
void RemoveCommandByPlayer(NNSFndList *, NNSSndStrmPlayer *);
void CreateStrmThread(NNSSndStrmThread *, u32);
void FreeChannel(NNSSndStrmPlayer *);
BOOL AllocChannel(NNSSndStrmPlayer *c, s32 n, u8 *x);
void ShutdownPlayer__sndarc_stream(NNSSndStrmPlayer *);
void ForceStopStrm__sndarc_stream(NNSSndStrmPlayer *);
void StopStrm(NNSSndStrmPlayer *, s32);
BOOL PrepareStrmCore(NNSSndStrmPlayer **h, NNSSndArcStrmInfo *info, s32 playerNo, s32 prio, s32 strmNo, u32 offset,
                     NNSSndStrmCallback strmCallback, void *strmCallbackArg, NNSSndArcStrmCallback sndArcStrmCallback,
                     void *sndArcStrmCallbackArg);
void FreePlayer(NNSSndStrmPlayer *);
NNSSndStrmPlayer *AllocPlayer(NNSSndStrmPlayer **, s32, s32);
void NNS_SndStrmHandleRelease(NNSSndStrmPlayer **);
void NNS_SndArcStrmStartPrepared(NNSSndStrmPlayer **);
BOOL NNS_SndArcStrmPrepare(NNSSndStrmPlayer **, s32, s32);
BOOL SetupStrmPlayers(void *);
void OutputEffectMono(s16 *l, s16 *r, u32 len);

// HeadphoneProc (NitroSystem output_effect.c, capture effect 2): each channel gets the other one mixed in 24 samples
// late at a quarter of its level, clamped to s16 (Cut_S32toS16). The `register` local is the library's own.
static inline s16 Cut_S32toS16(s32 val)
{
    if (val < (s16)-0x8000)
        val = (s16)-0x8000;
    else if (val > (s16)0x7fff)
        val = (s16)0x7fff;
    return (s16)val;
}

void OutputEffectHeadphone(void *bufferL_p, void *bufferR_p, u32 len, EffCtl *info)
{
    s16 *lp = (s16 *)bufferL_p;
    s16 *rp = (s16 *)bufferR_p;
    const unsigned long samples = len >> 1;
    int i;
    s32 l;
    s32 r;
    int offset;
    unsigned long rest_samples;

    offset = 0;
    while (samples > 24 + offset) {
        for (i = 0; i < 24; i++) {
            const register int x = i + offset;

            l = lp[x] + info->u.e[i].a;
            r = rp[x] + info->u.e[i].b;

            lp[x] = Cut_S32toS16(l);
            rp[x] = Cut_S32toS16(r);
            info->u.e[i].a = (r + 1) >> 2;
            info->u.e[i].b = (l + 1) >> 2;
        }
        offset += 24;
    }

    rest_samples = samples - offset;
    for (i = 0; i < rest_samples; i++) {
        const int x = i + offset;

        l = lp[x] + info->u.e[i].a;
        r = rp[x] + info->u.e[i].b;
        lp[x] = Cut_S32toS16(l);
        rp[x] = Cut_S32toS16(r);
    }

    for (i = 0; i < 24 - rest_samples; i++) {
        info->u.e[i].a = info->u.e[i + rest_samples].a;
        info->u.e[i].b = info->u.e[i + rest_samples].b;
    }

    for (i = 0; i < rest_samples; i++) {
        const long x = (long)(i + 24 - rest_samples);

        info->u.e[x].a = (rp[i + offset] + 1) >> 2;
        info->u.e[x].b = (lp[i + offset] + 1) >> 2;
    }
}

// NNSi_SndCaptureEffect mono-mix-like (capture effect 3: average of both channels)
void OutputEffectMono(s16 *l, s16 *r, u32 len)
{
    u32 n = len >> 1;
    u32 i;
    for (i = 0; i < n; i++) {
        l[i] = (l[i] + r[i] + 1) >> 1;
    }
    MIi_CpuCopyFast(l, r, len);
}

// NNS_SndArcStrmInit-like (thread priority, heap)
void NNS_SndArcStrmInit(u32 prio, void *heap)
{
    s32 i;
    NNSSndStrmPlayer *c;
    if (data_021fbda8 != 0) {
        SetupStrmPlayers(heap);
        return;
    }
    data_021fbda8 = 1;
    NNS_FndInitList(&data_021fbdb4, 0);
    for (i = 0; i < 8; i++) {
        NNS_FndAppendListObject(&data_021fbdb4, &data_021fbdd8[i]);
    }
    OS_InitMutex(&data_021fbdc0);
    data_021fbdb0 = data_021fbf60;
    c = data_021fc650;
    for (i = 0; i < 4; i++, c++) {
        c->activeFlag = 0;
        FS_InitFile(&c->file);
        NNS_SndStrmInit(PlayerStrm(c));
        c->playerNo = i;
        c->numChannels = 0;
        c->buffer = 0;
        c->bufSize = 0;
        c->allocChannelCount = 0;
    }
    SetupStrmPlayers(heap);
    CreateStrmThread(&data_021fc160, prio);
}

// NNSi_SndArcStrmSetupBuffers-like (read channel tables, allocate stream buffers from heap)
#ifdef N3DS_DIRECT
// The enhanced port: the ARM7's sound runs on a thread of its own ahead of the game's,
// whose answers to the stream's alarms come up to a frame late or later; with the DS's 2 KB a channel (blocks of
// about 5 ms, refilled within 16 ms) most blocks were dropped as silence, so the buffers are 16 times as large (blocks
// of about 86 ms at the title's 47605 Hz). The sound heap's block still marks the buffer's lifetime
// (StrmBufDisposeCallback).
static u8 sStrmBuf[4][6 << 15] __attribute__((aligned(32)));
static void *sStrmHeapMem[4];
#endif

BOOL SetupStrmPlayers(void *heap)
{
    s32 i;
    u32 size;
    NNSSndStrmPlayer *c;
    s32 j;
    u8 *info;
    void *mem;
    c = data_021fc650;
    for (i = 0; i < 4; i++, c++) {
        info = NNS_SndArcGetStrmPlayerInfo(i);
        if (info == 0) continue;
        c->numChannels = info[0];
        for (j = 0; j < info[0]; j++) {
            c->chNoList[j] = (info + j)[1];
        }
        if (heap != 0) {
            size = c->numChannels << 11;
            mem = NNS_SndHeapAlloc(heap, size, StrmBufDisposeCallback, c, 0);
            if (mem == 0) return 0;
            ForceStopStrm__sndarc_stream(c);
#ifdef N3DS_DIRECT
            sStrmHeapMem[i] = mem;
            mem = sStrmBuf[i];
            size = c->numChannels << 15;
#endif
            c->buffer = mem;
            c->bufSize = size;
        }
    }
    return 1;
}

// NNS_SndArcStrmPrepare-like (lookup stream info then start impl)
BOOL NNS_SndArcStrmPrepare(NNSSndStrmPlayer **h, s32 strmNo, s32 len)
{
    NNSSndArcStrmInfo *info = NNS_SndArcGetStrmInfo(strmNo);
    if (info == 0) return 0;
    return PrepareStrmCore(h, info, info->playerNo, info->playerPrio, strmNo, len, 0, 0, 0, 0);
}

// NNS_SndArcStrmStartPlay flag-like
void NNS_SndArcStrmStartPrepared(NNSSndStrmPlayer **h)
{
    NNSSndStrmPlayer *c = *h;
    if (c != 0) c->startFlag = 1;
}

// NNS_SndArcStrmStart-like (handle, strmNo, len)
BOOL NNS_SndArcStrmStart(NNSSndStrmPlayer **h, s32 a, s32 b)
{
    if (NNS_SndArcStrmPrepare(h, a, b) == 0) return 0;
    NNS_SndArcStrmStartPrepared(h);
    return 1;
}

// NNS_SndArcStrmStop-like (handle, fade frames)
void NNS_SndArcStrmStop(NNSSndStrmPlayer **h, s32 frames)
{
    NNSSndStrmPlayer *c = *h;
    if (c == 0) return;
    StopStrm(c, frames);
}

// NNS_SndArcStrmSetVolume-like (volume, fade frames)
void NNS_SndArcStrmMoveVolume(NNSSndStrmPlayer **h, s32 vol, s32 frames)
{
    NNSSndStrmPlayer *c = *h;
    if (c == 0) return;
    if (c->fadeOutFlag != 0) return;
    NNSi_SndFaderSet(&c->fader, vol << 8, frames);
}

// NNS_SndArcStrmInitHandle-like
void NNS_SndStrmHandleInit(NNSSndStrmPlayer **h)
{
    *h = 0;
}

// NNSi_SndArcStrmHandleDetach-like
void NNS_SndStrmHandleRelease(NNSSndStrmPlayer **h)
{
    NNSSndStrmPlayer *c = *h;
    if (c != 0) {
        c->handle = 0;
        *h = 0;
    }
}

// NNS_SndArcStrmGetLoopTime-like (ms)
u32 func_0210e648(NNSSndStrmPlayer **h)
{
    NNSSndStrmPlayer *c = *h;
    if (c == 0) return 0;
    return DS_UDIV64((u64)c->curSample * (u64)1000, (u64)c->info.sampleRate);
}

// NNS_SndArcStrmGetTotalTime-like (ms)
u32 func_0210e5fc(NNSSndStrmPlayer **h)
{
    NNSSndStrmPlayer *c = *h;
    if (c == 0) return 0;
    return DS_UDIV64((u64)c->info.loopEnd * (u64)1000, (u64)c->info.sampleRate);
}

// NNS_SndArcStrmMain-like (per-frame update of the 4 contexts: start, fade, volume)
void NNSi_SndArcStrmMain(void)
{
    s32 i;
    s32 v;
    s32 a;
    NNSSndStrmPlayer *c;
    c = data_021fc650;
    for (i = 0; i < 4; i++, c++) {
        if (c->activeFlag == 0) continue;
        if (c->finishCounter == 0) {
            ForceStopStrm__sndarc_stream(c);
            continue;
        }
        if (c->startFlag != 0 && c->prepareFlag != 0) {
            NNS_SndStrmStart(PlayerStrm(c));
            c->playFlag = 1;
            c->startFlag = 0;
        }
        if (c->playFlag == 0) continue;
        NNSi_SndFaderUpdate(&c->fader);
        a = data_02139fb4[c->initVolume];
        v = data_02139fb4[NNSi_SndFaderGet(&c->fader) >> 8];
        v = v + a;
        if (v != c->volume) {
            NNS_SndStrmSetVolume(PlayerStrm(c), v);
            c->volume = v;
        }
        if (c->fadeOutFlag != 0) {
            if (NNSi_SndFaderIsFinished(&c->fader) != 0) ForceStopStrm__sndarc_stream(c);
        }
    }
}

// NNSi_SndArcStrmAcquire-like (take context idx with priority)
NNSSndStrmPlayer *AllocPlayer(NNSSndStrmPlayer **h, s32 idx, s32 prio)
{
    NNSSndStrmPlayer *c;
    if (*h != 0) NNS_SndStrmHandleRelease(h);
    c = &data_021fc650[idx];
    if (c->buffer == 0) return 0;
    if (c->activeFlag) {
        if (prio < c->prio) return 0;
        ForceStopStrm__sndarc_stream(c);
    }
    c->prio = prio;
    c->activeFlag = 1;
    c->handle = h;
    *h = c;
    return c;
}

// NNSi_SndArcStrmReset-like (detach handle, clear flags)
void FreePlayer(NNSSndStrmPlayer *c)
{
    NNSSndStrmPlayer **h = c->handle;
    if (h != 0) {
        *h = 0;
        c->handle = 0;
    }
    c->activeFlag = 0;
    c->startFlag = 0;
    c->playFlag = 0;
}

// NNSi_SndArcStrmStartImpl-like: open the stream file, set up the context and NNSSndStrm
BOOL PrepareStrmCore(NNSSndStrmPlayer **h, NNSSndArcStrmInfo *info, s32 playerNo, s32 prio, s32 strmNo, u32 offset,
                     NNSSndStrmCallback strmCallback, void *strmCallbackArg, NNSSndArcStrmCallback sndArcStrmCallback,
                     void *sndArcStrmCallbackArg)
{
    NNSSndStrmPlayer *c;
    s32 fmt;
    s32 n;
    FSFileID fid;

    c = AllocPlayer(h, playerNo, prio);
    if (c == 0) return 0;
    if (NNS_SndArcReadFile(info->fileId, &c->info, sizeof(NNSSndStrmData), 0) != sizeof(NNSSndStrmData)) {
        FreePlayer(c);
        return 0;
    }
    func_0210b48c(&fid);
    if (FS_OpenFileFast(&c->file, fid) == 0) {
        FreePlayer(c);
        return 0;
    }
    c->fileOffset = NNS_SndArcGetFileOffset(info->fileId);
    c->curSample = (c->info.sampleRate * offset) / 1000;
    if (c->curSample != 0 && c->info.format == 2) {
        c->dirtyFlag = 1;
    } else {
        c->dirtyFlag = 0;
    }
    c->finishCounter = 4;
    c->finishFlag = 0;
    c->playFlag = 0;
    c->prepareFlag = 0;
    c->startFlag = 0;
    c->fadeOutFlag = 0;
    c->commandCount = 0;
    c->strmCallback = strmCallback;
    c->strmCallbackArg = strmCallbackArg;
    c->sndArcStrmCallback = sndArcStrmCallback;
    c->sndArcStrmCallbackArg = sndArcStrmCallbackArg;
    c->strmNo = strmNo;
    c->volume = 0;
    c->initVolume = info->volume;
    NNSi_SndFaderInit(&c->fader);
    NNSi_SndFaderSet(&c->fader, 0x7f00, 1);
    switch (c->info.format) {
    case 0:
        fmt = 0;
        break;
    case 1:
    case 2:
        fmt = 1;
        break;
    }
    n = c->info.numChannels;
    if (info->flags & 1) n = 2;
    if (n > c->numChannels) n = c->numChannels;
    c->monoFlag = (n == 1);
    if (AllocChannel(c, n, c->chNoList) == 0) {
        FS_CloseFile(&c->file);
        FreePlayer(c);
        return 0;
    }
    if (NNS_SndStrmSetup(PlayerStrm(c), fmt, NNSi_SndArm7Addr(c->buffer), DS_UDIV((c->bufSize * n), c->numChannels), c->info.timer, 4,
                         StrmDataCallback, c) == 0) {
        FreeChannel(c);
        FS_CloseFile(&c->file);
        FreePlayer(c);
        return 0;
    }
    if (n == 2) {
        NNS_SndStrmSetChannelPan(PlayerStrm(c), 0, 0);
        NNS_SndStrmSetChannelPan(PlayerStrm(c), 1, 0x7f);
    }
    return 1;
}

// NNS_SndArcStrmStop(handle, fadeFrames)-like core
void StopStrm(NNSSndStrmPlayer *c, s32 frames)
{
    if (c->playFlag == 0) {
        ForceStopStrm__sndarc_stream(c);
        return;
    }
    if (frames == 0) {
        ForceStopStrm__sndarc_stream(c);
        return;
    }
    NNSi_SndFaderSet(&c->fader, 0, frames);
    c->fadeOutFlag = 1;
    c->prio = 0;
}

// NNS_SndArcStrmStop-like (immediate)
void ForceStopStrm__sndarc_stream(NNSSndStrmPlayer *c)
{
    OS_LockMutex(&data_021fc160.mutex);
    if (data_021fbdac != 0) OS_LockMutex(&data_021fbdac->mutex);
    if (c->playFlag) NNS_SndStrmStop(PlayerStrm(c));
    if (c->activeFlag) FS_CancelFile(&c->file);
    ShutdownPlayer__sndarc_stream(c);
    OS_UnlockMutex(&data_021fc160.mutex);
    if (data_021fbdac != 0) OS_UnlockMutex(&data_021fbdac->mutex);
}

// NNSi_SndArcStrmClose-like
void ShutdownPlayer__sndarc_stream(NNSSndStrmPlayer *c)
{
    if (c->activeFlag == 0) return;
    FreeChannel(c);
    FS_CloseFile(&c->file);
    RemoveCommandByPlayer(&data_021fc160.commandList, c);
    {
        NNSSndStrmThread *p = data_021fbdac;
        if (p != 0) RemoveCommandByPlayer(&p->commandList, c);
    }
    FreePlayer(c);
}

// NNSi_SndArcStrmChannelAlloc-like
BOOL AllocChannel(NNSSndStrmPlayer *c, s32 n, u8 *x)
{
    if (c->allocChannelCount == 0) {
        if (NNS_SndStrmAllocChannel(PlayerStrm(c), n, x) == 0) return 0;
    }
    c->allocChannelCount = c->allocChannelCount + 1;
    return 1;
}

// NNSi_SndArcStrmChannelRelease-like
void FreeChannel(NNSSndStrmPlayer *c)
{
    if (c->allocChannelCount == 0) return;
    c->allocChannelCount = c->allocChannelCount - 1;
    if (c->allocChannelCount != 0) return;
    NNS_SndStrmFreeChannel(PlayerStrm(c));
}

// NNS_SndArcStrmThread create-like (OS_CreateThread + job list, mutex, queue)
void CreateStrmThread(NNSSndStrmThread *t, u32 prio)
{
    OS_CreateThread(&t->thread, StrmThreadProc, t, &t->queue, 0x400, prio); // (the stack ends at the queue)
    NNS_FndInitList(&t->commandList, 0);
    OS_InitMutex(&t->mutex);
    t->queue.tail = 0;
    t->queue.head = t->queue.tail;
    OS_WakeupThreadDirect(&t->thread);
}

// NNSi_SndArcStrmJobCancel-like (drop all jobs of a stream)
void RemoveCommandByPlayer(NNSFndList *list, NNSSndStrmPlayer *c)
{
    u32 irq;
    NNSSndStrmLoadCommand *job;
    NNSSndStrmLoadCommand *next;
    irq = OS_DisableInterrupts();
    for (job = NNS_FndGetNextListObject(list, 0); job != 0; job = next) {
        next = NNS_FndGetNextListObject(list, job);
        if (job->player == c) {
            NNS_FndRemoveListObject(list, job);
            FreeCommandBuffer(job);
        }
    }
    OS_RestoreInterrupts(irq);
}

// NNSi_SndArcStrmJobPop-like (first job of a thread list)
NNSSndStrmLoadCommand *PopCommandBuffer(NNSFndList *list)
{
    u32 irq;
    NNSSndStrmLoadCommand *job;
    irq = OS_DisableInterrupts();
    job = NNS_FndGetNextListObject(list, 0);
    if (job != 0) {
        NNS_FndRemoveListObject(list, job);
        job->player->commandCount = job->player->commandCount - 1;
    }
    OS_RestoreInterrupts(irq);
    return job;
}

// NNSi_SndArcStrmJobAlloc-like (job from the free list)
NNSSndStrmLoadCommand *AllocCommandBuffer(void)
{
    u32 irq;
    NNSSndStrmLoadCommand *job;
    irq = OS_DisableInterrupts();
    job = NNS_FndGetNextListObject(&data_021fbdb4, 0);
    if (job != 0) NNS_FndRemoveListObject(&data_021fbdb4, job);
    OS_RestoreInterrupts(irq);
    return job;
}

// NNSi_SndArcStrmJobFree-like (job back to the free list)
void FreeCommandBuffer(NNSSndStrmLoadCommand *job)
{
    u32 irq = OS_DisableInterrupts();
    NNS_FndAppendListObject(&data_021fbdb4, job);
    OS_RestoreInterrupts(irq);
}

// NNSi_SndArcStrmHeapDestroyCallback-like (snd heap block destroy callback)
void StrmBufDisposeCallback(void *mem, u32 size, void *arg, u32 unused)
{
    NNSSndStrmPlayer *c = arg;
#ifdef N3DS_DIRECT
    if (mem != sStrmHeapMem[c - data_021fc650]) return;
#else
    if (mem != c->buffer) return;
#endif
    OS_LockMutex(&data_021fc160.mutex);
    if (data_021fbdac != 0) OS_LockMutex(&data_021fbdac->mutex);
    ForceStopStrm__sndarc_stream(c);
    c->buffer = 0;
    c->bufSize = 0;
    c->numChannels = 0;
    if (c->allocChannelCount > 0) {
        NNS_SndStrmFreeChannel(PlayerStrm(c));
        c->allocChannelCount = 0;
    }
    OS_UnlockMutex(&data_021fc160.mutex);
    if (data_021fbdac != 0) OS_UnlockMutex(&data_021fbdac->mutex);
}

// NNSi_SndArcStrmCallback-like: NNSSndStrm block callback (queues a load job for the stream thread)
void StrmDataCallback(s32 ch, s32 n, NNSSndStrmBufAddr *bufs, u32 size, s32 unused, void *arg)
{
    NNSSndStrmPlayer *c = arg;
    NNSSndStrmLoadCommand *job;
    NNSSndStrmThread *t;
    s32 i;

    if (c->commandCount >= 2) {
        for (job = NNS_FndGetNextListObject(&data_021fc160.commandList, 0); job != 0;
             job = NNS_FndGetNextListObject(&data_021fc160.commandList, job)) {
            if (job->player == c) break;
        }
        for (i = 0; i < job->numChannels; i++) {
            MI_CpuFill8(NNSi_SndStrmBufPtr(job->buffer[i]), 0, job->bufLen);
        }
        NNS_FndRemoveListObject(&data_021fc160.commandList, job);
        c->commandCount = c->commandCount - 1;
        FreeCommandBuffer(job);
    }
    job = AllocCommandBuffer();
    job->player = c;
    job->status = ch;
    job->numChannels = n;
    for (i = 0; i < n; i++) {
        job->buffer[i] = bufs[i];
    }
    job->bufLen = size;
    t = &data_021fc160;
    if (ch == 0) {
        if (data_021fbdac != 0) t = data_021fbdac;
    }
    c->commandCount = c->commandCount + 1;
    NNS_FndAppendListObject(&t->commandList, job);
    OS_WakeupThread(&t->queue);
}

// NNSi_SndArcStrmLoadNext-like: stream thread helper, asks the user callback for the next stream and re-reads its header
void RequestNextStrm(NNSSndStrmPlayer *c)
{
    u8 oldType;
    u16 oldRate;
    NNSSndArcStrmInfo *info;
    NNSSndArcStrmCallbackParam in;
    NNSSndArcStrmCallbackInfo out;
    BOOL ok;

    in.playerNo = c->playerNo;
    in.strmNo = c->strmNo;
    out.strmNo = c->strmNo;
    out.offset = 0;
    ok = c->sndArcStrmCallback(0, &in, &out, c->sndArcStrmCallbackArg);
    if (ok == 0) return;
    info = NNS_SndArcGetStrmInfo(out.strmNo);
    if (info == 0) return;
    oldType = c->info.format;
    oldRate = c->info.sampleRate;
    c->fileOffset = NNS_SndArcGetFileOffset(info->fileId);
    NNS_SndArcReadFile(info->fileId, &c->info, sizeof(NNSSndStrmData), 0);
    if (oldRate != c->info.sampleRate) return;
    if (oldType == 0 && c->info.format != 0) return;
    if (oldType != 0 && c->info.format == 0) return;
    c->strmNo = out.strmNo;
    c->curSample = (c->info.sampleRate * out.offset) / 1000;
    if (c->curSample != 0 && c->info.format == 2) {
        c->dirtyFlag = 1;
    } else {
        c->dirtyFlag = 0;
    }
    c->finishFlag = 0;
}

// ---- file-scope objects (autoload_3 .bss 0x021fbda8-0x021fcbd0). The DS's labels data_021fc62c / data_021fc644 are the
// mutex and the command list of data_021fc160. This definition order gives the original order after mwcc's size sort.
s32 data_021fbda8;
u8 *data_021fbdb0; // the ADPCM decode buffer (MakeWaveData, unk_0210c084.c)
NNSSndStrmThread *data_021fbdac;
NNSFndList data_021fbdb4;
OSMutex data_021fbdc0; // the decode buffer's mutex
NNSSndStrmLoadCommand data_021fbdd8[8];
u8 data_021fbf60[0x200] __attribute__((aligned(32)));
NNSSndStrmThread data_021fc160;
NNSSndStrmPlayer data_021fc650[4];

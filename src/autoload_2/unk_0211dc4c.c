// mwcc-flags: -nothumb -O4,p
#include "nitro/os_rtc.h"
// NitroSDK RTC (rtc.c) + CARD common/rom (card_common.c, card_rom.c), autoload_2 0x0211dc4c-0x0211e3fc. ARM code, mwcc 1.2/base -O4,p.
// Functions are in reverse address order (mwcc emits in reverse source order).
#include "uptr.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
#include "types64.h"
#include "nitro/io_access.h"
typedef int BOOL;

#include "sys/OSThread.h"  // the real types: a thread and its queues are larger on 64-bit hosts
#include "proto/autoload_2/unk_02113088.h"
#include "proto/autoload_2/unk_02117984.h"
#include "proto/autoload_2/unk_0211d680.h"
#include "proto/autoload_2/unk_0211dc4c.h"
#include "proto/autoload_2/unk_0211e558.h"
#include "proto/autoload_2/unk_0211ea4c.h"
#include "proto/main/unk_02000b44.h"
#include "proto/main/unk_0206d49c.h"
typedef OSThreadQueue OSQ;
typedef OSThread OST;

typedef struct {  // RTCWork (unk_0211cbe8.c): f4..f10 and f1c are pointers there
    u32 lock;
    uptr f4, f8, fc, f10;
    u32 command;
    u32 f18;
    uptr f1c;
    u32 result;
} RTCWork;

#include "sys/CardCmd.h"

typedef struct CARDCommon {
    CARDCmd *cmd;
    u32 f4;
    volatile u32 lockOwner;
    volatile u32 lockCount;
    OSQ queue;
    u32 lockType;
    uptr src;  // (card addresses or ARM9 memory: pointer-sized)
    uptr dst;
    u32 len;
    u32 op;
    u32 f2c, f30, f34;
    void (*callback)(void *);
    void *cbArg;
    void (*task)(struct CARDCommon *);
    OST thread;
    OST *curThread;
    u32 priority;
    OSQ tq;
    volatile u32 flag;
    u32 f118, f11c;
    u8 buf[0x100];
} CARDCommon;

typedef struct {
    void (*fn)(void *);
    u32 f4;
    u32 f8;
    u32 fc;
    u32 f10, f14, f18, f1c;
    u32 buf[128];           // 0x20: the 512-byte page cache
} RomDev;

extern RTCWork data_021feb90;
extern u16 data_021feb8c;
extern CARDCommon data_021fec00;
extern u32 data_021febb4;
extern u8 data_021febc0[];
extern u32 data_021ff220;
extern RomDev data_021ff240;
extern OSThreadInfo data_021fcc2c;  // OSi_ThreadInfo

u32 OS_DisableInterrupts(void);
void OS_RestoreInterrupts(u32);
void OS_UnlockCard(u32);
void OS_LockCard(u32);
void OS_SetThreadPriority(void *, u32);
void OS_WakeupThreadDirect(void *);
void OS_CreateThread(void *, void (*)(void *), void *, void *, u32, u32);
void DC_InvalidateRange(void *, u32);
void DC_FlushRange(void *, u32);
void DC_WaitWriteBufferEmpty(void);
void MI_StopDma(u32);
void MIi_CpuClearFast(u32, void *, u32);
void MI_CpuFill8(void *, u32, u32);
void MI_CpuCopy8(const void *, void *, u32);
BOOL PXI_IsCallbackReady(u32, u32);
s32 PXI_SendWordByFifo(u32, u32, u32);
void PXI_SetFifoRecvCallback(u32, void *);
void RtcWaitBusy(void);
void RtcGetResultCallback(void);
void RtcCommonCallback(void);
void CARDi_OnFifoRecv(void);
BOOL CARDi_Request(void *, u32, u32);
void CARDi_SetRomOp(u32, u32);
BOOL CARDi_ReadFromCache(void *);
BOOL CARDi_TryReadCardDma(void *);

u32 RTC_GetDateTimeAsync(void *, void *, void (*)(void), void *);
u32 RTC_GetTimeAsync(void *, void (*)(void), void *);
u32 RTC_GetDateAsync(void *, void (*)(void), void *);
BOOL RtcSendPxiCommand(u32);
BOOL RTCi_ReadRawDateAsync(void);
BOOL RTCi_ReadRawTimeAsync(void);
BOOL RTCi_ReadRawDateTimeAsync(void);
s32 RTCi_ConvertTimeToSecond(RTCTime *);
s32 RTC_ConvertDateToDay(RTCDate *);
void CARD_Enable(u32);
u32 CARD_IsEnabled(void);
void CARDi_UnlockResource(u32, u32);
void CARDi_LockResource(u32, u32);
void CARDi_SetTask(void (*)(CARDCommon *));
void CARDi_IdentifyBackupCore(s32);
void CARDi_RequestStreamCommandCore(CARDCommon *);
BOOL CARD_WaitRomAsync(void);
void CARDi_ReadRomSyncCore(CARDCommon *);
BOOL CARDi_TryWaitAsync(void);
BOOL CARDi_WaitAsync(void);

#define REG_MCCNT1 (*(volatile u32 *)0x040001a4)
#define REG_MCD1 (*(volatile u32 *)0x04100010)

// CARDi_ReadRomSyncCore~ (reads 512-byte pages through REG_MCCNT1 / REG_MCD1)
void CARDi_ReadCard(void *p) {
    RomDev *d = (RomDev *)p;
    CARDCommon *const c = &data_021fec00;
    u32 *dst;
    u32 *buf = d->buf;
    u32 mask = -512;
    do {
        u32 src = c->src;
        u32 page = src & mask;
        u32 n;
        u32 st;
        if (page != src || (dst = (u32 *)c->dst, ((uptr)dst & 3)) || c->len < 512) {
            dst = buf;
            d->f8 = page;
        }
        // the host build reads the page from the ROM image at once (each register access is a trap there)
        {
            extern void host_card_read(u32 addr, void *dst, u32 n);
            host_card_read(page, dst, 512);
            (void)n; (void)st;
        }
        if (dst == (u32 *)c->dst) {
            data_021fec00.src += 512;
            data_021fec00.dst += 512;
            data_021fec00.len -= 512;
            if (data_021fec00.len == 0) return;
        } else {
            if (!CARDi_ReadFromCache(d)) return;
        }
    } while (1);
}

// CARDi_ReadRomEnd~ (task body)
void CARDi_ReadRomSyncCore(CARDCommon *unused) {
    RomDev *d = &data_021ff240;
    if (CARDi_ReadFromCache(d)) d->fn(d);
    {
        CARDCommon *const c = &data_021fec00;
        void (*cb)(void *);
        void *arg;
        u32 irq;
        c->cmd->result = 0;
        cb = c->callback;
        arg = c->cbArg;
        irq = OS_DisableInterrupts();
        c->flag &= ~0x4c;
        OS_WakeupThread(&c->tq);
        if (c->flag & 0x10) OS_WakeupThreadDirect(&c->thread);
        OS_RestoreInterrupts(irq);
        if (cb) cb(arg);
    }
}

// CARDi_ReadRom
void CARDi_ReadRom(u32 cmd, u32 off, void *dst, u32 len, void (*cb)(void *), void *arg, BOOL async) {
    CARDCommon *const c = &data_021fec00;
    RomDev *d = &data_021ff240;
    u32 irq;
    CARD_CheckEnabled();
    irq = OS_DisableInterrupts();
    while (c->flag & 4) OS_SleepThread(&c->tq);
    c->flag |= 4;
    c->callback = cb;
    c->cbArg = arg;
    OS_RestoreInterrupts(irq);
    c->op = cmd;
    c->src = off + data_021ff220;
    c->dst = (uptr)dst;
    c->len = len;
    if (cmd <= 3) MI_StopDma(cmd);
#ifdef N3DS_DIRECT
    {   // the enhanced port: the whole read from the ROM image at once (card.c), not page by
        // page through the card's registers and DMA; then finished as the SDK finishes a read
        extern void host_card_read(u32 addr, void *dst, u32 n);
        host_card_read(c->src, (void *)c->dst, c->len);
        c->src += c->len, c->dst += c->len, c->len = 0;
        if (async) {
            CARDi_SetTask(CARDi_ReadRomSyncCore);
            return;
        }
        c->curThread = data_021fcc2c.current;
        CARDi_ReadRomSyncCore(c);
        return;
    }
#endif
    if (CARDi_TryReadCardDma(d)) {
        if (async) return;
        CARD_WaitRomAsync();
        return;
    }
    if (async) {
        CARDi_SetTask(CARDi_ReadRomSyncCore);
        return;
    }
    c->curThread = data_021fcc2c.current;
    CARDi_ReadRomSyncCore(c);
}

// CARD_Init
void CARD_Init(void) {
    CARDCommon *const c = &data_021fec00;
    if (c->flag != 0) return;
    c->flag = 1;
    c->src = c->dst = c->len = 0;
    c->op = -1;
    c->callback = 0;
    c->cbArg = 0;
    data_021ff220 = 0;
    CARDi_InitCommon();
    data_021ff240.fn = (void (*)(void *))CARDi_GetRomAccessor();
    CARD_InitPulledOutCallback();
}

// CARD_WaitRomAsync~ (tail call to CARDi_WaitAsync)
BOOL CARD_WaitRomAsync(void) {
    return CARDi_WaitAsync();
}

// returns the ROM read routine CARDi_ReadCard
void *CARDi_GetRomAccessor(void) {
    return CARDi_ReadCard;
}

// CARDi_ExecuteStreamTask~ (task body, 256-byte transfer loop)
void CARDi_RequestStreamCommandCore(CARDCommon *c) {
    u32 g = c->f2c;
    u32 mode = c->f34;
    u32 h = c->f30;
    u32 pg = 256;
    u32 one = 1;
    void (*cb)(void *);
    void *arg;
    u32 irq;
    OSi_ReferSymbol(0);  // (the DS address 0x02000bbc of an SDK version string; an empty function)
    do {
        u32 len = c->len;
        if (len > pg) len = pg;
        c->cmd->len = len;
        if (c->flag & 0x40) {
            c->flag &= ~0x40;
            c->cmd->result = 7;
            break;
        }
        if (mode == 0) {
            DC_InvalidateRange(c->buf, len);
            c->cmd->srcBuf = c->src;
            c->cmd->dstBuf = (uptr)c->buf;
        } else {
            MI_CpuCopy8((void *)c->src, c->buf, len);
            DC_FlushRange(c->buf, len);
            DC_WaitWriteBufferEmpty();
            c->cmd->srcBuf = (uptr)c->buf;
            c->cmd->dstBuf = c->dst;
        }
        if (!CARDi_Request(c, g, h)) break;
        if (mode == 2) {
            if (!CARDi_Request(c, 9, one)) break;
        } else if (mode == 0) {
            MI_CpuCopy8(c->buf, (void *)c->dst, len);
        }
        c->src += len;
        c->dst += len;
        c->len -= len;
    } while (c->len != 0);
    cb = c->callback;
    arg = c->cbArg;
    irq = OS_DisableInterrupts();
    c->flag &= ~0x4c;
    OS_WakeupThread(&c->tq);
    if (c->flag & 0x10) OS_WakeupThreadDirect(&c->thread);
    OS_RestoreInterrupts(irq);
    if (cb) cb(arg);
}

#ifdef N3DS_DIRECT
// The enhanced port: a stream command on the save memory done at once in the host
// (pxi7.c), not 256 bytes at a time through the ARM7 with a reply to wait for each, then finished as
// CARDi_RequestStreamCommandCore finishes. An async one (on the card thread) finishes a millisecond later: the game
// takes an async request that is done when it returns for a failed one (Backup_ReadAsync).
void OS_Sleep(u32 msec);
static void CARDi_StreamHost(CARDCommon *c) {
    extern u32 pxi7_backup(uptr src, uptr dst, u32 len, u32 op, u32 total);
    void (*cb)(void *) = c->callback;
    void *arg = c->cbArg;
    u32 irq;
    c->cmd->result = pxi7_backup(c->src, c->dst, c->len, c->f2c, c->cmd->f18);
    c->src += c->len, c->dst += c->len, c->len = 0;
    if (c->curThread == &c->thread) OS_Sleep(1);  // (an async request: on the card thread, CARDi_SetTask)
    irq = OS_DisableInterrupts();
    c->flag &= ~0x4c;
    OS_WakeupThread(&c->tq);
    if (c->flag & 0x10) OS_WakeupThreadDirect(&c->thread);
    OS_RestoreInterrupts(irq);
    if (cb) cb(arg);
}
#endif

// CARDi_RequestStreamCommand~ (src, dst, len, callback, arg, is_async, req_type, retry, mode)
BOOL CARDi_RequestStreamCommand(uptr a, uptr b, u32 len, void (*cb)(void *), void *arg, BOOL async, u32 g, u32 h, u32 mode) {
    CARDCommon *const c = &data_021fec00;
    u32 irq;
    OSi_ReferSymbol(0);  // (the DS address 0x02000bbc of an SDK version string; an empty function)
    irq = OS_DisableInterrupts();
    while (c->flag & 4) OS_SleepThread(&c->tq);
    c->flag |= 4;
    c->callback = cb;
    c->cbArg = arg;
    OS_RestoreInterrupts(irq);
    c->src = a;
    c->dst = b;
    c->len = len;
    c->f2c = g;
    c->f30 = h;
    c->f34 = mode;
#ifdef N3DS_DIRECT
    if (async) {  // (CARDi_StreamHost on the card thread)
        CARDi_SetTask(CARDi_StreamHost);
        return 1;
    }
    data_021fec00.curThread = data_021fcc2c.current;
    CARDi_StreamHost(c);
    return c->cmd->result == 0;
#endif
    if (async) {
        CARDi_SetTask(CARDi_RequestStreamCommandCore);
        return 1;
    }
    data_021fec00.curThread = data_021fcc2c.current;
    CARDi_RequestStreamCommandCore(c);
    return c->cmd->result == 0;
}

// returns a field of the current command block (+0x18)
u32 CARD_GetBackupTotalSize(void) {
    return data_021fec00.cmd->f18;
}

// CARD_IdentifyBackup~ (type)
BOOL CARD_IdentifyBackup(u32 op) {
    CARDCommon *c = &data_021fec00;
    void (*cb)(void *);
    void *arg;
    u32 irq;
    OSi_ReferSymbol(0);  // (the DS address 0x02000bbc of an SDK version string; an empty function)
    if (op == 0) Fatal_Trap();
    CARD_CheckEnabled();
    irq = OS_DisableInterrupts();
    while (c->flag & 4) OS_SleepThread(&c->tq);
    c->flag |= 4;
    c->callback = 0;
    c->cbArg = 0;
    OS_RestoreInterrupts(irq);
    CARDi_IdentifyBackupCore(op);
    data_021fec00.curThread = data_021fcc2c.current;
    CARDi_Request(c, 2, 1);
    c->cmd->srcBuf = 0;
    c->cmd->dstBuf = (uptr)c->buf;
    c->cmd->len = 1;
    CARDi_Request(c, 6, 1);
    cb = c->callback;
    arg = c->cbArg;
    irq = OS_DisableInterrupts();
    c->flag &= ~0x4c;
    OS_WakeupThread(&c->tq);
    if (c->flag & 0x10) OS_WakeupThreadDirect(&c->thread);
    OS_RestoreInterrupts(irq);
    if (cb) cb(arg);
    return c->cmd->result == 0;
}

// CARD_TryWaitRomAsync~ (tail call to CARDi_TryWaitAsync)
BOOL CARD_TryWaitBackupAsync(void) {
    return CARDi_TryWaitAsync();
}

// sets the cancel flag 0x40 under IRQ disable (CARD_CancelAll~)
void CARD_CancelBackupAsync(void) {
    u32 irq = OS_DisableInterrupts();
    data_021fec00.flag |= 0x40;
    OS_RestoreInterrupts(irq);
}

// ---- file-scope objects (autoload_3 .bss 0x021ff220-0x021ff460; this definition order gives the original order after mwcc's size
// sort)
u32 data_021ff220;
RomDev data_021ff240 __attribute__((aligned(32)));

#include "sys/DtorEntry.h"
#include "host_mem.h"
#include "nitro/fs.h"
#include "proto/autoload_2/unk_02113088.h"
#include "proto/autoload_2/unk_02119020.h"
#include "proto/autoload_2/unk_02119434.h"
#include "proto/autoload_2/unk_02119f84.h"
#include "proto/autoload_2/unk_0211a258.h"
#include "proto/autoload_2/unk_0211a6c0.h"
#include "proto/autoload_2/unk_0211dc4c.h"
#include "proto/main/unk_0206d49c.h"
// mwcc-flags: -nothumb -O4,p
// NitroSDK FS (fs_file / fs_archive / fs_overlay) + MATH_CalcHMACMD5, autoload_2 0x02119434-0x02119de4. ARM code.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef int BOOL;

typedef struct {
    u32 w[23];
} MD5Context;

typedef struct {
    u8 b[20];
} Digest20;

typedef struct DtorEntry DtorEntry;

extern u32 OS_DisableInterrupts(void);
extern void OS_RestoreInterrupts(u32);
extern void MI_CpuFill8(void *dst, u32 v, u32 n);
extern void MI_CpuCopy8(const void *src, void *dst, u32 n);
extern int FSi_TranslateCommand(FSFile *file, u32 cmd);
extern void FSi_ReleaseCommand(FSFile *file, u32 result);
extern void FS_NotifyArchiveAsyncEnd(FSArc *arc, u32 result);
extern void FS_SetArchiveProc(FSArc *arc, int (*proc)(FSFile *, u32), u32 mask);
extern void FS_LoadArchive(FSArc *arc, uptr base, u32 fat, u32 fat_size, u32 fnt, u32 fnt_size,
                          int (*read)(FSArc *, void *, u32, u32), int (*write)(FSArc *, void *, u32, u32));
extern void FS_RegisterArchiveName(FSArc *arc, const char *name, int len);
extern BOOL FSi_SendCommand(FSFile *file, u32 cmd);
extern u32 OS_GetLockID(void);
extern void CARD_UnlockRom(u32);
extern void CARD_LockRom(u32);
extern void CARDi_ReadRom(u32 dma, u32 src, void *dst, u32 len, void (*cb)(FSArc *), FSArc *arg, BOOL async);
extern void DC_FlushRange(u32 addr, u32 len);
extern void DC_InvalidateRange(u32 addr, u32 len);
extern void IC_InvalidateRange(u32 addr, u32 len);
extern void MIi_UncompressBackward(u32 addr);
extern void DGT_Hash2CalcHmac(void *digest, const void *data, u32 len, const void *key, u32 keylen);
extern void DGT_Hash1Reset(MD5Context *ctx);
extern void DGT_Hash1SetSource(MD5Context *ctx, const void *data, u32 len);
extern void DGT_Hash1GetDigest_R(void *digest, MD5Context *ctx);

extern FSDirPos data_021fea6c;
extern u32 data_021fea78;
extern u32 data_021fea7c;
extern u32 data_021fea80;
extern FSOvtCache data_021fea84;
extern FSOvtCache data_021fea8c;
extern FSArc data_021fea94;
extern char data_0213bff8[];
extern u8 *data_0213bffc;
extern u32 data_0213c000;
extern DtorEntry *data_0220066c;
extern Digest20 data_020e74ec[];
extern Digest20 data_020e74ec_end[];

#define REG_BOOTTYPE (*HW_SYSWORK(volatile u16, wmBootBuf))

static inline BOOL IsBusy(volatile FSFile *f) {
    BOOL r;
    r = (f->stat & 1) != 0;
    return r;
}
static inline BOOL IsCanceled(volatile FSFile *f) {
    BOOL r;
    r = (f->stat & 2) != 0;
    return r;
}
static inline BOOL IsBlocking(volatile FSFile *f) {
    BOOL r;
    r = (f->stat & 4) != 0;
    return r;
}
static inline BOOL IsSuspended(volatile FSArc *a) {
    BOOL r;
    r = (a->flag & 8) != 0;
    return r;
}
static inline BOOL IsStarted(volatile FSArc *a) {
    BOOL r;
    r = (a->flag & 0x10) != 0;
    return r;
}
static inline const FSROMTable *GetFNT(void) {
    return HW_SYSWORK_AT(const FSROMTable, romHeader, 0x40);
}
static inline const FSROMTable *GetFAT(void) {
    return HW_SYSWORK_AT(const FSROMTable, romHeader, 0x48);
}
static inline BOOL IsLoaded(volatile FSArc *a) {
    BOOL r;
    r = (a->flag & 2) != 0;
    return r;
}
static inline BOOL IsIdle(volatile FSFile *f) {
    BOOL r;
    r = (f->stat & 0x44) == 0;
    return r;
}

int FSi_RomArchiveProc(FSFile *file, u32 cmd);
u32 FSi_ReadDummyCallback(void);
u32 FSi_EmptyArchiveProc(void);
u32 FSi_WriteDummyCallback(void);
int FSi_ReadRomCallback(FSArc *arc, void *dst, u32 src, u32 len);
void FSi_InitRom(u32 dma);
BOOL FS_OpenFileFast(FSFile *file, FSFileID id);
BOOL FS_OpenFileDirect(FSFile *file, FSArc *arc, u32 start, u32 end, int id);
BOOL FS_ConvertPathToFileID(FSFileID *id, const char *path);
BOOL FSi_FindPath(FSFile *file, const char *path, FSFileID *id, FSDirPos *pos);
s32 FSi_ReadFileCore(FSFile *file, void *dst, s32 len, BOOL async);
s32 FS_ReadFile(FSFile *file, void *dst, s32 len);
BOOL FS_CloseFile(FSFile *file);
BOOL FS_WaitAsync(FSFile *file);
BOOL FSi_CompareDigest(const u8 *expected, const void *data, u32 len);
BOOL FS_LoadOverlayImage(FSOverlayInfo *p);
BOOL FS_LoadOverlayInfo(FSOverlayInfo *p, int target, u32 id);
BOOL FS_UnloadOverlayImage(FSOverlayInfo *p);
u32 FSi_GetOverlayBinarySize(const FSOverlayInfoHeader *h);

// FS_Init
void FS_Init(u32 dma) {
    if (data_021fea78) return;
    data_021fea78 = 1;
    FSi_InitRom(dma);
}

// FS_InitFile
void FS_InitFile(FSFile *file) {
    file->next = file->prev = 0;
    file->queue.head = file->queue.tail = 0;
    file->arc = 0;
    file->command = 14;
    file->stat = 0;
}

// FSi_SetPathCommand
BOOL FSi_FindPath(FSFile *file, const char *path, FSFileID *id, FSDirPos *dirpos) {
    FSDirPos pos;
    int i;
    u8 c = path[0];
    if (c == '/' || c == '\\') {
        pos.u.d.own_id = 0;
        pos.arc = data_021fea6c.arc;
        pos.pos = 0;
        pos.u.d.index = 0;
        path++;
    } else {
        pos = data_021fea6c;
        for (i = 0; i <= 3; i++) {
            u8 ch = path[i];
            if (ch == 0 || ch == '/' || ch == '\\') break;
            if (ch == ':') {
                FSArc *arc = FS_FindArchive(path, i);
                if (!arc) return 0;
                if (!IsLoaded(arc)) return 0;
                pos.arc = arc;
                pos.pos = 0;
                pos.u.d.index = 0;
                pos.u.d.own_id = 0;
                path += i + 1;
                ch = *path;
                if (ch == '/' || ch == '\\') path++;
                break;
            }
        }
    }
    file->arc = pos.arc;
    file->arg.findpath.path = path;
    file->arg.findpath.pos = pos;
    if (dirpos) {
        file->arg.findpath.find_directory = 1;
        file->arg.findpath.result = dirpos;
    } else {
        file->arg.findpath.find_directory = 0;
        file->arg.findpath.result = id;
    }
    return FSi_SendCommand(file, 4);
}

// FSi_ReadFileCore
s32 FSi_ReadFileCore(FSFile *file, void *dst, s32 len, BOOL async) {
    s32 pos = file->prop.file.pos;
    s32 rest = file->prop.file.end - pos;
    s32 len_org = len;
    file->arg.readfile.dst = dst;
    if (len > rest) len = rest;
    if (len < 0) len = 0;
    file->arg.readfile.len_org = len_org;
    file->arg.readfile.len = len;
    if (!async) file->stat |= 4;
    FSi_SendCommand(file, 0);
    if (!async) {
        len = FS_WaitAsync(file) ? file->prop.file.pos - pos : -1;
    }
    return len;
}

// FS_ConvertPathToFileID
BOOL FS_ConvertPathToFileID(FSFileID *id, const char *path) {
    FSFile tmp;
#ifdef N3DS_DIRECT  // (the enhanced port: a file of the chosen language from the EUR ROM, modern/ds/n3ds_lang.c)
    extern BOOL N3ds_LangFileID(FSFileID *id, const char *path);
    if (N3ds_LangFileID(id, path)) return 1;
#endif
    FS_InitFile(&tmp);
    return FSi_FindPath(&tmp, path, id, 0) ? 1 : 0;
}

// FS_OpenFileDirect
BOOL FS_OpenFileDirect(FSFile *file, FSArc *arc, u32 start, u32 end, int id) {
    file->arc = arc;
    file->arg.openfiledirect.index = id;
    file->arg.openfiledirect.top = start;
    file->arg.openfiledirect.bottom = end;
    if (!FSi_SendCommand(file, 7)) return 0;
    file->stat |= 0x10;
    file->stat &= ~0x20;
    return 1;
}

// FS_OpenFileFast
BOOL FS_OpenFileFast(FSFile *file, FSFileID id) {
    if (id.arc == 0) return 0;
    file->arc = id.arc;
    file->arg.openfilefast.id = id;
    if (!FSi_SendCommand(file, 6)) return 0;
    file->stat |= 0x10;
    file->stat &= ~0x20;
    return 1;
}

// FS_OpenFile
BOOL FS_OpenFile(FSFile *file, const char *path) {
    FSFileID id;
    if (FS_ConvertPathToFileID(&id, path) && FS_OpenFileFast(file, id)) return 1;
    return 0;
}

// FS_CloseFile
BOOL FS_CloseFile(FSFile *file) {
    if (!FSi_SendCommand(file, 8)) return 0;
    file->arc = 0;
    file->command = 14;
    file->stat &= ~0x30;
    return 1;
}

// FS_WaitAsync
BOOL FS_WaitAsync(FSFile *file) {
    BOOL done = 0;
    u32 irq = OS_DisableInterrupts();
    if (IsBusy(file)) {
        done = IsIdle(file);
        if (done) {
            file->stat |= 4;
            do {
                OS_SleepThread(&file->queue);
            } while (!(file->stat & 0x40));
        } else {
            do {
                OS_SleepThread(&file->queue);
            } while (IsBusy(file));
        }
    }
    OS_RestoreInterrupts(irq);
    if (done) return FSi_ExecuteSyncCommand(file);
    return file->error == 0;
}

// FS_CancelFile
void FS_CancelFile(FSFile *file) {
    u32 irq = OS_DisableInterrupts();
    if (IsBusy(file)) {
        file->stat |= 2;
        file->arc->flag |= 0x20;
    }
    OS_RestoreInterrupts(irq);
}

// FS_ReadFile
s32 FS_ReadFile(FSFile *file, void *dst, s32 len) {
    return FSi_ReadFileCore(file, dst, len, 0);
}

// FS_SeekFile
BOOL FS_SeekFile(FSFile *file, s32 pos, u32 origin) {
    switch (origin) {
    case 0:
        pos += file->prop.file.start;
        break;
    case 1:
        pos += file->prop.file.pos;
        break;
    case 2:
        pos += file->prop.file.end;
        break;
    default:
        return 0;
    }
    if (pos < file->prop.file.start) pos = file->prop.file.start;
    if (pos > file->prop.file.end) pos = file->prop.file.end;
    file->prop.file.pos = pos;
    return 1;
}

// FS_SetCurrentDirectory
BOOL FS_ChangeDir(const char *path) {
    FSDirPos pos;
    FSFile tmp;
    FS_InitFile(&tmp);
    if (!FSi_FindPath(&tmp, path, 0, &pos)) return 0;
    data_021fea6c = pos;
    return 1;
}

// FSi_GetPackedName
u32 FSi_GetPackedName(const char *name, int len) {
    u32 ret = 0;
    u32 d;
    int i;
    if (len <= 3) {
        for (i = 0; i < len; i++) {
            u32 c = (u8)name[i];
            if (c == 0) break;
            d = c - 'A';
            if (d <= 'Z' - 'A') c = d + 'a';
            else c = d + 'A';
            ret |= c << (i * 8);
        }
    }
    return ret;
}

// (a memory archive's positions are offsets from its base, the tables before the image negative: s32)
int FSi_ReadMemCallback(FSArc *arc, void *dst, u32 src, u32 len) {
    MI_CpuCopy8((void *)(arc->base + (s32)src), dst, len);
    return 0;
}

int FSi_WriteMemCallback(FSArc *arc, void *src, u32 dst, u32 len) {
    MI_CpuCopy8(src, (void *)(arc->base + (s32)dst), len);
    return 0;
}

// the tables loaded into memory (FS_LoadArchiveTables): src is their offset in arc->load_mem
int FSi_ReadMemoryCore(FSArc *arc, void *dst, u32 src, u32 len) {
    MI_CpuCopy8((u8 *)arc->load_mem + src, dst, len);
    return 0;
}

// FSi_NextCommand
FSFile *FSi_NextCommand(volatile FSArc *arc) {
    FSFile file;
    FSFile *cur;
    u32 irq = OS_DisableInterrupts();
    if (arc->flag & 0x20) {
        arc->flag &= ~0x20;
        cur = arc->list.next;
        if (cur) {
            do {
                FSFile *next = cur->next;
                if (IsCanceled(cur)) {
                    if (arc->list.next == cur) arc->list.next = next;
                    FSi_ReleaseCommand(cur, 3);
                    if (next == 0) next = arc->list.next;
                }
                cur = next;
            } while (cur);
        }
    }
    if (!(arc->flag & 0x40) && !(IsSuspended(arc)) && (cur = arc->list.next) != 0) {
        BOOL start = !(IsStarted(arc));
        if (start) arc->flag |= 0x10;
        OS_RestoreInterrupts(irq);
        if (start && (arc->proc_mask & 0x200)) arc->proc(cur, 9);
        irq = OS_DisableInterrupts();
        cur->stat |= 0x40;
        if (IsBlocking(cur)) {
            OS_WakeupThread(&cur->queue);
            OS_RestoreInterrupts(irq);
            return 0;
        }
        OS_RestoreInterrupts(irq);
        return cur;
    }
    if (arc->flag & 0x10) {
        arc->flag &= ~0x10;
        if (arc->proc_mask & 0x400) {
            FS_InitFile(&file);
            file.arc = (FSArc *)arc;
            arc->proc(&file, 10);
        }
    }
    if (arc->flag & 0x40) {
        arc->flag &= ~0x40;
        arc->flag |= 8;
        OS_WakeupThread((OSThreadQueue *)&arc->queue2);
    }
    OS_RestoreInterrupts(irq);
    return 0;
}

// FSi_ExecuteSyncCommand
void FSi_ExecuteAsyncCommand(FSFile *file) {
    FSArc *arc = file->arc;
    while (file) {
        u32 irq = OS_DisableInterrupts();
        file->stat |= 0x40;
        if (IsBlocking(file)) {
            OS_WakeupThread(&file->queue);
            OS_RestoreInterrupts(irq);
            return;
        }
        file->stat |= 8;
        OS_RestoreInterrupts(irq);
        if (FSi_TranslateCommand(file, file->command) == 6) return;
        file = FSi_NextCommand(arc);
    }
}

// FSi_EndCommand
int FSi_ExecuteSyncCommand(FSFile *file) {
    FSi_ReleaseCommand(file, FSi_TranslateCommand(file, file->command));
    {
        FSFile *next = FSi_NextCommand(file->arc);
        if (next) FSi_ExecuteAsyncCommand(next);
    }
    return file->error == 0;
}

// ---- file-scope objects (autoload_3 .bss 0x021fea78-0x021fea7c)
u32 data_021fea78;

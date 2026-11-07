#include "proto/autoload_2/unk_0210fa84.h"
#include "host_mem.h"
#include "proto/autoload_2/unk_02111ccc.h"
// mwcc-flags: -nothumb -O4,p
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#include "uptr.h"
typedef int s32;

extern s32 data_0213bfec; // GXi_DmaId
void MI_DmaCopy32(s32 dmaNo, const void *src, void *dest, u32 size); // MI_DmaCopy32
void MIi_CpuCopy32(const void *src, void *dest, u32 size);            // MI_CpuCopy32
void MI_DmaCopy16(s32 dmaNo, const void *src, void *dest, u32 size); // MI_DmaCopy16
void MIi_CpuCopy16(const void *src, void *dest, u32 size);            // MI_CpuCopy16
extern u32 data_021fcbf8;
extern s32 data_021fcbfc;
extern const u16 data_02139f44[8];
extern u32 data_021fcbf4, data_021fcc00, data_021fcc04, data_021fcc08;
void MI_WaitDma(s32 dmaNo);                                   // MI_WaitDma
s32 GX_ResetBankForTexPltt(void);
void MI_DmaCopy32Async(s32 dmaNo, const void *src, void *dest, u32 size, void *callback, void *arg);

static inline void copy32(s32 dmaNo, const void *src, void *dest, u32 size) {
    if (dmaNo != -1 && size > 48) {
        MI_DmaCopy32(dmaNo, src, dest, size);
    } else {
        MIi_CpuCopy32(src, dest, size);
    }
}

static inline void copy16(s32 dmaNo, const void *src, void *dest, u32 size) {
    if (dmaNo != -1 && size > 28) {
        MI_DmaCopy16(dmaNo, src, dest, size);
    } else {
        MIi_CpuCopy16(src, dest, size);
    }
}

// GX_EndLoadTex
void GX_EndLoadTex(void) {
    if (data_0213bfec != -1) {
        MI_WaitDma(data_0213bfec);
    }
#ifndef N3DS_DIRECT  // (the enhanced port: they stayed mapped, GX_BeginLoadTex)
    GX_SetBankForTex(data_021fcc00);
#endif
    data_021fcc08 = 0;
    data_021fcc04 = 0;
    data_021fcbf4 = 0;
    data_021fcc00 = 0;
}

// GX_BeginLoadBGExtPltt
void GX_BeginLoadTexPltt(void) {
#ifdef N3DS_DIRECT  // (the banks stay mapped: GX_LoadTexPltt writes into their memory)
    u32 GXi_TexPlttBanks(void);
    s32 v = (s32)GXi_TexPlttBanks();
#else
    s32 v = GX_ResetBankForTexPltt();
#endif
    data_021fcbfc = v;
    data_021fcbf8 = (u32)data_02139f44[v >> 4] << 12;
}

// GX_LoadBGExtPltt
void GX_LoadTexPltt(const void *src, u32 offset, u32 size) {
#ifdef N3DS_DIRECT  // (the enhanced port: straight into the banks' own memory, at once)
    MIi_CpuCopy32(src, host_vram_lcdc_direct(data_021fcbf8 + offset), size);
    return;
#endif
    u8 *dest = (u8 *)HostVram_CpuPtr(data_021fcbf8 + offset);  // (a VRAM address: LCDC)
    if (data_0213bfec != -1) {
        MI_DmaCopy32Async(data_0213bfec, src, dest, size, 0, 0);
    } else {
        MIi_CpuCopy32(src, dest, size);
    }
}

// GX_EndLoadBGExtPltt
void GX_EndLoadTexPltt(void) {
    if (data_0213bfec != -1) {
        MI_WaitDma(data_0213bfec);
    }
#ifndef N3DS_DIRECT  // (the enhanced port: they stayed mapped, GX_BeginLoadTexPltt)
    GX_SetBankForTexPltt(data_021fcbfc);
#endif
    data_021fcbfc = 0;
    data_021fcbf8 = 0;
}

// GX_LoadBGPltt (0x05000000)
void GX_LoadBGPltt(const void *src, u32 offset, u32 size) {
    copy16(data_0213bfec, src, (u8 *)HW_PLTT + offset, size);
}

// GXS_LoadBGPltt (0x05000400)
void GXS_LoadBGPltt(const void *src, u32 offset, u32 size) {
    copy16(data_0213bfec, src, (u8 *)(HW_PLTT + 0x400) + offset, size);
}

// GX_LoadOBJPltt (0x05000200)
void GX_LoadOBJPltt(const void *src, u32 offset, u32 size) {
    copy16(data_0213bfec, src, (u8 *)(HW_PLTT + 0x200) + offset, size);
}

// GXS_LoadOBJPltt (0x05000600)
void GXS_LoadOBJPltt(const void *src, u32 offset, u32 size) {
    copy16(data_0213bfec, src, (u8 *)(HW_PLTT + 0x600) + offset, size);
}

// GX_LoadOAM (main engine OAM, 0x07000000)
void GX_LoadOAM(const void *src, u32 offset, u32 size) {
    copy32(data_0213bfec, src, (u8 *)HW_OAM + offset, size);
}

// GXS_LoadOAM (sub engine OAM, 0x07000400)
void GXS_LoadOAM(const void *src, u32 offset, u32 size) {
    copy32(data_0213bfec, src, (u8 *)(HW_OAM + 0x400) + offset, size);
}

// ---- file-scope objects (.rodata 0x02139f44-0x02139f54)
const u16 data_02139f44[8] = {0x0000, 0x6880, 0x6890, 0x6880, 0x6894, 0x0000, 0x6890, 0x6880};

// ---- file-scope objects (autoload_3 .bss 0x021fcbf4-0x021fcc0c; this definition order gives the original order after mwcc's size
// sort)
u32 data_021fcbf8;
u32 data_021fcbf4;
u32 data_021fcc08;
s32 data_021fcbfc;
u32 data_021fcc00;
u32 data_021fcc04;

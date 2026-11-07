// mwcc-flags: -nothumb
// NitroSDK GX (gx.c and gx_vramcnt.c), autoload_2 0x0210f154-0x0210f900. ARM code, mwcc 1.2/base, default -O4,s.
// data_021fcbd8 is the 13-halfword VRAM bank state (index: 0 lcdc, 1 bg, 2 obj, 3 arm7, 4 tex, 5 texPltt,
// 6 clearImage, 7 bgExtPltt, 8 objExtPltt, 9 subBg, 10 subObj, 11 subBgExtPltt, 12 subObjExtPltt); the game and
// the delinked callers also refer to its members by their own symbols (data_021fcbda ... data_021fcbf0).
#include "nitro/io_access.h"
#include "host_labels.h"
#include "proto/autoload_2/unk_0210f154.h"
#include "proto/main/unk_0206d49c.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int BOOL;

extern u16 data_0213bfe8; // sIsDispOn
extern u32 data_0213bfec; // dma number used by GX_Init (-1: CPU)
extern u16 data_021fcbd0; // GX VRAM lock id
extern u16 data_021fcbd4; // sDispMode
extern u16 data_021fcbd8[13];
#define data_021fcbda HOST_LABEL(u16 *, data_021fcbda)
#define data_021fcbdc HOST_LABEL(u16 *, data_021fcbdc)
#define data_021fcbde HOST_LABEL(u16 *, data_021fcbde)
#define data_021fcbe0 HOST_LABEL(u16 *, data_021fcbe0)
#define data_021fcbe2 HOST_LABEL(u16 *, data_021fcbe2)
#define data_021fcbe4 HOST_LABEL(u16 *, data_021fcbe4)
#define data_021fcbe6 HOST_LABEL(u16 *, data_021fcbe6)
#define data_021fcbe8 HOST_LABEL(u16 *, data_021fcbe8)
#define data_021fcbea HOST_LABEL(u16 *, data_021fcbea)
#define data_021fcbec HOST_LABEL(u16 *, data_021fcbec)
#define data_021fcbee HOST_LABEL(u16 *, data_021fcbee)
#define data_021fcbf0 HOST_LABEL(u16 *, data_021fcbf0)

extern u32 OS_GetLockID(void);
extern void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size);
extern void MIi_CpuClear32(u32 data, void *dest, u32 size);
extern void OSi_UnlockVram(u16 a, u16 b);
extern void GX_VRAMCNT_SetLCDC_(u32 mask);

#define reg_GX_DISPSTAT (*(volatile u16 *)0x04000004)
#define reg_GX_DISPCNT (*(volatile u32 *)0x04000000)
#define reg_GXS_DB_DISPCNT (*(volatile u32 *)0x04001000)
#define reg_GX_POWCNT (*(volatile u16 *)0x04000304)
#define VRAMCNT(n) (*(volatile u8 *)(0x04000240 + (n)))

void GX_SetBankForSubOBJ(u32 bank);
void GX_SetBankForSubBGExtPltt(u32 bank);
void GX_SetBankForSubOBJExtPltt(u32 bank);
u32 resetBankForX_(u16 *p);
u32 GX_ResetBankForTex(void);
u32 GX_ResetBankForTexPltt(void);
u32 disableBankForX_(u16 *p);
u32 GX_DisableBankForBG(void);
u32 GX_DisableBankForOBJ(void);
u32 GX_DisableBankForBGExtPltt(void);
u32 GX_DisableBankForOBJExtPltt(void);
u32 GX_DisableBankForTex(void);
u32 GX_DisableBankForTexPltt(void);
u32 GX_DisableBankForClearImage(void);
u32 GX_DisableBankForARM7(void);
u32 GX_DisableBankForLCDC(void);
u32 GX_DisableBankForSubBG(void);
u32 GX_DisableBankForSubOBJ(void);
u32 GX_DisableBankForSubBGExtPltt(void);
u32 GX_DisableBankForSubOBJExtPltt(void);
u32 GX_GetBankForTex(void);
u32 getBankSize_(u32 mask);
u32 GX_GetSizeOfTexPltt(void);
BOOL GX_HBlankIntr(BOOL enable);
BOOL GX_VBlankIntr(BOOL enable);

// GX_SetBankForSubOBJ
void GX_SetBankForSubOBJ(u32 bank) {
    data_021fcbd8[0] = (u16)((data_021fcbd8[0] | data_021fcbd8[10]) & ~bank);
    data_021fcbd8[10] = bank;
    switch (bank) {
    case 0: break;
    case 8: IO_W(&VRAMCNT(3), 0x84); break;
    case 0x100: IO_W(&VRAMCNT(9), 0x82); break;
    }
    GX_VRAMCNT_SetLCDC_(data_021fcbd8[0]);
}

// GX_SetBankForSubBGExtPltt
void GX_SetBankForSubBGExtPltt(u32 bank) {
    data_021fcbd8[0] = (u16)((data_021fcbd8[0] | data_021fcbd8[11]) & ~bank);
    data_021fcbd8[11] = bank;
    switch (bank) {
    case 0x80:
        IO_OR(&reg_GXS_DB_DISPCNT, 0x40000000);
        IO_W(&VRAMCNT(8), 0x82);
        break;
    case 0:
        IO_AND(&reg_GXS_DB_DISPCNT, ~0x40000000);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_021fcbd8[0]);
}

// GX_SetBankForSubOBJExtPltt
void GX_SetBankForSubOBJExtPltt(u32 bank) {
    data_021fcbd8[0] = (u16)((data_021fcbd8[0] | data_021fcbd8[12]) & ~bank);
    data_021fcbd8[12] = bank;
    switch (bank) {
    case 0x100:
        IO_OR(&reg_GXS_DB_DISPCNT, 0x80000000);
        IO_W(&VRAMCNT(9), 0x83);
        break;
    case 0:
        IO_AND(&reg_GXS_DB_DISPCNT, ~0x80000000);
        break;
    }
    GX_VRAMCNT_SetLCDC_(data_021fcbd8[0]);
}

// GXi_ResetBank (bank mask goes back to LCDC)
u32 resetBankForX_(u16 *p) {
    u16 v = *p;
    *p = 0;
    data_021fcbd8[0] |= v;
    GX_VRAMCNT_SetLCDC_(v);
    return v;
}

// GX_ResetBankForTex
u32 GX_ResetBankForTex(void) { return resetBankForX_(&data_021fcbe0); }

// GX_ResetBankForTexPltt
u32 GX_ResetBankForTexPltt(void) { return resetBankForX_(&data_021fcbe2); }

#ifdef N3DS_DIRECT
// the enhanced port (VRAM): the texture and texture palette banks, left where they are for an
// upload (GX_BeginLoadTex, GX_BeginLoadTexPltt)
u32 GXi_TexBanks(void) { return data_021fcbe0; }
u32 GXi_TexPlttBanks(void) { return data_021fcbe2; }
#endif

// GXi_DisableBank (clears the VRAMCNT registers of the banks in *p)
u32 disableBankForX_(u16 *p) {
    u32 v = *p;
    *p = 0;
    if (v & 1) IO_W(&VRAMCNT(0), 0);
    if (v & 2) IO_W(&VRAMCNT(1), 0);
    if (v & 4) IO_W(&VRAMCNT(2), 0);
    if (v & 8) IO_W(&VRAMCNT(3), 0);
    if (v & 0x10) IO_W(&VRAMCNT(4), 0);
    if (v & 0x20) IO_W(&VRAMCNT(5), 0);
    if (v & 0x40) IO_W(&VRAMCNT(6), 0);
    if (v & 0x80) IO_W(&VRAMCNT(8), 0);
    if (v & 0x100) IO_W(&VRAMCNT(9), 0);
    OSi_UnlockVram(v, data_021fcbd0);
    return v;
}

// GX_DisableBankForBG
u32 GX_DisableBankForBG(void) { return disableBankForX_(&data_021fcbda); }

// GX_DisableBankForOBJ
u32 GX_DisableBankForOBJ(void) { return disableBankForX_(&data_021fcbdc); }

// GX_DisableBankForBGExtPltt
u32 GX_DisableBankForBGExtPltt(void) {
    IO_AND(&reg_GX_DISPCNT, ~0x40000000);
    return disableBankForX_(&data_021fcbe6);
}

// GX_DisableBankForOBJExtPltt
u32 GX_DisableBankForOBJExtPltt(void) {
    IO_AND(&reg_GX_DISPCNT, ~0x80000000);
    return disableBankForX_(&data_021fcbe8);
}

// GX_DisableBankForTex
u32 GX_DisableBankForTex(void) { return disableBankForX_(&data_021fcbe0); }

// GX_DisableBankForTexPltt
u32 GX_DisableBankForTexPltt(void) { return disableBankForX_(&data_021fcbe2); }

// GX_DisableBankForClearImage
u32 GX_DisableBankForClearImage(void) { return disableBankForX_(&data_021fcbe4); }

// GX_DisableBankForARM7
u32 GX_DisableBankForARM7(void) { return disableBankForX_(&data_021fcbde); }

// GX_DisableBankForLCDC
u32 GX_DisableBankForLCDC(void) { return disableBankForX_(&data_021fcbd8[0]); }

// GX_DisableBankForSubBG
u32 GX_DisableBankForSubBG(void) { return disableBankForX_(&data_021fcbea); }

// GX_DisableBankForSubOBJ
u32 GX_DisableBankForSubOBJ(void) { return disableBankForX_(&data_021fcbec); }

// GX_DisableBankForSubBGExtPltt
u32 GX_DisableBankForSubBGExtPltt(void) {
    IO_AND(&reg_GXS_DB_DISPCNT, ~0x40000000);
    return disableBankForX_(&data_021fcbee);
}

// GX_DisableBankForSubOBJExtPltt
u32 GX_DisableBankForSubOBJExtPltt(void) {
    IO_AND(&reg_GXS_DB_DISPCNT, ~0x80000000);
    return disableBankForX_(&data_021fcbf0);
}

// GX_GetBankForTex
u32 GX_GetBankForTex(void) { return data_021fcbd8[4]; }

// GXi_GetBankSize (total size in bytes of a VRAM bank mask)
u32 getBankSize_(u32 mask) {
    u32 size = 0;
    if (mask & 1) size += 0x20000;
    if (mask & 2) size += 0x20000;
    if (mask & 4) size += 0x20000;
    if (mask & 8) size += 0x20000;
    if (mask & 0x10) size += 0x10000;
    if (mask & 0x20) size += 0x4000;
    if (mask & 0x40) size += 0x4000;
    if (mask & 0x80) size += 0x8000;
    if (mask & 0x100) size += 0x4000;
    return size;
}

// GX_GetSizeOfTexPltt
u32 GX_GetSizeOfTexPltt(void) { return getBankSize_(data_021fcbd8[5]); }

// GXi_InitVRamState (clears the bank state and VRAMCNT_A..I; called by GX_Init)
void GX_InitGXState(void) {
    data_021fcbd8[0] = 0;
    data_021fcbd8[1] = 0;
    data_021fcbd8[2] = 0;
    data_021fcbd8[3] = 0;
    data_021fcbd8[4] = 0;
    data_021fcbd8[5] = 0;
    data_021fcbd8[6] = 0;
    data_021fcbd8[7] = 0;
    data_021fcbd8[8] = 0;
    data_021fcbd8[9] = 0;
    data_021fcbd8[10] = 0;
    data_021fcbd8[11] = 0;
    data_021fcbd8[12] = 0;
    IO_W((volatile u32 *)0x04000240, 0);
    IO_W((volatile u8 *)0x04000244, 0);
    IO_W((volatile u8 *)0x04000245, 0);
    IO_W((volatile u8 *)0x04000246, 0);
    IO_W((volatile u16 *)0x04000248, 0);
}

// GX_Init
void GX_Init(void) {
    IO_OR(&reg_GX_POWCNT, 0x8000);
    IO_W(&reg_GX_POWCNT, (u16)((IO_R(&reg_GX_POWCNT) & 0xfffffdf1) | 0x20e));
    IO_OR(&reg_GX_POWCNT, 1);
    GX_InitGXState();
    if (data_021fcbd0 == 0) {
        do {
            u32 id = OS_GetLockID();
            if (id == (u32)-3) Fatal_Trap();
            data_021fcbd0 = (u16)id;
        } while (data_021fcbd0 == 0);
    }
    IO_W(&reg_GX_DISPSTAT, 0);
    IO_W(&reg_GX_DISPCNT, 0);
    if (data_0213bfec != (u32)-1) {
        MI_DmaFill32(data_0213bfec, (void *)0x04000008, 0, 0x60);
        IO_W((volatile u16 *)0x0400006c, 0);
        MI_DmaFill32(data_0213bfec, (void *)0x04001000, 0, 0x70);
    } else {
        volatile u32 tmp = 0;
        MIi_CpuClear32(tmp, (void *)0x04000008, 0x60);
        IO_W((volatile u16 *)0x0400006c, 0);
        {
            volatile u32 tmp2 = 0;
            MIi_CpuClear32(tmp2, (void *)0x04001000, 0x70);
        }
    }
    IO_W((volatile u16 *)0x04000020, 0x100);
    IO_W((volatile u16 *)0x04000026, 0x100);
    IO_W((volatile u16 *)0x04000030, 0x100);
    IO_W((volatile u16 *)0x04000036, 0x100);
    IO_W((volatile u16 *)0x04001020, 0x100);
    IO_W((volatile u16 *)0x04001026, 0x100);
    IO_W((volatile u16 *)0x04001030, 0x100);
    IO_W((volatile u16 *)0x04001036, 0x100);
}

// GX_HBlankIntr
BOOL GX_HBlankIntr(BOOL enable) {
    volatile u16 *p = &reg_GX_DISPSTAT;
    BOOL pre = IO_R(p) & 0x10;
    if (enable) IO_OR(p, 0x10);
    else IO_AND(p, ~0x10);
    return pre;
}

// GX_VBlankIntr
BOOL GX_VBlankIntr(BOOL enable) {
    volatile u16 *p = &reg_GX_DISPSTAT;
    BOOL pre = IO_R(p) & 0x8;
    if (enable) IO_OR(p, 0x8);
    else IO_AND(p, ~0x8);
    return pre;
}

// GX_DispOff
void GX_DispOff(void) {
    u32 cnt = IO_R(&reg_GX_DISPCNT);
    data_0213bfe8 = 0;
    data_021fcbd4 = (u16)((cnt & 0x30000) >> 16);
    IO_W(&reg_GX_DISPCNT, cnt & ~0x30000);
}

// GX_DispOn
void GX_DispOn(void) {
    u32 mode;
    data_0213bfe8 = 1;
    mode = data_021fcbd4;
    if (mode != 0) {
        IO_W(&reg_GX_DISPCNT, (IO_R(&reg_GX_DISPCNT) & ~0x30000) | (mode << 16));
    } else {
        IO_OR(&reg_GX_DISPCNT, 0x10000);
    }
}

// ---- file-scope objects (autoload_3 .bss 0x021fcbd8-0x021fcbf4): the VRAM bank assignment record of gx_vramcnt.c;
// data_021fcbda .. data_021fcbf0 are its members (interior labels, autoload_3 lcf_symbols.txt)
u16 data_021fcbd8[13];

// ---- labels inside this unit's variables (generated: tools/host_labels.py) ----
#include "host_labels.h"

HOST_LABEL_DEFINE(data_021fcbda, (char *)&data_021fcbd8[1]);
HOST_LABEL_DEFINE(data_021fcbdc, (char *)&data_021fcbd8[2]);
HOST_LABEL_DEFINE(data_021fcbde, (char *)&data_021fcbd8[3]);
HOST_LABEL_DEFINE(data_021fcbe0, (char *)&data_021fcbd8[4]);
HOST_LABEL_DEFINE(data_021fcbe2, (char *)&data_021fcbd8[5]);
HOST_LABEL_DEFINE(data_021fcbe4, (char *)&data_021fcbd8[6]);
HOST_LABEL_DEFINE(data_021fcbe6, (char *)&data_021fcbd8[7]);
HOST_LABEL_DEFINE(data_021fcbe8, (char *)&data_021fcbd8[8]);
HOST_LABEL_DEFINE(data_021fcbea, (char *)&data_021fcbd8[9]);
HOST_LABEL_DEFINE(data_021fcbec, (char *)&data_021fcbd8[10]);
HOST_LABEL_DEFINE(data_021fcbee, (char *)&data_021fcbd8[11]);
HOST_LABEL_DEFINE(data_021fcbf0, (char *)&data_021fcbd8[12]);

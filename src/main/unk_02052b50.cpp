#include "types.h"
#include "sys/RecordFile.h"
#include "n3ds_options.h"
#include "game/InfoTableSet.h"
#include "nitro/io_access.h"
#include "proto/main/unk_02052b50.h"

extern "C" {
extern u32 data_0213bfec;
void MI_DmaFill32(u32, void *, u32, u32);
void GX_DisableBankForLCDC();
void GX_DisableBankForBG();
void GX_DisableBankForOBJ();
void GX_DisableBankForARM7();
void GX_DisableBankForTex();
void GX_DisableBankForTexPltt();
void GX_DisableBankForClearImage();
void GX_DisableBankForBGExtPltt();
void GX_DisableBankForOBJExtPltt();
void GX_DisableBankForSubBG();
void GX_DisableBankForSubOBJ();
void GX_DisableBankForSubBGExtPltt();
void GX_DisableBankForSubOBJExtPltt();
s32 Item_MakeFurniture(s32 a, s32 b);
s32 Item_GetFurnitureIndex(void *p);
s32 Item_IsFurniture(void *p);
}




struct FtrInfoRecord {
    u8 fossilGroup;
    u8 indoorFlags;
    u8 indoorFlags2;
    u8 unk_03;
    u8 unk_04;
    u8 previewScale;
    u8 previewOffsetY;
    u8 unk_07[4];
    u8 ftrName;
};

#define CLAMP(n) if (n >= 0x6e9) n = 0x6e8;

extern "C" {
}

static inline BOOL Unk_02052dac_Bit(s32 v, s32 n) {
    if ((v >> n) & 1) return TRUE;
    return FALSE;
}

static inline BOOL Unk_02052c54_Range(u16 *p) {
    BOOL r = FALSE;
    if (*p >= 0x450c && *p <= 0x45db) r = TRUE;
    return r;
}

extern "C" {

void Gfx_DisableAllBanks()
{
    GX_DisableBankForLCDC();
    GX_DisableBankForBG();
    GX_DisableBankForOBJ();
    GX_DisableBankForARM7();
    GX_DisableBankForTex();
    GX_DisableBankForTexPltt();
    GX_DisableBankForClearImage();
    GX_DisableBankForBGExtPltt();
    GX_DisableBankForOBJExtPltt();
    GX_DisableBankForSubBG();
    GX_DisableBankForSubOBJ();
    GX_DisableBankForSubBGExtPltt();
    GX_DisableBankForSubOBJExtPltt();
}

void Gfx_ResetDisplayRegs()
{
    volatile u16 *r = (volatile u16 *)0x4000000;
    u8 *b = (u8 *)r;
    IO_OR((volatile u16 *)(b + 0x304), 0x820e);
    Gfx_DisableAllBanks();
    IO_W((volatile u32 *)b, IO_R((volatile u32 *)b) & 0xf000f);
    MI_DmaFill32(data_0213bfec, b + 8, 0, 0x48);
    MI_DmaFill32(data_0213bfec, b + 0x60, 0, 8);
    IO_W((volatile u16 *)(b + 0x6c), 0);
    IO_W((volatile u32 *)(b + 0x1000), IO_R((volatile u32 *)(b + 0x1000)) & 0x10000);
    MI_DmaFill32(data_0213bfec, b + 0x1008, 0, 0x48);
    IO_W((volatile u16 *)(b + 0x106c), 0);
    u16 v = 0x100;
    IO_W((volatile u16 *)(b + 0x20), v);
    IO_W((volatile u16 *)(b + 0x26), v);
    IO_W((volatile u16 *)(b + 0x30), v);
    IO_W((volatile u16 *)(b + 0x36), v);
    IO_W((volatile u16 *)(b + 0x1020), v);
    IO_W((volatile u16 *)(b + 0x1026), v);
    IO_W((volatile u16 *)(b + 0x1030), v);
    IO_W((volatile u16 *)(b + 0x1036), v);
}

}

InfoTableSet::InfoTableSet()
{
}

InfoTableSet::~InfoTableSet()
{
    close();
}

InfoTableSet gFtrInfo;

extern "C" {

void FtrInfoTables_Open(void *p)
{
#ifdef N3DS_DIRECT  // (the EUR records' size, n3ds_options.h)
    ((InfoTableSet *)p)->open((void *)"/ftr_info/always.bin", 8, (void *)"/ftr_info/indoor.bin", 4, (void *)"/ftr_info/dma.bin",
                              N3ds_LangEur() ? 0x20 : 0x1c, 0x800);
    return;
#endif
    ((InfoTableSet *)p)->open((void *)"/ftr_info/always.bin", 8, (void *)"/ftr_info/indoor.bin", 4, (void *)"/ftr_info/dma.bin", 0x1c, 0x800);
}

void FtrInfoTables_Close(void *p)
{
    ((InfoTableSet *)p)->close();
}

void FtrInfoTables_LoadIndoor(void *p, s32 a)
{
    ((InfoTableSet *)p)->loadIndoor(a);
}

void FtrInfoTables_FreeIndoor(void *p)
{
    ((InfoTableSet *)p)->freeIndoor();
}

void FtrInfo_Init()
{
    FtrInfoTables_Open(&gFtrInfo);
}

void FtrInfo_Exit()
{
    FtrInfoTables_Close(&gFtrInfo);
}

void FtrInfo_LoadIndoor(s32 a)
{
    FtrInfoTables_LoadIndoor(&gFtrInfo, a);
}

void FtrInfo_FreeIndoor()
{
    FtrInfoTables_FreeIndoor(&gFtrInfo);
}

s32 FtrInfo_GetPrice(s32 n)
{
    CLAMP(n)
    u16 *r = (u16 *)gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[0];
    }
    return 0;
}

s32 FtrInfo_GetDmaUnk02(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[2];
    }
    return 0;
}

s32 FtrInfo_GetDmaUnk03Fx(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    s32 v;
    if (r) {
        v = r[3];
    } else {
        v = 0;
    }
    return (v << 12) >> 4;
}

s32 FtrInfo_GetNameForm()
{
    return -1;
}

s32 FtrInfo_GetNameAttrB(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[9];
    }
    return 0;
}

s32 FtrInfo_GetNameAttrA(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[10];
    }
    return 0;
}

s32 FtrInfo_GetDmaUnk07(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[7];
    }
    return 0;
}

s32 FtrInfo_GetDmaUnk08(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    s32 v;
    if (r) {
        v = r[8];
    } else {
        v = 0;
    }
    s32 m = -1;
    if (v == 0xff) {
        v = m;
    }
    return v;
}

s32 FtrInfo_GetColor1(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[1];
    }
    return 0;
}

s32 FtrInfo_GetColor2(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[2];
    }
    return 0;
}

s32 FtrInfo_GetSeries(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[0];
    }
    return 0;
}

s32 Ftr_GetSeries(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetSeries(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetClass(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[4];
    }
    return 0;
}

s32 Ftr_GetClass(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetClass(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetUnk05(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[5];
    }
    return 0;
}

s32 Ftr_GetUnk05(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetUnk05(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetCollectionGroup(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[3];
    }
    return 0;
}

s32 Ftr_GetCollectionGroup(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetCollectionGroup(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_TestAlwaysFlag4(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return (r[7] >> 4) & 1 ? TRUE : FALSE;
    }
    return 0;
}

s32 FtrInfo_GetFlagPairA(s32 n)
{
    s32 t;
    BOOL a;
    if (n >= 0x6e9) t = 0x6e8; else t = n;
    u8 *r = gFtrInfo.getAlways()->getRecord(t);
    if (r) {
        a = r[7] & 1 ? TRUE : FALSE;
    } else {
        a = FALSE;
    }
    CLAMP(n)
    u8 *q = gFtrInfo.getAlways()->getRecord(n);
    if (a) {
        return 1;
    }
    BOOL b;
    if (q) {
        b = (q[7] >> 1) & 1 ? TRUE : FALSE;
    } else {
        b = FALSE;
    }
    if (b) {
        return 2;
    }
    return 0;
}

s32 Ftr_GetFlagPairA(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetFlagPairA(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetFlagPairB(s32 n)
{
    s32 t;
    BOOL a;
    if (n >= 0x6e9) t = 0x6e8; else t = n;
    u8 *r = gFtrInfo.getAlways()->getRecord(t);
    if (r) {
        a = (r[7] >> 2) & 1 ? TRUE : FALSE;
    } else {
        a = FALSE;
    }
    CLAMP(n)
    u8 *q = gFtrInfo.getAlways()->getRecord(n);
    if (a) {
        return 1;
    }
    BOOL b;
    if (q) {
        b = (q[7] >> 3) & 1 ? TRUE : FALSE;
    } else {
        b = FALSE;
    }
    if (b) {
        return 2;
    }
    return 0;
}

s32 Ftr_GetFlagPairB(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetFlagPairB(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetUnk06(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getAlways()->getRecord(n);
    if (r) {
        return r[6];
    }
    return 0;
}

s32 Ftr_GetUnk06(u16 *item)
{
    if (Item_IsFurniture(item)) {
        return FtrInfo_GetUnk06(Item_GetFurnitureIndex(item));
    }
    return 0;
}

s32 FtrInfo_GetDmaUnk04(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getDma()->getRecord(n);
    if (r) {
        return r[4];
    }
    return 0;
}

s32 FtrInfo_IsLucky(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getIndoor()->getRecord(n);
    if (r) {
        return r[1] & 1 ? TRUE : FALSE;
    }
    return 0;
}

s32 FtrInfo_TestIndoorFlag2(s32 n)
{
    CLAMP(n)
    u8 *r = gFtrInfo.getIndoor()->getRecord(n);
    if (r) {
        return (r[1] >> 2) & 1 ? TRUE : FALSE;
    }
    return 0;
}

BOOL FtrInfo_TestIndoorFlag3(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(i);
    if (r) {
        { s32 v = r->indoorFlags; if ((v >> 3) & 1) return TRUE; }
        return FALSE;
    }
    return FALSE;
}

s32 FtrInfo_GetIndoorFlagPair(s32 i) {
    s32 j = i >= 0x6e9 ? 0x6e8 : i;
    BOOL f;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(j);
    if (r) {
        f = Unk_02052dac_Bit(r->indoorFlags, 4);
    } else {
        f = FALSE;
    }
    if (f) return 1;
    if (i >= 0x6e9) i = 0x6e8;
    r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(i);
    if (r) {
        f = Unk_02052dac_Bit(r->indoorFlags, 5);
    } else {
        f = FALSE;
    }
    if (f) return 2;
    return 0;
}

u8 *FtrInfo_GetName(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getDma()->getRecord(i);
#ifdef N3DS_DIRECT
    if (r && N3ds_LangEur()) return &r->ftrName + 1;  // (the EUR record's form byte, n3ds_options.h)
#endif
    if (r) return &r->ftrName;
    return 0;
}

#ifdef N3DS_DIRECT
s32 N3ds_FtrNameForm(s32 i) {  // FtrInfo_GetNameForm with the EUR records' form byte (src/main/unk_02062530.cpp)
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = N3ds_LangEur() ? (FtrInfoRecord *)gFtrInfo.getDma()->getRecord(i) : 0;
    return r ? (s8)(&r->ftrName)[0] : -1;
}
#endif

BOOL FtrInfo_TestIndoorFlag6(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(i);
    if (r) {
        { s32 v = r->indoorFlags; if ((v >> 6) & 1) return TRUE; }
        return FALSE;
    }
    return FALSE;
}

BOOL FtrInfo_TestIndoorFlag7(s32 i) {
    s32 j = i >= 0x6e9 ? 0x6e8 : i;
    BOOL f;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(j);
    if (r) {
        f = Unk_02052dac_Bit(r->indoorFlags, 7);
    } else {
        f = FALSE;
    }
    s32 t = FtrInfo_GetDmaUnk02(i);
    if (t != 0x23 && t != 0x24 && t != 0x25) return f;
    return FALSE;
}

BOOL Ftr_TestIndoorFlag7(u16 *p) {
    if (Item_IsFurniture(p)) return FtrInfo_TestIndoorFlag7(Item_GetFurnitureIndex(p));
    return FALSE;
}

BOOL FtrInfo_TestIndoorFlagC(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(i);
    if (r) {
        if (r->indoorFlags2 & 1) return TRUE;
        return FALSE;
    }
    return FALSE;
}

BOOL Ftr_TestIndoorFlagC(u16 *p) {
    if (Item_IsFurniture(p)) return FtrInfo_TestIndoorFlagC(Item_GetFurnitureIndex(p));
    return FALSE;
}

u32 FtrInfo_GetDmaUnk05Fx(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getDma()->getRecord(i);
    u32 v;
    if (r) v = r->previewScale;
    else v = 0;
    return v << 12;
}

s8 FtrInfo_GetDmaUnk06(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getDma()->getRecord(i);
    u8 v;
    if (r) v = r->previewOffsetY;
    else v = 0;
    return (s8)v;
}

u8 FtrInfo_GetIndoorUnk0(s32 i) {
    if (i >= 0x6e9) i = 0x6e8;
    FtrInfoRecord *r = (FtrInfoRecord *)gFtrInfo.getIndoor()->getRecord(i);
    if (r) return r->fossilGroup;
    return 0;
}

s32 Item_GetFossilGroup(u16 *p) {
    if (Unk_02052c54_Range(p)) return FtrInfo_GetIndoorUnk0(Item_GetFurnitureIndex(p));
    return 0;
}

u32 Fossil_CountInGroup(u32 v) {
    u32 cnt = 0;
    u32 i = 0;
    u16 tmp;
    for (; i < 0x34; i++) {
        tmp = i < 0x34 ? 0x450c + i * 4 : 0x450c;
        if (v == Item_GetFossilGroup(&tmp)) cnt++;
    }
    return cnt;
}

u32 Ftr_GetIndexInSeries(u16 *p) {
    s32 t = Ftr_GetSeries(p);
    u32 cnt = 0;
    s32 i = 0;
    BOOL z0 = FALSE, z8 = FALSE, zc = FALSE;
    u16 e;
    for (; (u32)i < 0x6e9; i++) {
        BOOL m;
        e = Item_MakeFurniture(i, z0);
        if (Item_IsFurniture(&e)) {
            s32 a = Item_GetFurnitureIndex(&e);
            m = (a == Item_GetFurnitureIndex(p)) ? TRUE : z8;
        } else {
            m = (e == *p) ? TRUE : zc;
        }
        if (m) return (u8)cnt;
        if (t == FtrInfo_GetSeries(i)) cnt++;
    }
    return 0;
}

u32 FtrInfo_CountInSeries(u32 v) {
    u8 cnt = 0;
    volatile u16 tmp[1];
    for (u32 i = 0, z = 0; i < 0x6e9; i++) {
        tmp[0] = Item_MakeFurniture(i, z);
        if (v == FtrInfo_GetSeries(i)) cnt++;
    }
    return cnt;
}

}

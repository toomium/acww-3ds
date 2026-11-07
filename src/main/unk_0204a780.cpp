#include "types.h"
#include "n3ds_options.h"
#include "host_labels.h"
#include "field/MoneyRockState.h"
#include "town/TownState.h"
#include "town/TownEventRecord.h"
#include "town/TownBlockMap.h"
#include "net/CommManager.h"
#include "proto/main/unk_0204a780.h"
#include "proto/main/unk_0204eeb4.h"
#include "proto/main/unk_02052b50.h"
#include "proto/main/unk_02060b9c.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02097444.h"

// An 8-byte bss block (used by unk_02041e00.cpp).
// Owned here by position: it lies between the data of the neighbouring files in link order and fits this file's
// size order (linkprep check); the original file is this one or another file between those neighbours.

MoneyRockState data_021c47bc;


extern "C" void Item_SetId(u16 *p, u32 v);
extern "C" void Item_FromPlacedForm(u16 *out, u16 *in);
struct ItemId {
    u16 v;
    ItemId();
    ItemId(u32 x) { Item_SetId(&v, x); }
    ItemId(u16 *src) { Item_FromPlacedForm(&v, src); }
    ~ItemId();
};


struct ItemIdNoInit {
    u16 v;
    ItemIdNoInit() {}
    ~ItemIdNoInit() {}  // ItemId's (empty) destructor on the DS
};

// ---- unk_0204a754.cpp
namespace nA {
extern "C" {

BOOL Item_IsFullHeadwear(u16 *p);
BOOL Item_IsWallpaper(u16 *p);
BOOL Item_IsUmbrella(u16 *p);
BOOL Item_IsPaper(u16 *p);
BOOL Item_IsAccessory(u16 *p);
BOOL Item_IsShirt(u16 *p);
BOOL Item_IsCarpet(u16 *p);
BOOL Item_IsHat(u16 *p);
BOOL Item_IsInRange(u16 *p, u32 lo, u32 hi);
s32 Item_GetId(u16 *p);
BOOL Item_IsWateredTulip(u16 *p);
BOOL Item_IsWateredPansy(u16 *p);
BOOL Item_IsWateredCosmos(u16 *p);
BOOL Item_IsWateredRose(u16 *p);
BOOL Item_IsFlowerWatered(u16 *p);
BOOL Item_IsFlowerFresh(u16 *p);
BOOL Item_IsRose(u16 *p);
BOOL Item_IsCosmos(u16 *p);
BOOL Item_IsPansy(u16 *p);
BOOL Item_IsTulip(u16 *p);
BOOL Item_IsFlowerParched(u16 *p);
BOOL Item_IsParchedRose(u16 *p);
BOOL Item_IsParchedCosmos(u16 *p);
BOOL Item_IsParchedPansy(u16 *p);
BOOL Item_IsParchedTulip(u16 *p);
BOOL Item_IsStump(u16 *p);
BOOL Item_IsPalmStump(u16 *p);
BOOL Item_IsCedarStump(u16 *p);
BOOL Item_IsTreeOrSpecialStump(u16 *p);
BOOL Item_IsSpecialStump(u16 *p);
BOOL Item_IsTreeStump(u16 *p);
BOOL Item_IsLargeStump(u16 *p);
BOOL Item_IsMediumStump(u16 *p);
BOOL Item_IsSmallStump(u16 *p);
BOOL Item_IsCherryTree(u16 *p);
BOOL Item_IsPeachTree(u16 *p);
BOOL Item_IsPearTree(u16 *p);
BOOL Item_IsOrangeTree(u16 *p);
BOOL Item_IsAppleTree(u16 *p);
BOOL Item_IsTree(u16 *p);
BOOL Item_IsPalmTree(u16 *p);
BOOL Item_IsFestiveCedar(u16 *p);
BOOL Item_IsSpecialCedar(u16 *p);
BOOL Item_IsAcornTree(u16 *p);
BOOL Item_IsSpecialTree(u16 *p);
BOOL Item_IsMoneyTree(u16 *p);
BOOL Item_IsFruitTree(u16 *p);
BOOL Item_IsCedarPlain(u16 *p);
BOOL Item_IsTreePlain(u16 *p);
BOOL Item_IsTreeStage3(u16 *p);
BOOL Item_IsTreeStage2(u16 *p);
BOOL Item_IsTreeStage1(u16 *p);
BOOL Item_IsTreeStage0(u16 *p);
u16 Item_MakeFullHeadwear(u32 x);
u16 Item_MakeWallpaper(u32 x);
u16 Item_MakeUmbrella(u32 x);
u16 Item_MakeAccessory(u32 x);
u16 Item_MakeCarpet(u32 x);
u16 Item_MakeHat(u32 x);
u16 Item_MakeInsect(u32 x);
u16 Item_MakeShirt(u32 x);
static inline BOOL Unk_0204a7d0_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
BOOL ItemList_IsFullHeadwear(u16 *p);
BOOL Item_IsFullHeadwear(u16 *p);
BOOL ItemList_IsWallpaper(u16 *p);
BOOL Item_IsWallpaper(u16 *p);
BOOL ItemList_IsUmbrella(u16 *p);
BOOL Item_IsUmbrella(u16 *p);
BOOL ItemList_IsPaper(u16 *p);
BOOL Item_IsPaper(u16 *p);
BOOL ItemList_IsAccessory(u16 *p);
BOOL Item_IsAccessory(u16 *p);
BOOL ItemList_IsShirt(u16 *p);
BOOL Item_IsShirt(u16 *p);
BOOL ItemList_IsCarpet(u16 *p);
BOOL Item_IsCarpet(u16 *p);
BOOL ItemList_IsHat(u16 *p);
BOOL Item_IsHat(u16 *p);
void Item_SetId(u16 *p, u32 v);
void Item_Assign(u16 *p, u32 v);
BOOL Item_IsFlower(u16 *p);
s32 Item_GetId(u16 *p);
BOOL Item_IsFlowerWatered(u16 *p);
BOOL Item_IsWateredRose(u16 *p);
BOOL Item_IsInRange(u16 *p, u32 lo, u32 hi);
BOOL Item_IsWateredCosmos(u16 *p);
BOOL Item_IsWateredPansy(u16 *p);
BOOL Item_IsWateredTulip(u16 *p);
BOOL Item_IsFlowerFresh(u16 *p);
BOOL Item_IsRose(u16 *p);
BOOL Item_IsCosmos(u16 *p);
BOOL Item_IsPansy(u16 *p);
BOOL Item_IsTulip(u16 *p);
BOOL Item_IsFlowerParched(u16 *p);
BOOL Item_IsParchedRose(u16 *p);
BOOL Item_IsParchedCosmos(u16 *p);
BOOL Item_IsParchedPansy(u16 *p);
BOOL Item_IsParchedTulip(u16 *p);
s32 Item_GetStumpSize(u16 *p);
BOOL Item_IsStump(u16 *p);
BOOL Item_IsPalmStump(u16 *p);
BOOL Item_IsCedarStump(u16 *p);
BOOL Item_IsTreeOrSpecialStump(u16 *p);
BOOL Item_IsSpecialStump(u16 *p);
BOOL Item_IsTreeStump(u16 *p);
BOOL Item_IsLargeStump(u16 *p);
BOOL Item_IsMediumStump(u16 *p);
BOOL Item_IsSmallStump(u16 *p);
s32 Item_GetFruitTreeFruit(u16 *p);
BOOL Item_IsCherryTree(u16 *p);
BOOL Item_IsPeachTree(u16 *p);
BOOL Item_IsPearTree(u16 *p);
BOOL Item_IsOrangeTree(u16 *p);
BOOL Item_IsAppleTree(u16 *p);
s32 Item_GetTreeStage(u16 *p);
BOOL Item_IsTree(u16 *p);
BOOL Item_IsPalmTree(u16 *p);
BOOL Item_IsFestiveCedar(u16 *p);
BOOL Item_IsSpecialCedar(u16 *p);
BOOL Item_IsAcornTree(u16 *p);
BOOL Item_IsSpecialTree(u16 *p);
BOOL Item_IsMoneyTree(u16 *p);
BOOL Item_IsFruitTree(u16 *p);
BOOL Item_IsCedarPlain(u16 *p);
BOOL Item_IsTreePlain(u16 *p);
BOOL Item_IsFruitTreeLastNoFruit(u16 *p);
BOOL Item_IsTreeGrown(u16 *p);
BOOL Item_IsTreeStage3(u16 *p);
BOOL Item_IsTreeStage2(u16 *p);
BOOL Item_IsTreeStage1(u16 *p);
}
}

// ---- unk_0204b08c.cpp
namespace nB {
extern "C" {

BOOL Item_IsPaper(u16 *p);
BOOL Item_IsShirt(u16 *p);
BOOL Item_IsInRange(u16 *p, u32 lo, u32 hi);
s32 Item_GetId(u16 *p);
void Item_SetId(u16 *p, u32 v);
void Item_Assign(u16 *p, u32 v);
u32 Item_GetFlowerAltCount();
u32 Item_GetFlowerItemCount();
BOOL ItemInfo_TestFlag1(u16 *p);
s32 ItemInfo_GetIndoorUnk0(u16 *p);
s32 ItemInfo_TestFlag4(u16 *p);
BOOL ItemInfo_TestFlag3(u16 *p);
s32 Fish_GetBaseSize(s32 x);
void Item_FromPlacedForm(u16 *out, u16 *in);
BOOL Item_IsTreeStage0(u16 *p);
u16 Item_MakePlayerHouse(u32 x);
BOOL Item_IsPlayerHouse(u16 *p);
u16 Item_MakeSnowman(u32 x);
s32 Item_GetSnowmanIndex(u16 *p);
BOOL Item_IsSnowman(u16 *p);
u16 Item_MakeNookShop(u32 x);
s32 Item_GetNookShopLevel(u16 *p);
BOOL Item_IsNookShop(u16 *p);
u16 Item_MakeNeighborHouse(u32 x);
u16 Item_MakeBuilding(u32 x);
BOOL Item_IsBuildingOrOccupied(u16 *p);
BOOL Item_IsBuilding(u16 *p);
void Item_SetFurnitureDirection(u16 *p, s32 y);
u16 Item_MakeFurniture(s32 a, s32 b);
s32 Item_GetFurnitureIndex(u16 *p);
s32 Item_GetFurnitureDirection(u16 *p);
BOOL Item_IsFurnitureOrF031(u16 *p);
BOOL Item_IsOccupiedF031(u16 *p);
BOOL ItemList_IsFurniture(u16 *p);
BOOL Item_IsFurniture(u16 *p);
s32 Item_GetIdClass(u16 *p);
BOOL Item_IsNormalItem(u16 *p);
u16 Item_MakePaper(u32 a, s32 b);
s32 Item_GetPaperCount(u16 *p);
s32 Item_GetPaperIndex(u16 *p);
BOOL Item_IsFlowerAltItem(u16 *p);
u16 Item_MakeFlowerItem(u32 x);
s32 Item_GetFlowerAltIndex(u16 *p);
BOOL Item_IsFlowerAltRange(u16 *p);
BOOL Item_IsFlowerItem(u16 *p);
s32 Item_GetFlowerItemIndex(u16 *p);
BOOL Item_IsFlowerItemRange(u16 *p);
void Item_GetNthFlowerAlt(u16 *out, u32 n);
void Item_CopyB(u16 *dst, u16 *src);
u16 Item_MakeFlowerAlt(u32 x);
void Item_GetNthFlowerItem(u16 *out, u32 n);
s32 Item_GetFlowerAltOrdinal(u16 *p);
s32 Item_GetFlowerItemOrdinal(u16 *p);
void Item_SetDesign(u16 *out, u32 a, u32 b);
u16 Item_MakeDesign(u32 a, u32 b);
u16 Item_MakeDesignByIndex(u32 x);
s32 Item_GetDesignSlot(u16 *p);
s32 Item_GetDesignIndex(u16 *p);
BOOL Item_IsDesign(u16 *p);
s32 Item_GetDesignPlayer(u16 *p);
s32 Item_FindMoneyBagForAmount(s32 a, BOOL up, s32 *out);
u16 Item_MakeMoneyBag(u32 x);
BOOL Item_GetShirtUnkGroup(u16 *p);
BOOL Item_TestInfoFlag4(u16 *p);
BOOL Item_TestInfoFlag3(u16 *p);
u16 Item_GetInsectBaseSize(u16 *p);
s32 Item_GetInsectIndex(u16 *p);
BOOL Item_IsInsect(u16 *p);
s32 Item_GetFishBaseSize(u16 *p);
s32 Item_GetFishIndex(u16 *p);
BOOL Item_IsFish(u16 *p);
BOOL Item_IsTreeStage0(u16 *p);
u16 Item_MakePlayerHouse(u32 x);
BOOL Item_IsPlayerHouse(u16 *p);
u16 Item_MakeSnowman(u32 x);
s32 Item_GetSnowmanIndex(u16 *p);
BOOL Item_IsSnowman(u16 *p);
u16 Item_MakeNookShop(u32 x);
s32 Item_GetNookShopLevel(u16 *p);
BOOL Item_IsNookShop(u16 *p);
u16 Item_MakeNeighborHouse(u32 x);
u16 Item_MakeBuilding(u32 x);
BOOL Item_IsBuildingOrOccupied(u16 *p);
BOOL Item_IsBuilding(u16 *p);
void Item_SetFurnitureDirection(u16 *p, s32 y);
u16 Item_MakeFurniture(s32 a, s32 b);
s32 Item_GetFurnitureIndex(u16 *p);
s32 Item_GetFurnitureDirection(u16 *p);
BOOL Item_IsFurnitureOrF031(u16 *p);
BOOL Item_IsOccupiedF031(u16 *p);
BOOL ItemList_IsFurniture(u16 *p);
BOOL Item_IsFurniture(u16 *p);
s32 Item_GetIdClass(u16 *p);
BOOL Item_IsNormalItem(u16 *p);
u16 Item_MakePaper(u32 a, s32 b);
s32 Item_GetPaperCount(u16 *p);
s32 Item_GetPaperIndex(u16 *p);
BOOL Item_IsFlowerAltItem(u16 *p);
u16 Item_MakeFlowerItem(u32 x);
s32 Item_GetFlowerAltIndex(u16 *p);
static inline BOOL Unk_0204b408_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
BOOL Item_IsFlowerAltRange(u16 *p);
BOOL Item_IsFlowerItem(u16 *p);
s32 Item_GetFlowerItemIndex(u16 *p);
BOOL Item_IsFlowerItemRange(u16 *p);
void Item_GetNthFlowerAlt(u16 *out, u32 n);
void Item_CopyB(u16 *dst, u16 *src);
u16 Item_MakeFlowerAlt(u32 x);
void Item_GetNthFlowerItem(u16 *out, u32 n);
s32 Item_GetFlowerAltOrdinal(u16 *p);
s32 Item_GetFlowerItemOrdinal(u16 *p);
void Item_SetDesign(u16 *out, u32 a, u32 b);
u16 Item_MakeDesign(u32 a, u32 b);
u16 Item_MakeDesignByIndex(u32 x);
s32 Item_GetDesignSlot(u16 *p);
s32 Item_GetDesignIndex(u16 *p);
BOOL Item_IsDesign(u16 *p);
s32 Item_GetDesignPlayer(u16 *p);
s32 Item_FindMoneyBagForAmount(s32 a, BOOL up, s32 *out);
u16 Item_MakeMoneyBag(u32 x);
BOOL Item_GetShirtUnkGroup(u16 *p);
BOOL Item_TestInfoFlag4(u16 *p);
BOOL Item_TestInfoFlag3(u16 *p);
u16 Item_GetInsectBaseSize(u16 *p);
s32 Item_GetInsectIndex(u16 *p);
BOOL Item_IsInsect(u16 *p);
s32 Item_GetFishBaseSize(u16 *p);
s32 Item_GetFishIndex(u16 *p);
}
}

// ---- unk_0204b9c0.cpp
namespace nC {
extern "C" {

struct Unk_0204c084_Data {
    u8 pad[0x15];
    s8 perfectStreak;
    u8 pad16;
    u8 seasonPeriod;
};
BOOL Item_IsFullHeadwear(u16 *p);
s32 Item_GetId(u16 *p);
void func_0204b950_dummy();
BOOL Item_IsInsect(u16 *p);
BOOL Item_IsFurniture(u16 *p);
BOOL Item_IsNormalItem(u16 *p);
s32 Item_GetIdClass(u16 *p);
s32 Item_GetFurnitureIndex(u16 *p);
void Item_Copy(u16 *dst, u16 *src);
s32 ItemInfo_IsHoldable(u16 *p);
s32 ItemInfo_GetKind(u16 *p);
s32 ItemInfo_GetUnk07(u16 *p);
s32 ItemInfo_GetUnk02(u16 *p);
s32 ItemInfo_GetPrice(const ItemId &p);
s32 NookShop_IsSaleTime(void *p);
s32 NookPoints_GetRank(u32 x);
void *PlayerData_GetNookPoints(void *p);  // (src/main/unk_02097d1c.cpp)
u16 *NookPoints_GetValuePtr(u16 *p);
s32 Clock_GetDate(ClockDate *d);
s32 Date_DaysBetween(ClockDate *a, ClockDate *b);
s32 Clock_GetYear();
void Event_GetRange(s32 *a, s32 *b, u32 c);
s32 _s32_div_f(s32 a, s32 b);
s32 FX_Div(s32 a, s32 b);
s32 Date_GetWeekday(u32 a, u32 b, u32 c);
void DateTime_AddDays(void *p, s32 n);
// (src/main/unk_0204cc1c.cpp; its proto header is not included here: town/TownMap.h -> item/ItemId.h collides with
// this file's own ItemId)
TownBlockMap *TownBlockMap_Get();
u16 *BlockMap_GetItemPtr(BlockMap *g, s32 hx, s32 hy, s32 lx, s32 ly, u8 layer);
BOOL BlockMap_SetItemAtUnit(BlockMap *g, u16 *a, s32 x, s32 z, u8 d);
extern void *gCommManager;
extern u8 gSaveData[];
#define data_021ed1b0 HOST_LABEL(Unk_0204c084_Data *, data_021ed1b0)
#define gSaveTownEvents HOST_LABEL(TownEventRecord (*)[], gSaveTownEvents)
BOOL Item_IsFish(u16 *p);
s32 Item_GetFishIndex(u16 *p);
BOOL Item_IsGyroid(u16 *p);
BOOL Item_IsFossil(u16 *p);
BOOL Item_IsPainting(u16 *p);
BOOL Item_IsMoneyBag(u16 *p);
s32 Item_GetMemberPrice(u16 *p);
s32 Item_GetPrice(u16 *p);
s32 Item_GetInfoUnk07(u16 *p);
BOOL Item_IsInvalid(u16 *p);
u8 *Item_GetSaveData();
BOOL Item_IsFruit(u16 *p);
s32 TownState_GetNativeFruit(u8 *p);
u16 Item_MakeFruit(s32 n);
BOOL Item_Equals(u16 *a, u16 *b);
s32 NookPoints_GetRankOf(u16 *p);
s32 TownState_FindEvent(TownState *x, u32 id);
TownEventRecord *TownState_GetEvent(s32 i);
ClockDate *TownState_GetPlayerDate(u8 *p, s32 i);
static inline BOOL Unk_0204b9c0_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
BOOL Item_IsFish(u16 *p);
s32 Item_GetFishWaterClass(u16 *p);
BOOL Item_GetIfNotCreature(u16 *a, u16 *out);
void Item_Copy(u16 *dst, u16 *src);
BOOL func_0204bab8(u16 *p);
s32 Item_IsHoldable(u16 *p);
s32 Item_GetKind(u16 *p);
BOOL Item_IsPainting(u16 *p);
BOOL Item_IsGyroid(u16 *p);
BOOL Item_IsFossil(u16 *p);
s32 Item_GetInfoUnk07(u16 *p);
s32 Item_GetInfoUnk02(u16 *p);
static inline BOOL Unk_0204bd14_A(u16 *p) {
    BOOL r = TRUE;
    if (!(*p == 0xf030 || *p == 0xf031)) r = FALSE;
    return r;
}
static inline BOOL Unk_0204bd14_B(u16 *p) {
    BOOL r = TRUE;
    if (!(Unk_0204bd14_A(p) || *p == 0xfffd)) r = FALSE;
    return r;
}
static inline BOOL Unk_0204bd14_C(u16 *p) {
    BOOL r = TRUE;
    if (!(Unk_0204bd14_B(p) || *p == 0xfffe)) r = FALSE;
    return r;
}
BOOL Item_IsMarker(u16 *p);
BOOL Item_IsInvalid(u16 *p);
s32 Item_GetShopPrice(u16 *p);
BOOL Item_IsMoneyBag(u16 *p);
s32 Item_GetMemberPrice(u16 *p);
s32 NookPoints_GetRankOf(u16 *p);
s32 Item_GetPrice(u16 *p);
BOOL Item_Equals(u16 *a, u16 *b);
u16 Item_MakeFruit(s32 n);
s32 TownState_GetNativeFruit(u8 *p);
BOOL Item_IsFruit(u16 *p);
void TownState_SetSeasonPeriod(u32 n);
u32 TownState_GetSeasonPeriod();
void TownState_UpdatePerfectStreak(TownState *p, s32 k, s32 add);
BOOL TownState_IsPerfectStreak15();
BOOL TownState_IsPlayerDateNotToday(u8 *p);
void TownState_SetPlayerDateToday(u8 *p);
ClockDate *TownState_GetPlayerDate(u8 *p, s32 i);
s32 TownState_AddEvent(TownState *x, u32 id);
s32 TownState_FindEvent(TownState *x, u32 id);
void TownState_RemoveEvent(TownState *x, u32 id);
void TownState_ClearEvents();
TownEventRecord *TownState_GetEvent(s32 i);
void TownState_ClearUnk16(TownState *p);
void TownState_ClearUnk0c(TownState *p);
void TownState_PickNextWeekDate(u8 *dst, u8 *src);
struct Unk_0204c290_V {
    u16 v;
    u16 pad;
};
void Town_ReplaceSouthCedars();
u8 *Item_GetSaveData();
}
}

namespace nC {
extern "C" void Town_ReplaceSouthCedars() {
    TownBlockMap *w;
    s32 wd, ht, x, y;
    s32 cx, cy;
    w = TownBlockMap_Get();
    if (w) {
        s32 *q = &w->width;
        wd = q[0] << 4;
        ht = q[1] << 4;
        for (y = 0x30; y < ht; y++) {
            x = 0;
            if (x < wd) {
                goto test;
            loop:
                {
                    cx = x >> 4; cy = y >> 4;
                    u16 *it = BlockMap_GetItemPtr(w, cx, cy, x - (cx << 4), y - (cy << 4), 0);
                    if (it) {
                        if (Unk_0204b9c0_R(it, 0x5d, 0x61)) {
                            u16 v = 0x2a;
                            BlockMap_SetItemAtUnit(w, &v, x, y, 0);
                        }
                    }
                }
                x++;
            test:
                if (x < wd) goto loop;
            }
        }
    }
}
}

namespace nC {
extern "C" void TownState_PickNextWeekDate(u8 *dst, u8 *src) {
    u32 l[2];
    l[0] = 0;
    l[1] = 0;
    s32 d = Date_GetWeekday(src[2], src[1], src[0]);
    s32 r;
    do {
        r = Random_GlobalBelow(7);
    } while (r == 1 || r == 2);
    l[0] = 0;
    l[1] = 0;
    ((u8 *)l)[5] = src[2];
    ((u8 *)l)[4] = src[1];
    ((u8 *)l)[3] = src[0];
    DateTime_AddDays(l, (7 - d) + r);
    dst[10] = ((u8 *)l)[5];
    dst[9] = ((u8 *)l)[4];
    dst[8] = ((u8 *)l)[3];
}
}

namespace nC {
extern "C" void TownState_ClearUnk0c(TownState *p) { for (s32 i = 0; i < 10; i++) p->unk_0c[i] = 0; }
}

namespace nC {
extern "C" void TownState_ClearUnk16(TownState *p) { for (s32 i = 0; i < 11; i++) p->unk_16[i] = 0; }
}

namespace nC {
extern "C" TownEventRecord *TownState_GetEvent(s32 i) { return &gSaveTownEvents[i]; }
}

namespace nC {
extern "C" void TownState_ClearEvents() {
    TownEventRecord *e = TownState_GetEvent(0);
    for (s32 i = 0; i < 4; e++, i++) {
        e->eventId = 0x63;
        e->start = 1;
        e->end = 1;
    }
}
}

namespace nC {
extern "C" void TownState_RemoveEvent(TownState *x, u32 id) {
    s32 i = TownState_FindEvent(x, id);
    if (i >= 0) {
        TownEventRecord *e = TownState_GetEvent(i);
        e->eventId = 0x63;
        e->start = 1;
        e->end = 1;
    }
}
}

namespace nC {
extern "C" s32 TownState_FindEvent(TownState *x, u32 id) {
    s32 r = -1;
    TownEventRecord *e = TownState_GetEvent(0);
    for (s32 i = 0; i < 4; e++, i++) {
        if (id == e->eventId) { r = i; break; }
    }
    return r;
}
}

namespace nC {
extern "C" s32 TownState_AddEvent(TownState *x, u32 id) {
    s32 i = TownState_FindEvent(x, id);
    if (i < 0) {
        i = TownState_FindEvent(x, 0x63);
        if (i >= 0) {
            TownEventRecord *e = TownState_GetEvent(i);
            e->eventId = id;
            Event_GetRange(&e->start, &e->end, id);
            e->year = Clock_GetYear();
        }
    }
    return i;
}
}

namespace nC {
extern "C" ClockDate *TownState_GetPlayerDate(u8 *p, s32 i) { return (ClockDate *)(p + 0x58) + i; }
}

namespace nC {
extern "C" void TownState_SetPlayerDateToday(u8 *p) { Clock_GetDate(TownState_GetPlayerDate(p, PlayerData_GetCurrentIndex())); }
}

namespace nC {
extern "C" BOOL TownState_IsPlayerDateNotToday(u8 *p) {
    BOOL r = FALSE;
    s32 i = PlayerData_GetCurrentIndex();
    ClockDate t;
    Clock_GetDate(&t);
    p += 0x58;
    if (Date_DaysBetween(&t, (ClockDate *)p + i)) r = TRUE;
    return r;
}
}

namespace nC {
extern "C" BOOL TownState_IsPerfectStreak15() {
    BOOL r = FALSE;
    if (data_021ed1b0.perfectStreak >= 0xf) r = TRUE;
    return r;
}
}

namespace nC {
extern "C" void TownState_UpdatePerfectStreak(TownState *p, s32 k, s32 add) {
    if (k == 4) {
        s32 v = p->perfectStreak;
        if (v < 0) p->perfectStreak = 1;
        else p->perfectStreak = v + add;
    } else {
        p->perfectStreak = -1;
    }
}
}

namespace nC {
extern "C" u32 TownState_GetSeasonPeriod() { return data_021ed1b0.seasonPeriod; }
}

namespace nC {
extern "C" void TownState_SetSeasonPeriod(u32 n) {
    if (!((CommManager *)(gCommManager))->isOnline()) {
        if (n >= 0x17) n = 0x16;
        data_021ed1b0.seasonPeriod = n;
    }
}
}

namespace nC {
extern "C" BOOL Item_IsFruit(u16 *p) { if (Unk_0204b9c0_R(p, 0x1518, 0x151c)) return TRUE; return FALSE; }
}

namespace nC {
extern "C" s32 TownState_GetNativeFruit(u8 *p) { return *(s32 *)(p + 4); }
}

namespace nC {
extern "C" u16 Item_MakeFruit(s32 n) { if ((u32)n < 5) return n + 0x1518; return 0x1518; }
}

namespace nC {
extern "C" BOOL Item_Equals(u16 *a, u16 *b) {
    if (Item_IsFurniture(a)) {
        if (Item_GetFurnitureIndex(a) == Item_GetFurnitureIndex(b)) return TRUE;
        return FALSE;
    }
    if (*a == *b) return TRUE;
    return FALSE;
}
}

namespace nC {
extern "C" s32 Item_GetPrice(u16 *p) {
#ifdef N3DS_DIRECT
    if (n3ds_fruit_count(*p) > 1) {  // (n3ds_options.h) a fruit stack: its fruit's price (Nook's local fruit rule) times n
        u16 one = n3ds_fruit_one(*p);
        return Item_GetPrice(&one) * n3ds_fruit_count(*p);
    }
#endif
    ItemId a(p);
    switch (Item_GetIdClass(&a.v)) {
    case 1:
        if (Item_IsMoneyBag(&a.v)) {
            return ItemInfo_GetPrice(ItemId(Item_GetId(&a.v))) * 10;
        } else if (Item_IsFruit(&a.v)) {
            ItemId c(Item_MakeFruit(TownState_GetNativeFruit(Item_GetSaveData() + 0x15e54)));
            if (Item_Equals(&a.v, &c.v)) {
                return ItemInfo_GetPrice(ItemId(Item_GetId(&a.v))) / 5;
            }
        } else if (Item_GetId(p) == 0x1406 || Item_GetId(p) == 0x1407) {
            return ItemInfo_GetPrice(ItemId(Item_GetId(&a.v))) * 100;
        }
        return ItemInfo_GetPrice(ItemId(Item_GetId(&a.v)));
    case 3:
    case 4: {
        s32 x = Item_GetFurnitureIndex(&a.v);
        if (x == 0x208) return FtrInfo_GetPrice(x) * 100;
        return FtrInfo_GetPrice(x);
    }
    default:
        return 0;
    }
}
}

#pragma dont_inline on
namespace nC {
extern "C" s32 NookPoints_GetRankOf(u16 *p) { return NookPoints_GetRank(*p); }
}
#pragma dont_inline reset

namespace nC {
extern "C" s32 Item_GetMemberPrice(u16 *p) {
    s32 a = Item_GetPrice(p);
    if (Item_IsMoneyBag(p)) return a;
    void *g = PlayerData_GetCurrent();
    if (g) {
        s32 c = NookPoints_GetRankOf(NookPoints_GetValuePtr((u16 *)PlayerData_GetNookPoints(g)));
        s32 k = 0;
        switch (c) {
        case 2: k = 5; break;
        case 3: k = 10; break;
        case 4: k = 20; break;
        }
        if (k == 0) return a;
        a = a * (100 - k);
        if (a >= 100) return _s32_div_f(a, 100);
        return FX_Div(a << 12, 0x64000) >> 12;
    }
    return a;
}
}

namespace nC {
extern "C" BOOL Item_IsMoneyBag(u16 *p) { if (Unk_0204b9c0_R(p, 0x1492, 0x14fd)) return TRUE; return FALSE; }
}

#pragma dont_inline on
namespace nC {
extern "C" u8 *Item_GetSaveData() { return gSaveData; }
}
#pragma dont_inline reset

namespace nC {
extern "C" s32 Item_GetShopPrice(u16 *p) {
    s32 a = Item_GetMemberPrice(p);
    if (Item_IsMoneyBag(p)) return a;
    a >>= NookShop_IsSaleTime(Item_GetSaveData() + 0x15db4);
    return a;
}
}

namespace nC {
extern "C" BOOL Item_IsInvalid(u16 *p) { if (*p == 0xffff) return TRUE; return FALSE; }
}

namespace nC {
extern "C" BOOL Item_IsMarker(u16 *p) {
    BOOL r = TRUE;
    if (!(Unk_0204bd14_C(p) || Item_IsInvalid(p))) r = FALSE;
    return r;
}
}

namespace nC {
extern "C" s32 Item_GetInfoUnk02(u16 *p) {
    switch (Item_GetIdClass(p)) {
    case 1: {
        ItemId e(Item_GetId(p));
        return ItemInfo_GetUnk02(&e.v);
    }
    case 3:
    case 4:
        if (Item_IsFossil(p)) return 0xa7;
        if (Item_IsGyroid(p)) return 1;
        return 0;
    default:
        return 0;
    }
}
}

namespace nC {
extern "C" s32 Item_GetInfoUnk07(u16 *p) {
    ItemId a(p);
    switch (Item_GetIdClass(&a.v)) {
    case 1: {
        ItemId b(Item_GetId(&a.v));
        return ItemInfo_GetUnk07(&b.v);
    }
    case 3:
    case 4:
        if (Item_IsGyroid(&a.v)) return 1;
        return 0;
    default:
        return 0;
    }
}
}

namespace nC {
extern "C" BOOL Item_IsFossil(u16 *p) { if (Unk_0204b9c0_R(p, 0x450c, 0x45db)) return TRUE; return FALSE; }
}

namespace nC {
extern "C" BOOL Item_IsGyroid(u16 *p) { if (Unk_0204b9c0_R(p, 0x45dc, 0x47d7)) return TRUE; return FALSE; }
}

namespace nC {
extern "C" BOOL Item_IsPainting(u16 *p) { if (Unk_0204b9c0_R(p, 0x3894, 0x38e3)) return TRUE; return FALSE; }
}

namespace nC {
extern "C" s32 Item_GetKind(u16 *p) {
    ItemId a(p);
    if (Item_GetIdClass(&a.v) == 1) {
        ItemId b(Item_GetId(p));
        return ItemInfo_GetKind(&b.v);
    } else if (Item_IsFurniture(&a.v)) {
        if (Item_IsFossil(&a.v)) return 0x39;
        if (Item_IsGyroid(&a.v)) return 0x3a;
        if (Item_IsPainting(&a.v)) return 0x3a;
        return 0x3b;
    }
    return 0;
}
}

namespace nC {
extern "C" s32 Item_IsHoldable(u16 *p) {
    if (Item_GetIdClass(p) == 1) {
        ItemId e(Item_GetId(p));
        return ItemInfo_IsHoldable(&e.v);
    }
    return 0;
}
}

namespace nC {
extern "C" BOOL func_0204bab8(u16 *p) {
    if (Item_IsFullHeadwear(p)) {
        if (Item_GetInfoUnk07(p) == 8) return TRUE;
        return FALSE;
    }
    return FALSE;
}
}

namespace nC {
extern "C" void Item_Copy(u16 *dst, u16 *src) { *dst = *src; }
}

namespace nC {
extern "C" BOOL Item_GetIfNotCreature(u16 *a, u16 *out) {
    ItemId e1(0xfff1);
    BOOL r = FALSE;
    ItemId e2(a);
    if ((Item_IsNormalItem(&e2.v) || Item_IsFurniture(&e2.v)) && !Item_IsInsect(&e2.v) && !Item_IsFish(&e2.v)) {
        Item_Copy(&e1.v, &e2.v);
        r = TRUE;
    }
    if (out) Item_Copy(out, &e1.v);
    return r;
}
}

namespace nC {
extern "C" s32 Item_GetFishWaterClass(u16 *p) {
    if (Item_IsFish(p)) {
        switch (Fish_GetWaterKind(Item_GetFishIndex(p))) {
        case 1: return 0;
        case 3: return 1;
        case 2: return 2;
        case 0: return 3;
        }
    }
    return 4;
}
}

namespace nC {
extern "C" BOOL Item_IsFish(u16 *p) { if (Unk_0204b9c0_R(p, 0x12e8, 0x131f)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetFishIndex(u16 *p) { if (Item_IsFish(p)) return Item_GetId(p) - 0x12e8; return -1; }
}

namespace nB {
extern "C" s32 Item_GetFishBaseSize(u16 *p) {
    if (Item_IsFish(p)) return Fish_GetBaseSize(Item_GetFishIndex(p));
    return 10;
}
}

namespace nB {
extern "C" BOOL Item_IsInsect(u16 *p) { if (Unk_0204b408_R(p, 0x12b0, 0x12e7)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetInsectIndex(u16 *p) { if (Item_IsInsect(p)) return Item_GetId(p) - 0x12b0; return -1; }
}

namespace nB {
extern "C" u16 Item_GetInsectBaseSize(u16 *p) {
    if (Item_IsInsect(p)) return Insect_GetBaseSize((u8)Item_GetInsectIndex(p));
    return 0;
}
}

namespace nB {
extern "C" BOOL Item_TestInfoFlag3(u16 *p) {
    ItemIdNoInit e;
    Item_FromPlacedForm(&e.v, p);
    if (Item_GetIdClass(&e.v) == 1) return ItemInfo_TestFlag3(&e.v);
    if (Item_IsFurniture(&e.v)) return Ftr_TestIndoorFlag7(&e.v);
    return FALSE;
}
}

namespace nB {
extern "C" BOOL Item_TestInfoFlag4(u16 *p) {
    ItemIdNoInit e;
    Item_FromPlacedForm(&e.v, p);
    if (Item_GetIdClass(&e.v) == 1) return ItemInfo_TestFlag4(&e.v);
    if (Item_IsFurniture(&e.v)) return Ftr_TestIndoorFlagC(&e.v);
    return TRUE;
}
}

namespace nB {
extern "C" BOOL Item_GetShirtUnkGroup(u16 *p) {
    if (Item_IsShirt(p)) {
        ItemId e(Item_GetId(p));
        return ItemInfo_GetIndoorUnk0(&e.v);
    }
    return FALSE;
}
}

namespace nB {
extern "C" u16 Item_MakeMoneyBag(u32 x) { if (x < 0x6c) return x + 0x1492; return 0x1492; }
}

namespace nB {
extern "C" s32 Item_FindMoneyBagForAmount(s32 a, BOOL up, s32 *out) {
    ItemId e;
    u32 i;
    if (out) *out = 0;
    if (up) {
        Item_Assign(&e.v, 0x14fd);
        if (Item_GetPrice(&e.v) < a) return 0xfff1;
        for (i = 0; i < 0x6c; i++) {
            Item_Assign(&e.v, Item_MakeMoneyBag(i));
            s32 v = Item_GetPrice(&e.v);
            if (v >= a) {
                if (out) *out = v - a;
                return Item_GetId(&e.v);
            }
        }
    } else {
        Item_Assign(&e.v, 0x1492);
        if (Item_GetPrice(&e.v) > a) return 0xfff1;
        for (i = 0x6c; i != 0; i--) {
            Item_Assign(&e.v, Item_MakeMoneyBag(i - 1));
            s32 v = Item_GetPrice(&e.v);
            if (a >= v) {
                if (out) *out = a - v;
                return Item_GetId(&e.v);
            }
        }
    }
    return 0xfff1;
}
}

namespace nB {
extern "C" s32 Item_GetDesignPlayer(u16 *p) {
    s32 t = Item_GetDesignIndex(p);
    s32 r = -1;
    if (t != r) r = (t >> 3) & 3;
    return r;
}
}

namespace nB {
extern "C" BOOL Item_IsDesign(u16 *p) { if (Unk_0204b408_R(p, 0x1188, 0x11a7)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetDesignIndex(u16 *p) { if (Item_IsDesign(p)) return Item_GetId(p) - 0x1188; return -1; }
}

namespace nB {
extern "C" s32 Item_GetDesignSlot(u16 *p) {
    s32 t = Item_GetDesignIndex(p);
    s32 r = -1;
    if (t != r) t &= 7;
    else t = r;
    return t;
}
}

namespace nB {
extern "C" u16 Item_MakeDesignByIndex(u32 x) { if (x < 0x20) return x + 0x1188; return 0x1188; }
}

namespace nB {
extern "C" u16 Item_MakeDesign(u32 a, u32 b) { u32 x = (a & 3) * 8; return Item_MakeDesignByIndex(x + (b & 7)); }
}

namespace nB {
extern "C" void Item_SetDesign(u16 *out, u32 a, u32 b) { Item_SetId(out, Item_MakeDesign(a, b)); }
}

namespace nB {
extern "C" s32 Item_GetFlowerItemOrdinal(u16 *p) {
    if (Item_IsFlowerItem(p)) {
        u32 i;
        for (i = 0; i < Item_GetFlowerItemCount(); i++) {
            ItemIdNoInit e;
            Item_GetNthFlowerItem(&e.v, i);
            s32 c = Item_GetId(p);
            if (c == Item_GetId(&e.v)) return i;
        }
    }
    return -1;
}
}

namespace nB {
extern "C" s32 Item_GetFlowerAltOrdinal(u16 *p) {
    if (Item_IsFlowerAltItem(p)) {
        u32 i;
        for (i = 0; i < Item_GetFlowerAltCount(); i++) {
            ItemIdNoInit e;
            Item_GetNthFlowerAlt(&e.v, i);
            s32 c = Item_GetId(p);
            if (c == Item_GetId(&e.v)) return i;
        }
    }
    return -1;
}
}

namespace nB {
extern "C" void Item_GetNthFlowerItem(u16 *out, u32 n) {
    u32 count, i;
    if (n >= Item_GetFlowerItemCount()) n = 0;
    count = 0;
    for (i = 0; i < 0x21; i++) {
        ItemId e(Item_MakeFlowerItem(i));
        if (Item_IsFlowerItem(&e.v)) {
            if (n == count) {
                Item_CopyB(out, &e.v);
                return;
            }
            count++;
        }
    }
    Item_SetId(out, Item_MakeFlowerItem(0));
}
}

namespace nB {
extern "C" u16 Item_MakeFlowerAlt(u32 x) { if (x < 0x21) return x + 0x1471; return 0x1471; }
}

namespace nB {
extern "C" void Item_CopyB(u16 *dst, u16 *src) { *dst = *src; }
}

namespace nB {
extern "C" void Item_GetNthFlowerAlt(u16 *out, u32 n) {
    u32 count, i;
    if (n >= Item_GetFlowerAltCount()) n = 0;
    count = 0;
    for (i = 0; i < 0x21; i++) {
        ItemId e(Item_MakeFlowerAlt(i));
        if (Item_IsFlowerAltItem(&e.v)) {
            if (n == count) {
                Item_CopyB(out, &e.v);
                return;
            }
            count++;
        }
    }
    Item_SetId(out, Item_MakeFlowerAlt(0));
}
}

namespace nB {
extern "C" BOOL Item_IsFlowerItemRange(u16 *p) { if (Unk_0204b408_R(p, 0x1408, 0x1428)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetFlowerItemIndex(u16 *p) { if (Item_IsFlowerItemRange(p)) return Item_GetId(p) - 0x1408; return -1; }
}

namespace nB {
extern "C" BOOL Item_IsFlowerItem(u16 *p) {
    if (Item_IsFlowerItemRange(p) && Item_GetFlowerItemIndex(p) != 0x1e) return ItemInfo_TestFlag1(p);
    return FALSE;
}
}

namespace nB {
extern "C" BOOL Item_IsFlowerAltRange(u16 *p) { if (Unk_0204b408_R(p, 0x1471, 0x1491)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetFlowerAltIndex(u16 *p) { if (Item_IsFlowerAltRange(p)) return Item_GetId(p) - 0x1471; return -1; }
}

namespace nB {
extern "C" u16 Item_MakeFlowerItem(u32 x) { if (x < 0x21) return x + 0x1408; return 0x1408; }
}

namespace nB {
extern "C" BOOL Item_IsFlowerAltItem(u16 *p) {
    if (Item_IsFlowerAltRange(p)) {
        s32 t = Item_GetFlowerAltIndex(p);
        if (t != 0x1e) {
            ItemId e(Item_MakeFlowerItem(t));
            if (Item_IsFlowerItem(&e.v)) return FALSE;
            return TRUE;
        }
    }
    return FALSE;
}
}

namespace nB {
extern "C" s32 Item_GetPaperIndex(u16 *p) {
    if (Item_IsPaper(p)) return (Item_GetId(p) - 0x1000) >> 2;
    return -1;
}
}

namespace nB {
extern "C" s32 Item_GetPaperCount(u16 *p) { return ((Item_GetId(p) - 0x1000) & 3) + 1; }
}

namespace nB {
extern "C" u16 Item_MakePaper(u32 a, s32 b) {
    u32 base;
    if (a < 0x40) base = 0x1000 + a * 4;
    else base = 0x1000;
    return ((b - 1) & 3) + base;
}
}

namespace nB {
extern "C" BOOL Item_IsNormalItem(u16 *p) { if (Item_GetIdClass(p) == 1) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetIdClass(u16 *p) { return (*p & 0xf000) >> 12; }
}

namespace nB {
extern "C" BOOL Item_IsFurniture(u16 *p) { switch (Item_GetIdClass(p)) { case 3: case 4: return TRUE; } return FALSE; }
}

namespace nB {
extern "C" BOOL ItemList_IsFurniture(u16 *p) { return Item_IsFurniture(p); }
}

namespace nB {
extern "C" BOOL Item_IsOccupiedF031(u16 *p) { if (Item_GetId(p) == 0xf031) return TRUE; return FALSE; }
}

namespace nB {
extern "C" BOOL Item_IsFurnitureOrF031(u16 *p) { if (Item_IsOccupiedF031(p) || Item_IsFurniture(p)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" s32 Item_GetFurnitureDirection(u16 *p) { return Item_GetId(p) & 3; }
}

namespace nB {
extern "C" s32 Item_GetFurnitureIndex(u16 *p) { return (Item_GetId(p) - 0x3000) >> 2; }
}

namespace nB {
extern "C" u16 Item_MakeFurniture(s32 a, s32 b) { return 0x3000 + a * 4 + b; }
}

namespace nB {
extern "C" void Item_SetFurnitureDirection(u16 *p, s32 y) {
    if (Item_IsFurniture(p)) *p = Item_MakeFurniture(Item_GetFurnitureIndex(p), y);
}
}

namespace nB {
extern "C" BOOL Item_IsBuilding(u16 *p) { return Item_IsInRange(p, 0x5000, 0x5021); }
}

namespace nB {
extern "C" BOOL Item_IsBuildingOrOccupied(u16 *p) { if (Item_GetId(p) == 0xf030 || Item_IsBuilding(p)) return TRUE; return FALSE; }
}

namespace nB {
extern "C" u16 Item_MakeBuilding(u32 x) { return x < 0x22 ? x + 0x5000 : 0x5000; }
}

namespace nB {
extern "C" u16 Item_MakeNeighborHouse(u32 x) { return x < 0x8 ? x + 0x5001 : 0x5001; }
}

namespace nB {
extern "C" BOOL Item_IsNookShop(u16 *p) { return Item_IsInRange(p, 0x500d, 0x5010); }
}

namespace nB {
extern "C" s32 Item_GetNookShopLevel(u16 *p) { if (Item_IsNookShop(p)) return Item_GetId(p) - 0x500d; return -1; }
}

namespace nB {
extern "C" u16 Item_MakeNookShop(u32 x) { return x < 0x4 ? x + 0x500d : 0x500d; }
}

namespace nB {
extern "C" BOOL Item_IsSnowman(u16 *p) { return Item_IsInRange(p, 0xb001, 0xb003); }
}

namespace nB {
extern "C" s32 Item_GetSnowmanIndex(u16 *p) { if (Item_IsSnowman(p)) return Item_GetId(p) - 0xb001; return -1; }
}

namespace nB {
extern "C" u16 Item_MakeSnowman(u32 x) { return x < 0x3 ? x + 0xb001 : 0xb001; }
}

namespace nB {
extern "C" BOOL Item_IsPlayerHouse(u16 *p) { return Item_IsInRange(p, 0x5014, 0x501a); }
}

namespace nB {
extern "C" u16 Item_MakePlayerHouse(u32 x) { return x < 0x7 ? x + 0x5014 : 0x5014; }
}

namespace nB {
extern "C" BOOL Item_IsTreeStage0(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x26: case 0x2f: case 0x37: case 0x3f: case 0x47: case 0x4f: case 0x57: case 0x5d: case 0xc8:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreeStage1(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x27: case 0x30: case 0x38: case 0x40: case 0x48: case 0x50: case 0x58: case 0x5e: case 0xc9:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreeStage2(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x28: case 0x31: case 0x39: case 0x41: case 0x49: case 0x51: case 0x59: case 0x5f: case 0xca:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreeStage3(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x29: case 0x32: case 0x3a: case 0x42: case 0x4a: case 0x52: case 0x5a: case 0x60: case 0xcb:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreeGrown(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x2a: case 0x33: case 0x3b: case 0x43: case 0x4b:
    case 0x53: case 0x5b: case 0x61:
    case 0x66: case 0x67: case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d:
    case 0xcc:
        r = TRUE;
        break;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsFruitTreeLastNoFruit(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x36: case 0x3e: case 0x46: case 0x4e: case 0x56: case 0xcf:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreePlain(u16 *p) { return Item_IsInRange(p, 0x26, 0x2a); }
}

namespace nA {
extern "C" BOOL Item_IsCedarPlain(u16 *p) { return Item_IsInRange(p, 0x5d, 0x61); }
}

namespace nA {
extern "C" BOOL Item_IsFruitTree(u16 *p) { return Item_IsInRange(p, 0x2f, 0x56); }
}

namespace nA {
extern "C" BOOL Item_IsMoneyTree(u16 *p) { return Item_IsInRange(p, 0x57, 0x5b); }
}

namespace nA {
extern "C" BOOL Item_IsSpecialTree(u16 *p) { return Item_IsInRange(p, 0x66, 0x68); }
}

namespace nA {
extern "C" BOOL Item_IsAcornTree(u16 *p) { if (*p == 0x69) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsSpecialCedar(u16 *p) { return Item_IsInRange(p, 0x6a, 0x6c); }
}

namespace nA {
extern "C" BOOL Item_IsFestiveCedar(u16 *p) { if (*p == 0x6d) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsPalmTree(u16 *p) { return Item_IsInRange(p, 0xc8, 0xcf); }
}

namespace nA {
extern "C" BOOL Item_IsTree(u16 *p) {
    if (Item_IsTreePlain(p) || Item_IsCedarPlain(p) || Item_IsFruitTree(p) || Item_IsMoneyTree(p) || Item_IsSpecialTree(p) || Item_IsAcornTree(p) || Item_IsSpecialCedar(p) || Item_IsFestiveCedar(p) || Item_IsPalmTree(p)) return TRUE;
    return FALSE;
}
}

namespace nA {
extern "C" s32 Item_GetTreeStage(u16 *p) {
    s32 r = -1;
    if (Item_IsTree(p)) {
        if (Item_IsTreeStage0(p)) r = 0;
        else if (Item_IsTreeStage1(p)) r = 1;
        else if (Item_IsTreeStage2(p)) r = 2;
        else if (Item_IsTreeStage3(p)) r = 3;
        else r = 4;
    }
    return r;
}
}

namespace nA {
extern "C" void Item_Assign(u16 *p, u32 v) { *p = v; }
}

namespace nA {
extern "C" BOOL Item_IsAppleTree(u16 *p) { return Item_IsInRange(p, 0x37, 0x3e); }
}

namespace nA {
extern "C" BOOL Item_IsOrangeTree(u16 *p) { return Item_IsInRange(p, 0x3f, 0x46); }
}

namespace nA {
extern "C" BOOL Item_IsPearTree(u16 *p) { return Item_IsInRange(p, 0x47, 0x4e); }
}

namespace nA {
extern "C" BOOL Item_IsPeachTree(u16 *p) { return Item_IsInRange(p, 0x2f, 0x36); }
}

namespace nA {
extern "C" BOOL Item_IsCherryTree(u16 *p) { return Item_IsInRange(p, 0x4f, 0x56); }
}

namespace nA {
extern "C" s32 Item_GetFruitTreeFruit(u16 *p) {
    s32 r = 0;
    if (Item_IsAppleTree(p)) {
    } else if (Item_IsOrangeTree(p)) r = 1;
    else if (Item_IsPearTree(p)) r = 2;
    else if (Item_IsPeachTree(p)) r = 3;
    else if (Item_IsCherryTree(p)) r = 4;
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsSmallStump(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x2b: case 0x62: case 0xd0: case 0xff:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsMediumStump(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x2c: case 0x63: case 0xd1: case 0x100:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsLargeStump(u16 *p) {
    BOOL r = FALSE;
    switch (Item_GetId(p)) {
    case 0x2d: case 0x64: case 0xd2: case 0x101:
        r = TRUE;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsTreeStump(u16 *p) { return Item_IsInRange(p, 0x2b, 0x2e); }
}

namespace nA {
extern "C" BOOL Item_IsSpecialStump(u16 *p) { return Item_IsInRange(p, 0xff, 0x102); }
}

namespace nA {
extern "C" BOOL Item_IsTreeOrSpecialStump(u16 *p) { if (Item_IsTreeStump(p) || Item_IsSpecialStump(p)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsCedarStump(u16 *p) { return Item_IsInRange(p, 0x62, 0x65); }
}

namespace nA {
extern "C" BOOL Item_IsPalmStump(u16 *p) { return Item_IsInRange(p, 0xd0, 0xd3); }
}

namespace nA {
extern "C" BOOL Item_IsStump(u16 *p) { if (Item_IsTreeOrSpecialStump(p) || Item_IsCedarStump(p) || Item_IsPalmStump(p)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" s32 Item_GetStumpSize(u16 *p) {
    s32 r = -1;
    if (Item_IsStump(p)) {
        if (Item_IsSmallStump(p)) r = 1;
        else if (Item_IsMediumStump(p)) r = 2;
        else if (Item_IsLargeStump(p)) r = 3;
        else r = 4;
    }
    return r;
}
}

namespace nA {
extern "C" BOOL Item_IsParchedTulip(u16 *p) { return Item_IsInRange(p, 0x6e, 0x73); }
}

namespace nA {
extern "C" BOOL Item_IsParchedPansy(u16 *p) { return Item_IsInRange(p, 0x74, 0x79); }
}

namespace nA {
extern "C" BOOL Item_IsParchedCosmos(u16 *p) { return Item_IsInRange(p, 0x7a, 0x7f); }
}

namespace nA {
extern "C" BOOL Item_IsParchedRose(u16 *p) { return Item_IsInRange(p, 0x80, 0x87); }
}

namespace nA {
extern "C" BOOL Item_IsFlowerParched(u16 *p) { if (Item_IsParchedTulip(p) || Item_IsParchedPansy(p) || Item_IsParchedCosmos(p) || Item_IsParchedRose(p)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsTulip(u16 *p) { return Item_IsInRange(p, 0x0, 0x5); }
}

namespace nA {
extern "C" BOOL Item_IsPansy(u16 *p) { return Item_IsInRange(p, 0x6, 0xb); }
}

namespace nA {
extern "C" BOOL Item_IsCosmos(u16 *p) { return Item_IsInRange(p, 0xc, 0x11); }
}

namespace nA {
extern "C" BOOL Item_IsRose(u16 *p) { if (Item_IsInRange(p, 0x12, 0x19) || *p == 0x1c) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsFlowerFresh(u16 *p) { if (Item_IsTulip(p) || Item_IsPansy(p) || Item_IsCosmos(p) || Item_IsRose(p)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsWateredTulip(u16 *p) { return Item_IsInRange(p, 0x8a, 0x8f); }
}

namespace nA {
extern "C" BOOL Item_IsWateredPansy(u16 *p) { return Item_IsInRange(p, 0x90, 0x95); }
}

namespace nA {
extern "C" BOOL Item_IsWateredCosmos(u16 *p) { return Item_IsInRange(p, 0x96, 0x9b); }
}

namespace nA {
extern "C" BOOL Item_IsInRange(u16 *p, u32 lo, u32 hi) { BOOL r = FALSE; if (*p >= lo && *p <= hi) r = TRUE; return r; }
}

namespace nA {
extern "C" BOOL Item_IsWateredRose(u16 *p) { if (Item_IsInRange(p, 0x9c, 0xa3) || *p == 0xa5) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL Item_IsFlowerWatered(u16 *p) { if (Item_IsWateredTulip(p) || Item_IsWateredPansy(p) || Item_IsWateredCosmos(p) || Item_IsWateredRose(p)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" s32 Item_GetId(u16 *p) { return *p; }
}

namespace nA {
extern "C" BOOL Item_IsFlower(u16 *p) {
    BOOL r = FALSE;
    if (Item_IsFlowerParched(p) || Item_IsFlowerFresh(p) || Item_IsFlowerWatered(p)) {
        r = TRUE;
    } else {
        switch (Item_GetId(p)) {
        case 0x1a: case 0x1d: case 0x1e: case 0x88: case 0xa4:
            r = TRUE;
        }
    }
    return r;
}
}

namespace nA {
extern "C" void Item_SetId(u16 *p, u32 v) { *p = v; }
}

namespace nA {
extern "C" u16 Item_MakeInsect(u32 x) { if (x < 0x38) return x + 0x12b0; return 0x12b0; }
}

namespace nA {
extern "C" u16 Item_MakeHat(u32 x) { if (x < 0x40) return x + 0x13c8; return 0x13c8; }
}

namespace nA {
extern "C" u16 Item_MakeCarpet(u32 x) { if (x < 0x44) return x + 0x1144; return 0x1144; }
}

namespace nA {
extern "C" u16 Item_MakeShirt(u32 x) { if (x < 0x100) return x + 0x11a8; return 0x11a8; }
}

namespace nA {
extern "C" u16 Item_MakeAccessory(u32 x) { if (x < 0x40) return x + 0x1431; return 0x1431; }
}

namespace nA {
extern "C" BOOL Item_IsHat(u16 *p) { if (Unk_0204a7d0_R(p, 0x13c8, 0x1407)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsHat(u16 *p) { return Item_IsHat(p); }
}

namespace nA {
extern "C" BOOL Item_IsCarpet(u16 *p) { if (Unk_0204a7d0_R(p, 0x1144, 0x1187)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsCarpet(u16 *p) { return Item_IsCarpet(p); }
}

namespace nA {
extern "C" BOOL Item_IsShirt(u16 *p) { if (Unk_0204a7d0_R(p, 0x11a8, 0x12a7)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsShirt(u16 *p) { return Item_IsShirt(p); }
}

namespace nA {
extern "C" BOOL Item_IsAccessory(u16 *p) { if (Unk_0204a7d0_R(p, 0x1431, 0x1470)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsAccessory(u16 *p) { return Item_IsAccessory(p); }
}

namespace nA {
extern "C" BOOL Item_IsPaper(u16 *p) { if (Unk_0204a7d0_R(p, 0x1000, 0x10ff)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsPaper(u16 *p) { return Item_IsPaper(p); }
}

namespace nA {
extern "C" BOOL Item_IsUmbrella(u16 *p) { if (Unk_0204a7d0_R(p, 0x1380, 0x139f)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsUmbrella(u16 *p) { return Item_IsUmbrella(p); }
}

namespace nA {
extern "C" BOOL Item_IsWallpaper(u16 *p) { if (Unk_0204a7d0_R(p, 0x1100, 0x1143)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsWallpaper(u16 *p) { return Item_IsWallpaper(p); }
}

namespace nA {
extern "C" BOOL Item_IsFullHeadwear(u16 *p) { if (Unk_0204a7d0_R(p, 0x13a8, 0x13c7)) return TRUE; return FALSE; }
}

namespace nA {
extern "C" BOOL ItemList_IsFullHeadwear(u16 *p) { return Item_IsFullHeadwear(p); }
}

namespace nA {
extern "C" u16 Item_MakeUmbrella(u32 x) { if (x < 0x20) return x + 0x1380; return 0x1380; }
}

namespace nA {
extern "C" u16 Item_MakeWallpaper(u32 x) { if (x < 0x44) return x + 0x1100; return 0x1100; }
}

namespace nA {
extern "C" u16 Item_MakeFullHeadwear(u32 x) { if (x < 0x20) return x + 0x13a8; return 0x13a8; }
}

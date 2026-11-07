#include "ds_div.h"
#include "host_labels.h"
#include "uptr.h"
#include "types.h"
#include "n3ds_options.h"
#include "sys/StackPad.h"
#include "sys/ClockDateTime.h"
#include "gfx/VecFx32.h"
#include "net/CommManager.h"
#include "gfx/EffectSplEmitter.h"
#include "actor/Actor.h"
#include "town/TownBlockMap.h"
#include "town/BlockMap.h"
#include "field/ItemDropUnit.h"
#include "item/MotherLetterDate.h"
#include "save/SaveData.h"
extern SaveData gSaveData;
extern "C" u16 *PendingUnit_GetActivePosOfAid(u32 a);
#include "proto/main/unk_0206269c.h"
#include "proto/main/unk_02039b6c.h"
#include "proto/main/unk_02039c08.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02095cdc.h"
#include "proto/ov004/unk_ov004_0222bed0.h"
#include "town/TownMap.h"
#include "game/RoostGuestRoll.h"
#include "game/TurnipMarket.h"
#include "npc/VillagerPlan.h"
#include "npc/VillagerData.h"
#include "town/TownUpdateThread.h"
#include "game/Unk_02042104_Date.h"
#include "game/EventDayEntry.h"
#include "talk/MsgString9BStorage.h"
#include "town/TownEventRecord.h"
#include "field/FieldAction.h"
#include "player/FieldActionFxEvent.h"
#include "game/Unk_0204da0c_Size.h"
#include "game/Unk_0204da0c_Map.h"
#include "item/ItemPickSpec.h"
#include "npc/VillagerDataItemView.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "room/HouseData.h"
#include "save/BlancaFaceRecord.h"
#include "save/ContestRecord.h"
#include "save/MuseumData.h"
#include "save/SaveRecord4.h"
#include "talk/MsgString9B.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02113088.h"
#include "proto/main/unk_02002b1c.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_020407fc.h"
#include "proto/main/unk_02041e00.h"
#include "room/RoomScoreEvaluator.h"
#include "field/MoneyRockState.h"
#include "town/RoomBlockMap.h"
#include "proto/main/unk_02051218.h"
#include "proto/main/unk_0205ef08.h"
#include "proto/main/unk_02060b9c.h"
#include "proto/main/unk_0206269c.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_0208f268.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02095cdc.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02097d1c.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_020a427c.h"
#include "proto/main/unk_020b0e60.h"
#include "proto/main/unk_020c00c0.h"
#include "proto/ov003/unk_ov003_022192b4.h"
#include "proto/ov004/unk_ov004_0222bed0.h"
#include "proto/main/unk_020594dc.h"


struct Unk_0204a768_Pos : Vec2 {
    Unk_0204a768_Pos() { x = 0; y = 0; }
};

// member at offset 0x18 of gTownEval (data_021c40e4)
struct TownEvalAdvice {
    s32 acreX;
    s32 acreZ;
    s32 reason;
    TownEvalAdvice();
    ~TownEvalAdvice();
};

// TownEvalAcre::flags bits (TownEval_EvaluateAcre sets them by mask, the town summary reads them as bitfields).
struct TownEvalAcreFlags {
    s32 rafflesia : 1;       // 0x0001 item 0x1b
    s32 jacobsLadder : 1;    // 0x0002 item 0x1a
    s32 hasFlowers : 1;      // 0x0004 a flower other than dandelions / gold roses
    s32 beeTree : 1;         // 0x0008 items 0x67, 0x6b
    s32 furnitureTree : 1;   // 0x0010 items 0x66, 0x6a
    s32 bellTree : 1;        // 0x0020 items 0x68, 0x6c
    s32 moneyRocks : 4;      // 0x03c0 one bit per player (money rocks 0xe8 + 5 * player)
    s32 coconut : 1;         // 0x0400 item 0x1548
    s32 unk_b11 : 2;         // 0x0800 trees 0x26..0x2a / 0x66..0x68, 0x1000 grown cedars
    s32 pitfallSeed : 1;     // 0x2000 buried item 0x1566
    s32 spoiledTurnips : 1;  // 0x4000 items 0x154a..0x1553
};

// TownEval::townFlags bits (union of the acre bits, TownEval_EvaluateAndClean).
struct TownEvalFlags {
    s32 moneyRocks : 4;
    s32 coconut : 1;
    s32 pitfallSeed : 1;
    s32 spoiledTurnips : 1;
    s32 hasGarbage : 1;
};

// One acre's environment rating (TownEval_EvaluateAcre / EvaluateAndCleanAcre, scored by TownEval_CalcScore).
struct TownEvalAcre {
    /* 0x00 */ s32 score;
    /* 0x04 */ u8 trees[2][2];               // grown trees per 8x8 quadrant
    /* 0x08 */ u16 weeds;
    /* 0x0a */ u16 groundItems;
    /* 0x0c */ u16 garbage;
    /* 0x0e */ u16 flowers;
    /* 0x10 */ Unk_0204a768_Pos rafflesiaPos;  // unit in the acre, -1 = none
    /* 0x18 */ u16 rocks;
    /* 0x1a */ u16 litCedars;
    /* 0x1c */ u8 shells;
    /* 0x1d */ u8 buriedFossils;
    /* 0x20 */ union {
        u32 flags;
        TownEvalAcreFlags bits;
    };
};

// gTownEval (0x284 bytes; data_021c40d0, data_021c40e4, data_021c40ec, data_021c4110 are its offsets 4, 0x18,
// 0x20, 0x44). Its destructor is implicit (it destroys advice).
struct TownEval {
    /* 0x00 */ u32 rank;
    /* 0x04 */ s32 counts[5];             // acres per score bin
    /* 0x18 */ TownEvalAdvice advice;
    /* 0x24 */ u8 jacobsAcres;
    /* 0x25 */ u8 flowerAcres;
    /* 0x26 */ u16 flowers;
    /* 0x28 */ u16 weeds;
    /* 0x2a */ u16 trees;
    /* 0x2c */ u8 furnitureTreeAcres;
    /* 0x2d */ u8 buriedFossils;
    /* 0x2e */ u8 rocks;
    /* 0x2f */ u8 pad_2f;
    /* 0x30 */ union {
        u32 townFlags;
        TownEvalFlags townBits;
    };
    /* 0x34 */ Unk_0204a768_Pos rafflesiaBlock;
    /* 0x3c */ Unk_0204a768_Pos rafflesiaUnit;
    /* 0x44 */ TownEvalAcre acres[4][4];
};

// One requested tool/item action on a map unit (element of sFieldActions, 0x24 bytes). FieldAction_Add fills it and
// FieldAction_Submit sends it (record 0x31) or applies it offline; state 0 free, 1 sent, 2 accepted, 3 rejected.
struct FieldActionRequest {
    /* 0x00 */ u32 aid;
    /* 0x04 */ u32 area;
    /* 0x08 */ s32 state;
    /* 0x0c */ s32 kind;
    /* 0x10 */ u16 item;
    /* 0x12 */ u16 oldItem;
    /* 0x14 */ Unk_0204a768_Pos unit;
    /* 0x1c */ u8 layer;
    /* 0x1d */ u8 unk_1d;
    /* 0x1e */ s8 unk_1e;
    /* 0x1f */ u8 pad_1f;
    /* 0x20 */ s32 mode;
};

// sFieldActions (0x46c bytes; data_021c43bc is its offset 0x6c)
struct FieldActionState {
    FieldActionRequest requests[3];
    u8 chopCounts[0x400];
};

extern "C" void LitCedarPos_Construct(u32 *p);
// element of sLitCedars (24 x 8 bytes); the constructor is LitCedarPos_Construct of another unit
struct LitCedarPos {
    u32 unk_00[2];
    LitCedarPos() { LitCedarPos_Construct(unk_00); }
};

// sPendingUnits (0x144 bytes; data_021c3f8c is its offset 4: 20 entries of 0x10 bytes)
struct PendingUnitList {
    u32 enabled;
    PendingUnit entries[20];
};

namespace nZ {
extern "C" {
}
}


// types the merged units share
struct FlowerHybridEntry {
    s32 weight;
    u16 item;
};
struct FlowerHybridList {
    s32 numEntries;
    const FlowerHybridEntry *entries;
};
struct Unk_02042588_Pos {
    s32 x, y;
    Unk_02042588_Pos(s32 a, s32 b) : x(a), y(b) {}
    Unk_02042588_Pos(const Unk_02042588_Pos &o) : x(o.x), y(o.y) {}
};
struct Unk_020434f0_P {
    s32 a;
    s32 b;
    Unk_020434f0_P() {}
    Unk_020434f0_P(const Unk_020434f0_P &o) : a(o.a), b(o.b) {}
};
struct Unk_02045f6c_Rgb {
    u8 a, b, c;
};
struct Unk_02048cc4_Pos {
    s32 x, y;
    Unk_02048cc4_Pos(s32 a, s32 b) : x(a), y(b) {}
    Unk_02048cc4_Pos(const Unk_02048cc4_Pos &o) : x(o.x), y(o.y) {}
};
struct TownEvalAcrePos {
    s32 acreX;
    s32 acreZ;
};

// The functions and variables the merged units share, with their real types (definitions below or in other units)
extern "C" {
void Area_PlaceItem(u32 a, s32 x, s32 y, u32 v, s32 flag);
u16 *BlockMap_GetItemPtr(BlockMap *g, s32 hx, s32 hy, s32 lx, s32 ly, u8 layer);
void BlockMap_PlaceItem(BlockMap *a, s32 x, s32 y, u32 v, s32 flag);
void BlockMap_PlaceItemAt(TownUpdater *a0, BlockMap *a1, s32 a2, s32 a3, s32 a4, s32 a5, u16 a6, s32 a7);
BOOL BlockMap_SetItemAtUnit(BlockMap *g, u16 *a, s32 x, s32 z, u8 d);
void ChopCount_ClearAll(void);
s32 ChopCount_Get(FieldActionState *m, Vec2 *p);
void ChopCount_Set(FieldActionState *m, Vec2 *p, u32 v);
void Clock_GetDateTime(ClockDateTime *p);
void Clock_GetMinuteHour(u8 *out);
void DateTime_AddDays(ClockDateTime *p, s32 n);
s32 DateTime_DiffDays(ClockDateTime *a, ClockDateTime *b);
void DateTime_SubDays(ClockDateTime *p, s32 n);
void DateTime_SubHours(ClockDateTime *p, s32 n);
s32 Date_GetWeekday(u32 a, u32 b, u32 c);
s32 Event_GetState(s32 a, void *b, s32 c);
u8 FengShui_GetWestTotal();
void FieldActionFx_ClearAll(FieldActionFxEvent *p);
s32 FieldAction_Add(FieldActionRequest *e, u32 a1, s32 type, Vec2 *pos, u16 a5, u16 a6, u8 a7, u8 a8, s32 a9, s8 a10);
void FieldAction_HostProcess(FieldActionRequestMsg *src, u8 flag, s32 t);
s32 FieldAction_RequestChop(s32 a, u16 *id, Vec2 *pos, s32 mode);
s32 FieldAction_RequestDig(s32 a, BlockMap *b, u16 *id, Vec2 *pos, u16 e, s32 f);
s32 FieldAction_RequestPlaceAtPending(s32 a, u16 b);
s32 FieldAction_RequestShake(s32 a, u16 *id, Vec2 *pos, s32 mode);
s32 FieldAction_RequestWater(s32 a, u16 *id, Vec2 *pos, s32 d);
void FieldAction_WaterFlowers(u32 self, Vec2 *p);
void FieldActions_Init(FieldActionState *p);
void FieldActions_Update(FieldActionState *p);
s32 FieldItemFx_StartDrop(s32 a, u32 b, Unk_02042588_Pos p, VecFx32Copy v);
void FieldItemFx_StartPlant(s32 a, s32 b, Unk_02042588_Pos c, VecFx32Copy d);
s32 FieldItemFx_StartPop(s32 a, s32 b, Unk_02042588_Pos c);
u32 FieldPlayer_GetHeldItem(u32 slot);
void FieldPos_FromUnitCenter(VecFx32 *v, s32 x, s32 z);
void FieldPos_ToUnit(s32 *ox, s32 *oz, VecFx32 *v);
void FieldUnit_FromBlockUnit(s32 *ox, s32 *oz, s32 a, s32 b, s32 c, s32 d);
s32 Field_AidOrLocal(s32 a);
s32 Field_AidOrZero(s32 a);
s32 Field_IsLocalAid(s32 a);
BOOL Field_IsUnitClearOfOthers(Vec2 *p, s32 idx);
u16 Flower_GetWateredForm(TownUpdater *a, u16 *q, s32 flag);
BOOL Item_IsTreeGrown(u16 *p);
BOOL Item_IsTreeStage0(u16 *p);
u16 Item_RandomSeashell(TownUpdater *a);
BOOL Item_ToPlantedFieldId(u16 *out, u16 *out2, u16 c);
void MI_CpuCopy8(const void *src, void *dst, u32 n);
void MI_CpuFill8(void *dst, u32 v, u32 n);
void MoneyRock_Init(MoneyRockState *self);
void MoneyRock_OnHit(MoneyRockState *self, Vec2 *p);
void MoneyRock_Reset(MoneyRockState *self);
void PendingUnit_ApplyAt(Vec2 *pos, s32 flag);
void PendingUnit_CancelAt(Vec2 *pos, s32 flag);
void PendingUnit_Clear(PendingUnit *p);
s32 PendingUnit_Find(Vec2 *pos, s32 flag);
PendingUnit *PendingUnit_Get(u32 i);
s32 PendingUnit_IndexAt(Vec2 *pos, s32 flag);
void PendingUnit_Replace(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
BOOL PendingUnit_Reserve(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
void PendingUnit_Reset(PendingUnit *e);
void PendingUnits_ClearAll(PendingUnitList *a);
void PendingUnits_Enable();
void RoomScoreEvaluator_Construct(RoomScoreEvaluator *p);
void RoomScoreEvaluator_Destruct();
BOOL SceneId_IsTown(u32 a);
BOOL SceneId_IsTownUnk31(u32 a);
TownBlockMap *TownBlockMap_Get();
s32 TownEval_Evaluate(TownEval *a, TownBlockMap *b);
void TownEval_EvaluateAcre(TownEvalAcre *out, BlockMap *map, s32 x, s32 z);
void TownEval_EvaluateAndClean(TownEval *a, TownBlockMap *b, s32 w, s32 h);
void TownEval_EvaluateAndCleanAcre(TownEvalAcre *out, BlockMap *map, s32 x, s32 z);
s32 TownEval_GetScoreRank(TownEval *a, s32 v);
void TownEval_PickAdvice(TownEvalAdvice *out, TownEvalAcrePos *in, TownEvalAcre *res);
void Town_AdvanceDays(TownUpdater *a, ClockDateTime *b, ClockDateTime *c, s32 n, u8 e, s32 f);
void Town_BreedFlowers(TownUpdater *a, TownBlockMap *b, s32 c, s32 d);
void Town_BuryFossils(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_BuryGyroids(TownUpdater *a, BlockMap *b, s32 c, s32 d, u8 e);
void Town_BuryPitfallSeed(TownUpdater *a, BlockMap *b, s32 c, s32 d);
BOOL Town_CanBuryAt(BlockMap *p, s32 x, s32 y);
BOOL Town_CanSpawnCloverAt(BlockMap *p, s32 x, s32 y);
BOOL Town_CanSpawnWeedAt(BlockMap *p, s32 x, s32 y);
void Town_ClearObjectsFcFd(TownUpdater *a);
BOOL Town_ForEachNeighbor(BlockMap *m, Unk_02048cc4_Pos size, Unk_02048cc4_Pos pos, BOOL (*cb)(BlockMap *, Unk_02048cc4_Pos, Unk_02048cc4_Pos, Unk_02048cc4_Pos));
BOOL Town_IsSaplingBlockerAt(BlockMap *a, Unk_02048cc4_Pos pos, Unk_02048cc4_Pos centre, Unk_02048cc4_Pos size);
s32 Town_PlaceAtRandomUnit(TownUpdater *a, BlockMap *b, s32 n, Vec2 *arr, u16 e, s32 g);
void Town_SpawnBeeTrees(TownUpdater *a, BlockMap *b, s32 w, s32 h);
void Town_SpawnBellTrees(TownUpdater *a, BlockMap *b, s32 w, s32 h);
void Town_SpawnFurnitureTrees(TownUpdater *a, BlockMap *b, s32 c, s32 d);
BOOL Town_SpawnItemInAcre(TownUpdater *a, BlockMap *b, s32 c, s32 d, u16 e, s32 (*f)(BlockMap *, s32, s32), s32 g);
void Town_SpawnSpecialTrees(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_ThinSaplings(TownUpdater *a, BlockMap *m, Unk_02048cc4_Pos size);
BOOL Town_TryBreedFlowers(BlockMap *m, Unk_02048cc4_Pos p, Unk_02048cc4_Pos q, Unk_02048cc4_Pos r);
void Town_UpdateFlowers(TownUpdater *a, BlockMap *m);
void Town_UpdateFlowersExtraDay(TownUpdater *a, BlockMap *m, s32 c);
void Town_UpdateRedTurnips(TownUpdater *a, TownBlockMap *b, s32 c);
void Town_UpdateSaplings(TownUpdater *a, BlockMap *m, Unk_02048cc4_Pos size);
void Town_UpdateSeashells(TownUpdater *a);
void Town_UpdateSeashellsOffline(TownUpdater *a);
void Town_UpdateSeashellsOnline(TownUpdater *a);
void Town_UpdateSpecialRocks(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_WitherCrowdedSaplings(TownUpdater *a, BlockMap *m, Unk_02048cc4_Pos size);
void Town_WitherSapling(BlockMap *a, u32 v, s32 x, s32 y);
s32 Town_WitherSaplingAt(BlockMap *a, Unk_02048cc4_Pos pos, Unk_02048cc4_Pos centre, Unk_02048cc4_Pos size);
BOOL TreeDrop_Spawn(FieldActionResultMsg *o, FieldActionRequestMsg *s, u32 x);
extern const u8 data_020c910c[4];
extern const u16 data_020c912c[4];
extern const u8 data_020c96b8[20];
extern const u16 data_020c9754[12];
extern const s32 data_020c97cc[3][2];
extern const u8 data_020c9850[48];
extern const u16 *data_020da2ac[7];
extern const u16 *data_020da2c8[7];
extern const u16 *data_020da2e4[7];
extern MoneyRockState data_021c47bc;
#define gSaveRecycleBin HOST_LABEL(u8 (*)[], gSaveRecycleBin)
#define gSaveTownState HOST_LABEL(TownState *, gSaveTownState)
#define gSaveVillagers HOST_LABEL(u8 (*)[], gSaveVillagers)
extern BlockMap *gSceneBlockMap;
extern TownEval gTownEval;
extern TownUpdater gTownUpdater;
extern const u32 sBbsEventMsgs[30];
extern const u8 sDropUnitOffsets[12];
extern const u32 sDropUnitOrder[8][9];
extern FieldActionFxEvent sFieldActionFxSlots[5];
extern FieldActionState sFieldActions;
extern const FlowerHybridList *sFlowerHybridTable[4][10];
extern const u32 sFlowerParchChance[27];
extern const u32 sFlowerParchChanceExtraDay[27];
extern const u16 sFruitSaplings[6];
extern const u16 sFruitSaplings2[6];
extern LitCedarPos sLitCedars[24];
extern const u8 sMoneyRockBags[20];
extern const u8 sNeighborOffsets8[8];
extern PendingUnitList sPendingUnits;
extern const u16 sSaplingFruitItems[6];
extern const s32 sSeashellWeights[9];
extern const s32 sTreeChopHits[4];
extern const u16 sTreeDropFruit[6];
extern const u8 sTreeDropOffsets[12];
extern const u32 sTreeDropSearchOrder[3][8];
extern const u32 sTreeDropSlots[3];
extern const s8 sWaterOffsetsX[8];
extern const s8 sWaterOffsetsZ[8];
}

// ---- unk_0204198c.cpp
namespace nA {
extern "C" {

s32 TownUpdateThread_Destroy();
s32 TownUpdateThread_StartCtx(TownUpdateThread *p);
void TownUpdateThread_SetArgs(TownUpdateThread *p, u8 *a, u8 *b, u32 c, u8 d);
void TownUpdateThread_Init(TownUpdateThread *p);
void OS_CreateThread(void *th, void *fn, void *arg, void *stack, u32 size, u32 prio);
void OS_WakeupThreadDirect(void *th);
u32 DC_FlushAll();
void TownUpdateThread_Main(u8 *arg);
#define data_021ed20c HOST_LABEL(u8 (*)[], data_021ed20c)
extern u8 sTownBbsUpdateCtx[];
s32 TownBbs_GetDaysToPost(void *o, u8 *a, Unk_02042104_Date *d);
void TownBbs_CatchUpPelicanDate(void *o, u8 *base, Unk_02042104_Date *d);
void TownBbs_PostDays(void *o, u8 *base, s32 cnt, Unk_02042104_Date *d, s32 flag);
void TownBbs_PostEventsForDay(void *o, u8 *base, Unk_02042104_Date *d);
s32 Game_IsIntroPeriod();
void TownState_PickNextWeekDate(u8 *dst, u8 *src);
void TownBbs_PostPelicanNotice(void *o, SaveData *save, Unk_02042104_Date *d);
void TownBbs_PostSlogan(void *o, SaveData *save, Unk_02042104_Date *d);
s32 EventSchedule_CollectDayAll(EventDayEntry *z, Unk_02042104_Date *d);
void TownBbs_PostDayEvents(void *o, EventDayEntry *z, Unk_02042104_Date *d);
void *SaveVillagers_Get(void *p, s32 i);
void Villager_GetSpeciesName(MsgString9BStorage *o, u32 v);
s32 Event_GetStateAt(u32 ty, Unk_02042104_Date *d, s32 v);
s32 TownBbs_UnpackUsedMask(void *o, u8 *dst, u8 *src, s32 n);
void TownBbs_PostRandomUnused(void *o, s32 n, const char *fmt, u8 *p, s32 len, Unk_02042104_Date *d);
void TownBbs_PackUsedMask(void *o, u8 *dst, u8 *src, s32 n);
void TownBbs_PostDayEvents(void *o, EventDayEntry *z, Unk_02042104_Date *d);
void TownBbs_PostPelicanNotice(void *o, SaveData *save, Unk_02042104_Date *d);
void TownBbs_PostSlogan(void *o, SaveData *save, Unk_02042104_Date *d);
void TownBbs_PostRandomUnused(void *o, s32 n, const char *fmt, u8 *p, s32 len, Unk_02042104_Date *d);
void TownBbs_PackUsedMask(void *o, u8 *dst, u8 *src, s32 n);
s32 TownBbs_UnpackUsedMask(void *o, u8 *dst, u8 *src, s32 n);
s32 FieldActionFx_Take(FieldActionFxEvent *out, u32 v);
struct Unk_02042104_Pair {
    s32 hi, lo;
};
extern u8 gFieldSceneKind;
inline BOOL Unk_02042104_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}
void FieldActionFx_Start(FieldActionResultMsg *p);
}
}

// ---- unk_020422a8.cpp
namespace nB {
extern "C" {

static inline BOOL Unk_02042660_InRange(u32 c, u32 lo, u32 hi) { BOOL r = FALSE; if (c >= lo && c <= hi) r = TRUE; return r; }
inline BOOL Unk_02042830_IsZero(u8 v) { return v == 0 ? TRUE : FALSE; }
extern u8 gFieldSceneKind;
extern CommManager *gCommManager;
void MoneyRock_Update(MoneyRockState *);
s32 _s32_div_f(s32, s32);
void Scene_SetUnitItem(s32 x, s32 y, u32 item, s32 flag);
s32 Scene_GetCurrent(void);
void FieldAction_Submit(u8, u32);
void Town_ClearObjectFcFdAtPos(TownUpdater *, VecFx32 *);
s32 Scene_InTown(void);
s32 Ground_IsNeighbourReachable(s32, s32, s32, s32, s32);
u16 *FieldAction_CheckFreeUnit(u32, BlockMap *, s32, s32, Vec2 *);
s32 VillagerTrend_NotifyUnk5(void *);
void FieldAction_Clear(FieldActionRequest *, s32);
BOOL MoneyRock_TrySpawnBagAt(MoneyRockState *self, BlockMap *m, Vec2 *p);
void MoneyRock_SpawnBag(MoneyRockState *self, Vec2 *p);
void MoneyRock_Update(MoneyRockState *self);
void FieldAction_Clear(FieldActionRequest *e, s32 i);
FieldActionRequest *FieldAction_Get(s32 i);
void Field_ClearObjectFcFdAt(VecFx32 *p);
void FieldAction_Release(s32 i);
s32 FieldAction_PollDrop(s32 idx);
u16 *FieldAction_CheckFreeUnit(u32 a, BlockMap *m, s32 x, s32 y, Vec2 *p);
s32 FieldAction_FindDropUnit(u32 kind, Vec2 *p, u8 *out);
s32 FieldAction_RequestPlaceAtPendingForAid(s32 a, u16 b);
}
}

// ---- unk_02042bbc.cpp
namespace nC {
extern "C" {

typedef Unk_02042588_Pos Pos;
extern CommManager *gCommManager;
static inline BOOL Unk_0204301c_Rng(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}
s32 FieldAction_FindDropUnit(u32 kind, Vec2 *p, u8 *out);
s32 Item_GetFruitTreeFruit(u16 *p);
s32 Scene_InTown();
s32 BlockMap_IsBuriedAtUnit(void *o, s32 x, s32 y);
s32 FieldAction_RequestTool(s32 a, Vec2 *p, s32 sel, s32 c, u16 e);
s32 FieldAction_RequestPickUp(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestPickUpOutdoor(s32 a, Vec2 *p);
s32 FieldAction_RequestPickUpIndoor(s32 a, Vec2 *p);
s32 FieldAction_RequestFillHole(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestDrop(s32 a, u16 b);
s32 FieldAction_RequestAtFreeUnit(s32 a, s32 b, u16 c);
s32 FieldAction_RequestDropForAid(s32 a, u16 b);
s32 FieldAction_RequestDropOrPlace(s32 a, u16 b);
s32 FieldAction_RequestDrop(s32 a, u16 b);
struct DropRequestArgs {
    u8 layer;
    u8 pad;
    u16 item;
    u16 oldItem;
};
s32 FieldAction_RequestAtFreeUnit(s32 a, s32 b, u16 c);
s32 FieldAction_PollResult(s32 idx);
s32 FieldAction_RequestPitfallAt(s8 a, Vec2 *p);
s32 FieldAction_RequestPitfall(s32 a, Vec2 *p);
s32 FieldAction_RequestFillHoleForAid(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestFillHole(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestPickUpForAid(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestPickUpIndoor(s32 a, Vec2 *p);
s32 FieldAction_RequestPickUpOutdoor(s32 a, Vec2 *p);
s32 FieldAction_RequestPickUp(s32 a, Vec2 *p, s32 c);
s32 FieldAction_RequestToolAtPendingForAid(s32 a, s32 b, s32 c, u16 d);
s32 FieldAction_RequestToolAtPending(s32 a, s32 b, s32 c, u16 d);
s32 FieldAction_RequestToolAtPendingForAid(s32 a, s32 b, s32 c, u16 d);
s32 FieldAction_RequestToolAtPending(s32 a, s32 b, s32 c, u16 d);
s32 FieldAction_RequestToolForAid(s32 a, Vec2 *p, s32 c, s32 d, u16 e);
s32 FieldAction_RequestTool(s32 a, Vec2 *p, s32 sel, s32 d, u16 e);
}
}

// ---- unk_020434f0.cpp
namespace nD {
extern "C" {

BOOL Item_IsTreeStage1(u16 *p);
s32 Item_GetTreeStage(u16 *p);
BOOL Town_CanReleaseBees(void);
s32 BlockMap_IsBuriedAtUnit(void *b, s32 x, s32 y);
BOOL Town_IsSaplingBlocker(u16 *p);
BOOL Item_IsFlower(u16 *p);
void FieldAction_ResolveBuryItem(s32 a, u16 *p, u16 *q, u8 *r, u16 e);
extern CommManager *gCommManager;
void Town_ClearBeesReleased(void);
void Town_SetBeesReleased(void);
void FieldAction_TryMoneyTree(u16 *out, u8 *flag, u16 c);
BOOL Town_CanReleaseBees(void);
void FieldAction_ResolveBuryItem(s32 a, u16 *p, u16 *q, u8 *r, u16 e);
static inline BOOL Unk_02043c28_InR(u16 &a, volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u16 x = *p;
    u16 b = *p;
    a = x;
    if (b >= lo && a <= hi) {
        r = TRUE;
    }
    return r;
}
static inline s32 Unk_02043c28_Idx(u32 v, u32 lo, u32 hi) {
    if (v >= lo && v <= hi) {
        return v - lo;
    }
    return -1;
}
BOOL TownState_IsPlayerDateNotToday(u8 *p);
s32 Item_GetPrice(volatile u16 *p);
void TownState_SetPlayerDateToday(u8 *p);
void FieldAction_TryMoneyTree(u16 *out, u8 *flag, u16 c);
static inline BOOL Unk_020437d0_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}
struct DigRequestArgs {
    u8 b;
    u16 item;
    u16 oldItem;
};
static inline BOOL Unk_02043540_Or(u16 *p) {
    BOOL r = TRUE;
    BOOL k = FALSE;
    if (*p >= 0xd4 && *p <= 0xda) {
        k = TRUE;
    }
    if (!k) {
        if (!(*p >= 0xdb && *p <= 0xe1)) {
            r = FALSE;
        }
    }
    return r;
}
static inline BOOL Unk_02043400_Or(BOOL prev, u32 c, u32 lo, u32 hi) {
    BOOL r = TRUE;
    if (!prev) {
        if (!(c >= lo && c <= hi)) {
            r = FALSE;
        }
    }
    return r;
}
static inline BOOL Unk_02043400_Eq(BOOL prev, u32 c, u32 v) {
    BOOL r = TRUE;
    if (!prev) {
        if (c != v) {
            r = FALSE;
        }
    }
    return r;
}
static inline BOOL Unk_02043400_K(u32 c) {
    volatile BOOL k1 = FALSE;
    if (c >= 0x26 && c <= 0x2a) k1 = TRUE;
    BOOL k2 = Unk_02043400_Or(k1, c, 0x5d, 0x61);
    BOOL k3 = Unk_02043400_Or(k2, c, 0x2f, 0x56);
    BOOL k4 = Unk_02043400_Or(k3, c, 0x57, 0x5b);
    BOOL k5 = Unk_02043400_Or(k4, c, 0x66, 0x68);
    BOOL k6 = Unk_02043400_Eq(k5, c, 0x69);
    BOOL k7 = Unk_02043400_Or(k6, c, 0x6a, 0x6c);
    BOOL k8 = Unk_02043400_Eq(k7, c, 0x6d);
    BOOL k9 = Unk_02043400_Or(k8, c, 0xc8, 0xcf);
    return k9;
}
}
}

// ---- unk_02043e70.cpp
namespace nE {
extern "C" {

extern CommManager *gCommManager;
extern void (*data_020da2a4[1])(EffectSplEmitter *);
extern void (*data_020da2a8[1])(EffectSplEmitter *);
s32 Flower_GetSpecies(u16 *a);
s32 Flower_GetColor(u16 *a);
void FlowerFx_SetParams(FlowerFxParams *g, s32 a, s32 b, s32 c, u8 d, VecFx32 *v, s16 e);
s32 Item_IsFlower(u16 *p);
s32 Scene_GetCurrent(void);
void Flower_TrampleAt(VecXZ *p, s32 f);
void Flower_TrampleTile(void *m, VecXZ *p, s32 f);
void Flower_SpawnPetalFx(u16 *a, VecXZ *p, s16 c, s32 d);
void Flower_PlayTrampleFx(VecXZ *p);
void Flower_Trample(VecXZ *p);
void Flower_TrampleAt(VecXZ *p, s32 f);
void Flower_TrampleTile(void *m, VecXZ *p, s32 f);
void DeadTurnip_SpawnDigFx(VecXZ *p);
void Weed_SpawnPullFx(u16 *a, VecXZ *p);
void Flower_SpawnPetalFxAt(u16 *a, VecXZ *p, s16 c, s32 d);
static inline BOOL Unk_02044098_R(u16 *p, u32 lo, u32 hi)
{
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}
void Flower_SpawnPetalFx(u16 *a, VecXZ *pos, s16 c, s32 d);
void FlowerFx_SetParams(FlowerFxParams *g, s32 a, s32 b, s32 c, u8 d, VecFx32 *v, s16 e);
struct Unk_020441f0_Color {
    u16 r : 5;
    u16 g : 5;
    u16 b : 5;
};
u16 Sky_GetLightColor(s32 a);
void Vec_RotateY(VecFx32 *t, s32 a);
s32 Vec_SafeNormalize(VecFx32 *t);
void FlowerFx_InitBySpecies(EffectSplEmitter *p);
void FlowerFx_InitByColor(EffectSplEmitter *p);
struct Unk_02044490_H6 { u16 a, b, c; };
void FieldAction_ApplyResult(FieldActionResultMsg *e);
void FieldAction_CancelPendingForResult(FieldActionResultMsg *e);
void FieldAction_ApplyResultOffscreen(FieldActionResultMsg *e, s32 t);
void FieldActionFx_Start(FieldActionResultMsg *e);
void FieldAction_OnNetResult(FieldActionResultMsg *e, s32 t);
void FieldAction_ApplyResultOffscreen(FieldActionResultMsg *e, s32 m);
void FieldAction_CancelPendingForResult(FieldActionResultMsg *e);
void FieldAction_ApplyResult(FieldActionResultMsg *e);
struct Unk_02044774_PP { u16 v; };
}
}

// ---- unk_020449e8.cpp
namespace nF {
extern "C" {

extern CommManager *gCommManager;
static inline BOOL Unk_020449e8_R(volatile u16 *p, u32 &c, u32 lo, u32 hi)
{
    BOOL r = FALSE;
    c = *p;
    u32 b = *p;
    if (b >= lo && c <= hi) {
        r = TRUE;
    }
    return r;
}
static inline void Unk_02044dd8_Set(u8 *pp, s32 y, s32 x)
{
    pp[1] = x;
    pp[0] = y;
}
static inline BOOL Unk_02044aa8_R(volatile u16 *p, u32 c, u32 lo, u32 hi)
{
    BOOL r = FALSE;
    u32 a = *p;
    if (a >= lo && c <= hi) {
        r = TRUE;
    }
    return r;
}
s32 func_01ffcb0c(s32 a, s32 b);
s32 PendingUnit_FindByAid(s32 a, Vec2 *p, s32 b);
BOOL Town_CanReleaseBees();
s32 Item_GetFruitTreeFruit(u16 *p);
void TreeDrop_SpawnFruit(FieldActionResultMsg *o, u32 x, u32 code);
void TreeDrop_SpawnSpecial(FieldActionResultMsg *o, FieldActionRequestMsg *s, u32 x);
u32 Tree_GetDropItem(FieldActionResultMsg *a, u32 id);
BOOL TreeDrop_FindUnit(FieldActionResultMsg *a, Vec2 *p, u32 idx, BlockMap *obj);
BOOL FieldAction_CanReserveUnit(u32 idx, Vec2 *p);
BOOL FieldAction_PreApplyOffline(FieldActionRequestMsg *s, FieldActionRequest *e);
BOOL Field_IsUnitClearOfOthersLocal(Vec2 *p);
void TreeDrop_SpawnSpecial(FieldActionResultMsg *o, FieldActionRequestMsg *s, u32 x);
void TreeDrop_SpawnFruit(FieldActionResultMsg *o, u32 x, u32 code);
u32 Tree_GetDropItem(FieldActionResultMsg *a, u32 id);
BOOL TreeDrop_FindUnit(FieldActionResultMsg *a, Vec2 *p, u32 idx, BlockMap *obj);
void FieldAction_Submit(u8 idx, u32 arg);
BOOL FieldAction_PreApplyOffline(FieldActionRequestMsg *s, FieldActionRequest *e);
BOOL FieldAction_CanReserveUnit(u32 idx, Vec2 *p);
BOOL Field_IsUnitClearOfOthersLocal(Vec2 *p);
BOOL Field_IsUnitClearOfOthersForAid(Vec2 *p, u32 x);
s32 PendingUnit_FindBySlot(u32 a, u32 b);
s32 PendingUnit_FindActiveOfAid(u32 a);
s32 PendingUnit_FindFree();
s32 PendingUnit_FindForAid(u32 a, Vec2 *p, s32 b);
}
}

// ---- unk_020452ec.cpp
namespace nG {
extern "C" {

extern u8 gTownJunkInsectFlags[];
s32 PendingUnit_FindActiveOfAid(u32 aid);
s32 PendingUnit_FindFree();
void PendingUnit_Flush(PendingUnit *e);
void PendingUnit_Commit(PendingUnit *e);
void PendingUnit_CommitForAid(PendingUnit *e);
void PendingUnit_Apply(PendingUnit *e);
void PendingUnit_WriteToMap(PendingUnit *e);
void PendingUnit_NoteJunk(PendingUnit *e);
void PendingUnit_SetIndex(s32 idx, u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
BOOL PendingUnit_CanAddAt(Vec2 *pos);
BOOL PendingUnit_Add(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
void Tree_DropItems(PackedUnitPos *q, u32 kind, Vec2 *pos);
BOOL Scene_InTown();
s32 FieldItemFx_CancelAt(u32 kind, Vec2 *pos);
static inline s32 Unk_021c3f8c_GetX(PackedUnitPos p) { return p.v >> 8; }
static inline s32 Unk_021c3f8c_GetY(PackedUnitPos p) { return p.v & 0xff; }
static inline void Unk_0204548c_Unpack(PackedUnitPos px, PackedUnitPos py, Vec2 *out) {
    out->x = Unk_021c3f8c_GetX(px);
    out->y = Unk_021c3f8c_GetY(py);
}
s32 PendingUnit_FindByAid(s32 aid, Vec2 *pos, s32 flag);
void PendingUnits_Flush();
void PendingUnit_ClearActiveOfAid(u32 aid);
void PendingUnit_CommitForAid(PendingUnit *e);
void PendingUnit_CommitAt(Vec2 *pos, s32 flag);
void PendingUnit_ApplyAtIfAid(Vec2 *pos, u32 v, s32 flag);
void PendingUnit_ApplyIndex(s32 i);
void PendingUnit_Flush(PendingUnit *e);
void PendingUnit_Commit(PendingUnit *e);
void PendingUnit_NoteJunk(PendingUnit *e);
void PendingUnit_WriteToMap(PendingUnit *e);
void PendingUnit_Set(PendingUnit *e, u8 aid, u16 pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
void PendingUnit_SetIndex(s32 idx, u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
BOOL PendingUnit_CanAddAt(Vec2 *pos);
BOOL PendingUnit_Add(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g);
void PendingUnit_Apply(PendingUnit *e);
}
}

// ---- unk_02045c18.cpp
namespace nH {
extern "C" {

struct Unk_02045d40_Ent {
    s32 a;
    s32 b;
};
struct Unk_02046a0_Ts {  // now, and the town's last update at 6:00
    ClockDateTimeWords now, last;
};
struct RoomScoreEvaluatorBuf {
    u8 b[0x14];
};
static inline BOOL Unk_02046358_R1(u16 *p) {
    BOOL r = FALSE;
    if (*p >= 0x1531 && *p <= 0x153a) {
        r = TRUE;
    }
    return r;
}
extern u8 gTownJunkInsectFlags[];
extern CommManager *gCommManager;
s32 TownJunkInsects_Apply(void *);
void Town_MarkEventApplied(TownUpdater *a, u32 b);
s32 _s32_div_f(s32, s32);
s32 FieldPos_FromBlockUnit(void *, s32, s32, s32, s32);
s32 TownJunkInsects_InitFromEval(void *);
Unk_02045d40_Ent *LitCedarList_Get();
s32 Clock_Update(s32);
void Town_RefillSeashells(TownUpdater *, BlockMap *);
void Town_RebuildLitCedarList();
void TownState_UpdateSeasonPeriod();
s32 Town_IsClockBeforeLastUpdate();
void Turnips_SpoilAll(void *);
s32 Scene_GetCurrent();
void Town_RefreshEvents(TownUpdater *, s32);
void TownState_UpdateSeasonPeriod();
s32 Town_IsClockBeforeLastUpdate();
void Turnips_SpoilAll(void *);
s32 Scene_GetCurrent();
void Town_RefreshEvents(TownUpdater *, s32);
void Town_UpdateDay(s32);
#define gSavePlayers HOST_LABEL(u8 (*)[], gSavePlayers)
void *HouseRoomMaps_Get(s32);
s32 Town_IsSandAt(BlockMap *p, s32 x, s32 y);
void BlockMap_SpoilTurnips(void *, TownBlockMap *);
s32 Scene_InTown();
s32 Scene_InTownUnk31();
s32 EventWeekSlots_Update();
s32 TownBbs_UpdateDaily();
void TownState_PickNextWeekDate(u8 *dst, u8 *src);
#define data_021ed29c HOST_LABEL(u8 (*)[], data_021ed29c)
s32 TownState_SetSeasonPeriod(u32);
s32 DateTime_GetSeasonPeriod(void *);
s32 TownUpdateThread_Request(void *, void *, s32, s32);
void Players_SpoilTurnips();
void Town_SpoilTurnips(void *);
void HouseRooms_SpoilTurnips(void *);
void RecycleBin_SpoilTurnips(void *);
void Town_ApplyElapsedDays(ClockDateTime *, ClockDateTime *, s32, s32, s32);
s32 DateTime_DiffMinutes(void *, void *);
void Field_UpdateActions();
void Field_ResetActions();
void TownUpdater_MarkEventApplied(u32 id);
void Town_RefreshDailyObjects();
void LitCedarList_Clear();
void LitCedarList_Add(s32 a, s32 b);
Unk_02045d40_Ent *LitCedarList_Get();
s32 LitCedarList_Find(s32 a, s32 b);
BOOL Town_GetRafflesiaPos(void *out);
void *Town_GetUpdater();
void *TownEval_GetAdvice();
s32 Town_GetEnvironmentRank();
void Field_OnEnter();
void Town_InitNew();
void Town_OnLoad();
BOOL Town_IsClockBeforeLastUpdate();
void Town_RebuildLitCedarList();
void Town_CheckDayChange();
void Town_UpdateDay(s32 flag);
void TownState_UpdateSeasonPeriod();
void Town_ApplyElapsedDays(ClockDateTime *a, ClockDateTime *b, s32 c, s32 d, s32 e);
void Turnips_SpoilAll(void *p);
void RecycleBin_SpoilTurnips(void *unused);
void HouseRooms_SpoilTurnips(void *p);
void Town_SpoilTurnips(void *p);
void Players_SpoilTurnips();
void BlockMap_SpoilTurnips(void *p, TownBlockMap *q);
s32 Town_WashUpBottle(TownUpdater *p);
void Town_RefillSeashells(TownUpdater *p, BlockMap *q);
}
}

// ---- unk_0204657c.cpp
namespace nI {
extern "C" {

extern CommManager *gCommManager;
#define gContestRecord HOST_LABEL(char (*)[], gContestRecord)
#define data_021ed0a0 HOST_LABEL(char (*)[], data_021ed0a0)
#define gSaveHouse HOST_LABEL(char (*)[], gSaveHouse)
#define gSaveLostAndFound HOST_LABEL(char (*)[], gSaveLostAndFound)
#define gSaveTownMap HOST_LABEL(char (*)[], gSaveTownMap)
#define data_021e58a7 HOST_LABEL(char (*)[], data_021e58a7)
#define data_021ed104 HOST_LABEL(char (*)[], data_021ed104)
#define gSaveBlancaFace HOST_LABEL(char (*)[], gSaveBlancaFace)
#define gSaveTownEventDate HOST_LABEL(u8 (*)[], gSaveTownEventDate)
void Town_SpawnSeashellsInAcre(TownUpdater *, BlockMap *, s32);
void TownEval_EvaluateAcreAt(TownEval *a, void *b, s32 i, s32 j);
s32 Town_IsSandAt(BlockMap *p, s32 x, s32 y);
void Town_RefreshEvents(TownUpdater *, s32);
void Town_UpdateEvents(TownUpdater *, BlockMap *, s32, s32, ClockDateTime *, s32);
s32 Game_IsIntroPeriod();
void Clock_GetDateTimeCleared(void *);
void Town_CleanupExpiredEvents(TownUpdater *a, BlockMap *p, s32 x, s32 y, SaveData *d);
void Town_ApplyDailyEvents(TownUpdater *a, BlockMap *b, s32 c, s32 d, ClockDateTime *e, s32 f, SaveData *g);
void Town_RemoveVisitorStructures(TownUpdater *, s32);
void Town_PlaceVisitorStructures(TownUpdater *a, SaveData *b, u32 c);
void Clock_GetCalendarKey(void *);
u16 *TownState_GetEvent(s32);
s32 Clock_GetYear();
void TownState_RemoveEvent(TownState *x, u32 id);
s32 Town_EndEventEffect(TownUpdater *, s32, BlockMap *, s32, s32);
void Town_UndecorateCedars(TownUpdater *, BlockMap *, s32);
void Town_ClearBorderTrees(void *);
void Town_WaterAll(TownUpdater *, BlockMap *, s32, s32);
void TownState_UpdatePerfectStreak(void *, s32, s32);
void Town_UpdateTrees(TownUpdater *, BlockMap *, s32, s32);
s32 DateTime_GetSeasonPeriod(void *);
void Town_SpawnWeeds(TownUpdater *, BlockMap *);
void Town_SpawnClover(TownUpdater *, BlockMap *, s32, s32);
void Town_SpawnDandelion(TownUpdater *, BlockMap *, s32, s32);
void Town_SpawnRandomFlower(TownUpdater *, BlockMap *, s32, s32);
void Town_UpdateVillagerHouseFlowers(TownUpdater *, BlockMap *, void *, s32);
void Town_TrySpawnJacobsLadder(TownUpdater *, BlockMap *, s32, s32);
void Town_UpdateRafflesia(TownUpdater *, BlockMap *, s32, s32);
void Town_SpawnCoconut(TownUpdater *, BlockMap *, s32);
void Town_UpgradeBuriedShovels(TownUpdater *, BlockMap *, s32, s32);
void NookShop_ApplyRenovation();
void Sky_OnDayChange(s32);
void NookShop_UpdateDaily(void *, s32);
void SaveVillagers_OnNewDay(void *a, void *b);
void Turnips_SpoilAll(void *);
namespace Unk_0204657c_N {
}
struct Unk_020465a4_L {
    u8 a;
    u8 b;
    u16 t;
};
void Town_SpawnSeashellsInAcre(TownUpdater *a, BlockMap *b, s32 c);
void Town_WaterAll(TownUpdater *a, BlockMap *p, s32 w, s32 h);
static inline BOOL Unk_02046ba8_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
void Town_RefreshEventsOffline();
void Town_RefreshEvents(TownUpdater *a, s32 b);
struct Unk_02046c80_D {
    u32 a;
    u32 b;
};
void Town_UpdateEvents(TownUpdater *a, BlockMap *p, s32 x, s32 y, ClockDateTime *c, s32 f);
static inline BOOL Unk_02046d28_Z(s32 v) {
    return v == 0 ? TRUE : FALSE;
}
void Town_RemoveVisitorStructures(TownUpdater *a, s32 b);
void Town_CleanupExpiredEvents(TownUpdater *a, BlockMap *p, s32 x, s32 y, SaveData *d);
s32 Town_EndEventEffect(TownUpdater *a, s32 id, BlockMap *p, s32 x, s32 y);
struct Unk_0204674c_O {
    u32 v[6];
};
}
}

// ---- unk_02046e90.cpp
namespace nJ {
extern "C" {

struct Unk_020473cc_Date {
    u32 w0, w1;
    Unk_020473cc_Date() { w0 = 0; w1 = 0; }
};
struct DayMonthYear { u8 b[3]; };
extern u8 data_020da2a0[];
EventDayEntry *Event_GetTodayList();
void Clock_GetCalendarKey(void *);
void TownState_AddEvent(void *, u32);
void LitCedarList_Clear();
void LitCedarList_Add(s32, s32);
void Town_SpawnVillagerHouseFlowers(TownUpdater *, BlockMap *, s32, s32);
s32 TownState_FindEvent(TownState *x, u32 id);
s32 Date_DaysBetween(u8 *a, u8 *b);
s32 BlockMap_IsBuried(void *, s32, s32, s32, s32);
s32 Weather_GetCurrent();
s32 func_02133150_dummy();
void Town_UpgradeBuriedShovels(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_ClearObjectFcFdAtPos(TownUpdater *a, VecFx32 *b);
void Town_SpawnCoconut(TownUpdater *a, BlockMap *b, s32 c);
s32 Town_GetSandAttr(BlockMap *p, s32 x, s32 y);
void Town_ApplyVillagerFlowerEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, ClockDateTime *d, s32 e, s32 f, SaveData *g);
void Town_DropAcorns(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i);
void Town_ApplyAcornEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i);
void Town_DropAcorns(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i);
void Town_DropAcornsInAcre(TownUpdater *a, BlockMap *b, s32 c, s32 d);
u32 Item_RandomAcorn(TownUpdater *a);
s32 Town_CountEventDays(TownUpdater *a, s32 b, u8 *c, ClockDateTime *d, s32 e, s32 f, SaveData *g);
void Town_UndecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z);
void Town_DecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z);
void Town_DecorateCedars(TownUpdater *a, BlockMap *m, s32 n);
void Town_MarkEventApplied(TownUpdater *a, u32 b);
void Town_PlaceVisitorStructures(TownUpdater *a, SaveData *b, u32 c);
void Town_ApplyDailyEvents(TownUpdater *a, BlockMap *b, s32 c, s32 d, ClockDateTime *e, s32 f, SaveData *g);
void Town_MarkEventApplied(TownUpdater *a, u32 b);
void Town_UndecorateCedars(TownUpdater *a, BlockMap *b, s32 c);
void Town_UndecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z);
void Town_DecorateCedars(TownUpdater *a, BlockMap *m, s32 n);
static inline BOOL Unk_020470b8_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
void Town_DecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z);
void Town_ApplyVillagerFlowerEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, ClockDateTime *d, s32 e, s32 f, SaveData *g);
void Town_ApplyAcornEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i);
void Town_DropAcorns(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i);
void Town_DropAcornsInAcre(TownUpdater *a, BlockMap *b, s32 x, s32 z);
u32 Item_RandomAcorn(TownUpdater *a);
s32 Town_CountEventDays(TownUpdater *a, s32 b, u8 *c, ClockDateTime *d, s32 e, s32 f, SaveData *g);
void Town_UpgradeBuriedShovels(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnCoconut(TownUpdater *a, BlockMap *b, s32 c);
void Town_ClearObjectFcFdAtPos(TownUpdater *a, VecFx32 *b);
}
}

// ---- unk_02047798.cpp
namespace nK {
extern "C" {

s32 _s32_div_f(s32 a, s32 b);
s32 SaveVillagers_GetUnk3830Index(void *p);
void *SaveVillagers_Get(void *p, s32 i);
s32 Villager_IsSettledExcept(void *a, s32 b);
void *Villager_GetPlan(void *p);  // (defined returning s32 in unk_02077ac4.cpp)
s32 HousePos_IsValid(void *p);
BOOL Town_IsGrass(BlockMap *p, s32 x, s32 y);
BOOL Town_IsPlantable(BlockMap *a, s32 x, s32 y);
BOOL Town_ConvertRockInAcre(TownUpdater *a, BlockMap *b, TownEvalAcre *cell, s32 d, s32 e, s32 f);
s32 Town_ConvertRandomRock(TownUpdater *a, BlockMap *b, s32 c, s32 d, s32 e, s32 f);
s32 Town_SpawnRafflesia(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_WitherRafflesia(TownUpdater *a, BlockMap *b);
void Town_SpawnJacobsLadder(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_PickJacobsLadderAcre(Vec2 *out, void *a, s32 c, s32 d);
void Town_SpawnVillagerHouseFlowers(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnFlowersAroundHouse(TownUpdater *a, BlockMap *b, Vec2 *pos, s32 d);
BOOL Pos_IsInList(void *a, Vec2 *pos, Vec2 *arr, s32 n);
static inline BOOL Unk_02047798_InRange(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
s32 Town_ConvertRandomRock(TownUpdater *a, BlockMap *b, s32 c, s32 d, s32 e, s32 f);
BOOL Town_ConvertRockInAcre(TownUpdater *a, BlockMap *b, TownEvalAcre *cell, s32 d, s32 e, s32 f);
static inline u16 *Unk_02047ac0_Get(BlockMap *b, TownEval *s, s32 u, s32 v, s32 x, s32 y) {
    return BlockMap_GetItemPtr(b, x + 1, y + 1, u, v, 0);
}
static inline void Unk_02047ac0_Set(TownUpdater *a, BlockMap *b, s32 u, s32 v, s32 x, s32 y) {
    BlockMap_PlaceItemAt(a, b, x + 1, y + 1, u, v, 0x89, 0);
}
void Town_UpdateRafflesia(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_WitherRafflesia(TownUpdater *a, BlockMap *b);
s32 Town_SpawnRafflesia(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_TrySpawnJacobsLadder(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnJacobsLadder(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_PickJacobsLadderAcre(Vec2 *out, void *a, s32 c, s32 d);
void Town_SpawnClover(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnDandelion(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnRandomFlower(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_UpdateVillagerHouseFlowers(TownUpdater *a, BlockMap *b, void *c, s32 d);
struct Vec2Ctor : Vec2 {
    Vec2Ctor(s32 px, s32 py) { x = px; y = py; }
};
void Town_SpawnVillagerHouseFlowers(TownUpdater *a, BlockMap *b, s32 c, s32 d);
void Town_SpawnFlowersAroundHouse(TownUpdater *a, BlockMap *b, Vec2 *pos, s32 d);
void Town_SpawnWeeds(TownUpdater *a, BlockMap *b);
BOOL Pos_IsInList(void *a, Vec2 *pos, Vec2 *arr, s32 n);
}
}

// ---- unk_020480a8.cpp
namespace nL {
extern "C" {

struct TreeQuota {
    s32 trees;
    s32 numSaplings;
    Vec2 saplingUnits[64];
    u16 saplingItems[64];
};
struct Unk_020485d4_Size {
    s32 w, h;
    Unk_020485d4_Size(s32 a, s32 b) : w(a), h(b) {}
};
extern CommManager *gCommManager;
s32 Town_MakeSpecialTree(TownUpdater *a, BlockMap *b, s32 x, s32 y, s32 e);
void BlockMap_SetBuriedAtUnit(void *b, s32 x, s32 y);
void BlockMap_ClearBuriedAtUnit(void *b, s32 x, s32 y);
s32 Scene_GetCurrent();
s32 Town_IsSaplingBlocker(u16 *t);
void Town_LimitSaplingsPerQuadrant(TownUpdater *a, BlockMap *q, s32 w, s32 h);
void Town_GrowTrees(TownUpdater *a, BlockMap *q, Unk_020485d4_Size *p);
void Town_GrowTree(BlockMap *q, u16 *t, s32 x, s32 y);
void TreeQuota_InitAll(TreeQuota (*s)[2], TownEvalAcre *c);
void TreeQuota_AddAt(TreeQuota (*s)[2], u16 id, s32 x, s32 y, s32 i, s32 j);
void TreeQuota_ApplyAll(TreeQuota (*s)[2]);
void TreeQuota_Apply(TreeQuota *s);
void TreeQuota_Add(TreeQuota *s, u16 id, s32 x, s32 y);
void TreeQuota_Init(TreeQuota *s, TownEvalAcre *c, s32 i, s32 j);
s32 Town_CollectFreeUnits(TownUpdater *a, Vec2 *arr, BlockMap *c, s32 d, s32 e, s32 (*fn)(BlockMap *, s32, s32));
struct Unk_020481b8_PosZ { s32 x, y; Unk_020481b8_PosZ() { x = 0; y = 0; } };
s32 Town_MakeSpecialTree(TownUpdater *a, BlockMap *b, s32 x, s32 y, s32 e);
s32 Town_CollectFreeUnits(TownUpdater *a, Vec2 *arr, BlockMap *c, s32 d, s32 e, s32 (*fn)(BlockMap *, s32, s32));
BOOL Town_IsSandAt(BlockMap *p, s32 x, s32 y);
s32 Town_GetSandAttr(BlockMap *p, s32 x, s32 y);
BOOL Town_IsPlantable(BlockMap *p, s32 x, s32 y);
BOOL Town_IsGrass(BlockMap *p, s32 x, s32 y);
BOOL Town_IsGrass(BlockMap *p, s32 x, s32 y);
void Town_UpdateTrees(TownUpdater *a, BlockMap *q, s32 c, s32 d);
static inline BOOL Unk_02048634_Check(u16 *p) {
    BOOL f9 = TRUE, f8 = TRUE, f7 = TRUE, f6 = TRUE, f5 = TRUE, f4 = TRUE, f3 = TRUE, f2 = TRUE, f1 = FALSE;
    u32 v = *p;
    if (v >= 0x26 && v <= 0x2a) f1 = TRUE;
    if (!f1 && !(v >= 0x5d && v <= 0x61)) f2 = FALSE;
    if (!f2 && !(v >= 0x2f && v <= 0x56)) f3 = FALSE;
    if (!f3 && !(v >= 0x57 && v <= 0x5b)) f4 = FALSE;
    if (!f4 && !(v >= 0x66 && v <= 0x68)) f5 = FALSE;
    if (!f5 && !(v == 0x69)) f6 = FALSE;
    if (!f6 && !(v >= 0x6a && v <= 0x6c)) f7 = FALSE;
    if (!f7 && !(v == 0x6d)) f8 = FALSE;
    if (!f8 && !(v >= 0xc8 && v <= 0xcf)) f9 = FALSE;
    return f9;
}
void Town_GrowTrees(TownUpdater *a, BlockMap *q, Unk_020485d4_Size *sz);
void Town_LimitSaplingsPerQuadrant(TownUpdater *a, BlockMap *q, s32 w, s32 h);
void TreeQuota_ApplyAll(TreeQuota (*s)[2]);
void TreeQuota_AddAt(TreeQuota (*s)[2], u16 id, s32 x, s32 y, s32 i, s32 j);
void TreeQuota_InitAll(TreeQuota (*s)[2], TownEvalAcre *c);
void TreeQuota_Apply(TreeQuota *s);
void TreeQuota_Add(TreeQuota *s, u16 id, s32 x, s32 y);
void TreeQuota_Init(TreeQuota *s, TownEvalAcre *c, s32 i, s32 j);
}
}

// ---- unk_020489cc.cpp
namespace nM {
extern "C" {

typedef Unk_02048cc4_Pos Pos;
s32 _s32_div_f(s32 a, s32 b);
s32 Town_IsPlantable(BlockMap *m, s32 x, s32 y);
s32 Flower_GetSpecies(u16 *p);
u16 Flower_PickHybrid(u16 *a, u16 *b);
BOOL Town_HasNoBlockerAround(TownUpdater *a, BlockMap *m, Pos size, Pos pos);
void Town_CheckSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
void Town_CheckPalmSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
void Town_CheckCedarSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
void Town_WitherNeighborSaplings(TownUpdater *a, BlockMap *m, Pos size, Pos pos);
BOOL Town_TrySpawnFlowerAt(BlockMap *m, Pos p, Pos q, Pos r, u16 v);
BOOL Town_SpawnFlowerNearby(BlockMap *m, Pos size, Pos pos, u16 v);
static inline u16 *Cell(BlockMap *m, s32 x, s32 y, s32 layer)
{
    s32 hx = x >> 4, hy = y >> 4;
    return BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), layer);
}
static inline BOOL Match(u16 *p, BOOL z0, BOOL z1, BOOL z2, BOOL z3, BOOL z4, BOOL z5, BOOL z6, BOOL z7, BOOL z8)
{
    BOOL n0 = z0, n1 = 1, n2 = 1, n3 = 1, n4 = 1, n5 = 1, n6 = 1, n7 = 1, n8 = 1;
    u32 t = *p;
    if (t >= 0x26 && t <= 0x2a) n0 = 1;
    if (!n0 && !(t >= 0x5d && t <= 0x61)) n1 = z1;
    if (!n1 && !(t >= 0x2f && t <= 0x56)) n2 = z2;
    if (!n2 && !(t >= 0x57 && t <= 0x5b)) n3 = z3;
    if (!n3 && !(t >= 0x66 && t <= 0x68)) n4 = z4;
    if (!n4 && t != 0x69) n5 = z5;
    if (!n5 && !(t >= 0x6a && t <= 0x6c)) n6 = z6;
    if (!n6 && t != 0x6d) n7 = z7;
    if (!n7 && !(t >= 0xc8 && t <= 0xcf)) n8 = z8;
    return n8;
}
static inline u16 *CellP(BlockMap *m, const Pos &p, s32 layer)
{
    s32 x = p.x;
    s32 y = p.y;
    s32 hx = x >> 4, hy = y >> 4;
    return BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), layer);
}
void Town_WitherNeighborSaplings(TownUpdater *a, BlockMap *m, Pos size, Pos pos);
void Town_CheckPalmSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
void Town_CheckCedarSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
void Town_CheckSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y);
BOOL Town_HasNoBlockerAround(TownUpdater *a, BlockMap *m, Pos size, Pos pos);
BOOL Town_SpawnFlowerNearby(BlockMap *m, Pos size, Pos pos, u16 v);
BOOL Town_TrySpawnFlowerAt(BlockMap *m, Pos p, Pos q, Pos r, u16 v);
static inline BOOL Unk_020489cc_Check(u16 *p) {
    BOOL f9 = TRUE, f8 = TRUE, f7 = TRUE, f6 = TRUE, f5 = TRUE, f4 = TRUE, f3 = TRUE, f2 = TRUE, f1 = FALSE;
    u32 v = *p;
    if (v >= 0x26 && v <= 0x2a) f1 = TRUE;
    if (!f1 && !(v >= 0x5d && v <= 0x61)) f2 = FALSE;
    if (!f2 && !(v >= 0x2f && v <= 0x56)) f3 = FALSE;
    if (!f3 && !(v >= 0x57 && v <= 0x5b)) f4 = FALSE;
    if (!f4 && !(v >= 0x66 && v <= 0x68)) f5 = FALSE;
    if (!f5 && !(v == 0x69)) f6 = FALSE;
    if (!f6 && !(v >= 0x6a && v <= 0x6c)) f7 = FALSE;
    if (!f7 && !(v == 0x6d)) f8 = FALSE;
    if (!f8 && !(v >= 0xc8 && v <= 0xcf)) f9 = FALSE;
    return f9;
}
}
}

// ---- unk_020492fc.cpp
namespace nN {
extern "C" {

s32 TownEval_GetTownRank(void *a, s32 *p, s32 w, s32 h);
s32 Flower_GetColor(u16 *p);
BOOL Item_IsBuildingOrOccupied(u16 *p);
BOOL Town_IsSaplingBlockerOrRock(u16 *p);
BOOL Town_IsSaplingBlocker(u16 *p);
s32 Flower_GetSpecies(u16 *p);
s32 Flower_GetSpecies(u16 *p);
s32 Flower_GetColor(u16 *p);
u16 Flower_PickHybrid(u16 *a, u16 *b);
BOOL Town_IsSaplingBlockerOrRock(u16 *p);
BOOL Town_IsSaplingBlocker(u16 *p);
BOOL Item_IsFruitTreeLastNoFruit(u16 *p);
void BlockMap_SetItem(void *a, u16 *t, s32 x, s32 y, s32 p4, s32 p5, s32 z);
void BlockMap_SetBuried(void *a, s32 x, s32 y, s32 p4, s32 p5);
void BlockMap_ClearBuried(void *a, s32 x, s32 y, s32 p4, s32 p5);
void BlockMap_SetBuriedAtUnit(void *a, s32 x, s32 y);
void BlockMap_ClearBuriedAtUnit(void *a, s32 x, s32 y);
BOOL Scene_InTown();
BOOL Scene_InTownUnk31();
RoomBlockMap *HouseRoomMaps_GetForScene(s32 a);
void Town_GrowTree(BlockMap *a, u16 *p, s32 x, s32 y);
void Scene_SetUnitItem(s32 x, s32 y, u32 item, s32 flag);
void TownEval_EvaluateAcreAt(TownEval *a, void *b, s32 i, s32 j);
s32 TownEval_GetTownRank(void *a, s32 *p, s32 w, s32 h);
}
}

// ---- unk_02049e1c.cpp
namespace nO {
extern "C" {

static inline BOOL Unk_02049e40_InRange(u32 v, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (v >= lo && v <= hi) r = TRUE;
    return r;
}
BOOL BlockMap_IsBuried(void *map, s32 x, s32 z, s32 a, s32 b);
BOOL Item_IsTreeStage3(u16 *p);
void TownEval_CalcScore(TownEvalAcre *out);
void TownEval_RemoveObject(TownEvalAcre *out, BlockMap *map, s32 x, s32 z, s32 n);
void TownEval_RemoveObject(TownEvalAcre *out, BlockMap *map, s32 x, s32 z, s32 n);
void TownEval_CalcScore(TownEvalAcre *out);
}
}


namespace nZ {
extern "C" {
void FlowerFx_InitBySpecies(EffectSplEmitter *p);
void FlowerFx_InitByColor(EffectSplEmitter *p);
extern const u16 data_020c90e4[2];
extern const u16 data_020c90e8[2];
extern const u16 data_020c90ec[2];
extern const u16 data_020c90f0[2];
extern const u16 data_020c90f4[2];
extern const u16 data_020c90f8[2];
extern const u16 data_020c90fc[2];
extern const u16 data_020c9100[2];
extern const u16 data_020c9104[2];
extern const u16 data_020c9108[2];
extern const u16 data_020c9110[2];
extern const FlowerHybridEntry data_020c9124[1];
extern const FlowerHybridEntry data_020c9134[1];
extern const FlowerHybridEntry data_020c913c[1];
extern const FlowerHybridEntry data_020c9144[1];
extern const FlowerHybridEntry data_020c914c[1];
extern const FlowerHybridEntry data_020c9154[1];
extern const FlowerHybridEntry data_020c915c[1];
extern const FlowerHybridEntry data_020c916c[1];
extern const FlowerHybridEntry data_020c9174[1];
extern const FlowerHybridEntry data_020c917c[1];
extern const FlowerHybridEntry data_020c9184[1];
extern const FlowerHybridEntry data_020c918c[1];
extern const FlowerHybridEntry data_020c9194[1];
extern const FlowerHybridEntry data_020c919c[1];
extern const FlowerHybridEntry data_020c91a4[1];
extern const FlowerHybridEntry data_020c91ac[1];
extern const FlowerHybridEntry data_020c91b4[1];
extern const FlowerHybridEntry data_020c91bc[1];
extern const u16 data_020c9218[8];
extern const u16 data_020c9228[8];
extern const u16 data_020c9238[8];
extern const FlowerHybridEntry data_020c9248[2];
extern const FlowerHybridEntry data_020c9258[2];
extern const FlowerHybridEntry data_020c9268[2];
extern const FlowerHybridEntry data_020c9278[2];
extern const FlowerHybridEntry data_020c9288[2];
extern const FlowerHybridEntry data_020c9298[2];
extern const FlowerHybridEntry data_020c92a8[2];
extern const FlowerHybridEntry data_020c92b8[2];
extern const FlowerHybridEntry data_020c92c8[2];
extern const FlowerHybridEntry data_020c92d8[2];
extern const FlowerHybridEntry data_020c92e8[2];
extern const FlowerHybridEntry data_020c92f8[2];
extern const FlowerHybridEntry data_020c9308[2];
extern const FlowerHybridEntry data_020c9318[2];
extern const FlowerHybridEntry data_020c9328[2];
extern const FlowerHybridEntry data_020c9338[2];
extern const FlowerHybridEntry data_020c9348[2];
extern const FlowerHybridEntry data_020c9358[2];
extern const FlowerHybridEntry data_020c9368[2];
extern const FlowerHybridEntry data_020c9378[2];
extern const FlowerHybridEntry data_020c9388[2];
extern const FlowerHybridEntry data_020c9398[2];
extern const FlowerHybridEntry data_020c93a8[2];
extern const FlowerHybridEntry data_020c93b8[2];
extern const FlowerHybridEntry data_020c93c8[2];
extern const FlowerHybridEntry data_020c93d8[2];
extern const FlowerHybridEntry data_020c93e8[2];
extern const FlowerHybridEntry data_020c93f8[2];
extern const FlowerHybridEntry data_020c9408[2];
extern const FlowerHybridEntry data_020c9418[2];
extern const FlowerHybridEntry data_020c9428[2];
extern const FlowerHybridEntry data_020c9438[2];
extern const FlowerHybridEntry data_020c9448[2];
extern const FlowerHybridEntry data_020c9458[2];
extern const FlowerHybridEntry data_020c9468[2];
extern const FlowerHybridEntry data_020c9478[2];
extern const FlowerHybridEntry data_020c9488[2];
extern const FlowerHybridEntry data_020c9498[2];
extern const FlowerHybridEntry data_020c94a8[2];
extern const FlowerHybridEntry data_020c94b8[2];
extern const FlowerHybridEntry data_020c94c8[2];
extern const FlowerHybridEntry data_020c94d8[2];
extern const FlowerHybridEntry data_020c94e8[2];
extern const FlowerHybridEntry data_020c94f8[2];
extern const FlowerHybridEntry data_020c9508[2];
extern const FlowerHybridEntry data_020c9518[2];
extern const FlowerHybridEntry data_020c9528[2];
extern const FlowerHybridEntry data_020c9538[2];
extern const FlowerHybridEntry data_020c9548[2];
extern const FlowerHybridEntry data_020c9558[2];
extern const FlowerHybridEntry data_020c9568[2];
extern const FlowerHybridEntry data_020c9578[2];
extern const FlowerHybridEntry data_020c9588[2];
extern const FlowerHybridEntry data_020c9598[2];
extern const FlowerHybridEntry data_020c95a8[2];
extern const FlowerHybridEntry data_020c95b8[2];
extern const FlowerHybridEntry data_020c95c8[2];
extern const FlowerHybridEntry data_020c95d8[2];
extern const FlowerHybridEntry data_020c95e8[2];
extern const FlowerHybridEntry data_020c95f8[2];
extern const FlowerHybridEntry data_020c9608[2];
extern const FlowerHybridEntry data_020c9618[2];
extern const FlowerHybridEntry data_020c9628[2];
extern const FlowerHybridEntry data_020c9638[2];
extern const FlowerHybridEntry data_020c9648[2];
extern const FlowerHybridEntry data_020c9658[2];
extern const FlowerHybridEntry data_020c9668[2];
extern const FlowerHybridEntry data_020c9678[2];
extern const FlowerHybridEntry data_020c9698[2];
extern const FlowerHybridEntry data_020c96a8[2];
extern const u16 data_020c96e0[10];
extern const FlowerHybridEntry data_020c96f4[3];
extern const FlowerHybridEntry data_020c970c[3];
extern const FlowerHybridEntry data_020c9724[3];
extern const FlowerHybridEntry data_020c973c[3];
extern const FlowerHybridEntry data_020c976c[3];
extern const FlowerHybridEntry data_020c9784[3];
extern const FlowerHybridEntry data_020c979c[3];
extern const FlowerHybridEntry data_020c97b4[3];
extern const FlowerHybridEntry data_020c97e4[3];
extern const FlowerHybridEntry data_020c97fc[3];
extern const FlowerHybridEntry data_020c9814[3];
extern const FlowerHybridList data_020c9880[9];
extern const FlowerHybridList data_020c98c8[9];
extern const FlowerHybridList data_020c9910[9];
extern const FlowerHybridList data_020c9958[9];
extern const FlowerHybridList data_020c99a0[9];
extern const FlowerHybridList data_020c99e8[9];
extern const FlowerHybridList data_020c9a30[9];
extern const FlowerHybridList data_020c9a78[9];
extern const FlowerHybridList data_020c9ac0[9];
extern const FlowerHybridList data_020c9b08[9];
extern const FlowerHybridList data_020c9b50[9];
extern const FlowerHybridList data_020c9b98[9];
extern const FlowerHybridList data_020c9be0[9];
extern const FlowerHybridList data_020c9c28[9];
extern const FlowerHybridList data_020c9c70[9];
extern const FlowerHybridList data_020c9cb8[9];
extern const FlowerHybridList data_020c9d00[9];
extern const FlowerHybridList data_020c9d48[9];
extern const FlowerHybridList data_020c9d90[9];
extern const FlowerHybridList data_020c9dd8[9];
extern const FlowerHybridList data_020c9e20[9];
extern const FlowerHybridList data_020c9e68[9];
extern const FlowerHybridList data_020c9eb0[9];
extern const FlowerHybridList data_020c9ef8[9];
extern const FlowerHybridList data_020c9f40[9];
extern const FlowerHybridList data_020c9f88[9];
extern const FlowerHybridList data_020c9fd0[9];
extern u8 data_020da2a0[4];
extern void (*data_020da2a4[1])(EffectSplEmitter *);
extern void (*data_020da2a8[1])(EffectSplEmitter *);
}
}

namespace nZ {
extern "C" {
PendingUnitList sPendingUnits;
extern const u16 data_020c9110[2] = {0x2617, 0x7fff};
extern const u16 data_020c9104[2] = {0x21ae, 0x0};
extern const FlowerHybridEntry data_020c9648[2] = {{80, 0x14}, {20, 0x18}};
extern const FlowerHybridEntry data_020c9498[2] = {{50, 0x1}, {50, 0x2}};
extern const FlowerHybridList data_020c9ac0[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {1, data_020c918c}, {3, data_020c9724}, {2, data_020c9378},
};
extern const FlowerHybridEntry data_020c9194[1] = {{100, 0xf}};
extern const FlowerHybridList *sFlowerHybridTable[4][10] = {
    {data_020c9cb8, data_020c9d00, data_020c9d90, data_020c9dd8, data_020c9e20, data_020c9e68, data_020c9eb0, data_020c9cb8, data_020c9cb8, data_020c9cb8},
    {data_020c9cb8, data_020c9f40, data_020c9f88, data_020c9880, data_020c98c8, data_020c9910, data_020c99a0, data_020c9cb8, data_020c9cb8, data_020c9cb8},
    {data_020c9cb8, data_020c99e8, data_020c9a78, data_020c9b08, data_020c9b50, data_020c9be0, data_020c9c70, data_020c9cb8, data_020c9cb8, data_020c9cb8},
    {data_020c9cb8, data_020c9d48, data_020c9ef8, data_020c9fd0, data_020c9958, data_020c9a30, data_020c9ac0, data_020c9b98, data_020c9c28, data_020c9cb8},
};
extern const FlowerHybridEntry data_020c9378[2] = {{80, 0x17}, {20, 0x19}};
extern const FlowerHybridEntry data_020c9784[3] = {{35, 0x12}, {35, 0x14}, {30, 0x16}};
extern const FlowerHybridEntry data_020c9638[2] = {{50, 0x14}, {50, 0x17}};
extern const FlowerHybridEntry data_020c9428[2] = {{50, 0x7}, {50, 0xa}};
extern const u16 sSaplingFruitItems[6] = {
    0x1518, 0x1519, 0x151a, 0x151b, 0x151c, 0x0000,
};
extern const FlowerHybridList data_020c9b98[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {2, data_020c93b8}, {2, data_020c93e8},
};
extern const FlowerHybridEntry data_020c9448[2] = {{50, 0x0}, {50, 0x4}};
extern const u16 sFruitSaplings2[6] = {
    0x0037, 0x003f, 0x0047, 0x002f, 0x004f, 0x0000,
};
extern const FlowerHybridEntry data_020c9568[2] = {{50, 0xd}, {50, 0x11}};
extern const FlowerHybridEntry data_020c9508[2] = {{50, 0x2}, {50, 0x5}};
extern const FlowerHybridEntry data_020c9538[2] = {{50, 0x3}, {50, 0x5}};
extern const FlowerHybridEntry data_020c97b4[3] = {{40, 0x0}, {40, 0x2}, {20, 0x4}};
extern const FlowerHybridList data_020c9be0[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {1, data_020c917c},
    {2, data_020c94a8}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridList data_020c9c70[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {2, data_020c9488}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c914c[1] = {{100, 0x14}};
extern const s8 sWaterOffsetsX[8] = {
    0, -1, 1, 0, 0, 0, 0, 0,
};
extern const FlowerHybridEntry data_020c94f8[2] = {{50, 0x13}, {50, 0x14}};
extern const FlowerHybridEntry data_020c9438[2] = {{50, 0x12}, {50, 0x15}};
extern const FlowerHybridEntry data_020c9518[2] = {{50, 0x6}, {50, 0x8}};
extern const FlowerHybridList data_020c9cb8[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c9418[2] = {{50, 0x1}, {50, 0x5}};
extern const FlowerHybridEntry data_020c9468[2] = {{80, 0x12}, {20, 0x17}};
extern const FlowerHybridEntry data_020c9124[1] = {{100, 0xc}};
extern const FlowerHybridList data_020c9d00[9] = {
    {0, NULL}, {2, data_020c94d8}, {3, data_020c979c},
    {3, data_020c97b4}, {2, data_020c96a8}, {2, data_020c9448},
    {2, data_020c93f8}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c9278[2] = {{50, 0xa}, {50, 0xb}};
extern const FlowerHybridEntry data_020c97fc[3] = {{35, 0x14}, {35, 0x12}, {30, 0x16}};
extern const FlowerHybridList data_020c9d48[9] = {
    {0, NULL}, {2, data_020c9358}, {3, data_020c976c},
    {3, data_020c9784}, {2, data_020c9438}, {2, data_020c9458},
    {2, data_020c9468}, {2, data_020c9478}, {1, data_020c9184},
};
extern const FlowerHybridEntry data_020c93a8[2] = {{80, 0x13}, {20, 0x17}};
extern const FlowerHybridEntry data_020c9338[2] = {{50, 0xd}, {50, 0xf}};
extern const FlowerHybridEntry data_020c9578[2] = {{50, 0x4}, {50, 0x5}};
extern const FlowerHybridList data_020c9d90[9] = {
    {0, NULL}, {0, NULL}, {1, data_020c915c},
    {2, data_020c9498}, {2, data_020c9408}, {2, data_020c94c8},
    {2, data_020c9418}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c9698[2] = {{50, 0x9}, {50, 0xa}};
extern const FlowerHybridEntry data_020c9144[1] = {{100, 0x3}};
extern const FlowerHybridEntry data_020c9618[2] = {{50, 0x6}, {50, 0xa}};
extern const FlowerHybridList data_020c9dd8[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {2, data_020c9318}, {2, data_020c9328}, {2, data_020c93c8},
    {2, data_020c9508}, {0, NULL}, {0, NULL},
};
TownUpdater gTownUpdater;
extern const FlowerHybridEntry data_020c91bc[1] = {{100, 0xa}};
extern const u16 *data_020da2e4[7] = {data_020c90f0, data_020c9100, data_020c90fc, data_020c90f4, data_020c9108, data_020c9108, data_020c9108};
extern const FlowerHybridEntry data_020c9658[2] = {{50, 0x7}, {50, 0x9}};
extern const FlowerHybridEntry data_020c91b4[1] = {{100, 0x4}};
extern const FlowerHybridList data_020c9f88[9] = {
    {0, NULL}, {0, NULL}, {1, data_020c916c},
    {3, data_020c9814}, {2, data_020c9658}, {2, data_020c9428},
    {2, data_020c9668}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridList data_020c9fd0[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {1, data_020c91ac}, {2, data_020c9608}, {3, data_020c97fc},
    {2, data_020c9638}, {2, data_020c9648}, {1, data_020c914c},
};
extern const FlowerHybridEntry data_020c91ac[1] = {{100, 0x14}};
extern const u16 data_020c9108[2] = {0x2f6b, 0x0};
extern const u8 sNeighborOffsets8[8] = {
    0x77, 0x87, 0x97, 0x78, 0x98, 0x79, 0x89, 0x99,
};
extern const FlowerHybridEntry data_020c9308[2] = {{80, 0x16}, {20, 0x19}};
extern const FlowerHybridEntry data_020c94b8[2] = {{50, 0x8}, {50, 0x9}};
extern const FlowerHybridEntry data_020c96a8[2] = {{50, 0x0}, {50, 0x3}};
extern const FlowerHybridEntry data_020c96f4[3] = {{35, 0xc}, {35, 0xd}, {30, 0xf}};
extern const FlowerHybridList data_020c9880[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {2, data_020c9678}, {2, data_020c94b8}, {2, data_020c92e8},
    {2, data_020c92d8}, {0, NULL}, {0, NULL},
};
FieldActionFxEvent sFieldActionFxSlots[5];
extern const u16 data_020c9238[8] = {0x1e12, 0x6bff, 0x10bf, 0x133f, 0x4d3f, 0x19f, 0x1c2d, 0x0};
extern const u8 sTreeDropOffsets[12] = {
    0x88, 0x78, 0x98, 0x89, 0x79, 0x99, 0x87, 0x77, 0x97, 0x00, 0x00, 0x00,
};
extern const u32 sFlowerParchChance[27] = {
    0x00000005, 0x00000005, 0x00000005, 0x0000000f, 0x0000000f, 0x0000001e, 0x00000005, 0x00000005,
    0x00000005, 0x0000000f, 0x0000000f, 0x0000001e, 0x00000005, 0x00000005, 0x00000005, 0x0000000f,
    0x0000000f, 0x0000001e, 0x00000005, 0x00000005, 0x00000005, 0x0000000f, 0x00000019, 0x00000019,
    0x0000001e, 0x00000028, 0x0000001e,
};
extern const FlowerHybridEntry data_020c919c[1] = {{100, 0x16}};
extern const FlowerHybridList data_020c98c8[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {1, data_020c91a4}, {2, data_020c9698},
    {2, data_020c92b8}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridList data_020c9910[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {1, data_020c91bc},
    {2, data_020c9278}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c92a8[2] = {{50, 0xc}, {50, 0xe}};
extern const FlowerHybridEntry data_020c9298[2] = {{50, 0x16}, {50, 0x17}};
extern const FlowerHybridEntry data_020c9258[2] = {{70, 0x15}, {30, 0x18}};
extern const FlowerHybridEntry data_020c9608[2] = {{50, 0x14}, {50, 0x15}};
extern const FlowerHybridEntry data_020c95f8[2] = {{50, 0x6}, {50, 0x7}};
TownEval gTownEval;
}
}

TownEvalAdvice::TownEvalAdvice() {
    acreX = 0;
    acreZ = 0;
    acreX = -1;
    acreZ = -1;
    reason = -1;
}

TownEvalAdvice::~TownEvalAdvice() {}

namespace nZ {
extern "C" {
extern const u16 data_020c9754[12] = {
    0x0000, 0x0001, 0x0002, 0x0006, 0x0007, 0x0008, 0x000c, 0x000d, 0x000e, 0x0012, 0x0013, 0x0014,
};
extern const u16 data_020c90ec[2] = {0x1dae, 0x0};
extern const FlowerHybridEntry data_020c9184[1] = {{100, 0x12}};
extern const FlowerHybridEntry data_020c9318[2] = {{80, 0x2}, {20, 0x5}};
extern const FlowerHybridEntry data_020c9588[2] = {{80, 0x13}, {20, 0x19}};
extern const FlowerHybridEntry data_020c9134[1] = {{100, 0x5}};
extern const u32 sTreeDropSearchOrder[3][8] = {
    0x00000000, 0x00000003, 0x00000006, 0x00000001, 0x00000004, 0x00000007, 0x00000005, 0x00000008,
    0x00000000, 0x00000003, 0x00000006, 0x00000002, 0x00000005, 0x00000008, 0x00000004, 0x00000007,
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000007, 0x00000008,
};
extern const u8 data_020c9850[48] = {
    0x55, 0x65, 0x75, 0x85, 0x95, 0xa5, 0xb5, 0x56, 0x66, 0x76, 0x86, 0x96, 0xa6, 0xb6, 0x57, 0x67,
    0x77, 0x87, 0x97, 0xa7, 0xb7, 0x58, 0x68, 0x78, 0x98, 0xa8, 0xb8, 0x59, 0x69, 0x79, 0x89, 0x99,
    0xa9, 0xb9, 0x5a, 0x6a, 0x7a, 0x8a, 0x9a, 0xaa, 0xba, 0x5b, 0x6b, 0x7b, 0x8b, 0x9b, 0xab, 0xbb,
};
extern const u8 data_020c96b8[20] = {
    0x57, 0xa7, 0x58, 0xa8, 0x59, 0xa9, 0x5a, 0x6a, 0x7a, 0x9a, 0xaa, 0x5b, 0x6b, 0x7b, 0x8b, 0x9b,
    0xab, 0x00, 0x00, 0x00,
};
extern const FlowerHybridEntry data_020c9368[2] = {{50, 0xe}, {50, 0xf}};
extern const u16 sFruitSaplings[6] = {
    0x0037, 0x003f, 0x0047, 0x002f, 0x004f, 0x0000,
};
extern const s32 sSeashellWeights[9] = {
    2, 3, 5, 10, 10, 0, 20, 25,
    25,
};
extern const FlowerHybridEntry data_020c93b8[2] = {{50, 0x18}, {50, 0x12}};
extern const u16 data_020c90f4[2] = {0x2e27, 0x0};
extern const FlowerHybridEntry data_020c94e8[2] = {{50, 0xf}, {50, 0x10}};
extern const FlowerHybridEntry data_020c93c8[2] = {{50, 0x2}, {50, 0x4}};
extern const FlowerHybridEntry data_020c94c8[2] = {{50, 0x1}, {50, 0x4}};
extern const u16 data_020c9228[8] = {0x2212, 0x7fbf, 0x35f, 0x18, 0x7cbb, 0x11f, 0x74ad, 0x0};
extern const FlowerHybridEntry data_020c9174[1] = {{100, 0x15}};
extern const FlowerHybridEntry data_020c915c[1] = {{100, 0x1}};
extern const u16 *data_020da2ac[7] = {data_020c90e8, data_020c9104, data_020c90e4, data_020c90ec, data_020c90f8, data_020c90f8, data_020c90f8};
extern const FlowerHybridEntry data_020c92d8[2] = {{50, 0x8}, {50, 0xb}};
extern const FlowerHybridList data_020c9b08[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {1, data_020c9154}, {2, data_020c9368}, {2, data_020c9388},
    {2, data_020c9398}, {0, NULL}, {0, NULL},
};
extern const u16 data_020c90f0[2] = {0x1a48, 0x0};
extern const FlowerHybridEntry data_020c979c[3] = {{35, 0x0}, {35, 0x1}, {30, 0x3}};
extern const FlowerHybridEntry data_020c9458[2] = {{50, 0x12}, {50, 0x16}};
extern const FlowerHybridEntry data_020c9478[2] = {{50, 0x12}, {50, 0x18}};
extern const FlowerHybridEntry data_020c9488[2] = {{50, 0xd}, {50, 0x11}};
extern const FlowerHybridEntry data_020c94a8[2] = {{80, 0x10}, {20, 0x11}};
extern const FlowerHybridEntry data_020c916c[1] = {{100, 0x7}};
void (*data_020da2a8[1])(EffectSplEmitter *) = {
    FlowerFx_InitByColor,
};
extern const u32 sDropUnitOrder[8][9] = {
    0x00000000, 0x00000008, 0x00000006, 0x00000007, 0x00000004, 0x00000005, 0x00000002, 0x00000003,
    0x00000001, 0x00000000, 0x00000007, 0x00000008, 0x00000005, 0x00000006, 0x00000003, 0x00000004,
    0x00000001, 0x00000002, 0x00000000, 0x00000005, 0x00000007, 0x00000003, 0x00000008, 0x00000001,
    0x00000006, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x00000005, 0x00000001, 0x00000007,
    0x00000002, 0x00000008, 0x00000004, 0x00000006, 0x00000000, 0x00000001, 0x00000003, 0x00000002,
    0x00000005, 0x00000004, 0x00000007, 0x00000006, 0x00000008, 0x00000000, 0x00000002, 0x00000001,
    0x00000004, 0x00000003, 0x00000006, 0x00000005, 0x00000008, 0x00000007, 0x00000000, 0x00000004,
    0x00000002, 0x00000006, 0x00000001, 0x00000008, 0x00000003, 0x00000007, 0x00000005, 0x00000000,
    0x00000006, 0x00000008, 0x00000004, 0x00000007, 0x00000002, 0x00000005, 0x00000001, 0x00000003,
};
extern const FlowerHybridEntry data_020c9558[2] = {{50, 0x13}, {50, 0x18}};
extern const FlowerHybridEntry data_020c97e4[3] = {{35, 0x13}, {35, 0x12}, {30, 0x15}};
extern const FlowerHybridEntry data_020c91a4[1] = {{100, 0x9}};
extern const FlowerHybridEntry data_020c95a8[2] = {{80, 0xd}, {20, 0x11}};
extern const u16 data_020c90e4[2] = {0x1d6c, 0x0};
extern const FlowerHybridEntry data_020c95d8[2] = {{50, 0xc}, {50, 0x10}};
extern const FlowerHybridEntry data_020c9628[2] = {{50, 0x6}, {50, 0xb}};
extern const FlowerHybridList data_020c9e20[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {1, data_020c9144}, {2, data_020c9268},
    {2, data_020c9538}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridList data_020c9ef8[9] = {
    {0, NULL}, {0, NULL}, {2, data_020c93a8},
    {2, data_020c94f8}, {3, data_020c97e4}, {2, data_020c9528},
    {2, data_020c9548}, {2, data_020c9558}, {2, data_020c9588},
};
extern const FlowerHybridEntry data_020c9668[2] = {{50, 0x7}, {50, 0xb}};
extern const FlowerHybridEntry data_020c9814[3] = {{30, 0x7}, {30, 0x8}, {40, 0xa}};
extern const u32 sTreeDropSlots[3] = {
    0x00000001, 0x00000002, 0x00000003,
};
extern const u8 sDropUnitOffsets[12] = {
    0x88, 0x87, 0x77, 0x97, 0x78, 0x98, 0x79, 0x99, 0x89, 0x00, 0x00, 0x00,
};
extern const u32 sBbsEventMsgs[30] = {
    0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x0000001c,
    0x0000001d, 0x00000007, 0x00000008, 0x00000009, 0x0000000a, 0x0000000b, 0x0000000c, 0x0000000d,
    0x0000000e, 0x0000000f, 0x00000010, 0x00000011, 0x00000012, 0x00000013, 0x00000014, 0x00000015,
    0x00000016, 0x00000017, 0x00000018, 0x00000019, 0x0000001a, 0x0000001b,
};
extern const FlowerHybridEntry data_020c92e8[2] = {{50, 0x8}, {50, 0xa}};
extern const FlowerHybridEntry data_020c970c[3] = {{35, 0xd}, {35, 0xe}, {30, 0x10}};
void (*data_020da2a4[1])(EffectSplEmitter *) = {
    FlowerFx_InitBySpecies,
};
extern const FlowerHybridEntry data_020c92b8[2] = {{50, 0x9}, {50, 0xb}};
extern const FlowerHybridEntry data_020c9724[3] = {{40, 0x17}, {40, 0x18}, {20, 0x19}};
extern const FlowerHybridEntry data_020c9268[2] = {{50, 0x3}, {50, 0x4}};
extern const FlowerHybridEntry data_020c9288[2] = {{80, 0x15}, {20, 0x19}};
extern const FlowerHybridEntry data_020c92c8[2] = {{80, 0x15}, {20, 0x17}};
extern const FlowerHybridList data_020c99a0[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {1, data_020c913c}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c9328[2] = {{50, 0x2}, {50, 0x3}};
extern const FlowerHybridEntry data_020c9348[2] = {{50, 0xd}, {50, 0x10}};
extern const FlowerHybridEntry data_020c917c[1] = {{100, 0x10}};
extern const FlowerHybridEntry data_020c9548[2] = {{50, 0x13}, {50, 0x17}};
extern const FlowerHybridEntry data_020c9388[2] = {{50, 0xe}, {50, 0x10}};
extern const FlowerHybridEntry data_020c918c[1] = {{100, 0x17}};
extern const FlowerHybridEntry data_020c93d8[2] = {{80, 0xf}, {20, 0x11}};
extern const FlowerHybridList data_020c99e8[9] = {
    {0, NULL}, {1, data_020c9124}, {3, data_020c96f4},
    {2, data_020c92a8}, {2, data_020c95e8}, {2, data_020c95d8},
    {2, data_020c95c8}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c93f8[2] = {{50, 0x0}, {50, 0x5}};
extern const u8 sMoneyRockBags[20] = {
    0x92, 0x14, 0x92, 0x14, 0x93, 0x14, 0x95, 0x14, 0x99, 0x14, 0x9b, 0x14, 0x9c, 0x14, 0x9e, 0x14,
    0xa2, 0x14, 0x00, 0x00,
};
extern const FlowerHybridList data_020c9b50[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {1, data_020c9194}, {2, data_020c94e8},
    {2, data_020c93d8}, {0, NULL}, {0, NULL},
};
FieldActionState sFieldActions;
extern const FlowerHybridList data_020c9c28[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {3, data_020c973c},
};
extern const FlowerHybridEntry data_020c94d8[2] = {{80, 0x0}, {20, 0x5}};
extern const FlowerHybridEntry data_020c9408[2] = {{50, 0x1}, {50, 0x3}};
extern const u16 *data_020da2c8[7] = {data_020c9218, data_020c9228, data_020c9238, data_020c96e0, data_020c9110, data_020c9110, data_020c9110};
extern const FlowerHybridEntry data_020c9598[2] = {{50, 0x6}, {50, 0x9}};
extern const FlowerHybridEntry data_020c95c8[2] = {{80, 0xc}, {20, 0x11}};
extern const FlowerHybridList data_020c9e68[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {1, data_020c91b4},
    {2, data_020c9578}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridList data_020c9f40[9] = {
    {0, NULL}, {2, data_020c95b8}, {2, data_020c95f8},
    {2, data_020c9518}, {2, data_020c9598}, {2, data_020c9618},
    {2, data_020c9628}, {0, NULL}, {0, NULL},
};
extern const FlowerHybridEntry data_020c9678[2] = {{70, 0x8}, {30, 0x9}};
extern const FlowerHybridEntry data_020c92f8[2] = {{50, 0x16}, {50, 0x18}};
u8 data_020da2a0[4] = {
    0x3d, 0x3f, 0x60, 0x00,
};
extern const u16 data_020c90f8[2] = {0x21ad, 0x0};
extern const FlowerHybridList data_020c9958[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {1, data_020c9174}, {2, data_020c9248},
    {2, data_020c92c8}, {2, data_020c9258}, {2, data_020c9288},
};
extern const u16 data_020c90fc[2] = {0x1a8a, 0x0};
extern const u16 data_020c9100[2] = {0x1e89, 0x0};
extern const u32 sFlowerParchChanceExtraDay[27] = {
    0x0000000f, 0x0000000f, 0x0000000f, 0x0000001e, 0x0000001e, 0x00000032, 0x0000000f, 0x0000000f,
    0x0000000f, 0x0000001e, 0x0000001e, 0x00000032, 0x0000000f, 0x0000000f, 0x0000000f, 0x0000001e,
    0x0000001e, 0x00000032, 0x0000000f, 0x0000000f, 0x0000000f, 0x0000001e, 0x0000001e, 0x00000032,
    0x00000032, 0x0000003c, 0x00000032,
};
extern const FlowerHybridEntry data_020c9398[2] = {{80, 0xe}, {20, 0x11}};
extern const FlowerHybridEntry data_020c9154[1] = {{100, 0xe}};
extern const FlowerHybridList data_020c9a30[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {1, data_020c919c},
    {2, data_020c9298}, {2, data_020c92f8}, {2, data_020c9308},
};
extern const FlowerHybridEntry data_020c9358[2] = {{80, 0x12}, {20, 0x18}};
extern const FlowerHybridEntry data_020c95e8[2] = {{50, 0xc}, {50, 0xf}};
extern const FlowerHybridEntry data_020c9528[2] = {{50, 0x13}, {50, 0x16}};
extern const u16 data_020c90e8[2] = {0x21ae, 0x0};
extern const FlowerHybridList data_020c9eb0[9] = {
    {0, NULL}, {0, NULL}, {0, NULL},
    {0, NULL}, {0, NULL}, {0, NULL},
    {1, data_020c9134}, {0, NULL}, {0, NULL},
};
extern const s32 sTreeChopHits[4] = {
    0, 1, 2, 2,
};
extern const u16 data_020c9218[8] = {0x21d0, 0x5f, 0x67fe, 0x23e, 0x5ddf, 0x6c97, 0x308c, 0x0};
extern const u8 data_020c910c[4] = {
    0x89, 0x78, 0x98, 0x00,
};
extern const FlowerHybridEntry data_020c973c[3] = {{20, 0x19}, {40, 0x18}, {40, 0x17}};
extern const u16 data_020c912c[4] = {
    0x00e8, 0x00ed, 0x00f2, 0x00f7,
};
extern const FlowerHybridEntry data_020c93e8[2] = {{80, 0x18}, {20, 0x19}};
extern const FlowerHybridEntry data_020c95b8[2] = {{80, 0x6}, {20, 0xb}};
extern const s32 data_020c97cc[3][2] = {
    0, 2, 1, 3, 2, 0,
};
extern const u16 data_020c96e0[10] = {0x2233, 0x1b, 0x5bdf, 0x33f, 0x61ff, 0x21f, 0x7cb9, 0x280a, 0x7e43, 0x7fff};
LitCedarPos sLitCedars[24];
extern const FlowerHybridEntry data_020c976c[3] = {{35, 0x12}, {35, 0x13}, {30, 0x15}};
extern const FlowerHybridEntry data_020c913c[1] = {{100, 0xb}};
extern const FlowerHybridEntry data_020c9248[2] = {{50, 0x15}, {50, 0x16}};
extern const s8 sWaterOffsetsZ[8] = {
    0, 0, 0, -1, 1, 0, 0, 0,
};
extern const FlowerHybridList data_020c9a78[9] = {
    {0, NULL}, {0, NULL}, {2, data_020c95a8},
    {3, data_020c970c}, {2, data_020c9338}, {2, data_020c9348},
    {2, data_020c9568}, {0, NULL}, {0, NULL},
};
extern const u16 sTreeDropFruit[6] = {
    0x1518, 0x1519, 0x151a, 0x151b, 0x151c, 0x0000,
};
}
}

namespace nO {
extern "C" void TownEval_PickAdvice(TownEvalAdvice *out, TownEvalAcrePos *in, TownEvalAcre *res) {
    out->acreX = in->acreX;
    out->acreZ = in->acreZ;
    if (out->acreX < 0) {
        out->reason = -1;
    } else {
        s32 sc[6];
        s32 *q, i;
        q = sc;
        for (i = 0; i < 6; i++) *q++ = 0;
        s32 n = res->trees[0][0] + res->trees[1][0] + res->trees[0][1] + res->trees[1][1];
        if (n > 15) {
            sc[0] = (n - 15) * 2;
        } else if (n < 12) {
            sc[1] = (12 - n) * 2;
        }
        s32 a = res->weeds;
        if (a >= 3) sc[2] = a - 2;
        s32 b = res->groundItems;
        if (b >= 3) sc[3] = b - 2;
        sc[4] = res->garbage * 10;
        if (((s32)(res->flags << 31) >> 31) != 0) sc[5] = 50;
        s32 best = -1;
        s32 bi = -1;
        q = sc;
        for (i = 0; i < 6; q++, i++) {
            if (*q > best) {
                best = *q;
                bi = i;
            }
        }
        out->reason = bi;
    }
}
}

namespace nO {
extern "C" void TownEval_CalcScore(TownEvalAcre *out) {
    s32 n = out->trees[0][0] + out->trees[1][0] + out->trees[0][1] + out->trees[1][1];
    out->score = 100;
    if (n > 15) {
        out->score += (n - 15) * -2;
    } else if (n < 12) {
        out->score += (12 - n) * -2;
    }
    if (out->weeds >= 3) out->score -= out->weeds - 2;
    if (out->groundItems >= 3) out->score -= out->groundItems - 2;
    out->score += out->garbage * -10;
    if (((s32)(out->flags << 31) >> 31) == 1) out->score -= 50;
    if (out->flowers >= 3) out->score += out->flowers - 2;
}
}

namespace nO {
extern "C" void TownEval_RemoveObject(TownEvalAcre *out, BlockMap *map, s32 x, s32 z, s32 n) {
    s32 a = n % 16;
    s32 b = n / 16;
    s32 ox, oz;
    FieldUnit_FromBlockUnit(&ox, &oz, x, z, a, b);
    BlockMap_PlaceItem(map, ox, oz, 0xfff1, 0);
}
}

namespace nO {
extern "C" void TownEval_EvaluateAndCleanAcre(TownEvalAcre *out, BlockMap *map, s32 x, s32 z) {
    s32 j, i;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            out->trees[j][i] = 0;
        }
    }
    out->weeds = 0;
    out->groundItems = 0;
    out->garbage = 0;
    out->flowers = 0;
    out->flags = 0;
    out->rafflesiaPos.x = -1;
    out->rafflesiaPos.y = -1;
    out->rocks = 0;
    out->litCedars = 0;
    out->shells = 0;
    out->buriedFossils = 0;
    u16 *p = BlockMap_GetItemPtr(map, x, z, 0, 0, 0);
    i = 0;
    BOOL fl[3];
    fl[0] = FALSE;
    fl[1] = FALSE;
    fl[2] = FALSE;
    for (; i < 0x100; p++, i++) {
        u16 v = *p;
        switch ((v & 0xf000) >> 12) {
        case 0:
            if ((v >= 0x26 && v <= 0x2a) || (v >= 0x5d && v <= 0x61) || (v >= 0x2f && v <= 0x56) ||
                (v >= 0x57 && v <= 0x5b) || (v >= 0x66 && v <= 0x68) || v == 0x69 || (v >= 0x6a && v <= 0x6c) ||
                v == 0x6d || (v >= 0xc8 && v <= 0xcf)) {
                out->trees[(i & 0xf) >> 3][i >> 7]++;
                switch (*p - 0x66) {
                case 0:
                case 4:
                    out->flags |= 0x10;
                    break;
                case 1:
                case 5:
                    out->flags |= 8;
                    break;
                case 2:
                case 6:
                    out->flags |= 0x20;
                    break;
                case 3:
                    break;
                }
                BOOL x1 = fl[0];
                u32 t = *p;
                if (t >= 0x26 && t <= 0x2a) x1 = TRUE;
                if (x1 || (t >= 0x66 && t <= 0x68)) {
                    out->flags |= 0x800;
                } else if (t >= 0x5d && t <= 0x61 && (Item_IsTreeStage3(p) || Item_IsTreeGrown(p))) {
                    out->flags |= 0x1000;
                } else if (*p == 0x6d) {
                    out->litCedars++;
                }
            } else if (v == 0x25 || v == 0x5c || v == 0xc7 || (v >= 0x6e && v <= 0x73) || (v >= 0x74 && v <= 0x79) ||
                       (v >= 0x7a && v <= 0x7f) || (v >= 0x80 && v <= 0x87) || v == 0x89) {
                TownEval_RemoveObject(out, map, x, z, i);
            } else if (v >= 0x21 && v <= 0x24) {
                out->weeds++;
            } else {
                BOOL x2 = fl[1];
                if (v <= 0x1d) x2 = TRUE;
                if (x2) {
                    switch (v - 0x1a) {
                    case 1:
                        out->rafflesiaPos.x = i % 16;
                        out->rafflesiaPos.y = i / 16;
                        out->flags = (out->flags & ~1) | 1;
                        break;
                    case 0:
                        out->flags |= 2;
                        out->flowers++;
                        break;
                    case 2:
                    case 3:
                        out->flowers++;
                        break;
                    default:
                        out->flags |= 4;
                        out->flowers++;
                        break;
                    }
                } else if ((v >= 0x8a && v <= 0x8f) || (v >= 0x90 && v <= 0x95) || (v >= 0x96 && v <= 0x9b) ||
                           (v >= 0x9c && v <= 0xa3) || v == 0xa5) {
                    out->flags |= 4;
                    out->flowers++;
                } else if (v >= 0xe3 && v <= 0xe7) {
                    out->rocks++;
                } else if (v >= 0xe8 && v <= 0xfb) {
                    s32 k = (v - 0xe8) / 5;
                    out->bits.moneyRocks |= 1 << k;
                }
            }
            break;
        case 1:
            if (v == 0x1548) {
                out->flags |= 0x400;
            } else if (v >= 0x1320 && v <= 0x1322) {
                out->garbage++;
            } else if (v == 0x1568) {
                TownEval_RemoveObject(out, map, x, z, i);
            } else {
                s32 r = i % 16;
                s32 q = i / 16;
                if (!BlockMap_IsBuried(map, x, z, r, q)) {
                    BOOL x3 = fl[2];
                    u32 t = *p;
                    if (t >= 0x1554 && t <= 0x155c) x3 = TRUE;
                    if (x3) {
                        out->shells++;
                    } else if (t >= 0x154a && t <= 0x1553) {
                        out->flags |= 0x4000;
                    } else if (t >= 0x1518 && t <= 0x151c) {
                    } else if (t >= 0x1542 && t <= 0x1546) {
                    } else {
                        out->groundItems++;
                    }
                } else {
                    switch (*p) {
                    case 0x1549:
                        out->buriedFossils++;
                        break;
                    case 0x1566:
                        out->flags |= 0x2000;
                        break;
                    }
                }
            }
            break;
        case 2: {
            s32 r = i % 16;
            s32 q = i / 16;
            if (!BlockMap_IsBuried(map, x, z, r, q)) out->groundItems++;
            break;
        }
        case 3:
        case 4:
            out->groundItems++;
            break;
        }
    }
    TownEval_CalcScore(out);
}
}

namespace nO {
extern "C" void TownEval_EvaluateAcre(TownEvalAcre *out, BlockMap *map, s32 x, s32 z) {
    s32 j, i;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            out->trees[j][i] = 0;
        }
    }
    out->weeds = 0;
    out->groundItems = 0;
    out->garbage = 0;
    out->flowers = 0;
    out->flags = 0;
    out->rafflesiaPos.x = -1;
    out->rafflesiaPos.y = -1;
    out->rocks = 0;
    out->litCedars = 0;
    out->shells = 0;
    out->buriedFossils = 0;
    u16 *p = BlockMap_GetItemPtr(map, x, z, 0, 0, 0);
    i = 0;
    BOOL fl[3];
    fl[0] = FALSE;
    fl[1] = FALSE;
    fl[2] = FALSE;
    for (; i < 0x100; p++, i++) {
        u16 v = *p;
        switch ((v & 0xf000) >> 12) {
        case 0:
            if ((v >= 0x26 && v <= 0x2a) || (v >= 0x5d && v <= 0x61) || (v >= 0x2f && v <= 0x56) ||
                (v >= 0x57 && v <= 0x5b) || (v >= 0x66 && v <= 0x68) || v == 0x69 || (v >= 0x6a && v <= 0x6c) ||
                v == 0x6d || (v >= 0xc8 && v <= 0xcf)) {
                out->trees[(i & 0xf) >> 3][i >> 7]++;
                switch (*p - 0x66) {
                case 0:
                case 4:
                    out->flags |= 0x10;
                    break;
                case 1:
                case 5:
                    out->flags |= 8;
                    break;
                case 2:
                case 6:
                    out->flags |= 0x20;
                    break;
                case 3:
                    break;
                }
                BOOL x1 = fl[0];
                u32 t = *p;
                if (t >= 0x26 && t <= 0x2a) x1 = TRUE;
                if (x1 || (t >= 0x66 && t <= 0x68)) {
                    out->flags |= 0x800;
                } else if (t >= 0x5d && t <= 0x61 && Item_IsTreeGrown(p)) {
                    out->flags |= 0x1000;
                } else if (*p == 0x6d) {
                    out->litCedars++;
                }
            } else if (v >= 0x21 && v <= 0x24) {
                out->weeds++;
            } else {
                BOOL x2 = fl[1];
                if (v <= 0x1d) x2 = TRUE;
                if (x2) {
                    switch (v - 0x1a) {
                    case 1:
                        out->rafflesiaPos.x = i % 16;
                        out->rafflesiaPos.y = i / 16;
                        out->flags = (out->flags & ~1) | 1;
                        break;
                    case 0:
                        out->flags |= 2;
                        out->flowers++;
                        break;
                    case 2:
                    case 3:
                        out->flowers++;
                        break;
                    default:
                        out->flags |= 4;
                        out->flowers++;
                        break;
                    }
                } else if ((v >= 0x8a && v <= 0x8f) || (v >= 0x90 && v <= 0x95) || (v >= 0x96 && v <= 0x9b) ||
                           (v >= 0x9c && v <= 0xa3) || v == 0xa5 || (v >= 0x6e && v <= 0x73) ||
                           (v >= 0x74 && v <= 0x79) || (v >= 0x7a && v <= 0x7f) || (v >= 0x80 && v <= 0x87)) {
                    out->flags |= 4;
                    out->flowers++;
                }
            }
            break;
        case 1:
            if (v >= 0x1320 && v <= 0x1322) {
                out->garbage++;
            } else {
                s32 r = i % 16;
                s32 q = i / 16;
                if (!BlockMap_IsBuried(map, x, z, r, q)) {
                    BOOL x3 = fl[2];
                    u32 t = *p;
                    if (t >= 0x1554 && t <= 0x155c) x3 = TRUE;
                    if (x3) {
                        out->shells++;
                    } else if (t >= 0x154a && t <= 0x1553) {
                        out->flags |= 0x4000;
                    } else if (t >= 0x1518 && t <= 0x151c) {
                    } else if (t >= 0x1542 && t <= 0x1546) {
                    } else {
                        out->groundItems++;
                    }
                } else {
                    switch (*p) {
                    case 0x1549:
                        out->buriedFossils++;
                        break;
                    case 0x1566:
                        out->flags |= 0x2000;
                        break;
                    }
                }
            }
            break;
        case 2: {
            s32 r = i % 16;
            s32 q = i / 16;
            if (!BlockMap_IsBuried(map, x, z, r, q)) out->groundItems++;
            break;
        }
        case 3:
        case 4:
            out->groundItems++;
            break;
        }
    }
    TownEval_CalcScore(out);
}
}

namespace nO {
extern "C" s32 TownEval_GetScoreRank(TownEval *a, s32 v) {
    if (v < 0x4b) return 0;
    if (v < 0x50) return 1;
    if (v < 0x5f) return 2;
    if (v < 0x65) return 3;
    return 4;
}
}

namespace nN {
extern "C" void TownEval_EvaluateAndClean(TownEval *a, TownBlockMap *b, s32 w, s32 h) {
    s32 i;
    s32 v;
    s32 by;
    s32 bx;
    s32 best;
    s32 j;
    s32 k;
    by = -1;
    bx = -1;
    best = 0;
    a->jacobsAcres = 0;
    a->flowers = 0;
    a->flowerAcres = 0;
    a->weeds = 0;
    a->trees = 0;
    a->furnitureTreeAcres = 0;
    a->buriedFossils = 0;
    a->rafflesiaBlock.x = by;
    a->rafflesiaBlock.y = by;
    a->rafflesiaUnit.x = by;
    a->rafflesiaUnit.y = by;
    
    a->rocks = 0;
    *(u8 *)&a->townFlags = 0;
    for (k = 0; k < 5; k++) {
        gTownEval.counts[k] = 0;
    }
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            TownEvalAcre *cell = &a->acres[i][j];
            TownEval_EvaluateAndCleanAcre(cell, b, i + 1, j + 1);
            if (cell->bits.jacobsLadder) a->jacobsAcres++;
            if (cell->bits.hasFlowers) a->flowerAcres++;
            if (cell->bits.furnitureTree) a->furnitureTreeAcres++;
            if (cell->buriedFossils) a->buriedFossils += cell->buriedFossils;
            a->rocks += cell->rocks;
            a->townBits.moneyRocks = a->townBits.moneyRocks | cell->bits.moneyRocks;
            a->townBits.coconut = a->townBits.coconut | cell->bits.coconut;
            a->townBits.pitfallSeed = a->townBits.pitfallSeed | cell->bits.pitfallSeed;
            a->trees += cell->trees[0][0] + cell->trees[1][0] + cell->trees[0][1] + cell->trees[1][1];
            a->flowers += cell->flowers;
            a->weeds += cell->weeds;
            s32 t = cell->garbage;
            if (t > 0) a->townBits.hasGarbage = 1;
            a->townBits.spoiledTurnips = a->townBits.spoiledTurnips | cell->bits.spoiledTurnips;
            v = cell->score;
            gTownEval.counts[TownEval_GetScoreRank(a, v)]++;
            if (v < best) {
                by = i;
                bx = j;
                best = v;
            }
            if (cell->bits.rafflesia) {
                a->rafflesiaBlock.x = i;
                a->rafflesiaBlock.y = j;
                { s32 yy = cell->rafflesiaPos.y; s32 xx = cell->rafflesiaPos.x; a->rafflesiaUnit.x = xx; a->rafflesiaUnit.y = yy; }
            }
        }
    }
    TownEvalAcrePos pr;
    pr.acreX = by;
    pr.acreZ = bx;
    // no acre below the best score (0 here): by = bx = -1, and PickAdvice doesn't look at the acre (the DS code passed
    // the address of acres[-1][-1], outside the object)
    TownEval_PickAdvice(&a->advice, &pr, by < 0 ? 0 : &a->acres[by][bx]);
    a->rank = TownEval_GetTownRank(a, nZ::gTownEval.counts, w, h);
}
}

namespace nN {
extern "C" s32 TownEval_Evaluate(TownEval *a, TownBlockMap *b) {
    s32 w, h;
    s32 best;
    s32 v;
    s32 by, bx;
    s32 i, j, k;
    by = -1;
    bx = -1;
    best = 10000;
    a->jacobsAcres = 0;
    a->flowers = 0;
    a->flowerAcres = 0;
    a->weeds = 0;
    a->trees = 0;
    a->furnitureTreeAcres = 0;
    a->buriedFossils = 0;
    a->rafflesiaBlock.x = by;
    a->rafflesiaBlock.y = by;
    a->rafflesiaUnit.x = by;
    a->rafflesiaUnit.y = by;
    
    a->rocks = 0;
    *(u8 *)&a->townFlags = 0;
    for (k = 0; k < 5; k++) {
        gTownEval.counts[k] = 0;
    }
    w = b->width - 2;
    h = b->height - 2;
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            TownEvalAcre *cell = &a->acres[i][j];
            TownEval_EvaluateAcre(cell, b, i + 1, j + 1);
            if (cell->bits.jacobsLadder) a->jacobsAcres++;
            if (cell->bits.hasFlowers) a->flowerAcres++;
            if (cell->bits.furnitureTree) a->furnitureTreeAcres++;
            if (cell->buriedFossils) a->buriedFossils += cell->buriedFossils;
            a->townBits.pitfallSeed = a->townBits.pitfallSeed | cell->bits.pitfallSeed;
            a->trees += cell->trees[0][0] + cell->trees[1][0] + cell->trees[0][1] + cell->trees[1][1];
            a->flowers += cell->flowers;
            a->weeds += cell->weeds;
            s32 t = cell->garbage;
            if (t > 0) a->townBits.hasGarbage = 1;
            a->townBits.spoiledTurnips = a->townBits.spoiledTurnips | cell->bits.spoiledTurnips;
            v = cell->score;
            gTownEval.counts[TownEval_GetScoreRank(a, v)]++;
            if (v < best) {
                by = i;
                bx = j;
                best = v;
            }
            if (cell->bits.rafflesia) {
                a->rafflesiaBlock.x = i;
                a->rafflesiaBlock.y = j;
                { s32 yy = cell->rafflesiaPos.y; s32 xx = cell->rafflesiaPos.x; a->rafflesiaUnit.x = xx; a->rafflesiaUnit.y = yy; }
            }
        }
    }
    TownEvalAcrePos pr;
    pr.acreX = by;
    pr.acreZ = bx;
    TownEval_PickAdvice(&a->advice, &pr, &a->acres[by][bx]);
    s32 rank = TownEval_GetTownRank(a, nZ::gTownEval.counts, w, h);
    a->rank = rank;
    return rank;
}
}

namespace nN {
extern "C" s32 TownEval_GetTownRank(void *a, s32 *p, s32 w, s32 h) {
    if (p[0] == w * h) return 0;
    if (p[0] > 0 || p[1] > 0) return 1;
    if (p[2] > 0) return 2;
    if (p[4] < 8) return 3;
    return 4;
}
}

namespace nN {
extern "C" void TownEval_EvaluateAcreAt(TownEval *a, void *b, s32 i, s32 j) {
    TownEval_EvaluateAcre(&a->acres[i][j], (TownBlockMap *)b, i + 1, j + 1);
}
}

namespace nN {
extern "C" void Scene_SetUnitItem(s32 x, s32 y, u32 item, s32 flag) {
    if (Scene_InTown() || Scene_InTownUnk31()) {
        Field_SetUnitItem(x, y, item, flag);
    } else {
        Room_SetItemAtUnit(x, y, item, 0);
    }
}
}

namespace nN {
extern "C" void Area_PlaceItem(u32 a, s32 x, s32 y, u32 v, s32 flag) {
    BlockMap *q;
    if (SceneId_IsTown(a) || SceneId_IsTownUnk31(a)) {
        q = TownBlockMap_Get();
        if (q != NULL) {
            u16 t = 0xfff1;
            t = v;
            BlockMap_SetItemAtUnit(q, &t, x, y, 0);
            if (flag) {
                BlockMap_SetBuriedAtUnit(q, x, y);
            } else {
                BlockMap_ClearBuriedAtUnit(q, x, y);
            }
        }
    } else {
        q = HouseRoomMaps_GetForScene(a);
        if (q != NULL) {
            u16 t = 0xfff1;
            t = v;
            BlockMap_SetItemAtUnit(q, &t, x, y, 0);
        }
    }
}
}

namespace nN {
extern "C" void BlockMap_PlaceItem(BlockMap *a, s32 x, s32 y, u32 v, s32 flag) {
    u16 t = 0xfff1;
    t = v;
    BlockMap_SetItemAtUnit(a, &t, x, y, 0);
    if (flag) {
        BlockMap_SetBuriedAtUnit(a, x, y);
    } else {
        BlockMap_ClearBuriedAtUnit(a, x, y);
    }
}
}

namespace nN {
extern "C" void BlockMap_PlaceItemAt(TownUpdater *a0, BlockMap *a1, s32 a2, s32 a3, s32 a4, s32 a5, u16 a6, s32 a7) {
    u16 t = 0xfff1;
    t = a6;
    u32 l5 = a5, l4 = a4;
    BlockMap_SetItem(a1, &t, a2, a3, l4, l5, 0);
    if (a7) {
        BlockMap_SetBuried(a1, a2, a3, l4, l5);
    } else {
        BlockMap_ClearBuried(a1, a2, a3, l4, l5);
    }
}
}

namespace nN {
extern "C" void Town_WitherSapling(BlockMap *a, u32 v, s32 x, s32 y) {
    u32 nv = 0x25;
    switch (v) {
    case 0x26:
        break;
    case 0x5d:
        nv = 0x5c;
        break;
    case 0xc8:
        nv = 0xc7;
        break;
    }
    s32 bx = x >> 4;
    s32 by = y >> 4;
    u8 *p = &nZ::gTownEval.acres[bx][by].trees[0][0];
    p += ((x - (bx << 4)) >> 3) * 2;
    s32 yi = (y - (by << 4)) >> 3;
    p[yi]--;
    BlockMap_PlaceItem(a, x, y, nv, 0);
}
}

namespace nN {
extern "C" void Town_GrowTree(BlockMap *a, u16 *p, s32 x, s32 y) {
    u16 v = *p;
    if (Item_IsFruitTreeLastNoFruit(p)) {
        v = v - 3;
    } else if (!Item_IsTreeGrown(p)) {
        v = v + 1;
    }
    BlockMap_PlaceItem(a, x, y, v, 0);
}
}

namespace nN {
extern "C" BOOL Town_IsSaplingBlocker(u16 *p) {
    BOOL result = FALSE;
    if (Item_IsBuildingOrOccupied(p)) {
        result = TRUE;
    } else {
        BOOL h = TRUE;
        BOOL a = TRUE;
        BOOL b = TRUE;
        BOOL c = TRUE;
        BOOL d = TRUE;
        BOOL e = TRUE;
        BOOL f = TRUE;
        BOOL g = TRUE;
        BOOL i = FALSE;
        u16 v = *p;
        if (v >= 0x26 && v <= 0x2a) i = TRUE;
        if (!i) {
            if (!(v >= 0x5d && v <= 0x61)) g = FALSE;
        }
        if (!g) {
            if (!(v >= 0x2f && v <= 0x56)) f = FALSE;
        }
        if (!f) {
            if (!(v >= 0x57 && v <= 0x5b)) e = FALSE;
        }
        if (!e) {
            if (!(v >= 0x66 && v <= 0x68)) d = FALSE;
        }
        if (!d) {
            if (v != 0x69) c = FALSE;
        }
        if (!c) {
            if (!(v >= 0x6a && v <= 0x6c)) b = FALSE;
        }
        if (!b) {
            if (v != 0x6d) a = FALSE;
        }
        if (!a) {
            if (!(v >= 0xc8 && v <= 0xcf)) h = FALSE;
        }
        if (h && !Item_IsTreeStage0(p)) {
            result = TRUE;
        } else {
            BOOL d4 = TRUE, d3 = TRUE, d2 = TRUE, d1 = FALSE;
            u16 w = *p;
            if (w >= 0x2b && w <= 0x2e) d1 = TRUE;
            if (!d1) {
                if (!(w >= 0xff && w <= 0x102)) d2 = FALSE;
            }
            if (!d2) {
                if (!(w >= 0x62 && w <= 0x65)) d3 = FALSE;
            }
            if (!d3) {
                if (!(w >= 0xd0 && w <= 0xd3)) d4 = FALSE;
            }
            if (d4) result = TRUE;
        }
    }
    return result;
}
}

namespace nN {
extern "C" BOOL Town_IsSaplingBlockerOrRock(u16 *p) {
    BOOL r = FALSE;
    if (Town_IsSaplingBlocker(p)) {
        r = TRUE;
    } else {
        BOOL b = TRUE, a = r;
        u16 v = *p;
        if (v >= 0xe3 && v <= 0xe7) a = TRUE;
        if (!a) {
            if (!(v >= 0xe8 && v <= 0xfb)) b = FALSE;
        }
        if (b) r = TRUE;
    }
    return r;
}
}

namespace nN {
extern "C" BOOL Town_IsSaplingBlockerAt(BlockMap *a, Unk_02048cc4_Pos pos, Unk_02048cc4_Pos centre, Unk_02048cc4_Pos size) {
    BOOL r = FALSE;
    s32 x = pos.x;
    s32 y = pos.y;
    s32 bx = x >> 4;
    s32 by = y >> 4;
    u16 *p = BlockMap_GetItemPtr(a, bx, by, x - (bx << 4), y - (by << 4), 0);
    if (p != NULL) {
        r = Town_IsSaplingBlockerOrRock(p);
    }
    return r;
}
}

namespace nN {
extern "C" s32 Town_WitherSaplingAt(BlockMap *a, Unk_02048cc4_Pos pos, Unk_02048cc4_Pos centre, Unk_02048cc4_Pos size) {
    s32 x = pos.x;
    s32 y = pos.y;
    s32 bx = x >> 4;
    s32 by = y >> 4;
    u16 *p = BlockMap_GetItemPtr(a, bx, by, x - (bx << 4), y - (by << 4), 0);
    if (p != NULL) {
        if (Item_IsTreeStage0(p)) {
            Town_WitherSapling(a, *p, pos.x, pos.y);
        }
    }
    return 0;
}
}

namespace nN {
extern "C" s32 Flower_GetSpecies(u16 *p) {
    s32 r = 7;
    BOOL a = FALSE;
    u16 v = *p;
    if (v <= 5) a = TRUE;
    if (a || (v >= 0x8a && v <= 0x8f) || (v >= 0x6e && v <= 0x73)) {
        r = 0;
    } else if ((v >= 6 && v <= 0xb) || (v >= 0x90 && v <= 0x95) || (v >= 0x74 && v <= 0x79)) {
        r = 1;
    } else if ((v >= 0xc && v <= 0x11) || (v >= 0x96 && v <= 0x9b) || (v >= 0x7a && v <= 0x7f)) {
        r = 2;
    } else if ((v >= 0x12 && v <= 0x19) || v == 0x1c || (v >= 0x9c && v <= 0xa3) || v == 0xa5 || (v >= 0x80 && v <= 0x87)) {
        r = 3;
    } else {
        switch (v) {
        case 0x1a:
        case 0x88:
        case 0xa4:
            r = 4;
            break;
        case 0x1d:
            r = 5;
            break;
        case 0x1e:
            r = 6;
            break;
        }
    }
    return r;
}
}

namespace nN {
extern "C" s32 Flower_GetColor(u16 *p) {
    s32 r = 1;
    BOOL d = TRUE, c = TRUE, b = TRUE, a = FALSE;
    u16 v = *p;
    if (v <= 5) a = TRUE;
    if (!a) {
        if (!(v >= 6 && v <= 0xb)) b = FALSE;
    }
    if (!b) {
        if (!(v >= 0xc && v <= 0x11)) c = FALSE;
    }
    if (!c) {
        if (!((v >= 0x12 && v <= 0x19) || v == 0x1c)) d = FALSE;
    }
    if (d) {
        switch (Flower_GetSpecies(p)) {
        case 0:
            r = *p + 1;
            break;
        case 1:
            r = *p - 5;
            break;
        case 2:
            r = *p - 0xb;
            break;
        case 3:
            if (*p == 0x1c) r = 9;
            else r = *p - 0x11;
            break;
        }
    } else if ((v >= 0x8a && v <= 0x8f) || (v >= 0x90 && v <= 0x95) || (v >= 0x96 && v <= 0x9b) || (v >= 0x9c && v <= 0xa3) || v == 0xa5) {
        switch (Flower_GetSpecies(p)) {
        case 0:
            r = *p - 0x89;
            break;
        case 1:
            r = *p - 0x8f;
            break;
        case 2:
            r = *p - 0x95;
            break;
        case 3:
            if (*p == 0xa5) r = 7;
            else r = *p - 0x9b;
            break;
        }
    } else if ((v >= 0x6e && v <= 0x73) || (v >= 0x74 && v <= 0x79) || (v >= 0x7a && v <= 0x7f) || (v >= 0x80 && v <= 0x87)) {
        switch (Flower_GetSpecies(p)) {
        case 0:
            r = *p - 0x6d;
            break;
        case 1:
            r = *p - 0x73;
            break;
        case 2:
            r = *p - 0x79;
            break;
        case 3:
            r = *p - 0x7f;
            break;
        }
    }
    return r;
}
}

namespace nN {
extern "C" u16 Flower_PickHybrid(u16 *a, u16 *b) {
    s32 kind, rnd;
    u16 result;
    rnd = Random_GlobalBelow(100);
    result = 0xfff1;
    kind = Flower_GetSpecies(a);
    s32 lo = Flower_GetColor(a);
    s32 hi = Flower_GetColor(b);
    if (lo > hi) {
        s32 t = lo;
        lo = hi;
        hi = t;
    }
    s32 n;
    const FlowerHybridEntry *e;
    e = (sFlowerHybridTable[kind][lo] + hi)->entries;
    if (e != NULL) {
        n = sFlowerHybridTable[kind][lo][hi].numEntries;
        for (; n != 0; e++, n--) {
            rnd -= e->weight;
            if (rnd <= 0) {
                result = e->item;
                break;
            }
        }
    }
    return result;
}
}

namespace nM {
extern "C" BOOL Town_TrySpawnFlowerAt(BlockMap *m, Pos p, Pos q, Pos r, u16 v) {
    BOOL result = FALSE;
    u16 *cell = CellP(m, p, 0);
    if (cell && *cell == 0xfff1) {
        if (Town_IsPlantable(m, p.x, p.y)) {
            BlockMap_PlaceItem(m, p.x, p.y, v, 0);
            result = TRUE;
        }
    }
    return result;
}
}

namespace nM {
extern "C" BOOL Town_TryBreedFlowers(BlockMap *m, Pos p, Pos q, Pos r) {
    BOOL result = FALSE;
    u16 *c1 = CellP(m, p, 0);
    if (c1) {
        BOOL f = FALSE;
        if (*c1 <= 0x19) {
            f = TRUE;
        }
        if (f) {
            u16 *c2 = CellP(m, q, 0);
            s32 t1 = Flower_GetSpecies(c2);
            s32 t2 = Flower_GetSpecies(c1);
            if (t1 == t2) {
                u16 v = Flower_PickHybrid(c2, c1);
                if (v != 0xfff1) {
                    if (Town_SpawnFlowerNearby(m, r, q, v)) {
                        result = 1;
                    } else {
                        result = Town_SpawnFlowerNearby(m, r, p, v);
                    }
                }
            }
        }
    }
    return result;
}
}

namespace nM {
extern "C" void Town_UpdateFlowers(TownUpdater *a, BlockMap *m) {
    s32 y;
    BOOL f;
    u32 t;
    u32 id;
    BOOL g;
    u16 *cell;
    s32 w;
    s32 h;
    s32 hx, hy;
    s32 x;
    Unk_0204da0c_Size *sz;
    sz = &((Unk_0204da0c_Map *)m)->size;
    w = sz->w << 4;
    h = sz->h << 4;
    for (y = 0; y < h; y++) {
        x = 0;
        if (x < w) {
            goto test;
        loop:
            hx = x >> 4; hy = y >> 4;
            cell = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
            if (cell) {
                id = 0xffff;
                f = FALSE;
                t = *cell;
                if (t >= 0x6e && t <= 0x89) {
                    f = TRUE;
                }
                if (f) {
                    id = 0xfff1;
                } else {
                    g = FALSE;
                    if (t <= 0x1a) {
                        g = TRUE;
                    }
                    if (g) {
                        if (Random_GlobalBelow(100) < (s32)sFlowerParchChance[t]) {
                            id = (u16)(*cell + 0x6e);
                        }
                    } else if (t == 0x1d) {
                        if (Random_GlobalBelow(100) < 0x1e) {
                            id = 0x1e;
                        }
                    } else if (t >= 0x8a && t <= 0xa4) {
                        id = (u16)(t - 0x8a);
                    } else if (t == 0xa5) {
                        id = 0x1c;
                    }
                }
                if (id != 0xffff) {
                    BlockMap_PlaceItem(m, x, y, id, 0);
                }
            }
            x++;
        test:
            if (x < w) goto loop;
        }
    }
}
}

namespace nM {
extern "C" void Town_UpdateFlowersExtraDay(TownUpdater *a, BlockMap *m, s32 c) {
    s32 x;
    Unk_0204da0c_Size *sz;
    s32 rem;
    u32 id;
    s32 y;
    u16 *cell;
    u32 t;
    s32 h;
    s32 w;
    s32 hx, hy;
    sz = &((Unk_0204da0c_Map *)m)->size;
    w = sz->w << 4;
    h = sz->h << 4;
    y = 0;
    rem = c % 7;
    for (; y < h; y++) {
        x = 0;
        if (x < w) {
            goto L_test;
        L_loop:
            {
            hx = x >> 4; hy = y >> 4;
            cell = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
            if (cell) {
                id = 0xffff;
                BOOL f = FALSE;
                t = *cell;
                if (t >= 0x6e && t <= 0x89) {
                    f = TRUE;
                }
                if (f) {
                    if (rem == 0) {
                        id = 0xfff1;
                    }
                } else if (t == 0x1e) {
                    if (rem == 0) {
                        if (Random_GlobalBelow(100) < 0x1e) {
                            id = 0xfff1;
                        }
                    }
                } else {
                    BOOL g = FALSE;
                    if (t <= 0x1a) {
                        g = TRUE;
                    }
                    if (g) {
                        if (Random_GlobalBelow(100) < (s32)sFlowerParchChanceExtraDay[t]) {
                            id = (u16)(*cell + 0x6e);
                        }
                    } else if (t == 0x1d) {
                        if (Random_GlobalBelow(100) < 0x1e) {
                            id = 0x1e;
                        }
                    }
                }
                if (id != 0xffff) {
                    BlockMap_PlaceItem(m, x, y, id, 0);
                }
            }
        }
            x++;
        L_test:
            if (x < w) goto L_loop;
        }
    }
}
}

namespace nM {
extern "C" BOOL Town_ForEachNeighbor(BlockMap *m, Pos size, Pos pos, BOOL (*cb)(BlockMap *, Pos, Pos, Pos)) {
    s32 i;
    BOOL result = FALSE;
    const u8 *p = sNeighborOffsets8;
    for (i = 0; i < 8; p++, i++) {
        u8 b = *p;
        s32 nx = pos.x - ((b >> 4) - 8);
        s32 ny = pos.y - ((b & 15) - 8);
        if (nx >= 0 && nx < size.x && ny >= 0 && ny < size.y) {
            if (nx != pos.x || ny != pos.y) {
                result = cb(m, Pos(nx, ny), pos, size);
                if (result) {
                    break;
                }
            }
        }
    }
    return result;
}
}

namespace nM {
extern "C" BOOL Town_SpawnFlowerNearby(BlockMap *m, Pos size, Pos pos, u16 v) {
    s32 result = 0;
    s32 r = Random_GlobalBelow(8);
    s32 i;
    for (i = 0; i < 8; i++) {
        u8 b = sNeighborOffsets8[r];
        s32 nx = pos.x - ((b >> 4) - 8);
        s32 ny = pos.y - ((b & 15) - 8);
        if (nx >= 0 && nx < size.x && ny >= 0 && ny < size.y) {
            if (Town_TrySpawnFlowerAt(m, Pos(nx, ny), pos, size, v)) {
                result = 1;
                break;
            }
        }
        r = (r + 1) % 8;
    }
    return result;
}
}

namespace nM {
extern "C" BOOL Town_HasNoBlockerAround(TownUpdater *a, BlockMap *m, Pos size, Pos pos) {
    BOOL r = TRUE;
    if (Town_ForEachNeighbor(m, size, pos, Town_IsSaplingBlockerAt)) {
        r = FALSE;
    }
    return r;
}
}

namespace nM {
extern "C" void Town_CheckSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y) {
    const Pos p(x, y);
    if (((BlockMap *)m)->getPlantFlag(p.x, p.y) != 1) {
        Town_WitherSapling(m, *cell, x, y);
    } else {
        if (!Town_HasNoBlockerAround(a, m, size, p)) {
            Town_WitherSapling(m, *cell, x, y);
        }
    }
}
}

namespace nM {
extern "C" void Town_CheckCedarSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y) {
    if (y >= 0x30) {
        Town_WitherSapling(m, *cell, x, y);
    } else {
        Town_CheckSapling(a, m, cell, size, x, y);
    }
}
}

namespace nM {
extern "C" void Town_CheckPalmSapling(TownUpdater *a, BlockMap *m, u16 *cell, Pos size, s32 x, s32 y) {
    if (y < 0x40) {
        Town_WitherSapling(m, *cell, x, y);
    } else {
        Town_CheckSapling(a, m, cell, size, x, y);
    }
}
}

namespace nM {
extern "C" void Town_UpdateSaplings(TownUpdater *a, BlockMap *m, Pos size) {
    s32 x, y;
    for (y = 0; y < size.y; y++) {
        x = 0;
        if (x < size.x) {
            goto test;
        loop:
            {
                u16 *cell = Cell(m, x, y, 0);
                if (cell && Item_IsTreeStage0(cell)) {
                    switch (*cell) {
                    case 0x5d:
                        Town_CheckCedarSapling(a, m, cell, size, x, y);
                        break;
                    case 0xc8:
                        Town_CheckPalmSapling(a, m, cell, size, x, y);
                        break;
                    default:
                        Town_CheckSapling(a, m, cell, size, x, y);
                        break;
                    }
                }
            }
            x++;
        test:
            if (x < size.x) goto loop;
        }
    }
}
}

namespace nM {
extern "C" void Town_WitherNeighborSaplings(TownUpdater *a, BlockMap *m, Pos size, Pos pos) {
    Town_ForEachNeighbor(m, size, pos, Town_WitherSaplingAt);
}
}

namespace nM {
extern "C" void Town_ThinSaplings(TownUpdater *a, BlockMap *m, Pos size) {
    s32 x, y, i;
    for (i = 0; i < 3; i++) {
        for (y = 0; y < size.y; y++) {
            x = data_020c97cc[i][y & 1];
            if (x < size.x) {
                goto test;
            loop:
                {
                    u16 *cell = Cell(m, x, y, 0);
                    if (cell) {
                        if (Item_IsTreeStage0(cell)) {
                            Town_WitherNeighborSaplings(a, m, size, Pos(x, y));
                        }
                    }
                }
                x += 4;
            test:
                if (x < size.x) goto loop;
            }
        }
    }
}
}

namespace nM {
extern "C" void Town_WitherCrowdedSaplings(TownUpdater *a, BlockMap *m, Pos size) {
s32 x, y; const u8 *p; s32 count, i; u16 *cell; s32 hx, hy;
    for (y = 0; y < size.y; y++) {
        x = 0;
        if (x < size.x) {
            goto L_test;
        L_loop:
            {

            hx = x >> 4; hy = y >> 4;
            cell = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
            if (cell) {
                if (Unk_020489cc_Check(cell)) {
                    if (Item_IsTreeStage0(cell)) {
count = 0; p = data_020c9850;
                        for (i = 0; i < 0x30; p++, i++) {
                            u8 b = *p;
                            s32 nx = x - ((b >> 4) - 8);
                            s32 ny = y - ((b & 15) - 8);
                            if (nx >= 0 && nx < size.x && ny >= 0 && ny < size.y) {
                                u16 *c2 = Cell(m, nx, ny, 0);
                                if (c2) {
                                    if (Unk_020489cc_Check(c2)) {
                                        count++;
                                    }
                                }
                            }
                        }
                        if (count >= 8) {
                            Town_WitherSapling(m, *cell, x, y);
                        }
                    }
                }
            }
        }
            x++;
        L_test:
            if (x < size.x) goto L_loop;
        }
    }
}
}

namespace nL {
extern "C" void TreeQuota_Init(TreeQuota *s, TownEvalAcre *c, s32 i, s32 j) {
    s32 k;
    s->trees = c->trees[i][j];
    s->numSaplings = 0;
    for (k = 0; k < 64; k++) {
        s->saplingUnits[k].x = -1;
        s->saplingUnits[k].y = -1;
    }
}
}

namespace nL {
extern "C" void TreeQuota_Add(TreeQuota *s, u16 id, s32 x, s32 y) {
    s->saplingItems[s->numSaplings] = id;
    s->saplingUnits[s->numSaplings].x = x;
    s->saplingUnits[s->numSaplings].y = y;
    s->numSaplings++;
}
}

namespace nL {
extern "C" void TreeQuota_Apply(TreeQuota *s) {
    if (s->trees >= 7) {
        s32 n = s->trees - 6;
        if (n > s->numSaplings) n = s->numSaplings;
        TownBlockMap *o = TownBlockMap_Get();
        s32 cnt = s->numSaplings;
        s32 m1 = -1;
        for (; n != 0; n--) {
            s32 c = Random_GlobalBelow(s->numSaplings);
            s32 j;
            for (j = 0; j < cnt; j++) {
                if (s->saplingUnits[j].x >= 0) {
                    c--;
                    if (c < 0) {
                        Town_WitherSapling(o, s->saplingItems[j], s->saplingUnits[j].x, s->saplingUnits[j].y);
                        s->saplingUnits[j].x = m1;
                        s->saplingUnits[j].y = m1;
                        s->numSaplings--;
                        break;
                    }
                }
            }
        }
    }
}
}

namespace nL {
extern "C" void TreeQuota_InitAll(TreeQuota (*s)[2], TownEvalAcre *c) {
    s32 x, y;
    for (y = 0; y < 2; y++) {
        for (x = 0; x < 2; x++) {
            TreeQuota_Init(&s[x][y], c, x, y);
        }
    }
}
}

namespace nL {
extern "C" void TreeQuota_AddAt(TreeQuota (*s)[2], u16 id, s32 x, s32 y, s32 i, s32 j) {
    s32 px, py;
    FieldUnit_FromBlockUnit(&px, &py, x, y, i, j);
    TreeQuota_Add(&s[(i >> 3) & 1][(j >> 3) & 1], id, px, py);
}
}

namespace nL {
extern "C" void TreeQuota_ApplyAll(TreeQuota (*s)[2]) {
    s32 x, y;
    for (y = 0; y < 2; y++) {
        for (x = 0; x < 2; x++) {
            TreeQuota_Apply(&s[x][y]);
        }
    }
}
}

namespace nL {
extern "C" void Town_LimitSaplingsPerQuadrant(TownUpdater *a, BlockMap *q, s32 w, s32 h) {
    TreeQuota s[2][2];
    s32 x, y, i;
    u16 *t;
    TreeQuota *sl = &s[0][0];
    Vec2 *pp;
    y = 0;
    do {
        pp = sl->saplingUnits;
        do {
            pp->x = y;
            pp->y = y;
            pp++;
        } while (pp != sl->saplingUnits + 64);
        sl++;
    } while (sl != &s[0][0] + 4);
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            t = BlockMap_GetItemPtr(q, x + 1, y + 1, 0, 0, 0);
            TreeQuota_InitAll(s, &nZ::gTownEval.acres[x][y]);
            for (i = 0; i < 256; t++, i++) {
                if (t != NULL && Item_IsTreeStage0(t) != 0) {
                    TreeQuota_AddAt(s, *t, x + 1, y + 1, i & 15, (i >> 4) & 15);
                }
            }
            TreeQuota_ApplyAll(s);
        }
    }
}
}

namespace nL {
extern "C" void Town_GrowTrees(TownUpdater *a, BlockMap *q, Unk_020485d4_Size *sz) {
    s32 x, y;
    for (y = 0; y < sz->h; y++) {
        x = 0;
        if (x < sz->w) {
            goto test;
        loop:
            {
                s32 bx = x >> 4;
                s32 by = y >> 4;
                u16 *t = BlockMap_GetItemPtr(q, bx, by, x - (bx << 4), y - (by << 4), 0);
                if (t != NULL) {
                    if (Unk_02048634_Check(t)) Town_GrowTree(q, t, x, y);
                }
            }
            x++;
        test:
            if (x < sz->w) goto loop;
        }
    }
}
}

namespace nL {
extern "C" void Town_UpdateTrees(TownUpdater *a, BlockMap *q, s32 c, s32 d) {
    TownBlockMap *qq = (TownBlockMap *)q;
    Vec2 *pp = (Vec2 *)&qq->width;
    s32 h = pp->y << 4;
    s32 w = pp->x << 4;
    Town_UpdateSaplings(a, q, Unk_02048cc4_Pos(w, h));
    Town_ThinSaplings(a, q, Unk_02048cc4_Pos(w, h));
    Town_WitherCrowdedSaplings(a, q, Unk_02048cc4_Pos(w, h));
    Town_LimitSaplingsPerQuadrant(a, q, c, d);
    Unk_020485d4_Size s4(w, h);
    Town_GrowTrees(a, q, &s4);
}
}

namespace nL {
extern "C" BOOL Town_CanSpawnCloverAt(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    s32 bx;
    if (((BlockMap *)p)->isGrassSurface(x, y) != 0) {
        s32 t = y + 1;
        bx = x >> 4;
        s32 ty = t >> 4;
        u16 *a = BlockMap_GetItemPtr(p, bx, ty, x - (bx << 4), t - (ty << 4), 0);
        if (a != NULL && Town_IsSaplingBlocker(a) == 0) {
            s32 t2 = y + 2;
            s32 ty2 = t2 >> 4;
            u16 *b = BlockMap_GetItemPtr(p, bx, ty2, x - ((u32)bx << 4), t2 - (ty2 << 4), 0);
            if (b != NULL && Town_IsSaplingBlocker(b) == 0) {
                r = TRUE;
            }
        }
    }
    return r;
}
}

namespace nL {
extern "C" BOOL Town_IsGrass(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    if (((BlockMap *)p)->getPlantFlag(x, y) == 1) r = TRUE;
    return r;
}
}

namespace nL {
extern "C" BOOL Town_CanSpawnWeedAt(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    s32 bx;
    if (Town_IsGrass(p, x, y) != 0) {
        s32 t = y + 1;
        bx = x >> 4;
        s32 ty = t >> 4;
        u16 *a = BlockMap_GetItemPtr(p, bx, ty, x - (bx << 4), t - (ty << 4), 0);
        if (a != NULL && Town_IsSaplingBlocker(a) == 0) {
            s32 t2 = y + 2;
            s32 ty2 = t2 >> 4;
            u16 *b = BlockMap_GetItemPtr(p, bx, ty2, x - ((u32)bx << 4), t2 - (ty2 << 4), 0);
            if (b != NULL && Town_IsSaplingBlocker(b) == 0) {
                r = TRUE;
            }
        }
    }
    return r;
}
}

namespace nL {
extern "C" BOOL Town_IsPlantable(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    switch (((BlockMap *)p)->getPlantFlag(x, y)) {
    case 0:
    case 1:
        r = TRUE;
    }
    return r;
}
}

namespace nL {
extern "C" BOOL Town_CanBuryAt(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    s32 bx;
    s32 k = ((BlockMap *)p)->getDigKind(x, y);
    switch (k) {
    case 0:
    case 1: {
        s32 t = y + 1;
        bx = x >> 4;
        s32 ty = t >> 4;
        u16 *a = BlockMap_GetItemPtr(p, bx, ty, x - (bx << 4), t - (ty << 4), 0);
        if (a != NULL && Town_IsSaplingBlocker(a) == 0) {
            s32 t2 = y + 2;
            s32 ty2 = t2 >> 4;
            u16 *b = BlockMap_GetItemPtr(p, bx, ty2, x - ((u32)bx << 4), t2 - (ty2 << 4), 0);
            if (b != NULL && Town_IsSaplingBlocker(b) == 0) {
                r = TRUE;
            }
        }
        break;
    }
    }
    return r;
}
}

namespace nL {
extern "C" s32 Town_GetSandAttr(BlockMap *p, s32 x, s32 y) {
    return ((BlockMap *)p)->isShore(x, y);
}
}

namespace nL {
extern "C" BOOL Town_IsSandAt(BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    if (((BlockMap *)p)->isShore(x, y) != 0) r = TRUE;
    return r;
}
}

namespace nL {
extern "C" s32 Town_CollectFreeUnits(TownUpdater *a, Vec2 *arr, BlockMap *c, s32 d, s32 e, s32 (*fn)(BlockMap *, s32, s32)) {
    s32 j, n, i;
    n = 0;
    i = 0;
    u32 ee = e;
    for (; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            u16 *t = BlockMap_GetItemPtr(c, d, ee, j, i, 0);
            if (t != NULL && *t == 0xfff1) {
                s32 px, py;
                FieldUnit_FromBlockUnit(&px, &py, d, ee, j, i);
                if (fn(c, px, py) != 0) {
                    arr->x = px;
                    arr->y = py;
                    arr++;
                    n++;
                }
            }
        }
    }
    return n;
}
}

namespace nL {
extern "C" s32 Town_PlaceAtRandomUnit(TownUpdater *a, BlockMap *b, s32 n, Vec2 *arr, u16 e, s32 g) {
    s32 r = -1;
    Vec2 *p;
    if (n > 0) {
        u16 t;
        r = Random_GlobalBelow(n);
        p = arr + r;
        if (gCommManager->isOnline() == 0) {
            t = e;
            BlockMap_SetItemAtUnit(b, &t, p->x, p->y, 0);
            if (g != 0) {
                BlockMap_SetBuriedAtUnit(b, p->x, p->y);
            } else {
                BlockMap_ClearBuriedAtUnit(b, p->x, p->y);
            }
        } else {
            s32 o = Scene_GetCurrent();
            ItemSync_SetAtUnit((s8)p->x, (s8)p->y, o, e, g);
        }
    }
    return r;
}
}

namespace nL {
extern "C" BOOL Town_SpawnItemInAcre(TownUpdater *a, BlockMap *b, s32 c, s32 d, u16 e, s32 (*f)(BlockMap *, s32, s32), s32 g) {
    Vec2 arr[256];
    s32 i, n;
    BOOL result;
    Vec2 *pp = arr;
    Vec2 *endp;
    result = FALSE;
    endp = arr + 256;
    do {
        pp->x = 0;
        pp->y = 0;
        pp++;
    } while (pp != endp);
    n = Town_CollectFreeUnits(a, arr, b, c, d, f);
    if (Town_PlaceAtRandomUnit(a, b, n, arr, e, g) >= 0) {
        result = TRUE;
    }
    return result;
}
}

namespace nL {
extern "C" s32 Town_MakeSpecialTree(TownUpdater *a, BlockMap *b, s32 x, s32 y, s32 e) {
    Unk_020481b8_PosZ pos[256];
    u16 id[256];
    s32 j, i, n;
    s32 result;
    n = 0;
    result = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            u16 *t = BlockMap_GetItemPtr(b, x, y, j, i, 0);
            if (t != NULL) {
                s32 px, py;
                FieldUnit_FromBlockUnit(&px, &py, x, y, j, i);
                switch (*t) {
                case 0x2a:
                    pos[n].x = px;
                    pos[n].y = py;
                    id[n] = 0x66;
                    n++;
                    break;
                case 0x61:
                    pos[n].x = px;
                    pos[n].y = py;
                    id[n] = 0x6a;
                    n++;
                    break;
                }
            }
        }
    }
    if (n > 0) {
        s32 k = Random_GlobalBelow(n);
        BlockMap_PlaceItem(b, pos[k].x, pos[k].y, (u16)(e + id[k]), 0);
        result = 1;
    }
    return result;
}
}

namespace nL {
extern "C" void Town_SpawnBeeTrees(TownUpdater *a, BlockMap *b, s32 w, s32 h) {
    BOOL ok;
    s32 x, y;
    for (x = 0; x < w; x++) {
        ok = TRUE;
        for (y = 0; y < h; y++) {
            if (nZ::gTownEval.acres[x][y].bits.beeTree != 0) {
                ok = FALSE;
                break;
            }
        }
        if (ok) {
            s32 r = Random_GlobalBelow(h) + 1;
            Town_MakeSpecialTree(a, b, x + 1, r, 1);
        }
    }
}
}

namespace nL {
extern "C" void Town_SpawnFurnitureTrees(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 n = 2 - nZ::gTownEval.furnitureTreeAcres;
    while (n != 0) {
        s32 x = Random_GlobalBelow(c);
        s32 y = Random_GlobalBelow(d);
        Town_MakeSpecialTree(a, b, x + 1, y + 1, 0);
        n--;
    }
}
}

namespace nL {
extern "C" void Town_SpawnBellTrees(TownUpdater *a, BlockMap *b, s32 w, s32 h) {
    s32 x, y;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (nZ::gTownEval.acres[x][y].bits.bellTree == 0) {
                Town_MakeSpecialTree(a, b, x + 1, y + 1, 2);
            }
        }
    }
}
}

namespace nK {
extern "C" void Town_SpawnSpecialTrees(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    Town_SpawnBeeTrees(a, b, c, d);
    Town_SpawnFurnitureTrees(a, b, c, d);
    Town_SpawnBellTrees(a, b, c, d);
}
}

namespace nK {
extern "C" BOOL Pos_IsInList(void *a, Vec2 *pos, Vec2 *arr, s32 n) {
    BOOL r = FALSE;
    for (; n != 0; arr++, n--) {
        if (pos->x == arr->x && pos->y == arr->y) {
            r = TRUE;
            break;
        }
    }
    return r;
}
}

namespace nK {
extern "C" void Town_SpawnWeeds(TownUpdater *a, BlockMap *b) {
    s32 r;
    Vec2 pos;
    Vec2 arr[3];
    s32 cnt = 0;
    s32 i;
    s32 z = 0;
    pos.x = 0; pos.y = 0;
    arr[0].x = 0; arr[0].y = 0; arr[1].x = 0; arr[1].y = 0;
    arr[2].x = 0; arr[2].y = 0;
    for (i = 0; i < 3; i++) {
        do {
            r = Random_GlobalBelow(16);
            pos.x = r % 4;
            pos.y = r / 4;
        } while (Pos_IsInList(a, &pos, arr, cnt));
        Town_SpawnItemInAcre(a, b, pos.x + 1, pos.y + 1, (u16)(Random_GlobalBelow(4) + 0x21), Town_CanSpawnWeedAt, z);
        arr[cnt].x = pos.x;
        arr[cnt].y = pos.y;
        cnt++;
    }
}
}

namespace nK {
extern "C" void Town_SpawnFlowersAroundHouse(TownUpdater *a, BlockMap *b, Vec2 *pos, s32 d) {
    Vec2 arr[17];
    Vec2 *q;
    s32 cnt;
    s32 x, y, xh, yh, i;
    s32 z = 0;
    q = arr;
    do { q->x = z; q->y = z; q++; } while (q != &arr[17]);
    cnt = z;
    for (i = 0; i < 17; i++) {
        u32 v = data_020c96b8[i];
        x = pos->x + (((s32)v >> 4) - 8);
        y = pos->y + ((v & 15) - 8);
        xh = x >> 4;
        yh = y >> 4;
        u16 *p = BlockMap_GetItemPtr(b, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (p && *p == 0xfff1 && Town_IsPlantable(b, x, y)) {
            arr[cnt].x = x;
            arr[cnt].y = y;
            cnt++;
        }
    }
    s32 n = d * 2;
    for (; cnt > 0; cnt--) {
        i = Town_PlaceAtRandomUnit(a, b, cnt, arr, data_020c9754[Random_GlobalBelow(12)], 0);
        n--;
        if (n <= 0) break;
        for (; i < 16; i++) {
            arr[i].x = arr[i + 1].x;
            arr[i].y = arr[i + 1].y;
        }
    }
}
}

namespace nK {
extern "C" void Town_SpawnVillagerHouseFlowers(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 r = SaveVillagers_GetUnk3830Index(gSaveVillagers);
    u8 *q;
    void *p;
    s32 i;
    for (i = 0; i < 8; i++) {
        p = SaveVillagers_Get(gSaveVillagers, i);
        if (Villager_IsSettledExcept(p, r)) {
            if (c == 0) {
                if (((VillagerPlan *)(VillagerPlanBlock_GetPlan(Villager_GetPlan(p))))->getState() != 5) continue;
            }
            q = ((VillagerDataItemView *)(p))->getHousePos();
            if (HousePos_IsValid(q)) {
                Vec2Ctor pos(q[0], q[1]);
                Town_SpawnFlowersAroundHouse(a, b, &pos, d);
            }
        }
    }
}
}

namespace nK {
extern "C" void Town_UpdateVillagerHouseFlowers(TownUpdater *a, BlockMap *b, void *c, s32 d) {
    u8 buf[8];
    MI_CpuCopy8(c, buf, 8);
    if (Event_GetState(14, buf, 0) != 2 ? TRUE : FALSE) {
        if (d > 7) d = 7;
        Town_SpawnVillagerHouseFlowers(a, b, 0, d);
    }
}
}

namespace nK {
extern "C" void Town_SpawnRandomFlower(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 x = Random_GlobalBelow(c);
    s32 y = Random_GlobalBelow(d);
    s32 i = Random_GlobalBelow(12);
    Town_SpawnItemInAcre(a, b, x + 1, y + 1, data_020c9754[i], Town_IsPlantable, 0);
}
}

namespace nK {
extern "C" void Town_BreedFlowers(TownUpdater *a, TownBlockMap *b, s32 c, s32 d) {
    s32 cnt, y, x;
    for (y = 0; y < d; y++) {
        for (x = 0; x < c; x++) {
            if (Random_GlobalBelow(100) < 20) {
                s32 i, j, k;
                s32 ox, oy;
                Vec2 from, to;
                Vec2 arr[256];
                Vec2 *q;
                Vec2 *end = &arr[256];
                q = arr;
                do { q->x = 0; q->y = 0; q++; } while (q != end);
                cnt = 0;
                for (i = 0; i < 16; i++) {
                    for (j = 0; j < 16; j++) {
                        u16 *p = BlockMap_GetItemPtr(b, x + 1, y + 1, j, i, 0);
                        if (p) {
                            BOOL ok = FALSE;
                            if (*p <= 0x19) ok = TRUE;
                            if (ok) {
                                FieldUnit_FromBlockUnit(&ox, &oy, x + 1, y + 1, j, i);
                                arr[cnt].x = ox;
                                arr[cnt].y = oy;
                                cnt++;
                            }
                        }
                    }
                }
                if (cnt > 0) {
                    k = Random_GlobalBelow(cnt);
                    s32 fx = b->width << 4;
                    s32 fy = b->height << 4;
                    to.x = arr[k].x;
                    to.y = arr[k].y;
                    from.x = fx;
                    from.y = fy;
                    Town_ForEachNeighbor(b, Unk_02048cc4_Pos(from.x, from.y), Unk_02048cc4_Pos(to.x, to.y), Town_TryBreedFlowers);
                }
            }
        }
    }
}
}

namespace nK {
extern "C" void Town_SpawnDandelion(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 x = Random_GlobalBelow(c);
    s32 y = Random_GlobalBelow(d);
    Town_SpawnItemInAcre(a, b, x + 1, y + 1, 0x1d, Town_IsGrass, 0);
}
}

namespace nK {
extern "C" void Town_SpawnClover(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 x = Random_GlobalBelow(c);
    s32 y = Random_GlobalBelow(d);
    s32 id = Random_GlobalBelow(100) < 2 ? 0x20 : 0x1f;
    Town_SpawnItemInAcre(a, b, x + 1, y + 1, id, Town_CanSpawnCloverAt, 0);
}
}

namespace nK {
extern "C" void Town_PickJacobsLadderAcre(Vec2 *out, void *a, s32 c, s32 d) {
    s32 t = c * d - nZ::gTownEval.jacobsAcres;
    if (t > 0) {
        s32 n = Random_GlobalBelow(t);
        s32 x, y;
        for (y = 0; y < d; y++) {
            for (x = 0; x < c; x++) {
                if (nZ::gTownEval.acres[x][y].bits.jacobsLadder == 0) {
                    if (n == 0) {
                        out->x = x;
                        out->y = y;
                        return;
                    }
                    n--;
                }
            }
        }
    }
    out->x = -1;
    out->y = -1;
}
}

namespace nK {
extern "C" void Town_SpawnJacobsLadder(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    Vec2 out;
    Town_PickJacobsLadderAcre(&out, a, c, d);
    s32 x = out.x;
    s32 y = out.y;
    if (x != -1) {
        Town_SpawnItemInAcre(a, b, x + 1, y + 1, 0x1a, Town_CanSpawnWeedAt, 0);
    }
}
}

namespace nK {
extern "C" void Town_TrySpawnJacobsLadder(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    if (gTownEval.rank == 4) {
        if (Random_GlobalBelow(100) < 50) Town_SpawnJacobsLadder(a, b, c, d);
    }
}
}

namespace nK {
extern "C" s32 Town_SpawnRafflesia(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 x = Random_GlobalBelow(c);
    s32 y = Random_GlobalBelow(d);
    return Town_SpawnItemInAcre(a, b, x + 1, y + 1, 0x1b, Town_CanSpawnWeedAt, 0);
}
}

namespace nK {
extern "C" void Town_WitherRafflesia(TownUpdater *a, BlockMap *b) {
    s32 u, v, x, y;
    u = gTownEval.rafflesiaUnit.x; v = gTownEval.rafflesiaUnit.y; x = gTownEval.rafflesiaBlock.x; y = gTownEval.rafflesiaBlock.y;
    if (BlockMap_GetItemPtr(b, x + 1, y + 1, u, v, 0)) {
        u = gTownEval.rafflesiaUnit.x; v = gTownEval.rafflesiaUnit.y; x = gTownEval.rafflesiaBlock.x; y = gTownEval.rafflesiaBlock.y;
        BlockMap_PlaceItemAt(a, b, x + 1, y + 1, u, v, 0x89, 0);
    }
}
}

namespace nK {
extern "C" void Town_UpdateRafflesia(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 x = gTownEval.rank;
    s32 y;
    if (x == 0) {
        x = gTownEval.rafflesiaBlock.x;
        y = gTownEval.rafflesiaBlock.y;
        if (x < 0 && y < 0) Town_SpawnRafflesia(a, b, c, d);
    } else {
        Town_WitherRafflesia(a, b);
    }
}
}

namespace nK {
extern "C" void Town_BuryFossils(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 n = 3 - nZ::gTownEval.buriedFossils;
    s32 x = Random_GlobalBelow(c - 1);
    s32 y = Random_GlobalBelow(d - 1);
    for (; n > 0; n--) {
        x = DS_UMOD((x + 1 + Random_GlobalBelow(c - 1)), c);
        y = DS_UMOD((y + 1 + Random_GlobalBelow(d - 1)), d);
        Town_SpawnItemInAcre(a, b, x + 1, y + 1, 0x1549, Town_CanBuryAt, 1);
    }
}
}

namespace nK {
extern "C" void Town_BuryPitfallSeed(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    if (gTownEval.townBits.pitfallSeed == 0) {
        s32 x = Random_GlobalBelow(c - 1);
        s32 y = Random_GlobalBelow(d - 1);
        Town_SpawnItemInAcre(a, b, x + 1, y + 1, 0x1566, Town_CanSpawnWeedAt, 1);
    }
}
}

namespace nK {
extern "C" BOOL Town_ConvertRockInAcre(TownUpdater *a, BlockMap *b, TownEvalAcre *cell, s32 d, s32 e, s32 f) {
    s32 x, y;
    s32 ox, oy;
    s32 v = cell->rocks;
    s32 n = Random_GlobalBelow(v);
    cell->rocks = v - 1;
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            u16 *p = BlockMap_GetItemPtr(b, e + 1, f + 1, x, y, 0);
            if (p) {
                BOOL ok = FALSE;
                u32 t = *p;
                if (t >= 0xe3 && t <= 0xe7) ok = TRUE;
                if (ok) {
                    n--;
                    if (n < 0) {
                        FieldUnit_FromBlockUnit(&ox, &oy, e + 1, f + 1, x, y);
                        s32 q = (s32)(t - 0xe3) % 5;
                        s32 w = data_020c912c[d];
                        BlockMap_PlaceItem(b, ox, oy, (u16)(w + q), 0);
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}
}

namespace nK {
extern "C" s32 Town_ConvertRandomRock(TownUpdater *a, BlockMap *b, s32 c, s32 d, s32 e, s32 f) {
    s32 r = Random_GlobalBelow(f);
    s32 x, y;
    for (y = 0; y < d; y++) {
        for (x = 0; x < c; x++) {
            TownEvalAcre *cell = &nZ::gTownEval.acres[x][y];
            s32 v = cell->rocks;
            if (v > 0) {
                r -= v;
                if (r < 0) {
                    if (Town_ConvertRockInAcre(a, b, cell, e, x, y)) return 1;
                }
            }
        }
    }
    return 0;
}
}

namespace nK {
extern "C" void Town_UpdateSpecialRocks(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 cnt = nZ::gTownEval.rocks;
    s32 i;
    for (i = 0; i < 4; i++) {
        if (cnt <= 0) return;
        if ((1 << i & gTownEval.townBits.moneyRocks) == 0) {
            if (Town_ConvertRandomRock(a, b, c, d, i, cnt)) cnt--;
        }
    }
    MoneyRock_Reset(&data_021c47bc);
}
}

namespace nK {
extern "C" void Town_ClearObjectsFcFd(TownUpdater *a) {
    s32 w;
    s32 j;
    s32 x;
    s32 y;
    s32 i;
    s32 h;
    TownBlockMap *m;
    m = TownBlockMap_Get();
    if (m) {
        w = m->width - 2;
        h = m->height - 2;
        for (y = 1; y <= h; y++) {
            for (x = 1; x <= w; x++) {
                for (i = 0; i < 16; i++) {
                    for (j = 0; j < 16; j++) {
                        u16 *p = BlockMap_GetItemPtr(m, x, y, j, i, 0);
                        if (p && Unk_02047798_InRange(p, 0xfc, 0xfd)) {
                            BlockMap_PlaceItemAt(a, m, x, y, j, i, 0xfff1, 0);
                        }
                    }
                }
            }
        }
    }
}
}

namespace nJ {
extern "C" void Town_ClearObjectFcFdAtPos(TownUpdater *a, VecFx32 *b) {
    TownBlockMap *m = TownBlockMap_Get();
    if (m != 0) {
        VecXZ v;
        v.x = 0;
        v.z = 0;
        FieldPos_ToUnit(&v.x, &v.z, b);
        long x = v.x, z = v.z;
        s32 tx = x >> 4, tz = z >> 4;
        u16 *p = BlockMap_GetItemPtr(m, tx, tz, x - (tx << 4), z - (tz << 4), 0);
        if (p != 0) {
            if (Unk_020470b8_R(p, 0xfc, 0xfd)) {
                Vec2 pos;
                pos.x = v.x;
                pos.y = v.z;
                if (PendingUnit_IndexAt(&pos, 0) < 0) {
                    BlockMap_PlaceItem(m, v.x, v.z, 0xfff1, 0);
                }
            }
        }
    }
}
}

namespace nJ {
extern "C" void Town_UpdateRedTurnips(TownUpdater *a, TownBlockMap *b, s32 c) {
    s32 *s = &b->width;
    s32 w = s[0] << 4;
    s32 h = s[1] << 4;
    s32 x, y;
    for (y = 0; y < h; y++) {
        x = 0;
        if (x < w) {
            goto L_test;
        L_loop:
            {
            s32 tx = x >> 4;
            s32 ty = y >> 4;
            u16 *p = BlockMap_GetItemPtr(b, tx, ty, x - (tx << 4), y - (ty << 4), 0);
            if (p != 0) {
                BOOL r = Unk_020470b8_R(p, 0xd4, 0xda);
                u16 t = *p;
                if (r) {
                    BlockMap_PlaceItem(b, x, y, 0xe2, 0);
                } else if (t >= 0xdb && t <= 0xe1) {
                    if (c > 1) {
                        BlockMap_PlaceItem(b, x, y, 0xe2, 0);
                    } else if (t != 0xe1) {
                        BlockMap_PlaceItem(b, x, y, (u16)(t - 6), 0);
                    } else {
                        BlockMap_PlaceItem(b, x, y, (u16)(t - 7), 0);
                    }
                } else if (t == 0xe2) {
                    BlockMap_PlaceItem(b, x, y, 0xfff1, 0);
                } else if (t >= 0x154a && t <= 0x1553) {
                    BlockMap_PlaceItem(b, x, y, 0xfff1, 0);
                }
            }
        }
            x++;
        L_test:
            if (x < w) goto L_loop;
        }
    }
}
}

namespace nJ {
extern "C" void Town_SpawnCoconut(TownUpdater *a, BlockMap *b, s32 c) {
    if (gTownEval.townBits.coconut == 0) {
        if (Random_GlobalBelow(100) < 10) {
            s32 t = Random_GlobalBelow(c) + 1;
            Town_SpawnItemInAcre(a, b, t, 4, 0x1548, Town_GetSandAttr, 0);
        }
    }
}
}

namespace nJ {
extern "C" void Town_BuryGyroids(TownUpdater *a, BlockMap *b, s32 c, s32 d, u8 e) {
    if (e != 0) {
        s32 r = Weather_GetCurrent();
        switch (r) {
        case 0:
        case 1:
        case 2: {
            u16 v[2];
            s32 px, py;
            v[0] = 0xfff1;
            px = Random_GlobalBelow(c - 1);
            py = Random_GlobalBelow(d - 1);
            for (s32 i = 0; i < 3; i++) {
                ItemPick_FromRange(&v[1], 0x45dc, 0x7f, 0, 0, 0, 1, 10, 0, 1);
                v[0] = v[1];
                px = DS_UMOD((px + 1 + Random_GlobalBelow(c - 1)), c);
                py = DS_UMOD((py + 1 + Random_GlobalBelow(d - 1)), d);
                Town_SpawnItemInAcre(a, b, px + 1, py + 1, v[0], Town_CanBuryAt, 1);
            }
            break;
        }
        }
    }
}
}

namespace nJ {
extern "C" void Town_UpgradeBuriedShovels(TownUpdater *a, BlockMap *b, s32 c, s32 d) {
    s32 y, x, i, j;
    for (y = 1; y < d + 1; y++) {
        for (x = 1; x < c + 1; x++) {
            for (i = 0; i < 16; i++) {
                for (j = 0; j < 16; j++) {
                    u16 *p = BlockMap_GetItemPtr(b, x, y, j, i, 0);
                    if (p != 0 && *p == 0x1369) {
                        if (BlockMap_IsBuried(b, x, y, j, i) != 0) {
                            BlockMap_PlaceItemAt(a, b, x, y, j, i, 0x136a, 1);
                        }
                    }
                }
            }
        }
    }
}
}

namespace nJ {
extern "C" s32 Town_CountEventDays(TownUpdater *a, s32 b, u8 *c, ClockDateTime *d, s32 e, s32 f, SaveData *g) {
    s32 r;
    if (e != 0) {
        if (TownState_FindEvent(&g->townState, b) < 0) {
            if (f != 0) {
                r = 1;
            } else {
                ClockDateTimeWords t;
                t.words[0] = 0;
                t.words[1] = 0;
                t.dt.year = d->year;
                t.dt.month = c[7];
                t.dt.day = c[6];
                r = DateTime_DiffDays(&t.dt, d) + 1;
            }
        } else {
            DayMonthYear t;
            t.b[2] = d->year;
            t.b[1] = d->month;
            t.b[0] = d->day;
            r = Date_DaysBetween(t.b, &g->townState.eventUpdateDay);
            if (r < 0) r = 0;
        }
    } else {
        r = 1;
    }
    return r;
}
}

namespace nJ {
extern "C" u32 Item_RandomAcorn(TownUpdater *a) {
    u32 b = 0x1542;
    b += Random_GlobalBelow(5);
    return (u16)b;
}
}

namespace nJ {
extern "C" void Town_DropAcornsInAcre(TownUpdater *a, BlockMap *b, s32 x, s32 z) {
    VecXZ arr[256];
    s32 j, i, n;
    VecXZ *q = arr;
    do { q->x = 0; q->z = 0; q++; } while (q != arr + 256);
    n = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            u16 *p = BlockMap_GetItemPtr(b, x + 1, z + 1, j, i, 0);
            if (p != 0) {
                if (Unk_020470b8_R(p, 0x26, 0x2a) || (*p >= 0x66 && *p <= 0x68)) {
                    if (Item_IsTreeGrown(p) != 0) {
                        s32 bx, bz;
                        FieldUnit_FromBlockUnit(&bx, &bz, x + 1, z + 1, j, i);
                        arr[n].x = bx;
                        arr[n].z = bz;
                        n++;
                    }
                }
            }
        }
    }
    if (n > 0) {
        s32 idx = Random_GlobalBelow(n);
        VecXZ *pick = arr + idx;
        const u8 *d = data_020c910c;
        for (s32 k = 0; k < 3; d++, k++) {
            s32 px = pick->x + (((s32)*d >> 4) - 8);
            s32 pz = pick->z + ((*d & 0xf) - 8);
            s32 tx = px >> 4;
            s32 tz = pz >> 4;
            u16 *p = BlockMap_GetItemPtr(b, tx, tz, px - (tx << 4), pz - (tz << 4), 0);
            if (p != 0 && *p == 0xfff1) {
                BlockMap_PlaceItem(b, px, pz, Item_RandomAcorn(a), 0);
            }
        }
    }
}
}

namespace nJ {
extern "C" void Town_DropAcorns(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i) {
    s32 cnt = Town_CountEventDays(a, 0x10, (u8 *)c, f, g, h, i);
    for (; cnt != 0; cnt--) {
        s32 x, y;
        for (y = 0; y < e; y++) {
            for (x = 0; x < d; x++) {
                if ((nZ::gTownEval.acres[x][y].flags & 0x800) != 0) {  // (flag bit 11: unk_b11's first bit)
                    Town_DropAcornsInAcre(a, b, x, y);
                }
            }
        }
    }
}
}

namespace nJ {
extern "C" void Town_ApplyAcornEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, s32 d, s32 e, ClockDateTime *f, s32 g, s32 h, SaveData *i) {
    Town_DropAcorns(a, b, c, d, e, f, g, h, i);
}
}

namespace nJ {
extern "C" void Town_ApplyVillagerFlowerEvent(TownUpdater *a, BlockMap *b, EventDayEntry *c, ClockDateTime *d, s32 e, s32 f, SaveData *g) {
    s32 r = Town_CountEventDays(a, 0xe, (u8 *)c, d, e, f, g);
    Town_SpawnVillagerHouseFlowers(a, b, 1, r);
}
}

namespace nJ {
extern "C" void Town_DecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z) {
    VecXZ arr[256];
    s32 j, i, n, k;
    VecXZ *q = arr;
    do { q->x = 0; q->z = 0; q++; } while (q != arr + 256);
    n = 0;
    k = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            u16 *p = BlockMap_GetItemPtr(m, x + 1, z + 1, j, i, 0);
            if (p != 0) {
                if (Unk_020470b8_R(p, 0x5d, 0x61) && Item_IsTreeGrown(p) != 0) {
                    s32 bx, bz;
                    FieldUnit_FromBlockUnit(&bx, &bz, x + 1, z + 1, j, i);
                    arr[n].x = bx;
                    arr[n].z = bz;
                    n++;
                } else if (*p == 0x6d) {
                    k++;
                }
            }
        }
    }
    for (s32 t = 3 - k; t != 0; t--) {
        if (n > 0) {
            s32 idx = Random_GlobalBelow(n);
            VecXZ *pick = arr + idx;
            BlockMap_PlaceItem(m, arr[idx].x, pick->z, 0x6d, 0);
            LitCedarList_Add(arr[idx].x, pick->z);
            arr[idx].x = arr[n - 1].x;
            pick->z = arr[n - 1].z;
            n--;
        }
    }
}
}

namespace nJ {
extern "C" void Town_DecorateCedars(TownUpdater *a, BlockMap *m, s32 n) {
    s32 j, i;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < n; j++) {
            Town_DecorateCedarsInAcre(a, m, j, i);
        }
    }
}
}

namespace nJ {
extern "C" void Town_UndecorateCedarsInAcre(TownUpdater *a, BlockMap *m, s32 x, s32 z) {
    s32 j, i;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            u16 *p = BlockMap_GetItemPtr(m, x + 1, z + 1, j, i, 0);
            if (p != 0 && *p == 0x6d) {
                s32 bx, bz;
                FieldUnit_FromBlockUnit(&bx, &bz, x + 1, z + 1, j, i);
                BlockMap_PlaceItem(m, bx, bz, 0x61, 0);
            }
        }
    }
}
}

namespace nJ {
extern "C" void Town_UndecorateCedars(TownUpdater *a, BlockMap *b, s32 c) {
    s32 j, i;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < c; j++) {
            Town_UndecorateCedarsInAcre(a, b, j, i);
        }
    }
    LitCedarList_Clear();
}
}

namespace nJ {
extern "C" void Town_MarkEventApplied(TownUpdater *a, u32 b) {
    TownState_AddEvent(&gSaveTownState, b);
}
}

namespace nJ {
extern "C" void Town_ApplyDailyEvents(TownUpdater *a, BlockMap *b, s32 c, s32 d, ClockDateTime *e, s32 f, SaveData *g) {
    EventDayEntry *p = Event_GetTodayList();
    u32 t;
    Clock_GetCalendarKey(&t);
    s32 h = ((SaveRecord4 *)g->unk_15fc5)->isDateActive();
    for (s32 i = 0; i < 7; p++, i++) {
        s32 ok;
        if (p->eventId == 99) continue;
        if (p->start > t) continue;
        if (p->end <= t) continue;
        ok = 0;
        switch (p->eventId) {
        case 0x10:
            Town_ApplyAcornEvent(a, b, p, c, d, e, f, h, g);
            ok = 1;
            break;
        case 0x11:
        case 0x61:
            Town_DecorateCedars(a, b, c);
            ok = 1;
            break;
        case 0x62:
            Town_PlaceCountdownSign();
            ok = 1;
            break;
        case 0xe:
            Town_ApplyVillagerFlowerEvent(a, b, p, e, f, h, g);
            ok = 1;
            break;
        }
        if (ok) Town_MarkEventApplied(a, p->eventId);
    }
}
}

namespace nJ {
extern "C" void Town_PlaceVisitorStructures(TownUpdater *a, SaveData *b, u32 c) {
    if (((SaveRecord4 *)b->unk_15fc5)->isDateActive() == 0) {
        ClockDateTimeWords d;
        u8 buf[8];
        d.words[0] = 0;
        d.words[1] = 0;
        Clock_GetDateTime(&d.dt);
        for (s32 i = 0; i < 3; i++) {
            MI_CpuCopy8(&d, buf, 8);
            s32 r = Event_GetState(data_020da2a0[i], buf, c);
            switch (r) {
            case 2:
            case 3:
                switch (i) {
                case 0: Town_PlaceReddTent(); break;
                case 1: Town_PlaceKatrinaTent(); break;
                case 2: Town_PlaceGracieCar(); break;
                }
                break;
            }
        }
    }
}
}

namespace nI {
extern "C" s32 Town_EndEventEffect(TownUpdater *a, s32 id, BlockMap *p, s32 x, s32 y) {
    BOOL r = FALSE;
    switch (id) {
    case 0x10:
        r = TRUE;
        break;
    case 0x11:
    case 0x61:
        Town_UndecorateCedars(a, p, x);
        r = TRUE;
        break;
    case 0x62:
        Town_RemoveCountdownSign();
        r = TRUE;
        break;
    }
    return r;
}
}

namespace nI {
extern "C" void Town_CleanupExpiredEvents(TownUpdater *a, BlockMap *p, s32 x, s32 y, SaveData *d) {
    u32 now;
    TownEventRecord *q;
    s32 i;
    Clock_GetCalendarKey(&now);
    q = (TownEventRecord *)TownState_GetEvent(0);
    for (i = 0; i < 4; q++, i++) {
        u32 id = q->eventId;
        if (id != 0x63) {
            u32 k = q->year;
            if (k == Clock_GetYear() && q->start <= now && q->end >= now) {
            } else {
                if (Town_EndEventEffect(a, id, p, x, y)) {
                    TownState_RemoveEvent(&d->townState, id);
                }
            }
        }
    }
}
}

namespace nI {
extern "C" void Town_RemoveVisitorStructures(TownUpdater *a, s32 b) {
    ClockDateTimeWords s;
    ClockDateTimeWords t1, t2, t3, t4;
    s.words[0] = 0;
    s.words[1] = 0;
    Clock_GetDateTime(&s.dt);
    MI_CpuCopy8(&s, &t1, 8);
    if (Unk_02046d28_Z(Event_GetState(0x3d, &t1, b))) Town_RemoveReddTent();
    MI_CpuCopy8(&s, &t2, 8);
    if (Unk_02046d28_Z(Event_GetState(0x3f, &t2, b))) Town_RemoveKatrinaTent();
    MI_CpuCopy8(&s, &t3, 8);
    if (Unk_02046d28_Z(Event_GetState(0x60, &t3, b))) Town_RemoveGracieCar();
    MI_CpuCopy8(&s, &t4, 8);
    if (Unk_02046d28_Z(Event_GetState(0x44, &t4, b))) Town_RemoveGulliverShip();
}
}

namespace nI {
extern "C" void Town_UpdateEvents(TownUpdater *a, BlockMap *p, s32 x, s32 y, ClockDateTime *c, s32 f) {
    if (!Game_IsIntroPeriod() || (f && !PlayerData_GetCurrent())) {
        ClockDateTimeWords s;
        u8 *q;
        s.words[0] = 0;
        s.words[1] = 0;
        Clock_GetDateTimeCleared(&s);
        DateTime_SubHours(&s.dt, 6);
        q = gSaveTownEventDate;
        if (q[2] != s.dt.year || q[1] != s.dt.month || q[0] != s.dt.day) {
            Town_CleanupExpiredEvents(a, p, x, y, &gSaveData);
            Town_ApplyDailyEvents(a, p, x, y, c, f, &gSaveData);
            q[2] = s.dt.year;
            q[1] = s.dt.month;
            q[0] = s.dt.day;
        }
        Town_RemoveVisitorStructures(a, f);
        Town_PlaceVisitorStructures(a, &gSaveData, f);
    }
}
}

namespace nI {
extern "C" void Town_RefreshEvents(TownUpdater *a, s32 b) {
    TownBlockMap *p = TownBlockMap_Get();
    if (p) {
        ClockDateTimeWords s;
        s32 x, y;
        s.words[0] = 0;
        s.words[1] = 0;
        Clock_GetDateTime(&s.dt);
        x = p->width - 2;
        y = p->height - 2;
        TownEval_Evaluate(&gTownEval, p);
        Town_UpdateEvents(a, p, x, y, &s.dt, b);
    }
}
}

namespace nI {
extern "C" void Town_RefreshEventsOffline() {
    Town_RefreshEvents(&gTownUpdater, 0);
}
}

namespace nI {
extern "C" u16 Flower_GetWateredForm(TownUpdater *a, u16 *q, s32 flag) {
    u16 r = 0xfff1;
    BOOL x6 = TRUE, x5 = TRUE, x4 = TRUE;
    BOOL x3 = FALSE;
    u16 v = *q;
    if (v >= 0x6e && v <= 0x73) x3 = TRUE;
    if (!x3) {
        if (!(v >= 0x74 && v <= 0x79)) x4 = FALSE;
    }
    if (!x4) {
        if (!(v >= 0x7a && v <= 0x7f)) x5 = FALSE;
    }
    if (!x5) {
        if (!(v >= 0x80 && v <= 0x87)) x6 = FALSE;
    }
    if (x6) {
        if (v == 0x86 && flag) {
            r = 0xa5;
        } else {
            r = (u16)(v + 0x1c);
        }
    } else if (v == 0x88) {
        r = 0xa4;
    } else if (v >= 0xd4 && v <= 0xda) {
        r = (u16)(v + 7);
    }
    return r;
}
}

namespace nI {
extern "C" void Town_WaterAll(TownUpdater *a, BlockMap *p, s32 w, s32 h) {
    s32 j;
    s32 i = 0, j0 = 0, z30 = 0, z28 = 0, z20 = 0, z24 = 0;
    for (; i < h; i++) {
        for (j = j0; j < w; j++) {
            u16 *q = (u16 *)BlockMap_GetItemPtr(p, j + 1, i + 1, z20, z20, z20);
            s32 k;
            for (k = z24; k < 0x100; q++, k++) {
                if (q && *q != 0xfff1) {
                    u16 v = Flower_GetWateredForm(a, q, z28);
                    if (v != 0xfff1) {
                        s32 o1, o2;
                        s32 rem = k % 16;
                        s32 quo = k / 16;
                        FieldUnit_FromBlockUnit(&o1, &o2, j + 1, i + 1, rem, quo);
                        BlockMap_PlaceItem(p, o1, o2, v, z30);
                    }
                }
            }
        }
    }
}
}

namespace nI {
extern "C" void Town_AdvanceDays(TownUpdater *a, ClockDateTime *b, ClockDateTime *c, s32 n, u8 e, s32 f) {
    TownBlockMap *p = TownBlockMap_Get();
    if (p) {
        s32 x = p->width - 2;
        s32 y = p->height - 2;
        s32 lim1, i, lim4, lim3, lim2, l, k, j, m;
        ClockDateTimeWords s1, s2;
        Unk_02096354_Arg t3;
        RoomScoreEvaluator obj;
        Town_ClearBorderTrees(p);
        if (e) Town_WaterAll(a, p, x, y);
        TownEval_EvaluateAndClean(&gTownEval, p, x, y);
        TownState_UpdatePerfectStreak(&gSaveTownState, (s32)gTownEval.rank, n);
        lim1 = n;
        if (lim1 > 5) lim1 = 5;
        for (i = 0; i < lim1; i++) Town_UpdateTrees(a, p, x, y);

        s1.words[0] = 0;
        s1.words[1] = 0;
        MI_CpuCopy8(c, &s1, 8);
        lim2 = n;
        if (lim2 > 0x16d) lim2 = 0x16d;
        DateTime_SubDays(&s1.dt, lim2);
        for (j = 0; j < lim2; j++) {
            s32 r = DateTime_GetSeasonPeriod(&s1.dt);
            switch (r) {
            case 0:
            case 1:
            case 2:
            case 0x16:
                Town_SpawnWeeds(a, p);
                break;
            default:
                Town_SpawnWeeds(a, p);
                if (Random_GlobalBelow(100) < 50) Town_SpawnClover(a, p, x, y);
                break;
            }
            DateTime_AddDays(&s1.dt, 1);
        }

        s2.words[0] = 0;
        s2.words[1] = 0;
        MI_CpuCopy8(c, &s2, 8);
        lim3 = n;
        if (lim3 > 0x1e) lim3 = 0x1e;
        DateTime_SubDays(&s2.dt, lim3);
        for (k = 0; k < lim3; k++) {
            s32 r = DateTime_GetSeasonPeriod(&s2.dt);
            switch (r) {
            case 0:
            case 1:
            case 2:
            case 0x16:
                break;
            default:
                if (Random_GlobalBelow(100) < 20) Town_SpawnDandelion(a, p, x, y);
                break;
            }
            DateTime_AddDays(&s2.dt, 1);
        }

        lim4 = n;
        if (lim4 > 0x3c) lim4 = 0x3c;
        Town_UpdateFlowers(a, p);
        Town_BreedFlowers(a, p, x, y);
        Town_SpawnRandomFlower(a, p, x, y);
        if (lim4 > 1) {
            for (l = 1; l < lim4; l++) {
                Town_UpdateFlowersExtraDay(a, p, l);
                Town_BreedFlowers(a, p, x, y);
                Town_SpawnRandomFlower(a, p, x, y);
            }
        }
        Town_UpdateVillagerHouseFlowers(a, p, c, n);
        t3.year = c->year;
        t3.month = c->month;
        t3.day = c->day;
        Town_UpdateEvents(a, p, x, y, c, f);
        Town_SpawnSpecialTrees(a, p, x, y);
        Town_TrySpawnJacobsLadder(a, p, x, y);
        Town_UpdateRafflesia(a, p, x, y);
        Town_BuryFossils(a, p, x, y);
        Town_BuryPitfallSeed(a, p, x, y);
        Town_UpdateSpecialRocks(a, p, x, y);
        Town_UpdateRedTurnips(a, p, n);
        Town_SpawnCoconut(a, p, x);
        Town_BuryGyroids(a, p, x, y, e);
        Town_UpgradeBuriedShovels(a, p, x, y);
        ((ContestRecord *)(gContestRecord))->sendResultLetters();
        ((MuseumData *)(data_021ed0a0))->checkCompletionLetters();
        char *const g = gSaveHouse;
        ((HouseData *)(g))->applyPendingWork();
        NookShop_ApplyRenovation();
        ((HouseData *)(g))->addRoachesForDays(n);
        Save_ConvertFakePaintings();
        LostAndFound_AddDailyItems((u16 *)gSaveData.lostAndFound, n);
        RecycleBin_AdvanceDays((u16 *)gSaveData.recycleBin, c, n);
        RoomScoreEvaluator_Construct(&obj);
        HappyRoom_EvaluateHouse(&obj);
        ((TownMap *)gSaveTownMap)->updateGroundSeason();
        Sky_OnDayChange(n);
        BottleLetter_OnNewDay(n);
        MotherLetter_OnNewDay(&t3, n);
        ((RoostGuestRoll *)data_021e58a7)->roll();
        NookShop_UpdateDaily(data_021ed104, 0);
        LostChild_AdvanceDays(n);
        if (n > 0) {
            char *g2 = gSaveBlancaFace;
            if (((BlancaFaceRecord *)(g2))->getState() == 1) ((BlancaFaceRecord *)(g2))->setState(2);
        }
        SaveVillagers_OnNewDay(gSaveVillagers, c);
        m = Date_GetWeekday(b->year, b->month, b->day);
        s32 q2 = Date_GetWeekday(c->year, c->month, c->day);
        s32 d = q2 - m;
        if (q2 == 0 || n >= 7 || d < 0) Turnips_SpoilAll(a);
        RoomScoreEvaluator_Destruct();
    }
    ChopCount_ClearAll();
    Town_ClearObjectsFcFd(a);
}
}

namespace nI {
extern "C" u16 Item_RandomSeashell(TownUpdater *a) {
    u16 n = 0x1554;
    s32 r = Random_GlobalBelow(100);
    const s32 *q = sSeashellWeights;
    s32 i;
    for (i = 0; i < 9; q++, i++) {
        r -= *q;
        if (r < 0) break;
        n++;
    }
    return n;
}
}

namespace nI {
extern "C" void Town_SpawnSeashellsInAcre(TownUpdater *a, BlockMap *b, s32 c) {
    TownEval_EvaluateAcreAt(&gTownEval, b, c, 3);
    s32 lv = nZ::gTownEval.acres[c][3].shells;
    if (lv < 4) {
        Town_SpawnItemInAcre(a, b, c + 1, 4, Item_RandomSeashell(a), Town_IsSandAt, 0);
    }
}
}

namespace nI {
extern "C" void Town_UpdateSeashellsOffline(TownUpdater *a) {
    u16 t;
    Clock_GetMinuteHour((u8 *)&t);  // {minute, hour}
    if (t != gTownUpdater.seashellTime) {
        if ((s32)(*(u8 *)&t) % 10 == 3) {
            Actor *o = (Actor *)PlayerActor_GetActor(4);
            if (o) {
                TownBlockMap *p = TownBlockMap_Get();
                u32 *o2 = (u32 *)&o->position;
                s32 r = Random_GlobalBelow(4);
                s32 x = (s32)o2[0] >> 17;
                if (!((s32)o2[2] >> 17 == 4 && x == r + 1)) {
                    Town_SpawnSeashellsInAcre(a, p, r);
                }
            }
            gTownUpdater.seashellTime = t;
        }
    }
}
}

namespace nI {
extern "C" void Town_UpdateSeashellsOnline(TownUpdater *a) {
    if (gCommManager->isMyAid(0)) {
        Unk_020465a4_L l;
        s32 x[2];
        s32 y;
        Clock_GetMinuteHour((u8 *)&l.t);  // {minute, hour}
        if (l.t != gTownUpdater.seashellTime) {
            if ((s32)(*(u8 *)&l.t) % 10 == 3) {
                s32 found = 0;
                s32 i;
                for (i = 0; i < 4; i++) {
                    if (PlayerActor_GetSlotPosXZ(&l.a, x, &y, -1, i)) {
                        if (SceneId_IsTown(l.a) || SceneId_IsTownUnk31(l.a)) {
                            if ((s32)y >> 17 == 4) {
                                found = 1;
                                break;
                            }
                        }
                    }
                }
                if (!found) {
                    TownBlockMap *p = TownBlockMap_Get();
                    s32 r = Random_GlobalBelow(4);
                    Town_SpawnSeashellsInAcre(a, p, r);
                }
                gTownUpdater.seashellTime = l.t;
            }
        }
    }
}
}

namespace nI {
extern "C" void Town_UpdateSeashells(TownUpdater *a) {
    if (gCommManager->isOnline()) {
        Town_UpdateSeashellsOnline(a);
    } else {
        Town_UpdateSeashellsOffline(a);
    }
}
}

namespace nH {
extern "C" void Town_RefillSeashells(TownUpdater *p, BlockMap *q) {
    TownBlockMap *m = (TownBlockMap *)q;
    s32 h = m->width - 2;
    s32 i;
    for (i = 0; i < h; i++) {
        s32 n = 2 - nZ::gTownEval.acres[i][3].shells;
        while (n > 0) {
            Town_SpawnItemInAcre(p, q, i + 1, 4, Item_RandomSeashell(p), Town_IsSandAt, 0);
            n--;
        }
    }
    Clock_GetMinuteHour((u8 *)&gTownUpdater.seashellTime);  // {minute, hour}
}
}

namespace nH {
extern "C" s32 Town_WashUpBottle(TownUpdater *p) {
    s32 r5 = Random_GlobalBelow(4);
    TownBlockMap *q = TownBlockMap_Get();
    s32 i;
    for (i = 0; i < 4; i++) {
        if (Town_SpawnItemInAcre(p, q, r5 + 1, 4, 0x1520, Town_IsSandAt, 0) != 0) {
            return 1;
        }
        r5 = (r5 + 1) & 3;
    }
    return 0;
}
}

namespace nH {
extern "C" void BlockMap_SpoilTurnips(void *p, TownBlockMap *q) {
    if (q != 0) {
        Unk_0204da0c_Size *sp = (Unk_0204da0c_Size *)&q->unitsX;
        s32 h, j, i, w, ih;
        w = sp->w;
        h = sp->h;
        for (i = 0; i < h; i++) {
            j = 0;
            if (w > 0) {
                goto test;
            loop:
                {
                    s32 jh = j >> 4;
                    ih = i >> 4;
                    u16 *e = BlockMap_GetItemPtr(q, jh, ih, j - (jh << 4), i - (ih << 4), 0);
                    if (e != 0) {
                        if (Unk_02046358_R1(e)) {
                            s32 t;
                            u16 v, x;
                            if (*e >= 0x1531 && *e <= 0x153a) {
                                t = *e - 0x1531;
                            } else {
                                t = -1;
                            }
                            if ((u32)t < 10) {
                                x = 0x154a + t;
                            } else {
                                x = 0x154a;
                            }
                            v = x;
                            BlockMap_SetItemAtUnit(q, &v, j, i, 0);
                        }
                    }
                }
                j++;
            test:
                if (j < w) goto loop;
            }
        }
    }
}
}

namespace nH {
extern "C" void Players_SpoilTurnips() {
    s32 i, j;
    s32 z = 0;
    s32 z2 = 0;
    for (i = 0; i < 4; i++) {
        if (PlayerDataArray_IsUsed(gSavePlayers, i) != 0) {
            void *q = PlayerData_GetResident(gSavePlayers, i);
            for (j = z; j < 15; j++) {
                void *r = ((PlayerData *)(q))->getInventory();
                u16 *e = ((PlayerInventory *)(r))->getPocket(j);
                if (e != 0) {
                    if (Unk_02046358_R1(e)) {
                        s32 t;
                        u16 w, x;
                        if (*e >= 0x1531 && *e <= 0x153a) {
                            t = *e - 0x1531;
                        } else {
                            t = -1;
                        }
                        if ((u32)t < 10) {
                            x = 0x154a + t;
                        } else {
                            x = 0x154a;
                        }
                        w = x;
                        ((PlayerInventory *)(r))->setPocket((u16 *)(&w), j, z2);
                    }
                }
            }
        }
    }
}
}

namespace nH {
extern "C" void Town_SpoilTurnips(void *p) {
    BlockMap_SpoilTurnips(p, (TownBlockMap *)TownBlockMap_Get());
}
}

namespace nH {
extern "C" void HouseRooms_SpoilTurnips(void *p) {
    s32 i;
    for (i = 0; i < 5; i++) {
        BlockMap_SpoilTurnips(p, (TownBlockMap *)HouseRoomMaps_Get(i));
    }
}
}

namespace nH {
extern "C" void RecycleBin_SpoilTurnips(void *unused) {
    u16 *bin = (u16 *)gSaveData.recycleBin;
    u16 *p = bin;
    s32 i;
    for (i = 0; i < 15; p++, i++) {
        u32 v = *p;
        if (v >= 0x1531 && v <= 0x153a) {
            u16 r;
            s32 t;
            if (v >= 0x1531 && v <= 0x153a) {
                t = v - 0x1531;
            } else {
                t = -1;
            }
            if ((u32)t < 10) {
                r = 0x154a + t;
            } else {
                r = 0x154a;
            }
            bin[i] = r;
        }
    }
}
}

namespace nH {
extern "C" void Turnips_SpoilAll(void *p) {
    Players_SpoilTurnips();
    Town_SpoilTurnips(p);
    HouseRooms_SpoilTurnips(p);
    RecycleBin_SpoilTurnips(p);
}
}

namespace nH {
extern "C" void Town_ApplyElapsedDays(ClockDateTime *a, ClockDateTime *b, s32 c, s32 d, s32 e) {
    TownUpdateThread *t = gTownUpdater.updateThread;
    if (t != 0) {
        TownUpdateThread_Request(a, b, c, d);
    } else {
        Town_AdvanceDays(&gTownUpdater, a, b, c, d, e);
    }
}
}

namespace nH {
extern "C" void TownState_UpdateSeasonPeriod() {
    ClockDateTimeWords t;
    t.words[0] = 0;
    t.words[1] = 0;
    Clock_GetDateTime(&t.dt);
    DateTime_SubHours(&t.dt, 6);
    TownState_SetSeasonPeriod((u8)DateTime_GetSeasonPeriod(&t.dt));
}
}

namespace nH {
extern "C" void Town_UpdateDay(s32 flag) {
    struct {
        Unk_02046a0_Ts t;
        ClockDate c3;
    } l;
    RoomScoreEvaluator o;
    ClockDate *src;
    SaveData *base = &gSaveData;
    s32 dt, days, hours;
    l.t.now.words[0] = 0;
    l.t.now.words[1] = 0;
    l.t.last.words[0] = 0;
    l.t.last.words[1] = 0;
    Clock_GetDateTime(&l.t.now.dt);
    src = &gSaveTownState.lastUpdate;
    l.t.last.words[0] = 0;
    l.t.last.words[1] = 0;
    l.t.last.dt.year = src->year;
    l.t.last.dt.month = src->month;
    l.t.last.dt.day = src->day;
    l.t.last.dt.hour = 6;
    dt = DateTime_DiffMinutes(&l.t.last.dt, &l.t.now.dt);
    days = dt / 60 / 24;
    TownState_UpdateSeasonPeriod();
    if (days >= 1 && flag == 0) {
        if (Scene_InTown() != 0 || Scene_InTownUnk31() != 0) {
            EventWeekSlots_Update();
        }
    }
    TownBbs_UpdateDaily();
    if (days >= 1) {
        dt = Weather_GetPrevDayRain();
        if (flag == 0) {
            if (Scene_InTown() != 0 || Scene_InTownUnk31() != 0) {
                ((SaveRecord4 *)base->unk_15fc5)->expireDate();
            }
        }
        if (flag != 0 || Scene_InTown() != 0 || Scene_InTownUnk31() != 0) {
            Town_ApplyElapsedDays(&l.t.last.dt, &l.t.now.dt, days, dt, flag);
            DateTime_SubHours(&l.t.now.dt, 6);
            src->year = l.t.now.dt.year;
            src->month = l.t.now.dt.month;
            src->day = l.t.now.dt.day;
        }
    } else {
        if (flag != 0) {
            Town_RefreshEvents(&gTownUpdater, 1);
        }
        if (dt < 0) {
            l.c3.year = l.t.now.dt.year;
            l.c3.month = l.t.now.dt.month;
            l.c3.day = l.t.now.dt.day;
            Turnips_SpoilAll(&gTownUpdater);
            TownState_PickNextWeekDate((u8 *)&base->townState, (u8 *)&l.c3);
            RoomScoreEvaluator_Construct(&o);
            HappyRoom_EvaluateHouse(&o);
            RoomScoreEvaluator_Destruct();
        }
        DateTime_SubHours(&l.t.now.dt, 6);
        src->year = l.t.now.dt.year;
        src->month = l.t.now.dt.month;
        src->day = l.t.now.dt.day;
    }
    ((TurnipMarket *)data_021ed29c)->updateDay(days);
    PlayerData_UpdateDay();
}
}

namespace nH {
extern "C" void Town_CheckDayChange() {
    if (EventAnnounce_CanCheck() != 0) {
        if (gCommManager->isOnline() == 0) {
            Unk_02046a0_Ts t;
            t.now.words[0] = 0;
            t.now.words[1] = 0;
            t.last.words[0] = 0;
            t.last.words[1] = 0;
            ClockDate *const src = &gSaveTownState.lastUpdate;
            Clock_GetDateTime(&t.now.dt);
            t.last.words[0] = 0;
            t.last.words[1] = 0;
            t.last.dt.year = src->year;
            t.last.dt.month = src->month;
            t.last.dt.day = src->day;
            t.last.dt.hour = 6;
            s32 d = DateTime_DiffMinutes(&t.last.dt, &t.now.dt) / 60 / 24;
            if (d >= 1) {
                EventAnnounce_Request(-1, 99, 0);
            }
        }
        Town_UpdateSeashells(&gTownUpdater);
    }
}
}

namespace nH {
extern "C" void Town_RebuildLitCedarList() {
    s32 z;
    Unk_02045d40_Ent *q;
    s32 i;
    s32 w;
    u16 *e;
    s32 k;
    s32 j;
    TownBlockMap *p;
    p = (TownBlockMap *)TownBlockMap_Get();
    LitCedarList_Clear();
    if (p != 0) {
        w = p->width - 2;
        q = LitCedarList_Get();
        j = 0; z = 0; k = 0;
        for (; j < 2; j++) {
            i = z;
            for (; i < w; i++) {
                e = BlockMap_GetItemPtr(p, i + 1, j + 1, z, z, z);
                k = 0;
                for (; k < 0x100; e++, k++) {
                    if (*e == 0x6d) {
                        FieldUnit_FromBlockUnit(&q->a, &q->b, i + 1, j + 1, k % 16, k / 16);
                        q++;
                    }
                }
            }
        }
    }
}
}

namespace nH {
extern "C" BOOL Town_IsClockBeforeLastUpdate() {
    BOOL r = FALSE;
    if (MenuCtrl_IsClockMovedBack() != 0) {
        r = TRUE;
    } else {
        Unk_02046a0_Ts t;
        t.now.words[0] = r;
        t.now.words[1] = r;
        t.last.words[0] = r;
        t.last.words[1] = r;
        Clock_GetDateTime(&t.now.dt);
        ClockDate *const src = &gSaveTownState.lastUpdate;
        t.last.words[0] = r;
        t.last.words[1] = r;
        t.last.dt.year = src->year;
        t.last.dt.month = src->month;
        t.last.dt.day = src->day;
        t.last.dt.hour = 6;
        if (DateTime_DiffMinutes(&t.now.dt, &t.last.dt) > 0) {
            r = TRUE;
        }
    }
    return r;
}
}

namespace nH {
extern "C" void Town_OnLoad() {
    TownBlockMap *p = (TownBlockMap *)TownBlockMap_Get();
    Clock_Update(0);
    if (p != 0) {
        s32 w = p->width - 2;
        s32 h = p->height - 2;
        s32 i, j;
        s32 z = 0;
        for (i = 0; i < w; i++) {
            TownEvalAcre *row;
            j = z;
            row = nZ::gTownEval.acres[i];
            for (; j < h; j++) {
                TownEval_EvaluateAcre(&row[j], p, i + 1, j + 1);
            }
        }
        Town_RefillSeashells(&gTownUpdater, p);
    }
    Town_RebuildLitCedarList();
    TownState_UpdateSeasonPeriod();
    if (gCommManager->isOnline() == 0) {
        if (Town_IsClockBeforeLastUpdate() != 0) {
            Turnips_SpoilAll(&gTownUpdater);
        }
        if (Scene_GetCurrent() == 0x3f) {
            Town_RefreshEvents(&gTownUpdater, 1);
        } else {
            Town_UpdateDay(1);
        }
    }
    ChopCount_ClearAll();
    PendingUnits_ClearAll(&sPendingUnits);
    FieldActionFx_ClearAll(sFieldActionFxSlots);
    MoneyRock_Init(&data_021c47bc);
}
}

namespace nH {
extern "C" void Town_InitNew() {
    TownBlockMap *p = (TownBlockMap *)TownBlockMap_Get();
    if (p != 0) {
        s32 w = p->width - 2;
        s32 h = p->height - 2;
        TownEval_EvaluateAndClean(&gTownEval, p, w, h);
        Town_BuryFossils(&gTownUpdater, p, w, h);
        Town_BuryPitfallSeed(&gTownUpdater, p, w, h);
        Town_SpawnSpecialTrees(&gTownUpdater, p, w, h);
        Town_UpdateSpecialRocks(&gTownUpdater, p, w, h);
        Town_RefreshDailyObjects();
    }
}
}

namespace nH {
extern "C" void Field_OnEnter() {
    Town_GetEnvironmentRank();
    TownJunkInsects_InitFromEval(gTownJunkInsectFlags);
    Town_RefreshDailyObjects();
    PendingUnits_Enable();
}
}

namespace nH {
extern "C" s32 Town_GetEnvironmentRank() {
    TownBlockMap *p = TownBlockMap_Get();
    s32 r = 5;
    if (p != 0) {
        r = TownEval_Evaluate(&gTownEval, p);
    }
    return r;
}
}

namespace nH {
extern "C" void *TownEval_GetAdvice() {
    return ((u8 *)&nZ::gTownEval.advice);
}
}

namespace nH {
extern "C" void *Town_GetUpdater() {
    return &gTownUpdater;
}
}

namespace nH {
extern "C" BOOL Town_GetRafflesiaPos(void *out) {
    BOOL r = FALSE;
    s32 a = gTownEval.rafflesiaBlock.x;
    s32 b = gTownEval.rafflesiaBlock.y;
    s32 c = gTownEval.rafflesiaUnit.x;
    s32 d = gTownEval.rafflesiaUnit.y;
    if (a != -1 && b != -1 && c != -1 && d != -1) {
        if (TownBlockMap_Get() != 0) {
            FieldPos_FromBlockUnit(out, a + 1, b + 1, c, d);
            r = TRUE;
        }
    }
    return r;
}
}

namespace nH {
extern "C" s32 LitCedarList_Find(s32 a, s32 b) {
    s32 r = -1;
    if (gCommManager->isOnline() == 0) {
        Unk_02045d40_Ent *p = LitCedarList_Get();
        s32 i;
        for (i = 0; i < 24; i++) {
            if (p->a == a && p->b == b) {
                r = i;
                break;
            }
            p++;
        }
    } else {
        r = (a + b) % 24;
    }
    return r;
}
}

namespace nH {
extern "C" Unk_02045d40_Ent *LitCedarList_Get() {
    return (Unk_02045d40_Ent *)sLitCedars;
}
}

namespace nH {
extern "C" void LitCedarList_Add(s32 a, s32 b) {
    Unk_02045d40_Ent *p = LitCedarList_Get();
    s32 i;
    for (i = 0; i < 24; i++) {
        if (p->a == -1 && p->b == -1) {
            p->a = a;
            p->b = b;
            break;
        }
        p++;
    }
}
}

namespace nH {
extern "C" void LitCedarList_Clear() {
    Unk_02045d40_Ent *p = LitCedarList_Get();
    s32 i;
    for (i = 0; i < 24; i++) {
        p->a = -1;
        p->b = -1;
        p++;
    }
}
}

namespace nH {
extern "C" void Town_RefreshDailyObjects() {
    if (gCommManager->isOnline() == 0) {
        ChopCount_ClearAll();
        Town_ClearObjectsFcFd(&gTownUpdater);
    }
}
}

namespace nH {
extern "C" void TownUpdater_MarkEventApplied(u32 id) {
    Town_MarkEventApplied(&gTownUpdater, id);
}
}

namespace nH {
extern "C" void Field_ResetActions() {
    FieldActions_Init(&sFieldActions);
    PendingUnits_ClearAll(&sPendingUnits);
}
}

namespace nH {
extern "C" void Field_UpdateActions() {
    FieldActions_Update(&sFieldActions);
    TownJunkInsects_Apply(gTownJunkInsectFlags);
}
}

namespace nH {
extern "C" void PendingUnit_Clear(PendingUnit *p) {
    p->aid = 7;
    p->unit.v = 0xffff;
    p->item = 0xfff1;
    p->unitFlag = 0;
    p->committed = 0;
    p->pending = 0;
    p->slot = 7;
    p->layer = 0;
    p->unk_02_c = 15;
}
}

namespace nG {
extern "C" void PendingUnit_Set(PendingUnit *e, u8 aid, u16 pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g) {
    e->aid = aid;
    e->unit.v = pos;
    e->item = a;
    e->oldItem = c;
    e->kind = k;
    e->unitFlag = b;
    e->mode = c2;
    e->slot = d;
    e->layer = f;
    e->unk_02_c = g;
    e->pending = 1;
}
}

namespace nG {
extern "C" void PendingUnit_WriteToMap(PendingUnit *e) {
    if (Scene_InTown()) {
        Field_SetUnitItem(e->unit.v >> 8, e->unit.v & 0xff, e->item, e->unitFlag);
        PendingUnit_NoteJunk(e);
    } else {
        Room_SetItemAtUnit(e->unit.v >> 8, e->unit.v & 0xff, e->item, e->layer);
    }
}
}

namespace nG {
extern "C" void PendingUnit_NoteJunk(PendingUnit *e) {
    StackPad8 pad;
    u16 v = e->item;
    if (v >= 0x154a && v <= 0x1553) {
        gTownJunkInsectFlags[1] = 1;
    } else if (v >= 0x1320 && v <= 0x1322) {
        gTownJunkInsectFlags[0] = 1;
    }
}
}

namespace nG {
extern "C" void PendingUnit_Apply(PendingUnit *e) {
    s32 flag = 0;
    if (Scene_InTown()) {
        TownBlockMap *p = TownBlockMap_Get();
        if (p) {
            s32 x, y;
            y = e->unit.v & 0xff;
            x = e->unit.v >> 8;
            s32 xh = x >> 4;
            s32 yh = y >> 4;
            u16 *r = BlockMap_GetItemPtr(p, xh, yh, x - (xh << 4), y - (yh << 4), 0);
            if (r) {
                switch (e->kind) {
                case 18:
                    PendingUnit_ClearActiveOfAid(e->aid);
                case 17:
                case 19:
                case 24:
                case 26: {
                    BOOL x8 = TRUE, x7 = TRUE, x6 = TRUE, x5 = TRUE, x4 = TRUE, x3 = TRUE, x2 = TRUE, x1 = FALSE;
                    u16 t = *r;
                    if (t <= 5) {
                        x1 = TRUE;
                    }
                    if (!x1) {
                        if (t < 6 || t > 11) {
                            x2 = FALSE;
                        }
                    }
                    if (!x2) {
                        if (t < 12 || t > 17) {
                            x3 = FALSE;
                        }
                    }
                    if (!x3) {
                        if ((t < 18 || t > 25) && t != 28) {
                            x4 = FALSE;
                        }
                    }
                    if (!x4) {
                        if ((t < 0x8a || t > 0x8f) && (t < 0x90 || t > 0x95) && (t < 0x96 || t > 0x9b) && (t < 0x9c || t > 0xa3) && t != 0xa5) {
                            x5 = FALSE;
                        }
                    }
                    if (!x5) {
                        if (t != 0x1a) {
                            x6 = FALSE;
                        }
                    }
                    if (!x6) {
                        if (t != 0xa4) {
                            x7 = FALSE;
                        }
                    }
                    if (!x7) {
                        if (t != 0x1d) {
                            x8 = FALSE;
                        }
                    }
                    if (!x8) {
                        if (t >= 0x26 && t <= 0x2a) break;
                        if (t >= 0x5d && t <= 0x61) break;
                        if (t >= 0x2f && t <= 0x56) break;
                        if (t >= 0x57 && t <= 0x5b) break;
                        if (t >= 0x66 && t <= 0x68) break;
                        if (t == 0x69) break;
                        if (t >= 0x6a && t <= 0x6c) break;
                        if (t == 0x6d) break;
                        if (t >= 0xc8 && t <= 0xcf) break;
                        if (e->unitFlag != 0) {
                            flag = 1;
                        }
                    }
                    break;
                }
                case 6:
                case 7: {
                    if (e->mode != 2) {
                        u32 a = e->unitFlag;
                        Vec2 q;
                        q.x = x;
                        q.y = y;
                        ChopCount_Set(&sFieldActions, &q, (a + 1) & 3);
                    }
                    break;
                }
                case 10:
                    flag = e->unitFlag;
                    break;
                case 25:
                    if (e->unk_02_c >= 0) {
                        e->item = 0xfff1;
                    }
                    break;
                case 8:
                case 9:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 20:
                case 21:
                case 22:
                case 23:
                    break;
                }
            }
        }
    }
    e->unitFlag = (u8)flag;
    PendingUnit_WriteToMap(e);
    PendingUnit_Clear(e);
}
}

namespace nG {
extern "C" void PendingUnit_Commit(PendingUnit *e) {
    switch (e->kind) {
    case 4:
    case 22:
    case 25:
        PendingUnit_WriteToMap(e);
        e->committed = 1;
        e->pending = 0;
        break;
    case 10:
        PendingUnit_WriteToMap(e);
        e->committed = 1;
        e->pending = 0;
        e->item = e->oldItem;
        e->unitFlag = 1;
        break;
    default:
        PendingUnit_Apply(e);
        break;
    }
}
}

namespace nG {
extern "C" void PendingUnit_Reset(PendingUnit *e) {
    PendingUnit_Clear(e);
}
}

namespace nG {
extern "C" void PendingUnit_Flush(PendingUnit *e) {
    if (e->unit.v != 0xffff) {
        switch (e->kind) {
        case 4:
        case 10:
        case 22:
        case 25:
            PendingUnit_Clear(e);
            break;
        default:
            PendingUnit_Apply(e);
            break;
        }
    }
}
}

namespace nG {
extern "C" void PendingUnit_SetIndex(s32 idx, u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g) {
    PackedUnitPos p;
    p.b.x = pos->x;
    p.b.z = pos->y;
    PendingUnit_Set(&nZ::sPendingUnits.entries[idx], aid, p.v, a, c, k, b, c2, d, f, g);
}
}

namespace nG {
extern "C" BOOL PendingUnit_CanAddAt(Vec2 *pos) {
    BOOL result = TRUE;
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, 0);
    if (i >= 0) {
        if (PendingUnit_Get(i)->kind == 26) {
            PendingUnit_ApplyIndex(i);
        } else {
            result = FALSE;
        }
    }
    return result;
}
}

namespace nG {
extern "C" BOOL PendingUnit_Add(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g) {
    BOOL result = FALSE;
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    if (PendingUnit_CanAddAt(&t)) {
        s32 i = PendingUnit_FindFree();
        if (i >= 0) {
            Vec2 t2;
            t2.x = pos->x;
            t2.y = pos->y;
            PendingUnit_SetIndex(i, aid, &t2, a, c, k, b, c2, d, f, g);
            result = TRUE;
        }
    }
    return result;
}
}

namespace nG {
extern "C" BOOL PendingUnit_Reserve(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g) {
    BOOL result = FALSE;
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, f);
    if (i >= 0) {
        PendingUnit *e = &nZ::sPendingUnits.entries[i];
        if ((aid & 3) == nZ::sPendingUnits.entries[i].aid && e->committed != 0) {
            Vec2 t2;
            t2.x = pos->x;
            t2.y = pos->y;
            PendingUnit_SetIndex(i, aid, &t2, a, c, k, b, c2, d, f, g);
            result = TRUE;
        }
    } else {
        Vec2 t3;
        t3.x = pos->x;
        t3.y = pos->y;
        result = PendingUnit_Add(aid, &t3, a, c, k, b, c2, d, f, g);
    }
    return result;
}
}

namespace nG {
extern "C" void PendingUnit_Replace(u8 aid, Vec2 *pos, u16 a, u16 c, u8 k, u8 b, u8 c2, u8 d, u8 f, s8 g) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, f);
    if (i >= 0) {
        PendingUnit *e = &nZ::sPendingUnits.entries[i];
        if (e->committed == 0) {
            switch (e->kind) {
            case 1:
            case 2:
            case 6:
            case 7: {
                PackedUnitPos q;
                q.v = 0xfff1;
                q.v = e->oldItem;
                Vec2 t2;
                t2.x = pos->x;
                t2.y = pos->y;
                Tree_DropItems(&q, e->aid, &t2);
                break;
            }
            }
            PendingUnit_Apply(e);
        }
        Vec2 t3;
        t3.x = pos->x;
        t3.y = pos->y;
        PendingUnit_SetIndex(i, aid, &t3, a, c, k, b, c2, d, f, g);
    } else {
        Vec2 t4;
        t4.x = pos->x;
        t4.y = pos->y;
        PendingUnit_Add(aid, &t4, a, c, k, b, c2, d, f, g);
    }
}
}

namespace nG {
extern "C" void PendingUnit_ApplyAt(Vec2 *pos, s32 flag) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    PendingUnit_ApplyIndex(PendingUnit_Find(&t, flag));
}
}

namespace nG {
extern "C" void PendingUnit_ApplyIndex(s32 i) {
    if (i >= 0) {
        PendingUnit_Apply(&nZ::sPendingUnits.entries[i]);
    }
}
}

namespace nG {
extern "C" void PendingUnit_ApplyAtIfAid(Vec2 *pos, u32 v, s32 flag) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, flag);
    if (i >= 0) {
        PendingUnit *e = &nZ::sPendingUnits.entries[i];
        if ((u8)(v & 3) == nZ::sPendingUnits.entries[i].aid) {
            PendingUnit_Apply(e);
        }
    }
}
}

namespace nG {
extern "C" void PendingUnit_CancelAt(Vec2 *pos, s32 flag) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, flag);
    if (i >= 0) {
        PendingUnit *e = &nZ::sPendingUnits.entries[i];
        Vec2 xy;
        Unk_0204548c_Unpack(e->unit, e->unit, &xy);
        if (FieldItemFx_CancelAt(nZ::sPendingUnits.entries[i].aid, &xy) == 0) {
            switch (e->kind) {
            case 4:
            case 10:
            case 22:
            case 25:
                PendingUnit_CommitForAid(e);
                break;
            default:
                PendingUnit_Apply(e);
                break;
            }
        }
    }
}
}

namespace nG {
extern "C" void PendingUnit_CommitAt(Vec2 *pos, s32 flag) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    s32 i = PendingUnit_Find(&t, flag);
    if (i >= 0) {
        PendingUnit_CommitForAid(&nZ::sPendingUnits.entries[i]);
    }
}
}

namespace nG {
extern "C" void PendingUnit_CommitForAid(PendingUnit *e) {
    PendingUnit *p = nZ::sPendingUnits.entries;
    for (s32 i = 0; i < 20; p++, i++) {
        if (p->aid == e->aid && p->committed != 0) {
            p->committed = 0;
        }
    }
    PendingUnit_Commit(e);
}
}

namespace nG {
extern "C" void PendingUnit_ClearActiveOfAid(u32 aid) {
    s32 i = PendingUnit_FindActiveOfAid(aid);
    if (i >= 0) {
        PendingUnit_Clear(&nZ::sPendingUnits.entries[i]);
    }
}
}

namespace nG {
extern "C" s32 PendingUnit_IndexAt(Vec2 *pos, s32 flag) {
    Vec2 t;
    t.x = pos->x;
    t.y = pos->y;
    return PendingUnit_Find(&t, flag);
}
}

namespace nG {
extern "C" void PendingUnits_Enable() {
    sPendingUnits.enabled = 1;
}
}

namespace nG {
extern "C" void PendingUnits_Flush() {
    PendingUnit *e = PendingUnit_Get(0);
    for (s32 i = 0; i < 20; e++, i++) {
        PendingUnit_Flush(e);
    }
    sPendingUnits.enabled = 0;
}
}

namespace nG {
extern "C" s32 PendingUnit_Find(Vec2 *pos, s32 flag) {
    s32 result = -1;
    PendingUnit *e = nZ::sPendingUnits.entries;
    for (s32 i = 0; i < 20; e++, i++) {
        if (pos->x == Unk_021c3f8c_GetX(e->unit) && pos->y == Unk_021c3f8c_GetY(e->unit) && flag == e->layer) {
            result = i;
            break;
        }
    }
    return result;
}
}

namespace nG {
extern "C" s32 PendingUnit_FindByAid(s32 aid, Vec2 *pos, s32 flag) {
    s32 result = -1;
    PendingUnit *e = nZ::sPendingUnits.entries;
    for (s32 i = 0; i < 20; e++, i++) {
        if (aid == e->aid && pos->x == Unk_021c3f8c_GetX(e->unit) && pos->y == Unk_021c3f8c_GetY(e->unit) && flag == e->layer) {
            result = i;
            break;
        }
    }
    return result;
}
}

namespace nF {
extern "C" s32 PendingUnit_FindForAid(u32 a, Vec2 *p, s32 b) {
    Vec2 t;
    s32 x = Field_AidOrZero(a);
    t.x = p->x;
    t.y = p->y;
    return PendingUnit_FindByAid(x, &t, b);
}
}

namespace nF {
extern "C" s32 PendingUnit_FindFree() {
    Vec2 p;
    p.x = 0xff;
    p.y = 0xff;
    return PendingUnit_Find(&p, 0);
}
}

namespace nF {
extern "C" s32 PendingUnit_FindActiveOfAid(u32 a) {
    PendingUnit *e;
    s32 i, r;
    i = 0;
    r = -1;
    a = (u8)(a & 3);
    e = nZ::sPendingUnits.entries;
    for (; i < 20; e++, i++) {
        if (a == e->aid && e->committed) {
            r = i;
            break;
        }
    }
    return r;
}
}

namespace nF {
extern "C" s32 PendingUnit_FindBySlot(u32 a, u32 b) {
    s32 r = -1;
    if (!gCommManager->isOnline()) {
        a = 0;
    }
    PendingUnit *e = nZ::sPendingUnits.entries;
    for (s32 i = 0; i < 20; e++, i++) {
        if (a == e->aid && b == e->slot) {
            r = i;
            break;
        }
    }
    return r;
}
}

namespace nF {
extern "C" PendingUnit *PendingUnit_Get(u32 i) {
    return &nZ::sPendingUnits.entries[i];
}
}

namespace nF {
extern "C" u16 *PendingUnit_GetActivePosOfAid(u32 a) {
    PendingUnit *e;
    s32 i;
    u16 *r;
    r = NULL;
    if (!gCommManager->isOnline()) {
        a = 0;
    }
    e = nZ::sPendingUnits.entries;
    for (i = 0; i < 20; e++, i++) {
        if (a == e->aid && e->committed) {
            r = &e->unit.v;
            break;
        }
    }
    return r;
}
}

namespace nF {
extern "C" void PendingUnits_ClearAll(PendingUnitList *a) {
    PendingUnit *p = a->entries;
    for (s32 i = 0; i < 20; i++) {
        PendingUnit_Reset(p);
        p++;
    }
}
}

namespace nF {
extern "C" BOOL Field_IsUnitClearOfOthers(Vec2 *p, s32 idx) {
    VecFx32 v;
    if (!gCommManager->isOnline()) {
        return TRUE;
    }
    FieldPos_FromUnitCenter(&v, p->x, p->y);
    for (s32 i = 0; i < 4; i++) {
        if (i != idx) {
            Actor *o = (Actor *)PlayerActor_GetActor(i);
            if (o) {
                VecFx32 *q = &o->position;
                s32 dx = v.x - q->x;
                s32 dz = v.z - q->z;
                s32 a = func_01ffcb0c(dx, dx);
                s32 b = func_01ffcb0c(dz, dz);
                if (a + b < 0x1000) {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}
}

namespace nF {
extern "C" BOOL Field_IsUnitClearOfOthersForAid(Vec2 *p, u32 x) {
    Vec2 t;
    t.x = p->x;
    t.y = p->y;
    return Field_IsUnitClearOfOthers(&t, Field_AidOrLocal(x));
}
}

namespace nF {
extern "C" BOOL Field_IsUnitClearOfOthersLocal(Vec2 *p) {
    Vec2 t;
    s32 owner = gCommManager->localSlot;
    t.x = p->x;
    t.y = p->y;
    return Field_IsUnitClearOfOthers(&t, owner);
}
}

namespace nF {
extern "C" BOOL FieldAction_CanReserveUnit(u32 idx, Vec2 *p) {
    volatile u16 v = 0xfff1;
    BOOL res = FALSE;
    FieldActionRequest *e = &sFieldActions.requests[idx];
    Vec2 t;
    s32 r;
    s32 fl;
    v = e->oldItem;
    fl = e->layer;
    t.x = p->x;
    t.y = p->y;
    r = PendingUnit_IndexAt(&t, fl);
    if (r < 0) {
        Vec2 t2;
        t2.x = p->x;
        t2.y = p->y;
        if (Field_IsUnitClearOfOthersLocal(&t2)) {
            res = TRUE;
        }
    } else {
        PendingUnit *g = &nZ::sPendingUnits.entries[r];
        if (g->aid == (u8)(e->aid & 3)) {
            if (g->unk_02_c == e->unk_1e) {
                if (g->committed) {
                    res = TRUE;
                }
            }
        }
    }
    return res;
}
}

namespace nF {
extern "C" BOOL FieldAction_PreApplyOffline(FieldActionRequestMsg *s, FieldActionRequest *e) {
    volatile u16 v = 0xfff1;
    BOOL res = FALSE;
    v = s->item;
    switch (s->kind) {
    case 0x13: {
        u32 c;
        if (Unk_020449e8_R(&v, c, 0x1518, 0x151c)) {
            s->item = sFruitSaplings2[c - 0x1518];
        } else {
            switch (c) {
            case 0x1548:
                s->item = 0xc8;
                break;
            case 0x151d:
                s->item = 0x26;
                break;
            case 0x151e:
                s->item = 0x5d;
                break;
            }
        }
        break;
    }
    case 0xf: {
        Vec2 t;
        u32 r = Field_AidOrLocal(e->aid);
        t.x = e->unit.x;
        t.y = e->unit.y;
        FieldAction_WaterFlowers(r, &t);
        res = TRUE;
        break;
    }
    case 0xc:
    case 0xd:
    case 0x17:
        res = TRUE;
        break;
    case 0xe: {
        Vec2 t;
        t.x = e->unit.x;
        t.y = e->unit.y;
        MoneyRock_OnHit(&data_021c47bc, &t);
        break;
    }
    }
    return res;
}
}

namespace nF {
extern "C" void FieldAction_Submit(u8 idx, u32 arg) {
    BlockMap *o = gSceneBlockMap;
    FieldActionRequest *e = &sFieldActions.requests[idx];
    FieldActionRequestMsg s;
    Vec2 p;
    Vec2 p2;
    FieldActionResultMsg out;
    u16 *t;
    s32 x, y;
    if (o == NULL) {
        e->state = 3;
        return;
    }
    x = e->unit.x;
    y = e->unit.y;
    {
        s32 tx = x >> 4;
        s32 ty = y >> 4;
        t = BlockMap_GetItemPtr(o, tx, ty, x - (tx << 4), y - (ty << 4), 0);
    }
    if (t == NULL) {
        e->state = 3;
        return;
    }
    p.x = x;
    p.y = y;
    if (!FieldAction_CanReserveUnit(idx, &p)) {
        e->state = 3;
        return;
    }
    s.aid = e->aid;
    s.requestIndex = idx;
    s.unitFlag = e->unk_1d;
    s.kind = e->kind;
    ((u8 *)&s.unit)[1] = x;
    ((u8 *)&s.unit)[0] = y;
    s.oldItem = *t;
    s.item = e->item;
    s.mode = (u8)arg;
    s.layer = e->layer;
    CommManager *g = gCommManager;
    if (g->isOnline()) {
        if (NetArea_IsLocalOwner()) {
            FieldAction_HostProcess(&s, 1, g->myAid);
        } else {
            g = gCommManager;
            g->beginRecord();
            g->writeRecord((u8 *)(&s), 8);
            g->endRecord(0x31, 6);
        }
    } else {
        e->state = 2;
        if (!FieldAction_PreApplyOffline(&s, e)) {
            s8 sb = e->unk_1e;
            p2.x = e->unit.x;
            p2.y = e->unit.y;
            if (!PendingUnit_Reserve(0, &p2, s.item, s.oldItem, s.kind, s.unitFlag, s.mode, 4, s.layer, sb)) {
                e->state = 3;
            } else {
                s32 px = *(volatile s32 *)&e->unit.x;
                s32 py = *(volatile s32 *)&e->unit.y;
                u8 *pp = (u8 *)&out.unit;
                pp[1] = px;
                pp[0] = py;
                TreeDrop_Spawn(&out, &s, 0);
            }
        }
    }
}
}

namespace nF {
extern "C" BOOL TreeDrop_FindUnit(FieldActionResultMsg *a, Vec2 *p, u32 idx, BlockMap *obj) {
    s32 i, z0, z1;
    const u32 *base;
    const u32 *e;
    base = sTreeDropSearchOrder[idx];
    e = base;
    i = 0;
    z1 = 0;
    z0 = 0;
    for (; i < 8; e++, i++) {
        u32 d = sTreeDropOffsets[*e];
        s32 x = p->x + (((s32)d >> 4) - 8);
        s32 y = p->y + ((d & 0xf) - 8);
        s32 tx = x >> 4;
        s32 ty = y >> 4;
        u16 *t = BlockMap_GetItemPtr(obj, tx, ty, x - (tx << 4), y - (ty << 4), z0);
        if (t && *t == 0xfff1 && ((BlockMap *)obj)->canPlaceItem(x, y)) {
            Vec2 q;
            q.x = x;
            q.y = y;
            if (PendingUnit_IndexAt(&q, z1) < 0) {
                p->x = x;
                p->y = y;
                return TRUE;
            }
        }
    }
    u32 d = sTreeDropOffsets[*base];
    p->x = p->x + (((s32)d >> 4) - 8);
    p->y = p->y + ((d & 0xf) - 8);
    return FALSE;
}
}

namespace nF {
extern "C" u32 Tree_GetDropItem(FieldActionResultMsg *a, u32 id) {
    u32 r;
    switch (id) {
    case 0xcc:
        r = 0x1548;
        break;
    case 0x5b:
        r = 0x14b8;
        break;
    default: {
        u16 v = 0xfff1;
        v = id;
        r = sTreeDropFruit[Item_GetFruitTreeFruit(&v)];
    }
    }
    return r;
}
}

namespace nF {
extern "C" void TreeDrop_SpawnFruit(FieldActionResultMsg *o, u32 x, u32 code) {
    Vec2 pos;
    pos.x = 0;
    pos.y = 0;
    TownBlockMap *owner;
    u32 val;
    s32 n;
    owner = TownBlockMap_Get();
    if (owner != 0) {
        volatile u16 vx, vy, vp;
        s32 i;
        switch (code) { case 0xcc: n = 2; break; default: n = 3; break; }
        val = Tree_GetDropItem(o, code);
        for (i = 0; i < n; i++) {
            s32 d;
            vp = o->unit;
            u16 t = vp;
            vy = t;
            d = sTreeDropOffsets[sTreeDropSlots[i]];
            vx = t;
            pos.x = (vx >> 8) + ((d >> 4) - 8);
            pos.y = (vy & 0xff) + ((d & 0xf) - 8);
            if (TreeDrop_FindUnit(o, &pos, i, owner)) {
                Vec2 q;
                q.x = pos.x;
                q.y = pos.y;
                PendingUnit_Reserve(x, &q, val, 0xfff1, 0, 0, 0, i, 0, -1);
                s32 sy = pos.y;
                s32 sx = pos.x;
                o->dropUnitBytes[i][1] = sx;
                o->dropUnitBytes[i][0] = sy;
            } else {
                *(u16 *)o->dropUnitBytes[i] = 0xffff;
            }
        }
        o->dropItem = val;
    }
}
}

namespace nF {
extern "C" void TreeDrop_SpawnSpecial(FieldActionResultMsg *o, FieldActionRequestMsg *s, u32 x) {
    volatile u16 v0 = 0xfff1;
    volatile u16 v2;
    u16 ret;
    volatile u16 x1, y1, x2, y2, p1, p2;
    Vec2 pos;
    Vec2 q;
    VecFx32 vec;
    TownBlockMap *owner;
    u32 code;
    pos.x = 0;
    pos.y = 0;
    owner = TownBlockMap_Get();
    if (owner != 0) {
        v0 = s->oldItem;
        BOOL ok = TRUE;
        BOOL rr = FALSE;
        u32 c = v0;
        u32 b = v0;
        if (b >= 0x66 && c <= 0x68) rr = TRUE;
        if (!(rr || (c >= 0x6a && c <= 0x6c))) {
            ok = FALSE;
        }
        if (ok) {
            u32 sc = ((volatile FieldActionRequestMsg *)s)->oldItem;
            if (sc == 0x68 || sc == 0x6c) {
                code = 0x1492;
            } else {
                v2 = 0xfff1;
                {
                    ItemPickSpec rng;
                    rng.set(0, 0);
                    ItemPick_One(&ret, &rng, 0, 0, 1, 1, 0);
                    v2 = ret;
                }
                code = v2;
            }
        } else {
            code = 0x1492;
        }
        p1 = s->unit;
        u16 t1 = p1;
        y1 = t1;
        x1 = t1;
        FieldPos_FromUnitCenter(&vec, x1 >> 8, y1 & 0xff);
        u32 idx = Tree_GetDropSide(x, &vec);
        p2 = o->unit;
        u16 t2 = p2;
        y2 = t2;
        s32 d = sTreeDropOffsets[sTreeDropSlots[idx]];
        x2 = t2;
        pos.x = (x2 >> 8) + ((d >> 4) - 8);
        pos.y = (y2 & 0xff) + ((d & 0xf) - 8);
        if (TreeDrop_FindUnit(o, &pos, idx, owner)) {
            q.x = pos.x;
            q.y = pos.y;
            PendingUnit_Reserve(x, &q, code, 0xfff1, 0, 0, 0, 0, 0, -1);
            s32 sy = pos.y;
            s32 sx = pos.x;
            ((u8 *)&o->dropUnits[0])[1] = sx;
            ((u8 *)&o->dropUnits[0])[0] = sy;
        } else {
            o->dropUnits[0] = 0xffff;
        }
        o->dropItem = code;
    }
}
}

namespace nF {
extern "C" BOOL TreeDrop_Spawn(FieldActionResultMsg *o, FieldActionRequestMsg *s, u32 x) {
    BOOL res = FALSE;
    switch (s->kind) {
    case 1:
    case 6: {
        volatile u16 v = 0xfff1;

        for (s32 i = 0; i < 3; i++) {
            o->dropUnits[i] = 0xffff;
        }
        v = s->oldItem;
        BOOL rr = FALSE;
        u32 c = v;
        u32 b = v;
        if (b >= 0x2f && c <= 0x56) rr = TRUE;
        if (rr || (c >= 0xc8 && c <= 0xcf) || (c >= 0x57 && c <= 0x5b)) {
            if (Item_IsTreeGrown((u16 *)&v)) {
                TreeDrop_SpawnFruit(o, x, s->oldItem);
                res = TRUE;
            }
        } else if (c == 0x67 || c == 0x6b) {
            if (Town_CanReleaseBees()) {
                TreeDrop_SpawnSpecial(o, s, x);
                res = TRUE;
            }
        } else if ((c >= 0x66 && c <= 0x68) || (c >= 0x6a && c <= 0x6c)) {
            TreeDrop_SpawnSpecial(o, s, x);
            res = TRUE;
        }
        break;
    }
    }
    return res;
}
}

namespace nE {
extern "C" void FieldAction_HostProcess(FieldActionRequestMsg *src, u8 flag, s32 t) {
    s32 mask = 6;
    FieldActionResultMsg e;
    s32 i;
    e.hasDrops = 0;
    e.aid = (u8)t;
    e.kind = src->kind;
    e.requestIndex = src->requestIndex;
    e.accepted = flag;
    e.mode = src->mode;
    e.layer = src->layer;
    e.unitFlag = src->unitFlag;
    Unk_02044774_PP pp = *(Unk_02044774_PP *)&src->unit;
    *(Unk_02044774_PP *)&e.unit = pp;
    e.item = src->item;
    for (i = 0; i < 3; i++) {
        ((u16 *)&e + i)[3] = 0xffff;
    }
    s32 x = e.unit >> 8;
    s32 z = e.unit & 0xff;
    if (flag != 0) {
        switch (e.kind) {
        case 12:
        case 13:
        case 15:
        case 23:
            break;
        case 24: {
            Vec2 p;
            p.x = x;
            p.y = z;
            PendingUnit_Reserve((u8)t, &p, e.item, 0xfff1, e.kind, e.unitFlag, e.mode, 4, e.layer, -1);
            VecXZ q;
            q.x = x;
            q.z = z;
            FieldItemFx_StartPop(t, e.item, Unk_02042588_Pos(q.x, q.z));
            break;
        }
        case 26: {
            Vec2 p;
            p.x = x;
            p.y = z;
            PendingUnit_Reserve((u8)t, &p, e.item, 0xfff1, e.kind, e.unitFlag, e.mode, 4, e.layer, -1);
            if (!Field_IsLocalAid(t)) {
                VecXZ q;
                q.x = x;
                q.z = z;
                FieldItemFx_StartPop(t, e.item, Unk_02042588_Pos(q.x, q.z));
            }
            break;
        }
        default: {
            u32 h = src->oldItem;
            Vec2 p;
            p.x = x;
            p.y = z;
            PendingUnit_Reserve((u8)t, &p, e.item, h, e.kind, e.unitFlag, e.mode, 4, e.layer, -1);
            if (TreeDrop_Spawn(&e, src, t)) {
                e.hasDrops = 1;
                mask = 14;
            }
            break;
        }
        }
        if (Field_IsLocalAid(t)) {
            FieldActionRequest *r = &sFieldActions.requests[src->requestIndex];
            r->state = 2;
        } else {
            FieldActionFx_Start(&e);
        }
    } else {
        if (Field_IsLocalAid(t)) {
            FieldActionRequest *r = &sFieldActions.requests[src->requestIndex];
            r->state = 3;
        }
    }
    CommManager *g = gCommManager;
    g->beginRecord();
    g->writeRecord((u8 *)(void *)(&e), mask);
    g->endRecord(0x32, 4);
}
}

namespace nE {
extern "C" void FieldAction_ApplyResult(FieldActionResultMsg *e) {
    s32 x = e->unit >> 8;
    s32 z = e->unit & 0xff;
    switch (e->kind) {
    case 12:
    case 13:
    case 15:
    case 23:
        return;
    case 24:
    case 26: {
        VecXZ p;
        p.x = x;
        p.z = z;
        FieldItemFx_StartPop(e->aid, e->item, Unk_02042588_Pos(p.x, p.z));
        break;
    }
    }
    u32 h = 0xfff1;
    if (gSceneBlockMap != 0) {
        s32 xh = x >> 4;
        s32 zh = z >> 4;
        h = *(u16 *)BlockMap_GetItemPtr(gSceneBlockMap, xh, zh, x - (xh << 4), z - (zh << 4), 0);
    }
    Vec2 p1;
    s32 f7 = e->layer;
    s32 f5 = e->mode;
    s32 f3 = e->unitFlag;
    s32 t3 = e->kind;
    p1.x = x;
    p1.y = z;
    PendingUnit_Replace(e->aid, &p1, e->item, h, t3, f3, f5, 4, f7, -1);
    if (e->hasDrops) {
        u16 w = e->dropItem;
        s32 zero = 0;
        s32 i;
        for (i = 0; i < 3; i++) {
            u16 v = e->dropUnits[i];
            s32 px = v >> 8;
            s32 pz = v & 0xff;
            volatile u16 vv = e->dropUnits[i];
            if (vv != 0xffff) {
                Vec2 p2;
                s32 g7 = e->layer;
                p2.x = px;
                p2.y = pz;
                PendingUnit_Replace(e->aid, &p2, w, 0xfff1, zero, zero, zero, (u8)i, g7, -1);
            }
        }
    }
}
}

namespace nE {
extern "C" void FieldAction_CancelPendingForResult(FieldActionResultMsg *e) {
    s32 x = e->unit >> 8;
    s32 z = e->unit & 0xff;
    switch (e->kind) {
    case 12:
    case 13:
    case 15:
    case 23:
        return;
    }
    if (gSceneBlockMap != 0) {
        s32 xh = x >> 4;
        s32 zh = z >> 4;
        BlockMap_GetItemPtr(gSceneBlockMap, xh, zh, x - (xh << 4), z - (zh << 4), 0);
    }
    Vec2 p1;
    s32 f1 = e->layer;
    p1.x = x;
    p1.y = z;
    PendingUnit_CancelAt(&p1, f1);
    if (e->hasDrops) {
        s32 i;
        for (i = 0; i < 3; i++) {
            u16 v = e->dropUnits[i];
            s32 px = v >> 8;
            s32 pz = v & 0xff;
            volatile u16 vv = e->dropUnits[i];
            if (vv != 0xffff) {
                Vec2 p2;
                s32 f2 = e->layer;
                p2.x = px;
                p2.y = pz;
                PendingUnit_CancelAt(&p2, f2);
            }
        }
    }
}
}

namespace nE {
extern "C" void FieldAction_ApplyResultOffscreen(FieldActionResultMsg *e, s32 m) {
    switch (e->kind) {
    case 12:
    case 13:
    case 15:
    case 23:
        return;
    }
    Area_PlaceItem(m, e->unit >> 8, e->unit & 0xff, e->item, e->unitFlag);
    if (e->hasDrops) {
        u16 w = e->dropItem;
        volatile s32 z = 0;
        s32 i;
        for (i = 0; i < 3; i++) {
            u16 v = e->dropUnits[i];
            Area_PlaceItem(m, v >> 8, v & 0xff, w, z);
        }
    }
}
}

namespace nE {
extern "C" void FieldAction_OnNetResult(FieldActionResultMsg *e, s32 t) {
    if (gCommManager->myAid == e->aid) {
        FieldActionRequest *r = &sFieldActions.requests[e->requestIndex];
        if (e->accepted == 0) {
            r->state = 3;
        } else {
            r->state = 2;
            FieldAction_ApplyResult(e);
        }
    } else if (e->accepted != 0) {
        if (t == Scene_GetCurrent() && sPendingUnits.enabled != 0) {
            if (PlayerActor_TestSlotFlag(10, 4)) {
                FieldActionFx_Start(e);
                FieldAction_ApplyResult(e);
            } else {
                FieldAction_ApplyResult(e);
                FieldAction_CancelPendingForResult(e);
            }
        } else {
            FieldAction_ApplyResultOffscreen(e, t);
        }
    }
}
}

namespace nE {
extern "C" void FlowerFx_SetParams(FlowerFxParams *g, s32 a, s32 b, s32 c, u8 d, VecFx32 *v, s16 e) {
    g->mode = a;
    g->species = b;
    g->color = c;
    g->unk_06 = d;
    g->pos.x = v->x;
    g->pos.y = v->y;
    g->pos.z = v->z;
    g->angle = e;
}
}

namespace nE {
extern "C" void FlowerFx_InitByColor(EffectSplEmitter *p) {
    FlowerFxParams *const g = &nZ::gTownUpdater.flowerFx;
    Unk_020441f0_Color c0, c2, c4;
    volatile u16 c6;
    VecFx32 t;
    VecFx32 *gv = (VecFx32 *)((u8 *)g + 8);
    p->posX = g->pos.x + (*(s32 **)p->resource)[1];
    p->posY = gv->y + (*(s32 **)p->resource)[2];
    p->posZ = gv->z + (*(s32 **)p->resource)[3];
    *(u16 *)&c0 = Sky_GetLightColor(3);
    c6 = *(u16 *)&c0;
    *(u16 *)&c2 = c6;
    *(u16 *)&c4 = data_020da2c8[g->species][g->color];
    c2.r = (c4.r * c2.r) / 31;
    c2.g = (c4.g * c2.g) / 31;
    c2.b = (c4.b * c2.b) / 31;
    p->color = *(u16 *)&c2;
    if (g->mode == 1) {
        p->genInterval = 2;
    }
    if (g->mode == 2) {
        t.x = 0x400;
        t.y = 0x1000;
        t.z = 0;
        Vec_RotateY(&t, g->angle);
        if (Vec_SafeNormalize(&t)) {
            s32 x = t.x, y = t.y, z = t.z;
            p->axis.x = x;
            p->axis.y = y;
            p->axis.z = z;
        }
    }
}
}

namespace nE {
extern "C" void FlowerFx_InitBySpecies(EffectSplEmitter *p) {
    FlowerFxParams *const g = &nZ::gTownUpdater.flowerFx;
    Unk_020441f0_Color c0, c2, c4;
    volatile u16 c6;
    VecFx32 t;
    VecFx32 *gv = (VecFx32 *)((u8 *)g + 8);
    p->posX = g->pos.x + (*(s32 **)p->resource)[1];
    p->posY = gv->y + (*(s32 **)p->resource)[2];
    p->posZ = gv->z + (*(s32 **)p->resource)[3];
    *(u16 *)&c0 = Sky_GetLightColor(3);
    c6 = *(u16 *)&c0;
    *(u16 *)&c2 = c6;
    if (g->unk_06 != 0) {
        *(u16 *)&c4 = *data_020da2ac[g->species];
    } else {
        *(u16 *)&c4 = *data_020da2e4[g->species];
    }
    c2.r = (c4.r * c2.r) / 31;
    c2.g = (c4.g * c2.g) / 31;
    c2.b = (c4.b * c2.b) / 31;
    p->color = *(u16 *)&c2;
    if (g->mode == 2) {
        t.x = 0x400;
        t.y = 0x1000;
        t.z = 0;
        Vec_RotateY(&t, g->angle);
        if (Vec_SafeNormalize(&t)) {
            s32 x = t.x, y = t.y, z = t.z;
            p->axis.x = x;
            p->axis.y = y;
            p->axis.z = z;
        }
    }
}
}

namespace nE {
extern "C" void Flower_SpawnPetalFx(u16 *a, VecXZ *pos, s16 c, s32 d) {
    s32 k, id1, id2;
    u8 flag;
    VecFx32 v, w;
    k = (u8)Flower_GetSpecies(a);
    switch (k) {
    case 0:
        id1 = 0x44;
        id2 = 0x6a;
        break;
    case 1:
        id1 = 0x42;
        id2 = 0x68;
        break;
    case 2:
        id1 = 0x3e;
        id2 = 0x64;
        break;
    case 3:
        if (*a == 0x1c || *a == 0xa5) {
            id1 = 0x40;
            id2 = 0x66;
        } else {
            id1 = 0x43;
            id2 = 0x69;
        }
        break;
    case 4:
        id1 = 0x41;
        id2 = 0x67;
        break;
    case 5:
        id1 = 0x3f;
        id2 = 0x65;
        break;
    case 6:
        id1 = 0x63;
        id2 = 0x65;
        break;
    }
    FieldPos_FromUnitCenter(&v, pos->x, pos->z);
    flag = 0;
    s32 t;
    BOOL f4 = TRUE, f3 = TRUE, f2 = TRUE, f1 = FALSE;
    u32 x = *a;
    if (x >= 0x6e && x <= 0x73) {
        f1 = TRUE;
    }
    if (!f1 && !(x >= 0x74 && x <= 0x79)) {
        f2 = FALSE;
    }
    if (!f2 && !(x >= 0x7a && x <= 0x7f)) {
        f3 = FALSE;
    }
    if (!f3 && !(x >= 0x80 && x <= 0x87)) {
        f4 = FALSE;
    }
    if (f4 || x == 0x88 || x == 0x89 || (x >= 0x8a && x <= 0x8f) || (x >= 0x90 && x <= 0x95) ||
        (x >= 0x96 && x <= 0x9b) || (x >= 0x9c && x <= 0xa3) || x == 0xa5) {
        goto yes;
    }
    if (x == 0xa4) {
yes:
        flag = 1;
        t = 0;
    } else {
        t = (u8)Flower_GetColor(a);
    }
    w = v;
    FlowerFx_SetParams(&nZ::gTownUpdater.flowerFx, d, k, t, flag, &w, c);
    EffectSpl_CreateOneShot(id1, &v, 0, data_020da2a8);
    if (d != 1) {
        EffectSpl_CreateOneShot(id2, &v, 0, data_020da2a4);
    }
}
}

namespace nE {
extern "C" void Flower_SpawnPetalFxAt(u16 *a, VecXZ *p, s16 c, s32 d) {
    VecXZ t;
    t.x = p->x;
    t.z = p->z;
    Flower_SpawnPetalFx(a, &t, c, d);
}
}

namespace nE {
extern "C" void Weed_SpawnPullFx(u16 *a, VecXZ *p) {
    s32 id;
    switch (*a) {
    case 0x1f:
        id = 0x77;
        break;
    case 0x20:
        id = 0x78;
        break;
    default:
        id = 0x76;
        break;
    }
    VecFx32 v;
    FieldPos_FromUnitCenter(&v, p->x, p->z);
    EffectSpl_CreateOneShot(id, &v, 0, gEffectSplDefaultInitCbs);
}
}

namespace nE {
extern "C" void DeadTurnip_SpawnDigFx(VecXZ *p) {
    VecFx32 v;
    FieldPos_FromUnitCenter(&v, p->x, p->z);
    EffectSpl_CreateOneShot(0x94, &v, 0, gEffectSplDefaultInitCbs);
}
}

namespace nE {
extern "C" void Flower_TrampleTile(void *m, VecXZ *p, s32 f) {
    if (Item_IsFlower((u16 *)m)) {
        VecXZ t;
        t.x = p->x;
        t.z = p->z;
        Flower_SpawnPetalFx((u16 *)m, &t, 0, f);
        if (f == 0) {
            ItemSync_SetAtUnit((s8)p->x, (s8)p->z, Scene_GetCurrent(), 0xfff1, 0);
        }
    }
}
}

namespace nE {
extern "C" void Flower_TrampleAt(VecXZ *p, s32 f) {
    TownBlockMap *m = TownBlockMap_Get();
    if (m) {
        s32 x = p->x;
        s32 z = p->z;
        s32 xh = x >> 4;
        s32 zh = z >> 4;
        void *r = BlockMap_GetItemPtr(m, xh, zh, x - (xh << 4), z - (zh << 4), 0);
        if (r) {
            VecXZ t;
            t.x = p->x;
            t.z = p->z;
            Flower_TrampleTile(r, &t, f);
        }
    }
}
}

namespace nE {
extern "C" void Flower_Trample(VecXZ *p) {
    TownBlockMap *m = TownBlockMap_Get();
    if (m) {
        s32 x = p->x;
        s32 z = p->z;
        s32 xh = x >> 4;
        s32 zh = z >> 4;
        void *r = BlockMap_GetItemPtr(m, xh, zh, x - (xh << 4), z - (zh << 4), 0);
        if (r) {
            s32 f = Random_GlobalBelow(0x50) < 0x46 ? 1 : 0;
            VecXZ t;
            t.x = p->x;
            t.z = p->z;
            Flower_TrampleTile(r, &t, f);
        }
    }
}
}

namespace nE {
extern "C" void Flower_PlayTrampleFx(VecXZ *p) {
    VecXZ t;
    t.x = p->x;
    t.z = p->z;
    Flower_TrampleAt(&t, 1);
}
}

namespace nE {
extern "C" s32 Field_AidOrLocal(s32 a) {
    CommManager *g = gCommManager;
    if (!g->isOnline()) {
        a = g->localSlot;
    }
    return a;
}
}

namespace nE {
extern "C" s32 Field_AidOrZero(s32 a) {
    if (!gCommManager->isOnline()) {
        a = 0;
    }
    return a;
}
}

namespace nE {
extern "C" s32 Field_IsLocalAid(s32 a) {
    CommManager *g = gCommManager;
    if (g->isOnline() == 0) {
        return TRUE;
    }
    return g->isMyAid(a);
}
}

namespace nE {
extern "C" u32 FieldPlayer_GetHeldItem(u32 slot) {
    u32 r = 0xfff1;
    void *pd = PlayerData_GetBySessionSlot(slot);
    if (pd) {
        u16 *p = ((PlayerData *)(pd))->getHeldItem();
        if (p) {
            r = *p;
        }
    }
    return r;
}
}

namespace nD {
extern "C" void FieldAction_TryMoneyTree(u16 *out, u8 *flag, u16 c) {
    CommManager *g = gCommManager;
    if (FieldPlayer_GetHeldItem(g->localSlot) == 0x136a) {
        volatile u16 t = 0xfff1;
        t = c;
        u16 a;
        if (Unk_02043c28_InR(a, &t, 0x1492, 0x14fd)) {
            *out = 0x26;
            *flag = 0;
            if (TownState_IsPlayerDateNotToday((u8 *)&gSaveData.townState)) {
                s32 x = Item_GetPrice(&t);
                s32 y = FengShui_GetWestTotal();
                s32 sum = x / 1000 + y / 25;
                if (sum > 100) {
                    sum = 100;
                }
                if (Random_GlobalBelow(100) < sum) {
                    *out = 0x57;
                    TownState_SetPlayerDateToday((u8 *)&gSaveData.townState);
                }
            }
        }
    }
}
}

namespace nD {
extern "C" BOOL Item_ToPlantedFieldId(u16 *out, u16 *out2, u16 c) {
    u16 t = 0xfff1;
    BOOL r = TRUE;
    t = c;
    u16 a;
    if (Unk_02043c28_InR(a, &t, 0x1408, 0x1428)) {
        *out = Unk_02043c28_Idx(a, 0x1408, 0x1428);
    } else if (a >= 0x1471 && a <= 0x1491) {
        *out = Unk_02043c28_Idx(a, 0x1471, 0x1491);
    } else if (a >= 0x137c && a <= 0x137c) {
        *out = 0x1e;
    } else if (a >= 0x14fe && a <= 0x1517) {
        u32 i = Unk_02043c28_Idx(a, 0x14fe, 0x1517);
        u16 v;
        *out = i;
        if (i < 0x21) {
            v = i + 0x1408;
        } else {
            v = 0x1408;
        }
        *out2 = v;
    } else if (a >= 0x151d && a <= 0x151e) {
        if (a == 0x151d) {
            *out = 0x26;
            *out2 = 0x26;
        } else {
            *out = 0x5d;
            *out2 = 0x5d;
        }
    } else if (Item_IsTreeStage0(&t)) {
        u16 a2;
        if (Unk_02043c28_InR(a2, &t, 0xc8, 0xcf)) {
            *out = 0xc8;
        } else {
            *out = 0x26;
        }
    } else {
        u16 a3;
        if (Unk_02043c28_InR(a3, &t, 0x1518, 0x151c)) {
            *out = sFruitSaplings[Unk_02043c28_Idx(a3, 0x1518, 0x151c)];
        } else if (c == 0x1548) {
            *out = 0xc8;
        } else if (c == 0x1567) {
            *out = 0xd4;
            *out2 = 0x1408;
        } else {
            r = FALSE;
        }
    }
    return r;
}
}

namespace nD {
extern "C" void FieldAction_ResolveBuryItem(s32 a, u16 *p, u16 *q, u8 *r, u16 e) {
    *p = e;
    *r = 1;
    if (Item_ToPlantedFieldId(p, q, e)) {
        *r = 0;
    } else if (Field_IsLocalAid(a)) {
        FieldAction_TryMoneyTree(p, r, e);
    }
}
}

namespace nD {
extern "C" BOOL Town_CanReleaseBees(void) {
    BOOL r = FALSE;
    CommManager *g = gCommManager;
    if (g->isSlotActive(g->myAid) == 0) {
        if (PlayerData_GetCurrent()->testFlag(1) == 0) {
            if (gTownUpdater.beesReleased == 0) {
                r = TRUE;
            }
        }
    }
    return r;
}
}

namespace nD {
extern "C" void Town_SetBeesReleased(void) {
    gTownUpdater.beesReleased = 1;
}
}

namespace nD {
extern "C" void Town_ClearBeesReleased(void) {
    gTownUpdater.beesReleased = 0;
}
}

namespace nD {
extern "C" s32 FieldAction_RequestShake(s32 a, u16 *id, Vec2 *pos, s32 mode) {
    s32 result = -1;
    BOOL k9 = TRUE, k8 = TRUE, k7 = TRUE, k6 = TRUE, k5 = TRUE, k4 = TRUE, k3 = TRUE, k2 = TRUE, k1 = FALSE;
    u16 code;
    u32 cc = *id;
    if (cc >= 0x26 && cc <= 0x2a) k1 = TRUE;
    if (!k1) { if (!(cc >= 0x5d && cc <= 0x61)) k2 = FALSE; }
    if (!k2) { if (!(cc >= 0x2f && cc <= 0x56)) k3 = FALSE; }
    if (!k3) { if (!(cc >= 0x57 && cc <= 0x5b)) k4 = FALSE; }
    if (!k4) { if (!(cc >= 0x66 && cc <= 0x68)) k5 = FALSE; }
    if (!k5) { if (cc != 0x69) k6 = FALSE; }
    if (!k6) { if (!(cc >= 0x6a && cc <= 0x6c)) k7 = FALSE; }
    if (!k7) { if (cc != 0x6d) k8 = FALSE; }
    if (!k8) { if (!(cc >= 0xc8 && cc <= 0xcf)) k9 = FALSE; }
    if (k9) {
        if (!Item_IsTreeStage0(id)) {
            if (!Item_IsTreeStage1(id)) {
                s32 kind = 1;
                s32 off = Item_GetTreeStage(id) - 1;
                if (Unk_020437d0_R(id, 0x26, 0x2a) || (*id >= 0x57 && *id <= 0x5b) || (*id >= 0x66 && *id <= 0x68) || *id == 0x69) {
                    code = off + 0x27;
                } else {
                    code = off + 0x5e;
                }
                if (*id == 0x67 || *id == 0x6b) {
                    if (Town_CanReleaseBees()) {
                        kind = 2;
                    } else {
                        code = *id;
                    }
                } else if ((*id >= 0x2f && *id <= 0x56) || (*id >= 0xc8 && *id <= 0xcf)) {
                    if (Item_IsTreeGrown(id)) {
                        code = *id + 1;
                    } else {
                        code = *id;
                    }
                } else if (*id >= 0x57 && *id <= 0x5b) {
                    if (!Item_IsTreeGrown(id)) {
                        code = *id;
                    }
                } else if (*id == 0x6d) {
                    code = *id;
                }
                s32 idv = *id;
                result = FieldAction_Add(sFieldActions.requests, a, kind, pos, code, idv, 0, 0, mode, -1);
            }
        }
    }
    return result;
}
}

namespace nD {
extern "C" s32 FieldAction_RequestChop(s32 a, u16 *id, Vec2 *pos, s32 mode) {
    s32 kind = 0;
    s32 v24 = 0;
    s32 result = -1;
    s32 off;
    u16 code;
    code = *id;
    if (((code >= 0x26 && code <= 0x2a) || (code >= 0x5d && code <= 0x61) || (code >= 0x2f && code <= 0x56) ||
         (code >= 0x57 && code <= 0x5b) || (code >= 0x66 && code <= 0x68) || code == 0x69 ||
         (code >= 0x6a && code <= 0x6c) || code == 0x6d || (code >= 0xc8 && code <= 0xcf)) &&
        !Item_IsTreeStage0(id)) {
        off = Item_GetTreeStage(id) - 1;
        kind = 6;
        v24 = (u8)ChopCount_Get(&sFieldActions, pos);
        if (mode != 2) {
            if (v24 >= sTreeChopHits[off]) {
                if (Unk_020437d0_R(id, 0xc8, 0xcf)) {
                    code = off + 0xd0;
                } else if ((*id >= 0x5d && *id <= 0x61) || *id == 0x6d) {
                    code = off + 0x62;
                } else if (*id >= 0x2f && *id <= 0x56) {
                    code = off + 0xff;
                } else {
                    code = off + 0x2b;
                }
            } else {
                if (Unk_020437d0_R(id, 0x26, 0x2a) || (*id >= 0x57 && *id <= 0x5b) || (*id >= 0x66 && *id <= 0x68) || *id == 0x69) {
                    code = off + 0x27;
                } else {
                    code = off + 0x5e;
                }
                if (*id == 0x67 || *id == 0x6b) {
                    if (Town_CanReleaseBees()) {
                        kind = 7;
                    } else {
                        code = *id;
                    }
                } else if ((*id >= 0x2f && *id <= 0x56) || (*id >= 0xc8 && *id <= 0xcf)) {
                    if (Item_IsTreeGrown(id)) {
                        code = *id + 1;
                    } else {
                        code = *id;
                    }
                } else if (*id == 0x6d) {
                    code = *id;
                }
            }
        }
    } else {
        if (Unk_020437d0_R(id, 0xe3, 0xe7)) {
            kind = 0xd;
        } else if (*id >= 0xe8 && *id <= 0xfb) {
            kind = 0xc;
            if (!gCommManager->isOnline()) {
                s32 q = (*id - 0xe8) / 5;
                if (q == PlayerData_GetCurrentIndex()) {
                    if (mode != 2) {
                        kind = 0xe;
                    }
                }
            }
        } else {
            switch ((s32)(*id & 0xf000) >> 12) {
            case 0:
                if (*id == 0x1b || *id == 0x89) {
                    kind = 0x17;
                }
                break;
            case 1:
            case 3:
            case 4:
                kind = 0xc;
                break;
            case 2:
                break;
            }
        }
    }
    if (kind != 0) {
        s32 idv = *id;
        result = FieldAction_Add(sFieldActions.requests, a, kind, pos, code, idv, v24, 0, mode, -1);
    }
    return result;
}
}

namespace nD {
extern "C" s32 FieldAction_RequestDig(s32 a, BlockMap *b, u16 *id, Vec2 *pos, u16 e, s32 f) {
    s32 kind = 0x17;
    s32 result = -1;
    DigRequestArgs l;
    u32 c;
    l.item = ((BlockMap *)b)->getDigKind(pos->x, pos->y) == 0 ? 0xfc : 0xfd;
    l.b = 0;
    l.oldItem = *id;
    c = *id;
    if (c == 0xfff1) {
        switch (((BlockMap *)b)->getDigKind(pos->x, pos->y)) {
        case 0:
        case 1:
            kind = 8;
            break;
        case 2:
            kind = 0xc;
            l.item = c;
            break;
        case 3:
            kind = 0x17;
            l.item = c;
            break;
        }
    } else if (c >= 0xe3 && c <= 0xe7) {
        kind = 0xd;
        l.item = c;
    } else if (c >= 0xe8 && c <= 0xfb) {
        kind = 0xc;
        l.item = c;
        if (!gCommManager->isOnline()) {
            s32 q = (*id - 0xe8) / 5;
            if (q == PlayerData_GetCurrentIndex()) {
                kind = 0xe;
            }
        }
    } else if (c >= 0xfc && c <= 0xfd) {
        l.item = e;
        l.oldItem = e;
        kind = 0x13;
        if (e != 0xfff1) {
            FieldAction_ResolveBuryItem(a, &l.item, &l.oldItem, &l.b, e);
        }
    } else if ((c >= 0x2b && c <= 0x2e) || (c >= 0xff && c <= 0x102) || (c >= 0x62 && c <= 0x65) || (c >= 0xd0 && c <= 0xd3)) {
        kind = 0xb;
    } else if (Town_IsSaplingBlocker(id) || c == 0x1b || c == 0x89) {
        kind = 0xc;
        l.item = *id;
    } else {
        BOOL k2 = TRUE;
        BOOL k1 = TRUE;
        u32 d = *id;
        if (!(d == 0x25 || d == 0x5c)) k1 = FALSE;
        if (!k1 && d != 0xc7) k2 = FALSE;
        if (k2 || (((d >= 0x26 && d <= 0x2a) || (d >= 0x5d && d <= 0x61) || (d >= 0x2f && d <= 0x56) ||
                    (d >= 0x57 && d <= 0x5b) || (d >= 0x66 && d <= 0x68) || d == 0x69 || (d >= 0x6a && d <= 0x6c) ||
                    d == 0x6d || (d >= 0xc8 && d <= 0xcf)) &&
                   Item_IsTreeStage0(id))) {
            kind = 0xb;
        } else if (Item_IsFlower(id) || Unk_020437d0_R(id, 0x21, 0x24) || (*id >= 0x1f && *id <= 0x20) || *id == 0xe2) {
            kind = 8;
        } else {
            s32 r = BlockMap_IsBuriedAtUnit(b, pos->x, pos->y);
            if (r != 0 || Unk_02043540_Or(id)) {
                kind = 9;
                if (Unk_020437d0_R(id, 0xd4, 0xda)) {
                    l.oldItem = *id + 0x1467;
                } else {
                    l.oldItem = *id + 0x1460;
                }
            } else {
                switch ((s32)(*id & 0xf000) >> 12) {
                case 1:
                case 3:
                case 4:
                    kind = 0xc;
                    l.item = *id;
                    break;
                }
            }
        }
    }
    if (kind != 0) {
        result = FieldAction_Add(sFieldActions.requests, a, kind, pos, l.item, l.oldItem, l.b, 0, f, -1);
    }
    return result;
}
}

namespace nD {
extern "C" s32 FieldAction_RequestWater(s32 a, u16 *id, Vec2 *pos, s32 d) {
    s32 result = -1;
    if (Field_IsLocalAid(a)) {
        s32 c = *id;
        result = FieldAction_Add(sFieldActions.requests, a, 0xf, pos, c, c, 0, 0, d, -1);
    }
    return result;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestTool(s32 a, Vec2 *p, s32 sel, s32 d, u16 e) {
    TownBlockMap *o = TownBlockMap_Get();
    s32 r = -1;
    if (o) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = BlockMap_GetItemPtr(o, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (m) {
            switch (sel) {
            case 0:
                r = FieldAction_RequestShake(a, m, p, d);
                break;
            case 1:
                r = FieldAction_RequestChop(a, m, p, d);
                break;
            case 2:
                r = FieldAction_RequestDig(a, o, m, p, e, d);
                break;
            case 3:
                break;
            case 4:
                r = FieldAction_RequestWater(a, m, p, d);
                break;
            }
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestToolForAid(s32 a, Vec2 *p, s32 c, s32 d, u16 e) {
    s32 t = Field_AidOrZero(a);
    Vec2 q = *p;
    return FieldAction_RequestTool(t, &q, c, d, e);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestToolAtPending(s32 a, s32 b, s32 c, u16 d) {
    s32 r = -1;
    if (!Field_IsLocalAid(a)) {
        return -1;
    }
    u16 *p = PendingUnit_GetActivePosOfAid(a);
    if (p) {
        u16 v = *p;
        Vec2 q;
        q.x = v >> 8;
        q.y = v & 0xff;
        r = FieldAction_RequestTool(a, &q, b, c, d);
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestToolAtPendingForAid(s32 a, s32 b, s32 c, u16 d) {
    return FieldAction_RequestToolAtPending(Field_AidOrZero(a), b, c, d);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPickUp(s32 a, Vec2 *p, s32 c) {
    if (Scene_InTown()) {
        Vec2 q = *p;
        return FieldAction_RequestPickUpOutdoor(a, &q);
    }
    Vec2 q = *p;
    return FieldAction_RequestPickUpIndoor(a, &q);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPickUpOutdoor(s32 a, Vec2 *p) {
    BlockMap *map = gSceneBlockMap;
    s32 r = -1;
    if (map) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = (u16 *)BlockMap_GetItemPtr(map, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (m) {
            u16 c = *m;
            s32 v = 0;
            if (c == 0xfff1) {
                r = -1;
            } else {
                if ((c >= 0x21 && c <= 0x24) || c == 0x1f) {
                    v = 5;
                } else {
                    goto other;
                }
            }
            goto done;
        other:
            if (((c & 0xf000) >> 12) == 1) {
                if (!BlockMap_IsBuriedAtUnit(map, p->x, p->y)) {
                    goto v3;
                }
            }
            {
                BOOL t3;
                t3 = TRUE;
                BOOL t2;
                t2 = TRUE;
                BOOL t1;
                t1 = TRUE;
                BOOL t0;
                t0 = FALSE;
                u16 c2;
                c2 = *m;
                if (c2 <= 5) {
                    t0 = TRUE;
                }
                if (t0 == 0) {
                    if (c2 < 6 || c2 > 0xb) {
                        t1 = FALSE;
                    }
                }
                if (t1 == 0) {
                    if (c2 < 0xc || c2 > 0x11) {
                        t2 = FALSE;
                    }
                }
                if (t2 == 0) {
                    if ((c2 < 0x12 || c2 > 0x19) && c2 != 0x1c) {
                        t3 = FALSE;
                    }
                }
                if (t3 != 0 || c == 0x1a || c == 0x1d || c == 0x1e) {
                    goto v3;
                }
                switch ((c2 & 0xf000) >> 12) {
                case 3:
                case 4:
                    if (!BlockMap_IsBuriedAtUnit(map, p->x, p->y)) {
                        goto v3;
                    }
                }
                BOOL in;
                in = FALSE;
                c2 = *m;
                if (c2 >= 0xa7 && c2 <= 0xc6) {
                    in = TRUE;
                }
                if (in) {
                v3:
                    v = 3;
                } else if ((c2 >= 0x6e && c2 <= 0x73) || (c2 >= 0x74 && c2 <= 0x79) || (c2 >= 0x7a && c2 <= 0x7f) ||
                    (c2 >= 0x80 && c2 <= 0x87) || (c2 >= 0x8a && c2 <= 0x8f) || (c2 >= 0x90 && c2 <= 0x95) ||
                    (c2 >= 0x96 && c2 <= 0x9b) || (c2 >= 0x9c && c2 <= 0xa3) || c2 == 0xa5 || c == 0x88 || c == 0xa4) {
                    v = 5;
                } else if (c == 0x20) {
                    v = 0x15;
                }
                goto done;
            }
        done:
            if (v) {
                r = FieldAction_Add(sFieldActions.requests, a, v, p, 0xfff1, c, 0, 0, 0, -1);
            }
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPickUpIndoor(s32 a, Vec2 *p) {
    BlockMap *map = gSceneBlockMap;
    s32 z = 0;
    s32 r = -1;
    if (map) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = (u16 *)BlockMap_GetItemPtr(map, xh, yh, x - (xh << 4), y - (yh << 4), z);
        if (m) {
            s32 z2 = z;
            s32 w = z2;
            u16 c = *m;
            u16 s20;
            u8 s24;
            if (((c & 0xf000) >> 12) == 1) {
                w = 3;
                s20 = c;
                s24 = z2;
            } else {
                s32 x2 = p->x;
                s32 y2 = p->y;
                s32 xh2 = x2 >> 4;
                s32 yh2 = y2 >> 4;
                u16 *m2 = (u16 *)BlockMap_GetItemPtr(map, xh2, yh2, x2 - (xh2 << 4), y2 - (yh2 << 4), 1);
                if (m2) {
                    u16 c2 = *m2;
                    if (((c2 & 0xf000) >> 12) == 1) {
                        w = 3;
                        s20 = c2;
                        s24 = 1;
                    }
                }
            }
            if (w) {
                r = FieldAction_Add(sFieldActions.requests, a, w, p, 0xfff1, s20, 0, s24, 0, -1);
            }
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPickUpForAid(s32 a, Vec2 *p, s32 c) {
    s32 t = Field_AidOrZero(a);
    Vec2 q = *p;
    return FieldAction_RequestPickUp(t, &q, c);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestFillHole(s32 a, Vec2 *p, s32 c) {
    BlockMap *map = gSceneBlockMap;
    s32 r = -1;
    if (map) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = (u16 *)BlockMap_GetItemPtr(map, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (m) {
            s32 v = 0;
            u16 id = 0xfff1;
            BOOL in = Unk_0204301c_Rng(m, 0xfc, 0xfd);
            if (in && c != 2) {
                v = 0x14;
                id = 0xfff1;
            }
            if (v) {
                r = FieldAction_Add(sFieldActions.requests, a, v, p, id, id, 1, 0, 0, -1);
            }
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestFillHoleForAid(s32 a, Vec2 *p, s32 c) {
    s32 t = Field_AidOrZero(a);
    Vec2 q = *p;
    return FieldAction_RequestFillHole(t, &q, c);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPitfall(s32 a, Vec2 *p) {
    if (!Scene_InTown()) {
        return -1;
    }
    TownBlockMap *o = TownBlockMap_Get();
    s32 r = -1;
    if (o) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = (u16 *)BlockMap_GetItemPtr(o, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (m && *m == 0x1566 && BlockMap_IsBuriedAtUnit(o, p->x, p->y)) {
            u16 t = 0xfd;
            s32 q = Field_AidOrZero(a);
            if (!((BlockMap *)o)->getDigKind(p->x, p->y)) {
                t = 0xfc;
            }
            r = FieldAction_Add(sFieldActions.requests, q, 0x19, p, t, 0xfff1, 0, 0, 0, -1);
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPitfallAt(s8 a, Vec2 *p) {
    if (!Scene_InTown()) {
        return -1;
    }
    TownBlockMap *o = TownBlockMap_Get();
    s32 r = -1;
    if (o) {
        s32 x = p->x;
        s32 y = p->y;
        s32 xh = x >> 4;
        s32 yh = y >> 4;
        u16 *m = (u16 *)BlockMap_GetItemPtr(o, xh, yh, x - (xh << 4), y - (yh << 4), 0);
        if (m && *m == 0x1566 && BlockMap_IsBuriedAtUnit(o, p->x, p->y)) {
            u16 t = 0xfd;
            if (!((BlockMap *)o)->getDigKind(p->x, p->y)) {
                t = 0xfc;
            }
            r = FieldAction_Add(sFieldActions.requests, 0, 0x19, p, t, 0xfff1, 0, 0, 0, a);
        }
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_PollResult(s32 idx) {
    BlockMap *map = gSceneBlockMap;
    s32 r = 0;
    FieldActionRequest *e = &sFieldActions.requests[idx];
    CommManager *obj;
    if (e->state == 0 || e->kind == 0 ||
        (obj = gCommManager, ((CommManager *)(obj))->isOnline() != 0 && e->aid != obj->myAid)) {
        return 2;
    }
    if (map == NULL) {
        return 0;
    }
    s32 x = e->unit.x;
    s32 y = e->unit.y;
    s32 xh = x >> 4;
    s32 yh = y >> 4;
    u16 *m = (u16 *)BlockMap_GetItemPtr(map, xh, yh, x - (xh << 4), y - (yh << 4), 0);
    if (m == NULL) {
        return 0;
    }
    switch (e->state) {
    case 2:
        switch (e->kind) {
        case 0x13: {
            if (e->item != 0xfff1) {
                Actor *o = (Actor *)PlayerActor_GetActor(4);
                if (o) {
                    u16 v = e->oldItem;
                    VecFx32Copy *q = (VecFx32Copy *)&o->position;
                    volatile u16 id = 0xfff1;
                    id = e->item;
                    if (Item_IsTreeStage0((u16 *)&id)) {
                        BOOL in = FALSE;
                        u16 v1 = id;
                        u16 v2 = id;
                        if (v2 >= 0x2f && v1 <= 0x56) {
                            in = TRUE;
                        }
                        if (in) {
                            v = sSaplingFruitItems[Item_GetFruitTreeFruit((u16 *)&id)];
                        } else if (e->item == 0xc8) {
                            v = 0x1548;
                        }
                    }
                    FieldItemFx_StartPlant(e->aid, v, *(Pos *)&e->unit, *q);
                }
            }
            r = 1;
            break;
        }
        case 3:
        case 0x15:
            if (*m == 0xfff1) {
                u8 f = e->layer;
                PendingUnit_ApplyAt(&e->unit, f);
                r = 2;
            } else {
                r = 1;
            }
            break;
        case 0x18:
        case 0x1a:
            if (((CommManager *)(obj))->isOnline() == 0) {
                FieldItemFx_StartPop(e->aid, e->item, *(Pos *)&e->unit);
            }
            r = 1;
            break;
        default:
            r = 1;
            break;
        }
        break;
    case 3:
        r = 2;
        break;
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestAtFreeUnit(s32 a, s32 b, u16 c) {
    DropRequestArgs l;
    Vec2 pos;
    pos.x = 0;
    pos.y = 0;
    s32 r = -1;
    if (FieldAction_FindDropUnit(b, &pos, &l.layer)) {
        if (b == 0x18) {
            Item_ToPlantedFieldId(&l.item, &l.oldItem, c);
        } else {
            l.item = c;
            l.oldItem = c;
        }
        r = FieldAction_Add(sFieldActions.requests, a, b, &pos, l.item, l.oldItem, 0, l.layer, 0, -1);
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestDrop(s32 a, u16 b) {
    volatile u16 id = 0xfff1;
    id = b;
    BOOL in = FALSE;
    u16 v1 = id;
    u16 v2 = id;
    if (v2 >= 0xa7 && v1 <= 0xc6) {
        in = TRUE;
    }
    return FieldAction_RequestAtFreeUnit(a, in ? 0x1a : 0x10, b);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestPlaceAtPending(s32 a, u16 b) {
    s32 r = -1;
    if (!Field_IsLocalAid(a)) {
        return -1;
    }
    u16 *p = PendingUnit_GetActivePosOfAid(a);
    if (p) {
        u16 v = *p;
        Vec2 unit;
        unit.x = v >> 8;
        unit.y = v & 0xff;
        r = FieldAction_Add(sFieldActions.requests, a, 0x11, &unit, b, b, 0, 0, 0, -1);
    }
    return r;
}
}

namespace nC {
extern "C" s32 FieldAction_RequestDropOrPlace(s32 a, u16 b) {
    if (!Field_IsLocalAid(a)) {
        return -1;
    }
    return FieldAction_RequestAtFreeUnit(a, PendingUnit_GetActivePosOfAid(a) ? 0x12 : 0x10, b);
}
}

namespace nC {
extern "C" s32 FieldAction_RequestDropForAid(s32 a, u16 b) {
    return FieldAction_RequestDrop(Field_AidOrZero(a), b);
}
}

namespace nB {
extern "C" s32 FieldAction_RequestPlaceAtPendingForAid(s32 a, u16 b) {
    return FieldAction_RequestPlaceAtPending(Field_AidOrZero(a), b);
}
}

namespace nB {
extern "C" u16 *FieldAction_CheckFreeUnit(u32 a, BlockMap *m, s32 x, s32 y, Vec2 *p) {
    s32 hx = x >> 4;
    s32 hy = y >> 4;
    u16 *t = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
    if (t != NULL) {
        if (*t != 0xfff1) {
            t = NULL;
        } else {
            switch (a) {
            case 0x10:
            case 0x11:
            case 0x12:
                if (!Ground_IsNeighbourReachable(p->x, p->y, x, y, 0)) {
                    t = NULL;
                }
                break;
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                break;
            case 0x18: {
                s32 r = ((BlockMap *)m)->getPlantFlag(x, y);
                if (r != 0 && r != 1) {
                    t = NULL;
                }
                break;
            }
            case 0x19:
                break;
            case 0x1a:
                if (!Ground_IsNeighbourReachable(p->x, p->y, x, y, 1)) {
                    t = NULL;
                }
                break;
            }
        }
    }
    return t;
}
}

namespace nB {
extern "C" s32 FieldAction_FindDropUnit(u32 kind, Vec2 *p, u8 *out) {
    s32 res;
    const u32 *ent;
    s32 i;
    BlockMap *w;
    Actor *o;
    CommManager *g;
    s32 z;
    w = gSceneBlockMap;
    o = (Actor *)PlayerActor_GetActor(4);
    res = 0;
    if (w != NULL && o != NULL) {
        ent = sDropUnitOrder[Math_AngleToDir8(o->rotY)];
        FieldPos_ToUnit(&p->x, &p->y, &o->position);
        i = 0;
        g = gCommManager;
        z = i;
        for (; i < 9; ent++, i++) {
            u8 b = sDropUnitOffsets[*ent];
            s32 x = p->x + ((b >> 4) - 8);
            s32 y = p->y + ((b & 0xf) - 8);
            Vec2 q;
            u16 *t;
            q = *p;
            t = FieldAction_CheckFreeUnit(kind, w, x, y, &q);
            if (t != NULL && *t == 0xfff1) {
                Vec2 q2;
                q2.x = x;
                q2.y = y;
                if (PendingUnit_IndexAt(&q2, z) < 0) {
                    Vec2 q3;
                    s32 k = g->localSlot;
                    q3.x = x;
                    q3.y = y;
                    if (Field_IsUnitClearOfOthers(&q3, k)) {
                        p->x = x;
                        p->y = y;
                        if (out != NULL) {
                            *out = 0;
                        }
                        res = 1;
                        break;
                    }
                }
            }
            if (Scene_InTown() == 0) {
                if (Room_CanDropOnFurnitureAt(x, y)) {
                    p->x = x;
                    p->y = y;
                    if (out != NULL) {
                        *out = 1;
                    }
                    res = 1;
                    break;
                }
            }
        }
    }
    return res;
}
}

namespace nB {
extern "C" s32 FieldAction_PollDrop(s32 idx) {
    BlockMap *w = gSceneBlockMap;
    s32 res = 0;
    FieldActionRequest *e = &sFieldActions.requests[idx];
    if (w == NULL) {
        return 0;
    }
    switch (e->state) {
    case 2: {
        Actor *o = (Actor *)PlayerActor_GetActor(4);
        if (w != NULL && o != NULL) {
            VecFx32 *pv = &o->position;
            if (Unk_02042830_IsZero(gFieldSceneKind)) {
                volatile u16 tmp;
                tmp = 0xfff1;
                tmp = e->item;
                BOOL s8 = 1, s7 = 1, s6 = 1, s5 = 1, s4 = 1, s3 = 1, s2 = 1, s1 = 0;
                u32 v = tmp;
                if (tmp <= 5) {
                    s1 = 1;
                }
                if (!s1) {
                    if (!Unk_02042660_InRange(v, 6, 0xb)) {
                        s2 = 0;
                    }
                }
                if (!s2) {
                    if (!Unk_02042660_InRange(v, 0xc, 0x11)) {
                        s3 = 0;
                    }
                }
                if (!s3) {
                    if (!Unk_02042660_InRange(v, 0x12, 0x19) && v != 0x1c) {
                        s4 = 0;
                    }
                }
                if (!s4) {
                    if (!Unk_02042660_InRange(v, 0x8a, 0x8f) && !Unk_02042660_InRange(v, 0x90, 0x95) &&
                        !Unk_02042660_InRange(v, 0x96, 0x9b) && !Unk_02042660_InRange(v, 0x9c, 0xa3) && v != 0xa5) {
                        s5 = 0;
                    }
                }
                if (!s5) {
                    if (v != 0x1a) {
                        s6 = 0;
                    }
                }
                if (!s6) {
                    if (v != 0xa4) {
                        s7 = 0;
                    }
                }
                if (!s7 && v != 0x1d) {
                    s8 = 0;
                }
                if (s8 || Unk_02042660_InRange(v, 0xa7, 0xc6) || v == 0x1e) {
                    Vec2 q;
                    q.x = e->unit.x;
                    q.y = e->unit.y;
                    if (FieldItemFx_StartPop(e->aid, e->item, Unk_02042588_Pos(q.x, q.y))) {
                        res = 1;
                    }
                } else {
                    VecFx32 a;
                    a.x = pv->x;
                    a.y = pv->y;
                    a.z = pv->z;
                    Vec2 q;
                    q.x = e->unit.x;
                    q.y = e->unit.y;
                    if (FieldItemFx_StartDrop(e->aid, e->item, Unk_02042588_Pos(q.x, q.y), VecFx32Copy(a.x, a.y, a.z))) {
                        res = 1;
                    }
                }
            } else {
                VecFx32Copy a;
                a.x = pv->x;
                a.y = pv->y;
                a.z = pv->z;
                u32 b = e->layer;
                Unk_ov004_0222bf34_P2 q(e->unit.x, e->unit.y);
                if (ItemDrop_StartToUnit(e->aid, e->item, &q, &a, b)) {
                    res = 1;
                }
            }
        }
        break;
    }
    case 3:
        res = 2;
        break;
    }
    return res;
}
}

namespace nB {
extern "C" void FieldAction_Release(s32 i) {
    FieldAction_Clear(sFieldActions.requests, i);
}
}

namespace nB {
extern "C" void FieldAction_WaterFlowers(u32 self, Vec2 *p) {
    s32 flag, found, i;
    TownBlockMap *m;
    void *o;
    u16 *t;
    s32 y, x;
    s32 r;
    Vec2 q;
    const s8 *dx, *dy;
    m = TownBlockMap_Get();
    o = PlayerActor_GetActor(4);
    if (m == NULL || o == NULL) {
        return;
    }
    flag = 0;
    found = 0;
    if (FieldPlayer_GetHeldItem(self) == 0x1379) {
        flag = 1;
    }
    dx = sWaterOffsetsX;
    dy = sWaterOffsetsZ;
    for (i = 0; i < 5; dx++, dy++, i++) {
        x = p->x + *dx;
        y = p->y + *dy;
        q.x = x;
        q.y = y;
        if (PendingUnit_Find(&q, 0) < 0) {
            s32 hx = x >> 4, hy = y >> 4;
            t = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
            if (t != NULL) {
                r = Flower_GetWateredForm(&gTownUpdater, t, flag);
                if (r != 0xfff1) {
                    found = 1;
                    ItemSync_SetAtUnit((s8)x, (s8)y, Scene_GetCurrent(), r, 0);
                }
                {
                    u32 v = *t;
                    BOOL s4 = 1, s3 = 1, s2 = 1, s1 = 0;
                    if (v <= 5) {
                        s1 = 1;
                    }
                    if (!s1) {
                        if (!Unk_02042660_InRange(v, 6, 0xb)) {
                            s2 = 0;
                        }
                    }
                    if (!s2) {
                        if (!Unk_02042660_InRange(v, 0xc, 0x11)) {
                            s3 = 0;
                        }
                    }
                    if (!s3) {
                        if (!Unk_02042660_InRange(v, 0x12, 0x19) && v != 0x1c) {
                            s4 = 0;
                        }
                    }
                    if (s4 || Unk_02042660_InRange(v, 0x8a, 0x8f) || Unk_02042660_InRange(v, 0x90, 0x95) ||
                        Unk_02042660_InRange(v, 0x96, 0x9b) || Unk_02042660_InRange(v, 0x9c, 0xa3) || v == 0xa5 ||
                        Unk_02042660_InRange(v, 0x6e, 0x73) || Unk_02042660_InRange(v, 0x74, 0x79) ||
                        Unk_02042660_InRange(v, 0x7a, 0x7f) || Unk_02042660_InRange(v, 0x80, 0x87) || v == 0x1a ||
                        v == 0x88 || v == 0xa4 || (u16)(v + 0xffe3) <= 1) {
                        found = 1;
                    }
                }
            }
        }
    }
    if (found != 0) {
        if (gCommManager->isOnline() == 0) {
            VecFx32 buf;
            FieldPos_FromUnitCenter(&buf, p->x, p->y);
            VillagerTrend_NotifyUnk5(&buf);
        }
    }
}
}

namespace nB {
extern "C" void Field_ClearObjectFcFdAt(VecFx32 *p) {
    VecFx32 q = *p;
    Town_ClearObjectFcFdAtPos(&gTownUpdater, &q);
}
}

namespace nB {
extern "C" FieldActionRequest *FieldAction_Get(s32 i) {
    return &sFieldActions.requests[i];
}
}

namespace nB {
extern "C" s32 FieldAction_Add(FieldActionRequest *e, u32 a1, s32 type, Vec2 *pos, u16 a5, u16 a6, u8 a7, u8 a8, s32 a9, s8 a10) {
    s32 i, res = -1;
    for (i = 0; i < 3; e++, i++) {
        if (e->state == 0 && e->kind == 0) {
            if (type == 3 || type == 9 || type == 0x15) {
                if (a6 == 0x1520) {
                    if (Inventory_GetEmptyLetter() == 0) {
                        type++;
                    }
#ifdef N3DS_DIRECT
                } else if (Pocket_FindEmpty() < 0 && Pocket_FindFruitStack(&a6) == -1) {  // (or onto a fruit stack)
#else
                } else if (Pocket_FindEmpty() < 0) {
#endif
                    type++;
                }
            }
            e->aid = a1;
            e->area = NetArea_FindSlotInScene(Scene_GetCurrent());
            e->state = 1;
            e->kind = type;
            e->item = a5;
            e->oldItem = a6;
            s32 py = pos->y;
            e->unit.x = pos->x;
            e->unit.y = py;
            e->layer = a8;
            e->unk_1d = a7;
            e->mode = a9;
            e->unk_1e = a10;
            FieldAction_Submit(i, a9);
            res = i;
            break;
        }
    }
    return res;
}
}

namespace nB {
extern "C" void FieldAction_Clear(FieldActionRequest *e, s32 i) {
    FieldActionRequest *q = &e[i];
    q->state = 0;
    q->kind = 0;
}
}

namespace nB {
extern "C" void MoneyRock_Init(MoneyRockState *self) {
    MoneyRock_Reset(self);
}
}

namespace nB {
extern "C" void MoneyRock_Reset(MoneyRockState *self) {
    self->hits = 0;
    self->timer = -1;
}
}

namespace nB {
extern "C" void MoneyRock_Update(MoneyRockState *self) {
    if (gCommManager->isOnline() == 0) {
        if (self->timer > 0) {
            self->timer--;
        }
        if (self->timer == 0) {
            u16 v = *(u16 *)self;
            s32 x = v >> 8;
            s32 y = v & 0xff;
            Vec2 q;
            q.x = x;
            q.y = y;
            if (PendingUnit_Find(&q, 0) < 0) {
                u32 t = 0xe3;
                TownBlockMap *m = TownBlockMap_Get();
                self->timer = -1;
                if (m != NULL) {
                    s32 hx = x >> 4, hy = y >> 4;
                    u16 *tile = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
                    if (tile != NULL) {
                        t = (u16)((*tile - 0xe8) % 5 + 0xe3);
                    }
                }
                u16 w = *(u16 *)self;
                Scene_SetUnitItem(w >> 8, w & 0xff, t, 0);
            }
        }
    }
}
}

namespace nB {
extern "C" void MoneyRock_OnHit(MoneyRockState *self, Vec2 *p) {
    Vec2 q;
    if (self->timer < 0) {
        s32 py = p->y;
        self->x = p->x;
        self->y = py;
        self->timer = ((FengShui_GetWestTotal() * 0x3c0) >> 12) + 150;
    }
    self->hits = self->hits + 1;
    if (self->hits >= 8) {
        self->hits = 8;
    }
    q.x = p->x;
    q.y = p->y;
    MoneyRock_SpawnBag(self, &q);
}
}

namespace nB {
extern "C" void MoneyRock_SpawnBag(MoneyRockState *self, Vec2 *p) {
    TownBlockMap *m = (TownBlockMap *)TownBlockMap_Get();
    if (m != NULL) {
        Unk_0204da0c_Size *sz = (Unk_0204da0c_Size *)&m->width;
        s32 w = sz->w << 4;
        s32 h = sz->h << 4;
        const u8 *d = sNeighborOffsets8;
        s32 i;
        for (i = 0; i < 8; d++, i++) {
            u8 b = *d;
            s32 nx = p->x - ((b >> 4) - 8);
            s32 ny = p->y - ((b & 0xf) - 8);
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (nx == p->x && ny == p->y) {
                    continue;
                }
                Vec2 q;
                q.x = nx;
                q.y = ny;
                if (MoneyRock_TrySpawnBagAt(self, m, &q)) {
                    break;
                }
            }
        }
    }
}
}

namespace nB {
extern "C" BOOL MoneyRock_TrySpawnBagAt(MoneyRockState *self, BlockMap *m, Vec2 *p) {
    BOOL r = FALSE;
    s32 x = p->x, y = p->y;
    s32 hx = x >> 4, hy = y >> 4;
    u16 *t = BlockMap_GetItemPtr(m, hx, hy, x - (hx << 4), y - (hy << 4), 0);
    if (t != NULL && *t == 0xfff1) {
        if (((BlockMap *)m)->canPlaceItem(p->x, p->y) != 0) {
            Vec2 q;
            s32 qy = p->y;
            q.x = p->x;
            q.y = qy;
            PendingUnit_Reserve(0, &q, ((u16 *)sMoneyRockBags)[self->hits], 0xfff1, 0, 0, 0, 3, 0, -1);
            r = TRUE;
        }
    }
    return r;
}
}

namespace nB {
extern "C" void FieldActions_Init(FieldActionState *p) {
    s32 i;
    for (i = 0; i < 3; i++) {
        FieldAction_Clear(p->requests, i);
    }
    // the money rock state is the variable that follows sFieldActions in the original (offset 0x46c from p)
    MoneyRock_Init(&data_021c47bc);
}
}

namespace nB {
extern "C" void FieldActions_Update(FieldActionState *p) {
    // the money rock state is the variable that follows sFieldActions in the original (offset 0x46c from p)
    MoneyRock_Update(&data_021c47bc);
}
}

namespace nB {
extern "C" s32 ChopCount_Get(FieldActionState *m, Vec2 *p) {
    s32 idx = ((p->x - 0x10) >> 2) + ((p->y - 0x10) << 4);
    s32 sh = (p->x & 3) << 1;
    if (idx < 0 || idx >= 0x400) {
        idx = 0;
        sh = idx;
    }
    return (m->chopCounts[idx] >> sh) & 3;
}
}

namespace nB {
extern "C" void ChopCount_Set(FieldActionState *m, Vec2 *p, u32 v) {
    s32 idx = ((p->x - 0x10) >> 2) + ((p->y - 0x10) << 4);
    s32 sh = (p->x & 3) << 1;
    if (idx < 0 || idx >= 0x400) {
        idx = 0;
        sh = idx;
    }
    m->chopCounts[idx] = (v << sh) | (m->chopCounts[idx] & ~(3 << sh));
}
}

namespace nB {
extern "C" void ChopCount_ClearAll(void) {
    MI_CpuFill8(nZ::sFieldActions.chopCounts, 0, 0x400);
}
}

namespace nA {
extern "C" void FieldActionFx_ClearAll(FieldActionFxEvent *p) {
    s32 i;
    for (i = 0; i < 5; i++) {
        p->type = 0;
        p->pos = 0xffff;
        p++;
    }
}
}

namespace nA {
extern "C" void FieldActionFx_Start(FieldActionResultMsg *p) {
    struct {
        volatile u16 v[7];
    } W;
    s32 k;
    struct {
        Vec2 p[6];
        VecFx32 v[3];
    } m;
    FieldActionFxEvent *e = &sFieldActionFxSlots[p->aid];
    if (e->type != 0) {
        W.v[5] = e->pos;
        u16 t = W.v[5];
        W.v[1] = t;
        W.v[0] = t;
        m.p[0].x = W.v[0] >> 8;
        m.p[0].y = W.v[1] & 0xff;
        PendingUnit_CancelAt(&m.p[0], 0);
        if (p->hasDrops == 1) {
            volatile u16 *hp = p->dropUnits;
            s32 i;
            k = 0;
            for (i = 0; i < 3; hp++, i++) {
                if (*hp != 0xffff) {
                    u16 v = *hp;
                    m.p[1].x = v >> 8;
                    m.p[1].y = v & 0xff;
                    PendingUnit_CancelAt(&m.p[1], k);
                }
            }
        }
    }
    e->type = p->kind;
    W.v[4] = p->unit;
    u16 t2 = W.v[4];
    e->pos = t2;
    e->sub = p->mode;
    W.v[6] = p->unit;
    u16 t3 = W.v[6];
    W.v[3] = t3;
    W.v[2] = t3;
    s32 hi = W.v[2] >> 8;
    s32 lo = W.v[3] & 0xff;
    switch (p->kind) {
    case 16:
    case 17:
    case 18: {
        Actor *o = (Actor *)PlayerActor_GetActor(p->aid);
        if (o != 0) {
            VecFx32 *q = &o->position;
            if (Unk_02042104_IsZero(gFieldSceneKind)) {
                m.v[0] = *q;
                m.p[2].x = hi;
                m.p[2].y = lo;
                FieldItemFx_StartDrop(p->aid, p->item, Unk_02042588_Pos(m.p[2].x, m.p[2].y), VecFx32Copy(m.v[0].x, m.v[0].y, m.v[0].z));
            } else {
                m.v[1] = *q;
                m.p[3].x = hi;
                m.p[3].y = lo;
                Unk_ov004_0222bf34_P2 unit(m.p[3].x, m.p[3].y);
                VecFx32Copy from(m.v[1].x, m.v[1].y, m.v[1].z);
                ItemDrop_StartToUnit(p->aid, p->item, &unit, &from, 0);
            }
        }
        break;
    }
    case 15:
        m.p[4].x = hi;
        m.p[4].y = lo;
        FieldAction_WaterFlowers(p->aid, &m.p[4]);
        break;
    case 19:
        if (p->item != 0xfff1) {
            Actor *o = (Actor *)PlayerActor_GetActor(p->aid);
            if (o != 0) {
                VecFx32 *q = &o->position;
                m.v[2] = *q;
                m.p[5].x = hi;
                m.p[5].y = lo;
                FieldItemFx_StartPlant(p->aid, p->item, Unk_02042588_Pos(m.p[5].x, m.p[5].y), VecFx32Copy(m.v[2].x, m.v[2].y, m.v[2].z));
            }
        }
        break;
    }
}
}

namespace nA {
extern "C" s32 FieldActionFx_Take(FieldActionFxEvent *out, u32 v) {
    s32 r = 0;
    s32 idx = Field_AidOrZero(v);
    FieldActionFxEvent *e = &sFieldActionFxSlots[idx];
    if (e->type != 0) {
        out->type = e->type;
        volatile u16 t = e->pos;
        out->pos = t;
        out->sub = e->sub;
        e->type = r;
        r = 1;
    }
    return r;
}
}

namespace nA {
extern "C" s32 TownBbs_UnpackUsedMask(void *o, u8 *dst, u8 *src, s32 n) {
    s32 c = 0;
    s32 cur = *src;
    s32 i;
    for (i = 0; i < n; i++) {
        *dst = (cur >> (i & 7)) & 1;
        if (*dst == 0) {
            c++;
        }
        if ((i + 1) % 8 == 0) {
            src++;
            cur = *src;
        }
        dst++;
    }
    return c;
}
}

namespace nA {
extern "C" void TownBbs_PackUsedMask(void *o, u8 *dst, u8 *src, s32 n) {
    s32 i;
    for (i = 0; i < n; i++) {
        *dst = (src[7] << 7) | ((src[6] << 6) | ((src[5] << 5) | ((src[4] << 4) | ((src[3] << 3) | ((src[2] << 2) | (src[0] | (src[1] << 1)))))));
        src += 8;
        dst++;
    }
}
}

namespace nA {
extern "C" void TownBbs_PostRandomUnused(void *o, s32 n, const char *fmt, u8 *p, s32 len, Unk_02042104_Date *d) {
    s32 idx = 0;
    if (p != 0) {
        s32 i;
        for (i = 0; i < len; p++, idx++, i++) {
            if (*p == 0) {
                n--;
                if (n < 0) {
                    break;
                }
            }
        }
        *p = 1;
    }
    Bbs_PostMsgDated(idx, fmt, d->c5, d->c4, d->c3);
}
}

namespace nA {
extern "C" void TownBbs_PostSlogan(void *o, SaveData *save, Unk_02042104_Date *d) {
    u8 buf[0x54];
    Unk_02042104_Date tmp;
    s32 n = TownBbs_UnpackUsedMask(o, buf, save->townState.unk_16, 0x54);
    if (n == 0) {
        n = 0x54;
        MI_CpuFill8(buf, 0, n);
    }
    n = Random_GlobalBelow(n);
    MI_CpuCopy8(d, &tmp, 8);
    TownBbs_PostRandomUnused(o, n, "bbs_slogan", buf, 0x54, &tmp);
    TownBbs_PackUsedMask(o, save->townState.unk_16, buf, 11);
}
}

namespace nA {
extern "C" void TownBbs_PostPelicanNotice(void *o, SaveData *save, Unk_02042104_Date *d) {
    u8 buf[0x4c];
    Unk_02042104_Date tmp;
    s32 n = TownBbs_UnpackUsedMask(o, buf, save->townState.unk_0c, 0x4c);
    if (n == 0) {
        n = 0x4c;
        MI_CpuFill8(buf, 0, n);
    }
    n = Random_GlobalBelow(n);
    MI_CpuCopy8(d, &tmp, 8);
    TownBbs_PostRandomUnused(o, n, "bbs_pelican", buf, 0x4c, &tmp);
    TownBbs_PackUsedMask(o, save->townState.unk_0c, buf, 10);
}
}

namespace nA {
extern "C" void TownBbs_PostDayEvents(void *o, EventDayEntry *z, Unk_02042104_Date *d) {
    volatile s32 i, v8, v0c, v10, v14;
    Unk_02042104_Date tmp;
    MsgString9BStorage obj;
    s32 t, n;
    new (&obj) MsgString9B();
    i = 0;
    v10 = 0;
    v0c = 0;
    v8 = -1;
    v14 = i;
    do {
        t = v8;
        u32 ty = z->eventId;
        switch (ty) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7: {
            VillagerData *vd;
            VillagerId *id;
            if ((vd = (VillagerData *)SaveVillagers_Get(gSaveVillagers, ty)) != 0 && (id = (VillagerId *)vd->getVillagerId()) != 0) {
                Villager_GetSpeciesName(&obj, VillagerId_GetSpecies(id));
                MailText_SetSlot(v0c, &obj);
                t = Random_GlobalBelow(3) + 0x1e;
            }
            break;
        }
        default:
            if ((s32)ty >= 0x1c && (s32)ty < 0x3a) {
                MI_CpuCopy8(d, &tmp, 8);
                s32 r = Event_GetStateAt(ty, &tmp, v10);
                switch (r) {
                case 2:
                case 3:
                    t = sBbsEventMsgs[ty - 0x1c];
                    break;
                }
            }
            break;
        }
        if (t != ~v14) {
            Bbs_PostMsgDated(t, "bbs_event", d->c5, d->c4, d->c3);
        }
        z++;
        n = i + 1;
        i = n;
    } while (n < 7);
    ((MsgString9B *)((void *)(&obj)))->MsgString9B::~MsgString9B();
}
}

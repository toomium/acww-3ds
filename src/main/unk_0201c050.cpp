#include "uptr.h"
#include "host_labels.h"
#include "host_shared_calls.h"
#include "town/TownBlockCell.h"
// mwcc-flags: -str reuse
#include "types.h"
#include "n3ds_options.h"
#include "sys/ClockDateTime.h"
#include "gfx/VecFx32.h"
#include "item/PocketMatches.h"
#include "net/CommManager.h"
#include "npc/VillagerClothModel.h"
#include "talk/MsgStringBase.h"
#include "npc/VillagerId.h"
#include "npc/VillagerMood.h"
#include "talk/VillagerTalkRequestItemTopics.h"
#include "game/ReddPassword.h"
#include "talk/ReddPasswordString.h"
#include "item/Letter.h"
#include "npc/NpcAnimCtrl.h"
#include "npc/NpcSpeechState.h"
#include "talk/EncodedStringBase.h"
#include "npc/NpcObstacleProbe.h"
#include "npc/Unk_0201ad18.h"
#include "npc/NpcFootstepFx.h"
#include "talk/VillagerTalkRequestStartTopics.h"
#include "sys/ProcBase.h"
#include "npc/NpcResHandle.h"
#include "talk/EncodedString.h"
#include "npc/NpcTalkCtrl.h"
#include "talk/VillagerTalkKaraokeTopics.h"
#include "actor/Actor.h"
#include "actor/Character.h"
#include "talk/ConstellationEncodedString16.h"
#include "talk/VillagerTalkAcornTopics.h"
#include "gfx/ThreeLayerAnimModel.h"
#include "talk/MsgString.h"
#include "talk/TalkMsgRequest.h"
#include "talk/TalkWindowState.h"
#include "save/ContestRecord.h"
#include "save/SaveData.h"
#include "gfx/NNSG3dRS.h"
#include "town/TownBlockMap.h"
#include "town/BlockMap.h"
#include "town/MapBlock.h"
#include "talk/ActorTalkRequest.h"
#include "talk/VillagerTalk.h"
#include "talk/ConstellationMsgString17.h"
#include "talk/MsgString25B.h"
#include "talk/MsgString129.h"
#include "talk/MsgString9BStorage.h"
#include "talk/MsgString33B.h"
#include "talk/MsgString17.h"
#include "talk/MsgString9B.h"
#include "snd/SndSeEmitterKind1.h"
#include "actor/NpcActor.h"
#include "actor/VillagerActor.h"
#include "talk/VillagerTalkHobbyTopics.h"
#include "talk/VillagerTalkHolidayTopics.h"
#include "talk/VillagerTalkRequestReplyTopics.h"
#include "talk/VillagerTalkTopics.h"
#include "talk/TalkTopicMsg.h"
#include "save/TownId.h"
#include "npc/VillagerState.h"
#include "actor/ActorFollowCollider.h"
#include "actor/CharaClothTexRef.h"
#include "game/ReddShop.h"
#include "gfx/CachedModel.h"
#include "gfx/MatTexVramTask.h"
#include "item/ItemPickSpec.h"
#include "item/LetterView.h"
#include "npc/NpcActionCtrl.h"
#include "npc/NpcFaceAnim.h"
#include "npc/NpcMoveAnimSet.h"
#include "npc/VillagerDataItemView.h"
#include "save/VillagerData.h"
#include "player/PlayerData.h"
#include "player/PlayerId.h"
#include "player/PlayerInventory.h"
#include "save/Pattern.h"
#include "save/DramaPart.h"
#include "snd/SndSeEmitter.h"
#include "talk/ChoiceList.h"
#include "proto/autoload_2/unk_02128030.h"
#include "proto/autoload_2/unk_02133d04.h"
#include "proto/autoload_2/unk_020e92f4.h"
#include "proto/main/unk_02002b1c.h"
#include "proto/main/unk_02003008.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0201c050.h"
#include "proto/main/unk_0202e2d4.h"
#include "proto/main/unk_02034438.h"
#include "proto/main/unk_0203c23c.h"
#include "proto/main/unk_0203c638.h"
#include "proto/main/unk_0203c92c.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_0204cc1c.h"
#include "proto/main/unk_0204eeb4.h"
#include "proto/main/unk_0204f134.h"
#include "proto/main/unk_02052b50.h"
#include "proto/main/unk_0205c91c.h"
#include "proto/main/unk_02060b9c.h"
#include "proto/main/unk_0206269c.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02065330.h"
#include "proto/main/unk_0206d988.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_020815f8.h"
#include "proto/main/unk_020850e0.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_02090268.h"
#include "proto/main/unk_02093ff0.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02097d1c.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_0209cb74.h"
#include "proto/main/unk_020afeb4.h"
#include "proto/main/unk_020b0774.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/ov003/unk_ov003_02225800.h"
#include "npc/ErrandRecord.h"
#include "npc/SickVillagerRecord.h"
#include "npc/VillagerPlan.h"
#include "player/PlayerDailyTalkFlags.h"
#include "save/VillagerData.h"


class VillagerTalk;
class VillagerActor;


class VillagerTalk;
struct TalkChoiceTable;
struct CommManager;
class VillagerMood;
struct VillagerActor;
struct MsgString25B;
struct MsgString129;
struct MsgString33B;
class VillagerTalkTopics;
struct TalkTopicMsg;
struct TalkTopic;
class VillagerTalkHolidayTopics;
struct VillagerId;
struct PersonalitySleepHours;
struct TalkWindowState;
struct TalkMsgIndexItem;
class VillagerTalkAcornTopics;
class ActorTalkRequest;
class VillagerTalkHobbyTopics;
class VillagerTalkKaraokeTopics;
struct VillagerActor;
struct Unk_0201fb54_Date;
class EncodedStringBase;
class MsgStringBase;
class EncodedString;
class MsgString;
class ConstellationEncodedString16;
class ConstellationMsgString17;
struct TalkTopicPick;
struct TownIdView;
struct MsgString17;
struct MsgString9B;
struct RumorVillagerIndex;
struct RumorVillagerPair;
struct VillagerState;
class VillagerTalkRumorTopics;
struct TownId;
class VillagerTalkRequestItemTopics;
class VillagerTalkRequestStartTopics;
struct VillagerActor;
struct PresentOpinionTopicList;
class VillagerTalkRequestReplyTopics;
struct MsgString9BStorage;
struct PocketMatches;
struct VillagerStateView;
struct SingleByteString;
class ReddPassword;
struct Unk_0202b520_Pair;
struct FishSpawnEntry;
struct FishSpawnRow;
struct FishSpawnTable;
struct Unk_0202c654_Row;
struct Unk_0202c92c_Ent;
struct Vec2;
class VillagerActor;
class VillagerClothModel;
struct VillagerTalkTopicFns;
class Unk_02015b54;
struct NpcMoveAnimSet;
struct NpcFaceAnim;
struct NpcAnimCtrl;
struct Unk_0201ad18;
struct NpcSpeechState;
struct NpcLookAt;
struct NpcFootstepFx;
struct NpcActionCtrl;
class SndSeEmitter;
struct VillagerAnimHeapHandle;
struct VillagerMood;
struct Unk_02082014;
class Character;
class NpcActor;
class ActorTalkRequest;
class SpNpcTalkRequest;
class SpNpcActor;




extern "C" void TownId_Construct();
extern "C" void TownId_Destruct();
struct TownIdView {
    TownIdView() { TownId_Construct(); }
    ~TownIdView() { TownId_Destruct(); }
    /* 0x00 */ u16 townId;
    /* 0x02 */ u8 townName[8];
    /* 0x0a */ u8 pad_0a[6];
};



typedef void (VillagerTalk::*Unk_020d8938_Fn)();

typedef void (VillagerTalk::*Unk_020d8938_ArgFn)(void *arg);






// the owner of the VillagerTalkTopics object as its functions see it


struct TalkChoiceTable {
    u8 range[5][2];
    u8 pad_0a[2];
    VillagerTalkTopicFns *val[5];
    u8 count;
    s8 cancelIndex;
};




typedef BOOL (VillagerMood::*Unk_0201c078_Fn)(VillagerActor *s);


struct TalkTopic {
    const char *key;
    u8 variantCount;
};


typedef void (VillagerTalkTopics::*Unk_0201d2d0_Fn)();

typedef void (VillagerTalkTopics::*Unk_0201d2d0_OutFn)(TalkTopicMsg *);

typedef void (VillagerTalkHolidayTopics::*Unk_0201dc44_State)(TalkTopicMsg *out);




struct PersonalitySleepHours {
    u8 sleepHour;
    u8 sleepMinute;
    u8 wakeHour;
    u8 wakeMinute;
};




struct TalkMsgIndexItem {
    u8 msgIndex;
    u16 item;
};




typedef void (VillagerTalkAcornTopics::*Unk_0201e5a4_RetFn)(TalkTopicMsg *);

typedef void (VillagerTalkAcornTopics::*Unk_0201e5a4_Fn)(void *);

typedef void (VillagerTalkAcornTopics::*Unk_0201e5a4_VoidFn)();







typedef void (VillagerTalkHobbyTopics::*Unk_0201eea4_Fn)(void *);



struct Unk_0201fb54_Date {
    u32 a;
    u32 b;
};

typedef void (VillagerTalkKaraokeTopics::*Unk_0201f7d0_Fn)();

typedef void (VillagerTalkKaraokeTopics::*Unk_0201f7d0_OutFn)(TalkTopicMsg *);

typedef s32 (VillagerTalkTopics::*Unk_02020850_Fn)();



typedef void *(VillagerTalkTopics::*Unk_02021048_Fn)(u32 *, s32);

typedef const char *(VillagerTalkTopics::*Unk_02020d90_Fn)(u8 *, s32 *, u8 *, ClockDateTime *);





struct TalkTopicPick {
    const char *key;
    s32 part;
    u8 partSize;
    u8 variantCount;
};


struct RumorVillagerIndex { s32 v; };

struct RumorVillagerPair { RumorVillagerIndex a, b; };




typedef BOOL (VillagerTalkTopics::*Unk_0201d2d0_BFn)();














typedef void (VillagerTalkRequestStartTopics::*Unk_020254ec_Fn)();


// request item pickers (VillagerRequest_Pick*ForStep: one per errand step)
typedef void (*RequestDatePickerFn)(u16 *, ClockDateTime *);
typedef void (*RequestShirtPickerFn)(u16 *, u8 *, u16 *);
typedef void (*RequestFossilPickerFn)(u16 *, PlanErrand *, void *);

struct PresentOpinionTopicList {
    u32 v[3];
};





typedef void (VillagerTalkRequestReplyTopics::*Unk_02027a34_Fn)();

typedef void (VillagerTalkRequestReplyTopics::*Unk_02027a34_OutFn)(TalkTopicMsg *);

typedef BOOL (VillagerTalkRequestReplyTopics::*Unk_02027a34_TestFn)(void *, void *);






typedef s32 (VillagerTalkTopics::*Unk_0202a750_Fn)();



struct VillagerStateView {
    u8 pad_00[0x1d];
    union {
        u8 flags;
        struct {
            u8 f0 : 1;
            u8 f1 : 1;
            u8 f2 : 1;
        } bits;
    };
};

struct SingleByteString {
    u8 c[2];
};




struct Unk_0202b520_Pair {
    u16 unk_00;
    u16 fortuneItem;
};



struct FishSpawnEntry {
    u8 fish;
    u8 unk_01;
    u8 weight;
};

struct FishSpawnRow {
    FishSpawnEntry *entries;
    u8 count;
};

struct FishSpawnTable {
    FishSpawnRow **slots;
};

struct Unk_0202c654_Row {
    u8 *entries;
    u8 count;
};

struct Unk_0202c92c_Ent {
    u8 *entries;
    u8 count;
};









// ---- class chain of VillagerTalk (vtable 0x020d8930): TalkMsgRequest <- ActorTalkRequest <- VillagerTalk.


typedef void (VillagerTalk::*Unk_020d8938_Fn2)(u32 a, s32 b);
typedef s32 (VillagerTalk::*Unk_020d8938_FnS)();
typedef void (VillagerTalk::*Unk_020d8938_FnArg)(u32);


// ---- class chain of VillagerActor (vtable 0x020d89c0):



inline SndSeEmitterKind1::~SndSeEmitterKind1() {}
inline NpcActor::~NpcActor() {}


















class VillagerTalkRumorTopics : public VillagerTalkTopics {
public:
    void *pickMemoryWithReceivedItem(void **arr, s32 n);
    void *pickMemoryAny(void **arr, s32 n);
    void *pickMemoryWithTime(void **arr, s32 n);
    void *pickMemoryLetterFromResident(void **arr, s32 n);
    void *pickMemoryWithCompliment(void **arr, s32 n);
    void *pickMemoryLetterFromVillagerTown(void **arr, s32 n);
    void *pickMemoryDislikedOld(void **arr, s32 n);
    void *pickMemoryLikedOld(void **arr, s32 n);
    BOOL selectTsuItem();
    BOOL selectTsuHome();
    BOOL selectTsuSpot();
    BOOL selectTsuFriend();
    BOOL selectTsuHappyroom();
};







extern "C" {
extern "C++" {}

void NpcActor_OnJointCalc(NNSG3dRS *self);
s32 FishPick_PickInSlot(u16 *, s32 *, s32, FishSpawnRow **);
s32 FishPick_CountInSlot(s32, FishSpawnRow **);
s32 FishPick_PickInRow(u16 *, s32 *, s32, FishSpawnRow *);
s32 FishPick_CountInRow(s32, FishSpawnRow *);
BOOL MapBlock_HasAnyAttr(MapBlock *o, u32 mask);
u16 *MapBlock_GetItemPtr(void *cell, u32 x, u32 y, u32 z);
BOOL Camera_FocusOnPoint(VecFx32 *a);
s32 Event_GetState(s32 a, void *b, s32 c);
s32 Event_GetDaysSinceStart(u32);
s32 Item_GetFurnitureIndex(u16 *p);
BOOL Item_IsFurniture(u16 *p);
s32 Item_FindMoneyBagForAmount(s32 a, BOOL up, s32 *out);
BOOL Item_GetShirtUnkGroup(u16 *p);
s32 Item_GetPrice(u16 *p);
TownBlockMap * TownBlockMap_Get();
u8 FengShui_GetWestTotal();
void * VillagerAnimHeapRef_GetHeap(s32 *);
s32 TalkRepeat_GetLevel(void *);
void * VillagerState_GetTalkRepeat(void *);
void VillagerState_SetHeldItem(void *, void *);
u16 * VillagerState_GetHeldItem(void *);
s32 VillagerState_SetUnk1dBit1(void *);
s32 VillagerState_TickMoodTimer(void *);
s32 VillagerState_GetMoodTimer(void *);
s32 VillagerState_SetMoodTimer(void *, u32);
s32 VillagerState_AddMoodTimer(void *, u32);
s32 VillagerState_SetMood(void *, u32);
s32 VillagerState_GetMood(void *);
s32 VillagerState_GetActivity(void *);
void * VillagerState_GetErrand(void *);
s32 SaveVillagers_IsTuneRequester(void *, void *);
void SaveVillagers_SetTuneRequester(void *, void *);
void SaveVillagers_SetNicknameDate(void *, void *);
s32 SaveVillagers_IsNicknameCooldownOver(void *, void *);
s32 SaveVillagers_FindBirthdayVillager(void *, void *);
s32 VillagerEvent_FindUpcomingIndex(s32, void *);
s32 VillagerEvent_GetTodayIndex();
s32 SaveVillagers_GetUnk3830Index(void *);
void * SaveVillagers_GetUnk3830(void *);
void SaveVillagers_UpdateErrands(void *);
u32 Villager_GetPlayerErrandKind(void *, void *, void *);
void SaveVillagers_AddRelation(void *, void *, void *, s32);
s32 SaveVillagers_GetRelationLevel(void *, s32, s32);
void SaveVillagers_SetLastMovedInById(void *, void *);
void * SaveVillagers_PickRandomTalkPartner(void *, void *, s32);
s32 Random_PickSetBit(u32, u32, u32);
void * SaveVillagers_PickRandomExcept(void *, void *, s32);
void * SaveVillagers_Get(void *, s32);
s32 SaveVillagers_FindIndex(void *, void *);
void Villager_UpdateVisitorRecord(void *, u32);
s32 Villager_IsAsleep(void *, void *);
void Villager_UpdatePlanErrand(void *);
u32 Villager_GetFurnitureTasteIndex(void *);
s32 Villager_IsGreeter(void *);
s32 Villager_GetMostValuableReceivedItem(u16 *, void *);
s32 Villager_PickRandomReceivedItem(void *, void *);
s32 Villager_RemoveReceivedItem(void *, void *);
void Villager_AddReceivedItem(void *, void *);
void Villager_UpdateImpression(void *, void *, u32);
s32 Villager_GetMoveInKind(void *);
s32 Villager_IsInPlayerErrand(void *, void *);
s32 Villager_IsJustMovedIn(void *);
s32 Villager_IsMovingIn(void *);
s32 Villager_GetResidentStatus(void *);
u8 * Villager_GetPlan(void *);
VillagerState *Villager_GetState(void *);
s32 Villager_GetIndex(void *);
s32 Villager_CountFurnitureLike(void *, void *);
void * Villager_SetNicknameFor(void *, void *, s32, void *);
void Villager_SetNicknameFromMsgFor(void *, void *, void *);
s32 Villager_CollectOtherMemories(void *, void *, void *, u32);
s32 Villager_PickMemorySlotForNew(void *);
s32 Villager_FindMemoryIndexById(void *, void *);
void * Villager_FindMemory(void *, void *);
void * Villager_GetMemory(void *, u32);
u8 Villager_GetFurnitureTasteScore(VillagerData *a, u16 *b);
u8 * Villager_GetFurnitureTaste(void *, u32);
u8 * Villager_GetFashionTaste(void *);
s32 Villager_GetUpcomingBirthdayDay(void *, s32, void *);
void * Villager_GetBirthday(void *);
void Villager_SetCatchphrase(void *, void *, s32);
void * VillagerMemory_GetPlayerId(void *);
void * VillagerMemory_GetTownId(void *);
void * VillagerMemory_GetTalkDate(void *);
void VillagerMemory_RecordTalk(void *, void *, u8 *, u8 *);
BOOL VillagerMemory_InitForPlayer(void *, void *, u8 *, u8 *);
s32 VillagerMemory_IsUsed(void *);
PersonalitySleepHours * Personality_GetSleepHours(u32);
u8 Date_GetStarSignOf(void *);
u32 Date_GetStarSign(u32, u32);
u8 FurnitureTaste_GetTextIndex(u8 *);
void PlanErrand_AdvanceStep(PlanErrand *self);
s32 Clock_GetTimeOfDay();
s32 Date_GetNthWeekdayDay(u32 a, u32 b, s32 c, s32 d);
u32 Clock_GetYear();
void Clock_GetDate(u8 *out);
void DateTime_AddMinutes(ClockDateTime *p, s32 n);
s32 DateTime_DiffMinutes(ClockDateTime *a, ClockDateTime *b);
s32 DateTime_DiffDays(ClockDateTime *a, ClockDateTime *b);
void Clock_GetDateTime(ClockDateTime *p);
BOOL EncodedString_SetRaw(void *buf, const void *src, s32 len);  // (defined with StrBuf *: the buffer is an EncodedString)
s32 ReddPassword_CurrentPlayerKnows();
s32 ReddShop_IsTentOpen();
ConstellationStore *Constellation_GetData();
u32 Scene_GetCurrent();
s32 Scene_InVillagerHouse();
s32 Scene_GetVillagerHouse();
s32 Scene_InMuseumRoom();
s32 SceneId_GetMuseumRoom(u32 a);
s32 Scene_InNookShop();
s32 Weather_GetFallingPrecip();
void * MI_CpuFill8(void *, s32, u32);
void MI_CpuCopy8(const void *src, void *dst, u32 size);
extern u8 gTalkMsgIndexNone[];
extern const u16 data_020c6cc8[2];
extern CommManager *const gCommManager;
extern u8 gTalkMsgIndexEnd;
#define gSaveVillagers HOST_LABEL(u8 (*)[], gSaveVillagers)
#define gContestRecord HOST_LABEL(u8 (*)[], gContestRecord)
extern SaveData gSaveData;
extern u8 gFieldSceneKind;
#define gSavePlayers HOST_LABEL(u8 (*)[], gSavePlayers)
extern BlockMap *gSceneBlockMap;
#define gSaveTownId HOST_LABEL(u16 (*)[], gSaveTownId)
#define data_021ed2c0 HOST_LABEL(u8 (*)[], data_021ed2c0)
extern const u16 data_020c6cf0[2];
}

static inline BOOL Unk_020d8938_IsSet(u16 *p)
{
    return *p != 0xfff1;
}

static inline BOOL Unk_0201f170_InRange(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 a = *p;
    u32 b = *p;
    if (b >= lo && a <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_02020b38_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

static inline s32 Unk_02021340_GetZ(VecFx32 *p) { return p->z; }

static inline BOOL Unk_02021ef8_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

static inline BOOL Unk_02021d50_R(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 a = *p;
    u32 b = *p;
    if (b >= lo && a <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_020238b0_InRange(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_020242d8_R1(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}


static inline BOOL Unk_02024df4_Range(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_02025090_Ne(u16 *p) {
    return *p != 0xfff1;
}

static inline BOOL R1(u16 *p) {
    BOOL r = FALSE;
    if (*p >= 0x450c && *p <= 0x45db) r = TRUE;
    return r;
}

static inline BOOL R2(volatile u16 *p) {
    BOOL r = FALSE;
    u16 a = *p;
    u16 b = *p;
    if (b >= 0x450c && a <= 0x45db) r = TRUE;
    return r;
}

static inline BOOL Unk_02026214_R1(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_02026ab0_R1(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_020270ec_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}

static inline BOOL Unk_0202849c_R(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u16 a = *p;
    u16 b = *p;
    if (b >= lo && a <= hi) r = TRUE;
    return r;
}

static inline BOOL Unk_02028a48_R1(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}

static inline BOOL Unk_0202849c_F(BOOL r4) {
    return gFieldSceneKind == 0 ? TRUE : r4;
}

static inline BOOL Unk_020295d0_Range(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_02029234_Range(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_020298c8_R1(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}

static inline BOOL IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

static inline BOOL InRange(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_0202be64_InRange(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_0202c094_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

static inline BOOL Unk_0202cb34_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}

static inline BOOL Unk_0202cb34_Check(u16 *p) {
    BOOL f8 = TRUE, f7 = TRUE, f6 = TRUE, f5 = TRUE, f4 = TRUE, f3 = TRUE, f2 = TRUE, f1 = FALSE;
    u32 v = *p;
    if (v <= 5) f1 = TRUE;
    if (!f1) {
        if (v < 6 || v > 0xb) f2 = FALSE;
    }
    if (!f2) {
        if (v < 0xc || v > 0x11) f3 = FALSE;
    }
    if (!f3) {
        if ((v < 0x12 || v > 0x19) && v != 0x1c) f4 = FALSE;
    }
    if (!f4) {
        if ((v < 0x8a || v > 0x8f) && (v < 0x90 || v > 0x95) && (v < 0x96 || v > 0x9b) && (v < 0x9c || v > 0xa3) && v != 0xa5) f5 = FALSE;
    }
    if (!f5) {
        if (v != 0x1a) f6 = FALSE;
    }
    if (!f6) {
        if (v != 0xa4) f7 = FALSE;
    }
    if (!f7) {
        if (v != 0x1d) f8 = FALSE;
    }
    return f8;
}

static inline MapBlock *Unk_0202cb34_Cell(TownBlockMap *g, s32 x, s32 y) {
    if ((u32)x < (u32)g->width && (u32)y < (u32)g->height && g->blocks != NULL) {
        return &g->blocks[y * g->width + x];
    }
    return NULL;
}

static inline void Unk_0202cb34_GetSize(TownBlockMap *g, Vec2 *out) {
    Vec2 *ps = (Vec2 *)&g->width;
    out->x = ps->x;
    out->y = ps->y;
}

static inline BOOL Unk_0202d664_Range(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_0202daf8_InRange(u16 v, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (v >= lo && v <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_0202e318_IsOne(u8 v) {
    if (v == 1) {
        return TRUE;
    }
    return FALSE;
}

extern "C" {
extern const u8 data_020c74f0[2];
extern const u8 data_020c74f4[2];
extern const s8 sDangerousInsects[2];
extern const u8 sSmallTalkChoiceMsgRange[2];
extern const u8 sLeaveChoiceMsgRange[2];
extern const u8 sInsectHabitatKinds[4];
extern const u8 data_020c7508[4];
extern const u8 sTsuDramaMsgBlocks[4];
extern const s8 sCatchPlanStageMinutes[5];
extern const u8 data_020c7518[5];
extern const u8 sInsectRarityWeights[5];
extern const u8 sFishRarityWeights[5];
extern const u8 data_020c7530[6];
extern const u8 sNicknamePatternBases[6];
extern const TalkTopic sTalkTopicAiSad;
extern const TalkTopic sTalkTopicQTimeover;
extern const TalkTopic sTalkTopicQIcancel;
extern const TalkTopic sTalkTopicQ07Scold2;
extern const TalkTopic sTalkTopicQ06Lost;
extern const TalkTopic sTalkTopicQ06Open;
extern const TalkTopic sTalkTopicApBuy;
extern const TalkTopic sTalkTopicQ12Full;
extern const TalkTopic sTalkTopicQ07Scold;
extern const TalkTopic sTalkTopicQ10Req;
extern const TalkTopic sTalkTopicQ10Reserve;
extern const TalkTopic sTalkTopicApMail;
extern const TalkTopic sTalkTopicAi7days;
extern const TalkTopic sTalkTopicQ06Over;
extern const TalkTopic sTalkTopicQ12FullB;
extern const TalkTopic sTalkTopicApPresent1;
extern const TalkTopic sTalkTopicAiBee;
extern const TalkTopic sTalkTopicAiFall;
extern const TalkTopic sTalkTopicAiBoom;
extern const TalkTopic sTalkTopicTsuSeHint;
extern const TalkTopic sTalkTopicTsuFlHint;
extern const TalkTopic sTalkTopicAiSnow1;
extern const TalkTopic sTalkTopicTsuFiHint;
extern const TalkTopic sTalkTopicEtcCancel;
extern const TalkTopic sTalkTopicApTrade;
extern const TalkTopic sTalkTopicAiAnger;
extern const TalkTopic sTalkTopicQ07Open;
extern const TalkTopic sTalkTopicTsuEvent2;
extern const TalkTopic sTalkTopicTsuEvent1;
extern const TalkTopic sTalkTopicEvFishing;
extern const TalkTopic sTalkTopicTsuFuAct;
extern const TalkTopic sTalkTopicQ07Over;
extern const TalkTopic sTalkTopicAiIndoor;
extern const TalkTopic sTalkTopicAiShop3;
extern const TalkTopic sTalkTopicTsuMove1;
extern const TalkTopic sTalkTopicQ07Mailgo;
extern const TalkTopic sTalkTopicQ01Plose;
extern const TalkTopic sTalkTopicQ12FullC;
extern const TalkTopic sTalkTopicEvKaraoke;
extern const TalkTopic sTalkTopicQFull;
extern const TalkTopic sTalkTopicQ02Nlose;
extern const TalkTopic sTalkTopicAiPassword;
extern const TalkTopic sTalkTopicQ01Nlose;
extern const TalkTopic sTalkTopicQ02Pdraw;
extern const TalkTopic sTalkTopicQ01Pwin;
extern const TalkTopic sTalkTopicQ02Pwin2;
extern const TalkTopic sTalkTopicEtcConnect;
extern const TalkTopic sTalkTopicTsuNoAct;
extern const TalkTopic sTalkTopicQ02Pay;
extern const TalkTopic sTalkTopicTsuFlAct;
extern const TalkTopic sTalkTopicQ01Pay;
extern const TalkTopic sTalkTopicApHabit;
extern const TalkTopic sTalkTopicApNickn;
extern const TalkTopic sTalkTopicAiRun;
extern const TalkTopic sTalkTopicQItem;
extern const TalkTopic sTalkTopicApItem;
extern const TalkTopic sTalkTopicAiPoison;
extern const TalkTopic sTalkTopicTsuInAct;
extern const TalkTopic sTalkTopicEvCountdown;
extern const TalkTopic sTalkTopicAiJoy;
extern const TalkTopic sTalkTopicQPreitem;
extern const TalkTopic sTalkTopicTsuFuHint;
extern const TalkTopic sTalkTopicAiToday1;
extern const TalkTopic sTalkTopicEvFmarket1;
extern const TalkTopic sTalkTopicTsuInHint;
extern const TalkTopic sTalkTopicAiPersis;
extern const TalkTopic sTalkTopicTsuStar;
extern const TalkTopic sTalkTopicEvAcorn;
extern const TalkTopic sTalkTopicQ06Open1;
extern const TalkTopic sTalkTopicQError3;
extern const TalkTopic sTalkTopicTsuDrama;
extern const TalkTopic sTalkTopicQError2;
extern const TalkTopic sTalkTopicAiRain2;
extern const TalkTopic sTalkTopicQError1;
extern const TalkTopic sTalkTopicEvGardeniing;
extern const TalkTopic sTalkTopicEtcPush;
extern const TalkTopic sTalkTopicApSell;
extern const TalkTopic sTalkTopicEvInsect;
extern const TalkTopic sTalkTopicQ12End;
extern const TalkTopic sTalkTopicQItemB;
extern const TalkTopic sTalkTopicQ12Thanks;
extern const TalkTopic sTalkTopicQ07Req;
extern const TalkTopic sTalkTopicQ12Report;
extern const TalkTopic sTalkTopicEvArbeit;
extern const TalkTopic sTalkTopicQTime;
extern const TalkTopic sTalkTopicQ06Req;
extern const TalkTopic sTalkTopicAiShop1;
extern const TalkTopic sTalkTopicQNo;
extern const TalkTopic sTalkTopicQYes;
extern const TalkTopic sTalkTopicTsuSpot;
extern const TalkTopic sTalkTopicAiToday2;
extern const TalkTopic sTalkTopicTsuItem;
extern const TalkTopic sTalkTopicQPreitemB;
extern const TalkTopic sTalkTopicEvFirework;
extern const TalkTopic sTalkTopicAiFortune;
extern const TalkTopic sTalkTopicQ02Plose;
extern const TalkTopic sTalkTopicTsuGhint;
extern const TalkTopic sTalkTopicAiTire;
extern const TalkTopic sTalkTopicApPresent2;
extern const TalkTopic sTalkTopicTsuMemory;
extern const TalkTopic sTalkTopicAiShop2;
extern const TalkTopic sTalkTopicTsuFriend;
extern const TalkTopic sTalkTopicAiFlea;
extern const TalkTopic sTalkTopicTsuHappyroom;
extern const TalkTopic sTalkTopicTsuAlways;
extern const TalkTopic sTalkTopicTsuDress;
extern const TalkTopic sTalkTopicQ02Nwin;
extern const TalkTopic sTalkTopicQ12Other;
extern const TalkTopic sTalkTopicQ01Pdraw;
extern const TalkTopic sTalkTopicTsuMove2;
extern const TalkTopic sTalkTopicEvAdmire;
extern const TalkTopic sTalkTopicQ02Pwin;
extern const TalkTopic sTalkTopicAiRain1;
extern const TalkTopic sTalkTopicAiForeign;
extern const TalkTopic sTalkTopicTsuSeAct;
extern const TalkTopic sTalkTopicQ01Revenge;
extern const TalkTopic sTalkTopicAi30days;
extern const TalkTopic sTalkTopicEvBirth;
extern const TalkTopic sTalkTopicQ02Revenge;
extern const TalkTopic sTalkTopicTsuClAct;
extern const TalkTopic sTalkTopicTsuFoAct;
extern const TalkTopic sTalkTopicQ01Nwin;
extern const TalkTopic sTalkTopicTsuFiAct;
extern const TalkTopic sTalkTopicQNoB;
extern const TalkTopic sTalkTopicQStart;
extern const TalkTopic sTalkTopicQ01Pwin2;
extern const TalkTopic sTalkTopicEvSnowfes;
extern const TalkTopic sTalkTopicEtcHit;
extern const TalkTopic sTalkTopicTsuNoHint;
extern const TalkTopic sTalkTopicQ07End;
extern const TalkTopic sTalkTopicAiSnow2;
extern const TalkTopic sTalkTopicQ06End;
extern const TalkTopic sTalkTopicQItemC;
extern const TalkTopic sTalkTopicQ06Bad;
extern const TalkTopic sTalkTopicQ06Normal;
extern const TalkTopic sTalkTopicQ06Good;
extern const TalkTopic sTalkTopicTsuClHint;
extern const TalkTopic sTalkTopicQ07Report;
extern const TalkTopic sTalkTopicTsuFoHint;
extern const TalkTopic sTalkTopicQ06Report;
extern const TalkTopic sTalkTopicQ07Con;
extern const TalkTopic sTalkTopicAiMfirst;
extern const TalkTopic sTalkTopicQ06Con;
extern const TalkTopic sTalkTopicQ07Fin;
extern const TalkTopic sTalkTopicQ06Fin;
extern const TalkTopic sTalkTopicQ07Show;
extern const TalkTopic sTalkTopicQ07Read;
extern const TalkTopic sTalkTopicQ10Leave;
extern const TalkTopic sTalkTopicQ07Open2;
extern const TalkTopic sTalkTopicQ10Con;
extern const TalkTopic sTalkTopicQ06Open2;
extern const TalkTopic sTalkTopicQ06Open3;
extern const TalkTopic sTalkTopicQ10Reserved;
extern const TalkTopic sTalkTopicQ06Payback;
extern const TalkTopic sTalkTopicQ07Joy;
extern const TalkTopic sTalkTopicQ06Get;
extern const char data_020c7a20[9];
extern const char data_020c7a2c[9];
extern const u32 sApTopicWeights[3];
extern const char data_020c7a44[12];
extern const char data_020c7a50[12];
extern const char data_020c7a5c[12];
extern const u32 sAcornRewardItemKinds[3];
extern const u8 sTsuTopicWeights[14];
extern const u32 sTalkTradeItemLists[4];
extern const TalkTopic sTalkTopicsTsuHome[2];
extern const u32 sMoodAnimIds[5];
extern const u32 sMoodAnimNextIds[5];
extern const RequestDatePickerFn sRequestInsectPickers[5];
extern const RequestDatePickerFn sRequestFishPickers[5];
extern const RequestFossilPickerFn sRequestFossilPickers[5];
extern const u32 sApPocketItemKinds[5];
extern const u32 sTalkErrandItemSpecs[6];
extern const TalkTopic sTalkTopicsAiFirst[3];
extern const RequestShirtPickerFn sRequestShirtPickers[7];
extern const u32 sTalkClothingItemSpecs[8];
extern const TalkTopic sTalkTopicsQEnd[5];
extern const u32 sTsuEvent2EventIds[12];
extern const u32 sTsuEvent1EventIds[14];
extern u8 data_020d7860[2];
extern char sVillagerClothMaterialName[2];
extern u8 sDeliveryMinutes[3];
extern const char *sVillagerClothMaterialNames[1];
extern char sTalkKeyQNo[5];
extern char sTalkKey3pKo[6];
extern char sTalkKeyQYes[6];
extern char sTalkKey3pFu[6];
extern char sTalkKey3pHa[6];
extern char sTalkKey3pBo[6];
extern char sTalkKey3pTa[6];
extern char sTalkKey3pGe[6];
extern char sTalkKeyQFull[7];
extern char sTalkKeyQTime[7];
extern char sTalkKeyAiBee[7];
extern char sTalkKeyAiRun[7];
extern char sTalkKeyQItem[7];
extern char sTalkKeyAiSad[7];
extern char sTalkKeyAiJoy[7];
extern char sTalkKeyApBuy[7];
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d78f0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d78f8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7900;
extern decltype(&VillagerTalkTopics::onDeliveryItemPicked) data_020d7908;
extern decltype(&VillagerTalkTopics::selectApPresent2) data_020d7910;
extern decltype(&VillagerTalkTopics::selectApItem) data_020d7918;
extern decltype(&VillagerTalkTopics::selectApPresent1) data_020d7920;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7928;
extern decltype(&VillagerTalkTopics::finishOpenedDelivery) data_020d7930;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7938;
extern decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7940;
extern decltype(&VillagerTalkTopics::onDeliveryItemPicked) data_020d7948;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7950;
extern decltype(&VillagerTalkTopics::endEtcPushHit) data_020d7958;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7960;
extern decltype(&VillagerTalkAcornTopics::onAcornPicked) data_020d7968;
extern decltype(&VillagerTalkTopics::advanceRequestStep) data_020d7970;
extern char sTalkKeyAiBoom[8];
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7980;
extern decltype(&VillagerTalkTopics::reopenWindow) data_020d7988;
extern decltype(&VillagerTalkTopics::endAiFall) data_020d7990;
extern decltype(&VillagerTalkTopics::onQ12FullClose) data_020d7998;
extern decltype(&VillagerTalk::setConstellationSlots) data_020d79a0;
extern char sTalkKeyApMail[8];
extern char sTalkKeyApSell[8];
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79b8;
extern decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d79c0;
extern decltype(&VillagerTalkKaraokeTopics::endEvFirework) data_020d79c8;
extern decltype(&VillagerTalkTopics::storeReservation) data_020d79d0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79d8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79e0;
extern decltype(&VillagerTalkTopics::selectApMail) data_020d79e8;
extern decltype(&VillagerTalkTopics::selectApBuy) data_020d79f0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79f8;
extern char sTalkKeyQ10Req[8];
extern decltype(&VillagerTalkTopics::selectApHabit) data_020d7a08;
extern char sTalkKeyQ01Pay[8];
extern decltype(&VillagerTalkTopics::selectApNickn) data_020d7a18;
extern decltype(&VillagerTalk::runCustomFn0) data_020d7a20;
extern decltype(&VillagerTalkHobbyTopics::endEvFishing) data_020d7a28;
extern decltype(&VillagerTalk::attrShowLetter) data_020d7a30;
extern decltype(&VillagerMood::beginMood2Effects) data_020d7a38;
extern decltype(&VillagerTalkRumorTopics::pickMemoryWithReceivedItem) data_020d7a40;
extern decltype(&VillagerTalkRequestReplyTopics::setDeliveryDeadline) data_020d7a48;
extern decltype(&VillagerTalkTopics::continueLateLetterShown) data_020d7a50;
extern decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d7a58;
extern decltype(&VillagerTalkTopics::advanceRequestStep) data_020d7a60;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7a68;
extern decltype(&VillagerTalkTopics::gotoQPwin2) data_020d7a70;
extern decltype(&VillagerTalkRumorTopics::pickMemoryWithCompliment) data_020d7a78;
extern TalkTopic data_020d7a80;
extern decltype(&VillagerTalkTopics::cancelRequestChain) data_020d7a88;
extern decltype(&VillagerTalkTopics::onNloseCatchPicked) data_020d7a90;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7a98;
extern decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7aa0;
extern decltype(&VillagerTalkTopics::updateEvBirthTurn) data_020d7aa8;
extern decltype(&VillagerTalkKaraokeTopics::continueEvKaraokeAction) data_020d7ab0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ab8;
extern decltype(&VillagerTalkTopics::gotoQEndOrRevenge) data_020d7ac0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ac8;
extern decltype(&VillagerMood::updateMood3Effects) data_020d7ad0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ad8;
extern decltype(&VillagerTalk::giveItemToPlayer) data_020d7ae0;
extern decltype(&VillagerTalkHolidayTopics::startEvBirthMove) data_020d7ae8;
extern decltype(&VillagerTalk::runCustomFn1) data_020d7af0;
extern decltype(&VillagerTalkTopics::selectTsuFiAct) data_020d7af8;
extern decltype(&VillagerTalk::attrOpenBirthdayEntry) data_020d7b00;
extern TalkTopic sTalkTopicQThanksDefault;
extern char sTalkKeyAiTire[8];
extern char sTalkKeyAiFlea[8];
extern decltype(&VillagerTalkTopics::onNloseCatchPicked) data_020d7b20;
extern decltype(&VillagerTalk::attrGiveItem) data_020d7b28;
extern decltype(&VillagerTalkTopics::selectTsuEvent) data_020d7b30;
extern decltype(&VillagerTalkTopics::selectTsuMemory) data_020d7b38;
extern TalkTopic data_020d7b40;
extern decltype(&VillagerTalkRumorTopics::selectTsuItem) data_020d7b48;
extern char sTalkKeyAiFall[8];
extern decltype(&VillagerTalkTopics::selectTsuFlAct) data_020d7b58;
extern decltype(&VillagerTalkRumorTopics::selectTsuSpot) data_020d7b60;
extern decltype(&VillagerMood::beginMood3Effects) data_020d7b68;
extern decltype(&VillagerTalkTopics::giveBackHeldItem) data_020d7b70;
extern decltype(&VillagerTalkRumorTopics::selectTsuHappyroom) data_020d7b78;
extern char sTalkKeyQ07End[8];
extern decltype(&VillagerTalkTopics::selectTsuAlways) data_020d7b88;
extern decltype(&VillagerTalk::receiveItemB) data_020d7b90;
extern decltype(&VillagerTalkTopics::selectTsuHobbyAct) data_020d7b98;
extern decltype(&VillagerTalkAcornTopics::continueAcornReceived) data_020d7ba0;
extern decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d7ba8;
extern char sTalkKeyQ07Con[8];
extern decltype(&VillagerTalk::receiveItem) data_020d7bb8;
extern decltype(&VillagerTalkTopics::gotoDeliveryFin) data_020d7bc0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7bc8;
extern decltype(&VillagerTalk::setHiraganaOn) data_020d7bd0;
extern decltype(&VillagerTalkTopics::keepDislikedPresent) data_020d7bd8;
extern decltype(&VillagerTalkTopics::selectApTrade) data_020d7be0;
extern decltype(&VillagerTalkTopics::checkReserveTime) data_020d7be8;
extern decltype(&VillagerMood::beginMood2Effects) data_020d7bf0;
extern TalkTopic sTalkTopicQ04Miss;
extern decltype(&VillagerTalkTopics::findTsuEvent2Message) data_020d7c00;
extern decltype(&VillagerTalkTopics::selectTsuSeAct) data_020d7c08;
extern decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7c10;
extern decltype(&VillagerTalkTopics::findTsuEvent1Message) data_020d7c18;
extern decltype(&VillagerTalkHobbyTopics::endEvInsect) data_020d7c20;
extern decltype(&VillagerTalkRequestReplyTopics::cancelRequest) data_020d7c28;
extern char sTalkKeyQ12End[8];
extern decltype(&VillagerTalkTopics::gotoQ12End) data_020d7c38;
extern char sTalkKeyQ06Req[8];
extern decltype(&VillagerTalk::attrPlayMemoryTune) data_020d7c48;
extern decltype(&VillagerTalk::attrShowLetter) data_020d7c50;
extern decltype(&VillagerTalkTopics::gotoEvArbeitEnd) data_020d7c58;
extern decltype(&VillagerTalkTopics::tryAddHandOverChoice) data_020d7c60;
extern decltype(&VillagerTalkTopics::endEtcPushHit) data_020d7c68;
extern decltype(&VillagerTalkTopics::onArbeitItemPicked) data_020d7c70;
extern decltype(&VillagerTalkRequestItemTopics::clearRequest) data_020d7c78;
extern TalkTopic sTalkTopicQCompDefault;
extern decltype(&VillagerTalkTopics::giveEvBirthPresent) data_020d7c88;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7c90;
extern decltype(&VillagerTalk::swapItemWithPlayer) data_020d7c98;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ca0;
extern RumorVillagerPair data_020d7ca8;
extern decltype(&VillagerTalk::buyItemFromPlayer) data_020d7cb0;
extern RumorVillagerPair data_020d7cb8;
extern char sTalkKeyQ07Req[8];
extern decltype(&VillagerTalkTopics::onNicknameEntered) data_020d7cc8;
extern decltype(&VillagerTalkTopics::giveBackHeldItem) data_020d7cd0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7cd8;
extern decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromVillagerTown) data_020d7ce0;
extern decltype(&VillagerTalkTopics::tryAddDeliveryRecipientChoice) data_020d7ce8;
extern decltype(&VillagerTalk::runCustomFn3) data_020d7cf0;
extern decltype(&VillagerTalkRequestItemTopics::gotoQClearOrEnd) data_020d7cf8;
extern decltype(&VillagerTalkTopics::tryAddActiveRequestChoice) data_020d7d00;
extern decltype(&VillagerTalkRequestReplyTopics::tryOfferNewRequest) data_020d7d08;
extern decltype(&VillagerTalkTopics::gotoQNlose) data_020d7d10;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7d18;
extern decltype(&VillagerTalkTopics::giveEvBirthPresent) data_020d7d20;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7d28;
extern decltype(&VillagerTalk::sellItemToPlayer) data_020d7d30;
extern TalkTopic sTalkTopicQClearDefault;
extern decltype(&VillagerTalkKaraokeTopics::endEvKaraokeMsg8) data_020d7d40;
extern decltype(&VillagerTalkTopics::checkRequestItem) data_020d7d48;
extern decltype(&VillagerTalk::runCustomFn2) data_020d7d50;
extern char sTalkKeyQ01End[8];
extern decltype(&VillagerTalkKaraokeTopics::waitEvKaraokeAction) data_020d7d60;
extern char sTalkKeyQ02Pay[8];
extern decltype(&VillagerTalkTopics::stopBirthdayBgm) data_020d7d70;
extern decltype(&VillagerTalkTopics::gotoQPay) data_020d7d78;
extern char sTalkKeyQ02End[8];
extern char sTalkKeyQ03End[8];
extern decltype(&VillagerTalkRumorTopics::pickMemoryLikedOld) data_020d7d90;
extern decltype(&VillagerTalkTopics::selectApSell) data_020d7d98;
extern decltype(&VillagerTalkRequestItemTopics::finishRequestChain) data_020d7da0;
extern decltype(&VillagerTalkTopics::onHabitEntered) data_020d7da8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7db0;
extern char sTalkKeyQ04End[8];
extern char sTalkKeyQ05End[8];
extern decltype(&VillagerTalkRumorTopics::pickMemoryDislikedOld) data_020d7dc8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7dd0;
extern decltype(&VillagerTalkTopics::selectTsuNoAct) data_020d7dd8;
extern decltype(&VillagerTalkHolidayTopics::updateEvBirthMove) data_020d7de0;
extern char sTalkKeyEtcHit[8];
extern decltype(&VillagerTalkTopics::playBirthdayBgm) data_020d7df0;
extern decltype(&VillagerTalk::runCustomFn4) data_020d7df8;
extern decltype(&VillagerTalkTopics::selectTsuClAct) data_020d7e00;
extern decltype(&VillagerMood::updateMood2Effects) data_020d7e08;
extern decltype(&VillagerTalkRumorTopics::pickMemoryWithCompliment) data_020d7e10;
extern decltype(&VillagerTalkTopics::selectTsuInAct) data_020d7e18;
extern decltype(&VillagerTalkTopics::selectTsuFoAct) data_020d7e20;
extern decltype(&VillagerTalkTopics::compareCatchPrice) data_020d7e28;
extern decltype(&VillagerTalkTopics::onCatchPicked) data_020d7e30;
extern TalkTopic sTalkTopicQ05TalkDefault;
extern decltype(&VillagerTalkTopics::cancelRequestChain) data_020d7e40;
extern TalkTopic sTalkTopicQConDefault;
extern decltype(&VillagerTalkKaraokeTopics::onEvAdmireWordEntered) data_020d7e50;
extern decltype(&VillagerTalkRumorTopics::pickMemoryWithTime) data_020d7e58;
extern char sTalkKeyQ05Con[8];
extern decltype(&VillagerMood::beginMood1Effects) data_020d7e68;
extern decltype(&VillagerTalkRumorTopics::pickMemoryAny) data_020d7e70;
extern TalkTopic sTalkTopicQReturnDefault;
extern decltype(&VillagerMood::updateMood1Effects) data_020d7e80;
extern decltype(&VillagerTalkTopics::gotoQRewardTopic) data_020d7e88;
extern char sTalkKeyApItem[8];
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7e98;
extern decltype(&VillagerTalkTopics::selectTsuStar) data_020d7ea0;
extern decltype(&VillagerTalkTopics::selectTsuDrama) data_020d7ea8;
extern decltype(&VillagerTalkRequestStartTopics::registerRequestDeclined) data_020d7eb0;
extern char sTalkKeyQStart[8];
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ec0;
extern decltype(&VillagerTalkRequestStartTopics::registerRequestDeferred) data_020d7ec8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ed0;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ed8;
extern decltype(&VillagerTalkTopics::selectTsuFuAct) data_020d7ee0;
extern decltype(&VillagerTalk::attrShowLetter) data_020d7ee8;
extern decltype(&VillagerTalkTopics::selectTsuNoHint) data_020d7ef0;
extern decltype(&VillagerTalkTopics::selectTsuSeHint) data_020d7ef8;
extern char sTalkKeyQ05Req[8];
extern decltype(&VillagerTalkTopics::selectTsuFlHint) data_020d7f08;
extern decltype(&VillagerTalkRumorTopics::selectTsuHome) data_020d7f10;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f18;
extern decltype(&VillagerTalkTopics::selectTsuFuHint) data_020d7f20;
extern decltype(&VillagerTalkTopics::selectTsuClHint) data_020d7f28;
extern decltype(&VillagerTalkTopics::selectTsuFoHint) data_020d7f30;
extern decltype(&VillagerTalk::showMoneyItem) data_020d7f38;
extern decltype(&VillagerTalkTopics::selectTsuFiHint) data_020d7f40;
extern decltype(&VillagerTalkRumorTopics::selectTsuFriend) data_020d7f48;
extern decltype(&VillagerTalkTopics::selectTsuInHint) data_020d7f50;
extern decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromResident) data_020d7f58;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f60;
extern char sTalkKeyQ06End[8];
extern decltype(&VillagerTalkHobbyTopics::onEvAdmireWordEnteredB) data_020d7f70;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f78;
extern decltype(&VillagerTalkTopics::selectTsuDress) data_020d7f80;
extern char sTalkKeyQ06Bad[8];
extern decltype(&VillagerTalkTopics::selectTsuGhint) data_020d7f90;
extern decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromResident) data_020d7f98;
extern decltype(&VillagerTalkTopics::onPresentOpinionChoice) data_020d7fa0;
extern decltype(&VillagerTalkTopics::selectTsuHobbyHint) data_020d7fa8;
extern char sTalkKeyQ06Con[8];
extern char sTalkKeyQ07Fin[8];
extern char sTalkKeyQ06Fin[8];
extern decltype(&VillagerTalkTopics::closeLetter) data_020d7fc8;
extern decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7fd0;
extern decltype(&VillagerTalk::setHiraganaOff) data_020d7fd8;
extern decltype(&VillagerTalkTopics::gotoDeliveryFin) data_020d7fe0;
extern decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7fe8;
extern decltype(&VillagerTalkTopics::finishOpenedLetter) data_020d7ff0;
extern char sTalkKeyQ10Con[8];
extern decltype(&VillagerTalkRequestReplyTopics::tryAddSickVillagerChoice) data_020d8000;
extern decltype(&VillagerTalk::playOwnerIdleAnim) data_020d8008;
extern decltype(&VillagerTalkTopics::onQ12EndClose) data_020d8010;
extern decltype(&VillagerTalkTopics::judgePresent) data_020d8018;
extern char sTalkKeyQ07Joy[8];
extern char sTalkKeyQ06Get[8];
extern char sTalkKeyEtcPush[9];
extern char sTalkKeyQ12Full[9];
extern char sTalkKeyQ06Over[9];
extern char sTalkKeyQ07Over[9];
extern char sTalkKeyQ06Lost[9];
extern char sTalkKeyQ06Open[9];
extern char sTalkKeyQ07Open[9];
extern char sTalkKeyQError1[9];
extern char sTalkKeyQError2[9];
extern char sTalkKeyQError3[9];
extern char sTalkKeyEvAcorn[9];
extern char sTalkKeyTsuStar[9];
extern char sTalkKeyQ07Read[9];
extern char sTalkKeyQ07Show[9];
extern char sTalkKeyQ06Good[9];
extern char sTalkKeyQ01Req1[9];
extern char sTalkKeyQ01Req3[9];
extern char sTalkKeyQ02Req1[9];
extern char sTalkKeyQ02Req3[9];
extern char sTalkKeyQ03Req3[9];
extern char sTalkKeyQ01Con1[9];
extern char sTalkKeyQ01Con3[9];
extern char sTalkKeyQ02Con1[9];
extern char sTalkKeyQ02Con3[9];
extern char sTalkKeyEvBirth[9];
extern char sTalkKeyQ03Con3[9];
extern char sTalkKeyQ01Pwin[9];
extern char sTalkKeyQ02Pwin[9];
extern char sTalkKeyAiFirst[9];
extern char sTalkKeyQ01Nwin[9];
extern char sTalkKeyQ02Nwin[9];
extern char sTalkKeyAiAnger[9];
extern char sTalkKeyAi7days[9];
extern char sTalkKeyAiShop1[9];
extern char sTalkKeyAiShop2[9];
extern char sTalkKeyAiShop3[9];
extern char sTalkKeyAiRain1[9];
extern char sTalkKeyAiSnow1[9];
extern char sTalkKeyQ04Miss[9];
extern char sTalkKeyQ05Miss[9];
extern char sTalkKeyAiRain2[9];
extern char sTalkKeyAiSnow2[9];
extern char sTalkKeyTsuSpot[9];
extern char sTalkKeyApHabit[9];
extern char sTalkKeyApNickn[9];
extern char sTalkKeyTsuItem[9];
extern char sTalkKeyApTrade[9];
extern char sTalkKeyQ01Comp[9];
extern char sTalkKeyQ02Comp[9];
extern char sTalkKeyQ03Comp[9];
extern char sTalkKeyQ05Comp[9];
extern char sTalkKeyEvArbeit[10];
extern char sTalkKeyQ12Other[10];
extern char sTalkKeyEvInsect[10];
extern char sTalkKeyQ07Scold[10];
extern char sTalkKeyQIcancel[10];
extern char sTalkKeyTsuDrama[10];
extern char sTalkKeyQ06Open1[10];
extern char sTalkKeyQ06Open3[10];
extern char sTalkKeyQ06Open2[10];
extern char sTalkKeyQ07Open2[10];
extern char sTalkKeyQ10Leave[10];
extern char sTalkKeyQPreitem[10];
extern char sTalkKeyTsuMove1[10];
extern char sTalkKeyTsuMove2[10];
extern char sTalkKeyQ01Pwin2[10];
extern char sTalkKeyQ01Plose[10];
extern char sTalkKeyQ02Plose[10];
extern char sTalkKeyQ02Pwin2[10];
extern char sTalkKeyQ01Pdraw[10];
extern char sTalkKeyQ02Pdraw[10];
extern char sTalkKeyAiNfirst[10];
extern char sTalkKeyAiMfirst[10];
extern char sTalkKeyTsuGhint[10];
extern char sTalkKeyAiPoison[10];
extern char sTalkKeyTsuDress[10];
extern char sTalkKeyQ01Nlose[10];
extern char sTalkKeyQ02Nlose[10];
extern char sTalkKeyAi30days[10];
extern char sTalkKeyAiIndoor[10];
extern char sTalkKeyAiToday1[10];
extern char sTalkKeyAiPersis[10];
extern char sTalkKeyAiToday2[10];
extern char sTalkKeyTsuShome[10];
extern char sTalkKeyTsuGhome[10];
extern char sTalkKeyEvAdmire[10];
extern char sTalkKeyQ04Dress[10];
extern char sTalkKeyEvFishing[11];
extern char sTalkKeyQ12Report[11];
extern char sTalkKeyQ12Thanks[11];
extern char sTalkKeyTsuEvent1[11];
extern char sTalkKeyQ07Mailgo[11];
extern char sTalkKeyTsuEvent2[11];
extern char sTalkKeyQ07Scold2[11];
extern char sTalkKeyQTimeover[11];
extern char sTalkKeyEtcCancel[11];
extern char sTalkKeyQ06Report[11];
extern char sTalkKeyQ07Report[11];
extern char sTalkKeyQ06Normal[11];
extern char sTalkKeyEvSnowfes[11];
extern char sTalkKeyQ03Req12[11];
extern char sTalkKeyQ03Req45[11];
extern char sTalkKeyQ04Req12[11];
extern char sTalkKeyQ04Req37[11];
extern char sTalkKeyTsuInAct[11];
extern char sTalkKeyTsuFiAct[11];
extern char sTalkKeyTsuFoAct[11];
extern char sTalkKeyTsuClAct[11];
extern char sTalkKeyQ03Con12[11];
extern char sTalkKeyQ03Con45[11];
extern char sTalkKeyTsuFuAct[11];
extern char sTalkKeyQ04Con12[11];
extern char sTalkKeyQ04Con37[11];
extern char sTalkKeyTsuFlAct[11];
extern char sTalkKeyTsuSeAct[11];
extern char sTalkKeyTsuNoAct[11];
extern char sTalkKeyEvKaraoke[11];
extern char sTalkKeyAiForeign[11];
extern char sTalkKeyTsuAlways[11];
extern char sTalkKeyAiFortune[11];
extern char sTalkKeyTsuFriend[11];
extern char sTalkKeyQ03Thanks[11];
extern char sTalkKeyQ04Thanks[11];
extern char sTalkKeyQ05Thanks[11];
extern char sTalkKeyTsuMemory[11];
extern char sTalkKeyQ01Return[11];
extern char sTalkKeyQ02Return[11];
extern char sTalkKeyQ03Return[11];
extern char sTalkKeyQ05Return[11];
extern char sTalkKeyQ06Payback[12];
extern char sTalkKeyQ10Reserve[12];
extern char sTalkKeyTsuInHint[12];
extern char sTalkKeyEvFmarket1[12];
extern char sTalkKeyTsuFiHint[12];
extern char sTalkKeyTsuFoHint[12];
extern char sTalkKeyTsuClHint[12];
extern PresentOpinionTopicList sPresentOpinionTopics;
extern char sTalkKeyTsuFuHint[12];
extern char sTalkKeyTsuFlHint[12];
extern char sTalkKeyTsuSeHint[12];
extern char sTalkKeyTsuNoHint[12];
extern char sTalkKeyQ05Talk12[12];
extern char sTalkKeyQ05Talk35[12];
extern char sTalkKeyQ05Talk67[12];
extern char sTalkKeyAiPassword[12];
extern char sTalkKeyQ01Revenge[12];
extern char sTalkKeyQ02Revenge[12];
extern char sTalkKeyEvFirework[12];
extern char sTalkKeyApPresent1[12];
extern char sTalkKeyApPresent2[12];
extern char sTalkKeyEtcConnect[12];
extern char sTalkKeyQ10Reserved[13];
extern char sTalkKeyQ01Req245[13];
extern char sTalkKeyQ02Req245[13];
extern char sTalkKeyEvCountdown[13];
extern char sTalkKeyQ01Con245[13];
extern char sTalkKeyQ02Con245[13];
extern char sTalkKeyEvGardeniing[14];
extern char sTalkKeyTsuHappyroom[14];
extern const char *sTalkKeysQ01Req[5];
extern const char *sTalkKeysQ02Req[5];
extern const char *sTalkKeysQ03Req[5];
extern const char **sTalkKeysQReq[5];
extern const char *sTalkKeysQ01Con[5];
extern const char *sTalkKeysQ02Con[5];
extern const char *sTalkKeysQ03Con[5];
extern const char **sTalkKeysQCon[5];
extern const char *sTalkKeysQReturn[5];
extern const char *sTalkKeysQComp[5];
extern const char *sTalkKeys3p[6];
extern const char *sTalkKeysQ04Req[7];
extern const char *sTalkKeysQ05Req[7];
extern const char *sTalkKeysQ04Con[7];
extern const char *sTalkKeysQ05Con[7];
extern const char *sTalkKeysQ05Talk[7];
extern u8 sFishRowFlags[2];
extern u8 sFishSlotFlags[3];
extern u8 sFishRarityWork[8];
extern u8 sInsectRarityWork[8];
extern u8 sInsectSlotFlags[8];
extern u8 sTsuTopicWeightsWork[14];
extern VillagerTalkTopicFns sConnectTopic[1];
extern VillagerTalkTopicFns sEtcCancelTopicTable[1];
extern u8 sVillagerTexturePathBuf[32];
extern u8 sVillagerModelPathBuf[32];
extern u32 sTalkInputBuffer[8];
extern VillagerTalkTopicFns sSmallTalkTopicTable[2];
extern VillagerTalkTopicFns sHouseVisitTsuTopicTable[3];
extern VillagerTalkTopicFns sEvGardeniingTopicTable[4];
extern VillagerTalkTopicFns sEvSnowfesTopicTable[4];
extern VillagerTalkTopicFns sEtcConnectTopicTable[6];
extern VillagerTalkTopicFns sEvBirthTopicTable[7];
extern VillagerTalkTopicFns sEvAdmireTopicTable[8];
extern VillagerTalkTopicFns sEvCountdownTopicTable[8];
extern VillagerTalkTopicFns sEvKaraokeTopicTable[9];
extern VillagerTalkTopicFns sEvAcornTopicTable[9];
extern Letter sTalkLetter;
extern VillagerTalkTopicFns sApSubTopics[13];
extern VillagerTalkTopicFns sTalkBeginTopics[17];
extern VillagerTalkTopicFns sRequestTopicsA[26];
extern VillagerTalkTopicFns sRequestTopicsB[47];
}

extern "C" {
decltype(&VillagerTalkRumorTopics::selectTsuSpot) data_020d7b60 = &VillagerTalkRumorTopics::selectTsuSpot;
const TalkTopic sTalkTopicQ02Pdraw = {sTalkKeyQ02Pdraw, 0x2};
decltype(&VillagerTalkTopics::selectApSell) data_020d7d98 = &VillagerTalkTopics::selectApSell;
const TalkTopic sTalkTopicEvFmarket1 = {sTalkKeyEvFmarket1, 0x6};
decltype(&VillagerTalkKaraokeTopics::onEvAdmireWordEntered) data_020d7e50 = &VillagerTalkKaraokeTopics::onEvAdmireWordEntered;
const TalkTopic sTalkTopicQ02Pwin2 = {sTalkKeyQ02Pwin2, 0x1};
char sTalkKeyAiRain1[9] = "ai_rain1";
char sTalkKeyTsuFuAct[11] = "tsu_fu_act";
decltype(&VillagerTalkTopics::selectTsuClAct) data_020d7e00 = &VillagerTalkTopics::selectTsuClAct;
const TalkTopic sTalkTopicQ06End = {sTalkKeyQ06End, 0x3};
char sTalkKeyAiTire[8] = "ai_tire";
char sTalkKeyQ04Con12[11] = "q04_con1_2";
TalkTopic sTalkTopicQCompDefault = {NULL, 0x00000003};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7bc8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7c10 = &VillagerTalk::playOwnerIdleAnim;
const TalkTopic sTalkTopicQ12End = {sTalkKeyQ12End, 0x3};
char sTalkKeyAiFall[8] = "ai_fall";
const TalkTopic sTalkTopicAiShop3 = {sTalkKeyAiShop3, 0x5};
char sTalkKeyQ02Pay[8] = "q02_pay";
decltype(&VillagerTalkTopics::selectTsuInAct) data_020d7e18 = &VillagerTalkTopics::selectTsuInAct;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7c90 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkRumorTopics::pickMemoryWithReceivedItem) data_020d7a40 = &VillagerTalkRumorTopics::pickMemoryWithReceivedItem;
}

extern "C" void TalkChoiceList_SetIndices(ActorTalkRequest *a, u8 *src, s32 n, s32 m) {
    void *h = a->getChoiceList();
    ((ChoiceList *)(h))->reset(n, m);
    s32 zero = 0;
    u8 buf[2];
    for (s32 i = 0; i < n; src++, i++) {
        buf[0] = *src;
        buf[1] = 0xff;
        ((ChoiceList *)(h))->setEntry(i, (const u8 *)(buf), zero, (const u8 *)(buf + 1), NULL, zero);
    }
    ((ChoiceList *)(h))->loadTexts();
}

extern "C" BOOL Talk_CheckAndSetPlayerFlag(u32 a, BOOL flag) {
    void *o = PlayerData_GetCurrent();
    if (o == NULL) {
        return FALSE;
    }
    void *h = ((PlayerData *)(o))->getDailyTalkFlags();
    BOOL r = TRUE;
    if (((PlayerDailyTalkFlags *)(h))->test(a) == 0) {
        r = FALSE;
    }
    if (flag) {
        ((PlayerDailyTalkFlags *)(h))->set(a);
    }
    if (r) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL Talk_IsDramaPending(void *unused, Unk_020874e8_Bits *p, u32 mode) {
    ClockDateTimeWords buf;
    buf.words[0] = 0;
    buf.words[1] = 0;
    Clock_GetDateTime(&buf.dt);
    if (Drama_GetAvailablePart(buf.dt.year, buf.dt.month, buf.dt.day, p)) {
        if (p->a == mode) {
            return TRUE;
        }
    }
    return FALSE;
}

extern "C" void Talk_AdvanceDrama(void *unused, Unk_020874e8_Bits *part) {
    DramaRecord_SetLastPart(PlayerData_GetCurrent()->getDramaRecord(), part);
}

extern "C" BOOL Talk_IsInOwnTown() {
    void *g = gCommManager;
    if ((u32)((CommManager *)(g))->isOnline()) {
        if (((CommManager *)(g))->isMyAid(0) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

VillagerActor::VillagerActor() {}

VillagerActor::~VillagerActor() {}

void VillagerActor::attachVillagerData() {
    if (((npcHandle & 0xf000) >> 12) == 0xe) {
        villagerData = static_cast<VillagerData *>(SaveVillagers_Get(gSaveVillagers, (u16)getNpcIndex()));
        if (villagerData != NULL) {
            villagerState = Villager_GetState(villagerData);
        } else {
            villagerState = NULL;
        }
    }
}

s32 VillagerActor::getSpeciesOrNone() {
    s32 r = -1;
    if (villagerData != NULL) {
        if (((u32)(villagerData->getVillagerId())->isValid()) == 1) {
            r = VillagerId_GetSpecies(villagerData->getVillagerId());
        }
    }
    return r;
}

void VillagerActor::getName(MsgString *buf) {
    if (villagerData != NULL) {
        (villagerData->getVillagerId())->getName(buf);
    }
}

u32 VillagerActor::getGender() {
    u32 r = 2;
    if (villagerData != NULL) {
        r = (villagerData->getVillagerId())->getGender();
    }
    return r;
}

BOOL VillagerActor::canPlayTalkMelody() { return TRUE; }

void VillagerActor::onTalkMelodyPlayed() {}

u16 VillagerActor::getSpecies() {
    if (((npcHandle & 0xf000) >> 12) == 0xe && villagerData != NULL) {
        return VillagerId_GetSpecies(villagerData->getVillagerId());
    }
    return 0xffff;
}

u8 *VillagerActor::getTexturePath() {
    s32 v = getSpeciesOrNone();
    if (v == -1) {
        v = 0;
    }
    Str_SPrintf((char *)sVillagerTexturePathBuf, "npc/model/%d/%d.nsbtx", v & 0xf8, v);
    return sVillagerTexturePathBuf;
}

u8 *VillagerActor::getModelPath() {
    s32 v = getSpeciesOrNone();
    if (v == -1) {
        v = 0;
    }
    Str_SPrintf((char *)sVillagerModelPathBuf, "npc/model/%d/%d.nsbmd", v & 0xf8, v);
    return sVillagerModelPathBuf;
}

void *VillagerActor::getVillagerData() { return villagerData; }

void VillagerActor::addMood(u32 a, s32 b) {
    if (mood.isActive()) {
        mood.addMood(a, b);
    }
}

BOOL VillagerActor::isPickable() { return TRUE; }

BOOL VillagerActor::vfunc_ac() { return FALSE; }

BOOL VillagerActor::consumeFleaRemoved() { return FALSE; }

void VillagerActor::setShirt(u16 *p, BOOL flag) {
    BOOL in = FALSE;
    u16 v = *p;
    if (v >= 0x11a8 && v <= 0x12a7) {
        in = TRUE;
    }
    if (in || (v >= 0x12a8 && v <= 0x12af)) {
        void *o = getVillagerData();
        (&clothModel)->change(this, p);
        if (o != NULL) {
            ((VillagerData *)(o))->setShirt(p);
            if (flag) {
                VillagerSync_Shirt(o, p);
            }
        }
    }
}

BOOL VillagerActor::preCreate() {
    if (!NpcActor::preCreate()) {
        return FALSE;
    }
    attachVillagerData();
    talkPartner = NULL;
    habitTopicKind = ((u32)Random_GlobalBelow(2));
    mood.start();
    ((VillagerTalk *)this)->refreshEventKind();
    return TRUE;
}

BOOL VillagerActor::loadAnimSet() {
    s32 *t = (s32 *)animHeapHandle.getVillagerAnimHeapRef();
    if (!(&model)->allocJointRecord(VillagerAnimHeapRef_GetHeap(t))) {
        return FALSE;
    }
    if ((&model)->allocLayer3Anims(VillagerAnimHeapRef_GetHeap(t))) {
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerActor::onCreate() {
    u16 h = 0x11a8;
    if (!this->NpcActor::onCreate()) {
        return FALSE;
    }
    if (!animHeapHandle.getVillagerAnimHeapRef()) {
        if (!animHeapHandle.acquire()) {
            return FALSE;
        }
        if (!this->loadAnimSet()) {
            return FALSE;
        }
    }
    if (!faceAnim.load(this)) {
        return FALSE;
    }
    if (!animCtrl.initForActor((this), data_020c6cf0[0])) {
        return FALSE;
    }
    actionCtrl.startAction((this), 0, 1, 0, 0, 0, 0, 0);
    if (villagerData) {
        h = *((VillagerData *)(villagerData))->getShirt();
    }
    if (!clothModel.init(this, &h)) {
        return FALSE;
    }
    collider.setupForActor(this, 0x1000, 0x2000, 8, 0x2fc, 2, (u8)(this)->getNpcIndex(), 0x1000);
    if (NpcRegistry_AddVillager(this, &npcHandle)) {
    } else {
        return FALSE;
    }
    return TRUE;
}

BOOL VillagerActor::preDelete() {
    if (!this->Character::preDelete()) {
        return FALSE;
    }
    NpcRegistry_RemoveVillager(&npcHandle);
    return TRUE;
}

BOOL VillagerActor::onDelete() {
    if (!this->NpcActor::onDelete()) {
        return FALSE;
    }
    animHeapHandle.release();
    clothModel.release();
    mood.stop();
    return TRUE;
}

void VillagerActor::setFlag834() { unk_834 = 1; }

void VillagerActor::clearFlag834() { unk_834 = 0; }

BOOL VillagerActor::isFlag834() {
    if (unk_834) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void Npc_GetStateHeldItem(u16 *out, VillagerActor *obj) {
    *out = 0xfff1;
    if (obj->getVillagerData() != 0) {
        if ((((VillagerData *)(obj->getVillagerData()))->getVillagerId())->isValid() != 0) {
            if (Villager_GetState(obj->getVillagerData()) != 0) {
                *out = *VillagerState_GetHeldItem(Villager_GetState(obj->getVillagerData()));
            }
        }
    }
}

void VillagerTalk::setSpeakerStateUnk(void *arg) {
    if (((VillagerActor *)this)->getVillagerData() != 0) {
        if ((((VillagerData *)(((VillagerActor *)this)->getVillagerData()))->getVillagerId())->isValid() != 0) {
            if (Villager_GetState(((VillagerActor *)this)->getVillagerData()) != 0) {
                VillagerState_SetHeldItem(Villager_GetState(((VillagerActor *)this)->getVillagerData()), arg);
            }
        }
    }
}

VillagerClothModel *VillagerClothModel::construct() {
    new (this) MatTexVramTask();
    clothItem = 0xfff1;
    new (texPatBuf) NpcClothTexHandle();
    return this;
}

VillagerClothModel *VillagerClothModel::destruct() {
    ((NpcClothTexHandle *)(texPatBuf))->NpcClothTexHandle::~NpcClothTexHandle();
    return this;
}

BOOL VillagerClothModel::init(VillagerActor *parent, u16 *id) {
    clothItem = 0xffff;
    if (!((NpcClothTexHandle *)texPatBuf)->acquire()) {
        return FALSE;
    }
    if (change(parent, id)) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerClothModel::buildTexture(VillagerActor *parent, u16 *id) {
    u8 *r4 = (u8 *)((NpcClothTexHandle *)texPatBuf)->getClothTex();
    void *r6 = 0;
    if (r4) {
        BOOL in = FALSE;
        if (*id >= 0x11a8 && *id <= 0x12a7) {
            in = TRUE;
        }
        if (in == 1) {
            ((CharaClothTexRef *)(r4))->loadItem((u16 *)(id), 0, 1, 1);
        } else if (parent->villagerData) {
            CharaClothTexRef_LoadPattern(r4, parent->villagerData->getPattern());
        } else {
            ((CharaClothTexRef *)(r4))->loadItem((u16 *)(id), 0, 1, 1);
        }
        void *buf = CharaClothTexRef_GetBuffer(r4);
        if (buf) {
            r6 = ClothTex_GetTexThunk(buf);
        }
    }
    return r6;
}

BOOL VillagerClothModel::change(VillagerActor *parent, u16 *id) {
    BOOL result = FALSE;
    BOOL same;
    void *p;
    if (Item_IsFurniture(id)) {
        u32 a = Item_GetFurnitureIndex(id);
        if (a == Item_GetFurnitureIndex(&clothItem)) {
            same = TRUE;
        } else {
            same = FALSE;
        }
    } else {
        if (*id == clothItem) {
            same = TRUE;
        } else {
            same = FALSE;
        }
    }
    if (!same || (Unk_0202d664_Range(id, 0x12a8, 0x12af) && Unk_0202d664_Range(&clothItem, 0x12a8, 0x12af))) {
        p = buildTexture(parent, id);
        if (p) {
            result = ((MatTexVramTask *)(this))->request(parent->model.resMdl, sVillagerClothMaterialNames[0], p, 0, 0);
            clothItem = *id;
        }
    }
    return result;
}

void VillagerClothModel::release() {
    ((MatTexVramTask *)(this))->cancel();
    ((NpcClothTexHandle *)texPatBuf)->release();
}

u16 *VillagerClothModel::getItem() { return &clothItem; }

VillagerTalk::VillagerTalk() {
    itemFromPlayer = 0xfff1;
    itemToPlayer = 0xfff1;
}

VillagerTalk::~VillagerTalk() {
}

void VillagerTalk::begin(VillagerActor *owner, u32 idx) {
    void *r6;
    if (PlayerData_GetCurrent() != 0) {
        r6 = PlayerData_GetCurrent()->getPlayerId();
    } else {
        r6 = 0;
    }
    resetMsg();
    actor = owner;
    if (idx < 0x11) {
        setTopicFns(&sTalkBeginTopics[idx]);
    }
    unk_c4 = nullptr;
    unk_cc = nullptr;
    Unk_020d8938_Fn t = nullptr;
    taskDoneFn = t;
    nextTaskDoneFn = t;
    deferredFn = t;
    updateFn = t;
    closeFn = t;
    itemFromPlayer = 0xfff1;
    unk_134 = 0;
    memoryIndex = -1;
    unk_128_p = NULL;
    partnerMemoryIndex = -1;
    unk_130_p = NULL;
    if (r6 != 0 && actor != 0) {
        memoryIndex = Villager_FindMemoryIndexById(actor->villagerData, r6);
        unk_128_p = Villager_GetMemory(actor->villagerData, memoryIndex);
    }
    this->clearChoiceValues();
    errandRecord = 0;
    unk_154 = 0;
    unk_155 = 0;
    unk_158 = 0;
    unk_15c = 0;
    unk_160 = 0;
    unk_164 = 0;
    Unk_020d8938_Fn t2 = nullptr;
    unk_168 = t2;
    unk_170 = t2;
    unk_178 = t2;
    unk_180 = t2;
    unk_188 = t2;
    itemToPlayer = 0xfff1;
    itemToPlayerIsReceived = 0;
    unk_19c = 0;
    MI_CpuFill8(((u8 *)&sTalkInputBuffer), 0, 0x20);
    unk_138 = 0;
}

void VillagerTalk::update() {
    this->ActorTalkRequest::update();
    if (updateFn) {
        (this->*updateFn)();
    }
}

void VillagerTalk::setTaskDoneFn(Unk_020d8938_Fn fn) { taskDoneFn = fn; }

void VillagerTalk::setNextTaskDoneFn(Unk_020d8938_Fn fn) { nextTaskDoneFn = fn; }

void VillagerTalk::onTaskDone(u32 id) {
    Unk_020d8938_Fn t;
    if (taskDoneFn) {
        (this->*taskDoneFn)();
        t = nullptr;
        taskDoneFn = t;
        if (nextTaskDoneFn) {
            taskDoneFn = nextTaskDoneFn;
        }
        nextTaskDoneFn = t;
    }
}

void VillagerTalk::setDeferredFn(Unk_020d8938_Fn fn) { deferredFn = fn; }

void VillagerTalk::runDeferred() {
    if (deferredFn) {
        (this->*deferredFn)();
        deferredFn = nullptr;
    }
}

void VillagerTalk::clearTopicFns() {
    selectFn = nullptr;
    Unk_020d8938_Fn t = nullptr;
    unk_b4 = t;
    unk_bc = t;
}

void VillagerTalk::setTopicFns(VillagerTalkTopicFns *t) {
    selectFn = t->a;
    unk_b4 = t->b;
    unk_bc = t->c;
}

void VillagerTalk::setChoiceFn(Unk_020d8938_Fn fn) { unk_cc = fn; }

extern "C" void Talk_SelectTopicMessage(void *self, void *buf, void *out, s32 a3, u8 s0, const char *s1, u8 s2, s32 s3, u8 s4) {
    u32 k = s2;
    u8 t = k * s3;
    if (s4) {
        k = s4;
    }
    Villager_MakePersonalityFileName(buf, a3, s1, s0);
    *(u8 *)out = t + Random_GlobalBelow(k);
}

void *VillagerTalk::getActorByIndex(s32 idx) {
    switch (idx) {
    case 0:
        return actor;
    case 1:
        if (actor) {
            return ((VillagerTalk *)actor)->getPartner();
        }
    }
    return 0;
}

void *VillagerTalk::getSpeakerData() {
    void *r = NULL;
    VillagerActor *p = (VillagerActor *)getActorByIndex(this->getSpeakerIndex());
    if (p) {
        r = p->villagerData;
    }
    return r;
}

void VillagerTalk::setErrandRecord(ErrandRecord *v) { errandRecord = v; }

ErrandRecord *VillagerTalk::getErrandRecord() { return errandRecord; }

void VillagerTalk::start(TalkStartMsg *arg) {
    if (selectFn) {
        (this->*reinterpret_cast<Unk_020d8938_ArgFn>(selectFn))(arg);
    }
    return;
}

extern "C" void VillagerTalk_EnsureMemory(VillagerTalk *self, void **pa, s32 *pb, void *c, void *d) {
    if (self->unk_138 == 0) {
        if (d == NULL) {
            if (PlayerData_GetCurrent() != 0) {
                d = PlayerData_GetCurrent()->getPlayerId();
            }
        }
        if (d != NULL && c != NULL) {
            if (*pa == NULL) {
                *pb = Villager_FindMemoryIndexById(c, d);
                if (*pb == -1) {
                    *pb = Villager_PickMemorySlotForNew(c);
                    if (*pb != -1) {
                        // the decomp passed only c: on the DS the slot index was still in r1 (loaded for the -1 test)
                        *pa = Villager_GetMemory(c, *pb);
                        VillagerMemory_InitForPlayer(*pa, d, 0, 0);
                        VillagerSync_MemoryInit(c, *pb);
                    }
                }
            } else {
                VillagerMemory_RecordTalk(*pa, d, 0, 0);
                VillagerSync_MemorySetPlayer(c, *pb);
            }
        }
    }
}

void VillagerTalk::onTag09_9() {
    this->requestSwitchSpeaker(0);
}

void VillagerTalk::onActionTag0() {
    NpcActor *s = this->getActionActor();
    if (s != NULL) {
        s->addMood(0, 0);
    }
}

void VillagerTalk::onActionTag1(u32 a) {
    NpcActor *s = this->getActionActor();
    if (s != NULL) {
        s->addMood(1, a);
    }
}

void VillagerTalk::onActionTag2(u32 a) {
    NpcActor *s = this->getActionActor();
    if (s != NULL) {
        s->addMood(2, a);
    }
}

void VillagerTalk::onActionTag3(u32 a) {
    NpcActor *s = this->getActionActor();
    if (s != NULL) {
        s->addMood(3, a);
    }
}

void VillagerTalk::onActionTag4(u32 a) {
    NpcActor *s = this->getActionActor();
    if (s != NULL) {
        s->addMood(4, a);
    }
}

void VillagerTalk::onWindowClose() {
    if (closeFn != 0) {
        (this->*closeFn)();
        closeFn = nullptr;
    }
}

extern "C" s32 Talk_FindFlaggedPocketItem(void *p, u16 *q) {
    u16 *e = ((PlayerInventory *)(p))->getPocket(0);
    s32 i;
    BOOL v;
    for (i = 0; i < 15; i++) {
        if (((PlayerInventory *)(p))->getPocketFlags(i) == 2) {
            if (Item_IsFurniture(e) != 0) {
                v = (((s32)Item_GetFurnitureIndex(e)) == ((s32)Item_GetFurnitureIndex(q))) ? TRUE : FALSE;
            } else {
                v = (e[0] == q[0]) ? TRUE : FALSE;
            }
            if (v) {
                return i;
            }
        }
        e++;
    }
    return -1;
}

extern "C" LetterView *Talk_FindLetterState7or8(void *p) {
    u8 *e = (u8 *)((PlayerInventory *)(p))->getLetter(0);
    LetterView *r = NULL;
    s32 i;
    for (i = 0; i < 10; i++) {
        if ((u8)(((LetterView *)(e))->getState() + 0xf9) <= 1) {
            r = (LetterView *)e;
            break;
        }
        e += 0xf4;
    }
    return r;
}

void VillagerTalk::playOwnerIdleAnim() {
    (&actor->actionCtrl)->requestStand(2, (u16)data_020c6cc8[0]);
}

extern "C" s32 Talk_PickWeightedIndex(u8 *p, s32 n) {
    s32 result = -1;
    u8 sum = 0;
    s32 i;
    u8 r;
    for (i = 0; i < n; i++) {
        sum = sum + p[i];
    }
    if (sum != 0) {
        r = Random_GlobalBelow(sum);
        sum = 0;
        for (i = 0; i < n; i++) {
            sum = sum + p[i];
            if (r < sum) {
                result = i;
                break;
            }
        }
    }
    return result;
}

extern "C" void Talk_PickRandomTradeItem(u16 *out) {
    ItemPickSpec o1;
    o1.set(sTalkTradeItemLists[Random_GlobalBelow(4)], 0);
    ItemPickSpec o2 = o1;
    ItemPick_One(out, &o2, NULL, NULL, 1, 1, NULL);
}

extern "C" void Talk_PickItemFromSpecs(u16 *out, const u32 *tbl, s32 idx, PlayerData *player) {
    u16 h[2];
    s32 len;
    s32 r;
    r = Random_GlobalBelow(idx);
    *out = 0xfff1;
    tbl = tbl + r * 2;
    len = tbl[1];
    ItemPickSpec o1;
    o1.set(tbl[0], tbl[1]);
    {
        ItemPickSpec o2 = o1;
        ItemPick_One(&h[0], &o2, (u8 *)player, NULL, 1, 1, &len);  // ItemPick_One's u8 * is the player (its catalog)
        *out = h[0];
    }
    if (*out == 0xfff1) {
        ItemPickSpec o3;
        o3.set(tbl[0], len);
        ItemPickSpec o4 = o3;
        ItemPick_OneSimple(&h[1], &o4);
        *out = h[1];
    }
}

extern "C" void Talk_PickErrandItem(u16 *out, PlayerData *player) {
    Talk_PickItemFromSpecs(out, sTalkErrandItemSpecs, 3, player);
}

extern "C" void Talk_PickClothingItem(u16 *out, PlayerData *player) {
    Talk_PickItemFromSpecs(out, sTalkClothingItemSpecs, 4, player);
}

extern "C" void InsectPick_GetMissingHabitats(s32 *out, s32 *cnt) {
    TownBlockMap *g;
    u8 flags;
    s32 j, i;
    u16 *v;
    g = TownBlockMap_Get();
    flags = 0;
    *cnt = 0;
    if (g != NULL) {
        Vec2 sz;
        Unk_0202cb34_GetSize(g, &sz);
        Vec2 pos;
        for (pos.y = 1; pos.y < sz.y - 1; pos.y++) {
            for (pos.x = 1; pos.x < sz.x - 1; pos.x++) {
                MapBlock *cell = Unk_0202cb34_Cell(g, pos.x, pos.y);
                if (cell != NULL) {
                    v = MapBlock_GetItemPtr(cell, 0, 0, 0);
                    if (v != NULL) {
                        for (i = 0; i < 16; i++) {
                            for (j = 0; j < 16; j++) {
                                if (Unk_0202cb34_R(v, 0xc8, 0xcf) == 1) {
                                    if ((flags & 2) != 0) {
                                        flags |= 2;
                                    }
                                } else if (Unk_0202cb34_Check(v) == 1) {
                                    if ((flags & 4) != 0) {
                                        flags |= 4;
                                    }
                                }
                                if (flags == 6) {
                                    break;
                                }
                                v++;
                            }
                            if (flags == 6) {
                                break;
                            }
                        }
                    }
                }
                if (flags == 6) {
                    break;
                }
            }
            if (flags == 6) {
                break;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (((flags >> i) & 1) == 0) {
            *out = sInsectHabitatKinds[i];
            (*cnt)++;
            out++;
        }
    }
}

extern "C" s32 Talk_ArrayContains(s32 v, s32 *arr, s32 n) {
    s32 i;
    if (arr != NULL) {
        for (i = 0; i < n; i++) {
            if (*arr == v) {
                return 1;
            }
            arr++;
        }
    }
    return 0;
}

extern "C" s32 InsectPick_CountInSlot(s32 c, u8 *p, s32 n, s32 *arr, s32 cnt) {
    s32 result = 0;
    s32 prev;
    s32 i;
    s32 d;
    if (p != NULL && n > 0) {
        prev = 0;
        for (i = 0; i < n; i++) {
            d = p[1] - prev;
            if (d > 0) {
                if (c == InsectSpawn_GetWeightClass((u8)d)) {
                    if (Talk_ArrayContains(Insect_GetHabitat(p[0]), arr, cnt) == 0) {
                        result++;
                    }
                }
            } else {
                result = 0;
                break;
            }
            prev = p[1];
            p += 2;
        }
    }
    return result;
}

extern "C" s32 InsectPick_PickInSlot(u16 *a, s32 *b, s32 c, u8 *p, s32 n, s32 *arr, s32 cnt) {
    s32 result = 0;
    s32 i;
    s32 prev;
    s32 k;
    s32 d;
    s32 q;
    u16 t;
    if (n > 0 && p != NULL) {
        i = InsectPick_CountInSlot(c, p, n, arr, cnt);
        if (i > 0) {
            k = Random_GlobalBelow(i);
            prev = 0;
            for (i = 0; i < n; i++) {
                d = p[1] - prev;
                if (d > 0) {
                    if (c == InsectSpawn_GetWeightClass((u8)d)) {
                        q = Insect_GetHabitat(p[0]);
                        if (Talk_ArrayContains(q, arr, cnt) == 0) {
                            if (k == 0) {
                                if (p[0] < 0x38) {
                                    t = p[0] + 0x12b0;
                                } else {
                                    t = 0x12b0;
                                }
                                *a = t;
                                *b = q;
                                result = 1;
                                break;
                            } else {
                                k--;
                            }
                        }
                    }
                }
                prev = p[1];
                p += 2;
            }
        }
    }
    return result;
}

extern "C" s32 InsectPick_PickForHours(u16 *a, s32 *b, s32 *c, s32 d, Unk_0202c92c_Ent *tbl, s32 *arr, s32 cnt, u8 lo, u8 hi) {
    s32 n = 0;
    s32 result = 0;
    s32 prev;
    s32 r;
    s32 i;
    s32 k;
    s32 end;
    u8 *flag;
    if (tbl != NULL) {
        prev = 6;
        MI_CpuFill8(sInsectSlotFlags, 0, prev);
        i = lo;
        end = hi;
        for (; i <= end; i++) {
            r = Insect_HourToTimeSlot((u8)i);
            flag = &sInsectSlotFlags[r];
            if (*flag == 0 && r != prev) {
                if (InsectPick_CountInSlot(d, tbl[r].entries, tbl[r].count, arr, cnt) > 0) {
                    *flag = 1;
                    n++;
                }
                prev = r;
            }
        }
        if (n > 0) {
            k = Random_GlobalBelow(n);
            for (n = 0; n < 6; n++) {
                if (sInsectSlotFlags[n] != 0) {
                    if (k == 0) {
                        result = InsectPick_PickInSlot(a, b, d, tbl->entries, tbl->count, arr, cnt);
                        *c = n;
                        break;
                    } else {
                        k--;
                    }
                }
                tbl++;
            }
        }
    }
    return result;
}

extern "C" s32 InsectPick_PickAnyHour(u16 *a, s32 *b, s32 *c, s32 d, Unk_0202c92c_Ent *tbl, s32 *arr, s32 cnt) {
    return InsectPick_PickForHours(a, b, c, d, tbl, arr, cnt, 0, 0x17);
}

extern "C" s32 InsectPick_PickForMonthAnyHour(u16 *a, s32 lo, s32 hi, u8 kind) {
    return InsectPick_PickForMonth(a, lo, hi, 0, 0x17, kind);
}

extern "C" s32 InsectPick_PickForMonth(u16 *a, s32 lo, s32 hi, s32 d, s32 e, u8 kind) {
    Unk_0202c92c_Ent *tbl = (Unk_0202c92c_Ent *)Insect_GetSpawnTable(kind - 1);
    s32 out0, out1;
    u8 mask;
    s32 n;
    s32 i;
    s32 z;
    if (tbl != NULL) {
        mask = 0;
        n = hi - lo + 1;
        out0 = 0;
        out1 = 0;
        for (; lo <= hi; lo++) {
            mask = mask | (1 << lo);
        }
        z = 0;
        for (; n > 0; n--) {
            s32 k = Random_GlobalBelow(n);
            for (i = z; i < 5; i++) {
                if (((mask >> i) & 1) != 0) {
                    if (k == 0) {
                        if (InsectPick_PickForHours(a, &out0, &out1, i, tbl, NULL, z, d, e) != 0) {
                            return 1;
                        }
                        mask = mask & ~(1 << i);
                        break;
                    } else {
                        k--;
                    }
                }
            }
        }
    }
    return 0;
}

extern "C" s32 InsectPick_PickWeightedRarity(u16 *a, s32 *idxOut, s32 *b, s32 *c, ClockDateTime *now) {
    s32 result = 0;
    Unk_0202c92c_Ent *tbl = (Unk_0202c92c_Ent *)Insect_GetSpawnTable(now->month - 1);
    s32 arr[4];
    s32 cnt;
    s32 i;
    s32 idx;
    if (tbl != NULL) {
        cnt = 0;
        MI_CpuCopy8(sInsectRarityWeights, sInsectRarityWork, 5);
        InsectPick_GetMissingHabitats(arr, &cnt);
        for (i = 0; i < 5; i++) {
            idx = Talk_PickWeightedIndex(sInsectRarityWork, 5);
            if (idx < 0 || idx >= 5) {
                break;
            }
            if (InsectPick_PickAnyHour(a, b, c, idx, tbl, arr, cnt) == 1) {
                *idxOut = idx;
                result = 1;
                break;
            }
            sInsectRarityWork[idx] = 0;
        }
    }
    return result;
}

extern "C" void InsectPick_PickNow(u16 *p) {
    ClockDateTimeWords s;
    u8 buf[5];
    s32 out;
    s32 v;
    u32 loc[4];
    s32 t;
    Unk_0202c654_Row *row;
    s32 n;
    Unk_0202c654_Row *tbl;
    s.words[0] = 0;
    s.words[1] = 0;
    n = 5;
    out = 0;
    Clock_GetDateTime(&s.dt);
    t = Insect_HourToTimeSlot(((u8 *)&s)[2]);
    tbl = (Unk_0202c654_Row *)Insect_GetSpawnTable(((u8 *)&s)[4] - 1);
    if (tbl != NULL && t >= 0 && t < 6) {
        v = 0;
        row = tbl + t;
        MI_CpuFill8(buf, 0, n);
        InsectPick_GetMissingHabitats((s32 *)loc, &v);
        while (n > 0) {
            s32 r = Random_GlobalBelow(n);
            s32 j;
            for (j = 0; j < 5; j++) {
                if (buf[j] == 0) {
                    if (r == 0) {
                        InsectPick_PickInSlot(p, &out, j, (u8 *)row->entries, row->count, (s32 *)loc, v);
                        buf[j] = 1;
                        break;
                    }
                    r--;
                }
            }
            if (Unk_0202be64_InRange(p, 0x12b0, 0x12e7)) {
                break;
            }
            n--;
        }
    }
}

extern "C" s32 InsectPick_IsAvailable(u16 *p, s32 lo, s32 hi, s32 id) {
    u8 *e;
    u8 n;
    s32 last;
    Unk_0202c654_Row *tbl;
    s32 d;
    u32 idx;
    s32 t;
    s32 cur;
    s32 k;
    s32 prev;

    if (Unk_0202be64_InRange(p, 0x12b0, 0x12e7)) {
        tbl = (Unk_0202c654_Row *)Insect_GetSpawnTable(id - 1);
        if (tbl != NULL) {
            prev = 6;
            idx = (u8)(Unk_0202be64_InRange(p, 0x12b0, 0x12e7) ? *p - 0x12b0 : -1);
            for (; lo <= hi; lo++) {
                t = Insect_HourToTimeSlot((u8)lo);
                if (t != prev) {
                    e = tbl[t].entries;
                    if (e != NULL) {
                        last = 0;
                        k = 0;
                        n = tbl[0].count;
                        for (; k < n; k++) {
                            cur = e[1];
                            d = cur - last;
                            if (idx == e[0] && d > 0) {
                                return 1;
                            }
                            last = cur;
                            e += 2;
                        }
                    }
                    prev = t;
                }
            }
        }
    }
    return 0;
}

extern "C" s32 FishPick_CountInRow(s32 key, FishSpawnRow *p) {
    FishSpawnEntry *en = p->entries;
    s32 cnt = 0;
    if (en != NULL && p->count != 0) {
        s32 i;
        for (i = 0; i < p->count; en++, i++) {
            if (en->fish < 0x38 && en->weight != 0 && key == FishSpawn_GetWeightClass(en->weight)) {
                cnt++;
            }
        }
    }
    return cnt;
}

extern "C" s32 FishPick_PickInRow(u16 *a, s32 *b, s32 key, FishSpawnRow *row) {
    FishSpawnEntry *en = row->entries;
    s32 result = 0;
    s32 n;
    if (en != NULL && row->count != 0 && (n = FishPick_CountInRow(key, row)) > 0) {
        s32 r = Random_GlobalBelow(n);
        s32 i;
        for (i = 0; i < row->count; en++, i++) {
            if (en->weight != 0 && en->fish < 0x38 && key == FishSpawn_GetWeightClass(en->weight)) {
                if (r == 0) {
                    u16 v;
                    if (en->fish < 0x38) {
                        v = en->fish + 0x12e8;
                    } else {
                        v = 0x12e8;
                    }
                    *a = v;
                    *b = en->unk_01;
                    result = 1;
                    break;
                }
                r--;
            }
        }
    }
    return result;
}

extern "C" s32 FishPick_CountInSlot(s32 key, FishSpawnRow **p) {
    FishSpawnRow *r6 = *p;
    s32 sum = 0;
    if (r6 != NULL) {
        s32 i;
        for (i = 0; i < 2; i++) {
            sum += FishPick_CountInRow(key, r6 + i);
        }
    }
    return sum;
}

extern "C" s32 FishPick_PickInSlot(u16 *a, s32 *b, s32 key, FishSpawnRow **p) {
    FishSpawnRow *r5 = *p;
    s32 result = 0;
    if (r5 != NULL) {
        s32 cnt = 0;
        s32 i;
        MI_CpuFill8(sFishRowFlags, result, 2);
        for (i = 0; i < 2; i++) {
            if (FishPick_CountInRow(key, r5 + i) > 0) {
                sFishRowFlags[i] = 1;
                cnt++;
            }
        }
        if (cnt > 0) {
            s32 r = Random_GlobalBelow(cnt);
            for (i = 0; i < 2; r5++, i++) {
                if (sFishRowFlags[i] == 1) {
                    if (r == 0) {
                        result = FishPick_PickInRow(a, b, key, r5);
                        break;
                    }
                    r--;
                }
            }
        }
    }
    return result;
}

extern "C" s32 FishPick_PickForHours(u16 *a, s32 *b, s32 *c, s32 key, FishSpawnTable *g, u8 lo0, u8 hi0) {
    FishSpawnRow **r5 = g->slots;
    s32 result = 0;
    if (r5 != NULL) {
        s32 prev = 3;
        s32 cnt = 0;
        s32 i, t;
        MI_CpuFill8(sFishSlotFlags, cnt, prev);
        s32 lo = lo0;
        s32 hi = hi0;
        for (; lo <= hi; lo++) {
            t = FishTable_GetHourSlot((u8)lo);
            u8 *fl = sFishSlotFlags + t;
            if (*fl == 0 && t != prev) {
                if (FishPick_CountInSlot(key, r5 + t) > 0) {
                    *fl = 1;
                    cnt++;
                }
                prev = t;
            }
        }
        if (cnt > 0) {
            s32 r = Random_GlobalBelow(cnt);
            for (i = 0; i < 3; r5++, i++) {
                if (sFishSlotFlags[i] == 1) {
                    if (r == 0) {
                        result = FishPick_PickInSlot(a, b, key, r5);
                        *c = i;
                        break;
                    }
                    r--;
                }
            }
        }
    }
    return result;
}

extern "C" s32 FishPick_PickAnyHour(u16 *a, s32 *b, s32 *c, s32 d, FishSpawnTable *e) {
    return FishPick_PickForHours(a, b, c, d, e, 0, 0x17);
}

extern "C" s32 FishPick_PickForDate(u16 *a, s32 lo, s32 hi, s32 d, u8 e, u8 x, u8 y) {
    FishSpawnTable *tbl = (FishSpawnTable *)FishTable_GetForDate(d, FishTable_IsLateMonth(e));
    if (tbl != NULL) {
        u8 mask = 0;
        s32 n = hi - lo + 1;
        s32 c1 = 0;
        s32 c2 = 0;
        for (; lo <= hi; lo++) {
            mask = mask | (1 << lo);
        }
        for (; n > 0; n--) {
            s32 r = Random_GlobalBelow(n);
            s32 j;
            for (j = 0; j < 5; j++) {
                if ((mask >> j) & 1) {
                    if (r == 0) {
                        if (FishPick_PickForHours(a, &c1, &c2, j, tbl, x, y)) {
                            return 1;
                        }
                        mask &= ~(1 << j);
                        break;
                    }
                    r--;
                }
            }
        }
    }
    return 0;
}

extern "C" s32 FishPick_PickForDateAnyHour(u16 *a, s32 b, s32 c, s32 d, u8 e) {
    return FishPick_PickForDate(a, b, c, d, e, 0, 0x17);
}

extern "C" s32 FishPick_PickWeightedRarity(u16 *a, s32 *outb, s32 *c, s32 *d, FishSpawnTable *e) {
    s32 result = 0;
    s32 i;
    MI_CpuCopy8(sFishRarityWeights, sFishRarityWork, 5);
    for (i = 0; i < 5; i++) {
        s32 r4 = Talk_PickWeightedIndex(sFishRarityWork, 5);
        if (r4 < 0 || r4 >= 5) {
            break;
        }
        if (FishPick_PickAnyHour(a, c, d, r4, e) == 1) {
            *outb = r4;
            result = 1;
            break;
        }
        sFishRarityWork[r4] = 0;
    }
    return result;
}

extern "C" void FishPick_PickNow(u16 *p) {
    ClockDateTimeWords s;
    u8 buf[5];
    s32 out;
    s32 n;
    s.words[0] = 0;
    s.words[1] = 0;
    n = 5;
    Clock_GetDateTime(&s.dt);
    u32 b4 = ((u8 *)&s)[4];
    FishSpawnTable *tbl = (FishSpawnTable *)FishTable_GetForDate(b4, FishTable_IsLateMonth(((u8 *)&s)[3]));
    if (tbl != NULL && tbl->slots != NULL) {
        s32 t = FishTable_GetHourSlot(((u8 *)&s)[2]);
        if ((u32)t < 3) {
            FishSpawnRow **row = tbl->slots + t;
            MI_CpuFill8(buf, 0, n);
            while (n > 0) {
                s32 r = Random_GlobalBelow(n);
                s32 j;
                for (j = 0; j < 5; j++) {
                    if (buf[j] == 0) {
                        if (r == 0) {
                            FishPick_PickInSlot(p, &out, j, row);
                            buf[j] = 1;
                            break;
                        }
                        r--;
                    }
                }
                if (Unk_0202be64_InRange(p, 0x12e8, 0x131f)) {
                    break;
                }
                n--;
            }
        }
    }
}

extern "C" s32 FishPick_IsAvailable(u16 *p, s32 lo, s32 hi, s32 id, u8 e) {
    if (Unk_0202be64_InRange(p, 0x12e8, 0x131f)) {
        FishSpawnTable *tbl = (FishSpawnTable *)FishTable_GetForDate(id, FishTable_IsLateMonth(e));
        if (tbl != NULL && tbl->slots != NULL) {
            s32 prev = 3;
            u32 idx = (u8)(Unk_0202be64_InRange(p, 0x12e8, 0x131f) ? *p - 0x12e8 : -1);
            for (; lo <= hi; lo++) {
                s32 t = FishTable_GetHourSlot((u8)lo);
                if (t != prev) {
                    FishSpawnRow *row = tbl->slots[t];
                    if (row != NULL) {
                        FishSpawnEntry *en;
                        s32 j;
                        for (j = 0; j < 2; row++, j++) {
                            en = row->entries;
                            if (en != NULL && row->count != 0) {
                                s32 k;
                                for (k = 0; k < row->count; en++, k++) {
                                    if (en->fish < 0x38 && en->fish == idx && en->weight != 0) {
                                        return 1;
                                    }
                                }
                            }
                        }
                    }
                    prev = t;
                }
            }
        }
    }
    return 0;
}

extern "C" s32 InsectPick_GetHintVariant(s32 v) {
    switch (v) {
    case 12:
        return 0;
    case 1:
    case 2:
    case 4:
        return 1;
    }
    return 2;
}

extern "C" s32 Talk_FindInS8Array(s32 key, const s8 *arr, s32 n) {
    s32 i;
    for (i = 0; i < n; arr++, i++) {
        if (*arr == key) {
            return key;
        }
    }
    return -1;
}

extern "C" s32 Talk_FindNearbyInsectOfKinds(VecFx32Ctor *a, const s8 *b, s32 c, VecFx32 *d, s32 e) {
    if (Unk_0202c094_IsZero(gFieldSceneKind)) {
        s32 i;
        for (i = 0; i < 8; i++) {
            s32 t = Talk_FindInS8Array(Shared_FieldInsect_GetPosAndKind(a, (u8)i), b, c);
            if (t != -1) {
                if (Vec_DistXZ(d, a) < e) {
                    return t;
                }
            }
        }
    }
    return -1;
}

extern "C" s32 Talk_IsCatchPlanDue(VillagerActor *a, PlanErrand *b, ClockDateTimeWords *y, u32 flag) {
    void *s = PlanErrand_GetRecord(b);
    s32 r4 = PlanErrand_GetStep(b);
    s32 r6;
    if (((ErrandRecord *)(s))->isActive() != 0 && ((ErrandRecord *)(s))->getStep() == 0 && ((ErrandRecord *)(s))->getClass() == 0 && r4 != 0
        && ((PlayerId *)(PlanErrand_GetPlayer(b)))->isValid() != 0) {
        r6 = ((ErrandRecord *)(s))->getKind();
        if (r4 < PlanErrand_GetStepCount(r6)) {
            if (r6 == 0) {
                if (!Unk_0202be64_InRange(PlanErrand_GetShownItem(b), 0x12b0, 0x12e7)) {
                    goto ok;
                }
            }
            if (r6 != 1) {
                goto fail;
            }
            if (Unk_0202be64_InRange(PlanErrand_GetShownItem(b), 0x12e8, 0x131f)) {
                goto fail;
            }
        ok:
            ClockDateTimeWords *rec = ((ClockDateTimeWords *)PlanErrand_GetTime(b));
            if (flag == 0) {
                return 1;
            }
            if (*(long long *)rec == 0) {
                goto fail;
            }
            s32 r0 = DateTime_Compare(&y->dt, &rec->dt, 0x3f);
            if (r0 == 1) {
                s32 t;
                if (r4 < 5) {
                    t = sCatchPlanStageMinutes[r4];
                } else {
                    t = 0;
                }
                if (t > 0) {
                    if (DateTime_DiffMinutes(&rec->dt, &y->dt) >= t) {
                        return 1;
                    }
                }
            } else if (r0 == -1) {
                return 1;
            }
        }
    }
fail:
    return 0;
}

extern "C" void Talk_UpdateInsectCatchPlan(VillagerActor *a, PlanErrand *b, u8 *arr, void *c, u8 flag, ClockDateTimeWords *rec) {
    void *s = PlanErrand_GetRecord(b);
    if (((ErrandRecord *)(s))->getKind() == 0) {
        s32 r6 = PlanErrand_GetStep(b);
        if (Talk_IsCatchPlanDue(a, b, rec, flag)) {
            s32 r7;
            if (r6 < 5) {
                r7 = arr[r6];
            } else {
                r7 = 0;
            }
            s32 rnd = Random_GlobalBelow(100);
            u16 val = *(((ErrandRecord *)(s))->getItem());
            switch (r6) {
            case 0:
                break;
            case 1:
            case 3:
            case 4:
                if (rnd < r7) {
                    if (InsectPick_IsAvailable(&val, rec->dt.hour, rec->dt.hour, rec->dt.month)) {
                        *PlanErrand_GetShownItem(b) = val;
                    }
                }
                break;
            case 2:
                if (rnd < r7) {
                    s32 k = Talk_PickWeightedIndex((u8 *)c, 4);
                    if (InsectPick_PickForMonthAnyHour(&val, k, k + 1, rec->dt.month)) {
                        *PlanErrand_GetShownItem(b) = val;
                    }
                }
                break;
            }
            if (a->getVillagerData() != NULL) {
                if (Unk_0202be64_InRange(PlanErrand_GetShownItem(b), 0x12b0, 0x12e7)) {
                    VillagerSync_Act3F(a->getVillagerData(), PlanErrand_GetShownItem(b));
                }
            }
            u32 x = rec->words[0];
            u32 y = rec->words[1];
            ClockDateTimeWords *d = ((ClockDateTimeWords *)PlanErrand_GetTime(b));
            d->words[0] = x;
            d->words[1] = y;
        }
    }
}

void VillagerActor::updateFishCatchPlan(PlanErrand *s1, u8 *tbl, void *p2, u8 p3, ClockDateTimeWords *arr) {
    ClockDateTime *bytes = &arr->dt;
    void *v;
    s32 t;
    u32 lim;
    s32 roll;
    u16 val;
    v = PlanErrand_GetRecord(s1);
    if (((ErrandRecord *)(v))->getKind() == 1) {
        t = PlanErrand_GetStep(s1);
        if (Talk_IsCatchPlanDue(this, s1, arr, p3) != 0) {
            if (t < 5) {
                lim = tbl[t];
            } else {
                lim = 0;
            }
            roll = Random_GlobalBelow(100);
            val = *(((ErrandRecord *)(v))->getItem());
            switch (t) {
            case 0:
                break;
            case 1:
            case 3:
            case 4:
                if (roll < (s32)lim) {
                    if (FishPick_IsAvailable(&val, bytes->hour, bytes->hour, bytes->month, bytes->day) != 0) {
                        *PlanErrand_GetShownItem(s1) = val;
                    }
                }
                break;
            case 2:
                if (roll < (s32)lim) {
                    u32 r = ((u32)Talk_PickWeightedIndex((u8 *)p2, 4));
                    if (FishPick_PickForDateAnyHour(&val, r, r + 1, bytes->month, bytes->day) != 0) {
                        *PlanErrand_GetShownItem(s1) = val;
                    }
                }
                break;
            }
            if (getVillagerData()) {
                if (InRange(PlanErrand_GetShownItem(s1), 0x12e8, 0x131f)) {
                    VillagerSync_Act3F(getVillagerData(), PlanErrand_GetShownItem(s1));
                }
            }
            u32 w0 = arr->words[0];
            u32 w1 = arr->words[1];
            u32 *d = (u32 *)PlanErrand_GetTime(s1);
            d[0] = w0;
            d[1] = w1;
        }
    }
}

void VillagerActor::updateCatchPlans(u8 *a, void *b, u32 c) {
    if (getVillagerData()) {
        PlanErrand *s = (VillagerPlanBlock_GetErrand(Villager_GetPlan(getVillagerData())));
        ClockDateTimeWords arr;
        arr.words[0] = 0;
        arr.words[1] = 0;
        Clock_GetDateTime(&arr.dt);
        Talk_UpdateInsectCatchPlan(this, s, a, b, c, &arr);
        updateFishCatchPlan(s, a, b, c, &arr);
    }
}

s32 VillagerTalkTopics::getGreetingStatus(s32 *out) {
    TownId *idb = (TownId *)gSaveTownId;
    BOOL f5;
    ClockDateTime *h;
    void *sc;
    u16 *b;
    u16 *a;
    ClockDateTimeWords arr;
    VillagerStateView *rec;
    if (PlayerData_GetCurrent()) {
        void *t0 = PlayerData_GetCurrent()->getPlayerId();
        rec = (VillagerStateView *)t0;
    } else {
        rec = NULL;
    }
    arr.words[0] = 0;
    arr.words[1] = 0;
    if (unk_128_p == NULL) {
        return 0;
    }
    if (rec != NULL && ((PlayerId *)(rec))->isValid() != 0 && ((TownId *)(rec))->getTownRelation() != 0) {
        a = (u16 *)PlayerId_GetTownId(rec);
        b = (u16 *)PlayerId_GetTownId(VillagerMemory_GetPlayerId(unk_128_p));
        if (b[0] != a[0] || memcmp(b + 1, a + 1, 8) != 0) {
            return 6;
        }
    }
    Clock_GetDateTime(&arr.dt);
    h = (ClockDateTime *)VillagerMemory_GetTalkDate(unk_128_p);  // (VillagerMemory: the talk date)
    if (idb != NULL) {
        b = ((u16 *)VillagerMemory_GetTownId(unk_128_p));
        if (idb->townId != b[0] || memcmp(idb->townName, b + 1, 8) != 0) {
            return 5;
        }
    }
    if (DateTime_Compare(h, &arr.dt, 0x3f) == 1) {
        return 1;
    }
    sc = actor->getVillagerData();
    f5 = FALSE;
    if (sc != NULL && (((VillagerData *)(sc))->getVillagerId())->isValid() != 0 && Villager_GetState(sc) != NULL &&
        ((VillagerStateView *)Villager_GetState(sc))->bits.f2 != 0) {
        f5 = TRUE;
    }
    *out = DateTime_DiffDays(h, &arr.dt);
    if (((TownId *)(rec))->getTownRelation() != 0) {
        if (*out >= 1 && !f5) {
            return 1;
        }
        return 2;
    }
    if (*out >= 60) {
        return 4;
    }
    if (*out >= 14) {
        return 3;
    }
    if (*out >= 1 && !f5) {
        return 1;
    }
    return 2;
}

BOOL VillagerTalkTopics::hasFallen() {
    return FALSE;
}

BOOL VillagerTalkTopics::isBeeSwarmOut() {
    BOOL f = IsZero(gFieldSceneKind);
    if (f && Insect_IsBeeSwarmOut() != 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::isFirstMeeting(s32 x) {
    if (x == 0) {
        return TRUE;
    }
    return FALSE;
}

const TalkTopic *VillagerTalkTopics::getFirstMeetingTopic(u32 *out, void *scene) {
    void *p = PlayerData_GetCurrent();
    s32 t = Villager_GetMoveInKind(scene);
    if (t == 1) {
        if ((((PlayerData *)(p))->getPlayerId())->getTownRelation() != 0) {
            t = 0;
        }
    }
    if (t == 2) {
        ((ActorTalkRequest *)this)->setTownNameSlot(((VillagerDataItemView *)(scene))->getMovedFromTownId(), 1);
    } else {
        *out = Clock_GetTimeOfDay();
    }
    return &sTalkTopicsAiFirst[t];
}

BOOL VillagerTalkTopics::isForeignMemory(s32 x) {
    if (x == 5) {
        return TRUE;
    }
    return FALSE;
}

const TalkTopic *VillagerTalkTopics::getForeignTopic() {
    if (unk_128_p) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(unk_128_p), 1);
    }
    return &sTalkTopicAiForeign;
}

BOOL VillagerTalkTopics::findDangerousInsect(volatile s32 *out, VecFx32 *p) {
    VecFx32Ctor buf;
    *out = Talk_FindNearbyInsectOfKinds(&buf, sDangerousInsects, 2, p, 0x6000);
    s32 t = *out;
    BOOL r = FALSE;
    if (t != -1) {
        r = TRUE;
    }
    return r;
}

const TalkTopic *VillagerTalkTopics::getPoisonTopic(s32 x) {
    u16 v;
    u16 t;
    if (x == -1) {
        x = sDangerousInsects[Random_GlobalBelow(2)];
    }
    if (x != -1) {
        if ((u32)x < 0x38) {
            t = x + 0x12b0;
        } else {
            t = 0x12b0;
        }
        v = t;
        ((ActorTalkRequest *)this)->setItemNameSlot(&v, 0, 7);
    }
    return &sTalkTopicAiPoison;
}

BOOL VillagerTalkTopics::hasFleas() {
    return actor->consumeFleaRemoved();
}

BOOL VillagerTalkTopics::isPlayerStung(VillagerStateView *rec) {
    void *p = PlayerData_GetCurrent();
    if (rec->bits.f0 == 0 && p != NULL && PlayerData_HasStungFace(p) != 0) {
        return TRUE;
    }
    return FALSE;
}

const TalkTopic *VillagerTalkTopics::getBeeStingTopic(u32 *out, VillagerStateView *rec) {
    *out = Clock_GetTimeOfDay();
    rec->bits.f0 = 1;
    return &sTalkTopicAiBee;
}

BOOL VillagerTalkTopics::isMoodAngry(s32 x) {
    if (x == 2) {
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::isMoodSad(s32 x) {
    if (x == 3) {
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::isMoodTired(s32 x) {
    if (x == 4) {
        return TRUE;
    }
    return FALSE;
}

void VillagerTalkTopics::selectGreeting(TalkTopicMsg *out) {
    VillagerData *scene = actor->villagerData;
    VillagerStateView *rec = (VillagerStateView *)Villager_GetState(scene);
    u32 unk20;
    s32 status;
    s32 unk24;
    s32 kind;
    const TalkTopic *d;
    s32 sel;
    s32 v28;
    Unk_0202b520_Pair pair;
    s32 n;
    s32 v34;
    s32 x;
    unk20 = VillagerId_GetPersonality(scene->getVillagerId());
    n = 0;
    status = getGreetingStatus(&n);
    unk24 = VillagerState_GetMood(rec);
    kind = Weather_GetFallingPrecip();
    v34 = 0;
    PlayerData_GetCurrent();
    v28 = 0;
    sel = -1;
    pair.unk_00 = 0xfff1;
    x = sel;
    setGreetingArgs();
    if (hasFallen()) {
        d = &sTalkTopicAiFall;
        if (status == 0) {
            noMemoryUpdate = 1;
        }
    } else if (isBeeSwarmOut()) {
        d = &sTalkTopicAiRun;
        if (status == 0) {
            noMemoryUpdate = 1;
        }
    } else if (isFirstMeeting(status)) {
        d = getFirstMeetingTopic((u32 *)&v34, scene);
    } else if (status == 6) {
        d = &sTalkTopicAiMfirst;
        v34 = 7;
        v28 = 3;
    } else if (findDangerousInsect(&x, &actor->position)) {
        d = getPoisonTopic(x);
    } else if (hasFleas()) {
        d = &sTalkTopicAiFlea;
    } else if (isForeignMemory(status)) {
        d = getForeignTopic();
    } else if (status == 4) {
        d = &sTalkTopicAi30days;
        if (n >= 0x186) {
            sel = 1;
            v34 = 5;
            v28 = sel;
        }
    } else if (status == 3) {
        d = &sTalkTopicAi7days;
    } else if (isPlayerStung(rec)) {
        d = getBeeStingTopic((u32 *)&v34, rec);
    } else if (isMoodAngry(unk24)) {
        d = &sTalkTopicAiAnger;
    } else if (isMoodSad(unk24)) {
        d = &sTalkTopicAiSad;
    } else if (isMoodTired(unk24)) {
        d = &sTalkTopicAiTire;
    } else if (Villager_IsGreeter(scene) && ReddShop_IsTentOpen() && !ReddPassword_CurrentPlayerKnows()) {
        // (the decomp kept the string in a u32[0x34 / 4]: too small for the object on 64-bit hosts)
        ReddPasswordString obj;
        ((ReddShop *)(data_021ed2c0))->getPassword()->getAnswerText(&obj);
        d = &sTalkTopicAiPassword;
        ((TalkWindowState *)(window))->setSlot(4, &obj);
    } else if (gCommManager->isOnline() == 0 && unk_128_p != NULL && ((VillagerMemory *)(unk_128_p))->hasFortuneGreeting() != 0 &&
               Pocket_FindEmpty() != -1) {
        d = &sTalkTopicAiFortune;
        ItemPickSpec t;
        t.set(0, 0);
        ItemPick_One(&pair.fortuneItem, &t, NULL, NULL, 1, 1, NULL);
        itemToPlayer = pair.fortuneItem;
        itemToPlayerIsReceived = 0;
        ((ActorTalkRequest *)this)->setItemNameSlot(&itemToPlayer, 0, 7);
        if (unk_128_p != NULL) {
            ((VillagerMemory *)(unk_128_p))->clearFortuneGreeting();
        }
    } else if (Scene_InNookShop()) {
        d = &sTalkTopicAiShop1;
        if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(scene)))->getState() != 4) {
            v34 = 1;
        }
    } else if (Scene_GetCurrent() == 10) {
        d = &sTalkTopicAiShop2;
        if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(scene)))->getState() != 3) {
            v34 = 1;
        }
    } else if (Scene_InMuseumRoom() && SceneId_GetMuseumRoom(Scene_GetCurrent()) == 1) {
        d = &sTalkTopicAiShop3;
    } else if (status == 1) {
        if (Scene_InVillagerHouse()) {
            d = &sTalkTopicAiIndoor;
        } else if (kind == 1) {
            d = &sTalkTopicAiRain1;
        } else if (kind == 2) {
            d = &sTalkTopicAiSnow1;
        } else {
            d = &sTalkTopicAiToday1;
        }
        v34 = Clock_GetTimeOfDay();
    } else if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(scene)))->getState() == 8) {
        d = &sTalkTopicAiBoom;
    } else if (TalkRepeat_GetLevel(VillagerState_GetTalkRepeat(rec)) != 0) {
        d = &sTalkTopicAiPersis;
        if (TalkRepeat_GetLevel(VillagerState_GetTalkRepeat(rec)) == 2) {
            v34 = 1;
        }
    } else if (unk24 == 1) {
        d = &sTalkTopicAiJoy;
        v34 = Clock_GetTimeOfDay();
    } else {
        BOOL f = IsZero(gFieldSceneKind);
        if (f && kind == 1) {
            d = &sTalkTopicAiRain2;
        } else if (f && kind == 2) {
            d = &sTalkTopicAiSnow2;
        } else {
            d = &sTalkTopicAiToday2;
            if (Random_GlobalBelow(9) != 0) {
                v28 = (u8)(d->variantCount - 1);
            }
            v34 = Clock_GetTimeOfDay();
        }
    }
    if (d != NULL) {
        if (sel == -1) {
            sel = d->variantCount;
        }
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, unk20, d->key, (u8)sel, v34, v28);
    }
    if (n > 0) {
        s32 a, b;
        if (n >= 0x46) {
            a = 9;
        } else {
            a = n / 7;
        }
        if (n >= 0x186) {
            b = 12;
        } else {
            b = n / 30;
        }
        ((ActorTalkRequest *)(this))->setNumberSlot(a, 2, 1, 0, 0);
        ((ActorTalkRequest *)(this))->setNumberSlot(b, 3, 2, 0, 0);
    }
    rec->flags |= 4;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::setStringTableArg(u32 a, u32 b) {
    u32 v = 0;
    const char *r = TopicWord_PickRandom(&v, b);
    SingleByteString s;
    s.c[0] = v;
    s.c[1] = 0;
    ((ActorTalkRequest *)(this))->setSlotFromString(a, (u8 *)&s, r);
}

void VillagerTalkTopics::setFashionArg(u32 a, void *b) {
    u8 *p = (u8 *)Villager_GetFashionTaste(b);
    if (p) {
        SingleByteString s;
        s.c[0] = *p;
        s.c[1] = 0;
        ((ActorTalkRequest *)(this))->setSlotFromString(a, (u8 *)&s, "st_fashion");
    }
}

void VillagerTalkTopics::setGreetingArgs() {
    VillagerData *r4 = actor->villagerData;
    u8 buf[2];
    s32 i;
    if (r4 != 0) {
        buf[0] = Date_GetStarSignOf(Villager_GetBirthday(r4));
        buf[1] = 0;
        ((ActorTalkRequest *)(this))->setSlotFromString(0, buf, "st_constellation");
        this->setFashionArg(6, r4);
    }
    for (i = 0; i < 4; i++) {
        this->setStringTableArg(i + 7, data_020c7508[i]);
    }
}

void VillagerTalkTopics::ensureSpeakerMemory() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &unk_128_p, &memoryIndex, actor->villagerData, 0);
}

void VillagerTalkTopics::continueAfterGreeting() {
    void *r6 = PlayerData_GetCurrent();
    VillagerData *r7 = actor->villagerData;
    PlayerErrands *errands = static_cast<PlayerErrands *>(((PlayerData *)(r6))->getErrands());
    void *r4;
    u32 v = ((s32)Villager_GetPlayerErrandKind(gSaveVillagers, r7, r6));
    TalkTopicMsg out;
    u8 b;
    ClockDateTimeWords buf;
    if (v < 0x16) {
        buf.words[0] = 0;
        buf.words[1] = 0;
        Clock_GetDateTime(&buf.dt);
        switch (Errand_GetGroup(v)) {
        case 0:
            break;
        case 1:
            errandSlot = PlayerErrands_GetSlot(errands, Errand_GetSlotIndex(v));
            if (errandSlot != 0) {
                if ((PlayerErrandSlot_GetRecord(errandSlot))->getStep() == 0) {
                    if (DateTime_Compare(ErrandRecord_GetTime(PlayerErrandSlot_GetRecord(errandSlot)), &buf.dt, 0x3f) == -1) {
                        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[6]);
                        ((VillagerTalk *)this)->setErrandRecord(PlayerErrandSlot_GetRecord(errandSlot));
                    } else {
                        switch ((PlayerErrandSlot_GetRecord(errandSlot))->getKind()) {
                        case 10:
                            r4 = ((PlayerData *)(r6))->getInventory();
                            if (Talk_FindFlaggedPocketItem(r4, (PlayerErrandSlot_GetRecord(errandSlot))->getItem()) == -1) {
                                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[9]);
                                ((VillagerTalk *)this)->setErrandRecord(PlayerErrandSlot_GetRecord(errandSlot));
                            }
                            break;
                        case 19:
                            LetterView *letter = Talk_FindLetterState7or8(((PlayerData *)(r6))->getInventory());
                            if (letter != 0 && letter->getState() == 8) {
                                Letter_Clear(letter);
                                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[9]);
                                ((VillagerTalk *)this)->setErrandRecord(PlayerErrandSlot_GetRecord(errandSlot));
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case 3: {
            // (the decomp read an uninitialized r4 here: on the DS r4 still held the errands from getErrands)
            HouseVisitInvite *p = &errands->houseVisitInvite;
            ErrandRecord *q = &p->errand;
            ClockDateTime *s = (ClockDateTime *)&p->visitTime;  // (HouseVisitInvite::visitTime: the date as two words)
            if (q->isActive() != 0 && q->getKind() == 0x15 && q->getStep() == 0 && HouseVisitInvite_IsFrom(p, (VillagerIdCtorDtor *)(r7)->getVillagerId()) != 0 && DateTime_Compare(&buf.dt, s, 0x3f) == 1) {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[46]);
                ((VillagerTalk *)this)->setErrandRecord(q);
            }
            break;
        }
        }
    }
    if (((VillagerTalk *)this)->getErrandRecord() == NULL) {
        ((VillagerTalk *)this)->setTopicFns(sConnectTopic);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::select3pTalk(TalkTopicMsg *out) {
    TalkTopic local = data_020d7a80;
    u8 t = VillagerId_GetPersonality(actor->villagerData->getVillagerId());
    u32 r7 = 0;
    void *r6;
    s32 i;
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &unk_128_p, &memoryIndex, actor->villagerData, 0);
    if (((VillagerTalk *)actor)->getPartner()) {
        r6 = ((VillagerTalk *)actor)->getPartner()->villagerData;
        r7 = VillagerId_GetPersonality(((VillagerData *)(r6))->getVillagerId());
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &partnerMemory, &partnerMemoryIndex, r6, 0);
    }
    if (r7 < 6) {
        local.key = sTalkKeys3p[r7];
    }
    if (local.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, t, local.key, local.variantCount, 0, 0);
        out->fileName = topicFile;
        out->msgIndex = topicIndex;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    ((ActorTalkRequest *)(this))->setVillagerNameSlot(actor->villagerData->getVillagerId(), 0);
    if (((VillagerTalk *)actor)->getPartner()) {
        ((ActorTalkRequest *)(this))->setVillagerNameSlot((((VillagerTalk *)actor)->getPartner()->villagerData)->getVillagerId(), 1);
    }
    for (i = 0; i < 5; i++) {
        this->setStringTableArg(i + 2, data_020c7518[i]);
    }
}

BOOL VillagerTalkTopics::selectApHabit() {
    VillagerActor *p = actor;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(p->villagerData->getVillagerId()), sTalkTopicApHabit.key, 9, p->habitTopicKind & 1, sTalkTopicApHabit.variantCount);
    ((VillagerTalk *)this)->setTopicFns(sApSubTopics);
    return TRUE;
}

BOOL VillagerTalkTopics::selectApItem() {
    s32 r4 = 0;
    u32 r6;
    if (unk_128_p != 0) {
        r6 = 0;
        if (((VillagerMemory *)(unk_128_p))->isGiftGiven() == 0) {
            if (((VillagerMemory *)(unk_128_p))->getFriendship() >= 0x50) {
                if (Pocket_FindEmpty() != -1) {
                    ((VillagerMemory *)(unk_128_p))->setGiftGiven();
                    r4 = 1;
                } else {
                    r4 = 2;
                }
            } else {
                r4 = 3;
            }
        }
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicApItem.key, sTalkTopicApItem.variantCount, r4, 0);
        if ((actor->villagerData->getVillagerId())->isValid()) {
            r6 = VillagerId_GetSpecies(actor->villagerData->getVillagerId());
        }
        itemToPlayer = r6 < 0x9c ? 0x47d8 + r6 * 4 : 0x47d8;
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectApNickn() {
    ClockDate v;
    Clock_GetDate(&v.day);
    if (unk_128_p != 0 && ((VillagerMemory *)(unk_128_p))->getFriendship() >= 0 && SaveVillagers_IsNicknameCooldownOver(gSaveVillagers, &v)) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicApNickn.key, sTalkTopicApNickn.variantCount, 0, 0);
        ((VillagerTalk *)this)->setTopicFns(&sApSubTopics[6]);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectApMail() {
    VillagerData *r6 = actor->villagerData;
    s32 flag = 0;
    s32 i;
    u32 buf[8];
    s32 n;
    for (i = 0; i < 8; i++) {
        buf[i] = flag;
    }
    n = Villager_CollectOtherMemories(r6, buf, PlayerData_GetCurrent()->getPlayerId(), 2);
    if (n > 0) {
        unk_134 = Talk_PickRandomMemory(r6, (void **)buf, n, (BOOL (*)(void *, void *))((void *)((void (*)())Talk_IsMemoryLetterSender)));
    }
    if (unk_134 == 0) {
        flag = 1;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicApMail.key, sTalkTopicApMail.variantCount, flag, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectApPresent1() {
    u16 v[2];
    void *r4;
    if (Pocket_FindEmpty() != -1) {
        r4 = actor->villagerData;
        Villager_PickRandomReceivedItem(v, r4);
        itemToPlayer = v[0];
        if (itemToPlayer != 0xfff1) {
            itemToPlayerIsReceived = 1;
        } else {
            Talk_PickRandomTradeItem(&v[1]);
            itemToPlayer = v[1];
            itemToPlayerIsReceived = 0;
        }
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(((VillagerData *)(r4))->getVillagerId()), sTalkTopicApPresent1.key, sTalkTopicApPresent1.variantCount, 0, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectApPresent2() {
    u16 v;
    if (Pocket_FindEmpty() != -1) {
        Talk_PickRandomTradeItem(&v);
        itemToPlayer = v;
        itemToPlayerIsReceived = 0;
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicApPresent2.key, sTalkTopicApPresent2.variantCount, 0, 0);
        return TRUE;
    }
    return FALSE;
}

extern "C" void Talk_PickPocketItemForPlan(u16 *out, s32 idx) {
    PocketMatches buf;
    s32 i, n;
    *out = 0xfff1;
    if (PlanState_GetGroup(idx) == 0 && (u32)idx < 5) {
        if (Pocket_CountKind(&buf, sApPocketItemKinds[idx]) > 0) {
            n = Random_GlobalBelow(buf.count);
            for (i = 0; i < 15; i++) {
                if ((buf.pocketMask >> i) & 1) {
                    if (n == 0) {
                        *out = Pocket_GetItem(i);
                        break;
                    }
                    n--;
                }
            }
        }
    }
}

extern "C" s32 Talk_RoundBells(s32 x) {
    x = (x + 5) / 10 * 10;
    if (x < 10) {
        x = 10;
    }
    return x;
}

BOOL VillagerTalkTopics::selectApBuy() {
    VillagerData *r6 = actor->villagerData;
    u16 v;
    Talk_PickPocketItemForPlan(&v, (VillagerPlanBlock_GetPlan(Villager_GetPlan(r6)))->getState());
    itemFromPlayer = v;
    if (itemFromPlayer != 0xfff1) {
        s32 t = Random_GlobalBelow2(0xccd) + 0xccd;
        t *= Item_GetPrice(&itemFromPlayer);
        price = (t >> 14) + ((u32)FengShui_GetWestTotal()) * 3;
        price = Talk_RoundBells(price);
        if (price <= ((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getTotalBells(1)) {
            Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(r6->getVillagerId()), sTalkTopicApBuy.key, sTalkTopicApBuy.variantCount, 0, 0);
            ((ActorTalkRequest *)(this))->setNumberSlot(price, 1, 10, 1, 0);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectApTrade() {
    VillagerData *p = actor->villagerData;
    u16 buf[3];
    Talk_PickPocketItemForPlan((u16 *)(&buf[0]), (VillagerPlanBlock_GetPlan(Villager_GetPlan(p)))->getState());
    itemFromPlayer = buf[0];
    if (itemFromPlayer != 0xfff1) {
        Villager_PickRandomReceivedItem(&buf[1], p);
        itemToPlayer = buf[1];
        if (itemToPlayer != 0xfff1) {
            itemToPlayerIsReceived = 1;
        } else {
            Talk_PickRandomTradeItem((u16 *)(&buf[2]));
            itemToPlayer = buf[2];
            itemToPlayerIsReceived = 0;
        }
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(p->getVillagerId()), sTalkTopicApTrade.key, sTalkTopicApTrade.variantCount, 0, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectApSell() {
    VillagerData *p = actor->villagerData;
    u16 buf[2];
    s32 t, u;
    (VillagerPlanBlock_GetPlan(Villager_GetPlan(p)))->getState();
    if (Pocket_FindEmpty() != -1) {
        Villager_PickRandomReceivedItem(&buf[0], p);
        itemToPlayer = buf[0];
        if (itemToPlayer != 0xfff1) {
            itemToPlayerIsReceived = 1;
        } else {
            Talk_PickRandomTradeItem((u16 *)(&buf[1]));
            itemToPlayer = buf[1];
            itemToPlayerIsReceived = 0;
        }
        if (itemToPlayer != 0xfff1) {
            t = Random_GlobalBelow2(0xccd) + 0xccd;
            t = (t * Item_GetPrice(&itemToPlayer)) >> 12;
            u = FengShui_GetWestTotal() * 3;
            if (t <= u) {
                u = 0;
            }
            price = Talk_RoundBells(t - u);
            if (price > 0xbb8) {
                price = 0xbb8;
            }
            if (price <= ((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getTotalBells(1)) {
                Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(p->getVillagerId()), sTalkTopicApSell.key, sTalkTopicApSell.variantCount, 0, 0);
                ((ActorTalkRequest *)(this))->setNumberSlot(price, 2, 10, 1, 0);
                return TRUE;
            }
        }
    }
    return FALSE;
}

void VillagerTalkTopics::selectApTopic(TalkTopicMsg *out) {
    static Unk_0202a750_Fn tbl[9] = { data_020d7a08, data_020d79e8, data_020d7918, data_020d7a18, data_020d7920, data_020d7910, data_020d79f0, data_020d7be0, data_020d7d98 };
    s32 r, i;
    u8 arr[9];
    r = 0;
    MI_CpuCopy8(sApTopicWeights, arr, 9);
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, NULL);
    while (r == 0) {
        i = this->pickTsuTopicIndex((u8 *)arr, 9);
        if (i >= 0 && i < 9) {
            r = (this->*tbl[i])();
            if (r == 0) {
                arr[i] = 0;
            }
        } else {
            (this->*tbl[Random_GlobalBelow(2)])();
            break;
        }
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    this->setStringTableArg(4, 3);
    this->setStringTableArg(6, 4);
    if (itemFromPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
    }
    if (itemToPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 1, 7);
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 2, 7);
    }
    customFn0 = (Unk_020238b0_Fn)(data_020d7ee8);
    customFn1 = (Unk_020238b0_Fn)(data_020d7cb0);
    customFn2 = (Unk_020238b0_Fn)(data_020d7c98);
    customFn3 = (Unk_020238b0_Fn)(data_020d7d30);
    customFn4 = (Unk_020238b0_Fn)(data_020d7ae0);
}

extern "C" {
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ac8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkTopics::onNloseCatchPicked) data_020d7a90 = &VillagerTalkTopics::onNloseCatchPicked;
const TalkTopic sTalkTopicQ06Normal = {sTalkKeyQ06Normal, 0x3};
decltype(&VillagerTalkTopics::selectTsuFoAct) data_020d7e20 = &VillagerTalkTopics::selectTsuFoAct;
decltype(&VillagerTalkTopics::stopBirthdayBgm) data_020d7d70 = &VillagerTalkTopics::stopBirthdayBgm;
char sTalkKeyTsuSeAct[11] = "tsu_se_act";
TalkTopic sTalkTopicQ04Miss = {sTalkKeyQ04Miss, 0x3};
const TalkTopic sTalkTopicEtcHit = {sTalkKeyEtcHit, 0x5};
const u8 data_020c7508[4] = {
    0x01, 0x00, 0x03, 0x02,
};
const char *sTalkKeysQ02Con[5] = {
    sTalkKeyQ02Con1, sTalkKeyQ02Con245, sTalkKeyQ02Con3, sTalkKeyQ02Con245, sTalkKeyQ02Con245,
};
const TalkTopic sTalkTopicQ10Leave = {sTalkKeyQ10Leave, 0x3};
char sTalkKeyTsuNoAct[11] = "tsu_no_act";
char sTalkKeyAiJoy[7] = "ai_joy";
decltype(&VillagerTalkRequestReplyTopics::tryOfferNewRequest) data_020d7d08 = &VillagerTalkRequestReplyTopics::tryOfferNewRequest;
char sTalkKey3pTa[6] = "3p_ta";
const TalkTopic sTalkTopicApSell = {sTalkKeyApSell, 0x3};
char sTalkKeyAiForeign[11] = "ai_foreign";
decltype(&VillagerTalkTopics::selectTsuSeAct) data_020d7c08 = &VillagerTalkTopics::selectTsuSeAct;
decltype(&VillagerTalkTopics::checkRequestItem) data_020d7d48 = &VillagerTalkTopics::checkRequestItem;
const TalkTopic sTalkTopicAiShop1 = {sTalkKeyAiShop1, 0x4};
char sTalkKeyEvCountdown[13] = "ev_countdown";
const TalkTopic sTalkTopicAiToday2 = {sTalkKeyAiToday2, 0x4};
char sTalkKeyApBuy[7] = "ap_buy";
const TalkTopic sTalkTopicApPresent2 = {sTalkKeyApPresent2, 0x3};
decltype(&VillagerTalkHolidayTopics::startEvBirthMove) data_020d7ae8 = &VillagerTalkHolidayTopics::startEvBirthMove;
char sTalkKeyQ01Con245[13] = "q01_con2_4_5";
char sTalkKeyAiSnow1[9] = "ai_snow1";
const TalkTopic sTalkTopicEvCountdown = {sTalkKeyEvCountdown, 0x2};
const u8 sInsectRarityWeights[5] = {
    0x1a, 0x17, 0x14, 0x11, 0x0e,
};
const TalkTopic sTalkTopicQFull = {sTalkKeyQFull, 0x3};
const TalkTopic sTalkTopicQ06Payback = {sTalkKeyQ06Payback, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79f8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicEtcCancel = {sTalkKeyEtcCancel, 0x3};
VillagerTalkTopicFns sEvAcornTopicTable[9];
const TalkTopic sTalkTopicQItemB = {sTalkKeyQItem, 0x3};
decltype(&VillagerTalkTopics::gotoEvArbeitEnd) data_020d7c58 = &VillagerTalkTopics::gotoEvArbeitEnd;
u8 sFishRowFlags[2];
const TalkTopic sTalkTopicQ01Plose = {sTalkKeyQ01Plose, 0x2};
decltype(&VillagerMood::updateMood3Effects) data_020d7ad0 = &VillagerMood::updateMood3Effects;
decltype(&VillagerTalkRequestItemTopics::clearRequest) data_020d7c78 = &VillagerTalkRequestItemTopics::clearRequest;
decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromResident) data_020d7f58 = &VillagerTalkRumorTopics::pickMemoryLetterFromResident;
decltype(&VillagerTalkTopics::onArbeitItemPicked) data_020d7c70 = &VillagerTalkTopics::onArbeitItemPicked;
char sTalkKeyTsuDress[10] = "tsu_dress";
decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromResident) data_020d7f98 = &VillagerTalkRumorTopics::pickMemoryLetterFromResident;
const TalkTopic sTalkTopicEvAcorn = {sTalkKeyEvAcorn, 0x3};
decltype(&VillagerTalkTopics::onDeliveryItemPicked) data_020d7908 = &VillagerTalkTopics::onDeliveryItemPicked;
decltype(&VillagerTalkTopics::selectTsuHobbyAct) data_020d7b98 = &VillagerTalkTopics::selectTsuHobbyAct;
decltype(&VillagerTalkRequestItemTopics::finishRequestChain) data_020d7da0 = &VillagerTalkRequestItemTopics::finishRequestChain;
const TalkTopic sTalkTopicQNo = {sTalkKeyQNo, 0x3};
char sTalkKeyTsuAlways[11] = "tsu_always";
VillagerTalkTopicFns sEvAdmireTopicTable[8];
char sTalkKeyQ02Nlose[10] = "q02_nlose";
char sTalkKeyQ04Miss[9] = "q04_miss";
char sTalkKeyTsuFriend[11] = "tsu_friend";
char sTalkKeyAi30days[10] = "ai_30days";
const u32 sMoodAnimNextIds[5] = {
    0x00000001, 0x000000e6, 0x000000e8, 0x000000ea, 0x000000e8,
};
decltype(&VillagerTalkTopics::giveBackHeldItem) data_020d7b70 = &VillagerTalkTopics::giveBackHeldItem;
const char *sTalkKeysQ05Talk[7] = {
    sTalkKeyQ05Talk12, sTalkKeyQ05Talk12, sTalkKeyQ05Talk35, sTalkKeyQ05Talk35, sTalkKeyQ05Talk35, sTalkKeyQ05Talk67, sTalkKeyQ05Talk67,
};
const RequestDatePickerFn sRequestInsectPickers[5] = {
    VillagerRequest_PickInsectStep0, VillagerRequest_PickInsectStep1, NULL, VillagerRequest_PickInsectStep3,
    VillagerRequest_PickInsectStep4,
};
char sTalkKeyAiIndoor[10] = "ai_indoor";
char sTalkKeyAiToday1[10] = "ai_today1";
const TalkTopic sTalkTopicTsuInHint = {sTalkKeyTsuInHint, 0x1};
decltype(&VillagerTalk::attrPlayMemoryTune) data_020d7c48 = &VillagerTalk::attrPlayMemoryTune;
char sTalkKeyTsuSpot[9] = "tsu_spot";
decltype(&VillagerTalkTopics::selectApHabit) data_020d7a08 = &VillagerTalkTopics::selectApHabit;
const char data_020c7a5c[12] = "q05_clear";
const TalkTopic sTalkTopicTsuEvent2 = {sTalkKeyTsuEvent2, 0x1};
decltype(&VillagerMood::beginMood2Effects) data_020d7a38 = &VillagerMood::beginMood2Effects;
decltype(&VillagerTalk::attrShowLetter) data_020d7a30 = &VillagerTalk::attrShowLetter;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7cd8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicQ06Open = {sTalkKeyQ06Open, 0x3};
const TalkTopic sTalkTopicQ12Report = {sTalkKeyQ12Report, 0x3};
const u32 sAcornRewardItemKinds[3] = {
    0x00000000, 0x00000002, 0x00000001,
};
decltype(&VillagerTalkTopics::selectApPresent1) data_020d7920 = &VillagerTalkTopics::selectApPresent1;
const TalkTopic sTalkTopicAiBoom = {sTalkKeyAiBoom, 0x5};
const TalkTopic sTalkTopicQ02Plose = {sTalkKeyQ02Plose, 0x2};
char sTalkKeyTsuHappyroom[14] = "tsu_happyroom";
const RequestDatePickerFn sRequestFishPickers[5] = {
    VillagerRequest_PickFishStep0, VillagerRequest_PickFishStep1, NULL, VillagerRequest_PickFishStep3,
    VillagerRequest_PickFishStep4,
};
const TalkTopic sTalkTopicAiTire = {sTalkKeyAiTire, 0x5};
decltype(&VillagerTalkRumorTopics::pickMemoryLikedOld) data_020d7d90 = &VillagerTalkRumorTopics::pickMemoryLikedOld;
const TalkTopic sTalkTopicTsuHappyroom = {sTalkKeyTsuHappyroom, 0x1};
const TalkTopic sTalkTopicEvFirework = {sTalkKeyEvFirework, 0x2};
decltype(&VillagerTalkTopics::gotoQPay) data_020d7d78 = &VillagerTalkTopics::gotoQPay;
decltype(&VillagerTalkKaraokeTopics::endEvKaraokeMsg8) data_020d7d40 = &VillagerTalkKaraokeTopics::endEvKaraokeMsg8;
u8 sFishSlotFlags[3];
TalkTopic sTalkTopicQClearDefault = {NULL, 0x00000003};
decltype(&VillagerTalkTopics::onNicknameEntered) data_020d7cc8 = &VillagerTalkTopics::onNicknameEntered;
char sTalkKeyQ03Thanks[11] = "q03_thanks";
TalkTopic data_020d7a80 = {NULL, 0x00000003};
decltype(&VillagerTalk::attrGiveItem) data_020d7b28 = &VillagerTalk::attrGiveItem;
char sTalkKeyQ04Thanks[11] = "q04_thanks";
decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7aa0 = &VillagerTalk::playOwnerIdleAnim;
char sTalkKeyAiPersis[10] = "ai_persis";
decltype(&VillagerTalkTopics::gotoQNlose) data_020d7d10 = &VillagerTalkTopics::gotoQNlose;
const TalkTopic sTalkTopicAiFall = {sTalkKeyAiFall, 0x3};
char sTalkKeyTsuMemory[11] = "tsu_memory";
char sTalkKeyApSell[8] = "ap_sell";
const TalkTopic sTalkTopicTsuItem = {sTalkKeyTsuItem, 0x3};
char sTalkKeyTsuShome[10] = "tsu_shome";
const TalkTopic sTalkTopicQ06Good = {sTalkKeyQ06Good, 0x3};
char sTalkKeyApTrade[9] = "ap_trade";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79e0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyTsuGhome[10] = "tsu_ghome";
decltype(&VillagerTalkTopics::keepDislikedPresent) data_020d7bd8 = &VillagerTalkTopics::keepDislikedPresent;
const RequestShirtPickerFn sRequestShirtPickers[7] = {
    NULL, NULL, VillagerRequest_PickShirtOtherGroup, VillagerRequest_PickShirtOtherGroup,
    VillagerRequest_PickShirtOfGroup, VillagerRequest_PickShirtOfGroup, VillagerRequest_PickShirtOfGroupForPlayer,
};
const TalkTopic sTalkTopicAiToday1 = {sTalkKeyAiToday1, 0x3};
char sTalkKeyEvAdmire[10] = "ev_admire";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7938 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ10Req[8] = "q10_req";
const char *sTalkKeysQReturn[5] = {
    sTalkKeyQ01Return, sTalkKeyQ02Return, sTalkKeyQ03Return, NULL, sTalkKeyQ05Return,
};
const TalkTopic sTalkTopicQ07Open = {sTalkKeyQ07Open, 0x3};
const TalkTopic sTalkTopicQError3 = {sTalkKeyQError3, 0x2};
const TalkTopic sTalkTopicQ07Over = {sTalkKeyQ07Over, 0x1};
char sTalkKeyQ03Return[11] = "q03_return";
const TalkTopic sTalkTopicQ06Lost = {sTalkKeyQ06Lost, 0x3};
decltype(&VillagerTalkTopics::selectApPresent2) data_020d7910 = &VillagerTalkTopics::selectApPresent2;
char sTalkKeyApPresent1[12] = "ap_present1";
VillagerTalkTopicFns sEvCountdownTopicTable[8];
decltype(&VillagerTalkTopics::onHabitEntered) data_020d7da8 = &VillagerTalkTopics::onHabitEntered;
char sTalkKeyApPresent2[12] = "ap_present2";
decltype(&VillagerTalkTopics::selectTsuInHint) data_020d7f50 = &VillagerTalkTopics::selectTsuInHint;
char sTalkKeyQ02Comp[9] = "q02_comp";
const TalkTopic sTalkTopicEvInsect = {sTalkKeyEvInsect, 0x3};
const TalkTopic sTalkTopicQ07End = {sTalkKeyQ07End, 0x3};
const char *sTalkKeysQComp[5] = {
    sTalkKeyQ01Comp, sTalkKeyQ02Comp, sTalkKeyQ03Comp, sTalkKeyQ04Dress, sTalkKeyQ05Comp,
};
decltype(&VillagerTalkRumorTopics::pickMemoryLetterFromVillagerTown) data_020d7ce0 = &VillagerTalkRumorTopics::pickMemoryLetterFromVillagerTown;
VillagerTalkTopicFns sEtcConnectTopicTable[6];
char sTalkKeyQ06End[8] = "q06_end";
char sTalkKeyQ05Return[11] = "q05_return";
char sTalkKeyQ03Comp[9] = "q03_comp";
decltype(&VillagerTalkTopics::advanceRequestStep) data_020d7a60 = &VillagerTalkTopics::advanceRequestStep;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7a68 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkTopics::gotoQPwin2) data_020d7a70 = &VillagerTalkTopics::gotoQPwin2;
const TalkTopic sTalkTopicQ07Read = {sTalkKeyQ07Read, 0x3};
decltype(&VillagerTalk::setConstellationSlots) data_020d79a0 = &VillagerTalk::setConstellationSlots;
char sTalkKeyQ06Payback[12] = "q06_payback";
const TalkTopic sTalkTopicQ12FullB = {sTalkKeyQ12Full, 0x2};
decltype(&VillagerTalkTopics::cancelRequestChain) data_020d7a88 = &VillagerTalkTopics::cancelRequestChain;
char sTalkKeyEtcPush[9] = "etc_push";
decltype(&VillagerTalkTopics::selectApMail) data_020d79e8 = &VillagerTalkTopics::selectApMail;
char sTalkKeyQItem[7] = "q_item";
const TalkTopic sTalkTopicQ07Req = {sTalkKeyQ07Req, 0x3};
const TalkTopic sTalkTopicQ12Full = {sTalkKeyQ12Full, 0x2};
char sTalkKeyQ01Pay[8] = "q01_pay";
const TalkTopic sTalkTopicTsuSeHint = {sTalkKeyTsuSeHint, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7900 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicApTrade = {sTalkKeyApTrade, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d78f0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const u8 sFishRarityWeights[5] = {
    0x1a, 0x17, 0x14, 0x11, 0x0e,
};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7928 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ10Reserve[12] = "q10_reserve";
char sTalkKeyQ12Thanks[11] = "q12_thanks";
const TalkTopic sTalkTopicQ10Reserve = {sTalkKeyQ10Reserve, 0x3};
decltype(&VillagerTalk::swapItemWithPlayer) data_020d7c98 = &VillagerTalk::swapItemWithPlayer;
const char *sTalkKeysQ04Req[7] = {
    sTalkKeyQ04Req12, sTalkKeyQ04Req12, sTalkKeyQ04Req37, sTalkKeyQ04Req37, sTalkKeyQ04Req37, sTalkKeyQ04Req37, sTalkKeyQ04Req37,
};
char sTalkKeyQ07Over[9] = "q07_over";
VillagerTalkTopicFns sRequestTopicsB[47];
decltype(&VillagerTalkTopics::judgePresent) data_020d8018 = &VillagerTalkTopics::judgePresent;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ca0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkTopics::giveBackHeldItem) data_020d7cd0 = &VillagerTalkTopics::giveBackHeldItem;
const TalkTopic sTalkTopicQ06Req = {sTalkKeyQ06Req, 0x3};
char sTalkKeyEvFmarket1[12] = "ev_fmarket1";
const u32 sTalkTradeItemLists[4] = {
    0x00000000, 0x00000002, 0x00000003, 0x00000004,
};
const char *sVillagerClothMaterialNames[1] = {
    sVillagerClothMaterialName,
};
char sTalkKeyQ06Lost[9] = "q06_lost";
}

void VillagerTalkTopics::openHabitAcceptChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x4b, 0x4f, &sApSubTopics[1]);
    TalkChoiceTable_Set(this, (&s), 1, 0x50, 0x54, &sApSubTopics[5]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ac8);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectApHabitPart1(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicApHabit.key, 4, 1, sTalkTopicApHabit.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openHabitKeyboard(s32) {
    MI_CpuFill8(((u8 *)&sTalkInputBuffer), 0, 0x20);
    if (actor->habitTopicKind == 0) {
        ((ActorTalkRequest *)(this))->setSubSceneKind2(0x15, (u8 *)&sTalkInputBuffer, 10, 0);
    } else {
        ((ActorTalkRequest *)(this))->setSubSceneKind2(0x16, (u8 *)&sTalkInputBuffer, 16, 0);
    }
    ((ActorTalkRequest *)(this))->openSubScene(6);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7da8);
}

void VillagerTalkTopics::onHabitEntered() {
    u8 b;
    TalkTopicMsg out;
    void *p;
    if (MenuCtrl_IsResultOk()) {
        p = actor->villagerData;
        ((VillagerTalk *)this)->setTopicFns(&sApSubTopics[3]);
        if (actor->habitTopicKind == 0) {
            Villager_SetCatchphrase(p, MenuCtrl_GetText(), 10);
        } else if (memory != 0 && VillagerMemory_IsUsed((void *)memory)) {
            ((VillagerMemory *)((void *)memory))->setGreeting(MenuCtrl_GetText(), 16);
        } else {
            void *v = MenuCtrl_GetText();
            ((VillagerDataItemView *)(p))->setGreetingFor(v, 16, PlayerData_GetCurrent()->getPlayerId());
        }
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::retryHabitInput() {
    openHabitKeyboard(0);
}

void VillagerTalkTopics::selectApHabitConfirm(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApHabit.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    if (actor->habitTopicKind == 0) {
        topicIndex = 6;
    } else {
        topicIndex = 11;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openHabitConfirmChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x4a, 0x4a, &sApSubTopics[4]);
    TalkChoiceTable_Set(this, (&s), 1, 0x49, 0x49, &sApSubTopics[2]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7db0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectApHabitB(TalkTopicMsg *out) {
    VillagerActor *p = actor;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(p->villagerData->getVillagerId()), sTalkTopicApHabit.key, 5, p->habitTopicKind & 1, sTalkTopicApHabit.variantCount);
    topicIndex += 7;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    actor->habitTopicKind = (actor->habitTopicKind + 1) & 1;
}

void VillagerTalkTopics::selectApHabitDeclined(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApHabit.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    if (actor->habitTopicKind == 0) {
        topicIndex = 2;
    } else {
        topicIndex = 3;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openNicknameAcceptChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x45, 0x45, &sApSubTopics[7]);
    TalkChoiceTable_Set(this, (&s), 1, 0x46, 0x46, &sApSubTopics[12]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ad8);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectApNicknProposal(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApNickn.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    topicIndex = 2;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

extern "C" BOOL Talk_MakeRandomNickname(MsgString *a, MsgString *b, u32 c) {
    s32 base, mask, i, j, k, r;
    u8 v;
    if (c < 6) {
        base = sNicknamePatternBases[c];
    } else {
        base = 0;
    }
    mask = 0xff;
    k = 8;
    for (i = 0; i < 8; i++) {
        r = Random_GlobalBelow(k);
        for (j = 0; j < 8; j++) {
            if ((mask >> j) & 1) {
                if (r == 0) {
                    v = j + base;
                    if (String_MakeNickname(a, b, &v)) {
                        return TRUE;
                    }
                    mask = (u8)(mask & ~(1 << j));
                    break;
                }
                r--;
            }
        }
        k--;
    }
    return FALSE;
}

void VillagerTalkTopics::makeNickname() {
    ClockDate x;
    MsgString9BStorage a;
    MsgString9BStorage b;
    void *r4 = PlayerData_GetCurrent()->getPlayerId();
    new (&a) MsgString9B();
    new (&b) MsgString9B();
    ((PlayerId *)(r4))->getNameString((MsgString *)(&b));
    if (Talk_MakeRandomNickname((MsgString *)&a, (MsgString *)&b, VillagerId_GetPersonality(actor->villagerData->getVillagerId())) == 0) {
        ((MsgString *)(&a))->copy((MsgString *)(&b));
    }
    if (unk_128_p != 0) {
        Clock_GetDate(&x.day);
        ((VillagerMemory *)(unk_128_p))->setNicknameFromMsg(&a);
        SaveVillagers_SetNicknameDate(gSaveVillagers, &x);
    } else {
        Villager_SetNicknameFromMsgFor(actor->villagerData, &a, r4);
    }
    ((TalkWindowState *)(window))->setSlot(0, &a);
    ((MsgString9B *)(&b))->MsgString9B::~MsgString9B();
    ((MsgString9B *)(&a))->MsgString9B::~MsgString9B();
}

void VillagerTalkTopics::openNicknameLikeChoice() {
    u8 b;
    TalkTopicMsg out;
    TalkChoiceTable s;
    if (unk_128_p != 0 && ((VillagerMemory *)(unk_128_p))->getFriendship() >= 0x40) {
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_Set(this, (&s), 0, 0x47, 0x47, &sApSubTopics[8]);
        TalkChoiceTable_Set(this, (&s), 1, 0x48, 0x48, &sApSubTopics[11]);
        s.count = 2;
        s.cancelIndex = -1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ab8);
        ((TalkWindowState *)(window))->openChoices(1);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sApSubTopics[11]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
    }
}

void VillagerTalkTopics::selectApNicknDisliked(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApNickn.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    topicIndex = 3;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openNicknameKeyboard(s32 unused) {
    MI_CpuFill8(((u8 *)&sTalkInputBuffer), 0, 0x20);
    ((ActorTalkRequest *)(this))->setSubSceneKind2(0x11, (u8 *)&sTalkInputBuffer, 8, 0);
    ((ActorTalkRequest *)(this))->openSubScene(6);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7cc8);
}

void VillagerTalkTopics::onNicknameEntered() {
    u8 b;
    TalkTopicMsg out;
    MsgString9BStorage t;
    if (MenuCtrl_IsResultOk() != 0) {
        VillagerData *r4 = actor->villagerData;
        ((VillagerTalk *)this)->setTopicFns(&sApSubTopics[9]);
        if (unk_128_p != 0 && VillagerMemory_IsUsed(unk_128_p) != 0) {
            ((VillagerMemory *)(unk_128_p))->setNickname(((void *)MenuCtrl_GetText()), 8);
        } else {
            void *r6 = ((void *)MenuCtrl_GetText());
            unk_128_p = Villager_SetNicknameFor(r4, r6, 8, PlayerData_GetCurrent()->getPlayerId());
        }
        if (unk_128_p != 0) {
            new (&t) MsgString9B();
            ((VillagerMemory *)(unk_128_p))->getNickname(&t);
            ((TalkWindowState *)(window))->setSlot(0, &t);
            ((MsgString9B *)(&t))->MsgString9B::~MsgString9B();
        }
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectApNicknConfirm(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApNickn.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    topicIndex = 4;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openNicknameConfirmChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x4a, 0x4a, &sApSubTopics[11]);
    TalkChoiceTable_Set(this, (&s), 1, 0x49, 0x49, &sApSubTopics[10]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7f18);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::retryNicknameInput() {
    openNicknameKeyboard(0);
}

void VillagerTalkTopics::selectApNicknAccepted(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApNickn.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    topicIndex = 5;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectApNicknRefused(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicApNickn.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    topicIndex = 1;
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectEtcConnect(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEtcConnect.key, sTalkTopicEtcConnect.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

BOOL VillagerTalkTopics::tryAddHandOverChoice(void *a, void *b) {
    PlayerErrandSlot *r6;
    ErrandRecord *p;
    BOOL result;
    r6 = PlayerErrands_GetSlot((PlayerErrands *)b, 0);
    p = PlayerErrandSlot_GetRecord(r6);
    result = FALSE;
    if (p->isActive() != 0 && p->getSubGroup() == 1) {
        VillagerId *r4 = (PlayerErrandSlot_GetVillager(r6, 1));
        VillagerId *r7 = ((VillagerData *)(a))->getVillagerId();
        if (r7->townId == r4->townId && memcmp(r7->townName, r4->townName, 8) == 0 && r7->species == r4->species && PlayerErrandSlot_IsStepDone(r6) == 0) {
            s32 t = p->getKind();
            switch (t) {
            case 0xe:
            case 0x10:
            case 0x11: {
                TalkChoiceTable s;
                TalkChoiceTable_Init(this, (&s));
                errandSlot = r6;
                ((VillagerTalk *)this)->setErrandRecord(p);
                TalkChoiceTable_Set(this, (&s), 0, 0x1d, 0x1d, &sRequestTopicsB[28]);
                TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
                TalkChoiceTable_SetRange(this, (&s), 2, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
                s.count = 3;
                s.cancelIndex = s.count - 1;
                ((VillagerTalk *)this)->setupChoiceMenu(&s);
                ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7a98);
                ((TalkWindowState *)(window))->openChoices(1);
                result = TRUE;
            }
            }
        }
    }
    return result;
}

BOOL VillagerTalkTopics::tryAddDeliveryRecipientChoice(void *a, void *b) {
    struct { void *p10; ClockDateTimeWords pair; } l;
    ErrandRecord *p;
    PlayerErrandSlot *v;
    s32 r6;
    u8 cnt;
    BOOL r7;
    TalkChoiceTable s;
    v = PlayerErrandSlots_FindByVillager((PlayerErrandSlot *)b, (VillagerIdCtorDtor *)((VillagerData *)(a))->getVillagerId(), 1);
    if (v != 0) {
        p = PlayerErrandSlot_GetRecord(v);
    } else {
        p = 0;
    }
    r6 = p != 0 ? p->getSubGroup() : 4;
    r7 = FALSE;
    if (r6 == 0 || r6 == 2) {
        if (PlayerErrandSlot_IsStepDone(v) == 0) {
            cnt = 0;
            l.pair.words[0] = cnt;
            l.pair.words[1] = cnt;
            l.p10 = PlayerData_GetCurrent();
            Clock_GetDateTime(&l.pair.dt);
            TalkChoiceTable_Init(this, (&s));
            errandSlot = v;
            ((VillagerTalk *)this)->setErrandRecord(p);
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, cnt)), cnt);
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 1)), 1);
            ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), cnt, 7);
            if (DateTime_Compare(ErrandRecord_GetTime(((VillagerTalk *)this)->getErrandRecord()), &l.pair.dt, 0x3f) == -1) {
                if (r6 != 0) {
                    if (r6 == 2) {
                        TalkChoiceTable_Set(this, (&s), cnt, 0x2b, 0x2b, &sRequestTopicsA[10]);
                        cnt++;
                        r7 = TRUE;
                    }
                } else {
                    TalkChoiceTable_Set(this, (&s), cnt, 0x27, 0x27, &sRequestTopicsA[10]);
                    cnt++;
                    r7 = TRUE;
                }
            } else {
                if (r6 != 0) {
                    if (r6 == 2) {
                        TalkChoiceTable_SetRange(this, (&s), cnt, (const u8 *)data_020c74f0, &sRequestTopicsA[13]);
                        cnt++;
                        r7 = TRUE;
                    }
                } else {
                    void *inv = ((PlayerData *)(l.p10))->getInventory();
                    if (Talk_FindFlaggedPocketItem(inv, (PlayerErrandSlot_GetRecord(errandSlot))->getItem()) != -1) {
                        TalkChoiceTable_SetRange(this, (&s), cnt, (const u8 *)data_020c74f4, &sRequestTopicsA[13]);
                        cnt++;
                        r7 = TRUE;
                    }
                }
            }
            if (r7 != 0) {
                TalkChoiceTable_SetRange(this, (&s), cnt, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
                TalkChoiceTable_SetRange(this, (&s), (u8)(cnt + 1), (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
                u8 t = cnt + 2;
                s.count = t;
                s.cancelIndex = t - 1;
                ((VillagerTalk *)this)->setupChoiceMenu(&s);
                ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7a68);
                ((TalkWindowState *)(window))->openChoices(1);
            }
        }
    }
    return r7;
}

BOOL VillagerTalkTopics::tryAddDeliveryClientChoice(void *unused, void *errands, s32 b) {
    BOOL r = FALSE;
    errandSlot = PlayerErrands_GetSlot((PlayerErrands *)errands, Errand_GetSlotIndex(b));
    if (errandSlot != 0) {
        TalkChoiceTable s;
        TalkChoiceTable_Init(this, (&s));
        ((VillagerTalk *)this)->setErrandRecord(PlayerErrandSlot_GetRecord(errandSlot));
        ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, r)), r);
        ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 1)), 1);
        ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), r, 7);
        if (PlayerErrandSlot_IsStepDone(errandSlot) != 0) {
            TalkChoiceTable_Set(this, (&s), r, 0x23, 0x23, &sRequestTopicsA[22]);
        } else {
            TalkChoiceTable_Set(this, (&s), r, 0x1e, 0x1e, &sRequestTopicsA[21]);
        }
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
        TalkChoiceTable_SetRange(this, (&s), 2, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 3;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7fd0);
        ((TalkWindowState *)(window))->openChoices(1);
        r = TRUE;
    }
    return r;
}

extern "C" BOOL Talk_IsInsectItem(u16 *p) {
    return Unk_020298c8_R1(p, 0x12b0, 0x12e7);
}

extern "C" BOOL Talk_FilterPickInsect(u16 *p, s32 v) {
    BOOL r = FALSE;
    if (Unk_020298c8_R1(p, 0x12b0, 0x12e7) && v == 0) r = TRUE;
    return r;
}

extern "C" BOOL Talk_IsFishItem(u16 *p) {
    return Unk_020298c8_R1(p, 0x12e8, 0x131f);
}

extern "C" BOOL Talk_FilterPickFish(u16 *p, s32 v) {
    BOOL r = FALSE;
    if (Unk_020298c8_R1(p, 0x12e8, 0x131f) && v == 0) r = TRUE;
    return r;
}

s32 VillagerTalkTopics::openCreatureRequestMenu(BOOL (*f)(u16 *), s32 pa, s32 pb) {
    BOOL a = TRUE;
    u8 idx = 0;
    BOOL res = FALSE;
    TalkChoiceTable buf;
    void *p, *q;
    PocketMatches tmp;
    TalkChoiceTable_Init(this, (&buf));
    switch (((VillagerTalk *)this)->getErrandRecord()->getStep()) {
    case 0:
        p = PlanErrand_GetPlayer(planErrand);
        q = PlayerData_GetCurrent()->getPlayerId();
        if (*(u16 *)q == *(u16 *)p && memcmp((u8 *)q + 2, (u8 *)p + 2, 8) == 0 && ((PlayerId *)(q))->equals((PlayerId *)(p)) != 0) {
            if (((u32)PlanErrand_GetStep(planErrand)) == 2) {
                if (f((u16 *)((void *)PlanErrand_GetShownItem(planErrand))) != 0 && Pocket_CountMatching(&tmp, f) > 0) {
                    TalkChoiceTable_Set(this, &buf, idx, pa, pa, &sRequestTopicsB[8]);
                } else {
                    TalkChoiceTable_Set(this, (&buf), 0, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            } else {
                if (f(((VillagerTalk *)this)->getErrandRecord()->getItem()) != 0) {
                    if (Pocket_FindItem(((VillagerTalk *)this)->getErrandRecord()->getItem()) != -1) {
                        TalkChoiceTable_Set(this, &buf, idx, pb, pb, &sRequestTopicsB[13]);
                    } else {
                        TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                    }
                } else {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            }
            if (*((VillagerTalk *)this)->getErrandRecord()->getItem() != 0xfff1) {
                ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0, 7);
            }
            idx++;
            res = TRUE;
        }
        break;
    case 1:
        if (Random_GlobalBelow(10) & 1) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[26]);
            a = idx;
            idx++;
        }
        res = TRUE;
        break;
    case 2:
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[27]);
        idx++;
        a = res;
        res = TRUE;
        break;
    }
    if (res == TRUE) {
        if (a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
            idx++;
        }
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        idx++;
        buf.count = idx;
        buf.cancelIndex = idx - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&buf);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7f78);
        ((TalkWindowState *)(window))->openChoices(1);
    }
    return res;
}

extern "C" BOOL Talk_IsNonFossilFurniture(u16 *p) {
    BOOL r = FALSE;
    if (Item_IsFurniture(p)) {
        if (!Unk_020295d0_Range(p, 0x450c, 0x45db)) {
            r = TRUE;
        }
    }
    return r;
}

extern "C" BOOL Talk_FilterPickNonFossilFurniture(u16 *p, s32 x) {
    if (Talk_IsNonFossilFurniture(p) && x == 0) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL Talk_IsShirtItem(u16 *p) {
    if (Unk_020295d0_Range(p, 0x11a8, 0x12a7)) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL Talk_FilterPickShirt(u16 *p, s32 x) {
    if (Talk_IsShirtItem(p) && x == 0) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL Talk_IsFossilItem(u16 *p) {
    if (Unk_020295d0_Range(p, 0x450c, 0x45db)) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL Talk_FilterPickFossil(u16 *p, s32 x) {
    if (Talk_IsFossilItem(p) && x == 0) {
        return TRUE;
    }
    return FALSE;
}

s32 VillagerTalkTopics::openFurnitureRequestMenu() {
    BOOL a = TRUE;
    u8 idx = 0;
    BOOL res = FALSE;
    TalkChoiceTable buf;
    void *p, *q;
    PocketMatches tmp;
    TalkChoiceTable_Init(this, (&buf));
    switch (((VillagerTalk *)this)->getErrandRecord()->getStep()) {
    case 0:
        p = PlanErrand_GetPlayer(planErrand);
        q = PlayerData_GetCurrent()->getPlayerId();
        if (*(u16 *)q == *(u16 *)p && memcmp((u8 *)q + 2, (u8 *)p + 2, 8) == 0 && ((PlayerId *)(q))->equals((PlayerId *)(p)) != 0) {
            if (Pocket_CountMatching(&tmp, Talk_IsNonFossilFurniture) > 0) {
                TalkChoiceTable_Set(this, (&buf), idx, 0x1c, 0x1c, &sRequestTopicsB[18]);
            } else {
                TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
            }
            idx++;
            res = TRUE;
        }
        break;
    case 1:
        if (Random_GlobalBelow(10) & a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[26]);
            a = idx;
            idx++;
        }
        res = TRUE;
        break;
    case 2:
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[27]);
        idx++;
        a = res;
        res = TRUE;
        break;
    }
    if (res == TRUE) {
        if (a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
            idx++;
        }
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        idx++;
        buf.count = idx;
        buf.cancelIndex = idx - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&buf);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d79d8);
        ((TalkWindowState *)(window))->openChoices(1);
    }
    return res;
}

s32 VillagerTalkTopics::openShirtRequestMenu() {
    BOOL a = TRUE;
    u8 idx = 0;
    BOOL res = FALSE;
    TalkChoiceTable buf;
    void *p, *q;
    PocketMatches tmp;
    TalkChoiceTable_Init(this, (&buf));
    switch (((VillagerTalk *)this)->getErrandRecord()->getStep()) {
    case 0:
        p = PlanErrand_GetPlayer(planErrand);
        q = PlayerData_GetCurrent()->getPlayerId();
        if (*(u16 *)q == *(u16 *)p && memcmp((u8 *)q + 2, (u8 *)p + 2, 8) == 0 && ((PlayerId *)(q))->equals((PlayerId *)(p)) != 0) {
            if (((u32)PlanErrand_GetStep(planErrand)) <= 1) {
                if (Pocket_CountMatching(&tmp, Talk_IsShirtItem) > 0) {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x1b, 0x1b, &sRequestTopicsB[18]);
                } else {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            } else {
                if (Unk_02029234_Range(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0x11a8, 0x12a7)) {
                    if (Pocket_FindItem(((VillagerTalk *)this)->getErrandRecord()->getItem()) != -1) {
                        TalkChoiceTable_Set(this, (&buf), idx, 0x1b, 0x1b, &sRequestTopicsB[18]);
                    } else {
                        TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                    }
                } else {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            }
            if (Unk_02029234_Range(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0x11a8, 0x12a7)) {
                ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0, 7);
            }
            idx++;
            res = TRUE;
        }
        break;
    case 1:
        res = TRUE;
        break;
    case 2:
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[27]);
        idx++;
        a = res;
        res = TRUE;
        break;
    }
    if (res == TRUE) {
        if (a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
            idx++;
        }
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        idx++;
        buf.count = idx;
        buf.cancelIndex = idx - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&buf);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7950);
        ((TalkWindowState *)(window))->openChoices(1);
    }
    return res;
}

extern "C" u32 Talk_GetPocketMaskForItem(u16 *p) {
    void *s = PlayerData_GetCurrent()->getInventory();
    u16 *q = ((PlayerInventory *)(s))->getPocket(0);
    u32 mask = 0;
    s32 i = 0;
    do {
        if (((PlayerInventory *)(s))->getPocketFlags(i) == 0) {
            BOOL r;
            if (Item_IsFurniture(q)) {
                u32 a = Item_GetFurnitureIndex(q);
                r = (a == Item_GetFurnitureIndex(p)) ? TRUE : FALSE;
            } else {
                r = (*q == *p) ? TRUE : FALSE;
            }
            if (r) {
                mask = (u16)(mask | (1 << i));
            }
        }
        q++;
        i++;
    } while (i < 15);
    return mask;
}

extern "C" u32 Talk_GetWantedFossilPocketMask(PlanErrand *h) {
    void *s = PlayerData_GetCurrent()->getInventory();
    u16 *q = ((PlayerInventory *)(s))->getPocket(0);
    u16 tmp;
    u16 l[3];
    u32 mask;
    s32 i, k;
    l[0] = 0xfff1;
    l[1] = 0xfff1;
    l[2] = 0xfff1;
    mask = 0;
    for (i = 0; i < 3; i++) {
        if (PlanErrand_TestFlag(h, i) == 0) {
            FossilGroup_GetItem(&tmp, PlanErrand_GetFossilGroup(h), (u8)i);
            l[i] = tmp;
        } else {
            l[i] = 0xfff1;
        }
    }
    for (i = 0; i < 15; q++, i++) {
        if (((PlayerInventory *)(s))->getPocketFlags(i) == 0) {
            BOOL in = FALSE;
            if (*q >= 0x450c && *q <= 0x45db) {
                in = TRUE;
            }
            if (in) {
                for (k = 0; k < 3; k++) {
                    BOOL r;
                    if (Item_IsFurniture(q)) {
                        u32 a = Item_GetFurnitureIndex(q);
                        r = (a == Item_GetFurnitureIndex(&l[k])) ? TRUE : FALSE;
                    } else {
                        r = (*q == l[k]) ? TRUE : FALSE;
                    }
                    if (r) {
                        mask = (u16)(mask | (1 << i));
                        break;
                    }
                }
            }
        }
    }
    return mask;
}

s32 VillagerTalkTopics::openFossilRequestMenu() {
    BOOL a = TRUE;
    u8 idx = 0;
    BOOL res = FALSE;
    TalkChoiceTable buf;
    void *p, *q;
    struct { u8 t[2]; PocketMatches tmp; } L;
    TalkChoiceTable_Init(this, (&buf));
    switch (((VillagerTalk *)this)->getErrandRecord()->getStep()) {
    case 0:
        p = PlanErrand_GetPlayer(planErrand);
        q = PlayerData_GetCurrent()->getPlayerId();
        if (*(u16 *)q == *(u16 *)p && memcmp((u8 *)q + 2, (u8 *)p + 2, 8) == 0 && ((PlayerId *)(q))->equals((PlayerId *)(p)) != 0) {
            if (((u32)PlanErrand_GetStep(planErrand)) <= 1) {
                if (Pocket_CountMatching(&L.tmp, Talk_IsFossilItem) > 0) {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x1a, 0x1a, &sRequestTopicsB[18]);
                } else {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            } else if (((u32)PlanErrand_GetStep(planErrand)) >= 2) {
                if (Talk_GetWantedFossilPocketMask(planErrand) != 0) {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x1a, 0x1a, &sRequestTopicsB[18]);
                } else {
                    TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
                }
            } else {
                TalkChoiceTable_Set(this, (&buf), idx, 0x14, 0x14, &sRequestTopicsB[6]);
            }
            if (((u32)PlanErrand_GetStep(planErrand)) >= 2) {
                s32 r = FossilGroup_ToIndex(PlanErrand_GetFossilGroup(planErrand));
                if (r != -1) {
                    L.t[0] = r;
                    L.t[1] = 0;
                    ((ActorTalkRequest *)(this))->setSlotFromString(0, L.t, "st_fossil");
                }
            }
            idx++;
            res = TRUE;
        }
        break;
    case 1:
        if (Random_GlobalBelow(10) & a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[26]);
            a = idx;
            idx++;
        }
        res = TRUE;
        break;
    case 2:
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[27]);
        idx++;
        a = res;
        res = TRUE;
        break;
    }
    if (res == TRUE) {
        if (a) {
            TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
            idx++;
        }
        TalkChoiceTable_SetRange(this, (&buf), idx, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        idx++;
        buf.count = idx;
        buf.cancelIndex = idx - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&buf);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7960);
        ((TalkWindowState *)(window))->openChoices(1);
    }
    return res;
}

BOOL VillagerTalkTopics::tryAddCollectRequestChoice(void *a, void *b, u32 c) {
    u8 cc[2];
    u16 v1, v2, v3, v4, v5, v6, v7, v8;
    ClockDateTimeWords k;
    TalkChoiceTable s;
    BOOL r7 = FALSE;
    s32 r5, r6;
    planErrand = (VillagerPlanBlock_GetErrand(Villager_GetPlan(a)));
    ((VillagerTalk *)this)->setErrandRecord(PlanErrand_GetRecord(planErrand));
    r5 = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    if (((VillagerTalk *)actor)->getEventKind() == 0xb) {
        if (((PlayerId *)(PlanErrand_GetPlayer(planErrand)))->isValid() == 0) {
            r6 = PlanErrand_GetStep(planErrand);
            v1 = *((VillagerTalk *)this)->getErrandRecord()->getItem();
            r5 = r7;
            k.words[0] = r5;
            k.words[1] = r5;
            Clock_GetDateTime(&k.dt);
            switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
            case 0:
                if (Unk_0202849c_F(r7)) {
                    Npc_GetStateHeldItem((u16 *)(&v2), actor);
                    if (!Unk_0202849c_R(&v2, 0x1376, 0x1376)) goto done;
                }
                if (r6 == 2) {
                    r5 = 1;
                    goto done;
                }
                if (Unk_0202849c_R(&v1, 0x12b0, 0x12e7) && InsectPick_IsAvailable((u16 *)(&v1), 0, 0x17, ((u8 *)&k)[4]) != 0) {
                    itemFromPlayer = v1;
                } else {
                    VillagerRequest_PickInsectForStep((u16 *)(&v3), this, r6);
                    itemFromPlayer = v3;
                }
                if (Unk_02028a48_R1(&itemFromPlayer, 0x12b0, 0x12e7)) r5 = 1;
                break;
            case 1:
                if (Unk_0202849c_F(r7)) {
                    Npc_GetStateHeldItem((u16 *)(&v4), actor);
                    if (!Unk_0202849c_R(&v4, 0x1374, 0x1374)) goto done;
                }
                if (r6 == 2) {
                    r5 = 1;
                    goto done;
                }
                if (Unk_0202849c_R(&v1, 0x12e8, 0x131f) && FishPick_IsAvailable((u16 *)(&v1), 0, 0x17, ((u8 *)&k)[4], ((u8 *)&k)[3]) != 0) {
                    itemFromPlayer = v1;
                } else {
                    VillagerRequest_PickFishForStep((u16 *)(&v5), this, r6);
                    itemFromPlayer = v5;
                }
                if (Unk_02028a48_R1(&itemFromPlayer, 0x12e8, 0x131f)) r5 = 1;
                break;
            case 2:
                if (Unk_0202849c_F(r7)) {
                    Npc_GetStateHeldItem((u16 *)(&v6), actor);
                    if (!Unk_0202849c_R(&v6, 0x1369, 0x1369)) goto done;
                }
                {
                    volatile u16 *pv = &v1;
                    u16 a1 = *pv;
                    u16 b1 = *pv;
                    if (b1 != 0xfff1) itemFromPlayer = a1;
                }
                r5 = 1;
                break;
            default:
                if (Unk_0202849c_F(r7)) {
                    Npc_GetStateHeldItem((u16 *)(&v7), actor);
                    if (v7 != 0xfff1) {
                        Npc_GetStateHeldItem((u16 *)(&v8), actor);
                        if (!Unk_0202849c_R(&v8, 0x1380, 0x139f)) goto done;
                    }
                }
                {
                    volatile u16 *pv = &v1;
                    u16 a1 = *pv;
                    u16 b1 = *pv;
                    if (b1 != 0xfff1) itemFromPlayer = a1;
                }
                r5 = 1;
                break;
            }
done:
            if (r5 != 0) {
                TalkChoiceTable_Init(this, (&s));
                TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[3]);
                TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
                s.count = 2;
                s.cancelIndex = s.count - 1;
                ((VillagerTalk *)this)->setupChoiceMenu(&s);
                ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7980);
                ((TalkWindowState *)(window))->openChoices(1);
                r7 = TRUE;
            }
        } else {
            switch (r5) {
            case 0:
                r7 = openCreatureRequestMenu((BOOL (*)(u16 *))((void *)((void (*)())Talk_IsInsectItem)), 0x28, 0x19);
                break;
            case 1:
                r7 = openCreatureRequestMenu((BOOL (*)(u16 *))((void *)((void (*)())Talk_IsFishItem)), 0x29, 0x18);
                break;
            case 2:
                r7 = openFossilRequestMenu();
                break;
            case 3:
                r7 = openShirtRequestMenu();
                break;
            case 4:
                r7 = openFurnitureRequestMenu();
                if (r7 == 1) {
                    VillagerData *p = actor->villagerData;
                    cc[0] = FurnitureTaste_GetTextIndex(Villager_GetFurnitureTaste(p, Villager_GetFurnitureTasteIndex(p)));
                    cc[1] = 0;
                    ((ActorTalkRequest *)(this))->setSlotFromString(0, cc, "st_furniture_taste");
                }
                break;
            }
        }
    }
    if (r7 == 0) {
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 2;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7900);
        ((TalkWindowState *)(window))->openChoices(1);
        r7 = TRUE;
    }
    return r7;
}

void VillagerTalkTopics::setVisitTimeArgs(ClockDateTime *p) {
    u32 r4 = p->hour;
    if (r4 >= 12) {
        r4 -= 12;
    }
    if (r4 == 0) {
        r4 = 12;
    }
    ((ActorTalkRequest *)(this))->setDaySlot(p->day, 0);
    ((ActorTalkRequest *)(this))->setNumberSlot(r4, 1, 2, 0, 0);
    ((ActorTalkRequest *)(this))->setNumberSlot(p->minute, 2, 2, 6, 9);
}

BOOL VillagerTalkTopics::tryAddVisitReminderChoice(void *a, void *b, u32 c) {
    TalkChoiceTable s;
    HouseVisitInvite *r7 = &static_cast<PlayerErrands *>(b)->houseVisitInvite;
    ErrandRecord *r6 = &r7->errand;
    BOOL r4 = FALSE;
    if (((VillagerTalk *)actor)->getEventKind() == 0xb && r6->isActive() != 0 && r6->getKind() == 0x15 && HouseVisitInvite_IsFrom(r7, (VillagerIdCtorDtor *)((VillagerData *)(a))->getVillagerId()) != 0 && r6->getStep() == 0) {
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_Set(this, (&s), r4, 0x30, 0x30, &sRequestTopicsB[45]);
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 2;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7938);
        ((TalkWindowState *)(window))->openChoices(1);
        setVisitTimeArgs((ClockDateTime *)&r7->visitTime);
        r4 = TRUE;
    }
    if (r4 == 0) {
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 2;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7928);
        ((TalkWindowState *)(window))->openChoices(1);
        r4 = TRUE;
    }
    return r4;
}

s32 VillagerTalkTopics::tryAddActiveRequestChoice(void *a, void *b) {
    s32 res;
    u32 r4;
    void *player = PlayerData_GetCurrent();
    res = 0;
    if (((PlayerData *)player)->testFlag(1) != 0) {
        goto end;
    }
    r4 = Villager_GetPlayerErrandKind(gSaveVillagers, a, player);
    if (r4 >= 0x16) {
        goto end;
    }
    switch (Errand_GetClass(r4)) {
    case 0:
        res = tryAddCollectRequestChoice(a, b, r4);
        break;
    case 1:
        switch (Errand_GetGroup(r4)) {
        case 0:
            break;
        case 1:
            res = tryAddDeliveryClientChoice(a, b, r4);
            break;
        case 3:
            res = tryAddVisitReminderChoice(a, b, r4);
            break;
        }
        break;
    }
end:
    return res;
}

void *VillagerTalkTopics::pickRandomOtherVillager() {
    void *v[1];
    func_02133ef8(v, sizeof(v));
    v[0] = actor->villagerData->getVillagerId();
    return SaveVillagers_PickRandomTalkPartner(gSaveVillagers, v, 1);
}

BOOL VillagerTalkTopics::tryOfferDeliveryRequest(void *a, void *b) {
    TalkChoiceTable s;
    ClockDateTimeWords k;
    ErrandRecord *r7 = static_cast<ErrandRecord *>(VillagerState_GetErrand(Villager_GetState(a)));
    PlayerErrandSlot *r4 = PlayerErrands_GetSlot((PlayerErrands *)b, r7->getSlotIndex());
    void *r6 = VillagerPlanBlock_GetPlan(Villager_GetPlan(a));
    s32 t;
    k.words[0] = 0;
    k.words[1] = 0;
    Clock_GetDateTime(&k.dt);
    if (r4 != NULL && (PlayerErrandSlot_GetRecord(r4))->isActive() == 0) {
        t = r7->getKind();
        if (Errand_CanOfferInPlanState(t, ((VillagerPlan *)(r6))->getState()) != 0 && ((VillagerTalk *)actor)->getEventKind() == 0xb && SaveVillagers_FindBirthdayVillager(gSaveVillagers, &k) == -1 && pickRandomOtherVillager() != 0) {
            TalkChoiceTable_Init(this, (&s));
            errandSlot = r4;
            TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsA[1]);
            TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
            ((VillagerTalk *)this)->setErrandRecord(r7);
            s.count = 2;
            s.cancelIndex = s.count - 1;
            ((VillagerTalk *)this)->setupChoiceMenu(&s);
            ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d78f0);
            ((TalkWindowState *)(window))->openChoices(1);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkTopics::tryOfferCollectRequest(void *a) {
    u16 v[8];
    TalkChoiceTable s;
    ErrandRecord *r6 = static_cast<ErrandRecord *>(VillagerState_GetErrand(Villager_GetState(a)));
    BOOL r4;
    v[0] = 0xfff1;
    r4 = FALSE;
    planErrand = (VillagerPlanBlock_GetErrand(Villager_GetPlan(a)));
    if (((VillagerTalk *)actor)->getEventKind() == 0xb) {
        switch (r6->getKind()) {
        case 0:
            if (Unk_0202849c_F(r4)) {
                Npc_GetStateHeldItem((u16 *)(&v[1]), actor);
                if (!Unk_0202849c_R(&v[1], 0x1376, 0x1376)) goto end;
            }
            VillagerRequest_PickInsectForStep((u16 *)(&v[2]), this, 0);
            v[0] = v[2];
            {
                BOOL ok = FALSE;
                volatile u16 *pv = &v[0];
                u16 a1 = *pv;
                u16 b1 = *pv;
                if (b1 >= 0x12b0 && a1 <= 0x12e7) ok = TRUE;
                if (ok) {
                    itemFromPlayer = a1;
                    r4 = TRUE;
                }
            }
            break;
        case 1:
            if (Unk_0202849c_F(r4)) {
                Npc_GetStateHeldItem((u16 *)(&v[3]), actor);
                if (!Unk_0202849c_R(&v[3], 0x1374, 0x1374)) goto end;
            }
            VillagerRequest_PickFishForStep((u16 *)(&v[4]), this, 0);
            v[0] = v[4];
            {
                BOOL ok = FALSE;
                volatile u16 *pv = &v[0];
                u16 a1 = *pv;
                u16 b1 = *pv;
                if (b1 >= 0x12e8 && a1 <= 0x131f) ok = TRUE;
                if (ok) {
                    itemFromPlayer = a1;
                    r4 = TRUE;
                }
            }
            break;
        case 2:
            if (Unk_0202849c_F(r4)) {
                Npc_GetStateHeldItem((u16 *)(&v[5]), actor);
                if (!Unk_0202849c_R(&v[5], 0x1369, 0x1369)) goto end;
            }
            r4 = TRUE;
            break;
        default:
            if (Unk_0202849c_F(r4)) {
                Npc_GetStateHeldItem((u16 *)(&v[6]), actor);
                if (v[6] != 0xfff1) {
                    Npc_GetStateHeldItem((u16 *)(&v[7]), actor);
                    if (!Unk_0202849c_R(&v[7], 0x1380, 0x139f)) goto end;
                }
            }
            r4 = TRUE;
            break;
        }
    }
end:
    if (r4) {
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[3]);
        ((VillagerTalk *)this)->setErrandRecord(r6);
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 2;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d78f8);
        ((TalkWindowState *)(window))->openChoices(1);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::tryOfferVisitRequest(void *a, void *b) {
    ErrandRecord *r6;
    ClockDateTimeWords k;
    ErrandRecord *t = static_cast<ErrandRecord *>(VillagerState_GetErrand(Villager_GetState(a)));
    k.words[0] = 0;
    k.words[1] = 0;
    Clock_GetDateTime(&k.dt);
    if (gCommManager->isSlotActive(gCommManager->myAid) == 0) {
        r6 = &static_cast<PlayerErrands *>(b)->houseVisitInvite.errand;
        if (r6->isActive() == 0 && Villager_IsInPlayerErrand(a, b) == 0 && Villager_GetResidentStatus(a) == 3 && ((VillagerTalk *)actor)->getEventKind() == 0xb && Villager_GetUpcomingBirthdayDay(a, 1, &k) == -1 && VillagerEvent_FindUpcomingIndex(1, &k) == 0xb) {
            TalkChoiceTable s;
            TalkChoiceTable_Init(this, (&s));
            TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, &sRequestTopicsB[39]);
            TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
            ((VillagerTalk *)this)->setErrandRecord(t);
            s.count = 2;
            s.cancelIndex = s.count - 1;
            ((VillagerTalk *)this)->setupChoiceMenu(&s);
            ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d79f8);
            ((TalkWindowState *)(window))->openChoices(1);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkRequestReplyTopics::tryOfferNewRequest(void *a, void *b) {
    void *s = VillagerState_GetErrand(Villager_GetState(a));
    BOOL r = FALSE;
    if (((ErrandRecord *)(s))->isActive() != 0) {
        if (PlayerData_GetCurrent()->testFlag(1) == 0) {
            if (Random_GlobalBelow(100) < 30) {
                switch (((ErrandRecord *)(s))->getClass()) {
                case 0:
                    r = static_cast<VillagerTalkTopics *>(this)->tryOfferCollectRequest(a);  // (the decomp also passed b: it takes one)
                    break;
                case 1:
                    switch (((ErrandRecord *)(s))->getGroup()) {
                    case 0:
                        break;
                    case 1:
                        r = ((VillagerTalkTopics *)this)->tryOfferDeliveryRequest(a, b);
                        break;
                    case 3:
                        r = ((VillagerTalkTopics *)this)->tryOfferVisitRequest(a, b);
                        break;
                    }
                    break;
                }
            }
        }
    }
    return r;
}

BOOL VillagerTalkRequestReplyTopics::tryAddSickVillagerChoice(void *arg) {
    TalkChoiceTable s;
    void *r7 = gSaveVillagers;
    s32 r4 = SaveVillagers_GetUnk3830Index(r7);
    if (r4 != -1) {
        if (r4 == Villager_GetIndex(arg)) {
            SickVillagerRecord *p = static_cast<SickVillagerRecord *>(SaveVillagers_GetUnk3830(r7));
            ErrandRecord *v = p->getErrand();
            if (v->isActive() != 0) {
                if (p->isRecentlyRecovered(NULL) != 0) {
                    u16 *q = (u16 *)p->getTopVisitor();  // (compared as halfwords below)
                    if (q != NULL) {
                        if (((PlayerId *)(q))->isValid() != 0) {
                            u16 *r6 = ((u16 *)PlayerData_GetCurrent()->getPlayerId());
                            VillagerTalkTopicFns *msg;
                            TalkChoiceTable_Init(this, (&s));
                            if (r6[0] == q[0] && memcmp(r6 + 1, q + 1, 8) == 0 && ((PlayerId *)(r6))->equals((PlayerId *)(q)) != 0) {
                                if (v->getStep() == 1) {
                                    msg = &sRequestTopicsB[37];
                                } else {
                                    msg = &sRequestTopicsB[33];
                                }
                            } else if (p->hasVisitor((PlayerId *)(r6)) != 0) {
                                msg = &sRequestTopicsB[32];
                            } else {
                                msg = &sRequestTopicsB[31];
                            }
                            TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, msg);
                            TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
                            s.count = 2;
                            s.cancelIndex = s.count - 1;
                            ((VillagerTalk *)this)->setupChoiceMenu(&s);
                            ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d79b8);
                            ((TalkWindowState *)(window))->openChoices(1);
                            ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)q, 1);
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    return FALSE;
}

void VillagerTalkRequestReplyTopics::openConnectMenu() {
    static Unk_02027a34_TestFn tbl[5] = { (Unk_02027a34_TestFn)data_020d7c60, (Unk_02027a34_TestFn)data_020d8000, (Unk_02027a34_TestFn)data_020d7ce8, (Unk_02027a34_TestFn)data_020d7d00, data_020d7d08 };
    VillagerData *r6 = actor->villagerData;
    void *r7 = PlayerData_GetCurrent()->getErrands();
    s32 i;
    for (i = 0; i < 5; i++) {
        if ((this->*tbl[i])(r6, r7) != 0) {
            break;
        }
    }
    if (i == 5) {
        TalkChoiceTable s;
        TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
        TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
        s.count = 2;
        s.cancelIndex = s.count - 1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7cd8);
        ((TalkWindowState *)(window))->openChoices(1);
    }
}

extern "C" {
char sTalkKeyTsuEvent1[11] = "tsu_event1";
decltype(&VillagerTalkTopics::updateEvBirthTurn) data_020d7aa8 = &VillagerTalkTopics::updateEvBirthTurn;
const TalkTopic sTalkTopicTsuMove1 = {sTalkKeyTsuMove1, 0x3};
decltype(&VillagerTalkKaraokeTopics::continueEvKaraokeAction) data_020d7ab0 = &VillagerTalkKaraokeTopics::continueEvKaraokeAction;
const TalkTopic sTalkTopicQ06Over = {sTalkKeyQ06Over, 0x1};
decltype(&VillagerTalkTopics::onQ12FullClose) data_020d7998 = &VillagerTalkTopics::onQ12FullClose;
decltype(&VillagerTalkTopics::reopenWindow) data_020d7988 = &VillagerTalkTopics::reopenWindow;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7980 = &VillagerTalkRequestReplyTopics::runChosenTopic;
}

void VillagerTalkRequestReplyTopics::runChosenTopic(TalkTopicMsg *, u32 idx) {
    u8 b;
    TalkTopicMsg out;
    if (choiceValues[idx] != NULL) {
        ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)choiceValues[idx]);
    } else {
        ((VillagerTalk *)this)->clearTopicFns();
    }
    if (selectFn) {
        (this->*selectFn)(&out);
        if (out.fileName != 0) {
            b = out.msgIndex;
            ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
        }
    }
}

void VillagerTalkRequestReplyTopics::selectDeliveryRequest(TalkTopicMsg *out) {
    const TalkTopic *d = NULL;
    s32 t = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    switch (t) {
    case 10:
        d = &sTalkTopicQ06Req;
        break;
    case 19:
        d = &sTalkTopicQ07Req;
        break;
    }
    if (d != NULL) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkRequestReplyTopics::pickDeliveryRecipient() {
    void *buf[1];
    func_02133ef8(buf, sizeof(buf));
    buf[0] = actor->villagerData->getVillagerId();
    deliveryRecipient = ((VillagerData *)(SaveVillagers_PickRandomTalkPartner(gSaveVillagers, buf, 1)))->getVillagerId();
    ((ActorTalkRequest *)(this))->setVillagerNameSlot(deliveryRecipient, 1);
}

void VillagerTalkRequestReplyTopics::gotoDeliveryTime() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[2]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkRequestReplyTopics::selectQTime(TalkTopicMsg *out) {
    u8 tbl[3];
    tbl[0] = sDeliveryMinutes[0];
    tbl[1] = sDeliveryMinutes[1];
    tbl[2] = sDeliveryMinutes[2];
    deliveryTimeIndex = Random_GlobalBelow(3);
    deliveryMinutes = tbl[deliveryTimeIndex];
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQTime.key, sTalkTopicQTime.variantCount, deliveryTimeIndex, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkRequestReplyTopics::openDeliveryAcceptChoice() {
    TalkChoiceTable s;
    VillagerTalkTopicFns *r4 = &sRequestTopicsA[4];
    s32 t = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    switch (t) {
    case 10:
        if (PlayerData_GetCurrent() != NULL) {
            if (((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->findEmptyPocket() != -1) {
                r4 = &sRequestTopicsA[5];
            }
        }
        break;
    case 19:
        if (Inventory_FindEmptyLetter() != -1) {
            r4 = &sRequestTopicsA[5];
        }
        break;
    }
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x21, 0x21, r4);
    TalkChoiceTable_Set(this, (&s), 1, 0x22, 0x22, &sRequestTopicsA[3]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7c90);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkRequestReplyTopics::selectQNoB(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQNo.key, sTalkTopicQNo.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7c28);
}

void VillagerTalkRequestReplyTopics::cancelRequest() {
    ((VillagerTalk *)this)->getErrandRecord()->clear();
    SaveVillagers_UpdateErrands(gSaveVillagers);
}

void VillagerTalkRequestReplyTopics::selectQFull(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQFull.key, sTalkTopicQFull.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkRequestReplyTopics::continueQFull() {
}

void VillagerTalkRequestReplyTopics::selectDeliveryAccepted(TalkTopicMsg *out) {
    const TalkTopic *d = NULL;
    s32 t = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    switch (t) {
    case 10:
        d = &sTalkTopicQYes;
        break;
    case 19:
        d = &sTalkTopicQ07Mailgo;
        break;
    }
    if (d != NULL) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5e;
}

void VillagerTalkRequestReplyTopics::setDeliveryDeadline() {
    if (((VillagerTalk *)this)->getErrandRecord() != NULL) {
        ClockDateTime *p = ErrandRecord_GetTime(((VillagerTalk *)this)->getErrandRecord());
        Clock_GetDateTime(p);
        DateTime_AddMinutes(p, deliveryMinutes);
        ((VillagerTalk *)this)->getErrandRecord()->setExtra(deliveryTimeIndex);
    }
}

void VillagerTalkRequestReplyTopics::acceptDeliveryRequest() {
    void *p7 = PlayerData_GetCurrent();
    ((PlayerData *)(p7))->getErrands();
    VillagerData *r6 = actor->villagerData;
    s32 r4 = 2;
    if (errandSlot != NULL) {
        s32 t = ((VillagerTalk *)this)->getErrandRecord()->getKind();
        VillagerId *h = r6->getVillagerId();
        PlayerErrandSlot_Start(errandSlot, t, h, deliveryRecipient);
        ((VillagerTalk *)this)->getErrandRecord()->clear();
        SaveVillagers_UpdateErrands(gSaveVillagers);
        ((VillagerTalk *)this)->setErrandRecord(PlayerErrandSlot_GetRecord(errandSlot));
        itemFromPlayer = *((VillagerTalk *)this)->getErrandRecord()->getItem();
        s32 k = ((VillagerTalk *)this)->getErrandRecord()->getKind();
        switch (k) {
        case 10: {
            s32 q = ((PlayerInventory *)(((PlayerData *)(p7))->getInventory()))->findEmptyPocket();
            ((PlayerInventory *)(((PlayerData *)(p7))->getInventory()))->setPocket((u16 *)(&itemFromPlayer), q, r4);
            break;
        }
        case 19: {
            Letter *x = Inventory_GetEmptyLetter();
            if (x != NULL) {
                PlayerErrandSlot_ComposeLetter(errandSlot, x);
            }
            r4 = 0;
            break;
        }
        }
    }
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemFromPlayer), r4, 5, 0);
    ((VillagerTalk *)this)->setTaskDoneFn(data_020d7aa0);
    ((VillagerTalk *)this)->setDeferredFn((Unk_020d8938_Fn)data_020d7a48);
}

s32 VillagerTalkRequestReplyTopics::getRequestKind() {
    return ((VillagerTalk *)this)->getErrandRecord()->getKind();
}

void VillagerTalkRequestReplyTopics::selectDeliveryLate(TalkTopicMsg *out) {
    const TalkTopic *d = NULL;
    s32 t = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    switch (t) {
    case 10:
        d = &sTalkTopicQ06Over;
        break;
    case 19:
        d = &sTalkTopicQ07Over;
        break;
    }
    if (d != NULL) {
        u8 v = VillagerId_GetPersonality(actor->villagerData->getVillagerId());
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, v, d->key, d->variantCount, ((VillagerTalk *)this)->getErrandRecord()->getExtra(), 0);
        if (errandSlot != NULL) {
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 0)), 0);
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 1)), 1);
            ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0, 7);
        }
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::continueLateLetterShown() {
    TalkTopicMsg out;
    u8 b;
    void *inv = PlayerData_GetCurrent()->getInventory();
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 19) {
        LetterView *r5 = Talk_FindLetterState7or8(inv);
        if (r5 != 0) {
            if (r5->getState() == 7) {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[7]);
            } else {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[8]);
            }
            Letter_Clear(r5);
        }
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::continueDeliveryLate() {
    TalkMsgIndexItem c;
    TalkTopicMsg out;
    s32 r4;
    void *r6 = PlayerData_GetCurrent()->getInventory();
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        r4 = Talk_FindFlaggedPocketItem(r6, ((VillagerTalk *)this)->getErrandRecord()->getItem());
        if (r4 != -1) {
            itemFromPlayer = *((VillagerTalk *)this)->getErrandRecord()->getItem();
            c.item = 0xfff1;
            ((PlayerInventory *)(r6))->setPocket((u16 *)(&c.item), r4, 0);
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[7]);
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[8]);
        }
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        c.msgIndex = out.msgIndex;
        ((TalkWindowState *)window)->setNextMessageIfUnset(&c.msgIndex, (void *)(out.fileName));
        break;
    case 19:
        itemFromPlayer = 0x1565;
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7a50);
        break;
    }
    SaveVillagers_AddRelation(gSaveVillagers, actor->villagerData->getVillagerId(), PlayerErrandSlot_GetVillager(errandSlot, 1), -10);
}

void VillagerTalkTopics::selectDeliveryLateReply(TalkTopicMsg *out) {
    const TalkTopic *r4 = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        r4 = &sTalkTopicQ06Payback;
        break;
    case 19:
        r4 = &sTalkTopicQ07Scold;
        break;
    }
    if (r4 != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), r4->key, r4->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::finishLateDelivery() {
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 10) {
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 2, 5, 0);
        ((VillagerTalk *)this)->setTaskDoneFn(data_020d7c10);
    }
    PlayerErrandSlot_Clear(errandSlot);
}

void VillagerTalkTopics::selectDeliveryLost(TalkTopicMsg *out) {
    const TalkTopic *r4 = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        r4 = &sTalkTopicQ06Lost;
        break;
    case 19:
        r4 = &sTalkTopicQ07Scold2;
        break;
    }
    if (r4 != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), r4->key, r4->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::finishLostDelivery() {
    void *r4 = actor->villagerData->getVillagerId();
    SaveVillagers_AddRelation(gSaveVillagers, r4, PlayerErrandSlot_GetVillager(errandSlot, 1), -20);
    PlayerErrandSlot_Clear(errandSlot);
}

void VillagerTalkTopics::selectDeliveryOpened(TalkTopicMsg *out) {
    const TalkTopic *r4 = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        r4 = &sTalkTopicQ06Open;
        break;
    case 19:
        r4 = &sTalkTopicQ07Open;
        break;
    }
    if (r4 != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), r4->key, r4->variantCount, 0, 0);
        if (errandSlot != 0) {
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 0)), 0);
            ((ActorTalkRequest *)(this))->setVillagerNameSlot((PlayerErrandSlot_GetVillager(errandSlot, 1)), 1);
            ((ActorTalkRequest *)(this))->setItemNameSlot(((VillagerTalk *)this)->getErrandRecord()->getItem(), 0, 7);
        }
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7930);
}

void VillagerTalkTopics::finishOpenedDelivery() {
    void *r4;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 19) {
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(((VillagerTalk *)this)->getErrandRecord()->getItem()), 0, 5, 0);
        ((VillagerTalk *)this)->setTaskDoneFn(data_020d7940);
    }
    r4 = actor->villagerData->getVillagerId();
    SaveVillagers_AddRelation(gSaveVillagers, r4, PlayerErrandSlot_GetVillager(errandSlot, 1), -20);
    PlayerErrandSlot_Clear(errandSlot);
}

void VillagerTalkTopics::selectQTimeover(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQTimeover.key, sTalkTopicQTimeover.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

extern "C" BOOL Talk_FilterPickDeliveryShirt(u16 *p, s32 v) {
    BOOL r4 = FALSE;
    if (Unk_020270ec_R(p, 0x11a8, 0x12a7) && v == 2) {
        r4 = TRUE;
    }
    return r4;
}

void VillagerTalkTopics::openDeliveryItemPicker() {
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickDeliveryShirt, 0xd, 0);
        ((ActorTalkRequest *)(this))->openSubScene(0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7908);
        break;
    case 19:
        ((ActorTalkRequest *)(this))->setSubSceneKind(0x28, 1);
        ((ActorTalkRequest *)(this))->openSubScene(2);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7948);
        break;
    default:
        ((ActorTalkRequest *)(this))->openSubScene(7);
        break;
    }
}

void VillagerTalkTopics::selectQIcancel(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQIcancel.key, sTalkTopicQIcancel.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::reopenWindow() {
    if (((TalkWindowState *)window) != 0) {
        ((TalkWindowState *)window)->nextState = 1;
    }
}

void VillagerTalkTopics::onDeliveryItemPicked() {
    TalkMsgIndexItem c;
    TalkTopicMsg out;
    if (MenuCtrl_IsResultOk() == 0) {
        if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 19 && ((TalkWindowState *)window) != 0) {
            ((TalkWindowState *)window)->nextState = 1;
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[11]);
    } else {
        s32 r4 = 2;
        s32 r6 = MenuCtrl_GetIndex();
        void *r7 = PlayerData_GetCurrent()->getInventory();
        switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
        case 10:
            itemFromPlayer = *((PlayerInventory *)(r7))->getPocket(r6);
            c.item = 0xfff1;
#ifdef N3DS_DIRECT
            c.item = n3ds_fruit_rest(itemFromPlayer);  // of a fruit stack one fruit is given, the rest stays
            itemFromPlayer = n3ds_fruit_one(itemFromPlayer);
#endif
            ((PlayerInventory *)(r7))->setPocket((u16 *)(&c.item), r6, 0);
            break;
        case 19:
            ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7988);
            itemFromPlayer = 0x1565;
            r4 = 0;
            break;
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[12]);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), r4, 4, 0);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    c.msgIndex = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessage((u8 *)(&c.msgIndex), (void *)(out.fileName));
}

void VillagerTalkTopics::selectDeliveryReceived(TalkTopicMsg *out) {
    const TalkTopic *r4 = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        r4 = &sTalkTopicQ06Get;
        break;
    case 19:
        r4 = &sTalkTopicQ07Joy;
        break;
    }
    if (r4 != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), r4->key, r4->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::continueDeliveryReceived() {
    TalkTopicMsg out;
    u8 b;
    LetterView *r4;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        ((ActorTalkRequest *)(this))->requestItemAct12();
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d8018);
        break;
    case 19:
        r4 = Talk_FindLetterState7or8(PlayerData_GetCurrent()->getInventory());
        if (r4 != 0) {
            if (r4->getState() == 7) {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[18]);
            } else {
                Letter_Clear(r4);
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[17]);
            }
            if (selectFn) {
                (this->*selectFn)(&out);
            }
            b = out.msgIndex;
            ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
        }
        break;
    }
    SaveVillagers_AddRelation(gSaveVillagers, PlayerErrandSlot_GetVillager(errandSlot, 0), actor->villagerData->getVillagerId(), 10);
}

void VillagerTalkTopics::judgePresent() {
    TalkTopicMsg out;
    u8 b;
    s32 r4 = 1;
    u8 *r6 = (u8 *)Villager_GetFashionTaste(actor->villagerData);
    if (Unk_020270ec_R(&itemFromPlayer, 0x11a8, 0x12a7)) {
        u8 v = Item_GetShirtUnkGroup(&itemFromPlayer);
        if (v == r6[0]) {
            r4 = 0;
        } else if (v == r6[1]) {
            r4 = 2;
        }
    }
    PlayerErrandSlot_SetPresentReaction((u8 *)errandSlot, r4);
    switch (r4) {
    case 0:
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[14]);
        break;
    case 2:
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[15]);
        break;
    default:
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[16]);
        break;
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ06Open1(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Open1.key, sTalkTopicQ06Open1.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::continuePresentLiked() {
    u8 b;
    TalkTopicMsg out;
    VillagerData *r4 = actor->villagerData;
    void *r6;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[20]);
    ((ActorTalkRequest *)(this))->requestItemAct0F();
    ((VillagerTalk *)this)->setTaskDoneFn(data_020d8008);
    r6 = PlayerErrandSlot_GetVillager(errandSlot, 0);
    SaveVillagers_AddRelation(gSaveVillagers, r6, r4->getVillagerId(), 0x14);
    itemFromPlayer = *((VillagerData *)(r4))->getShirt();
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ06Open3(TalkTopicMsg *out) {
    (PlayerErrandSlot_GetRecord(errandSlot))->setStep(1);
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Open3.key, sTalkTopicQ06Open3.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7bd8);
}

void VillagerTalkTopics::keepDislikedPresent() {
    VillagerData *r4 = actor->villagerData;
    ((ActorTalkRequest *)(this))->requestKeepItem();
    if (itemFromPlayer != 0xfff1) {
        Villager_AddReceivedItem(r4, &itemFromPlayer);
    }
}

void VillagerTalkTopics::selectQ06Open2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Open2.key, sTalkTopicQ06Open2.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::continuePresentAccepted() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[20]);
    ((ActorTalkRequest *)(this))->requestKeepItem();
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ07Open2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ07Open2.key, sTalkTopicQ07Open2.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7ff0);
}

void VillagerTalkTopics::finishOpenedLetter() {
    void *r4;
    (PlayerErrandSlot_GetRecord(errandSlot))->setStep(1);
    r4 = PlayerErrandSlot_GetVillager(errandSlot, 0);
    SaveVillagers_AddRelation(gSaveVillagers, r4, actor->villagerData->getVillagerId(), -0x14);
    ((ActorTalkRequest *)(this))->requestKeepItem();
    ((VillagerTalk *)this)->setTaskDoneFn(data_020d7fe8);
}

void VillagerTalkTopics::selectQ07Read(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ07Read.key, sTalkTopicQ07Read.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::gotoDeliveryFin() {
    u8 b;
    TalkTopicMsg out;
    if (((TalkWindowState *)window)->state == 5) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[20]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::continueLetterRead() {
    u8 b;
    TalkTopicMsg out;
    if (memory != 0 && ((VillagerMemory *)(memory))->getFriendship() >= 0x40) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[19]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
    } else {
        ((ActorTalkRequest *)(this))->requestKeepItem();
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7fe0);
    }
}

void VillagerTalkTopics::selectQ07Show(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ07Show.key, sTalkTopicQ07Show.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::closeLetter() {
    ((ActorTalkRequest *)(this))->requestKeepItem();
    ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7bc0);
}

void VillagerTalkTopics::showLetter() {
    ((ActorTalkRequest *)(this))->setMenu12Arg(Talk_FindLetterState7or8((PlayerData_GetCurrent())->getInventory()), 1);
    ((ActorTalkRequest *)(this))->openSubScene(5);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7fc8);
}

void VillagerTalkTopics::selectDeliveryFin(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ06Fin;
    LetterView *r7;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        (PlayerErrandSlot_GetRecord(errandSlot))->setStep(1);
        if (Unk_02026ab0_R1(&itemFromPlayer, 0x11a8, 0x12a7)) {
            Villager_AddReceivedItem(actor->villagerData, &itemFromPlayer);
        }
        break;
    case 0x13:
        r7 = Talk_FindLetterState7or8(PlayerData_GetCurrent()->getInventory());
        (PlayerErrandSlot_GetRecord(errandSlot))->setStep(1);
        d = &sTalkTopicQ07Fin;
        if (r7 != 0) {
            Letter_Clear(r7);
        }
        break;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectDeliveryReminder(TalkTopicMsg *out) {
    const TalkTopic *d = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        d = &sTalkTopicQ06Con;
        break;
    case 0x13:
        d = &sTalkTopicQ07Con;
        break;
    }
    if (errandSlot != NULL && d != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectDeliveryReport(TalkTopicMsg *out) {
    const TalkTopic *d = 0;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        d = &sTalkTopicQ06Report;
        break;
    case 0x13:
        d = &sTalkTopicQ07Report;
        break;
    }
    if (errandSlot != NULL && d != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

extern "C" void Talk_GetMoneyItem(u16 *out, u32 arg) {
    *out = 0xfff1;
    *out = Item_FindMoneyBagForAmount(arg, FALSE, NULL);
    if (*out == 0xfff1) {
        *out = 0x1492;
    }
}

void VillagerTalkTopics::continueDeliveryReport(void *p) {
    u16 h[3];
    TalkChoiceTable s;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 10:
        TalkChoiceTable_Init(this, (&s));
        TalkChoiceTable_Set(this, (&s), 0, 0x24, 0x24, 0);
        TalkChoiceTable_Set(this, (&s), 1, 0x25, 0x25, 0);
        TalkChoiceTable_Set(this, (&s), 2, 0x26, 0x26, 0);
        s.count = 3;
        s.cancelIndex = -1;
        ((VillagerTalk *)this)->setupChoiceMenu(&s);
        ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7fa0);
        ((TalkWindowState *)window)->openChoices(1);
        break;
    case 0x13: {
        s32 r = Random_GlobalBelow(100);
        itemFromPlayer = 0xfff1;
        if (r < 20) {
            Villager_PickRandomReceivedItem(&h[0], actor->villagerData);
            itemFromPlayer = h[0];
            if (itemFromPlayer != 0xfff1) {
                Villager_RemoveReceivedItem(actor->villagerData, &itemFromPlayer);
            }
        } else if (r < 0x28) {
            unk_19c_w = 0x1f4 + FengShui_GetWestTotal() * 4;
            unk_19c_w = ((u32)Talk_RoundBells(unk_19c_w));
            Talk_GetMoneyItem(&h[1], unk_19c_w);
            itemFromPlayer = h[1];
        }
        if (itemFromPlayer == 0xfff1) {
            Talk_PickErrandItem(&h[2], 0);
            itemFromPlayer = h[2];
        }
        gotoRewardOrEnd();
        break;
    }
    }
}

void VillagerTalkTopics::onPresentOpinionChoice(s32 unused, s32 idx) {
    u8 b;
    TalkTopicMsg out;
    PresentOpinionTopicList tbl = sPresentOpinionTopics;
    if (idx < 0 || idx >= 3) {
        idx = 0;
    }
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[tbl.v[idx]]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ06Good(TalkTopicMsg *out) {
    u16 loc[5];
    u8 t = *PlayerErrandSlot_GetPresentReaction((u8 *)errandSlot);
    itemFromPlayer = 0xfff1;
    switch (t) {
    case 0:
        Talk_PickErrandItem(&loc[0], 0);
        itemFromPlayer = loc[0];
        break;
    case 1:
        Villager_PickRandomReceivedItem(&loc[1], actor->villagerData);
        itemFromPlayer = loc[1];
        if (itemFromPlayer != 0xfff1) {
            Villager_RemoveReceivedItem(actor->villagerData, &itemFromPlayer);
        }
        break;
    default:
        Villager_PickRandomReceivedItem(&loc[2], actor->villagerData);
        itemFromPlayer = loc[2];
        if (itemFromPlayer != 0xfff1) {
            Villager_RemoveReceivedItem(actor->villagerData, &itemFromPlayer);
        } else {
            Talk_PickErrandItem(&loc[3], 0);
            itemFromPlayer = loc[3];
        }
        break;
    }
    if (itemFromPlayer == 0xfff1) {
        price = FengShui_GetWestTotal() * 4 + 0x1f4;
        price = Talk_RoundBells(price);
        Talk_GetMoneyItem(&loc[4], price);
        itemFromPlayer = loc[4];
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Good.key, sTalkTopicQ06Good.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ06Normal(TalkTopicMsg *out) {
    u16 loc[4];
    u8 t = *PlayerErrandSlot_GetPresentReaction((u8 *)errandSlot);
    itemFromPlayer = 0xfff1;
    switch (t) {
    case 0:
        Villager_PickRandomReceivedItem(&loc[0], actor->villagerData);
        itemFromPlayer = loc[0];
        if (itemFromPlayer != 0xfff1) {
            Villager_RemoveReceivedItem(actor->villagerData, &itemFromPlayer);
        }
        break;
    case 1:
        Talk_PickErrandItem(&loc[1], 0);
        itemFromPlayer = loc[1];
        break;
    }
    if (itemFromPlayer == 0xfff1) {
        price = FengShui_GetWestTotal() * 4 + 0x1f4;
        price = Talk_RoundBells(price);
        Talk_GetMoneyItem(&loc[2], price);
        itemFromPlayer = loc[2];
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Normal.key, sTalkTopicQ06Normal.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ06Bad(TalkTopicMsg *out) {
    u16 loc[4];
    u8 t = *PlayerErrandSlot_GetPresentReaction((u8 *)errandSlot);
    itemFromPlayer = 0xfff1;
    if (t == 2) {
        Talk_PickErrandItem(&loc[0], 0);
        itemFromPlayer = loc[0];
    }
    if (itemFromPlayer == 0xfff1) {
        price = FengShui_GetWestTotal() * 4 + 0x1f4;
        price = Talk_RoundBells(price);
        Talk_GetMoneyItem(&loc[1], price);
        itemFromPlayer = loc[1];
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ06Bad.key, sTalkTopicQ06Bad.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::gotoRewardOrEnd() {
    u8 b;
    TalkTopicMsg out;
    if (PlayerData_GetCurrent() != 0 && ((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->findEmptyPocket() != -1) {
        ((VillagerTalk *)this)->setTopicFns(sRequestTopicsB);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[2]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQPreitem(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQPreitem.key, sTalkTopicQPreitem.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::gotoRewardItem() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[1]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQItemC(TalkTopicMsg *out) {
    TalkTopic d;
    if (Unk_02026214_R1(&itemFromPlayer, 0x1492, 0x14fd)) {
        ((ActorTalkRequest *)(this))->setNumberNamedSlot(price, 1, 4, 1, 1, 0);
    } else {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 1, 7);
    }
    d = sTalkTopicQItemC;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::giveRewardItem() {
    void *p6 = PlayerData_GetCurrent();
    u8 *r4 = (u8 *)((PlayerData *)(p6))->getInventory();
    if (Unk_02026214_R1(&itemFromPlayer, 0x1492, 0x14fd)) {
        PlayerInventory_AddBells(r4, price, 1);
    } else if (itemFromPlayer != 0xfff1) {
        s32 v = ((PlayerInventory *)(r4))->findEmptyPocket();
        if (v != -1) {
            ((PlayerInventory *)r4)->setPocket((u16 *)(&itemFromPlayer), v, 0);
            Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(p6))->getCatalog()), &itemFromPlayer, 0, 1);
        }
    }
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemFromPlayer), 0, 5, 0);
}

void VillagerTalkTopics::gotoDeliveryEnd() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[2]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectDeliveryEnd(TalkTopicMsg *out) {
    const TalkTopic *d;
    s32 r = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    switch (r) {
    case 0xa:
        d = &sTalkTopicQ06End;
        break;
    case 0x13:
        d = &sTalkTopicQ07End;
        break;
    default:
        d = &sTalkTopicQ06End;
        break;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    if (errandSlot != 0) {
        PlayerErrandSlot_Clear(errandSlot);
    }
    pendingSe = 0x5f;
}

void VillagerTalkTopics::selectCollectRequest(TalkTopicMsg *out) {
    TalkTopic d = data_020d7b40;
    s32 r6 = 0;
    s32 r4;
    u8 buf[2];
    s32 idx;
    r4 = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    idx = r6;
    if ((PlanErrand_GetRecord(planErrand))->isActive() != 0) {
        r6 = ((u32)PlanErrand_GetStep(planErrand));
        r4 = (PlanErrand_GetRecord(planErrand))->getKind();
    }
    ((VillagerTalk *)this)->getErrandRecord()->getClassIndex(&idx);
    if (r6 < PlanErrand_GetStepCount(r4) && idx < 5) {
        d.key = sTalkKeysQReq[idx][r6];
        if (r4 == 4) {
            VillagerData *p = actor->villagerData;
            buf[0] = FurnitureTaste_GetTextIndex(Villager_GetFurnitureTaste(p, Villager_GetFurnitureTasteIndex(p)));
            buf[1] = 0;
            ((ActorTalkRequest *)(this))->setSlotFromString(0, buf, "st_furniture_taste");
        }
        if (r4 != 0 && r4 != 1) {
            pendingSe = 0x5e;
        }
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

extern "C" void VillagerRequest_PickInsectStep0(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!InsectPick_PickForMonthAnyHour(p, 0, 1, d->month)) {
        InsectPick_PickForMonthAnyHour(p, 2, 4, d->month);
    }
}

extern "C" void VillagerRequest_PickInsectStep1(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!InsectPick_PickForMonthAnyHour(p, 1, 2, d->month)) {
        if (!InsectPick_PickForMonthAnyHour(p, 0, 0, d->month)) {
            InsectPick_PickForMonthAnyHour(p, 3, 4, d->month);
        }
    }
}

extern "C" void VillagerRequest_PickInsectStep3(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!InsectPick_PickForMonthAnyHour(p, 2, 3, d->month)) {
        if (!InsectPick_PickForMonthAnyHour(p, 0, 1, d->month)) {
            InsectPick_PickForMonthAnyHour(p, 4, 4, d->month);
        }
    }
}

extern "C" void VillagerRequest_PickInsectStep4(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!InsectPick_PickForMonthAnyHour(p, 3, 4, d->month)) {
        InsectPick_PickForMonthAnyHour(p, 0, 2, d->month);
    }
}

extern "C" void VillagerRequest_PickInsectForStep(u16 *p, void *unused, u32 idx) {
    ClockDateTimeWords a;
    RequestDatePickerFn fn;
    if (idx < 5 && (fn = sRequestInsectPickers[idx]) != 0) {
        a.words[0] = 0;
        a.words[1] = 0;
        Clock_GetDateTime(&a.dt);
        fn(p, &a.dt);
    } else {
        *p = 0xfff1;
    }
}

extern "C" void VillagerRequest_PickFishStep0(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!FishPick_PickForDateAnyHour(p, 0, 1, d->month, d->day)) {
        FishPick_PickForDateAnyHour(p, 2, 4, d->month, d->day);
    }
}

extern "C" void VillagerRequest_PickFishStep1(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!FishPick_PickForDateAnyHour(p, 1, 2, d->month, d->day)) {
        if (!FishPick_PickForDateAnyHour(p, 0, 0, d->month, d->day)) {
            FishPick_PickForDateAnyHour(p, 3, 4, d->month, d->day);
        }
    }
}

extern "C" void VillagerRequest_PickFishStep3(u16 *p, ClockDateTime *d) {
    *p = 0xfff1;
    if (!FishPick_PickForDateAnyHour(p, 2, 3, d->month, d->day)) {
        if (!FishPick_PickForDateAnyHour(p, 0, 1, d->month, d->day)) {
            FishPick_PickForDateAnyHour(p, 4, 4, d->month, d->day);
        }
    }
}

extern "C" void VillagerRequest_PickFishStep4(u16 *out, ClockDateTime *d) {
    *out = 0xfff1;
    if (!FishPick_PickForDateAnyHour(out, 3, 4, d->month, d->day)) {
        FishPick_PickForDateAnyHour(out, 0, 2, d->month, d->day);
    }
}

extern "C" void VillagerRequest_PickFishForStep(u16 *out, void *unused, u32 idx) {
    if (idx < 5) {
        RequestDatePickerFn fn = sRequestFishPickers[idx];
        if (fn != 0) {
            ClockDateTimeWords pr;
            pr.words[0] = 0;
            pr.words[1] = 0;
            Clock_GetDateTime(&pr.dt);
            fn(out, &pr.dt);
            return;
        }
    }
    *out = 0xfff1;
}

extern "C" void VillagerRequest_PickShirtFromGroups(u16 *out, u32 mask, s32 cnt, void *x, u8 a, u16 *b) {
    u16 buf;
    s32 r;
    *out = 0xfff1;
    while ((r = Random_PickSetBit(mask, cnt, 10)) != -1) {
        ItemPick_FromRange(&buf, 0x11a8, 0x100, b, 1, x, a, r, 0, 1);
        *out = buf;
        break;
    }
}

extern "C" void VillagerRequest_PickShirtOtherGroup(u16 *out, u8 *p, u16 *arg) {
    u16 arr[2];
    u16 mask;
    s32 cnt;
    s32 i;
    *out = 0xfff1;
    mask = 0;
    cnt = 0;
    for (i = 0; i < 10; i++) {
        if (i != p[1]) {
            mask |= 1 << i;
            cnt++;
        }
    }
    VillagerRequest_PickShirtFromGroups(&arr[0], mask, cnt, PlayerData_GetCurrent(), 1, arg);
    *out = arr[0];
    if (*out == 0xfff1) {
        VillagerRequest_PickShirtFromGroups(&arr[1], mask, cnt, 0, 0, arg);
        *out = arr[1];
    }
}

extern "C" void VillagerRequest_PickShirtOfGroup(u16 *out, u8 *p, u16 *arg) {
    ItemPick_FromRange(out, 0x11a8, 0x100, arg, 1, 0, 0, *p, 0, 1);
}

extern "C" void VillagerRequest_PickShirtOfGroupForPlayer(u16 *out, u8 *p, u16 *arg) {
    u16 arr[2];
    *out = 0xfff1;
    ItemPick_FromRange(&arr[0], 0x11a8, 0x100, arg, 1, PlayerData_GetCurrent(), 0, *p, 0, 1);
    *out = arr[0];
    if (*out == 0xfff1) {
        ItemPick_FromRange(&arr[1], 0x11a8, 0x100, arg, 1, PlayerData_GetCurrent(), 1, *p, 0, 1);
        *out = arr[1];
    }
}

extern "C" void VillagerRequest_PickShirtForStep(u16 *out, VillagerTalkRequestStartTopics *self, u32 idx) {
    if (idx < 7) {
        RequestShirtPickerFn fn = sRequestShirtPickers[idx];
        if (fn != 0) {
            VillagerData *r6 = self->actor->villagerData;
            u8 *r7 = Villager_GetFashionTaste(r6);
            u16 t = *((VillagerData *)(r6))->getShirt();
            fn(out, r7, &t);
            return;
        }
    }
    *out = 0xfff1;
}

extern "C" void VillagerRequest_PickRandomFossil(u16 *out, PlanErrand *, void *) {
    u16 arr[2];
    ItemPick_FromRange(&arr[0], 0x450c, 0x34, 0, 0, 0, 1, 10, 0, 1);
    ItemPick_FromRange(&arr[1], 0x450c, 0x34, 0, 0, PlayerData_GetCurrent(), 0, 10, 0, 1);
    *out = 0xfff1;
    if (R2(&arr[0])) {
        if (R2(&arr[1]) && (Random_GlobalBelow(10) & 1)) {
            *out = arr[1];
        } else {
            *out = arr[0];
        }
    } else {
        *out = arr[1];
    }
}

extern "C" s32 Item_GetFossilPartIndexInGroup(u16 *p, s32 v) {
    u16 tmp;
    s32 idx;
    u32 i;
    s32 j;
    if (R1(p)) {
        tmp = 0xfff1;
        if (R1(p)) {
            idx = (*p - 0x450c) >> 2;
        } else {
            idx = -1;
        }
        for (i = 0; i < 0x34; i++) {
            tmp = i < 0x34 ? 0x450c + i * 4 : 0x450c;
            if (v == Item_GetFossilGroup(&tmp)) {
                for (j = 0; j < 3; j++) {
                    if (idx == j + (s32)i) return j;
                }
                break;
            }
        }
    }
    return -1;
}

// (returns the part index: the decomp had it void, its caller read the result through a cast)
extern "C" s32 Item_GetFossilPartIndex(u16 *p) {
    return Item_GetFossilPartIndexInGroup(p, Item_GetFossilGroup(p));
}

extern "C" void VillagerRequest_PickFossilOfGroup(u16 *out, PlanErrand *a, void *b) {
    u32 r4 = PlanErrand_GetFossilGroup(a);
    if (r4 == 0x18) {
        r4 = (u8)FossilGroup_PickMissing(((VillagerDataItemView *)(b))->getFurniture(), 10);
        PlanErrand_SetFossilGroup(a, r4);
        VillagerSync_Act3C03(b, r4);
    }
    if (r4 < 0x18) {
        FossilGroup_GetItem(out, r4, 0);
    } else {
        *out = 0xfff1;
    }
}

extern "C" void VillagerRequest_PickFossilForStep(u16 *out, VillagerTalkRequestStartTopics *self, u32 idx) {
    if (idx < 5) {
        RequestFossilPickerFn fn = sRequestFossilPickers[idx];
        if (fn != 0) {
            VillagerData *r6 = self->actor->villagerData;
            fn(out, (VillagerPlanBlock_GetErrand(Villager_GetPlan(r6))), r6);
            return;
        }
    }
    *out = 0xfff1;
}

void VillagerTalkRequestStartTopics::prepareRequestItem() {
    ClockDateTimeWords pr;
    u8 arr[2];
    u16 h1;
    u16 h2;
    s32 r5 = 0;
    pr.words[0] = r5;
    pr.words[1] = r5;
    if ((PlanErrand_GetRecord(planErrand))->isActive() != 0) {
        r5 = PlanErrand_GetStep(planErrand);
    }
    Clock_GetDateTime(&pr.dt);
    switch ((u32)((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 0:
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
        }
        break;
    case 1:
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
        }
        break;
    case 2: {
        if (itemFromPlayer == 0xfff1) {
            VillagerRequest_PickFossilForStep(&h1, this, r5);
            itemFromPlayer = h1;
        }
        if (r5 >= 2) {
            s32 v = PlanErrand_GetFossilGroup(planErrand);
            if (v >= 0x18) {
                if (R1(&itemFromPlayer)) {
                    v = Item_GetFossilGroup(&itemFromPlayer);
                }
            }
            if (v < 0x18) {
                s32 r = FossilGroup_ToIndex(v);
                if (r != -1) {
                    arr[0] = r;
                    arr[1] = 0;
                    ((ActorTalkRequest *)(this))->setSlotFromString(0, arr, "st_fossil");
                }
            }
        }
        break;
    }
    case 3:
        if (itemFromPlayer == 0xfff1) {
            VillagerRequest_PickShirtForStep(&h2, this, r5);
            itemFromPlayer = h2;
        }
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
        }
        break;
    case 4:
        break;
    }
    ((VillagerTalk *)this)->setDeferredFn((Unk_020d8938_Fn)data_020d7ec8);
}

void VillagerTalkRequestStartTopics::openRequestChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x1f, 0x1f, &sRequestTopicsB[4]);
    TalkChoiceTable_Set(this, (&s), 1, 0x20, 0x20, &sRequestTopicsB[5]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ec0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkRequestStartTopics::registerRequestDeferred() {
    registerRequestAccepted();
}

void VillagerTalkRequestStartTopics::selectQStart(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQStart.key, sTalkTopicQStart.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5e;
}

void VillagerTalkRequestStartTopics::registerRequestAccepted() {
    VillagerData *r6 = actor->villagerData;
    s32 r4 = 0;
    s32 r7;
    if ((PlanErrand_GetRecord(planErrand))->isActive() == 0) {
        r7 = ((VillagerTalk *)this)->getErrandRecord()->getKind();
        r4 = 1;
    } else {
        r7 = (PlanErrand_GetRecord(planErrand))->getKind();
    }
    PlanErrand_Assign(planErrand, r7, &itemFromPlayer, PlayerData_GetCurrent()->getPlayerId(), r4);
    VillagerSync_Act3D(r6, r7, &itemFromPlayer, (*(CommManager * *)&gCommManager)->myAid, r4);
    ((ErrandRecord *)(VillagerState_GetErrand(Villager_GetState(r6))))->clear();
}

void VillagerTalkRequestStartTopics::selectQNo(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQNoB.key, sTalkTopicQNoB.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    ((VillagerTalk *)this)->setDeferredFn((Unk_020d8938_Fn)data_020d7eb0);
}

void VillagerTalkRequestStartTopics::registerRequestDeclined() {
    VillagerData *r6 = actor->villagerData;
    s32 r4 = 0;
    s32 r7;
    if ((PlanErrand_GetRecord(planErrand))->isActive() == 0) {
        r7 = ((VillagerTalk *)this)->getErrandRecord()->getKind();
        r4 = 1;
    } else {
        r7 = (PlanErrand_GetRecord(planErrand))->getKind();
    }
    PlanErrand_Assign(planErrand, r7, &itemFromPlayer, 0, r4);
    VillagerSync_Act3D(r6, r7, &itemFromPlayer, 4, r4);
    ((ErrandRecord *)(VillagerState_GetErrand(Villager_GetState(r6))))->clear();
}

void VillagerTalkRequestStartTopics::selectQCon(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQConDefault;
    s32 r6 = PlanErrand_GetStep(planErrand);
    s32 r7 = ((VillagerTalk *)this)->getErrandRecord()->getKind();
    s32 x = 0;
    ((VillagerTalk *)this)->getErrandRecord()->getClassIndex(&x);
    if (r6 < PlanErrand_GetStepCount(r7) && x < 5) {
        d.key = sTalkKeysQCon[x][r6];
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkRequestStartTopics::gotoQ05Talk() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[7]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ05Talk(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQ05TalkDefault;
    s32 r = PlanErrand_GetStep(planErrand);
    if (r < 7) {
        d.key = sTalkKeysQ05Talk[r];
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openCatchPicker() {
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 0) {
        ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickInsect, 0xd, 1);
    } else {
        ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickFish, 0xd, 1);
    }
    ((ActorTalkRequest *)(this))->openSubScene(0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7e30);
}

void VillagerTalkTopics::onCatchPicked() {
    u8 b;
    TalkTopicMsg out;
    s32 r;
    if (MenuCtrl_IsResultOk() == 0) {
        if ((u32 *)window) {
            ((u32 *)window)[2] = 1;
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[11]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
    } else {
        r = MenuCtrl_GetIndex();
        itemFromPlayer = *((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getPocket(r);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 4, 0);
        ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7e28);
    }
}

void VillagerTalkTopics::compareCatchPrice() {
    u8 b;
    u16 h;
    TalkTopicMsg out;
    ClockDateTimeWords s;
    s32 a, c;
    a = 0;
    s.words[0] = 0;
    s.words[1] = 0;
    c = 0;
    Clock_GetDateTime(&s.dt);
    if (itemFromPlayer != 0xfff1) {
        a = Item_GetPrice(&itemFromPlayer);
    }
    h = *PlanErrand_GetShownItem(planErrand);
    if (h != 0xfff1) {
        c = Item_GetPrice(&h);
    }
    if (a == c) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[12]);
    } else if (a > c) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[9]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[11]);
    }
    if (itemFromPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 2, 7);
    }
    if (h != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&h, 3, 7);
    }
    if ((u32 *)window) {
        ((u32 *)window)[2] = 1;
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQPwin(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Pwin;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Pwin;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::takePlayerCatch() {
    u16 h;
    s32 r = MenuCtrl_GetIndex();
    h = 0xfff1;
    Pocket_SetItem(&h, 0, r);
    ((ActorTalkRequest *)(this))->requestKeepItem();
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7a70);
}

void VillagerTalkTopics::gotoQPwin2() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[10]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)((u32 *)window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQPwin2(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Pwin2;
    VillagerData *r7 = actor->villagerData;
    this->pickRequestReward();
    if (itemFromPlayer != 0xfff1) {
        Villager_AddReceivedItem(r7, &itemFromPlayer);
        if (memory != 0 && memoryIndex != (u32)-1) {
            ((VillagerMemory *)((void *)memory))->setReceivedItem((u16 *)(&itemFromPlayer));
            VillagerSync_ReceivedItem(r7, memoryIndex, &itemFromPlayer);
        }
    }
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Pwin2;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, topicIndex, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5f;
}

void VillagerTalkTopics::selectQPlose(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Plose;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Plose;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7e40);
}

void VillagerTalkTopics::cancelRequestChain() {
    ((ActorTalkRequest *)(this))->requestReturnItem();
    PlanErrand_ResetProgress(planErrand);
    VillagerSync_Act3C01(actor->villagerData);
}

void VillagerTalkTopics::selectQPdraw(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Pdraw;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Pdraw;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7a88);
}

void VillagerTalkTopics::checkVillagerCatch() {
    u8 buf[2];
    TalkTopicMsg out;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 0:
        if (Unk_02024df4_Range(PlanErrand_GetShownItem(planErrand), 0x12b0, 0x12e7)) {
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[14]);
            if (selectFn) {
                (this->*selectFn)(&out);
            }
            buf[0] = out.msgIndex;
            ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&buf[0]), (void *)(out.fileName));
        } else {
            ((ActorTalkRequest *)(this))->setPocketItem(Talk_GetPocketMaskForItem(((VillagerTalk *)this)->getErrandRecord()->getItem()), 0xd, 1);
            ((ActorTalkRequest *)(this))->openSubScene(0);
            ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7a90);
        }
        break;
    case 1:
        if (Unk_02024df4_Range(PlanErrand_GetShownItem(planErrand), 0x12e8, 0x131f)) {
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[14]);
            if (selectFn) {
                (this->*selectFn)(&out);
            }
            buf[1] = out.msgIndex;
            ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&buf[1]), (void *)(out.fileName));
        } else {
            ((ActorTalkRequest *)(this))->setPocketItem(Talk_GetPocketMaskForItem(((VillagerTalk *)this)->getErrandRecord()->getItem()), 0xd, 1);
            ((ActorTalkRequest *)(this))->openSubScene(0);
            ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7b20);
        }
        break;
    }
}

void VillagerTalkTopics::onNloseCatchPicked() {
    u8 b;
    u16 h;
    TalkTopicMsg out;
    if (MenuCtrl_IsResultOk() == 0) {
        if ((u32 *)window) {
            ((u32 *)window)[2] = 1;
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[11]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
    } else {
        s32 r = MenuCtrl_GetIndex();
        itemFromPlayer = *((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getPocket(r);
        h = 0xfff1;
        Pocket_SetItem(&h, 0, r);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 4, 0);
        ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7d10);
    }
}

void VillagerTalkTopics::gotoQNlose() {
    u8 b;
    TalkTopicMsg out;
    if ((u32 *)window) {
        ((u32 *)window)[2] = 1;
    }
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[15]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)((u32 *)window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQNwin(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Nwin;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Nwin;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    PlanErrand_ResetProgress(planErrand);
    VillagerSync_Act3C01(actor->villagerData);
}

void VillagerTalkTopics::selectQNlose(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Nlose;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Nlose;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::keepItemThenGotoQPay() {
    ((ActorTalkRequest *)(this))->requestKeepItem();
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7d78);
}

void VillagerTalkTopics::gotoQPay() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[16]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)((u32 *)window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkTopics::selectQPay(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Pay;
    VillagerData *r7 = actor->villagerData;
    this->pickRequestReward();
    if (itemFromPlayer != 0xfff1) {
        Villager_AddReceivedItem(r7, &itemFromPlayer);
        if (memory != 0 && memoryIndex != (u32)-1) {
            ((VillagerMemory *)((void *)memory))->setReceivedItem((u16 *)(&itemFromPlayer));
            VillagerSync_ReceivedItem(r7, memoryIndex, &itemFromPlayer);
        }
    }
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Pay;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5f;
}

void VillagerTalkTopics::gotoQEndOrRevenge() {
    TalkTopicMsg out;
    u8 b;
    if (((u32)PlanErrand_GetStep(planErrand)) >= 4) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[25]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[17]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::handOverRequestReward() {
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7ac0);
}

void VillagerTalkTopics::selectQRevenge(TalkTopicMsg *out) {
    const TalkTopic *d = &sTalkTopicQ01Revenge;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 1) {
        d = &sTalkTopicQ02Revenge;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7a60);
}

void VillagerTalkTopics::advanceRequestStep() {
    VillagerData *r4 = actor->villagerData;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 4) {
        if (Item_IsFurniture(&itemFromPlayer) != 0) {
            u32 v = Villager_GetFurnitureTasteScore(r4, &itemFromPlayer);
            u32 w = PlanErrand_AddCount(planErrand, (u8)v);
            VillagerSync_Act3C05(r4, (u8)w);
        }
    }
    ((VillagerTalk *)this)->getErrandRecord()->setStep(1);
    VillagerSync_Act3C00(r4, 1);
    VillagerState_SetUnk1dBit1(Villager_GetState(r4));
}

void VillagerTalkTopics::openRequestItemPicker() {
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 2:
        if (((u32)PlanErrand_GetStep(planErrand)) <= 1) {
            ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickFossil, 0xd, 1);
        } else {
            ((ActorTalkRequest *)(this))->setPocketItem(Talk_GetWantedFossilPocketMask(planErrand), 0xd, 1);
        }
        ((ActorTalkRequest *)(this))->openSubScene(0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7a58);
        break;
    case 3:
        if (((u32)PlanErrand_GetStep(planErrand)) <= 1) {
            ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickShirt, 0xd, 1);
        } else {
            ((ActorTalkRequest *)(this))->setPocketItem(Talk_GetPocketMaskForItem(((VillagerTalk *)this)->getErrandRecord()->getItem()), 0xd, 1);
        }
        ((ActorTalkRequest *)(this))->openSubScene(0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d79c0);
        break;
    case 4:
        ((ActorTalkRequest *)(this))->setPocketFilter(Talk_FilterPickNonFossilFurniture, 0xd, 1);
        ((ActorTalkRequest *)(this))->openSubScene(0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7ba8);
        break;
    }
}

void VillagerTalkTopics::onRequestItemPicked() {
    TalkTopicMsg out;
    u8 b;
    if (MenuCtrl_IsResultOk() == 0) {
        if ((TalkWindowState *)window) {
            ((TalkWindowState *)window)->nextState = 1;
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[11]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)window)->setNextMessage((u8 *)(&b), (void *)(out.fileName));
    } else {
        u32 r5 = MenuCtrl_GetIndex();
        itemFromPlayer = *((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getPocket(r5);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 4, 0);
        ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7d48);
    }
}

void VillagerTalkTopics::checkRequestItem() {
    TalkTopicMsg out;
    u8 b;
    VillagerData *r4 = actor->villagerData;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 2:
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[21]);
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
        }
        if (((u32)PlanErrand_GetStep(planErrand)) >= 2 && ((u32)PlanErrand_GetStep(planErrand)) <= 4) {
            u32 r6 = Item_GetFossilPartIndex(&itemFromPlayer);
            PlanErrand_SetFlag(planErrand, r6);
            VillagerSync_Act3C04(r4, r6);
        }
        break;
    case 3: {
        if (((u32)PlanErrand_GetStep(planErrand)) >= 2) {
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[21]);
        } else {
            u8 *r7 = ((u8 *)Villager_GetFashionTaste(r4));
            u16 *p = ((VillagerData *)(r4))->getShirt();
            BOOL eq;
            if (Item_IsFurniture(&itemFromPlayer) != 0) {
                s32 a = ((s32)Item_GetFurnitureIndex(&itemFromPlayer));
                if (a == ((s32)Item_GetFurnitureIndex(p))) {
                    eq = TRUE;
                } else {
                    eq = FALSE;
                }
            } else {
                if (itemFromPlayer == *p) {
                    eq = TRUE;
                } else {
                    eq = FALSE;
                }
            }
            if (eq) {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[20]);
            } else if (r7[1] != Item_GetShirtUnkGroup(&itemFromPlayer)) {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[21]);
            } else {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[19]);
            }
        }
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 2, 7);
        }
        break;
    }
    case 4:
        if (Villager_GetFurnitureTasteScore(r4, &itemFromPlayer) > 0) {
            if (Item_IsFurniture(&itemFromPlayer) != 0) {
                s32 v = Villager_CountFurnitureLike(r4, &itemFromPlayer);
                s32 w = Ftr_GetUnk05(&itemFromPlayer);
                if (v >= 2 || (v == 1 && w == 2)) {
                    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[20]);
                } else {
                    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[21]);
                }
            } else {
                ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[19]);
            }
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[19]);
        }
        if (itemFromPlayer != 0xfff1) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&itemFromPlayer, 0, 7);
        }
            break;
    }
    if ((TalkWindowState *)window) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQMiss(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQ04Miss;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 4) {
        d.key = sTalkKeyQ05Miss;
    }
    if ((TalkWindowState *)window) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7b70);
}

void VillagerTalkTopics::giveBackHeldItem() {
    ((ActorTalkRequest *)(this))->requestReturnItem();
}

void VillagerTalkTopics::selectQMissB(TalkTopicMsg *out) {
    u32 r6 = 2;
    const char *r7 = data_020c7a20;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 4) {
        r7 = data_020c7a2c;
        r6 = 3;
    }
    if ((TalkWindowState *)window) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), r7, 3, 1, (u8)r6);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7cd0);
}

void VillagerTalkTopics::selectQThanks(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQThanksDefault;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 2:
        d.key = sTalkKeyQ03Thanks;
        break;
    case 3:
        d.key = sTalkKeyQ04Thanks;
        break;
    default:
        d.key = sTalkKeyQ05Thanks;
        break;
    }
    if ((TalkWindowState *)window) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::takeRequestItem() {
    u32 r2 = MenuCtrl_GetIndex();
    u16 h = 0xfff1;
    Pocket_SetItem(&h, 0, r2);
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 3) {
        ((ActorTalkRequest *)(this))->requestItemAct0F();
        if (Unk_020242d8_R1(((VillagerData *)(actor->villagerData))->getShirt(), 0x11a8, 0x12a7)) {
            itemToPlayer = *((VillagerData *)(actor->villagerData))->getShirt();
        }
    } else {
        ((ActorTalkRequest *)(this))->requestKeepItem();
    }
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7e88);
}

void VillagerTalkTopics::gotoQRewardTopic() {
    TalkTopicMsg out;
    u8 b;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 2) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[23]);
    } else {
        if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 3) {
            ((VillagerTalk *)this)->playOwnerIdleAnim();
        }
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[22]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)window)->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQPreitemB(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQPreitemB.key, sTalkTopicQPreitemB.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkRequestItemTopics::gotoQItemB() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[23]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessageIfUnset(&b, (void *)(out.fileName));
}

void VillagerTalkRequestItemTopics::pickRequestReward() {
    u16 buf[16];
    PlayerData *r4;
    void *r10;
    void *r6;
    void *t;
    s32 r7, r5;
    if (planErrand == 0) {
        return;
    }
    r4 = PlayerData_GetCurrent();
    r10 = r4->getInventory();
    r6 = actor->villagerData;
    t = PlanErrand_GetRecord(planErrand);
    r7 = Random_GlobalBelow(100);
    r5 = PlanErrand_GetStep(planErrand);
    switch (((ErrandRecord *)(t))->getKind()) {
    case 0:
    case 1:
    case 2:
        switch (r5) {
        case 0:
        case 1:
            if (r7 < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x1f4;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[0], price);
                itemToPlayer = buf[0];
            }
            break;
        case 2:
        case 3:
            Villager_GetMostValuableReceivedItem(&buf[1], actor->villagerData);
            itemToPlayer = buf[1];
            if (itemToPlayer != 0xfff1) {
                Villager_RemoveReceivedItem(actor->villagerData, &itemToPlayer);
            } else if (Random_GlobalBelow(100) < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x2bc;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[2], price);
                itemToPlayer = buf[2];
            }
            break;
        default:
            Talk_PickErrandItem(&buf[3], r4);
            itemToPlayer = buf[3];
            break;
        }
        if (itemToPlayer == 0xfff1) {
            Talk_PickErrandItem(&buf[4], 0);
            itemToPlayer = buf[4];
        }
        break;
    case 3:
        switch (r5) {
        case 0:
        case 1:
            Villager_PickRandomReceivedItem(&buf[5], r6);
            itemToPlayer = buf[5];
            if (itemToPlayer != 0xfff1) {
                Villager_RemoveReceivedItem(r6, &itemToPlayer);
            } else if (r7 < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x1f4;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[6], price);
                itemToPlayer = buf[6];
            }
            break;
        case 2:
        case 3:
        case 4:
            Villager_GetMostValuableReceivedItem(&buf[7], r6);
            itemToPlayer = buf[7];
            if (itemToPlayer != 0xfff1) {
                Villager_RemoveReceivedItem(r6, &itemToPlayer);
            } else if (Random_GlobalBelow(100) < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x2bc;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[8], price);
                itemToPlayer = buf[8];
            }
            break;
        default:
            Talk_PickClothingItem(&buf[9], r4);
            itemToPlayer = buf[9];
            break;
        }
        if (itemToPlayer == 0xfff1) {
            Talk_PickClothingItem(&buf[10], 0);
            itemToPlayer = buf[10];
        }
        break;
    case 4:
        switch (r5) {
        case 0:
        case 1:
            if (r7 < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x1f4;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[11], price);
                itemToPlayer = buf[11];
            }
            break;
        case 2:
        case 3:
        case 4:
            Villager_GetMostValuableReceivedItem(&buf[12], r6);
            itemToPlayer = buf[12];
            if (itemToPlayer != 0xfff1) {
                Villager_RemoveReceivedItem(r6, &itemToPlayer);
            } else if (Random_GlobalBelow(100) < 30) {
                price = FengShui_GetWestTotal() * 4 + 0x2bc;
                price = Talk_RoundBells(price);
                Talk_GetMoneyItem(&buf[13], price);
                itemToPlayer = buf[13];
            }
            break;
        default:
            Talk_PickErrandItem(&buf[14], r4);
            itemToPlayer = buf[14];
            break;
        }
        if (itemToPlayer == 0xfff1) {
            Talk_PickErrandItem(&buf[15], 0);
            itemToPlayer = buf[15];
        }
        break;
    }
    if (Unk_020238b0_InRange(&itemToPlayer, 0x1492, 0x14fd)) {
        PlayerInventory_AddBells((u8 *)r10, price, 1);
    } else if (itemToPlayer != 0xfff1) {
        s32 r2 = ((PlayerInventory *)(r10))->findEmptyPocket();
        if (r2 != -1) {
            ((PlayerInventory *)(r10))->setPocket((u16 *)(&itemToPlayer), r2, 0);
            Catalog_SetItem(static_cast<u8 *>(r4->getCatalog()), &itemToPlayer, 0, 1);
        }
    }
    if (Unk_020238b0_InRange(&itemToPlayer, 0x1492, 0x14fd)) {
        ((ActorTalkRequest *)(this))->setNumberNamedSlot(price, 1, 4, 1, 1, 0);
    } else if (itemToPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 1, 7);
    }
}

void VillagerTalkRequestItemTopics::selectQItemB(TalkTopicMsg *out) {
    VillagerData *r6 = actor->villagerData;
    u16 saved = itemFromPlayer;
    if (((VillagerTalk *)this)->getErrandRecord()->getKind() == 3) {
        saved = itemToPlayer;
        itemToPlayer = 0xfff1;
    }
    pickRequestReward();
    if (saved != 0xfff1) {
        Villager_AddReceivedItem(r6, &saved);
    }
    if (itemFromPlayer != 0xfff1 && memory != 0 && memoryIndex != -1) {
        ((VillagerMemory *)(memory))->setReceivedItem((u16 *)(&itemFromPlayer));
        VillagerSync_ReceivedItem(r6, memoryIndex, &itemFromPlayer);
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQItem.key, sTalkTopicQItem.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5f;
}

void VillagerTalkRequestItemTopics::handOverQItemB() {
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7cf8);
}

void VillagerTalkRequestItemTopics::gotoQClearOrEnd() {
    u8 b;
    TalkTopicMsg out;
    s32 n = PlanErrand_GetStepCount(((VillagerTalk *)this)->getErrandRecord()->getKind());
    if (PlanErrand_GetStep(planErrand) >= n - 1) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[25]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[24]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkRequestItemTopics::selectQClear(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQClearDefault;
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 2:
        d.key = data_020c7a44;
        break;
    case 3:
        d.key = data_020c7a50;
        break;
    default:
        d.key = data_020c7a5c;
        break;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = ((Unk_020238b0_Fn)data_020d7970);
}

void VillagerTalkRequestItemTopics::selectQEnd(TalkTopicMsg *out) {
    const TalkTopic *e = sTalkTopicsQEnd;
    if (((VillagerTalk *)this)->getErrandRecord()->getClass() == 0) {
        s32 idx = 0;
        if (((VillagerTalk *)this)->getErrandRecord()->getClassIndex(&idx) != 0) {
            e = &sTalkTopicsQEnd[idx];
        }
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), e->key, e->variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = data_020d7da0;
}

void VillagerTalkRequestItemTopics::finishRequestChain() {
    VillagerData *r4 = actor->villagerData;
    (PlanErrand_GetRecord(planErrand))->setStep(3);
    VillagerSync_Act3C00(actor->villagerData, 3);
    if (gCommManager->isSlotActive(gCommManager->myAid) == 0) {
        Villager_UpdatePlanErrand(r4);
        VillagerState_SetUnk1dBit1(Villager_GetState(r4));
    }
}

void VillagerTalkRequestItemTopics::selectQReturn(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQReturnDefault;
    s32 idx = 0;
    ((VillagerTalk *)this)->getErrandRecord()->getClassIndex(&idx);
    if (idx < 5) {
        d.key = sTalkKeysQReturn[idx];
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
        out->fileName = topicFile;
        out->msgIndex = topicIndex;
    }
}

void VillagerTalkRequestItemTopics::selectQComp(TalkTopicMsg *out) {
    TalkTopic d = sTalkTopicQCompDefault;
    s32 idx = 0;
    ((VillagerTalk *)this)->getErrandRecord()->getClassIndex(&idx);
    if (idx < 5) {
        d.key = sTalkKeysQComp[idx];
    }
    if (d.key != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d.key, d.variantCount, 0, 0);
        out->fileName = topicFile;
        out->msgIndex = topicIndex;
    }
    ((VillagerTalk *)this)->setDeferredFn((Unk_020d8938_Fn)data_020d7c78);
}

void VillagerTalkRequestItemTopics::clearRequest() {
    PlanErrand_AdvanceStep(planErrand);
    VillagerSync_Act3C02(actor->villagerData);
}

extern "C" BOOL Talk_IsArbeitItem(u16 *p, s32 flags) {
    return Unk_020238b0_InRange(p, 0x1561, 0x1564);
}

void VillagerTalkRequestItemTopics::openArbeitItemPicker() {
    ((ActorTalkRequest *)this)->setPocketFilter(Talk_IsArbeitItem, 0xd, 0);
    ((ActorTalkRequest *)(this))->openSubScene(0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7c70);
}

void VillagerTalkTopics::onArbeitItemPicked() {
    u8 b;
    u16 h;
    TalkTopicMsg out;
    if (MenuCtrl_IsResultOk() != 0) {
        s32 t = MenuCtrl_GetIndex();
        itemFromPlayer = *((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getPocket(t);
        h = 0xfff1;
        Pocket_SetItem(&h, 0, t);
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[29]);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsA[11]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectEvArbeitReceive(TalkTopicMsg *out) {
    u16 h[2];
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicEvArbeit.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 0xe: {
        topicIndex = 0xa;
        ItemPickSpec o1;
        o1.set(0, 0);
        ItemPick_One(&h[0], &o1, NULL, NULL, 1, 1, NULL);
        itemToPlayer = h[0];
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 0, 7);
        ((VillagerTalk *)this)->getErrandRecord()->setStep(1);
        break;
    }
    case 0x10: {
        topicIndex = 0xc;
        ItemPickSpec o2;
        o2.set(3, 0);
        ItemPick_One(&h[1], &o2, NULL, NULL, 1, 1, NULL);
        itemToPlayer = h[1];
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 0, 7);
        ((VillagerTalk *)this)->getErrandRecord()->setStep(1);
        break;
    }
    case 0x11:
        if (memory != 0 && ((VillagerData *)(actor->villagerData))->hasLetterFrom((u16 *)(VillagerMemory_GetPlayerId(memory)))) {
            customFn2 = (Unk_020238b0_Fn)(data_020d7a30);
            unk_134 = memory;
            topicIndex = 0xe;
        } else {
            topicIndex = 0x16;
        }
        ((VillagerTalk *)this)->getErrandRecord()->setStep(1);
        break;
    default:
        topicIndex = 0xa;
        break;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::giveArbeitReward() {
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 0xe:
    case 0x10: {
        s32 t = Pocket_FindEmpty();
        if (t >= 0) {
            Pocket_SetItem(&itemToPlayer, 0, t);
            Catalog_SetItem(static_cast<u8 *>(PlayerData_GetCurrent()->getCatalog()), &itemToPlayer, 0, 1);
        }
        ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7c58);
        break;
    }
    case 0x11:
        ((VillagerTalk *)this)->clearTopicFns();
        break;
    }
}

void VillagerTalkTopics::gotoEvArbeitEnd() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[30]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectEvArbeitEnd(TalkTopicMsg *out) {
    Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicEvArbeit.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
    switch (((VillagerTalk *)this)->getErrandRecord()->getKind()) {
    case 0xe:
        topicIndex = 0xb;
        break;
    case 0x10:
        topicIndex = 0xd;
        break;
    default:
        topicIndex = 0xe;
        break;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ12Other(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12Other.key, sTalkTopicQ12Other.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ12Report(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12Report.key, sTalkTopicQ12Report.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ12Thanks(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12Thanks.key, sTalkTopicQ12Thanks.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::checkPocketsForQItem() {
    u8 b;
    TalkTopicMsg out;
    if (Pocket_FindEmpty() != -1) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[34]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[36]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQItem(TalkTopicMsg *out) {
    u16 h[2];
    Talk_PickErrandItem(&h[0], PlayerData_GetCurrent());
    itemToPlayer = h[0];
    if (itemToPlayer == 0xfff1) {
        Talk_PickErrandItem(&h[1], 0);
        itemToPlayer = h[1];
    }
    if (itemToPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 1, 7);
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQItemB.key, sTalkTopicQItemB.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::putQItemInPocket() {
    void *a = PlayerData_GetCurrent();
    void *b = ((PlayerData *)(a))->getInventory();
    s32 c = ((PlayerInventory *)(b))->findEmptyPocket();
    if (c == -1) {
        c = 0;
    }
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
    ((PlayerInventory *)(b))->setPocket((u16 *)(&itemToPlayer), c, 0);
    Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(a))->getCatalog()), &itemToPlayer, 0, 1);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7c38);
}

void VillagerTalkTopics::gotoQ12End() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[35]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ12End(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12End.key, sTalkTopicQ12End.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5f;
    onEndFn = (Unk_020238b0_Fn)(data_020d8010);
}

void VillagerTalkTopics::onQ12EndClose() {
    ((SickVillagerRecord *)(SaveVillagers_GetUnk3830(gSaveVillagers)))->resetRecord();
}

void VillagerTalkTopics::selectQ12Full(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12FullB.key, sTalkTopicQ12FullB.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7998);
}

void VillagerTalkTopics::onQ12FullClose() {
    void *p;
    p = ((SickVillagerRecord *)(SaveVillagers_GetUnk3830(gSaveVillagers)))->getErrand();
    if (((ErrandRecord *)(p))->isActive() != 0) {
        ((ErrandRecord *)(p))->setStep(1);
    }
}

void VillagerTalkTopics::selectQ12FullPart1(TalkTopicMsg *out) {
    u32 v = sTalkTopicQ12FullC.variantCount;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12FullC.key, v, 1, v);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::recheckPocketsForQItem() {
    u8 b;
    TalkTopicMsg out;
    if (Pocket_FindEmpty() != -1) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[34]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[38]);
    }
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectQ12FullPart2(TalkTopicMsg *out) {
    u32 v = sTalkTopicQ12Full.variantCount;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ12Full.key, v, 2, v);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ10Req(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ10Req.key, sTalkTopicQ10Req.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openQ10ReqChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x1f, 0x1f, &sRequestTopicsB[40]);
    TalkChoiceTable_Set(this, (&s), 1, 0x20, 0x20, &sRequestTopicsA[3]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d79e0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectQ10Reserve(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ10Reserve.key, sTalkTopicQ10Reserve.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openReserveTimeEntry() {
    ((ActorTalkRequest *)(this))->setSubSceneKind(0x31, 0);
    ((ActorTalkRequest *)(this))->openSubScene(2);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7be8);
}

void VillagerTalkTopics::checkReserveTime() {
    struct {
        u8 unk_00;
        u8 pad_01[3];
        ClockDateTimeWords a;
        ClockDateTimeWords b;
        TalkTopicMsg o;
    } l;
    l.a.words[0] = 0;
    l.a.words[1] = 0;
    l.b.words[0] = 0;
    l.b.words[1] = 0;
    Clock_GetDateTime(&l.b.dt);
    MI_CpuCopy8(&l.b, &l.a, 8);
    MenuCtrl_GetDateTime(&l.a);
    if (DateTime_DiffMinutes(&l.b.dt, &l.a.dt) <= 30) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[41]);
    } else if (Villager_IsAsleep(actor->villagerData, &l.a) != 0) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[43]);
    } else if (((u8 *)&l)[6] < 6) {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[42]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sRequestTopicsB[44]);
    }
    if (selectFn) {
        (this->*selectFn)(&l.o);
    }
    l.unk_00 = l.o.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&l.unk_00), (void *)(l.o.fileName));
}

void VillagerTalkTopics::selectQError1(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQError1.key, sTalkTopicQError1.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQError2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQError2.key, sTalkTopicQError2.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQError3(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQError3.key, sTalkTopicQError3.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ10Reserved(TalkTopicMsg *out) {
    ClockDateTimeWords s;
    s.words[0] = 0;
    s.words[1] = 0;
    MenuCtrl_GetDateTime(&s);
    this->setVisitTimeArgs((ClockDateTime *)(&s));
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ10Reserved.key, sTalkTopicQ10Reserved.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    pendingSe = 0x5e;
    ((VillagerTalk *)this)->setDeferredFn((Unk_020d8938_Fn)data_020d79d0);
}

void VillagerTalkTopics::storeReservation() {
    u8 *p = (u8 *)PlayerData_GetCurrent()->getErrands();
    ClockDateTimeWords s;
    s.words[0] = 0;
    s.words[1] = 0;
    MenuCtrl_GetDateTime(&s);
    HouseVisitInvite_Set((HouseVisitInvite *)(p + 0x88), (VillagerIdCtorDtor *)actor->villagerData->getVillagerId(), &s);
    ((ErrandRecord *)(VillagerState_GetErrand(Villager_GetState(actor->villagerData))))->clear();
    SaveVillagers_UpdateErrands(gSaveVillagers);
}

void VillagerTalkTopics::selectQ10Con(TalkTopicMsg *out) {
    u8 *p = (u8 *)PlayerData_GetCurrent()->getErrands();
    this->setVisitTimeArgs((ClockDateTime *)(p + 0xa0));
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ10Con.key, sTalkTopicQ10Con.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectQ10Leave(TalkTopicMsg *out) {
    u8 *p = (u8 *)PlayerData_GetCurrent()->getErrands();
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicQ10Leave.key, sTalkTopicQ10Leave.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    HouseVisitInvite_Clear((HouseVisitInvite *)(p + 0x88));
}

void VillagerTalkTopics::selectEtcCancel(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEtcCancel.key, sTalkTopicEtcCancel.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

s32 VillagerTalkTopics::selectTsuInHint() {
    struct {
        u8 buf[2];
        u16 h;
        ClockDateTimeWords t;
        s32 a, b, c;
    } l;
    s32 r = 0;
    l.t.words[0] = 0;
    l.t.words[1] = 0;
    l.a = 5;
    l.h = 0xfff1;
    l.b = 0;
    l.c = 0;
    Clock_GetDateTime(&l.t.dt);
    if (InsectPick_PickWeightedRarity(&l.h, &l.a, &l.b, &l.c, &l.t.dt) == 1) {
        if (l.a >= 3) {
            r = 1;
        }
        r = r * 3 + InsectPick_GetHintVariant(l.b);
        ((ActorTalkRequest *)(this))->setItemNameSlot(&l.h, 0, 7);
        if (l.c >= 3) {
            l.c--;
        }
        l.buf[0] = l.c;
        l.buf[1] = 0;
        ((ActorTalkRequest *)(this))->setSlotFromString(3, l.buf, "st_insect_time");
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuInHint.key, sTalkTopicTsuInHint.variantCount, r, 0);
        r = 1;
    }
    return r;
}

void VillagerTalkTopics::setFishTimeSlot(s32 a, u16 *p, s32 c, FishSpawnRow **arr, s32 last) {
    FishSpawnRow **pp = arr;
    s32 idx;
    s32 x = (u8)a;
    if (pp) {
        BOOL ok = FALSE;
        u16 v = *p;
        if (v >= 0x12e8 && v <= 0x131f) {
            ok = TRUE;
        }
        if (ok && c <= 1) {
            FishSpawnRow *e;
            FishSpawnEntry *rec;
            s32 i, j, k;
            if (v >= 0x12e8 && v <= 0x131f) {
                idx = v - 0x12e8;
            } else {
                idx = -1;
            }
            i = 0;
            j = 0;
            k = 0;
            for (; i < 3; pp++, i++) {
                e = *pp;
                if (e) {
                    for (j = 0; j < 2; e++, j++) {
                        rec = e->entries;
                        if (rec) {
                            for (k = 0; k < e->count; rec++, k++) {
                                if (rec->fish == idx && FishSpawn_GetWeightClass(rec->weight) <= 1) {
                                    break;
                                }
                            }
                            if (e->count < k) {
                                break;
                            }
                        }
                    }
                    if (j == 2) {
                        break;
                    }
                }
            }
            if (i == 3) {
                x = 3;
            }
        }
    }
    u8 buf[2];
    buf[0] = x;
    buf[1] = 0;
    ((ActorTalkRequest *)(this))->setSlotFromString(last, buf, "st_fish_time");
}

extern "C" s32 Talk_GetFishHintTimeVariant(s32 v) {
    s32 r = 0;
    switch (v) {
    case 0:
    case 2:
    case 4:
        r = 0;
        break;
    case 5:
    case 6:
        r = 1;
        break;
    case 1:
    case 3:
        r = 2;
        break;
    }
    return r;
}

s32 VillagerTalkTopics::selectTsuFiHint() {
    struct {
        u16 unk_00;
        u16 pad_02;
        ClockDateTimeWords t;
        s32 a, b, c;
    } l;
    s32 r;
    FishSpawnTable *q;
    r = 0;
    l.t.words[0] = 0;
    l.t.words[1] = 0;
    l.a = 5;
    l.unk_00 = 0xfff1;
    l.b = 0;
    l.c = 0;
    Clock_GetDateTime(&l.t.dt);
    u32 x = l.t.dt.month;
    q = (FishSpawnTable *)FishTable_GetForDate(x, FishTable_IsLateMonth(l.t.dt.day));
    if (q && FishPick_PickWeightedRarity((u16 *)(&l), &l.a, &l.b, &l.c, q) == 1) {
        r = (l.a >= 3 ? 1 : r) * 3 + Talk_GetFishHintTimeVariant(l.b);
        ((ActorTalkRequest *)(this))->setItemNameSlot((u16 *)&l, 0, 7);
        setFishTimeSlot(l.c, &l.unk_00, l.a, *(FishSpawnRow ***)q, 1);
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFiHint.key, sTalkTopicTsuFiHint.variantCount, r, 0);
        r = 1;
    }
    return r;
}

BOOL VillagerTalkTopics::selectTsuFoHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFoHint.key, sTalkTopicTsuFoHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuClHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuClHint.key, sTalkTopicTsuClHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuFuHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFuHint.key, sTalkTopicTsuFuHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuFlHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFlHint.key, sTalkTopicTsuFlHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuSeHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuSeHint.key, sTalkTopicTsuSeHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuNoHint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuNoHint.key, sTalkTopicTsuNoHint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuHobbyHint() {
    static BOOL (VillagerTalkTopics::*tbl[8])() = {
        data_020d7f50, data_020d7f40, data_020d7f30,
        data_020d7f28, data_020d7f20, data_020d7f08,
        data_020d7ef8, data_020d7ef0};
    s32 idx = 8;
    if (Unk_02021ef8_IsZero(gFieldSceneKind)) {
        if (actor->villagerData != 0) {
            idx = (VillagerPlanBlock_GetPlan(Villager_GetPlan(actor->villagerData)))->getState();
        }
    }
    if (Trend_IsValid(idx) != 0) {
        return (this->*tbl[idx])();
    }
    return FALSE;
}

extern "C" {
const TalkTopic sTalkTopicAi7days = {sTalkKeyAi7days, 0x6};
VillagerTalkTopicFns sConnectTopic[1];
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ab8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicQ06Get = {sTalkKeyQ06Get, 0x3};
const TalkTopic sTalkTopicApMail = {sTalkKeyApMail, 0x2};
const TalkTopic sTalkTopicQ07Scold = {sTalkKeyQ07Scold, 0x3};
decltype(&VillagerTalkTopics::endEtcPushHit) data_020d7958 = &VillagerTalkTopics::endEtcPushHit;
const TalkTopic sTalkTopicQ10Req = {sTalkKeyQ10Req, 0x3};
decltype(&VillagerTalkAcornTopics::onAcornPicked) data_020d7968 = &VillagerTalkAcornTopics::onAcornPicked;
const TalkTopic sTalkTopicApBuy = {sTalkKeyApBuy, 0x3};
decltype(&VillagerTalkTopics::onDeliveryItemPicked) data_020d7948 = &VillagerTalkTopics::onDeliveryItemPicked;
char sTalkKeyTsuDrama[10] = "tsu_drama";
char sTalkKeyQ01Req245[13] = "q01_req2_4_5";
char sTalkKeyTsuClHint[12] = "tsu_cl_hint";
char sTalkKeyTsuEvent2[11] = "tsu_event2";
char sTalkKeyQError3[9] = "q_error3";
decltype(&VillagerTalkTopics::selectApItem) data_020d7918 = &VillagerTalkTopics::selectApItem;
char sTalkKeyQ06Open1[10] = "q06_open1";
decltype(&VillagerTalk::attrShowLetter) data_020d7c50 = &VillagerTalk::attrShowLetter;
PresentOpinionTopicList sPresentOpinionTopics = {{0x17, 0x18, 0x19}};
const TalkTopic sTalkTopicTsuFiHint = {sTalkKeyTsuFiHint, 0x1};
const char *sTalkKeysQ05Req[7] = {
    sTalkKeyQ05Req, sTalkKeyQ05Req, sTalkKeyQ05Req, sTalkKeyQ05Req, sTalkKeyQ05Req, sTalkKeyQ05Req, sTalkKeyQ05Req,
};
char sTalkKeyTsuStar[9] = "tsu_star";
const TalkTopic sTalkTopicAiSad = {sTalkKeyAiSad, 0x5};
u8 sFishRarityWork[8];
static const char *const lit_020d785c = "";
const char *sTalkKeysQ02Req[5] = {
    sTalkKeyQ02Req1, sTalkKeyQ02Req245, sTalkKeyQ02Req3, sTalkKeyQ02Req245, sTalkKeyQ02Req245,
};
char sTalkKeyAiBoom[8] = "ai_boom";
char sTalkKeyQ06Get[8] = "q06_get";
char sTalkKeyQ07Scold2[11] = "q07_scold2";
char sTalkKeyQ07Read[9] = "q07_read";
decltype(&VillagerTalkRequestReplyTopics::cancelRequest) data_020d7c28 = &VillagerTalkRequestReplyTopics::cancelRequest;
decltype(&VillagerTalkTopics::onQ12EndClose) data_020d8010 = &VillagerTalkTopics::onQ12EndClose;
decltype(&VillagerTalk::playOwnerIdleAnim) data_020d8008 = &VillagerTalk::playOwnerIdleAnim;
char sTalkKeyQTimeover[11] = "q_timeover";
char sTalkKeyQNo[5] = "q_no";
const TalkTopic sTalkTopicsAiFirst[3] = {
    {sTalkKeyAiFirst, 0x3}, {sTalkKeyAiNfirst, 0x3}, {sTalkKeyAiMfirst, 0x6},
};
decltype(&VillagerTalkRequestReplyTopics::tryAddSickVillagerChoice) data_020d8000 = &VillagerTalkRequestReplyTopics::tryAddSickVillagerChoice;
char sTalkKeyEtcCancel[11] = "etc_cancel";
char sTalkKeyQ06Report[11] = "q06_report";
u8 sTsuTopicWeightsWork[14];
const TalkTopic sTalkTopicQ10Con = {sTalkKeyQ10Con, 0x3};
const TalkTopic sTalkTopicQ07Open2 = {sTalkKeyQ07Open2, 0x3};
char sTalkKeyTsuSeHint[12] = "tsu_se_hint";
const char *sTalkKeysQ03Req[5] = {
    sTalkKeyQ03Req12, sTalkKeyQ03Req12, sTalkKeyQ03Req3, sTalkKeyQ03Req45, sTalkKeyQ03Req45,
};
char sTalkKeyQ06Open3[10] = "q06_open3";
char sTalkKeyQ01Req1[9] = "q01_req1";
decltype(&VillagerTalkTopics::findTsuEvent2Message) data_020d7c00 = &VillagerTalkTopics::findTsuEvent2Message;
const TalkTopic sTalkTopicQ07Show = {sTalkKeyQ07Show, 0x3};
char sTalkKeyQ07Open2[10] = "q07_open2";
VillagerTalkTopicFns sEvGardeniingTopicTable[4];
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7fd0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkTopics::closeLetter) data_020d7fc8 = &VillagerTalkTopics::closeLetter;
const TalkTopic sTalkTopicQ06Fin = {sTalkKeyQ06Fin, 0x3};
const s8 sCatchPlanStageMinutes[5] = {
    -1, 5, 2, 5, 5,
};
char sTalkKeyQ07Fin[8] = "q07_fin";
char sTalkKeyQ06Con[8] = "q06_con";
char sTalkKeyQ01Req3[9] = "q01_req3";
char sTalkKeyQ07Report[11] = "q07_report";
decltype(&VillagerTalk::setHiraganaOn) data_020d7bd0 = &VillagerTalk::setHiraganaOn;
const TalkTopic sTalkTopicQ07Fin = {sTalkKeyQ07Fin, 0x3};
decltype(&VillagerTalkTopics::selectTsuGhint) data_020d7f90 = &VillagerTalkTopics::selectTsuGhint;
char sTalkKeyQ06Bad[8] = "q06_bad";
char sTalkKeyQ06Normal[11] = "q06_normal";
decltype(&VillagerTalkTopics::selectTsuDress) data_020d7f80 = &VillagerTalkTopics::selectTsuDress;
char sTalkKeyEvSnowfes[11] = "ev_snowfes";
const TalkTopic sTalkTopicQ07Con = {sTalkKeyQ07Con, 0x3};
const TalkTopic sTalkTopicQ06Report = {sTalkKeyQ06Report, 0x3};
char sTalkKeyQ03Req3[9] = "q03_req3";
char sTalkKeyQ07Con[8] = "q07_con";
char sTalkKeyQ02Req245[13] = "q02_req2_4_5";
const TalkTopic sTalkTopicQ07Report = {sTalkKeyQ07Report, 0x3};
const TalkTopic sTalkTopicTsuClHint = {sTalkKeyTsuClHint, 0x3};
char sTalkKeyQ01Con3[9] = "q01_con3";
char sTalkKeyTsuMove1[10] = "tsu_move1";
char sTalkKeyQ03Req12[11] = "q03_req1_2";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f60 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicQ06Bad = {sTalkKeyQ06Bad, 0x3};
const TalkTopic sTalkTopicQItemC = {sTalkKeyQItem, 0x3};
u8 sVillagerModelPathBuf[32];
const char *sTalkKeysQ04Con[7] = {
    sTalkKeyQ04Con12, sTalkKeyQ04Con12, sTalkKeyQ04Con37, sTalkKeyQ04Con37, sTalkKeyQ04Con37, sTalkKeyQ04Con37, sTalkKeyQ04Con37,
};
char sTalkKeyTsuInAct[11] = "tsu_in_act";
decltype(&VillagerTalkRumorTopics::selectTsuFriend) data_020d7f48 = &VillagerTalkRumorTopics::selectTsuFriend;
decltype(&VillagerTalkTopics::selectTsuAlways) data_020d7b88 = &VillagerTalkTopics::selectTsuAlways;
decltype(&VillagerTalk::showMoneyItem) data_020d7f38 = &VillagerTalk::showMoneyItem;
char sTalkKeyQ07End[8] = "q07_end";
VillagerTalkTopicFns sHouseVisitTsuTopicTable[3];
const TalkTopic sTalkTopicAiSnow2 = {sTalkKeyAiSnow2, 0x6};
decltype(&VillagerTalkTopics::selectTsuFuHint) data_020d7f20 = &VillagerTalkTopics::selectTsuFuHint;
const TalkTopic sTalkTopicTsuInAct = {sTalkKeyTsuInAct, 0x4};
const TalkTopic sTalkTopicTsuNoHint = {sTalkKeyTsuNoHint, 0x3};
decltype(&VillagerTalkRumorTopics::selectTsuHappyroom) data_020d7b78 = &VillagerTalkRumorTopics::selectTsuHappyroom;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f18 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicEvSnowfes = {sTalkKeyEvSnowfes, 0x3};
char sTalkKeyQ01Pwin[9] = "q01_pwin";
char sTalkKeyTsuFiAct[11] = "tsu_fi_act";
decltype(&VillagerTalkTopics::selectTsuFlHint) data_020d7f08 = &VillagerTalkTopics::selectTsuFlHint;
char sTalkKeyQ05Req[8] = "q05_req";
char sTalkKeyQ02Pwin[9] = "q02_pwin";
char sTalkKeyTsuFoAct[11] = "tsu_fo_act";
decltype(&VillagerTalkTopics::selectTsuNoHint) data_020d7ef0 = &VillagerTalkTopics::selectTsuNoHint;
char sTalkKeyAiFirst[9] = "ai_first";
decltype(&VillagerTalkTopics::selectTsuFuAct) data_020d7ee0 = &VillagerTalkTopics::selectTsuFuAct;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ed8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicQNoB = {sTalkKeyQNo, 0x3};
const TalkTopic sTalkTopicTsuFiAct = {sTalkKeyTsuFiAct, 0x4};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ed0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ01Plose[10] = "q01_plose";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ec0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ02Plose[10] = "q02_plose";
char sTalkKeyQStart[8] = "q_start";
decltype(&VillagerTalkRequestStartTopics::registerRequestDeclined) data_020d7eb0 = &VillagerTalkRequestStartTopics::registerRequestDeclined;
decltype(&VillagerTalkTopics::selectTsuDrama) data_020d7ea8 = &VillagerTalkTopics::selectTsuDrama;
decltype(&VillagerTalkTopics::selectTsuStar) data_020d7ea0 = &VillagerTalkTopics::selectTsuStar;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7e98 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ05Talk35[12] = "q05_talk3_5";
u8 sDeliveryMinutes[3] = {
    0x0a, 0x1e, 0x3c,
};
const TalkTopic sTalkTopicQ01Nwin = {sTalkKeyQ01Nwin, 0x3};
decltype(&VillagerMood::updateMood1Effects) data_020d7e80 = &VillagerMood::updateMood1Effects;
const TalkTopic sTalkTopicTsuFoAct = {sTalkKeyTsuFoAct, 0x4};
char sTalkKeyEvGardeniing[14] = "ev_gardeniing";
const char *sTalkKeysQ05Con[7] = {
    sTalkKeyQ05Con, sTalkKeyQ05Con, sTalkKeyQ05Con, sTalkKeyQ05Con, sTalkKeyQ05Con, sTalkKeyQ05Con, sTalkKeyQ05Con,
};
const TalkTopic sTalkTopicEvBirth = {sTalkKeyEvBirth, 0x1};
const TalkTopic sTalkTopicAi30days = {sTalkKeyAi30days, 0x6};
const TalkTopic sTalkTopicQ01Revenge = {sTalkKeyQ01Revenge, 0x3};
const TalkTopic sTalkTopicTsuSeAct = {sTalkKeyTsuSeAct, 0x4};
const TalkTopic sTalkTopicAiForeign = {sTalkKeyAiForeign, 0x6};
TalkTopic sTalkTopicQReturnDefault = {NULL, 0x00000003};
char sTalkKeyQ01Pdraw[10] = "q01_pdraw";
decltype(&VillagerTalkRumorTopics::pickMemoryAny) data_020d7e70 = &VillagerTalkRumorTopics::pickMemoryAny;
const TalkTopic sTalkTopicQ02Pwin = {sTalkKeyQ02Pwin, 0x2};
const u8 sTsuDramaMsgBlocks[4] = {
    0x03, 0x00, 0x02, 0x01,
};
const TalkTopic sTalkTopicEvAdmire = {sTalkKeyEvAdmire, 0x2};
char sTalkKeyAi7days[9] = "ai_7days";
char sTalkKeyQ03Con45[11] = "q03_con4_5";
char sTalkKeyQTime[7] = "q_time";
char sTalkKeyQ05Talk67[12] = "q05_talk6_7";
decltype(&VillagerTalkRumorTopics::pickMemoryWithTime) data_020d7e58 = &VillagerTalkRumorTopics::pickMemoryWithTime;
char sTalkKeyAiShop2[9] = "ai_shop2";
char sTalkKeyAiFlea[8] = "ai_flea";
char sTalkKeyQ04Con37[11] = "q04_con3_7";
VillagerTalkTopicFns sEvSnowfesTopicTable[4];
char sTalkKeyAiPassword[12] = "ai_password";
decltype(&VillagerTalkTopics::onCatchPicked) data_020d7e30 = &VillagerTalkTopics::onCatchPicked;
const TalkTopic sTalkTopicQ01Pwin = {sTalkKeyQ01Pwin, 0x2};
const TalkTopic sTalkTopicTsuDress = {sTalkKeyTsuDress, 0x1};
char sTalkKeyTsuFlAct[11] = "tsu_fl_act";
decltype(&VillagerTalk::receiveItemB) data_020d7b90 = &VillagerTalk::receiveItemB;
const TalkTopic sTalkTopicTsuAlways = {sTalkKeyTsuAlways, 0x14};
char sTalkKeyQ03Con3[9] = "q03_con3";
const TalkTopic sTalkTopicQ07Scold2 = {sTalkKeyQ07Scold2, 0x3};
char sTalkKeyEvKaraoke[11] = "ev_karaoke";
const u32 sTalkClothingItemSpecs[8] = {
    0x00000002, 0x00000000, 0x00000005, 0x0000001d, 0x00000006, 0x0000001d, 0x00000008, 0x0000001d,
};
decltype(&VillagerTalkTopics::tryAddHandOverChoice) data_020d7c60 = &VillagerTalkTopics::tryAddHandOverChoice;
const char data_020c7a50[12] = "q04_clear";
char sTalkKey3pBo[6] = "3p_bo";
char sTalkKeyEtcHit[8] = "etc_hit";
}

void VillagerTalkTopics::selectTsuActMessage(s32 r1, const TalkTopic *d) {
    s32 r6 = r1 != (VillagerPlanBlock_GetPlan(Villager_GetPlan(actor->villagerData)))->getState() ? 1 : 0;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), d->key, d->variantCount, r6, 0);
}

BOOL VillagerTalkTopics::selectTsuInAct() {
    s32 r4 = -1;
    u16 h[2];
    Npc_GetStateHeldItem(&h[1], actor);
    if (Unk_02021d50_R(&h[1], 0x1376, 0x1376)) {
        r4 = 0;
        if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(actor->villagerData)))->getState() != 0) {
            r4 = 2;
        }
    }
    if (r4 != -1) {
        h[0] = 0xfff1;
        InsectPick_PickNow(&h[0]);
        if (Unk_02021d50_R(&h[0], 0x12b0, 0x12e7)) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&h[0], 0, 7);
            Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuInAct.key, sTalkTopicTsuInAct.variantCount, r4, 0);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuFiAct() {
    s32 r4 = -1;
    u16 h[2];
    Npc_GetStateHeldItem(&h[1], actor);
    if (Unk_02021d50_R(&h[1], 0x1374, 0x1374)) {
        r4 = 0;
        if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(actor->villagerData)))->getState() != 1) {
            r4 = 1;
        }
    }
    if (r4 != -1) {
        h[0] = 0xfff1;
        FishPick_PickNow(&h[0]);
        if (Unk_02021d50_R(&h[0], 0x12e8, 0x131f)) {
            ((ActorTalkRequest *)(this))->setItemNameSlot(&h[0], 0, 7);
            Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFiAct.key, sTalkTopicTsuFiAct.variantCount, r4, 0);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuFoAct() {
    u16 h[4];
    Npc_GetStateHeldItem(h, actor);
    if (Unk_02021d50_R(h, 0x1369, 0x1369)) {
        selectTsuActMessage(2, &sTalkTopicTsuFoAct);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuClAct() {
    u16 h;
    Npc_GetStateHeldItem(&h, actor);
    volatile u16 *p = &h;
    u32 a = *p;
    u32 b = *p;
    if (b == 0xfff1 || (a >= 0x1380 && a <= 0x139f)) {
        selectTsuActMessage(3, &sTalkTopicTsuClAct);
        this->setFashionArg(5, actor->villagerData);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuFuAct() {
    u16 h;
    Npc_GetStateHeldItem(&h, actor);
    volatile u16 *p = &h;
    u32 a = *p;
    u32 b = *p;
    if (b == 0xfff1 || (a >= 0x1380 && a <= 0x139f)) {
        selectTsuActMessage(4, &sTalkTopicTsuFuAct);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuFlAct() {
    s32 r4 = -1;
    u16 h;
    Npc_GetStateHeldItem(&h, actor);
    if (Unk_02021d50_R(&h, 0x1378, 0x1378)) {
        r4 = 0;
        if ((VillagerPlanBlock_GetPlan(Villager_GetPlan(actor->villagerData)))->getState() != 5) {
            r4 = 2;
        }
    }
    if (r4 != -1) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFlAct.key, sTalkTopicTsuFlAct.variantCount, r4, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuSeAct() {
    u16 h;
    BlockMap *r5 = gSceneBlockMap;
    BOOL r4;
    Npc_GetStateHeldItem(&h, actor);
    r4 = FALSE;
    if (r5 != 0) {
        VecFx32 *v = (&actor->position);
        s32 px = v->x >> 17;
        s32 pz = v->z >> 17;
        if (BlockMap_BlockHasAllAttr(r5, px, pz, 8) != 0) {
            r4 = TRUE;
        }
    }
    if (r4 != 0) {
        volatile u16 *p = &h;
    u32 a = *p;
    u32 b = *p;
    if (b == 0xfff1 || (a >= 0x1380 && a <= 0x139f)) {
            selectTsuActMessage(6, &sTalkTopicTsuSeAct);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuNoAct() {
    u16 h;
    Npc_GetStateHeldItem(&h, actor);
    volatile u16 *p = &h;
    u32 a = *p;
    u32 b = *p;
    if (b == 0xfff1 || (a >= 0x1380 && a <= 0x139f)) {
        selectTsuActMessage(7, &sTalkTopicTsuNoAct);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuHobbyAct() {
    static BOOL (VillagerTalkTopics::*tbl[8])() = {
        data_020d7e18, data_020d7af8, data_020d7e20,
        data_020d7e00, data_020d7ee0, data_020d7b58,
        data_020d7c08, data_020d7dd8};
    s32 idx;
    if (Unk_02021ef8_IsZero(gFieldSceneKind)) {
        idx = VillagerState_GetActivity(Villager_GetState(actor->villagerData));
        if (Trend_IsValid(idx) != 0) {
            return (this->*tbl[idx])();
        }
    }
    return FALSE;
}

extern "C" {
const TalkTopic sTalkTopicQ07Mailgo = {sTalkKeyQ07Mailgo, 0x3};
const TalkTopic sTalkTopicAiShop2 = {sTalkKeyAiShop2, 0x4};
const TalkTopic sTalkTopicQ10Reserved = {sTalkKeyQ10Reserved, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7ad8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQFull[7] = "q_full";
char sTalkKeyQ04End[8] = "q04_end";
const TalkTopic sTalkTopicTsuMemory = {sTalkKeyTsuMemory, 0x1};
const TalkTopic sTalkTopicQ12Other = {sTalkKeyQ12Other, 0x3};
const TalkTopic sTalkTopicEvKaraoke = {sTalkKeyEvKaraoke, 0x3};
const u8 sInsectHabitatKinds[4] = {
    0x07, 0x0d, 0x01, 0x02,
};
char sTalkKeyQ03End[8] = "q03_end";
char sTalkKeyQ01Nlose[10] = "q01_nlose";
decltype(&VillagerTalkTopics::gotoQRewardTopic) data_020d7e88 = &VillagerTalkTopics::gotoQRewardTopic;
u32 sTalkInputBuffer[8];
decltype(&VillagerTalkTopics::endAiFall) data_020d7990 = &VillagerTalkTopics::endAiFall;
const TalkTopic sTalkTopicQ01Nlose = {sTalkKeyQ01Nlose, 0x3};
char sTalkKeyAiSnow2[9] = "ai_snow2";
const u8 data_020c7530[6] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
};
const char *sTalkKeysQ03Con[5] = {
    sTalkKeyQ03Con12, sTalkKeyQ03Con12, sTalkKeyQ03Con3, sTalkKeyQ03Con45, sTalkKeyQ03Con45,
};
char sTalkKeyApHabit[9] = "ap_habit";
const TalkTopic sTalkTopicQTime = {sTalkKeyQTime, 0x1};
decltype(&VillagerTalk::runCustomFn2) data_020d7d50 = &VillagerTalk::runCustomFn2;
const TalkTopic sTalkTopicTsuEvent1 = {sTalkKeyTsuEvent1, 0x2};
char sTalkKeyApNickn[9] = "ap_nickn";
const char **sTalkKeysQCon[5] = {
    sTalkKeysQ01Con, sTalkKeysQ02Con, sTalkKeysQ03Con, sTalkKeysQ04Con, sTalkKeysQ05Con,
};
const TalkTopic sTalkTopicAiFortune = {sTalkKeyAiFortune, 0x3};
const TalkTopic sTalkTopicQYes = {sTalkKeyQYes, 0x3};
char sTalkKeyQ02Revenge[12] = "q02_revenge";
const RequestFossilPickerFn sRequestFossilPickers[5] = {
    VillagerRequest_PickRandomFossil, VillagerRequest_PickRandomFossil, VillagerRequest_PickFossilOfGroup, NULL,
    NULL,
};
const TalkTopic sTalkTopicQPreitemB = {sTalkKeyQPreitem, 0x3};
decltype(&VillagerTalk::sellItemToPlayer) data_020d7d30 = &VillagerTalk::sellItemToPlayer;
char sTalkKeyQ05Thanks[11] = "q05_thanks";
const u32 sApPocketItemKinds[5] = {
    0x0000000d, 0x0000000e, 0x00000039, 0x00000005, 0x00000038,
};
char sTalkKey3pFu[6] = "3p_fu";
char sTalkKeyAiToday2[10] = "ai_today2";
const TalkTopic sTalkTopicQ07Joy = {sTalkKeyQ07Joy, 0x3};
char sTalkKeyQ02Con245[13] = "q02_con2_4_5";
char sTalkKeyQ01Comp[9] = "q01_comp";
decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d7ba8 = &VillagerTalkTopics::onRequestItemPicked;
decltype(&VillagerTalk::runCustomFn0) data_020d7a20 = &VillagerTalk::runCustomFn0;
decltype(&VillagerMood::updateMood2Effects) data_020d7e08 = &VillagerMood::updateMood2Effects;
char sTalkKeyQ02Return[11] = "q02_return";
RumorVillagerPair data_020d7ca8 = {{-1}, {-1}};
const TalkTopic sTalkTopicQ01Pdraw = {sTalkKeyQ01Pdraw, 0x2};
decltype(&VillagerTalkTopics::giveEvBirthPresent) data_020d7d20 = &VillagerTalkTopics::giveEvBirthPresent;
char sTalkKeyQ05End[8] = "q05_end";
}

BOOL VillagerTalkTopics::selectTsuGhint() {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuGhint.key, sTalkTopicTsuGhint.variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkTopics::selectTsuDress() {
    VillagerData *r7 = actor->villagerData;
    volatile u16 h = *(u16 *)((void *)((VillagerData *)(r7))->getShirt());
    s32 r4 = -1;
    u16 *r5;
    Pattern *t;
    u16 *q;
    if (PlayerData_GetCurrent() != 0) {
        r5 = (u16 *)PlayerData_GetCurrent()->getPlayerId();
    } else {
        r5 = 0;
    }
    if (r5 != 0) {
        if (Unk_02021d50_R((u16 *)&h, 0x12a8, 0x12af)) {
            t = r7->getPattern();
            q = (u16 *)t->getInfo()->getAuthor();  // (compared as halfwords)
            if (r5[0] == q[0] && memcmp(r5 + 1, q + 1, 8) == 0 && ((PlayerId *)(r5))->equals((PlayerId *)(q)) != 0) {
                r4 = 1;
            } else if (((PlayerId *)(r5))->equals((PlayerId *)(q)) == 0) {
                TownId *p = (TownId *)gSaveTownId;
                TownId *w = (TownId *)PlayerId_GetTownId(q);
                if (w->townId == p->townId && memcmp(w->townName, p->townName, 8) == 0) {
                    r4 = 2;
                } else {
                    r4 = 3;
                }
            }
            if (r4 != -1) {
                ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)(t->getInfo())->getAuthor(), 0);
                ((ActorTalkRequest *)this)->setTownNameSlot(PlayerId_GetTownId((t->getInfo())->getAuthor()), 1);
            }
        } else {
            r4 = 0;
        }
    }
    if (r4 != -1) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuDress.key, sTalkTopicTsuDress.variantCount, r4, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectTsuAlways() {
    s32 i;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuAlways.key, sTalkTopicTsuAlways.variantCount, 0, 0);
    for (i = 0; i < 6; i++) {
        this->setStringTableArg(i, data_020c7530[i]);
    }
    return TRUE;
}

BOOL VillagerTalkRumorTopics::selectTsuHappyroom() {
    VillagerState *p;
    VillagerData *ctx = actor->villagerData;
    if (ctx != NULL) {
        p = Villager_GetState(ctx);
    } else {
        p = NULL;
    }
    u8 mask = 0;
    u8 cnt = 0;
    if (p != NULL && p->roomScore >= 0) {
        s32 h = p->roomScore;
        u32 v = p->roomBonusFlags;
        if ((v & 1) != 0 || (v & 2) != 0 || (v & 4) != 0 || (v & 8) != 0 || (v & 0x10) != 0) {
            mask |= 1;
            cnt++;
        }
        if ((v & 0x20) != 0) {
            mask |= 2;
            cnt++;
        }
        if ((v & 0x40) != 0) {
            mask |= 4;
            cnt++;
        }
        if ((v & 0x80) != 0) {
            mask |= 8;
            cnt++;
        }
        if ((v & 0x100) != 0) {
            mask |= 0x10;
            cnt++;
        }
        if ((v & 0x200) != 0) {
            mask |= 0x20;
            cnt++;
        }
        if ((v & 0x400) != 0) {
            mask |= 0x40;
            cnt++;
        }
        u32 k = ((u32)Random_PickSetBit(mask, cnt, 7));
        if (k >= 7) {
            k = 7;
        }
        Talk_SelectTopicMessage(this, topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuHappyroom.key, sTalkTopicTsuHappyroom.variantCount, k, 0);
        ((ActorTalkRequest *)(this))->setNumberSlot(h, 0, 6, 1, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkRumorTopics::selectTsuFriend() {
    s32 x, y;
    void **pa;
    char c[2];
    void *a[2];
    s32 j, i;
    s32 sel;
    func_02133ef8(a, 8);
    RumorVillagerPair b = data_020d7ca8;
    c[0] = data_020d7860[0];
    c[1] = data_020d7860[1];
    VillagerId *d[2];
    func_02133ef8(d, sizeof(d));
    x = 0;
    y = 0;
    d[0] = (actor->villagerData->getVillagerId());
    j = 1;
    for (i = 0; i < 2; i++) {
        pa = &a[i];
        a[i] = SaveVillagers_PickRandomExcept(gSaveVillagers, d, j);
        if (a[i] != NULL) {
            (&b.a)[i].v = SaveVillagers_FindIndex(gSaveVillagers, (((VillagerData *)(a[i]))->getVillagerId()));
            c[i] = (((VillagerData *)(a[i]))->getVillagerId())->getGender();
        }
        if (j < 2) {
            if (*pa != NULL) {
                d[j] = (((VillagerData *)(*pa))->getVillagerId());
                j++;
            }
        }
    }
    {
    MsgString9B o;
    RumorVillagerPair e = data_020d7cb8;
    sel = c[1];
    if (sel == 0) {
        e.a.v = 1;
        e.b.v = 0;
    }
    for (i = 0; i < 2; i++) {
        (&o)->clear();
        (((VillagerData *)(a[(&e.a)[i].v]))->getVillagerId())->getName(&o);
        ((TalkWindowState *)((void *)window))->setSlot(i + 7, &o);
    }
    }
    if (b.a.v != -1 && b.b.v != -1) {
        s32 r = SaveVillagers_GetRelationLevel(gSaveVillagers, b.a.v, b.b.v);
        if (r == 2) {
            x = 1;
        } else if (r > 2) {
            x = 2;
        }
        if (c[0] == sel) {
            if (c[0] == 0) {
                y = 1;
            } else {
                y = 2;
            }
        }
    }
    y *= 3;
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuFriend.key, sTalkTopicTsuFriend.variantCount, x + y, 0);
    return TRUE;
}

BOOL VillagerTalkRumorTopics::selectTsuSpot() {
    s32 t = 4;
    BlockMap *map = gSceneBlockMap;
    if (Scene_InVillagerHouse() != 0) {
        s32 v = Scene_GetVillagerHouse();
        if (v == Villager_GetIndex(actor->villagerData)) {
            t = 3;
            goto end;
        }
    }
    if (map != NULL) {
        BOOL z = ((u8 *)&gFieldSceneKind)[0] == 0 ? TRUE : FALSE;
        if (z) {
            VillagerActor *sc = actor;
            VecFx32 *pp = &sc->position;
            u32 y = pp->z >> 17;
            u32 x = pp->x >> 17;
            MapBlock *cell;
            if (x < (u32)map->width && y < (u32)map->height && map->blocks != NULL) {
                cell = &map->blocks[x + y * map->width];
            } else {
                cell = NULL;
            }
            if (cell != NULL) {
                if (MapBlock_HasAnyAttr(cell, 0x200) == 1) {
                    t = 0;
                } else if (MapBlock_HasAnyAttr(cell, 2) == 1) {
                    t = 1;
                } else if (MapBlock_HasAnyAttr(cell, 0x800) == 1) {
                    t = 2;
                }
            }
        }
    }
end:
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuSpot.key, sTalkTopicTsuSpot.variantCount, t, 0);
    return TRUE;
}

BOOL VillagerTalkRumorTopics::selectTsuHome() {
    s32 t = 2;
    if (PlayerData_GetCurrent() != NULL) {
        t = (PlayerData_GetCurrent()->getPlayerId())->getTownRelation();
    }
    if (t >= 2) {
        t = 1;
    }
    t <<= 3;
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicsTsuHome[t >> 3].key, sTalkTopicsTsuHome[t >> 3].variantCount, 0, 0);
    return TRUE;
}

BOOL VillagerTalkRumorTopics::selectTsuItem() {
    void *t = PlayerData_GetCurrent();
    BOOL result = FALSE;
    if (t != 0) {
        if (((PlayerInventory *)(((PlayerData *)(t))->getInventory()))->findEmptyPocket() != -1) {
            u16 v;
            Talk_PickRandomTradeItem(&v);
            itemFromPlayer = v;
            if (itemFromPlayer != 0xfff1) {
                ((ActorTalkRequest *)this)->setItemNameSlot(&itemFromPlayer, result, 7);
                VillagerId *o = (actor->villagerData->getVillagerId());
                Talk_SelectTopicMessage(this, topicFile, &topicIndex, 30, VillagerId_GetPersonality(o), sTalkTopicTsuItem.key, sTalkTopicTsuItem.variantCount, result, result);
                result = TRUE;
            }
        }
    }
    return result;
}

extern "C" void *Talk_PickRandomMemory(void *ctx, void **arr, s32 n, BOOL (*cb)(void *, void *)) {
    s32 count = 0;
    void *result = NULL;
    u8 flags[8];
    s32 i;
    MI_CpuFill8(flags, count, 8);
    for (i = count; i < n; i++) {
        if (arr[i] != NULL) {
            if (cb(ctx, arr[i]) == 1) {
                flags[i] = 1;
                count++;
            }
        }
    }
    if (count > 0) {
        s32 k = Random_GlobalBelow(count);
        for (i = 0; i < n; i++) {
            if (flags[i] == 1) {
                if (k == 0) {
                    result = arr[i];
                    break;
                }
                k--;
            }
        }
    }
    return result;
}

extern "C" s32 Talk_GetDaysSinceMemoryTime(void *item) {
    ClockDateTimeWords o;
    o.words[0] = 0;
    o.words[1] = 0;
    ClockDateTime *id = (ClockDateTime *)VillagerMemory_GetTalkDate(item);  // (VillagerMemory: the talk date)
    s32 r = 0;
    Clock_GetDateTime(&o.dt);
    if (DateTime_Compare(id, &o.dt, 0x3f) == -1) {
        r = DateTime_DiffDays(id, &o.dt);
    }
    return r;
}

extern "C" BOOL Talk_IsMemoryLikedAndOld(void *ctx, void *item) {
    if (((VillagerMemory *)(item))->getFriendship() > 0 && Talk_GetDaysSinceMemoryTime(item) > 0) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerTalkRumorTopics::pickMemoryLikedOld(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_IsMemoryLikedAndOld);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
        ((ActorTalkRequest *)(this))->setNumberSlot(Talk_GetDaysSinceMemoryTime(res), 0, 10, 0, 0);
    }
    return res;
}

extern "C" BOOL Talk_IsMemoryDislikedAndOld(void *ctx, void *item) {
    if (((VillagerMemory *)(item))->getFriendship() <= 0 && Talk_GetDaysSinceMemoryTime(item) > 0) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerTalkRumorTopics::pickMemoryDislikedOld(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_IsMemoryDislikedAndOld);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
        ((ActorTalkRequest *)(this))->setNumberSlot(Talk_GetDaysSinceMemoryTime(res), 0, 10, 0, 0);
    }
    return res;
}

extern "C" s32 Talk_IsMemoryLetterSender(void *ctx, void *item) {
    return ((VillagerData *)(ctx))->hasLetterFrom((u16 *)(VillagerMemory_GetPlayerId(item)));
}

extern "C" BOOL Talk_IsMemoryLetterSenderFromVillagerTown(void *ctx, void *item) {
    u16 *mine = (u16 *)&static_cast<VillagerData *>(ctx)->letterTown;
    BOOL r = FALSE;
    if (((VillagerData *)(ctx))->hasLetterFrom((u16 *)(VillagerMemory_GetPlayerId(item))) != 0) {
        u16 *p = (u16 *)PlayerId_GetTownId(VillagerMemory_GetPlayerId(item));
        if (*p == *mine) {
            if (memcmp(p + 1, mine + 1, 8) == 0) {
                r = TRUE;
            }
        }
    }
    return r;
}

void *VillagerTalkRumorTopics::pickMemoryLetterFromVillagerTown(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_IsMemoryLetterSenderFromVillagerTown);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(&actor->villagerData->letterTown, 9);
    }
    return res;
}

extern "C" BOOL Talk_MemoryHasCompliment(void *ctx, void *item) {
    if (((VillagerMemory *)(item))->hasCompliment() == 1) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerTalkRumorTopics::pickMemoryWithCompliment(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_MemoryHasCompliment);
    if (res != NULL) {
        MsgString17 o;
        ((VillagerMemory *)(res))->getCompliment(&o);
        ((TalkWindowState *)((void *)window))->setSlot(1, &o);
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
    }
    return res;
}

extern "C" BOOL Talk_IsMemoryLetterSenderResident(void *ctx, void *item) {
    void *id = VillagerMemory_GetPlayerId(item);
    BOOL r = FALSE;
    BOOL ok = FALSE;
    if (((VillagerData *)(ctx))->hasLetterFrom((u16 *)(id)) == 1) {
        ok = TRUE;
    }
    if (ok) {
        if (PlayerDataArray_FindById(gSavePlayers, (u16 *)id) != -1) {
            r = TRUE;
        }
    }
    return r;
}

void *VillagerTalkRumorTopics::pickMemoryLetterFromResident(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_IsMemoryLetterSenderResident);
    if (res != NULL) {
        LetterView *a = reinterpret_cast<LetterView *>(actor->villagerData->getLetter());  // (LetterView: a view of the Letter)
        LetterParty *b = Letter_GetSenderPlayer(a);
        TownIdView o;
        TownId_CopyFrom(&o.townId, (u16 *)b);
        ((ActorTalkRequest *)this)->setTownNameSlot(&o, 9);
    }
    return res;
}

extern "C" BOOL Talk_MemoryHasTime(void *ctx, void *item) {
    if (((VillagerMemory *)(item))->hasTownTune() == 1) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerTalkRumorTopics::pickMemoryWithTime(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_MemoryHasTime);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
    }
    return res;
}

extern "C" BOOL Talk_MemoryAny(void *ctx, void *item) {
    return TRUE;
}

void *VillagerTalkRumorTopics::pickMemoryAny(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_MemoryAny);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
    }
    return res;
}

extern "C" BOOL Talk_MemoryHasReceivedItem(void *ctx, void *item) {
    if (*((VillagerMemory *)(item))->getReceivedItem() != 0xfff1) {
        return TRUE;
    }
    return FALSE;
}

void *VillagerTalkRumorTopics::pickMemoryWithReceivedItem(void **arr, s32 n) {
    void *res = Talk_PickRandomMemory(actor->villagerData, arr, n, Talk_MemoryHasReceivedItem);
    if (res != NULL) {
        ((ActorTalkRequest *)this)->setTownNameSlot(VillagerMemory_GetTownId(res), 9);
        ((ActorTalkRequest *)this)->setItemNameSlot(((VillagerMemory *)(res))->getReceivedItem(), 1, 7);
    }
    return res;
}

BOOL VillagerTalkTopics::selectTsuMemory() {
    void *r14;
    void *p18;
    void *r4;
    s32 idx;
    s32 n2;
    s32 n1;
    s32 i;
    s32 j;
    s32 k;
    s32 rnd;
    u8 buf[2];
    u8 flags[5];
    u32 arr[8];
    if ((*(CommManager * *)&gCommManager)->isSlotActive((*(CommManager * *)&gCommManager)->myAid) != 0) {
        return FALSE;
    }
    static Unk_02021048_Fn tbl[10] = {
        ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7d90), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7dc8), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7ce0),
        ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7e10), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7e58), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7e70),
        ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7a78), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7f58), ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7f98),
        ((void * (VillagerTalkTopics::*)(u32 *a, s32 n))data_020d7a40),
    };
    if (PlayerData_GetCurrent() != NULL) {
        r14 = PlayerData_GetCurrent()->getPlayerId();
    } else {
        r14 = 0;
    }
    p18 = actor->villagerData;
    r4 = 0;
    idx = 0;
    if (r14 != 0) {
        for (i = 0; i < 8; i++) {
            arr[i] = 0;
        }
        n1 = Villager_CollectOtherMemories(p18, arr, r14, 1);
        if (n1 > 0) {
            MI_CpuFill8(flags, 0, 5);
            for (k = 5; k > 0; k--) {
                rnd = Random_GlobalBelow(k);
                for (j = 0; j < 5; j++) {
                    if (flags[j] == 0) {
                        if (rnd == 0) {
                            r4 = (this->*tbl[j])(arr, n1);
                            if (r4 != 0) {
                                idx = j + 1;
                            }
                            flags[j] = 1;
                            break;
                        }
                        rnd--;
                    }
                }
                if (r4 != 0) {
                    break;
                }
            }
        }
        if (r4 == 0) {
            for (i = 0; i < 8; i++) {
                arr[i] = 0;
            }
            n2 = Villager_CollectOtherMemories(p18, arr, r14, 0);
            if (n2 > 0) {
                MI_CpuFill8(flags, 0, 5);
                for (k = 5; k > 0; k--) {
                    rnd = Random_GlobalBelow(k);
                    for (j = 0; j < 5; j++) {
                        if (flags[j] == 0) {
                            if (rnd == 0) {
                                r4 = (this->*tbl[j + 5])(arr, n2);
                                if (r4 != 0) {
                                    idx = j + 6;
                                }
                                flags[j] = 1;
                                break;
                            }
                            rnd--;
                        }
                    }
                    if (r4 != 0) {
                        break;
                    }
                }
            }
        }
    }
    if (r4 != 0) {
        MsgString9B obj;  // (constructed and destroyed only)
        buf[0] = ((VillagerMemory *)(r4))->getImpression();
        buf[1] = 0;
        ((ActorTalkRequest *)(this))->setSlotFromString(2, buf, "st_impress");
        ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)VillagerMemory_GetPlayerId(r4), 10);
    }
    unk_134 = r4;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuMemory.key, sTalkTopicTsuMemory.variantCount, idx, 0);
    customFn1 = (Unk_020238b0_Fn)(data_020d7c50);
    customFn2 = (Unk_020238b0_Fn)(data_020d7c48);
    return TRUE;
}

extern "C" {
const TalkTopic sTalkTopicQ12Thanks = {sTalkKeyQ12Thanks, 0x3};
const TalkTopic sTalkTopicAiPassword = {sTalkKeyAiPassword, 0x3};
char sTalkKeyQ02End[8] = "q02_end";
const TalkTopic sTalkTopicsQEnd[5] = {
    {sTalkKeyQ01End, 0x2}, {sTalkKeyQ02End, 0x2}, {sTalkKeyQ03End, 0x2}, {sTalkKeyQ04End, 0x2}, {sTalkKeyQ05End, 0x2},
};
char sTalkKeyQ05Comp[9] = "q05_comp";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79b8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
RumorVillagerPair data_020d7cb8 = {{0}, {1}};
char sTalkKeyEvFishing[11] = "ev_fishing";
char sTalkKeyAiSad[7] = "ai_sad";
char sTalkKeyEvArbeit[10] = "ev_arbeit";
char sTalkKeyQ12Full[9] = "q12_full";
char sTalkKeyQ12Other[10] = "q12_other";
const char data_020c7a2c[9] = "q05_miss";
char sTalkKeyAiBee[7] = "ai_bee";
char sTalkKeyQ06Over[9] = "q06_over";
decltype(&VillagerTalk::runCustomFn3) data_020d7cf0 = &VillagerTalk::runCustomFn3;
decltype(&VillagerTalkRumorTopics::pickMemoryDislikedOld) data_020d7dc8 = &VillagerTalkRumorTopics::pickMemoryDislikedOld;
const char *sTalkKeys3p[6] = {
    sTalkKey3pBo, sTalkKey3pHa, sTalkKey3pKo, sTalkKey3pFu, sTalkKey3pGe, sTalkKey3pTa,
};
const u32 sTsuEvent1EventIds[14] = {
    0x00000046, 0x00000047, 0x00000048, 0x00000049, 0x0000004a, 0x0000004b, 0x0000004c, 0x0000004d,
    0x0000004e, 0x0000004f, 0x00000050, 0x00000051, 0x00000052, 0x00000053,
};
decltype(&VillagerTalkTopics::tryAddDeliveryRecipientChoice) data_020d7ce8 = &VillagerTalkTopics::tryAddDeliveryRecipientChoice;
char sTalkKeyQ06Open[9] = "q06_open";
char sTalkKey3pGe[6] = "3p_ge";
const char *sTalkKeysQ01Req[5] = {
    sTalkKeyQ01Req1, sTalkKeyQ01Req245, sTalkKeyQ01Req3, sTalkKeyQ01Req245, sTalkKeyQ01Req245,
};
char sTalkKeyQ07Mailgo[11] = "q07_mailgo";
const TalkTopic sTalkTopicAiRain2 = {sTalkKeyAiRain2, 0x6};
decltype(&VillagerTalkKaraokeTopics::endEvFirework) data_020d79c8 = &VillagerTalkKaraokeTopics::endEvFirework;
char sTalkKeyQ07Open[9] = "q07_open";
const TalkTopic sTalkTopicApPresent1 = {sTalkKeyApPresent1, 0x3};
const TalkTopic sTalkTopicAiAnger = {sTalkKeyAiAnger, 0x5};
char sTalkKeyQError1[9] = "q_error1";
decltype(&VillagerTalkTopics::endEtcPushHit) data_020d7c68 = &VillagerTalkTopics::endEtcPushHit;
const TalkTopic sTalkTopicQ06Open1 = {sTalkKeyQ06Open1, 0x3};
decltype(&VillagerTalkTopics::finishOpenedDelivery) data_020d7930 = &VillagerTalkTopics::finishOpenedDelivery;
decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7940 = &VillagerTalk::playOwnerIdleAnim;
}

const char *VillagerTalkTopics::findTsuEvent1Message(u8 *a, s32 *b, u8 *c, ClockDateTime *d) {
    const char *result = NULL;
    u32 mask = 0;
    s32 count = 0;
    s32 i;
    ClockDateTime buf;
    s32 v;
    s32 r;
    for (i = 0; i < 14; i++) {
        MI_CpuCopy8(d, &buf, 8);
        if (Event_GetState(sTsuEvent1EventIds[i], &buf, 0) != 0) {
            mask = (u16)(mask | (1 << i));
            count++;
        }
    }
    if (count > 0) {
        r = Random_PickSetBit(mask, count, 14);
        v = -1;
        if (r >= 5) {
            if (r <= 12) {
                s32 k = r - 5;
                if (k != Villager_GetIndex(actor->villagerData)) {
                    void *p = SaveVillagers_Get(gSaveVillagers, k);
                    if (p != 0 && (((VillagerData *)(p))->getVillagerId())->isValid() != 0) {
                        u8 *e = (u8 *)Villager_GetBirthday(p);
                        if (e != 0) {
                            v = 5;
                            ((ActorTalkRequest *)this)->setVillagerNameSlot(((VillagerData *)(p))->getVillagerId(), 0);
                            ((ActorTalkRequest *)(this))->setMonthSlot(e[0], 1);
                            ((ActorTalkRequest *)(this))->setDaySlot(e[1], 2);
                        }
                    }
                }
            } else {
                v = 6;
            }
        } else {
            v = r;
        }
        if (v != -1) {
            *b = v;
            *a = sTalkTopicTsuEvent1.variantCount;
            *c = 0;
            result = sTalkTopicTsuEvent1.key;
        }
    }
    return result;
}

const char *VillagerTalkTopics::findTsuEvent2Message(u8 *a, s32 *b, u8 *c, ClockDateTime *d) {
    const char *result = NULL;
    s32 idx = -1;
    s32 i;
    ClockDateTime buf;
    for (i = 0; i < 12; i++) {
        MI_CpuCopy8(d, &buf, 8);
        if (Event_GetState(sTsuEvent2EventIds[i], &buf, 0) != 0) {
            idx = i;
            break;
        }
    }
    if ((u32)idx < 12) {
        if (idx % 3 == 2) {
            *b = (idx / 3 + 1) * 5 - 1;
            *c = 1;
        } else {
            *b = idx * 2 - idx / 3;
            *c = 2;
        }
        *a = 1;
        result = sTalkTopicTsuEvent2.key;
    }
    return result;
}

BOOL VillagerTalkTopics::selectTsuEvent() {
    static Unk_02020d90_Fn tbl[2] = {data_020d7c18, data_020d7c00};
    TalkTopicPick res[2];
    ClockDateTimeWords t;
    s32 i;
    TalkTopicPick *p;
    func_02133ef8(res, sizeof(res));
    p = 0;
    t.words[0] = 0;
    t.words[1] = 0;
    Clock_GetDateTime(&t.dt);
    for (i = 0; i < 2; i++) {
        res[i].key = (this->*tbl[i])(&res[i].partSize, &res[i].part, &res[i].variantCount, &t.dt);
    }
    if (res[0].key != 0) {
        if (res[1].key != 0 && Random_GlobalBelow(2) == 0) {
            p = &res[1];
        } else {
            p = &res[0];
        }
    } else if (res[1].key != 0) {
        p = &res[1];
    }
    if (p != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), p->key, p->partSize, p->part, p->variantCount);
        return TRUE;
    }
    return FALSE;
}

extern "C" {
const TalkTopic sTalkTopicEvFishing = {sTalkKeyEvFishing, 0x3};
char sTalkKeyEvAcorn[9] = "ev_acorn";
char sTalkKeyTsuFuHint[12] = "tsu_fu_hint";
char sTalkKeyQ06Req[8] = "q06_req";
char sTalkKey3pHa[6] = "3p_ha";
char sTalkKeyQ12End[8] = "q12_end";
char sTalkKeyQ07Joy[8] = "q07_joy";
decltype(&VillagerTalkHobbyTopics::endEvInsect) data_020d7c20 = &VillagerTalkHobbyTopics::endEvInsect;
decltype(&VillagerTalkTopics::findTsuEvent1Message) data_020d7c18 = &VillagerTalkTopics::findTsuEvent1Message;
char sTalkKeyTsuFlHint[12] = "tsu_fl_hint";
const TalkTopic sTalkTopicQ06Open3 = {sTalkKeyQ06Open3, 0x3};
const TalkTopic sTalkTopicQ06Open2 = {sTalkKeyQ06Open2, 0x3};
char sTalkKeyQ06Good[9] = "q06_good";
char sTalkKeyQ10Con[8] = "q10_con";
decltype(&VillagerTalk::playOwnerIdleAnim) data_020d7fe8 = &VillagerTalk::playOwnerIdleAnim;
VillagerTalkTopicFns sEvBirthTopicTable[7];
const u32 sApTopicWeights[3] = {
    0x0a0a0a0a, 0x0a080e0e, 0x0000000e,
};
decltype(&VillagerTalkTopics::checkReserveTime) data_020d7be8 = &VillagerTalkTopics::checkReserveTime;
decltype(&VillagerTalkTopics::selectApTrade) data_020d7be0 = &VillagerTalkTopics::selectApTrade;
char sTalkKeyQ10Leave[10] = "q10_leave";
decltype(&VillagerTalkTopics::onPresentOpinionChoice) data_020d7fa0 = &VillagerTalkTopics::onPresentOpinionChoice;
const TalkTopic sTalkTopicTsuFuHint = {sTalkKeyTsuFuHint, 0x3};
char sTalkKeyQ02Req1[9] = "q02_req1";
const TalkTopic sTalkTopicAiMfirst = {sTalkKeyAiMfirst, 0x1};
char sTalkKeyQ02Req3[9] = "q02_req3";
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7f78 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalkHobbyTopics::onEvAdmireWordEnteredB) data_020d7f70 = &VillagerTalkHobbyTopics::onEvAdmireWordEnteredB;
char sTalkKeyQ02Con1[9] = "q02_con1";
char sTalkKeyQ03Req45[11] = "q03_req4_5";
decltype(&VillagerTalkAcornTopics::continueAcornReceived) data_020d7ba0 = &VillagerTalkAcornTopics::continueAcornReceived;
char sTalkKeyQ04Req12[11] = "q04_req1_2";
char sTalkKeyTsuNoHint[12] = "tsu_no_hint";
decltype(&VillagerTalkTopics::storeReservation) data_020d79d0 = &VillagerTalkTopics::storeReservation;
char sTalkKeyQ01Pwin2[10] = "q01_pwin2";
char sTalkKeyQ02Con3[9] = "q02_con3";
char sTalkKeyEvBirth[9] = "ev_birth";
const char data_020c7a44[12] = "q03_clear";
const TalkTopic sTalkTopicAiPoison = {sTalkKeyAiPoison, 0x3};
const u32 sMoodAnimIds[5] = {
    0x00000000, 0x000000e5, 0x000000e7, 0x000000e9, 0x000000e7,
};
decltype(&VillagerMood::beginMood3Effects) data_020d7b68 = &VillagerMood::beginMood3Effects;
decltype(&VillagerTalkTopics::selectTsuSeHint) data_020d7ef8 = &VillagerTalkTopics::selectTsuSeHint;
decltype(&VillagerTalk::attrShowLetter) data_020d7ee8 = &VillagerTalk::attrShowLetter;
decltype(&VillagerTalkTopics::selectTsuFlAct) data_020d7b58 = &VillagerTalkTopics::selectTsuFlAct;
const char *sTalkKeysQ01Con[5] = {
    sTalkKeyQ01Con1, sTalkKeyQ01Con245, sTalkKeyQ01Con3, sTalkKeyQ01Con245, sTalkKeyQ01Con245,
};
const TalkTopic sTalkTopicAiRun = {sTalkKeyAiRun, 0x3};
u8 sInsectRarityWork[8];
const TalkTopic sTalkTopicApNickn = {sTalkKeyApNickn, 0x1};
decltype(&VillagerTalkRumorTopics::selectTsuItem) data_020d7b48 = &VillagerTalkRumorTopics::selectTsuItem;
char sTalkKeyQ02Nwin[9] = "q02_nwin";
TalkTopic data_020d7b40 = {NULL, 0x00000003};
const TalkTopic sTalkTopicQ01Pay = {sTalkKeyQ01Pay, 0x3};
const TalkTopic sTalkTopicQ02Revenge = {sTalkKeyQ02Revenge, 0x3};
decltype(&VillagerTalkTopics::selectTsuMemory) data_020d7b38 = &VillagerTalkTopics::selectTsuMemory;
char sTalkKeyQ03Con12[11] = "q03_con1_2";
char sTalkKeyQ02Pwin2[10] = "q02_pwin2";
VillagerTalkTopicFns sTalkBeginTopics[17];
char sTalkKeyQ02Pdraw[10] = "q02_pdraw";
const TalkTopic sTalkTopicTsuMove2 = {sTalkKeyTsuMove2, 0x5};
char sTalkKeyQ05Con[8] = "q05_con";
char sTalkKeyAiShop1[9] = "ai_shop1";
VillagerTalkTopicFns sSmallTalkTopicTable[2];
TalkTopic sTalkTopicQ05TalkDefault = {NULL, 0x00000003};
VillagerTalkTopicFns sEtcCancelTopicTable[1];
char sTalkKeyAiNfirst[10] = "ai_nfirst";
char sTalkKeyQ01Revenge[12] = "q01_revenge";
char sTalkKeyTsuGhint[10] = "tsu_ghint";
decltype(&VillagerTalkTopics::gotoQEndOrRevenge) data_020d7ac0 = &VillagerTalkTopics::gotoQEndOrRevenge;
decltype(&VillagerTalk::runCustomFn4) data_020d7df8 = &VillagerTalk::runCustomFn4;
decltype(&VillagerTalk::attrOpenBirthdayEntry) data_020d7b00 = &VillagerTalk::attrOpenBirthdayEntry;
char sTalkKey3pKo[6] = "3p_ko";
decltype(&VillagerTalkTopics::selectTsuNoAct) data_020d7dd8 = &VillagerTalkTopics::selectTsuNoAct;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7dd0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerTalk::giveItemToPlayer) data_020d7ae0 = &VillagerTalk::giveItemToPlayer;
const u32 sTsuEvent2EventIds[12] = {
    0x00000054, 0x00000055, 0x00000056, 0x00000057, 0x00000058, 0x00000059, 0x0000005a, 0x0000005b,
    0x0000005c, 0x0000005d, 0x0000005e, 0x0000005f,
};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7db0 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyAiFortune[11] = "ai_fortune";
const TalkTopic sTalkTopicQ02Nlose = {sTalkKeyQ02Nlose, 0x3};
char sTalkKeyQ05Miss[9] = "q05_miss";
const TalkTopic sTalkTopicTsuFriend = {sTalkKeyTsuFriend, 0x1};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d79d8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
char sTalkKeyQ01End[8] = "q01_end";
const TalkTopic sTalkTopicTsuGhint = {sTalkKeyTsuGhint, 0x5};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7d18 = &VillagerTalkRequestReplyTopics::runChosenTopic;
u8 data_020d7860[2] = {
    0x02, 0x02,
};
decltype(&VillagerTalkRequestItemTopics::gotoQClearOrEnd) data_020d7cf8 = &VillagerTalkRequestItemTopics::gotoQClearOrEnd;
char sTalkKeyEvFirework[12] = "ev_firework";
const TalkTopic sTalkTopicEtcConnect = {sTalkKeyEtcConnect, 0x3};
VillagerTalkTopicFns sRequestTopicsA[26];
char sTalkKeyQ01Return[11] = "q01_return";
const TalkTopic sTalkTopicAiFlea = {sTalkKeyAiFlea, 0x3};
decltype(&VillagerTalkRumorTopics::pickMemoryWithCompliment) data_020d7e10 = &VillagerTalkRumorTopics::pickMemoryWithCompliment;
TalkTopic sTalkTopicQConDefault = {NULL, 0x00000003};
char sTalkKeyQ04Dress[10] = "q04_dress";
char sTalkKeyEtcConnect[12] = "etc_connect";
decltype(&VillagerTalkTopics::selectTsuFoHint) data_020d7f30 = &VillagerTalkTopics::selectTsuFoHint;
const u8 sNicknamePatternBases[6] = {
    0x08, 0x10, 0x20, 0x00, 0x18, 0x28,
};
char sTalkKeyQ06Fin[8] = "q06_fin";
char sTalkKeyQ07Req[8] = "q07_req";
char sTalkKeyQ12Report[11] = "q12_report";
decltype(&VillagerTalk::buyItemFromPlayer) data_020d7cb0 = &VillagerTalk::buyItemFromPlayer;
const char data_020c7a20[9] = "q04_miss";
const TalkTopic sTalkTopicQIcancel = {sTalkKeyQIcancel, 0x3};
decltype(&VillagerTalkRequestReplyTopics::setDeliveryDeadline) data_020d7a48 = &VillagerTalkRequestReplyTopics::setDeliveryDeadline;
}

u32 VillagerTalkTopics::selectTsuDrama() {
    u8 r5 = sTalkTopicTsuDrama.variantCount;
    u32 r6 = 0;
    ClockDateTimeWords t;
    Unk_020874e8_Bits bits;
    s32 r4;
    u32 flag;
    t.words[0] = 0;
    t.words[1] = 0;
    r4 = -1;
    flag = 0;
    Clock_GetDateTime(&t.dt);
    if (Drama_GetAvailablePart(t.dt.year, t.dt.month, t.dt.day, &bits) != 0) {
        if (bits.a == 2 && bits.b == 3) {
            r4 = 1;
        } else {
            r4 = 0;
        }
    }
    switch (r4) {
    case 0: {
        u32 i = bits.a;
        if (i >= 4) {
            i = 0;
        }
        r6 = sTsuDramaMsgBlocks[i];
        flag = 1;
        break;
    }
    case 1:
        r5 = 1;
        r6 = bits.c + 12;
        flag = 1;
        break;
    }
    if (flag == 1) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuDrama.key, r5, r6, 0);
    }
    return flag;
}

BOOL VillagerTalkTopics::selectTsuStar() {
    void *base;
    s32 r4;
    void *r7;
    u8 *p14;
    u8 *p18;
    s32 r5;
    BOOL result;
    u8 *rec;
    void *q;
    if (!Unk_02020b38_IsZero(gFieldSceneKind)) {
        return FALSE;
    }
    base = Constellation_GetData();
    r4 = Constellation_FindVisibleNow();
    r7 = PlayerData_GetCurrent();
    p14 = 0;
    p18 = 0;
    r5 = -1;
    result = FALSE;
    if (r4 != -1 && ConstellationStore_IsUsed((ConstellationStore *)base, r4) != 0 && r7 != 0) {
        rec = (u8 *)base + r4 * 0x46;
        if (((PlayerId *)(rec))->isValid() != 0) {
            q = ((PlayerData *)(r7))->getPlayerId();
            if (*(u16 *)rec == *(u16 *)q && memcmp(rec + 2, (u8 *)q + 2, 8) == 0 && ((PlayerId *)(rec))->equals((PlayerId *)(q)) != 0) {
                r5 = 0;
            } else if (((TownId *)(rec))->getTownRelation() == 0) {
                if (PlayerId_FindResidentIndex(rec) != -1) {
                    r5 = 1;
                }
            } else if (((PlayerId *)(rec))->equals(((PlayerData *)(r7))->getPlayerId()) == 0) {
                r5 = 2;
            }
            if (r5 != -1) {
                p14 = rec;
                p18 = rec + 0x16;
            }
        }
    }
    if (r5 != -1) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuStar.key, sTalkTopicTsuStar.variantCount, r5, 0);
        result = TRUE;
        if (p18 != 0) {
            ConstellationEncodedString16 s;
            ConstellationMsgString17 t;
            EncodedString_SetRaw(&s, p18, 0x10);
            t.fromEncoded(&s, 0, 0);
            ((TalkWindowState *)(window))->setSlot(0, &t);
        }
        if (p14 != 0) {
            ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)p14, 1);
            ((ActorTalkRequest *)this)->setTownNameSlot(PlayerId_GetTownId(p14), 2);
        }
    }
    return result;
}

BOOL VillagerTalkTopics::selectEvArbeit() {
    s32 r = PlayerData_GetCurrent()->getArbeitTalkCount();
    if ((*(CommManager * *)&gCommManager)->isOnline() != 0) {
        r = 10;
    }
    if (r < 10) {
        Villager_MakePersonalityFileName(&topicFile, 30, sTalkTopicEvArbeit.key, VillagerId_GetPersonality(actor->villagerData->getVillagerId()));
        topicIndex = r;
        PlayerData_GetCurrent()->advanceArbeitTalkCount();
        customFn0 = (Unk_020238b0_Fn)(data_020d7bd0);
        customFn1 = (Unk_020238b0_Fn)(data_020d7fd8);
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerTalkTopics::selectEvFmarket1() {
    s32 r = Random_GlobalBelow(2);
    if (((VillagerTalk *)actor)->getEventKind() == 10 && r == 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvFmarket1.key, sTalkTopicEvFmarket1.variantCount, 0, 0);
        return TRUE;
    }
    return FALSE;
}

s32 VillagerTalkTopics::pickTsuTopicIndex(u8 *a, s32 b) {
    s32 r = Talk_PickWeightedIndex(a, b);
    if (r == -1) {
        r = 4;
    }
    return r;
}

void VillagerTalkTopics::setTsuTextVariables() {
    this->setStringTableArg(4, 3);
    this->setFashionArg(5, actor->villagerData);
    this->setStringTableArg(6, 4);
}

void VillagerTalkTopics::selectTsuTopic(TalkTopicMsg *out) {
    static Unk_02020850_Fn tbl[14] = { data_020d7fa8, data_020d7b98, data_020d7f90, data_020d7f80, data_020d7b88, ((Unk_02020850_Fn)data_020d7b78), ((Unk_02020850_Fn)data_020d7f48), ((Unk_02020850_Fn)data_020d7b60), ((Unk_02020850_Fn)data_020d7f10), ((Unk_02020850_Fn)data_020d7b48), data_020d7b38, data_020d7b30, ((Unk_02020850_Fn)data_020d7ea8), data_020d7ea0 };
    s32 r, i;
    MI_CpuCopy8(sTsuTopicWeights, sTsuTopicWeightsWork, 14);
    setTsuTextVariables();
    r = this->selectEvFmarket1();
    if (r == 0) {
        r = this->selectEvArbeit();
    }
    while (r == 0) {
        i = this->pickTsuTopicIndex((u8 *)sTsuTopicWeightsWork, 14);
        if (i >= 0 && i < 14) {
            r = (this->*tbl[i])();
            if (r == 0) {
                sTsuTopicWeightsWork[i] = 0;
            }
        } else {
            this->selectTsuAlways();
            break;
        }
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

extern "C" {
char sTalkKeyTsuFiHint[12] = "tsu_fi_hint";
const TalkTopic sTalkTopicQError1 = {sTalkKeyQError1, 0x2};
char sTalkKeyQIcancel[10] = "q_icancel";
char sTalkKeyQ10Reserved[13] = "q10_reserved";
char sTalkKeyQError2[9] = "q_error2";
const TalkTopic sTalkTopicAiSnow1 = {sTalkKeyAiSnow1, 0x3};
decltype(&VillagerTalkTopics::advanceRequestStep) data_020d7970 = &VillagerTalkTopics::advanceRequestStep;
const TalkTopic sTalkTopicQTimeover = {sTalkKeyQTimeover, 0x3};
decltype(&VillagerTalkTopics::continueLateLetterShown) data_020d7a50 = &VillagerTalkTopics::continueLateLetterShown;
decltype(&VillagerTalkTopics::gotoQ12End) data_020d7c38 = &VillagerTalkTopics::gotoQ12End;
Letter sTalkLetter;
const u8 sSmallTalkChoiceMsgRange[2] = {
    0x00, 0x09,
};
const TalkTopic sTalkTopicsTsuHome[2] = {
    {sTalkKeyTsuShome, 0x3}, {sTalkKeyTsuGhome, 0x3},
};
decltype(&VillagerTalkTopics::finishOpenedLetter) data_020d7ff0 = &VillagerTalkTopics::finishOpenedLetter;
decltype(&VillagerTalkTopics::gotoDeliveryFin) data_020d7fe0 = &VillagerTalkTopics::gotoDeliveryFin;
const TalkTopic sTalkTopicTsuFlHint = {sTalkKeyTsuFlHint, 0x3};
decltype(&VillagerTalkTopics::selectApBuy) data_020d79f0 = &VillagerTalkTopics::selectApBuy;
const TalkTopic sTalkTopicQ06Con = {sTalkKeyQ06Con, 0x3};
decltype(&VillagerTalkTopics::gotoDeliveryFin) data_020d7bc0 = &VillagerTalkTopics::gotoDeliveryFin;
char sTalkKeyQ01Con1[9] = "q01_con1";
const TalkTopic sTalkTopicAiJoy = {sTalkKeyAiJoy, 0x3};
}

void VillagerTalkTopics::startGreetingB() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
}

void VillagerTalkTopics::continueGreetingB() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(sEtcConnectTopicTable);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::openEtcConnectChoice() {
    TalkChoiceTable s;
    VillagerTalkTopicFns *p;
    if (Villager_IsMovingIn(actor->villagerData) != 0) {
        p = &sEtcConnectTopicTable[4];
    } else if (gCommManager->isOnline() != 0 && (PlayerData_GetCurrent()->getPlayerId())->getTownRelation() != 0) {
        p = &sEtcConnectTopicTable[5];
    } else {
        p = &sEtcConnectTopicTable[1];
    }
    TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, p);
    TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ed8);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectTsuMove1(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuMove1.key, sTalkTopicTsuMove1.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openTsuMove1Choice() {
    TalkChoiceTable s;
    TalkChoiceTable_Set(this, (&s), 0, 0x37, 0x39, &sEtcConnectTopicTable[2]);
    TalkChoiceTable_Set(this, (&s), 1, 0x3a, 0x3c, &sEtcConnectTopicTable[3]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7dd0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectTsuMove1B(TalkTopicMsg *out) {
    s32 v;
    if (Random_GlobalBelow(3) == 0 && gCommManager->isOnline() == 0) {
        VillagerData *r6 = actor->villagerData;
        void *r7;
        r7 = VillagerPlanBlock_GetPlan(Villager_GetPlan(r6));
        if (Villager_IsJustMovedIn(r6) == 0) {
            ((VillagerPlan *)(r7))->enterState0B();
            SaveVillagers_SetLastMovedInById(gSaveVillagers, r6->getVillagerId());
        }
        v = 3;
    } else {
        v = 2;
    }
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuMove1.key, sTalkTopicTsuMove1.variantCount, v, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectTsuMove1Part1(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuMove1.key, sTalkTopicTsuMove1.variantCount, 1, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectTsuMove2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicTsuMove2.key, sTalkTopicTsuMove2.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectTsuAlwaysEntry(TalkTopicMsg *out) {
    this->selectTsuAlways();
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

s32 VillagerTalkTopics::selectSituationGreeting(TalkTopicMsg *out) {
    const TalkTopic *d = 0;
    s32 v24 = -1;
    VillagerData *r18 = actor->villagerData;
    u8 *r6 = (u8 *)Villager_GetState(r18);
    s32 r20 = VillagerState_GetMood(r6);
    s32 result = 0;
    s32 v28 = 0;
    s32 v2c = 0;
    s32 r7 = this->getGreetingStatus((s32 *)(&v2c));
    this->setGreetingArgs();
    if (this->hasFallen() != 0) {
        d = &sTalkTopicAiFall;
        if (r7 == 0) {
            noMemoryUpdate = 1;
        }
    } else if (this->isBeeSwarmOut() != 0) {
        d = &sTalkTopicAiRun;
        if (r7 == 0) {
            noMemoryUpdate = 1;
        }
    } else if (this->isFirstMeeting(r7) != 0) {
        d = this->getFirstMeetingTopic((u32 *)(&v28), r18);
    } else if (this->findDangerousInsect((volatile s32 *)(&v24), &actor->position) != 0) {
        d = this->getPoisonTopic(v24);
    } else if (this->hasFleas() != 0) {
        d = &sTalkTopicAiFlea;
    } else if (this->isForeignMemory(r7) != 0) {
        d = this->getForeignTopic();
    } else if (this->isPlayerStung((VillagerStateView *)r6) != 0) {
        d = this->getBeeStingTopic((u32 *)(&v28), (VillagerStateView *)r6);
    } else if (this->isMoodAngry(r20) != 0) {
        d = &sTalkTopicAiAnger;
    } else if (this->isMoodSad(r20) != 0) {
        d = &sTalkTopicAiSad;
    } else if (this->isMoodTired(r20) != 0) {
        d = &sTalkTopicAiTire;
    }
    if (d != 0) {
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(r18->getVillagerId()), d->key, d->variantCount, v28, 0);
        result = 1;
    }
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    r6[0x1d] |= 4;
    return result;
}

void VillagerTalkTopics::selectEvKaraokeTalk(TalkTopicMsg *out) {
    VillagerData *r7 = actor->villagerData;
    TalkTopicMsg o2;
    if (selectSituationGreeting(out) != 0) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
    } else {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
        if (memory != 0 && ((VillagerMemory *)((void *)memory))->isTalkedToday() == 0) {
            Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(r7->getVillagerId()), (*(&sTalkTopicEvKaraoke)).key, (*(&sTalkTopicEvKaraoke)).variantCount, 0, 0);
            out->fileName = topicFile;
            out->msgIndex = topicIndex;
            ((VillagerMemory *)((void *)memory))->setTalkedToday();
        } else {
            if (SaveVillagers_IsTuneRequester(gSaveVillagers, r7->getVillagerId()) == 0) {
                switch (Random_GlobalBelow(3)) {
                case 0:
                    ((VillagerTalk *)this)->setTopicFns(sEvKaraokeTopicTable);
                    break;
                case 1:
                    ((VillagerTalk *)this)->setTopicFns(&sEvKaraokeTopicTable[7]);
                    break;
                default:
                    ((VillagerTalk *)this)->setTopicFns(&sEvKaraokeTopicTable[8]);
                    break;
                }
                if (selectFn) {
                    (this->*selectFn)(out);
                }
            } else {
                ((VillagerTalk *)this)->setTopicFns(&sEvKaraokeTopicTable[8]);
                if (selectFn) {
                    (this->*selectFn)(out);
                }
            }
        }
    }
}

void VillagerTalkTopics::selectEvKaraokeMsg3(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), (*(&sTalkTopicEvKaraoke)).key, 1, 3, (*(&sTalkTopicEvKaraoke)).variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openEvKaraokeChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0xc4, 0xc4, &sEvKaraokeTopicTable[1]);
    TalkChoiceTable_Set(this, (&s), 1, 0xd0, 0xd0, &sEvKaraokeTopicTable[6]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7f60);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg20(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 0x14, sTalkTopicEvKaraoke.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::continueEvKaraokeMsg20() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sEvKaraokeTopicTable[2]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkKaraokeTopics::startEvKaraokeAction() {
    ((ActorTalkRequest *)(this))->requestCloseWindow(3);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7d60);
}

void VillagerTalkKaraokeTopics::waitEvKaraokeAction() {
    ((ActorTalkRequest *)(this))->requestPlayRandomMelody();
    ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7ab0);
}

void VillagerTalkKaraokeTopics::continueEvKaraokeAction() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sEvKaraokeTopicTable[3]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
    ((ActorTalkRequest *)(this))->requestReopenWindow();
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg6(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 6, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::openEvKaraokeMsg6Choice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x44, 0x44, &sEvKaraokeTopicTable[4]);
    TalkChoiceTable_Set(this, (&s), 1, 0x52, 0x52, &sEvKaraokeTopicTable[5]);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ca0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg8(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 8, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7d40);
}

void VillagerTalkKaraokeTopics::endEvKaraokeMsg8() {
    Melody_SaveRandomPattern();
    SaveVillagers_SetTuneRequester(gSaveVillagers, actor->villagerData->getVillagerId());
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg10(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 0xa, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg12(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 0xc, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg14(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 0xe, sTalkTopicEvKaraoke.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::selectEvKaraokeMsg17(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvKaraoke.key, 1, 0x11, sTalkTopicEvKaraoke.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::openSmallTalkChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_SetRange(this, (&s), 0, (const u8 *)sSmallTalkChoiceMsgRange, sSmallTalkTopicTable);
    TalkChoiceTable_SetRange(this, (&s), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
    s.count = 2;
    s.cancelIndex = s.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7d28);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkKaraokeTopics::selectEvFirework(TalkTopicMsg *out) {
    u32 r5;
    VillagerData *r7 = actor->villagerData;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting(out) == 0) {
        ClockDateTimeWords d;
        d.words[0] = 0;
        d.words[1] = 0;
        r5 = Random_GlobalBelow(10) & 1;
        Clock_GetDateTime(&d.dt);
        if (r5 == 0) {
            u8 v = d.dt.hour;
            if (v < 0x13) {
                r5 = 2;
            } else if (v < 0x14) {
                r5 = 0;
            } else if (v < 0x16) {
                r5 = 1;
            } else {
                r5 = 2;
            }
        } else {
            u8 v = d.dt.day;
            if (v != 0) {
                r5 = (v - 1) / 7;
            } else {
                r5 = 0;
            }
            if (Date_GetNthWeekdayDay(d.dt.year, d.dt.month, 6, 5) != -1 && r5 >= 2) {
                r5--;
            }
            r5 = (r5 >= 5 ? 0 : r5) + 3;
        }
        Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(r7->getVillagerId()), sTalkTopicEvFirework.key, sTalkTopicEvFirework.variantCount, r5, 0);
        out->fileName = topicFile;
        out->msgIndex = topicIndex;
    }
    onEndFn = (Unk_020238b0_Fn)(data_020d79c8);
}

void VillagerTalkKaraokeTopics::endEvFirework() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
}

void VillagerTalkKaraokeTopics::selectEvAdmireTalk(TalkTopicMsg *out) {
    if (((VillagerTalkTopics *)this)->selectSituationGreeting(out) != 0) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
    } else {
        if (memory == 0 || ((VillagerMemory *)((void *)memory))->isTalkedToday() == 0) {
            ((VillagerTalk *)this)->setTopicFns(sEvAdmireTopicTable);
        } else if (Random_GlobalBelow(100) < 30) {
            ((VillagerTalk *)this)->setTopicFns(&sEvAdmireTopicTable[2]);
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sEvAdmireTopicTable[6]);
        }
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
        if (selectFn) {
            (this->*selectFn)(out);
        }
    }
}

void VillagerTalkKaraokeTopics::selectEvAdmire(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), (*(&sTalkTopicEvAdmire)).key, (*(&sTalkTopicEvAdmire)).variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::openEvAdmireWordEntry() {
    ((ActorTalkRequest *)(this))->setSubSceneKind2(0x17, (u8 *)&sTalkInputBuffer, 0x10, 0);
    ((ActorTalkRequest *)(this))->openSubScene(6);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7e50);
}

void VillagerTalkKaraokeTopics::saveEnteredCompliment() {
    void *r4;
    void *r5;
    if (MenuCtrl_IsResultOk() != 0) {
        r4 = actor->villagerData;
        if (memory != 0) {
            ((VillagerMemory *)(memory))->setCompliment(MenuCtrl_GetText(), 16);
        } else {
            void *r = MenuCtrl_GetText();
            ((VillagerDataItemView *)(r4))->setComplimentFor(r, 16, PlayerData_GetCurrent()->getPlayerId());
        }
    }
}

void VillagerTalkKaraokeTopics::onEvAdmireWordEntered() {
    u8 b;
    TalkTopicMsg out;
    saveEnteredCompliment();
    ((VillagerTalk *)this)->setTopicFns(&sEvAdmireTopicTable[1]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkKaraokeTopics::selectEvAdmireMsg2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), (*(&sTalkTopicEvAdmire)).key, 1, 2, (*(&sTalkTopicEvAdmire)).variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    if (memory != 0) {
        ((VillagerMemory *)((void *)memory))->setTalkedToday();
    }
}

void VillagerTalkKaraokeTopics::selectEvAdmireMsg7(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), (*(&sTalkTopicEvAdmire)).key, 1, 7, 3);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkKaraokeTopics::openEvAdmireChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x5a, 0x5a, &sEvAdmireTopicTable[5]);
    TalkChoiceTable_Set(this, (&s), 1, 0x6f, 0x6f, &sEvAdmireTopicTable[3]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7ed0);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkHobbyTopics::selectEvAdmireMsg12(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAdmire.key, 1, 12, sTalkTopicEvAdmire.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::openEvAdmireWordEntryB() {
    ((VillagerTalk *)this)->setSubSceneKind2(0x17, (u8 *)sTalkInputBuffer, 0x10, 0);
    ((VillagerTalk *)this)->openSubScene(6);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7f70);
}

void VillagerTalkHobbyTopics::onEvAdmireWordEnteredB() {
    u8 b;
    TalkTopicMsg res;
    this->saveEnteredCompliment();
    ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)((char *)&sEvAdmireTopicTable[4]));
    if (selectFn != 0) {
        (this->*(Unk_0201eea4_Fn)selectFn)(&res);
    }
    b = res.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)(res.fileName));
}

void VillagerTalkHobbyTopics::selectEvAdmireMsg14(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAdmire.key, 1, 14, sTalkTopicEvAdmire.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::selectEvAdmireMsg10(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAdmire.key, 1, 10, sTalkTopicEvAdmire.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::selectEvAdmireMsg4(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAdmire.key, 1, 4, 3);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::continueEvAdmireMsg4() {
    u8 b;
    TalkTopicMsg res;
    ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)((char *)&sEvAdmireTopicTable[7]));
    if (selectFn != 0) {
        (this->*(Unk_0201eea4_Fn)selectFn)(&res);
    }
    b = res.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)(res.fileName));
}

void VillagerTalkHobbyTopics::selectEtcConnectAdmire(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkHobbyTopics::openSmallTalkChoiceAdmire() {
    this->openSmallTalkChoice();
}

extern "C" u32 Talk_GetTourneyHourBlock(u32 x) {
    switch (x) {
    case 6:
    case 9:
    case 12:
    case 15:
        return 6;
    case 8:
    case 11:
    case 14:
    case 17:
        return 12;
    }
    return 9;
}

void VillagerTalkHobbyTopics::selectEvFishing(TalkTopicMsg *out) {
    u32 r6;
    void *r4;
    volatile u16 id;
    u16 v[3];
    u16 *q;
    ClockDateTimeWords t;
    void *sp14;
    u32 sp18;
    void *sp1c;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting(out)) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
        return;
    }
    ContestRecord *dp = (ContestRecord *)gSaveData.contestRecord;
    sp14 = actor->villagerData;
    if (dp) {
        ContestRecord_GetItem(v, dp);
        q = v;
    } else {
        v[1] = 0xfff1;
        q = &v[1];
    }
    id = *q;
    r6 = 0;
    t.words[0] = r6;
    t.words[1] = r6;
    Clock_GetDateTime(&t.dt);
    sp18 = t.dt.hour;
    if (memory == 0 || ((VillagerMemory *)(memory))->isTalkedToday() == 0) {
        r6 = 0;
    } else if (dp) {
        if (Unk_0201f170_InRange(&id, 0x12e8, 0x131f)) {
            sp1c = dp->getHolderPlayer();
            r4 = ((void *)(dp)->getHolderVillager());
            if (((VillagerId *)(r4))->isValid()) {
                VillagerId *a = (VillagerId *)r4;
                VillagerId *b = ((VillagerData *)(sp14))->getVillagerId();
                if (a->townId == b->townId && memcmp(a->townName, b->townName, 8) == 0 && a->species == b->species) {
                    r6 = 3;
                } else {
                    r6 = Talk_GetTourneyHourBlock(sp18);
                }
                ((ActorTalkRequest *)(this))->setVillagerNameSlot((VillagerId *)r4, 1);
            } else if (((PlayerId *)sp1c)->isValid()) {
                r6 = Talk_GetTourneyHourBlock(sp18);
                ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)(dp)->getHolderPlayer(), 1);
            }
            ((ActorTalkRequest *)(this))->setFixedPointSlot(dp->getSize(), 0, 1);
            ((ActorTalkRequest *)(this))->setItemNameSlot((u16 *)&id, 0, 7);
        }
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(((VillagerData *)(sp14))->getVillagerId()), sTalkTopicEvFishing.key, 1, r6, sTalkTopicEvFishing.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)((Unk_0201eea4_Fn)(data_020d7a28));
}

void VillagerTalkHobbyTopics::endEvFishing() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
    if (memory != 0) {
        ((VillagerMemory *)(memory))->setTalkedToday();
    }
}

void VillagerTalkHobbyTopics::selectEvInsect(TalkTopicMsg *out) {
    u32 r6;
    void *r4;
    volatile u16 id;
    u16 v[3];
    u16 *q;
    ClockDateTimeWords t;
    void *sp14;
    u32 sp18;
    void *sp1c;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting(out)) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
        return;
    }
    ContestRecord *dp = (ContestRecord *)gSaveData.contestRecord;
    sp14 = actor->villagerData;
    if (dp) {
        ContestRecord_GetItem(v, dp);
        q = v;
    } else {
        v[1] = 0xfff1;
        q = &v[1];
    }
    id = *q;
    r6 = 0;
    t.words[0] = r6;
    t.words[1] = r6;
    Clock_GetDateTime(&t.dt);
    sp18 = t.dt.hour;
    if (memory == 0 || ((VillagerMemory *)(memory))->isTalkedToday() == 0) {
        r6 = 0;
    } else if (dp) {
        if (Unk_0201f170_InRange(&id, 0x12b0, 0x12e7)) {
            sp1c = dp->getHolderPlayer();
            r4 = ((void *)(dp)->getHolderVillager());
            if (((VillagerId *)(r4))->isValid()) {
                VillagerId *a = (VillagerId *)r4;
                VillagerId *b = ((VillagerData *)(sp14))->getVillagerId();
                if (a->townId == b->townId && memcmp(a->townName, b->townName, 8) == 0 && a->species == b->species) {
                    r6 = 3;
                } else {
                    r6 = Talk_GetTourneyHourBlock(sp18);
                }
                ((ActorTalkRequest *)(this))->setVillagerNameSlot((VillagerId *)r4, 1);
            } else if (((PlayerId *)sp1c)->isValid()) {
                r6 = Talk_GetTourneyHourBlock(sp18);
                ((ActorTalkRequest *)this)->setPlayerNameSlot((PlayerId *)(dp)->getHolderPlayer(), 1);
            }
            ((ActorTalkRequest *)(this))->setNumberSlot(dp->getSize() >> 12, 0, 3, 0, 0);
            ((ActorTalkRequest *)(this))->setItemNameSlot((u16 *)&id, 0, 7);
        }
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(((VillagerData *)(sp14))->getVillagerId()), sTalkTopicEvInsect.key, 1, r6, sTalkTopicEvInsect.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)((Unk_0201eea4_Fn)(data_020d7c20));
}

void VillagerTalkHobbyTopics::endEvInsect() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
    if (memory != 0) {
        ((VillagerMemory *)(memory))->setTalkedToday();
    }
}

void VillagerTalkHobbyTopics::selectEvGardeniingTalk(void *arg) {
    VillagerData *r4 = actor->villagerData;
    u32 r6 = ((u32)Random_GlobalBelow(10)) & 1;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting((TalkTopicMsg *)arg)) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r4, 0);
        return;
    }
    if (memory == 0 || ((VillagerMemory *)(memory))->isTalkedToday() == 0) {
        ((VillagerTalk *)this)->setTopicFns(sEvGardeniingTopicTable);
    } else if (r6 == 0) {
        ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)((char *)&sEvGardeniingTopicTable[2]));
    } else {
        ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)((char *)&sEvGardeniingTopicTable[1]));
    }
    if (selectFn != 0) {
        (this->*(Unk_0201eea4_Fn)selectFn)(arg);
    }
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r4, 0);
    if (memory != 0) {
        ((VillagerMemory *)(memory))->setTalkedToday();
    }
}

void VillagerTalkHobbyTopics::selectEvGardeniing(TalkTopicMsg *out) {
    u32 a, b;
    s32 t = Event_GetDaysSinceStart(0xe);
    if (t < 0) {
        t = 0;
    }
    if (t == 0) {
        a = 2;
        b = 0;
    } else if (t >= 1 && t <= 2) {
        a = 3;
        b = 2;
    } else if (t >= 3 && t <= 5) {
        a = 3;
        b = 5;
    } else {
        a = 2;
        b = 8;
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvGardeniing.key, 1, b, (u8)a);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::selectEvGardeniingMsg13(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvGardeniing.key, 1, 13, sTalkTopicEvGardeniing.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::selectEvGardeniingMsg10(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvGardeniing.key, 1, 10, sTalkTopicEvGardeniing.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHobbyTopics::continueEvGardeniingMsg10() {
    u8 b;
    TalkTopicMsg res;
    ((VillagerTalk *)this)->setTopicFns((VillagerTalkTopicFns *)((char *)&sEvGardeniingTopicTable[3]));
    if (selectFn != 0) {
        (this->*(Unk_0201eea4_Fn)selectFn)(&res);
    }
    b = res.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)(res.fileName));
}

void VillagerTalkHobbyTopics::selectEtcConnectGardeniing(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkAcornTopics::openSmallTalkChoiceGardeniing() { this->openSmallTalkChoice(); }

extern "C" BOOL Talk_IsAcornItem(u16 *p) {
    BOOL r = FALSE;
    if (*p >= 0x1542 && *p <= 0x1546) {
        r = TRUE;
    }
    return r;
}

extern "C" BOOL Talk_AcornPickerFilter(u16 *p, s32 a) {
    BOOL r = FALSE;
    BOOL in = FALSE;
    if (*p >= 0x1542 && *p <= 0x1546) {
        in = TRUE;
    }
    if (in && a == 0) {
        r = TRUE;
    }
    return r;
}

void VillagerTalkAcornTopics::selectEvAcornTalk(void *arg) {
    VillagerData *r4 = actor->villagerData;
    u32 r6 = Random_GlobalBelow(10) & 1;
    u32 r7 = Random_GlobalBelow(10) & 1;
    PocketMatches tmp;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting((TalkTopicMsg *)arg)) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r4, 0);
        return;
    }
    if (memory == 0 || ((VillagerMemory *)((void *)memory))->isTalkedToday() == 0) {
        ((VillagerTalk *)this)->setTopicFns(sEvAcornTopicTable);
    } else if (r6 == 0) {
        ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[2]);
    } else if (r7 == 0) {
        if (Pocket_CountMatching(&tmp, Talk_IsAcornItem) > 0) {
            ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[4]);
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[2]);
        }
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[1]);
    }
    if (selectFn) {
        (this->*(Unk_0201e5a4_Fn)selectFn)(arg);
    }
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r4, 0);
    if (memory != 0) {
        ((VillagerMemory *)((void *)memory))->setTalkedToday();
    }
}

void VillagerTalkAcornTopics::selectEvAcorn(TalkTopicMsg *out) {
    u32 r4;
    u32 r6;
    s32 v = Event_GetDaysSinceStart(0x10);
    if (v < 0) {
        v = 0;
    }
    if (v == 0) {
        r4 = 2;
        r6 = 0;
    } else if (v >= 1 && v <= 5) {
        r4 = 3;
        r6 = 2;
    } else {
        r4 = 2;
        r6 = 5;
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, r6, r4);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::selectEvAcornMsg10(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 0xa, sTalkTopicEvAcorn.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::selectEvAcornMsg7(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 7, sTalkTopicEvAcorn.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::continueEvAcornMsg7() {
    TalkMsgIndexItem c;
    TalkTopicMsg t;
    ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[3]);
    if (selectFn) {
        (this->*selectFn)(&t);
    }
    c.msgIndex = t.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&c), (void *)t.fileName);
}

void VillagerTalkAcornTopics::selectEtcConnectAcorn(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkAcornTopics::openSmallTalkChoiceAcorn() { this->openSmallTalkChoice(); }

void VillagerTalkAcornTopics::selectEvAcornMsg13(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 0xd, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::openEvAcornGiveChoice() {
    TalkChoiceTable t;
    TalkChoiceTable_Init(this, (&t));
    TalkChoiceTable_Set(this, (&t), 0, 0x5b, 0x5d, &sEvAcornTopicTable[5]);
    TalkChoiceTable_Set(this, (&t), 1, 0x5e, 0x60, &sEvAcornTopicTable[8]);
    t.count = 2;
    t.cancelIndex = t.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&t);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7bc8);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkAcornTopics::openAcornPicker() {
    ((ActorTalkRequest *)this)->setPocketFilter(Talk_AcornPickerFilter, 0xd, 1);
    ((ActorTalkRequest *)this)->openSubScene(0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7968);
}

void VillagerTalkAcornTopics::onAcornPicked() {
    TalkMsgIndexItem c;
    TalkTopicMsg t;
    if (MenuCtrl_IsResultOk() != 0) {
        u32 r4 = MenuCtrl_GetIndex();
        itemFromPlayer = *((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getPocket(r4);
        c.item = 0xfff1;
        Pocket_SetItem(&c.item, 0, r4);
        ((ActorTalkRequest *)(this))->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
        ((VillagerTalk *)this)->setNextTaskDoneFn((Unk_020d8938_Fn)data_020d7ba0);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[8]);
        if (window != NULL) {
            ((TalkWindowState *)window)->nextState = 1;
        }
        if (selectFn) {
            (this->*selectFn)(&t);
        }
        c.msgIndex = t.msgIndex;
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&c), (void *)t.fileName);
    }
}

void VillagerTalkAcornTopics::continueAcornReceived() {
    TalkMsgIndexItem c;
    TalkTopicMsg t;
    ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[6]);
    if (window != NULL) {
        ((TalkWindowState *)window)->nextState = 1;
    }
    if (selectFn) {
        (this->*selectFn)(&t);
    }
    c.msgIndex = t.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&c), (void *)t.fileName);
}

void VillagerTalkAcornTopics::selectEvAcornMsg17(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 0x11, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::rollAcornReward() {
    s32 r4;
    if (memory != 0) {
        r4 = ((VillagerMemory *)((void *)memory))->getFriendship();
    } else {
        r4 = 0;
    }
    if (r4 + 0x100 > Random_GlobalBelow(0x200)) {
        TalkMsgIndexItem c;
        TalkTopicMsg t;
        ((VillagerTalk *)this)->setTopicFns(&sEvAcornTopicTable[7]);
        if (selectFn) {
            (this->*selectFn)(&t);
        }
        c.msgIndex = t.msgIndex;
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&c), (void *)t.fileName);
    } else {
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&(*(TalkMsgIndexItem *)&gTalkMsgIndexEnd)), (void *)(sTalkTopicEvAcorn.key));
    }
}

void VillagerTalkAcornTopics::selectEvAcornMsg19(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 0x13, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::giveAcornReward() {
    u16 a[2];
    BOOL r;
    void *r6 = PlayerData_GetCurrent();
    void *r7 = ((PlayerData *)(r6))->getInventory();
    s32 v = ((PlayerInventory *)(r7))->findEmptyPocket();
    if (v == -1) {
        v = 0;
    }
    if (Item_IsFurniture(&itemFromPlayer) != 0) {
        a[1] = 0x1546;
        if (Item_GetFurnitureIndex(&itemFromPlayer) == Item_GetFurnitureIndex(&a[1])) {
            r = TRUE;
        } else {
            r = FALSE;
        }
    } else {
        if (itemFromPlayer == 0x1546) {
            r = TRUE;
        } else {
            r = FALSE;
        }
    }
    if (r) {
        itemToPlayer = 0x1566;
    } else {
        ItemPickSpec obj;
        obj.set(sAcornRewardItemKinds[Random_GlobalBelow(3)], 0);
        ItemPick_One(a, &obj, NULL, NULL, 1, 1, NULL);
        itemToPlayer = a[0];
    }
    ((PlayerInventory *)((void *)r7))->setPocket((u16 *)(&itemToPlayer), v, 0);
    Catalog_SetItem(static_cast<u8 *>(((PlayerData *)r6)->getCatalog()), &itemToPlayer, 0, 1);
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
    if (itemToPlayer != 0xfff1) {
        ((ActorTalkRequest *)(this))->setItemNameSlot(&itemToPlayer, 0, 7);
    }
}

void VillagerTalkAcornTopics::selectEvAcornMsg15(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvAcorn.key, 1, 0xf, 2);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkAcornTopics::selectEvSnowfesTalk(void *arg) {
    VillagerData *r6 = actor->villagerData;
    VillagerId *v = ((ContestRecord *)(gContestRecord))->getVotedVillager();
    u32 r5 = 2;
    if (((VillagerTalkTopics *)this)->selectSituationGreeting((TalkTopicMsg *)arg)) {
        VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r6, 0);
        return;
    }
    if (Event_GetDaysSinceStart(0x11) == 6 && memory != 0 && ((VillagerMemory *)((void *)memory))->isTalkedToday() != 0) {
        r5 = 3;
    }
    r5 = Random_GlobalBelow(r5);
    if (memory == 0 || ((VillagerMemory *)((void *)memory))->isTalkedToday() == 0 || r5 == 2) {
        ((VillagerTalk *)this)->setTopicFns(sEvSnowfesTopicTable);
    } else if (r5 == 0 && v->isValid() != 0) {
        ((VillagerTalk *)this)->setTopicFns(&sEvSnowfesTopicTable[1]);
    } else {
        ((VillagerTalk *)this)->setTopicFns(&sEvSnowfesTopicTable[2]);
    }
    if (selectFn) {
        (this->*(Unk_0201e5a4_Fn)selectFn)(arg);
    }
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, r6, 0);
    if (memory != 0) {
        ((VillagerMemory *)((void *)memory))->setTalkedToday();
    }
}

void VillagerTalkHolidayTopics::selectEvSnowfes(TalkTopicMsg *out) {
    VillagerData *o = actor->villagerData;
    VillagerId *p = ((ContestRecord *)(gContestRecord))->getHolderVillager();
    s32 v = Event_GetDaysSinceStart(0x11);
    s32 a, b;
    if (v < 0) {
        v = 0;
    }
    if (v == 0) {
        a = 2;
        b = 3;
    } else if (v >= 1 && v <= 5) {
        a = 3;
        b = 5;
    } else if (((SaveData *)((u8 *)&gSaveData))->testFlag(0x11) && p->isValid()) {
        VillagerId *q = o->getVillagerId();
        if (p->townId == q->townId && memcmp(p->townName, q->townName, 8) == 0 && p->species == q->species) {
            a = 1;
            b = 0xb;
        } else {
            a = 3;
            b = 8;
        }
        ((ActorTalkRequest *)(this))->setVillagerNameSlot(p, 0);
    } else {
        a = 2;
        b = 0x12;
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvSnowfes.key, 1, b, (u8)a);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvSnowfesB(TalkTopicMsg *out) {
    VillagerId *p = ((ContestRecord *)(gContestRecord))->getVotedVillager();
    VillagerId *q = (actor->villagerData->getVillagerId());
    s32 x;
    if (p->townId == q->townId && memcmp(p->townName, q->townName, 8) == 0 && p->species == q->species) {
        x = 0xc;
    } else {
        x = 0xf;
    }
    if (p->isValid()) {
        ((ActorTalkRequest *)(this))->setVillagerNameSlot(p, 1);
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvSnowfes.key, 1, x, sTalkTopicEvSnowfes.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvSnowfesC(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvSnowfes.key, 1, 0, sTalkTopicEvSnowfes.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::continueEvSnowfesC() {
    TalkTopicMsg r;
    u8 b;
    ((VillagerTalk *)this)->setTopicFns(&sEvSnowfesTopicTable[3]);
    if (selectFn) {
        (this->*selectFn)(&r);
    }
    b = r.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)r.fileName);
}

void VillagerTalkHolidayTopics::selectEtcConnectSnowfes(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkHolidayTopics::openSmallTalkChoiceSnowfes() { this->openSmallTalkChoice(); }

void VillagerTalkHolidayTopics::selectEvCountdownTalk(TalkTopicMsg *out) {
    VillagerData *o = actor->villagerData;
    ClockDateTime t;
    u32 mn;
    u8 hi, lo;
    *(u32 *)&t = 0;
    *((u32 *)&t + 1) = 0;
    Clock_GetDateTime(&t);
    hi = t.day;
    lo = t.hour;
    mn = t.minute;
    if (hi != 0x1f || lo >= 0x17) {
        if (((VillagerTalkTopics *)this)->selectSituationGreeting(out)) {
            VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, o, 0);
            return;
        }
    }
    if (hi == 0x1f) {
        if (lo < 0x17) {
            ((VillagerTalk *)this)->setTopicFns(sEvCountdownTopicTable);
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[3]);
        }
    } else {
        PersonalitySleepHours *q = Personality_GetSleepHours(VillagerId_GetPersonality(o->getVillagerId()));
        if (lo < q->wakeHour || (lo == q->wakeHour && mn < q->wakeMinute)) {
            ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[4]);
        } else if (memory == 0 || ((VillagerMemory *)(memory))->isTalkedToday() == 0) {
            ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[5]);
        } else {
            ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[6]);
        }
        ((ActorTalkRequest *)(this))->setNumberSlot(t.year + 0x7d0, 0, 4, 0, 0);
    }
    if (selectFn) {
        (this->*selectFn)(out);
    }
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, o, 0);
    if (memory) {
        ((VillagerMemory *)(memory))->setTalkedToday();
    }
}

// a select state (called with the message to fill, as selectGreeting: on the DS a tail call passing it on in r1; the
// decomp lost the argument)
void VillagerTalkHolidayTopics::selectGreetingCountdown(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectGreeting(out);
}

void VillagerTalkHolidayTopics::continueGreetingCountdown() {
    TalkTopicMsg r;
    u8 b;
    ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[1]);
    if (selectFn) {
        (this->*selectFn)(&r);
    }
    b = r.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)r.fileName);
}

void VillagerTalkHolidayTopics::selectEtcConnectCountdownB(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkHolidayTopics::openEvCountdownChoice() {
    TalkChoiceTable buf;
    VillagerTalkTopicFns *p = sSmallTalkTopicTable;
    if ((Random_GlobalBelow(10) & 1) == 0) {
        p = &sEvCountdownTopicTable[2];
    }
    TalkChoiceTable_SetRange(this, (&buf), 0, (const u8 *)sSmallTalkChoiceMsgRange, p);
    TalkChoiceTable_SetRange(this, (&buf), 1, (const u8 *)sLeaveChoiceMsgRange, sEtcCancelTopicTable);
    buf.count = 2;
    buf.cancelIndex = buf.count - 1;
    ((VillagerTalk *)this)->setupChoiceMenu(&buf);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7e98);
    ((TalkWindowState *)((void *)window))->openChoices(1);
}

void VillagerTalkHolidayTopics::selectEvCountdown(TalkTopicMsg *out) {
    ClockDateTime t;
    s32 x;
    *(u32 *)&t = 0;
    *((u32 *)&t + 1) = 0;
    Clock_GetDateTime(&t);
    if (t.hour < 0xc) {
        x = 0;
    } else if (t.hour < 0x11) {
        x = 2;
    } else {
        x = 4;
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvCountdown.key, 1, x, sTalkTopicEvCountdown.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvCountdownB(TalkTopicMsg *out) {
    ClockDateTime t;
    s32 x;
    *(u32 *)&t = 0;
    *((u32 *)&t + 1) = 0;
    Clock_GetDateTime(&t);
    if (t.hour < 0x17 || (t.hour == 0x17 && t.minute < 0x1e)) {
        x = 6;
    } else if (t.minute < 0x37) {
        x = 8;
    } else if (t.minute < 0x3b) {
        x = 10;
    } else {
        x = 12;
    }
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvCountdown.key, 1, x, sTalkTopicEvCountdown.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvCountdownMsg14(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvCountdown.key, 1, 0xe, sTalkTopicEvCountdown.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvCountdownMsg16(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvCountdown.key, 1, 0x10, sTalkTopicEvCountdown.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::selectEvCountdownMsg18(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvCountdown.key, 1, 0x12, 3);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkHolidayTopics::continueEvCountdownMsg18() {
    TalkTopicMsg r;
    u8 b;
    ((VillagerTalk *)this)->setTopicFns(&sEvCountdownTopicTable[7]);
    if (selectFn) {
        (this->*selectFn)(&r);
    }
    b = r.msgIndex;
    ((TalkWindowState *)((void *)window))->setNextMessage((u8 *)(&b), (void *)r.fileName);
}

void VillagerTalkHolidayTopics::selectEtcConnectCountdown(TalkTopicMsg *out) {
    static_cast<VillagerTalkTopics *>(this)->selectEtcConnect(out);  // (a tail call on the DS: the decomp lost out)
}

void VillagerTalkHolidayTopics::openSmallTalkChoiceCountdown() { this->openSmallTalkChoice(); }

void VillagerTalkHolidayTopics::selectTsuAlwaysOnce(TalkTopicMsg *out) {
    ((VillagerTalkTopics *)this)->setTsuTextVariables();
    ((VillagerTalkTopics *)this)->selectTsuAlways();
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    ((VillagerTalk *)this)->clearTopicFns();
}

void VillagerTalkHolidayTopics::selectTsuSpotOnce(TalkTopicMsg *out) {
    ((VillagerTalkTopics *)this)->setTsuTextVariables();
    ((VillagerTalkRumorTopics *)this)->selectTsuSpot();
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    ((VillagerTalk *)this)->clearTopicFns();
}

void VillagerTalkHolidayTopics::selectTsuFriendOnce(TalkTopicMsg *out) {
    ((VillagerTalkTopics *)this)->setTsuTextVariables();
    ((VillagerTalkRumorTopics *)this)->selectTsuFriend();
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    ((VillagerTalk *)this)->clearTopicFns();
}

void VillagerTalkHolidayTopics::selectEvBirthMsg0(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, topicFile, &topicIndex, 0x1e, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 0, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
    closeFn = (Unk_020238b0_Fn)(data_020d7df0);
}

void VillagerTalkHolidayTopics::continueEvBirthMsg0() {
    ((ActorTalkRequest *)(this))->requestCloseWindow(0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7ae8);
}

void VillagerTalkHolidayTopics::startEvBirthMove() {
    volatile VecFx32 v;
    VillagerActor *volatile *pc = (VillagerActor *volatile *)&actor;
    VecFx32 *pv = &(*pc)->position;
    s32 x, z;
    v.x = x = pv->x;
    v.y = pv->y;
    v.z = z = pv->z;
    v.x = x + 0x1e00;
    z -= 0x2800;
    v.z = z;
    (&(*pc)->actionCtrl)->requestAction(2, 2, v.x, z, 0, 0, 0, 0, data_020c6cc8[0], 0);
    updateFn = (Unk_020238b0_Fn)(data_020d7de0);
}

void VillagerTalkHolidayTopics::updateEvBirthMove() {
    if ((&actor->actionCtrl)->getAction() == 2) {
        if ((&actor->actionCtrl)->isActionDone()) {
            (&actor->actionCtrl)->requestStand(2, data_020c6cc8[0]);
            updateFn = (Unk_020238b0_Fn)(data_020d7aa8);
        }
    }
}

void VillagerTalkTopics::updateEvBirthTurn() {
    TalkTopicMsg out;
    u8 b;
    VecFx32 v;
    if ((&actor->actionCtrl)->getAction() == 0 && (&actor->actionCtrl)->isActionDone() != 0) {
        v = *(&actor->position);
        v.y += 0x2000;
        Camera_FocusOnPoint(&v);
        ((VillagerTalk *)this)->setTopicFns(sEvBirthTopicTable);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        b = out.msgIndex;
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
        ((ActorTalkRequest *)(this))->requestReopenWindow();
        updateFn = nullptr;
    }
}

void VillagerTalkTopics::playBirthdayBgm() {
    Bgm_ReleasePriority(0x12);
    Bgm_RequestSilence(0xc, 0, 0xb);
    Bgm_Request(0xd, 0x2f, 0x7f, 1);
}

void VillagerTalkTopics::stopBirthdayBgm() {
    Bgm_Release(0x2f);
    Bgm_RequestSilence(0xc, 0x60, 0x79);
}

void VillagerTalkTopics::selectEvBirthMsg1(TalkTopicMsg *out) {
    void *r7;
    VillagerId *r4;
    s32 i;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 1, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    if (PlayerData_GetCurrent() != 0) {
        r7 = PlayerData_GetCurrent()->getPlayerId();
    } else {
        r7 = 0;
    }
    if (actor->villagerData != 0) {
        r4 = (actor->villagerData->getVillagerId());
    } else {
        r4 = 0;
    }
    partnerMask = 0;
    partnerCount = 0;
    if (gSaveVillagers != NULL && r7 != 0 && ((PlayerId *)(r7))->isValid() != 0 && r4 != 0 && r4->isValid() != 0) {
        for (i = 0; i < 8; i++) {
            void *v = SaveVillagers_Get(gSaveVillagers, i);
            if (v != 0 && (((VillagerData *)(v))->getVillagerId())->isValid() != 0) {
                VillagerId *w = ((VillagerData *)(v))->getVillagerId();
                if (w->townId != r4->townId || memcmp(w->townName, r4->townName, 8) != 0 || w->species != r4->species) {
                    void *m;
                    if ((m = Villager_FindMemory(v, r7)) != 0 && ((VillagerMemory *)(m))->getFriendship() >= 0x40) {
                        partnerMask |= 1 << i;
                        partnerCount++;
                    }
                }
            }
        }
    }
    if (partnerCount == 1) {
        partnerMask = 0;
        partnerCount = 0;
    }
    closeFn = (Unk_020238b0_Fn)(data_020d7d70);
}

void VillagerTalkTopics::continueEvBirthFriends() {
    u8 buf[2];
    u16 h;
    TalkTopicMsg out;
    if (partnerCount >= 2) {
        ((VillagerTalk *)this)->setTopicFns(&sEvBirthTopicTable[1]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        buf[0] = out.msgIndex;
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&buf[0]), (void *)(out.fileName));
    } else if (partnerCount == 1) {
        ((VillagerTalk *)this)->setTopicFns(&sEvBirthTopicTable[2]);
        if (selectFn) {
            (this->*selectFn)(&out);
        }
        buf[1] = out.msgIndex;
        ((TalkWindowState *)(window))->setNextMessage((u8 *)(&buf[1]), (void *)(out.fileName));
    } else {
        h = 0x3818;
        ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&h), 0, 5, 0);
        ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7c88);
    }
}

void VillagerTalkTopics::giveEvBirthPresent() {
    u8 b;
    u16 h;
    TalkTopicMsg out;
    void *p;
    h = 0x3818;
    Pocket_AddItem(&h, 0);
    p = PlayerData_GetCurrent();
    ((PlayerData *)(p))->setBirthdayTalkYear((u8)Clock_GetYear());
    ((VillagerTalk *)this)->setTopicFns(&sEvBirthTopicTable[4]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectEvBirthMsg5(TalkTopicMsg *out) {
    s32 i;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 5, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    for (i = 0; i < 8; i++) {
        if ((partnerMask >> i) & 1) {
            ((ActorTalkRequest *)(this))->setVillagerNameSlot(((VillagerData *)(SaveVillagers_Get(gSaveVillagers, i)))->getVillagerId(), 0);
            partnerMask &= ~(1 << i);
            partnerCount--;
            break;
        }
    }
}

void VillagerTalkTopics::selectEvBirthMsg6(TalkTopicMsg *out) {
    s32 i;
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 6, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    for (i = 0; i < 8; i++) {
        if ((partnerMask >> i) & 1) {
            ((ActorTalkRequest *)(this))->setVillagerNameSlot(((VillagerData *)(SaveVillagers_Get(gSaveVillagers, i)))->getVillagerId(), 0);
            partnerMask &= ~(1 << i);
            partnerCount--;
            break;
        }
    }
}

void VillagerTalkTopics::continueEvBirthMsg6() {
    u8 b;
    TalkTopicMsg out;
    ((VillagerTalk *)this)->setTopicFns(&sEvBirthTopicTable[3]);
    if (selectFn) {
        (this->*selectFn)(&out);
    }
    b = out.msgIndex;
    ((TalkWindowState *)(window))->setNextMessage((u8 *)(&b), (void *)(out.fileName));
}

void VillagerTalkTopics::selectEvBirthMsg7(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 7, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::continueEvBirthMsg7() {
    u16 h = 0x3818;
    ((ActorTalkRequest *)(this))->requestGiveItem((u16 *)(&h), 0, 5, 0);
    ((VillagerTalk *)this)->setTaskDoneFn((Unk_020d8938_Fn)data_020d7d20);
}

void VillagerTalkTopics::selectEvBirthMsg2(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 2, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::openEvBirthChoice() {
    TalkChoiceTable s;
    TalkChoiceTable_Init(this, (&s));
    TalkChoiceTable_Set(this, (&s), 0, 0x32, 0x32, &sEvBirthTopicTable[5]);
    TalkChoiceTable_Set(this, (&s), 1, 0x33, 0x33, &sEvBirthTopicTable[6]);
    s.count = 2;
    s.cancelIndex = -1;
    ((VillagerTalk *)this)->setupChoiceMenu(&s);
    ((VillagerTalk *)this)->setChoiceFn((Unk_020d8938_Fn)data_020d7d18);
    ((TalkWindowState *)(window))->openChoices(1);
}

void VillagerTalkTopics::selectEvBirthMsg3(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 3, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectEvBirthMsg4(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEvBirth.key, 1, 4, sTalkTopicEvBirth.variantCount);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
}

void VillagerTalkTopics::selectAiFall(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicAiFall.key, sTalkTopicAiFall.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    if (memory == 0) {
        noMemoryUpdate = 1;
    }
    onEndFn = (Unk_020238b0_Fn)(data_020d7990);
}

void VillagerTalkTopics::endAiFall() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
}

void VillagerTalkTopics::selectEtcHit(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEtcHit.key, sTalkTopicEtcHit.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7c68);
}

void VillagerTalkTopics::endEtcPushHit() {
    VillagerTalk_EnsureMemory((VillagerTalk *)this, &memory, &memoryIndex, actor->villagerData, 0);
}

void VillagerTalkTopics::selectEtcPush(TalkTopicMsg *out) {
    Talk_SelectTopicMessage(this, &topicFile, &topicIndex, 30, VillagerId_GetPersonality(actor->villagerData->getVillagerId()), sTalkTopicEtcPush.key, sTalkTopicEtcPush.variantCount, 0, 0);
    out->fileName = topicFile;
    out->msgIndex = topicIndex;
    onEndFn = (Unk_020238b0_Fn)(data_020d7958);
}

s32 VillagerTalk::attrGiveItem() {
    u16 buf = itemFromPlayer;
    if (itemToPlayer != 0xfff1) {
        buf = itemToPlayer;
    }
    if (buf != 0xfff1) {
        void *r4 = PlayerData_GetCurrent();
        PlayerInventory *r6 = (PlayerInventory *)((PlayerData *)(r4))->getInventory();
        s32 r2 = r6->findEmptyPocket();
        if (r2 != -1) {
            r6->setPocket((u16 *)(&buf), r2, 0);
            Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(r4))->getCatalog()), &buf, 0, 1);
            this->requestGiveItem((u16 *)(&buf), 0, 5, 0);
            return 1;
        }
    }
    return 0;
}

s32 VillagerTalk::attrShowLetter() {
    LetterView *r4;
    if (unk_134_p != NULL) {
        r4 = reinterpret_cast<LetterView *>(actor->villagerData->getLetter());  // (LetterView: a view of the Letter)
    } else {
        VillagerId *r6 = actor->villagerData->getVillagerId();
        MsgString25B l18;
        MsgString129 l78;
        MsgString33B l44;
        u8 v = 0;
        u8 b;
        u32 x14;
        r4 = (LetterView *)&sTalkLetter;  // (LetterView: the field view of a Letter)
        if (unk_128_p != NULL) {
            v = ((VillagerMemory *)(unk_128_p))->pickUnusedTopic();
        }
        b = v;
        MailText_LoadLetter(&l18, &l78, &l44, &x14, &b, "ap_secret");
        Letter_FillVillagerToVillager(r4, &l18, &l78, &l44, x14, ((u8 *)""), r6, r6);
    }
    this->setMenu12Arg(r4, 0);
    this->openSubScene(5);
    return 1;
}

s32 VillagerTalk::attrPlayMemoryTune() {
    if (unk_134_p != NULL) {
        this->requestPlayMelody((Vec2 *)(unk_134_p + 0x5c));
        return 1;
    }
    return 0;
}

s32 VillagerTalk::buyItemFromPlayer() {
    if (unk_19c_s > 0 && itemFromPlayer != 0xfff1) {
        u8 *r4 = (u8 *)PlayerData_GetCurrent()->getInventory();
        s32 r2 = Pocket_FindItem(&itemFromPlayer);
        if (r2 >= 0) {
            u16 buf[2];
            buf[0] = 0xfff1;
            ((PlayerInventory *)r4)->setPocket((u16 *)(buf), r2, 0);
            PlayerInventory_AddBells(r4, unk_19c_s, 1);
            Talk_GetMoneyItem(&buf[1], unk_19c_s);
            itemToPlayer = buf[1];
            this->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
            this->setTaskDoneFn(data_020d7bb8);
            return 1;
        }
    }
    return 0;
}

void VillagerTalk::receiveItem() {
    VillagerData *r4 = actor->villagerData;
    this->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
    if (r4 != NULL) {
        if (itemFromPlayer != 0xfff1) {
            Villager_AddReceivedItem(r4, &itemFromPlayer);
            if (unk_128_p != NULL) {
                ((VillagerMemory *)(unk_128_p))->setReceivedItem((u16 *)(&itemFromPlayer));
            }
        }
    }
    ((TalkWindowState *)((void *)window))->lockAdvance();
}

s32 VillagerTalk::swapItemWithPlayer() {
    if (itemToPlayer != 0xfff1 && itemFromPlayer != 0xfff1) {
        void *r4 = PlayerData_GetCurrent();
        u8 *r6 = (u8 *)((PlayerData *)(r4))->getInventory();
        s32 r2 = Pocket_FindItem(&itemFromPlayer);
        if (r2 >= 0) {
            ((PlayerInventory *)r6)->setPocket((u16 *)(&itemToPlayer), r2, 0);
            Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(r4))->getCatalog()), &itemToPlayer, 0, 1);
            this->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
            this->setTaskDoneFn(data_020d7b90);
            return 1;
        }
    }
    return 0;
}

void VillagerTalk::receiveItemB() {
    VillagerData *r4 = actor->villagerData;
    this->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
    if (r4 != NULL) {
        if (itemFromPlayer != 0xfff1) {
            Villager_AddReceivedItem(r4, &itemFromPlayer);
            if (unk_128_p != NULL) {
                ((VillagerMemory *)(unk_128_p))->setReceivedItem((u16 *)(&itemFromPlayer));
            }
        }
    }
    ((TalkWindowState *)((void *)window))->lockAdvance();
}

s32 VillagerTalk::sellItemToPlayer() {
    if (unk_19c_s > 0 && itemToPlayer != 0xfff1) {
        void *r6 = PlayerData_GetCurrent();
        u8 *r4 = (u8 *)((PlayerData *)(r6))->getInventory();
        s32 r2 = ((PlayerInventory *)r4)->findEmptyPocket();
        if (r2 >= 0) {
            ((PlayerInventory *)r4)->setPocket((u16 *)(&itemToPlayer), r2, 0);
            Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(r6))->getCatalog()), &itemToPlayer, 0, 1);
            if (itemToPlayerIsReceived == 1) {
                Villager_RemoveReceivedItem(actor->villagerData, &itemToPlayer);
            }
            PlayerInventory_AddBells(r4, -unk_19c_s, 1);
            this->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
            this->setTaskDoneFn(data_020d7f38);
            return 1;
        }
    }
    return 0;
}

void VillagerTalk::showMoneyItem() {
    u16 tmp;
    Talk_GetMoneyItem(&tmp, unk_19c_s);
    itemFromPlayer = tmp;
    this->requestTakeItem((u16 *)(&itemFromPlayer), 0, 5, 0);
    ((TalkWindowState *)((void *)window))->lockAdvance();
}

s32 VillagerTalk::giveItemToPlayer() {
    if (itemToPlayer != 0xfff1) {
        void *r4 = PlayerData_GetCurrent();
        u8 *r6 = (u8 *)((PlayerData *)(r4))->getInventory();
        s32 r2 = ((PlayerInventory *)r6)->findEmptyPocket();
        if (r2 >= 0) {
            ((PlayerInventory *)r6)->setPocket((u16 *)(&itemToPlayer), r2, 0);
            Catalog_SetItem(static_cast<u8 *>(((PlayerData *)(r4))->getCatalog()), &itemToPlayer, 0, 1);
            if (itemToPlayerIsReceived == 1) {
                Villager_RemoveReceivedItem(actor->villagerData, &itemToPlayer);
            }
            this->requestGiveItem((u16 *)(&itemToPlayer), 0, 5, 0);
            return 1;
        }
    }
    return 0;
}

s32 VillagerTalk::setHiraganaOn() {
    PlayerOptions_SetHiragana(1);
    PlayerOptions_Commit();
    return 1;
}

s32 VillagerTalk::setHiraganaOff() {
    PlayerOptions_SetHiragana(0);
    PlayerOptions_Commit();
    return 1;
}

s32 VillagerTalk::attrOpenBirthdayEntry() {
    this->setSubSceneKind(0x32, 0);
    this->openSubScene(2);
    this->setTaskDoneFn(data_020d79a0);
    return 1;
}

void VillagerTalk::setConstellationSlots() {
    u8 buf[2];
    u8 *p = PlayerData_GetCurrent()->getBirthday();
    u32 r = Date_GetStarSign(p[1], p[0]);
    this->setMonthSlot(p[1], 0);
    this->setDaySlot(p[0], 1);
    buf[0] = r;
    buf[1] = 0;
    this->setSlotFromString(2, buf, "st_constellation");
}

s32 VillagerTalk::runCustomFn0() {
    if (unk_168_s != 0) {
        return (this->*unk_168_s)();
    }
    return 0;
}

s32 VillagerTalk::runCustomFn1() {
    if (unk_170_s != 0) {
        return (this->*unk_170_s)();
    }
    return 0;
}

s32 VillagerTalk::runCustomFn2() {
    if (unk_178_s != 0) {
        return (this->*unk_178_s)();
    }
    return 0;
}

s32 VillagerTalk::runCustomFn3() {
    if (unk_180_s != 0) {
        return (this->*unk_180_s)();
    }
    return 0;
}

s32 VillagerTalk::runCustomFn4() {
    if (unk_188_s != 0) {
        return (this->*unk_188_s)();
    }
    return 0;
}

s32 VillagerTalk::runMsgAttrHandler(u32 idx) {
    static Unk_020d8938_FnS tbl[17] = {
        0, data_020d7b28, 0, 0, 0, 0, 0, data_020d7b00,
        0, 0, 0, 0, data_020d7a20, data_020d7af0, data_020d7d50,
        data_020d7cf0, data_020d7df8};
    if (idx < 17) {
        if (tbl[idx] != 0) {
            return (this->*tbl[idx])();
        }
    }
    return 0;
}

extern "C" {
VillagerTalkTopicFns sApSubTopics[13];
decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d79c0 = &VillagerTalkTopics::onRequestItemPicked;
const TalkTopic sTalkTopicQ01Pwin2 = {sTalkKeyQ01Pwin2, 0x1};
const TalkTopic sTalkTopicQStart = {sTalkKeyQStart, 0x3};
const TalkTopic sTalkTopicApItem = {sTalkKeyApItem, 0x2};
const TalkTopic sTalkTopicQItem = {sTalkKeyQItem, 0x3};
char sTalkKeyQ01Nwin[9] = "q01_nwin";
char sTalkKeyApItem[8] = "ap_item";
const TalkTopic sTalkTopicAiBee = {sTalkKeyAiBee, 0x3};
const TalkTopic sTalkTopicTsuClAct = {sTalkKeyTsuClAct, 0x4};
const TalkTopic sTalkTopicQ02Pay = {sTalkKeyQ02Pay, 0x3};
char sTalkKeyAiAnger[9] = "ai_anger";
decltype(&VillagerTalkRumorTopics::pickMemoryWithCompliment) data_020d7a78 = &VillagerTalkRumorTopics::pickMemoryWithCompliment;
VillagerTalkTopicFns sEvKaraokeTopicTable[9];
decltype(&VillagerTalkTopics::cancelRequestChain) data_020d7e40 = &VillagerTalkTopics::cancelRequestChain;
decltype(&VillagerTalkTopics::compareCatchPrice) data_020d7e28 = &VillagerTalkTopics::compareCatchPrice;
char sTalkKeyAiShop3[9] = "ai_shop3";
TalkTopic sTalkTopicQThanksDefault = {NULL, 0x00000003};
decltype(&VillagerTalkHolidayTopics::updateEvBirthMove) data_020d7de0 = &VillagerTalkHolidayTopics::updateEvBirthMove;
char sTalkKeyAiPoison[10] = "ai_poison";
const TalkTopic sTalkTopicQ12FullC = {sTalkKeyQ12Full, 0x2};
const TalkTopic sTalkTopicEtcPush = {sTalkKeyEtcPush, 0x5};
char sTalkKeyAiRain2[9] = "ai_rain2";
const TalkTopic sTalkTopicEvArbeit = {sTalkKeyEvArbeit, 0x1};
char sTalkKeyTsuItem[9] = "tsu_item";
char sTalkKeyAiRun[7] = "ai_run";
decltype(&VillagerTalkTopics::onNloseCatchPicked) data_020d7b20 = &VillagerTalkTopics::onNloseCatchPicked;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7d28 = &VillagerTalkRequestReplyTopics::runChosenTopic;
}

void VillagerTalk::onMessageStart(u32 a) {
    if (runMsgAttrHandler(a) == 0) {
        if (unk_b4_a != 0) {
            (this->*unk_b4_a)(a);
            unk_b4_a = 0;
        }
    }
}

void VillagerTalk::onMessageEnd(u32 a) {
    volatile u8 v = *((u8 *)((void *)window) + 0x19f7);
    if (runMsgAttrHandler(a) == 0) {
        if (unk_156 != 0) {
            Snd_PlaySe(unk_156);
            unk_156 = 0;
        }
        u8 w = v;
        if (w == (*(u8 *)&gTalkMsgIndexNone)) {
            if (unk_bc_a != 0) {
                (this->*unk_bc_a)(a);
            }
        } else if (w == gTalkMsgIndexEnd) {
            if (unk_c4_a != 0) {
                (this->*unk_c4_a)(a);
                unk_c4_a = 0;
            }
        }
    }
    if (v == gTalkMsgIndexEnd) {
        if (unk_128_p != NULL) {
            Villager_UpdateImpression(actor->villagerData, unk_128_p, 0);
            if (memoryIndex != -1) {
                VillagerSync_Impression(actor->villagerData, memoryIndex, ((VillagerMemory *)(unk_128_p))->getImpression());
            }
            if (actor->villagerData != NULL) {
                Villager_UpdateVisitorRecord(actor->villagerData, 0);
            }
        }
        if (unk_130_p != NULL) {
            if ((((VillagerTalk *)(actor))->getPartner()) != NULL) {
                Villager_UpdateImpression((((VillagerTalk *)(actor))->getPartner())->villagerData, unk_130_p, 0);
            }
        }
    }
}

extern "C" void TalkChoiceTable_Init(void *a, TalkChoiceTable *t) {
    for (s32 i = 0; i < 5; i++) {
        t->range[i][0] = 0;
        t->range[i][1] = 0;
        t->val[i] = 0;
    }
    t->count = 0;
    t->cancelIndex = -1;
}

extern "C" void TalkChoiceTable_Set(void *a, TalkChoiceTable *t, u32 i, u8 lo, u8 hi, VillagerTalkTopicFns *val) {
    t->range[i][0] = lo;
    t->range[i][1] = hi;
    t->val[i] = val;
}

extern "C" void TalkChoiceTable_SetRange(void *a, TalkChoiceTable *t, u32 i, const u8 *r, VillagerTalkTopicFns *val) {
    TalkChoiceTable_Set((void *)a, t, i, r[0], r[1], val);
}

void VillagerTalk::clearChoiceValues() {
    for (s32 i = 0; i < 5; i++) {
        choiceValues[i] = 0;
    }
}

void VillagerTalk::setupChoiceMenu(void *t_) {
    TalkChoiceTable *t = (TalkChoiceTable *)t_;
    void *h = this->getChoiceList();
    if (h != NULL) {
        clearChoiceValues();
        ((ChoiceList *)(h))->reset(t->count, t->cancelIndex);
        for (s32 i = 0; i < t->count; i++) {
            u8 r = t->range[i][0] + Random_GlobalBelow(t->range[i][1] - t->range[i][0] + 1);
            ((ChoiceList *)(h))->setEntry(i, (const u8 *)(&r), 0, (const u8 *)(gTalkMsgIndexNone), (const char *)0, 0);
            choiceValues[i] = t->val[i];
        }
        ((ChoiceList *)(h))->loadTexts();
    }
}

void VillagerTalk::onChoice(u32 a) {
    if (unk_cc_2) {
        void *h = this->getChoiceList();
        s32 x;
        if (h != NULL) {
            x = ((ChoiceList *)(h))->getResult();
        } else {
            x = -1;
        }
        (this->*unk_cc_2)(a, x);
        unk_cc_2 = nullptr;
    }
}

void VillagerTalk::setPartner(VillagerActor *v) { ((VillagerActor *)this)->talkPartner = v; }

VillagerActor *VillagerTalk::getPartner() { return ((VillagerActor *)this)->talkPartner; }

void VillagerTalk::setInvitedByPartner(u8 v) { ((VillagerActor *)this)->invitedByPartner = v; }

u8 VillagerTalk::isInvitedByPartner() { return ((VillagerActor *)this)->invitedByPartner; }

s32 VillagerTalk::canAcceptPartnerInvite() { return 0; }

s32 VillagerTalk::acceptPartnerInvite() { return 0; }

s32 VillagerTalk::endPartnerTalk() { return 0; }

// VillagerActor's: the same functions on the DS (symbols.txt labels of VillagerTalk's)
s32 VillagerActor::canAcceptPartnerInvite() { return 0; }

s32 VillagerActor::acceptPartnerInvite(VillagerActor *) { return 0; }

s32 VillagerActor::endPartnerTalk() { return 0; }

BOOL VillagerTalk::hasPartner() {
    if (((VillagerActor *)this)->talkPartner != NULL) {
        return TRUE;
    }
    return FALSE;
}

void VillagerTalk::refreshEventKind() {
    ((VillagerActor *)this)->eventKind = 0xb;
    if ((*(CommManager * *)&gCommManager)->isSlotActive((*(CommManager * *)&gCommManager)->myAid) == 0) {
        ((VillagerActor *)this)->eventKind = VillagerEvent_GetTodayIndex();
    }
}

u8 VillagerTalk::getEventKind() { return ((VillagerActor *)this)->eventKind; }

extern "C" void *VillagerMood_Construct(void *p) {
    new (p) SndSeEmitter();
    return p;
}

extern "C" void *VillagerMood_Destruct(void *p) {
    ((SndSeEmitter *)(p))->SndSeEmitter::~SndSeEmitter();
    return p;
}

void VillagerMood::reset() {
    active = 0;
    currentMood = 5;
    effectMood = 5;
    effectFrame = 0;
    effectFn = nullptr;
    clearPending();
    applyRequested = 0;
    effectsOn = 0;
}

void VillagerMood::start() {
    reset();
    ((SndSeEmitter *)(this))->callInit();
    effectsOn = 1;
    active = 1;
}

void VillagerMood::stop() {
    if (isActive()) {
        ((SndSeEmitter *)(this))->callStop();
    }
    active = 0;
}

BOOL VillagerMood::isActive() {
    if (active != 0) {
        return TRUE;
    }
    return FALSE;
}

void VillagerMood::setMoodAnimation(VillagerActor *s, u32 mode) {
    void *r = s->getVillagerData();
    if (mode == 4 && r != 0) {
        s32 t = VillagerId_GetPersonality(((VillagerData *)(r))->getVillagerId());
        if (t == 0 || t == 3) {
            mode = 3;
        } else {
            mode = 2;
        }
    }
    s->moveAnimSet.setStandAnim(sMoodAnimIds[mode]);
    s->moveAnimSet.setWalkAnim(sMoodAnimNextIds[mode]);
}

void VillagerMood::clearPending() {
    pendingMood = 5;
    pendingTime = 0;
}

void VillagerMood::addMood(u32 a, s32 b) {
    if ((*(CommManager * *)&gCommManager)->isSlotActive((*(CommManager * *)&gCommManager)->myAid) == 0) {
        if (a < 5) {
            u16 t = b * 0x4b0;
            if (a == pendingMood) {
                pendingTime = pendingTime + t;
            } else {
                pendingMood = a;
                pendingTime = t;
            }
        }
    }
}

void VillagerMood::requestApply() {
    if ((*(CommManager * *)&gCommManager)->isSlotActive((*(CommManager * *)&gCommManager)->myAid) == 0) {
        applyRequested = 1;
    }
}

void VillagerMood::playMoodEffect(VillagerActor *s, u32 a, u32 b) {
    if (effectsEnabled()) {
        VecFx32 v;
        s16 h;
        v.x = s->jointPos[0].x;
        v.y = s->jointPos[0].y;
        v.z = s->jointPos[0].z;
        h = s->rotY;
        Snd_SeEmitterPlayOneShot((SndSeEmitter *)pad_00, b, 0x7f, 0);
        Effect_Create(a, &v, &h, 0);
    }
}

void VillagerMood::updateSoundPos(VillagerActor *s) {
    VecFx32 v;
    VecFx32 *p = &s->position;
    v = *p;
    ((SndSeEmitter *)(this))->callUpdateRelative(&v);
}

void VillagerMood::enableEffects() { effectsOn = 1; }

void VillagerMood::disableEffects() { effectsOn = 0; }

BOOL VillagerMood::effectsEnabled() {
    if (effectsOn != 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL VillagerMood::isMoodAnim(VillagerActor *s) {
    s32 v = s->animCtrl.getAnimId(0);
    const u32 *p = sMoodAnimIds;
    const u32 *q = sMoodAnimNextIds;
    for (s32 i = 0; i < 5; p++, q++, i++) {
        if (v == *p || v == *q) {
            return TRUE;
        }
    }
    return FALSE;
}

void VillagerMood::playMood1EffectA(VillagerActor *s) { playMoodEffect(s, 0x5a, 0x7b); }

void VillagerMood::playMood1EffectB(VillagerActor *s) { playMoodEffect(s, 0x5b, 0x7b); }

BOOL VillagerMood::beginMood1Effects(VillagerActor *s) {
    effectFrame = 0;
    effectFn = data_020d7e80;
    return TRUE;
}

void VillagerMood::updateMood1Effects(VillagerActor *s) {
    if (isMoodAnim(s)) {
        if (effectFrame == 0) {
            playMood1EffectA(s);
        } else if (effectFrame == 0x14) {
            playMood1EffectB(s);
        }
        effectFrame = effectFrame + 1;
        if (effectFrame >= 0x28) {
            effectFrame = 0;
        }
    }
}

void VillagerMood::playMood2Effect(VillagerActor *s) { playMoodEffect(s, 0x5c, 0x83); }

BOOL VillagerMood::beginMood2Effects(VillagerActor *s) {
    effectFrame = 0;
    effectFn = data_020d7e08;
    return TRUE;
}

void VillagerMood::updateMood2Effects(VillagerActor *s) {
    if (isMoodAnim(s)) {
        if (effectFrame == 0) {
            playMood2Effect(s);
        }
        effectFrame = effectFrame + 1;
        if (effectFrame >= 0x14) {
            effectFrame = 0;
        }
    }
}

void VillagerMood::playMood3Effect(VillagerActor *s) { playMoodEffect(s, 0x62, 0x7f); }

BOOL VillagerMood::beginMood3Effects(VillagerActor *s) {
    effectFrame = 0;
    effectFn = data_020d7ad0;
    return TRUE;
}

void VillagerMood::updateMood3Effects(VillagerActor *s) {
    if (isMoodAnim(s)) {
        if (effectFrame == 0) {
            playMood3Effect(s);
        }
        effectFrame = effectFrame + 1;
        if (effectFrame >= 0xe) {
            effectFrame = 0;
        }
    }
}

BOOL VillagerMood::isMoodAnimStart(VillagerActor *s) {
    s32 v = s->animCtrl.getAnimId(0);
    const u32 *p = sMoodAnimIds;
    for (s32 i = 0; i < 5; p++, i++) {
        if (v == *p) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerMood::startMoodAnim(VillagerActor *s, u32 idx) {
    static Unk_0201c078_Fn tbl[5] = {
        nullptr,
        data_020d7e68,
        data_020d7a38,
        data_020d7b68,
        data_020d7bf0,
    };
    if (idx < 5) {
        if (tbl[idx]) {
            if ((this->*tbl[idx])(s)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

extern "C" {
decltype(&VillagerTalkRumorTopics::selectTsuHome) data_020d7f10 = &VillagerTalkRumorTopics::selectTsuHome;
const TalkTopic sTalkTopicTsuFoHint = {sTalkKeyTsuFoHint, 0x3};
const TalkTopic sTalkTopicAiIndoor = {sTalkKeyAiIndoor, 0x3};
const TalkTopic sTalkTopicTsuFuAct = {sTalkKeyTsuFuAct, 0x4};
decltype(&VillagerTalkTopics::tryAddActiveRequestChoice) data_020d7d00 = &VillagerTalkTopics::tryAddActiveRequestChoice;
char sTalkKeyQ07Scold[10] = "q07_scold";
const TalkTopic sTalkTopicEvGardeniing = {sTalkKeyEvGardeniing, 0x3};
const TalkTopic sTalkTopicQError2 = {sTalkKeyQError2, 0x2};
const u8 sTsuTopicWeights[14] = {
    0x08, 0x0c, 0x04, 0x04, 0x0c, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0a,
};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7950 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicAiPersis = {sTalkKeyAiPersis, 0x3};
decltype(&VillagerTalkTopics::selectApNickn) data_020d7a18 = &VillagerTalkTopics::selectApNickn;
char sTalkKeyQ07Show[9] = "q07_show";
decltype(&VillagerTalk::setHiraganaOff) data_020d7fd8 = &VillagerTalk::setHiraganaOff;
char sTalkKeyQPreitem[10] = "q_preitem";
decltype(&VillagerTalk::receiveItem) data_020d7bb8 = &VillagerTalk::receiveItem;
const TalkTopic sTalkTopicQPreitem = {sTalkKeyQPreitem, 0x3};
char sTalkKeyQ04Req37[11] = "q04_req3_7";
decltype(&VillagerTalkTopics::selectTsuClHint) data_020d7f28 = &VillagerTalkTopics::selectTsuClHint;
const u8 data_020c74f4[2] = {
    0x1d, 0x1d,
};
char sTalkKeyTsuClAct[11] = "tsu_cl_act";
const TalkTopic sTalkTopicApHabit = {sTalkKeyApHabit, 0x2};
const TalkTopic sTalkTopicTsuFlAct = {sTalkKeyTsuFlAct, 0x4};
decltype(&VillagerTalkTopics::selectTsuEvent) data_020d7b30 = &VillagerTalkTopics::selectTsuEvent;
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d78f8 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicQ02Nwin = {sTalkKeyQ02Nwin, 0x3};
decltype(&VillagerTalkTopics::playBirthdayBgm) data_020d7df0 = &VillagerTalkTopics::playBirthdayBgm;
decltype(&VillagerTalk::runCustomFn1) data_020d7af0 = &VillagerTalk::runCustomFn1;
const TalkTopic sTalkTopicAiRain1 = {sTalkKeyAiRain1, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7a98 = &VillagerTalkRequestReplyTopics::runChosenTopic;
const TalkTopic sTalkTopicTsuSpot = {sTalkKeyTsuSpot, 0x3};
decltype(&VillagerTalkRequestReplyTopics::runChosenTopic) data_020d7960 = &VillagerTalkRequestReplyTopics::runChosenTopic;
decltype(&VillagerMood::beginMood1Effects) data_020d7e68 = &VillagerMood::beginMood1Effects;
u8 sInsectSlotFlags[8];
char sTalkKeyQYes[6] = "q_yes";
char sTalkKeyTsuInHint[12] = "tsu_in_hint";
const TalkTopic sTalkTopicTsuDrama = {sTalkKeyTsuDrama, 0x3};
char sVillagerClothMaterialName[2] = "w";
const u8 sLeaveChoiceMsgRange[2] = {
    0x0a, 0x13,
};
decltype(&VillagerMood::beginMood2Effects) data_020d7bf0 = &VillagerMood::beginMood2Effects;
u8 sVillagerTexturePathBuf[32];
char sTalkKeyTsuMove2[10] = "tsu_move2";
const s8 sDangerousInsects[2] = {
    55, 54,
};
char sTalkKeyApMail[8] = "ap_mail";
const TalkTopic sTalkTopicTsuNoAct = {sTalkKeyTsuNoAct, 0x4};
decltype(&VillagerTalkRequestStartTopics::registerRequestDeferred) data_020d7ec8 = &VillagerTalkRequestStartTopics::registerRequestDeferred;
decltype(&VillagerTalkTopics::selectTsuFiAct) data_020d7af8 = &VillagerTalkTopics::selectTsuFiAct;
const TalkTopic sTalkTopicTsuStar = {sTalkKeyTsuStar, 0x2};
const u8 data_020c7518[5] = {
    0x03, 0x02, 0x05, 0x00, 0x01,
};
decltype(&VillagerTalkTopics::selectTsuHobbyHint) data_020d7fa8 = &VillagerTalkTopics::selectTsuHobbyHint;
char sTalkKeyEvInsect[10] = "ev_insect";
char sTalkKeyTsuFoHint[12] = "tsu_fo_hint";
decltype(&VillagerTalkHobbyTopics::endEvFishing) data_020d7a28 = &VillagerTalkHobbyTopics::endEvFishing;
char sTalkKeyQ05Talk12[12] = "q05_talk1_2";
const char **sTalkKeysQReq[5] = {
    sTalkKeysQ01Req, sTalkKeysQ02Req, sTalkKeysQ03Req, sTalkKeysQ04Req, sTalkKeysQ05Req,
};
decltype(&VillagerTalkTopics::onRequestItemPicked) data_020d7a58 = &VillagerTalkTopics::onRequestItemPicked;
const u8 data_020c74f0[2] = {
    0x2a, 0x2a,
};
decltype(&VillagerTalkKaraokeTopics::waitEvKaraokeAction) data_020d7d60 = &VillagerTalkKaraokeTopics::waitEvKaraokeAction;
decltype(&VillagerTalkTopics::giveEvBirthPresent) data_020d7c88 = &VillagerTalkTopics::giveEvBirthPresent;
char sTalkKeyQ06Open2[10] = "q06_open2";
char sTalkKeyAiMfirst[10] = "ai_mfirst";
decltype(&VillagerTalkTopics::selectTsuFiHint) data_020d7f40 = &VillagerTalkTopics::selectTsuFiHint;
const u32 sTalkErrandItemSpecs[6] = {
    0x00000000, 0x00000000, 0x00000003, 0x00000000, 0x00000004, 0x00000000,
};
}

void VillagerMood::updateMoodAnim(VillagerActor *s, s32 next) {
    if (next != effectMood && effectMood != 5) {
        effectFn = nullptr;
        effectMood = 5;
    } else if (!effectFn) {
        switch (s->animCtrl.getAnimId(0) - 0xe5) {
        case 0:
        case 1:
            if (next == 1) {
                if (startMoodAnim(s, 1)) {
                    effectMood = next;
                }
            }
            break;
        case 2:
        case 3:
            if (next == 2 || next == 4) {
                if (startMoodAnim(s, 2)) {
                    effectMood = next;
                }
            }
            break;
        case 4:
        case 5:
            if ((u8)(next + 0xfd) <= 1) {
                if (startMoodAnim(s, 3)) {
                    effectMood = next;
                }
            }
            break;
        }
    }
    if (effectFn) {
        (this->*effectFn)(s);
    }
}

void VillagerMood::update(VillagerActor *s) {
    void *a = Villager_GetState(s->villagerData);
    u32 b = VillagerState_GetMood(a);
    if (isActive()) {
        if ((*(CommManager * *)&gCommManager)->isSlotActive((*(CommManager * *)&gCommManager)->myAid) == 0) {
            if (s->talkCtrl.isBusy()) {
                b = 0;
                setMoodAnimation(s, b);
            } else {
                if (applyRequested != 0) {
                    u32 t = pendingMood;
                    if (t < 5) {
                        if (b != 0 && b == t) {
                            VillagerState_AddMoodTimer(a, pendingTime);
                        } else {
                            VillagerState_SetMoodTimer(a, pendingTime);
                        }
                        VillagerState_SetMood(a, pendingMood);
                        b = pendingMood;
                    }
                    applyRequested = 0;
                    clearPending();
                }
                VillagerState_TickMoodTimer(a);
                if (b != 0) {
                    if (VillagerState_GetMoodTimer(a) == 0) {
                        b = 0;
                        VillagerState_SetMood(a, b);
                    }
                }
                setMoodAnimation(s, b);
            }
            s32 v = s->animCtrl.getAnimId(0);
            if (v != s->animCtrl.resolveAnimId(0, &s->moveAnimSet)) {
                if (isMoodAnimStart(s)) {
                    s->actionCtrl.requestAction(0, s->actionCtrl.priority, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
                }
            }
            updateMoodAnim(s, b);
            currentMood = b;
            updateSoundPos(s);
        }
    }
}

extern "C" void NpcActor_JointCalcLayer3Cb(NNSG3dRS *p) {
    void *q = (void *)p->pRenderObj->ptrUser;
    if (q != NULL) {
        // (the user pointer is the NpcActor: NpcAnimCtrl::initForActor; the decomp used the DS offset 0xec of its
        // model, wrong on 64-bit hosts)
        static_cast<NpcActor *>(q)->model.onJointCalcPreLayer3(p);
    }
    p->cbVecFunc[6] = (void *)NpcActor_OnJointCalc; // NODEDESC
    p->cbVecTiming[6] = 2;
}

// ---- labels inside this unit's variables (generated: tools/host_labels.py) ----
#include "host_labels.h"

HOST_LABEL_DEFINE(data_021be810, (char *)&sHouseVisitTsuTopicTable[2]);

#include "uptr.h"
#include "host_labels.h"
#include "talk/MsgString33.h"
#include "talk/MsgString9B.h"
#include "types.h"
#include "n3ds_options.h"
#include "item/PocketMatches.h"
#include "item/ItemId.h"
#include "npc/VillagerId.h"
#include "sys/ClockDate.h"
#include "player/PlayerId.h"
#include "player/PlayerData.h"
#include "item/ItemPickSpec.h"
#include "item/Letter.h"
#include "item/LetterView.h"
#include "net/CommManager.h"
#include "player/PlayerInventory.h"
#include "save/TownId.h"
#include "talk/MsgString.h"
#include "npc/ErrandRecord.h"
#include "npc/SickVillagerRecord.h"
#include "npc/VillagerPlan.h"
#include "player/PlayerErrands.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02098e90.h"
#include "proto/autoload_2/unk_02128030.h"
#include "proto/autoload_2/unk_02133d04.h"
#include "proto/main/unk_020030d8.h"
#include "proto/main/unk_0203c23c.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_02052b50.h"
#include "proto/main/unk_0206269c.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02065330.h"
#include "proto/main/unk_02097d1c.h"
#include "proto/main/unk_020b2c50.h"
#include "save/VillagerData.h"

// Entry of the const table at 0x020d0604 (8 bytes): name, random range
struct TopicWordList {
    char *fileName;
    u8 wordCount;
};
// (ErrandRecord, PlanErrand: npc/ErrandRecord.h; SickVillagerRecord: npc/SickVillagerRecord.h; the player's errands:
// player/PlayerErrands.h; VillagerPlan, TrendScores: npc/VillagerPlan.h)

struct Unk_0209abac_Bits { u8 lo : 5; u8 hi : 3; };

// ======== data ========
extern "C" {
}

typedef BOOL (PlayerErrandSlot::*Unk_0209a4f4_Fn)();

// Entry of the slot-kind table (0x14 bytes): setup handler, second handler (always null), limit byte
struct ErrandKindSetup {
    Unk_0209a4f4_Fn setup;
    Unk_0209a4f4_Fn unk_08;
    u8 stepLimit;
};


// defined here, before Trend_GetToolItem, for the data order
// the slot kinds' setup handlers (PlayerErrandSlot member functions, called with the slot)
extern "C" {
BOOL ErrandSetup_WateringCan(void *p);
BOOL ErrandSetup_Carpet(void *p);
BOOL ErrandSetup_FurnitureParcel(void *p);
BOOL ErrandSetup_LetterBundle(void *p);
BOOL ErrandSetup_RandomShirt(void *p);
}
void *data_020e2190[2] = {(void *)ErrandSetup_RandomShirt, 0};
const u8 sFossilGroups[10] = {0x16, 0x17, 0x0a, 0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x15, 0x14};
void *data_020e21a0[2] = {(void *)ErrandSetup_Carpet, 0};
char data_020e21c0[9] = "st_sweet";
#pragma explicit_zero_data on
u8 data_020e218c[1] = {0};
#pragma explicit_zero_data reset
const char data_020d05bc[10] = "re_normal";

// Defined at the end of the unit (definition order sets the data layout).
extern const u32 sPlanErrandStepCounts[5];
extern const u32 sErrandClassStarts[2];
extern const u32 sErrandGroupStarts[4];
extern const u8 sTrendWeights[8];
extern const u16 sParcelItems[2];
extern const u16 sParcelRecipients[5];
extern const u8 sPlanStateGroupStarts[3];
extern const TopicWordList sTopicWordLists[6];
extern const u8 sFurnitureGoalCounts[6];
extern const u8 data_020d05a0[4];
extern const u8 data_020d0598[4];
extern ErrandKindSetup sErrandKindSetups[10];

// ======== unk_0209b494.cpp ========
extern "C" {
void MI_CpuCopy8(const void *src, void *dst, u32 size);
void MI_CpuFill8(void *dst, u32 v, u32 size);
}

extern "C" {
}

extern "C" void TrendScores_Copy(void *p, void *q);
extern "C" void TrendScores_Clear(void *p);
extern "C" void TrendScores_Destruct();
extern "C" void *TrendScores_Construct(void *p);
extern "C" char *TopicWord_PickRandom(u32 *out, s32 idx);

// the list's message file; out: a random word of it
extern "C" char *TopicWord_PickRandom(u32 *out, s32 idx)
{
    *out = Random_GlobalBelow(sTopicWordLists[idx].wordCount);
    return sTopicWordLists[idx].fileName;
}

extern "C" void *TrendScores_Construct(void *p)
{
    TrendScores_Clear(p);
    return p;
}

extern "C" void TrendScores_Destruct() {}

extern "C" void TrendScores_Clear(void *p)
{
    MI_CpuFill8(p, 0, 8);
}

extern "C" void TrendScores_Copy(void *p, void *q)
{
    MI_CpuCopy8(q, p, 8);
}

void TrendScores::copy(const u8 *src) { TrendScores_Copy(values, (void *)src); }

// (symbols.txt: VillagerPlan::pickTopScore, a label of this function)
u8 TrendScores::pickTop()
{
    u8 res = 8;
    u16 mask = 0;
    s32 max = -1;
    s32 cnt = 0;
    s32 i;
    for (i = 0; i < 8; i++) {
        s32 v = ((u8 *)this)[i];
        if (v > max) {
            max = v;
            mask = 1 << i;
            cnt = 1;
        } else if (max == v) {
            mask |= 1 << i;
            cnt++;
        }
    }
    if (cnt > 0) {
        s32 r = Random_GlobalBelow(cnt);
        for (i = 0; i < 8; i++) {
            if (((mask >> i) & 1) != 0) {
                if (r == 0) {
                    res = i;
                    break;
                }
                r--;
            }
        }
    }
    return res;
}


// (symbols.txt: VillagerPlan::resetAndHalveScores, a label of this function)
void TrendScores::resetAndHalve(u32 i)
{
    set((u8)i, 0);
    for (s32 j = 0; j < 8; j++) {
        u32 v = get((u8)j);
        set((u8)j, (v << 23) >> 24);
    }
}

// ======== unk_0209ab54.cpp ========
extern "C" {
void Clock_GetDateTime(ClockDateTime *p);
void DateTime_AddMinutes(ClockDateTime *p, s32 n);
BOOL DateTime_IsInvalid(u8 *p);
s32 DateTime_DiffMinutes(ClockDateTime *a, ClockDateTime *b);
s32 DateTime_Compare(ClockDateTime *a, ClockDateTime *b, u32 mask);
s32 DateTime_DiffDays(ClockDateTime *a, ClockDateTime *b);
void func_02135558(void *obj, void *dtor, void *reg);
}


extern "C" BOOL Trend_IsValid(u32 x);
extern "C" s32 PlanState_GetGroup(u32 x);

extern "C" void *PlanErrand_Destruct(PlanErrand *self);
extern "C" void *PlanErrand_Construct(PlanErrand *self);
extern "C" s32 Errand_GetSubGroup(u32 x);
extern "C" s32 Errand_GetSlotIndex(u32 x);
extern "C" ClockDateTime *ErrandRecord_GetTime(ErrandRecord *p);
extern "C" BOOL Errand_GetGroupIndex(s32 *out, s32 x);
extern "C" s32 Errand_GetGroup(u32 x);
extern "C" BOOL Errand_IsValidGroup(u32 x);
extern "C" BOOL Errand_GetClassIndex(s32 *out, s32 x);
extern "C" s32 Errand_GetClass(u32 x);
extern "C" BOOL Errand_IsValidClass(u32 x);
extern "C" BOOL Errand_IsValidKind(u32 x);
extern "C" void Trend_GetToolItem(u16 *out, s32 idx);
extern "C" ClockDateTime *VillagerPlan_GetStateDate(VillagerPlan *p);
extern "C" ClockDateTime *VillagerPlan_GetPlanTime(VillagerPlan *p);
extern "C" s32 PlanState_GetGroupIndex(u8 *out, s32 x);

void TrendScores::add(u8 i, u32 v) {
    if (Trend_IsValid(i)) {
        s32 t = values[i];
        t += v;
        if (t >= 0xff) t = 0xff;
        values[i] = t;
    }
}


void TrendScores::set(u8 i, u32 v) {
    if (Trend_IsValid(i)) values[i] = v;
}


u32 TrendScores::get(u32 i) {
    u32 r = 0;
    if (Trend_IsValid(i)) r = values[i];
    return r;
}


VillagerPlan::VillagerPlan() {
    planTime.words[0] = 0;
    planTime.words[1] = 0;
    stateTime.words[0] = 0;
    stateTime.words[1] = 0;
    clear();
}


VillagerPlan::~VillagerPlan() {
    }


void VillagerPlan::clear() {
    MI_CpuFill8(this, 0, 0x24);
    planState = 7;
    scores.clear();
    pendingScores.clear();
    planTime.words[0] = 0;
    planTime.words[1] = 0;
    stateTime.words[0] = 0;
    stateTime.words[1] = 0;
}


extern "C" BOOL Trend_IsValid(u32 x) {
    if (x < 8) return TRUE;
    return FALSE;
}

BOOL VillagerPlan::hasTrend() {
    return Trend_IsValid(planState); }


BOOL VillagerPlan::isValidState() {
    if (planState < 0xc) return TRUE;
    return FALSE;
}


void VillagerPlan::rollTrend(const u8 *trend, s32 flag) {
    scores.copy(trend);
    planState = scores.pickTop();
    if (flag != 0) {
        scores.resetAndHalve(planState);
    }
    Clock_GetDateTime(&planTime.dt);
    Clock_GetDateTime(&stateTime.dt);
}


u32 VillagerPlan::getState() {
    return planState; }


void VillagerPlan::setState(u32 v) {
    planState = v; }


extern "C" s32 PlanState_GetGroup(u32 x) {
    s32 r = 3;
    if (x < 5) {
        r = 0;
    } else if (x < 8) {
        r = 1;
    } else if (x < 0xc) {
        r = 2;
    }
    return r;
}

s32 VillagerPlan::getStateGroup() {
    return PlanState_GetGroup(planState); }


extern "C" s32 PlanState_GetGroupIndex(u8 *out, s32 x) {
    *out = PlanState_GetGroup(x);
    if (*out < 3) return x - sPlanStateGroupStarts[*out];
    return -1;
}

BOOL VillagerPlan::hasState2Flag() {
    if (flags.state2Flag) return TRUE;
    return FALSE;
}


void VillagerPlan::startTrendChange() {
    if (getState() == 2) {
        flags.state2Flag = 1;
    } else {
        flags.state2Flag = 0;
    }
    setState(8);
    Clock_GetDateTime(&planTime.dt);
    MI_CpuCopy8(&planTime, &stateTime, 8);
}


void VillagerPlan::enterState0B() {
    if (getState() == 2 || hasState2Flag()) {
        flags.state2Flag = 1;
    } else {
        flags.state2Flag = 0;
    }
    setState(0xb);
    Clock_GetDateTime(&planTime.dt);
    MI_CpuCopy8(&planTime, &stateTime, 8);
}


BOOL VillagerPlan::isTrendOlderThan(ClockDateTime *o, u32 c) {
    if (DateTime_IsInvalid((u8 *)&planTime.dt) == 0 && hasTrend()) {
        s32 t = DateTime_DiffMinutes(&planTime.dt, o);
        if (t < 0) t = -t;
        if ((u32)(t / 0x5a0) >= c) return TRUE;
        return FALSE;
    }
    return FALSE;
}


BOOL VillagerPlan::isTrendOlderThanWeek(ClockDateTime *o) {
    return isTrendOlderThan(o, 7); }


s32 VillagerPlan::updateTopTrend() {
    if (Trend_IsValid(planState)) {
        u32 i = scores.pickTop();
        if (Trend_IsValid(i)) {
            if (scores.get(i) == 0xff) {
                if (planState == i) {
                    scores.resetAndHalve(planState);
                    return planState;
                } else {
                    startTrendChange();
                    return 8;
                }
            }
        }
    }
    return 0xc;
}


s32 VillagerPlan::finishTrendChange() {
    if (planState == 8) {
        u32 i = scores.pickTop();
        if (Trend_IsValid(i)) {
            setState(i);
            scores.resetAndHalve(i);
            flags.state2Flag = 0;
            Clock_GetDateTime(&planTime.dt);
            MI_CpuCopy8(&planTime, &stateTime, 8);
            return i;
        }
    }
    return 0xc;
}


s32 VillagerPlan::leaveState0B(VillagerPlan *o) {
    if (planState == 0xb && o->planTime.dt.day != planTime.dt.day) {
        u32 i = scores.pickTop();
        if (Trend_IsValid(i)) {
            setState(i);
            scores.resetAndHalve(i);
            flags.state2Flag = 0;
            Clock_GetDateTime(&planTime.dt);
            MI_CpuCopy8(&planTime, &stateTime, 8);
            return i;
        }
    }
    return 0xc;
}


s32 VillagerPlan::finishMovingIn(ClockDateTime *x) {
    if (planState == 0xa) {
        if (x == 0 || (DateTime_Compare(x, &planTime.dt, 0x3f) == 1 && DateTime_DiffDays(&planTime.dt, x) >= 1)) {
            u32 i = scores.pickTop();
            if (Trend_IsValid(i)) {
                setState(i);
                scores.resetAndHalve(i);
                flags.state2Flag = 0;
                Clock_GetDateTime(&planTime.dt);
                MI_CpuCopy8(&planTime, &stateTime, 8);
                return i;
            }
        }
    }
    return 0xc;
}


s32 VillagerPlan::getTrendName(MsgString *out, s32 x) {
    if (!Trend_IsValid(x)) x = 7;
    u8 buf = x;
    return String_Load(out, &buf, "st_boom");
}


extern "C" ClockDateTime *VillagerPlan_GetPlanTime(VillagerPlan *p) { return &p->planTime.dt; }

extern "C" ClockDateTime *VillagerPlan_GetStateDate(VillagerPlan *p) { return &p->stateTime.dt; }

void VillagerPlan::applyPendingScores(s16 *p) {
    const u8 *w = sTrendWeights;
    s32 i;
    for (i = 0; i < 8; p++, w++, i++) {
        s32 v = pendingScores.get((u8)i);
        s32 wt = *w;
        if (wt != 0 && v != 0) {
            s32 m = *p * wt;
            scores.add((u8)i, (u8)((v * m) >> 12));
        }
    }
    pendingScores.clear();
}


BOOL VillagerPlan::addRandomScores(s16 *p) {
    BOOL r = FALSE;
    s32 i;
    for (i = 0; i < 8; p++, i++) {
        if (scores.get((u8)i) < 0xff) {
            s32 t = Random_GlobalBelow(10);
            if (t > 0) {
                scores.add((u8)i, (u8)((*p * t) >> 12));
            }
            r = TRUE;
        }
    }
    return r;
}


void VillagerPlan::addPendingScore(u8 idx, s32 delta) {
    if (Trend_IsValid(idx)) {
        s32 t = delta + pendingScores.get(idx);
        if (t < 0) {
            t = 0;
        } else if (t > 10) {
            t = 10;
        }
        pendingScores.set(idx, (u8)t);
    }
}


void VillagerPlan::addTrendOf(VillagerPlan *other, s16 *p) {
    u8 i = other->scores.pickTop();
    if (Trend_IsValid(i)) {
        scores.add(i, (u8)((p[i] * 10) >> 12));
    }
}


extern "C" void Trend_GetToolItem(u16 *out, s32 idx) {
    static ItemId tbl[8] = {
        ItemId(0x1376), ItemId(0x1374), ItemId(0x1369), ItemId(0xfff1),
        ItemId(0xfff1), ItemId(0x1378), ItemId(0xfff1), ItemId(0xfff1)};
    if (Trend_IsValid(idx)) {
        *out = tbl[idx].id;
    } else {
        *out = 0xfff1;
    }
}

void ErrandRecord::init() {
    dateTime.words[0] = 0;
    dateTime.words[1] = 0;
    item = 0xfff1;
    kind = 0x16;
    item = 0xfff1;
}


void ErrandRecord::func_0209ada0() {
    }


void ErrandRecord::clear() {
    kind = 0x16;
    item = 0xfff1;
    stepBits = stepBits & ~0x1f;
    stepBits = stepBits & ~0xe0;
}


extern "C" BOOL Errand_IsValidKind(u32 x) {
    if (x < 0x16) return TRUE;
    return FALSE;
}

BOOL ErrandRecord::isActive() {
    return Errand_IsValidKind(kind); }


void ErrandRecord::start(u8 a, u16 *p, u8 b) {
    kind = a;
    item = *p;
    stepBits = (stepBits & ~0x1f) | (b & 0x1f);
}


extern "C" BOOL Errand_IsValidClass(u32 x) {
    if (x < 2) return TRUE;
    return FALSE;
}

extern "C" s32 Errand_GetClass(u32 x) {
    s32 r = 2;
    if (x < 5) {
        r = 0;
    } else if (x < 0x16) {
        r = 1;
    }
    return r;
}

s32 ErrandRecord::getClass() {
    return Errand_GetClass(kind); }


extern "C" BOOL Errand_GetClassIndex(s32 *out, s32 x) {
    s32 k = Errand_GetClass(x);
    if (Errand_IsValidClass(k)) {
        *out = x - sErrandClassStarts[k];
        return TRUE;
    }
    return FALSE;
}

BOOL ErrandRecord::getClassIndex(s32 *out) {
    return Errand_GetClassIndex(out, kind); }


extern "C" BOOL Errand_IsValidGroup(u32 x) {
    if (x < 4) return TRUE;
    return FALSE;
}

extern "C" s32 Errand_GetGroup(u32 x) {
    s32 r = 4;
    if (x < 0xa) {
        r = 0;
    } else if (x < 0x14) {
        r = 1;
    } else if (x < 0x15) {
        r = 2;
    } else if (x < 0x16) {
        r = 3;
    }
    return r;
}

s32 ErrandRecord::getGroup() {
    return Errand_GetGroup(kind); }


extern "C" BOOL Errand_GetGroupIndex(s32 *out, s32 x) {
    s32 k = Errand_GetGroup(x);
    BOOL r = FALSE;
    if (Errand_IsValidGroup(k)) {
        *out = x - sErrandGroupStarts[k];
        r = TRUE;
    }
    return r;
}

BOOL ErrandRecord::getGroupIndex(s32 *out) {
    return Errand_GetGroupIndex(out, kind); }


u32 ErrandRecord::getKind() {
    return kind; }


void ErrandRecord::setTimeFromNow(s32 x) {
    Clock_GetDateTime(&dateTime.dt);
    DateTime_AddMinutes(&dateTime.dt, x);
}


extern "C" ClockDateTime *ErrandRecord_GetTime(ErrandRecord *p) { return &p->dateTime.dt; }

extern "C" s32 Errand_GetSlotIndex(u32 x) {
    s32 r = 2;
    if (Errand_GetGroup(x) == 1) {
        if (x < 0x13) {
            r = 0;
        } else if (x < 0x14) {
            r = 1;
        }
    }
    return r;
}

s32 ErrandRecord::getSlotIndex() {
    return Errand_GetSlotIndex(kind); }


extern "C" s32 Errand_GetSubGroup(u32 x) {
    s32 r = 4;
    if (Errand_GetGroup(x) == 1) {
        if (x < 0x13) {
            if (x < 0xb) {
                r = 0;
            } else {
                r = 1;
            }
        } else if (x < 0x14) {
            r = 2;
        } else if (x < 0x15) {
            r = 3;
        }
    }
    return r;
}

s32 ErrandRecord::getSubGroup() {
    return Errand_GetSubGroup(kind); }


u32 ErrandRecord::getStep() {
    return ((Unk_0209abac_Bits *)&stepBits)->lo; }


void ErrandRecord::setStep(u8 v) {
    stepBits = (stepBits & ~0x1f) | (v & 0x1f); }


u32 ErrandRecord::getExtra() {
    return ((Unk_0209abac_Bits *)&stepBits)->hi; }


void ErrandRecord::setExtra(u8 v) {
    stepBits = (stepBits & ~0xe0) | ((v & 7) << 5); }


u16 *ErrandRecord::getItem() {
    return &item; }


void ErrandRecord::setItem(u16 *p) {
    item = *p; }


extern "C" void *PlanErrand_Construct(PlanErrand *self) {
    self->errand.init();
    new (&self->player) PlayerId((void *)NULL); // unused by PlayerId::PlayerId(void *)
    self->shownItem = 0xfff1;
    return self;
}

extern "C" void *PlanErrand_Destruct(PlanErrand *self) {
    new (&self->player) PlayerId();
    self->errand.func_0209ada0();
    return self;
}


// ======== unk_0209a208.cpp ========
extern "C" {
void ItemPickSpec_Destruct(void *p);
BOOL Item_IsFurniture(u16 *p);
s32 Item_GetFurnitureIndex(u16 *p);
void __cxa_vec_cleanup(void *p, u32 n, u32 sz, void (*d)(void *));
void __cxa_vec_ctor(void *p, u32 n, u32 sz, void (*c)(void *), void (*d)(void *));
extern void *gCommManager;
}


extern "C" {
}


extern "C" void PlanErrand_SetStep(PlanErrand *p, u8 v);


extern "C" u8 ParcelErrand_CountPendingRecipients(void *unused, s32 v);
extern "C" u8 ParcelErrand_CountPending(void *unused, s32 v);
extern "C" void ParcelErrand_Clear(ParcelErrand *p);
extern "C" void PlayerErrandSlot_ComposeLetter(PlayerErrandSlot *self, void *arg);
extern "C" u8 *PlayerErrandSlot_GetPresentReaction(u8 *p);
extern "C" void PlayerErrandSlot_SetPresentReaction(u8 *p, u32 v);
extern "C" s32 PlayerErrandSlot_IsStepDone(void *self);
extern "C" s32 PlayerErrandSlot_IsPastStepLimit(void *self, s32 kind);
extern "C" BOOL Errand_CanOfferInPlanState(s32 a, u32 b);
extern "C" VillagerIdCtorDtor *PlayerErrandSlot_GetVillager(PlayerErrandSlot *p, s32 i);
extern "C" ErrandRecord *PlayerErrandSlot_GetRecord(PlayerErrandSlot *p);
extern "C" BOOL PlayerErrandSlot_Start(PlayerErrandSlot *self, s32 kind, VillagerId *r6, VillagerId *r3);
extern "C" void PlayerErrandSlot_Clear(PlayerErrandSlot *self);
extern "C" PlayerErrandSlot *PlayerErrandSlot_Destruct(PlayerErrandSlot *self);
extern "C" PlayerErrandSlot *PlayerErrandSlot_Construct(PlayerErrandSlot *self);
extern "C" PlanErrand *VillagerPlanBlock_GetErrand(void *p);
extern "C" VillagerPlan *VillagerPlanBlock_GetPlan(void *p);
extern "C" s32 VillagerPlanBlock_RollTrend(void *p, const u8 *trend, s32 flag);
extern "C" void VillagerPlanBlock_Clear(VillagerPlanBlock *p);
extern "C" VillagerPlanBlock *VillagerPlanBlock_Destruct(VillagerPlanBlock *p);
extern "C" VillagerPlanBlock *VillagerPlanBlock_Construct(VillagerPlanBlock *p);
extern "C" BOOL PlanErrand_IsFurnitureGoalMet(PlanErrand *self);
extern "C" u8 PlanErrand_AddCount(PlanErrand *p, s32 n);
extern "C" u8 PlanErrand_GetCount(PlanErrand *p);
extern "C" s32 Furniture_ScoreAttribute(u16 *self, u32 kind, s32 val);
extern "C" void FossilGroup_GetItem(u16 *out, s32 a, s32 b);
extern "C" u8 FossilGroup_PickMissing(u16 *p, s32 n);
extern "C" s32 FossilGroup_ToIndex(u32 v);
extern "C" void PlanErrand_ClearFlags(PlanErrand *p);
extern "C" BOOL PlanErrand_TestFlag(PlanErrand *p, u32 i);
extern "C" void PlanErrand_SetFlag(PlanErrand *p, s32 i);
extern "C" void PlanErrand_SetFossilGroup(PlanErrand *p, u32 v);
extern "C" u8 PlanErrand_GetFossilGroup(PlanErrand *p);
extern "C" u16 *PlanErrand_GetShownItem(PlanErrand *p);
extern "C" ClockDateTime *PlanErrand_GetTime(PlanErrand *p);
extern "C" s32 PlanErrand_GetStepCount(s32 x);
extern "C" PlayerId *PlanErrand_GetPlayer(PlanErrand *p);
extern "C" u8 PlanErrand_GetStep(PlanErrand *p);
extern "C" ErrandRecord *PlanErrand_GetRecord(PlanErrand *p);
extern "C" void PlanErrand_AdvanceStep(PlanErrand *self);
extern "C" void PlanErrand_MarkReady(PlanErrand *p);
extern "C" void PlanErrand_Assign(PlanErrand *self, s32 a1, u16 *p, void *obj, u8 flag);
extern "C" void PlanErrand_ResetProgress(PlanErrand *self);
extern "C" void PlanErrand_Begin(PlanErrand *self, s32 a, u16 *b);
extern "C" void PlanErrand_SetKind(PlanErrand *p, s32 a, u16 *b);
extern "C" void PlanErrand_Clear(PlanErrand *self);

extern "C" void PlanErrand_Clear(PlanErrand *self) {
    self->errand.clear();
    self->player.clear();
    self->step = 7;
    self->shownItem = 0xfff1;
    self->count = 0;
    self->fossilGroup = 0x18;
    self->flags = 0;
}

extern "C" void PlanErrand_SetKind(PlanErrand *p, s32 a, u16 *b) {
    p->errand.start(a, b, 0);
}

extern "C" void PlanErrand_Begin(PlanErrand *self, s32 a, u16 *b) {
    PlanErrand_Clear(self);
    PlanErrand_SetKind(self, a, b);
    self->step = 0;
}

extern "C" void PlanErrand_ResetProgress(PlanErrand *self) {
    u16 v;
    PlanErrand_GetRecord(self)->setStep(0);
    v = 0xfff1;
    PlanErrand_GetRecord(self)->setItem(&v);
    PlanErrand_GetPlayer(self)->clear();
    *PlanErrand_GetShownItem(self) = 0xfff1;
}

extern "C" void PlanErrand_Assign(PlanErrand *self, s32 a1, u16 *p, void *obj, u8 flag) {
    if (flag) {
        PlanErrand_Begin(self, a1, p);
    } else {
        PlanErrand_GetRecord(self)->setStep(0);
        PlanErrand_GetRecord(self)->setItem(p);
        if (PlanErrand_GetRecord(self)->getKind() == 2) {
            if (PlanErrand_GetStep(self) == 2) {
                BOOL r = FALSE;
                if (*p >= 0x450c && *p <= 0x45db) {
                    r = TRUE;
                }
                if (r) {
                    PlanErrand_SetFossilGroup(self, (u8)Item_GetFossilGroup(p));
                }
            }
        }
    }
    if (obj) {
        PlanErrand_GetPlayer(self)->setRaw(obj);
        Clock_GetDateTime(ErrandRecord_GetTime(PlanErrand_GetRecord(self)));
        PlanErrand_GetRecord(self)->setExtra(0);
    } else {
        PlanErrand_GetPlayer(self)->clear();
    }
}

extern "C" void PlanErrand_MarkReady(PlanErrand *p) {
    ErrandRecord *o = PlanErrand_GetRecord(p);
    if (o->isActive()) {
        if (o->getClass() == 0) {
            if (o->getStep() == 1) {
                o->setStep(2);
            }
        }
    }
}

extern "C" void PlanErrand_AdvanceStep(PlanErrand *self) {
    ErrandRecord *o = PlanErrand_GetRecord(self);
    s32 t;
    s32 a;
    s32 ok;
    if (o->isActive()) {
        if (o->getClass() == 0) {
            t = PlanErrand_GetStepCount(o->getKind());
            a = PlanErrand_GetStep(self);
            if (a < t - 1) {
                if (o->getKind() == 4) {
                    ok = PlanErrand_IsFurnitureGoalMet(self);
                } else {
                    ok = 1;
                }
                PlanErrand_ResetProgress(self);
                if (ok ? TRUE : FALSE) {
                    PlanErrand_SetStep(self, a + 1);
                }
            }
        }
    }
}

extern "C" ErrandRecord *PlanErrand_GetRecord(PlanErrand *p) {
    return &p->errand;
}

extern "C" u8 PlanErrand_GetStep(PlanErrand *p) {
    return p->step;
}

extern "C" void PlanErrand_SetStep(PlanErrand *p, u8 v) {
    p->step = v;
}

extern "C" PlayerId *PlanErrand_GetPlayer(PlanErrand *p) {
    return &p->player;
}

extern "C" s32 PlanErrand_GetStepCount(s32 x) {
    u32 idx;
    if (Errand_GetClass(x) == 0) {
        idx = 0;
        if (Errand_GetClassIndex((s32 *)&idx, x)) {
            return sPlanErrandStepCounts[idx];
        }
    }
    return 0;
}

extern "C" ClockDateTime *PlanErrand_GetTime(PlanErrand *p) {
    return ErrandRecord_GetTime(&p->errand);
}

extern "C" u16 *PlanErrand_GetShownItem(PlanErrand *p) {
    return &p->shownItem;
}

extern "C" u8 PlanErrand_GetFossilGroup(PlanErrand *p) {
    return p->fossilGroup;
}

extern "C" void PlanErrand_SetFossilGroup(PlanErrand *p, u32 v) {
    PlanErrand_ClearFlags(p);
    p->fossilGroup = v;
}

extern "C" void PlanErrand_SetFlag(PlanErrand *p, s32 i) {
    p->flags |= (1 << i);
}

extern "C" BOOL PlanErrand_TestFlag(PlanErrand *p, u32 i) {
    if (i < 3) {
        if ((p->flags >> i) & 1) {
            return TRUE;
        }
    }
    return FALSE;
}

extern "C" void PlanErrand_ClearFlags(PlanErrand *p) {
    p->flags = 0;
}

extern "C" s32 FossilGroup_ToIndex(u32 v) {
    s32 i;
    for (i = 0; i < 10; i++) {
        if (v == sFossilGroups[i]) {
            return i;
        }
    }
    return -1;
}

extern "C" u8 FossilGroup_PickMissing(u16 *p, s32 n) {
    u8 f[10];
    s32 cnt = 10;
    s32 i;
    s32 j;
    s32 k;
    MI_CpuFill8(f, 0, cnt);
    for (i = 0; i < n; i++, p++) {
        BOOL in = FALSE;
        if (*p >= 0x450c && *p <= 0x45db) {
            in = TRUE;
        }
        if (in) {
            for (j = 0; j < 10; j++) {
                if (sFossilGroups[j] == Item_GetFossilGroup(p)) {
                    if (f[j] == 0) {
                        f[j] = 1;
                        cnt--;
                        break;
                    }
                }
            }
        }
    }
    k = Random_GlobalBelow(cnt);
    j = 0;
    while (k >= 0) {
        if (f[j] == 0) {
            if (k == 0) break;
            k--;
            j++;
        } else {
            j++;
        }
    }
    return sFossilGroups[j];
}

extern "C" void FossilGroup_GetItem(u16 *out, s32 a, s32 b) {
    u16 tmp;
    u32 i;
    *out = 0xfff1;
    tmp = 0xfff1;
    for (i = 0; i < 0x34; i++) {
        tmp = i < 0x34 ? 0x450c + i * 4 : 0x450c;
        if (a == Item_GetFossilGroup(&tmp)) {
            u32 k = i + b;
            *out = k < 0x34 ? 0x450c + k * 4 : 0x450c;
            break;
        }
    }
}

extern "C" s32 Furniture_ScoreAttribute(u16 *self, u32 kind, s32 val) {
    s32 r = 0;
    if (Item_IsFurniture(self)) {
        {
            switch (kind) {
            case 0:
                if (val == Ftr_GetFlagPairB(self)) r = 2;
                break;
            case 1: {
                s32 idx = Item_GetFurnitureIndex(self);
                if (val == FtrInfo_GetColor1(idx)) r = 1;
                if (val == FtrInfo_GetColor2(idx)) r++;
                break;
            }
            case 2:
                if (val == Ftr_GetFlagPairA(self)) r = 2;
                break;
            case 3:
                if (val == Ftr_GetCollectionGroup(self)) r = 2;
                break;
            case 4:
                if (val == Ftr_GetSeries(self)) r = 2;
                break;
            }
        }
    }
    return r;
}

extern "C" u8 PlanErrand_GetCount(PlanErrand *p) {
    return p->count;
}

extern "C" u8 PlanErrand_AddCount(PlanErrand *p, s32 n) {
    s32 t = p->count + n;
    if (t > 14) {
        t = 14;
    }
    p->count = t;
    return p->count;
}

extern "C" BOOL PlanErrand_IsFurnitureGoalMet(PlanErrand *self) {
    ErrandRecord *o = PlanErrand_GetRecord(self);
    s32 t;
    if (o->isActive()) {
        if (o->getKind() == 4) {
            t = PlanErrand_GetStep(self);
            if (t < 6) {
                if (PlanErrand_GetCount(self) >= sFurnitureGoalCounts[t]) {
                    return TRUE;
                }
                return FALSE;
            }
        }
    }
    return FALSE;
}

extern "C" VillagerPlanBlock *VillagerPlanBlock_Construct(VillagerPlanBlock *p) {
    new (&p->plan) VillagerPlan();
    PlanErrand_Construct(&p->errand);
    return p;
}

extern "C" VillagerPlanBlock *VillagerPlanBlock_Destruct(VillagerPlanBlock *p) {
    PlanErrand_Destruct(&p->errand);
    p->plan.~VillagerPlan();
    return p;
}

extern "C" void VillagerPlanBlock_Clear(VillagerPlanBlock *p) {
    p->plan.clear();
    PlanErrand_Clear(&p->errand);
}

// (the villager data keeps the block untyped: Villager_GetPlan)
extern "C" s32 VillagerPlanBlock_RollTrend(void *p, const u8 *trend, s32 flag) {
    VillagerPlan *plan = &((VillagerPlanBlock *)p)->plan;
    plan->rollTrend(trend, flag);
    return plan->getState();
}

extern "C" VillagerPlan *VillagerPlanBlock_GetPlan(void *p) {
    return &((VillagerPlanBlock *)p)->plan;
}

extern "C" PlanErrand *VillagerPlanBlock_GetErrand(void *p) {
    return &((VillagerPlanBlock *)p)->errand;
}

extern "C" PlayerErrandSlot *PlayerErrandSlot_Construct(PlayerErrandSlot *self) {
    ((ErrandRecord *)(self))->init();
    for (s32 i = 0; i < 2; i++) VillagerId_Construct(&self->villagers[i]);
    return self;
}

extern "C" PlayerErrandSlot *PlayerErrandSlot_Destruct(PlayerErrandSlot *self) {
    for (s32 i = 2; i-- > 0;) VillagerId_Destruct(&self->villagers[i]);
    ((ErrandRecord *)(self))->func_0209ada0();
    return self;
}

extern "C" void PlayerErrandSlot_Clear(PlayerErrandSlot *self) {
    s32 i;
    ((ErrandRecord *)(self))->clear();
    for (i = 0; i < 2; i++) {
        VillagerId_Clear(&self->villagers[i]);
    }
    self->presentReaction = 0;
}

extern "C" BOOL PlayerErrandSlot_Start(PlayerErrandSlot *self, s32 kind, VillagerId *r6, VillagerId *r3) {
    BOOL r = FALSE;
    s32 idx = 0;
    u16 v;
    if (Errand_GetGroup(kind) == 1) {
        if (Errand_GetGroupIndex(&idx, kind)) {
            v = 0xfff1;
            ((ErrandRecord *)(self))->start(kind, &v, r);
            {
                ErrandKindSetup *e = &sErrandKindSetups[idx];
                if (e->setup) {
                    (self->*(e->setup))();
                }
            }
            if (r6) {
                VillagerId_Copy(&self->villagers[0], r6);
            }
            if (r3) {
                VillagerId_Copy(&self->villagers[1], r3);
            }
            r = TRUE;
        }
    }
    return r;
}

extern "C" ErrandRecord *PlayerErrandSlot_GetRecord(PlayerErrandSlot *p) {
    return p;
}

extern "C" VillagerIdCtorDtor *PlayerErrandSlot_GetVillager(PlayerErrandSlot *p, s32 i) {
    return &p->villagers[i];
}

extern "C" BOOL Errand_CanOfferInPlanState(s32 a, u32 b) { // b: the villager's plan state
    s32 t;
    BOOL r;
    if (((CommManager *)(gCommManager))->isOnline()) {
        return FALSE;
    }
    t = PlanState_GetGroup(b);
    r = FALSE;
    switch (a) {
    case 10:
        if (t == 1) r = TRUE;
        break;
    case 19:
        if (t == 1) r = TRUE;
        break;
    }
    return r;
}

extern "C" s32 PlayerErrandSlot_IsPastStepLimit(void *self, s32 kind) {
    BOOL r = FALSE;
    s32 idx;
    if (((ErrandRecord *)(self))->isActive()) {
        if (Errand_GetGroup(kind) == 1) {
            if (kind == ((ErrandRecord *)(self))->getKind()) {
                if (((ErrandRecord *)(self))->getGroupIndex(&idx)) {
                    if (((ErrandRecord *)(self))->getStep() > sErrandKindSetups[idx].stepLimit) {
                        r = TRUE;
                    }
                }
            }
        }
    }
    return r;
}

extern "C" s32 PlayerErrandSlot_IsStepDone(void *self) {
    return PlayerErrandSlot_IsPastStepLimit(self, ((ErrandRecord *)(self))->getKind());
}

extern "C" void PlayerErrandSlot_SetPresentReaction(u8 *p, u32 v) {
    p[0x24] = v;
}

extern "C" u8 *PlayerErrandSlot_GetPresentReaction(u8 *p) {
    return p + 0x24;
}

extern "C" BOOL ErrandSetup_RandomShirt(void *p) {
    ItemPickSpec x;
    u16 out;
    x.set(2, 0);
    ItemPick_One(&out, &x, 0, 0, 1, 1, 0);
    ((ErrandRecord *)(p))->setItem(&out);
    return TRUE;
}

extern "C" BOOL ErrandSetup_LetterBundle(void *p) {
    u16 v = 0x1565;
    ((ErrandRecord *)(p))->setItem(&v);
    return TRUE;
}

extern "C" void PlayerErrandSlot_ComposeLetter(PlayerErrandSlot *self, void *arg) {
    u8 b[5];
    u32 cnt;
    char buf[0x1e];
    s32 i;
    if (self->villagers[0].isValid() && self->villagers[1].isValid()) {
        MsgString9B name;
        MsgString33 word;
        cnt = 0;
        self->villagers[1].getName(&name);
        MailText_SetSlot(0, &name);
        name.clear();
        self->villagers[0].getName(&name);
        MailText_SetSlot(1, &name);
        for (i = 0; i < 6; i++) {
            const char *r = TopicWord_PickRandom(&cnt, i);
            word.clear();
            b[0] = cnt;
            String_LoadResolveAltText(&word, b, r);
            MailText_SetSlot(i + 2, &word);
        }
        self->villagers[0].makeFileName(buf, 0x1e, data_020d05bc);
        b[1] = Random_GlobalBelow(10);
        b[2] = Random_GlobalBelow(10);
        b[3] = Random_GlobalBelow(10);
        b[4] = Random_GlobalBelow(10);
        Letter_ComposeVillagerToVillagerZ((LetterView *)arg, &b[1], &b[2], &b[3], &b[4], buf, data_020e218c, &self->villagers[0],
                                          &self->villagers[1], 1);
    }
}

extern "C" BOOL ErrandSetup_FurnitureParcel(void *p) {
    u16 v = 0x1563;
    ((ErrandRecord *)(p))->setItem(&v);
    return TRUE;
}

extern "C" BOOL ErrandSetup_Carpet(void *p) {
    u16 v = 0x1561;
    ((ErrandRecord *)(p))->setItem(&v);
    return TRUE;
}

extern "C" BOOL ErrandSetup_WateringCan(void *p) {
    u16 v = 0x1564;
    ((ErrandRecord *)(p))->setItem(&v);
    return TRUE;
}

extern "C" void ParcelErrand_Clear(ParcelErrand *p) {
    p->clear();
    p->recipient = 5;
    p->pendingRecipients = 0;
}

extern "C" u8 ParcelErrand_CountPending(void *unused, s32 v) {
    u8 n = 0;
    s32 i = 0;
    for (; i < 5; i++) {
        if ((v >> i) & 1) {
            n = n + 1;
        }
    }
    return n;
}

extern "C" u8 ParcelErrand_CountPendingRecipients(void *unused, s32 v) {
    u8 n = ParcelErrand_CountPending(unused, v);
    if ((v >> 2) & 1) {
        if ((v >> 3) & 1) {
            n = n - 1;
        }
    }
    return n;
}


// ======== unk_020998b8.cpp ========
extern "C" {
#define gSaveVillagers HOST_LABEL(u8 (*)[], gSaveVillagers)
#define data_021ed104 HOST_LABEL(u8 (*)[], data_021ed104)
void *SaveVillagers_PickRandomTalkPartner(void *g, VillagerIdCtorDtor **a, s32 n);
BOOL Npc_GetName(MsgString *self, u16 *p);
s32 NookShop_IsOpenHour();
s32 NookShop_IsClosedToday(void *p);
}


extern "C" BOOL Arbeit_IsLetterRecipient(PlayerErrands *m, VillagerId *r);
extern "C" void Arbeit_Finish(PlayerErrands *m);
extern "C" void Arbeit_OnBbsPosted(PlayerErrands *m);
extern "C" void Arbeit_StartBbsTask(PlayerErrands *m);
extern "C" void Arbeit_StartWateringCanDelivery(PlayerErrands *m);
extern "C" void Arbeit_StartCarpetDelivery(PlayerErrands *m);
extern "C" BOOL Arbeit_OnLetterSent(PlayerErrands *m, void *p);
extern "C" void Arbeit_NotifyLetterWritten();
extern "C" void Arbeit_OnLetterWritten(void *p);
extern "C" void Arbeit_StartLetterTask(PlayerErrands *m);
extern "C" void Arbeit_StartFurnitureDelivery(PlayerErrands *m);
extern "C" void Arbeit_StartGreetings(PlayerErrands *m);
extern "C" void Arbeit_StartPlanting(PlayerErrands *m);
extern "C" BOOL Arbeit_IsOnDuty(PlayerErrands *m);
extern "C" void Arbeit_ClearOnDuty(PlayerErrands *m);
extern "C" void Arbeit_SetOnDuty(PlayerErrands *m);
extern "C" void Arbeit_Start(PlayerErrands *m);
extern "C" BOOL PlayerErrands_IsJobActive(PlayerErrands *m);
extern "C" void PlayerErrands_ClearJobVillagers(PlayerErrands *m);
extern "C" BOOL PlayerErrands_GetDeliveryRecipientName(PlayerErrands *m, MsgString *a, u16 *p);
extern "C" PlayerErrandSlot *PlayerErrandSlots_FindByVillager(PlayerErrandSlot *arr, VillagerId *p, s32 idx);
extern "C" PlayerErrandSlot *PlayerErrands_GetSlot(PlayerErrands *m, s32 i);
extern "C" void PlayerErrands_Clear(PlayerErrands *m);
extern "C" void HouseVisitInvite_Set(HouseVisitInvite *x, VillagerId *r, void *src);
extern "C" BOOL HouseVisitInvite_IsFrom(HouseVisitInvite *x, VillagerId *p);
extern "C" void HouseVisitInvite_Clear(HouseVisitInvite *x);
extern "C" BOOL ParcelErrand_IsFor(ParcelErrand *z, u16 *p);
extern "C" BOOL ParcelErrand_NextRecipient(ParcelErrand *z);
extern "C" BOOL ParcelErrand_GetRecipientName(ParcelErrand *z, MsgString *v);
extern "C" ErrandRecord *ParcelErrand_GetRecord(ParcelErrand *z);
extern "C" void ParcelErrand_Start(ParcelErrand *z);
extern "C" void ParcelErrand_RemoveRecipient(ParcelErrand *z, s32 i);
extern "C" s32 ParcelErrand_PickRecipient(ParcelErrand *z, s32 m);

extern "C" s32 ParcelErrand_PickRecipient(ParcelErrand *z, s32 m) {
    if (NookShop_IsOpenHour() == 0) {
        m = (u8)(m & ~3);
    } else if (NookShop_IsClosedToday(data_021ed104)) {
        m = (u8)(m & ~1);
    }
    s32 n = ParcelErrand_CountPending(z, m);
    if (n > 0) {
        s32 r = Random_GlobalBelow(n);
        for (s32 i = 0; i < 5; i++) {
            if ((m >> i) & 1) {
                if (r == 0) {
                    return i;
                }
                r--;
            }
        }
    }
    return -1;
}

extern "C" void ParcelErrand_RemoveRecipient(ParcelErrand *z, s32 i) {
    if ((u32)(i - 2) <= 1) {
        z->pendingRecipients &= ~4;
        z->pendingRecipients &= ~8;
    } else {
        z->pendingRecipients &= ~(1 << i);
    }
}

extern "C" void ParcelErrand_Start(ParcelErrand *z) {
    u32 r = Random_GlobalBelow(10) & 1;
    ParcelErrand_Clear(z);
    u16 v = sParcelItems[r];
    (z)->start(0x14, &v, 0);
    for (s32 i = 0; i < 5; i++) {
        z->pendingRecipients |= 1 << i;
    }
    r = ParcelErrand_PickRecipient(z, z->pendingRecipients);
    if (r < 5) {
        ParcelErrand_RemoveRecipient(z, r);
        z->recipient = r;
    }
}

extern "C" ErrandRecord *ParcelErrand_GetRecord(ParcelErrand *z) {
    return z;
}

extern "C" BOOL ParcelErrand_GetRecipientName(ParcelErrand *z, MsgString *v) {
    if (z->recipient < 5) {
        u16 t = sParcelRecipients[z->recipient];
        return Npc_GetName(v, &t);
    }
    return FALSE;
}

extern "C" BOOL ParcelErrand_NextRecipient(ParcelErrand *z) {
    if (z->pendingRecipients) {
        u32 r = ParcelErrand_PickRecipient(z, z->pendingRecipients);
        if (r < 5) {
            ParcelErrand_RemoveRecipient(z, r);
            z->recipient = r;
            (z)->setStep(0);
            u16 v = sParcelItems[Random_GlobalBelow(10) & 1];
            ((ErrandRecord *)(void *)(z))->setItem(&v);
            r = ParcelErrand_CountPendingRecipients(z, z->pendingRecipients);
            if ((r == 2 && (Random_GlobalBelow(10) & 1)) || r == 1) {
                z->pendingRecipients = 0;
            }
            return TRUE;
        } else {
            z->pendingRecipients = 0;
        }
    }
    return FALSE;
}

extern "C" BOOL ParcelErrand_IsFor(ParcelErrand *z, u16 *p) {
    if (z->recipient < 5 && PlayerData_GetCurrent() && !((TownId *)(PlayerData_GetCurrent()->getPlayerId()))->getTownRelation()) {
        if (((s32)(*p & 0xf000) >> 12) == 0xd && ((ErrandRecord *)(z))->isActive() && ((ErrandRecord *)(z))->getStep() == 0) {
            BOOL in = FALSE;
            if (*p >= 0xd019 && *p <= 0xd01c) {
                in = TRUE;
            }
            if (in) {
                if (z->recipient == 0) {
                    return TRUE;
                }
            } else {
                u16 v = sParcelRecipients[z->recipient];
                BOOL same;
                if (Item_IsFurniture(p)) {
                    u16 t = v;
                    if (Item_GetFurnitureIndex(p) == Item_GetFurnitureIndex(&t)) {
                        same = TRUE;
                    } else {
                        same = FALSE;
                    }
                } else {
                    if (*p == v) {
                        same = TRUE;
                    } else {
                        same = FALSE;
                    }
                }
                if (same) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

HouseVisitInvite::HouseVisitInvite() : visitTime(0), visitTimeHi(0) {
    VillagerId_Clear(this);
    (&errand)->clear();
    visitTime = 0;
    visitTimeHi = 0;
    MI_CpuFill8(unk_20, 0, 1);
}


HouseVisitInvite::~HouseVisitInvite() {
    }


extern "C" void HouseVisitInvite_Clear(HouseVisitInvite *x) {
    (&x->errand)->clear();
    VillagerId_Clear(x);
    x->visitTime = 0;
    x->visitTimeHi = 0;
    MI_CpuFill8(x->unk_20, 0, 1);
}

extern "C" BOOL HouseVisitInvite_IsFrom(HouseVisitInvite *x, VillagerId *p) {
    if (((ErrandRecord *)(&x->errand))->isActive() && ((VillagerId *)(p))->isValid() && p->townId == x->townId &&
        memcmp(p->townName, x->townName, 8) == 0 && p->species == x->species) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void HouseVisitInvite_Set(HouseVisitInvite *x, VillagerId *r, void *src) {
    HouseVisitInvite_Clear(x);
    u16 v = 0xfff1;
    (&x->errand)->start(0x15, &v, 0);
    (&x->errand)->setTimeFromNow(0);
    VillagerId_Copy(x, r);
    MI_CpuCopy8(src, &x->visitTime, 8);
}

PlayerErrands::PlayerErrands() {
    }


PlayerErrands::~PlayerErrands() {
    }


extern "C" void PlayerErrands_Clear(PlayerErrands *m) {
    for (s32 i = 0; i < 2; i++) {
        PlayerErrandSlot_Clear(&m->slots[i]);
    }
    PlayerErrands_ClearJobVillagers(m);
    ParcelErrand_Clear(&m->parcelErrand);
    HouseVisitInvite_Clear(&m->houseVisitInvite);
}

extern "C" PlayerErrandSlot *PlayerErrands_GetSlot(PlayerErrands *m, s32 i) {
    PlayerErrandSlot *r = 0;
    if (i >= 0 && i < 2) {
        r = &m->slots[i];
    }
    return r;
}

extern "C" PlayerErrandSlot *PlayerErrandSlots_FindByVillager(PlayerErrandSlot *arr, VillagerId *p, s32 idx) {
    PlayerErrandSlot *ret = 0;
    if (((VillagerId *)(void *)(p))->isValid()) {
        s32 i;
        for (i = 0; i < 2; i++) {
            PlayerErrandSlot *e = &arr[i];
            if (((ErrandRecord *)(PlayerErrandSlot_GetRecord(e)))->isActive()) {
                VillagerIdCtorDtor *q = PlayerErrandSlot_GetVillager(e, idx);
                if (q->townId == p->townId && memcmp(q->townName, p->townName, 8) == 0 && q->species == p->species) {
                    ret = e;
                    break;
                }
            }
        }
    }
    return ret;
}

// the name of the villager an item is for (the parcel's recipient or an errand slot's villager) into a
extern "C" BOOL PlayerErrands_GetDeliveryRecipientName(PlayerErrands *m, MsgString *a, u16 *p) {
    BOOL ret = FALSE;
    BOOL in = FALSE;
    if (*p >= 0x155f && *p <= 0x1560) {
        in = TRUE;
    }
    if (in) {
        if (((ErrandRecord *)ParcelErrand_GetRecord(&m->parcelErrand))->isActive()) {
            ret = ParcelErrand_GetRecipientName(&m->parcelErrand, a);
            goto end;
        }
    }
    {
        s32 k = 0;
        BOOL in2 = FALSE;
        if (*p >= 0x1561 && *p <= 0x1564) {
            in2 = TRUE;
        }
        if (in2) {
            k = 0;
        }
        PlayerErrandSlot *e = PlayerErrands_GetSlot(m, k);
        if (e) {
            ErrandRecord *y = PlayerErrandSlot_GetRecord(e);
            if ((y)->isActive()) {
                u16 *q = (y)->getItem();
                BOOL same;
                if (Item_IsFurniture(q)) {
                    if (Item_GetFurnitureIndex(q) == Item_GetFurnitureIndex(p)) {
                        same = TRUE;
                    } else {
                        same = FALSE;
                    }
                } else {
                    if (*q == *p) {
                        same = TRUE;
                    } else {
                        same = FALSE;
                    }
                }
                if (same) {
                    ret = PlayerErrandSlot_GetVillager(e, 1)->getName(a);
                }
            }
        }
    }
end:
    return ret;
}

extern "C" void PlayerErrands_ClearJobVillagers(PlayerErrands *m) {
    for (s32 i = 0; i < 3; i++) {
        VillagerId_Clear(&m->jobVillagers[i]);
    }
}

extern "C" BOOL PlayerErrands_IsJobActive(PlayerErrands *m) {
    ErrandRecord *y = PlayerErrandSlot_GetRecord(PlayerErrands_GetSlot(m, 0));
    if ((y)->isActive() && (y)->getSubGroup() == 1) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void Arbeit_Start(PlayerErrands *m) {
    PlayerErrandSlot *e = PlayerErrands_GetSlot(m, 0);
    PlayerErrandSlot_Clear(e);
    PlayerErrandSlot_Start(e, 0xb, 0, 0);
    PlayerErrands_ClearJobVillagers(m);
    Arbeit_ClearOnDuty(m);
}

extern "C" void Arbeit_SetOnDuty(PlayerErrands *m) {
    m->onDuty = 1;
}

extern "C" void Arbeit_ClearOnDuty(PlayerErrands *m) {
    m->onDuty = 0;
}

extern "C" BOOL Arbeit_IsOnDuty(PlayerErrands *m) {
    if (m->onDuty == 1) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void Arbeit_StartPlanting(PlayerErrands *m) {
    PlayerErrandSlot_Start(PlayerErrands_GetSlot(m, 0), 0xc, 0, 0);
}

extern "C" void Arbeit_StartGreetings(PlayerErrands *m) {
    PlayerErrandSlot_Start(PlayerErrands_GetSlot(m, 0), 0xd, 0, 0);
}

extern "C" void Arbeit_StartFurnitureDelivery(PlayerErrands *m) {
    PlayerErrandSlot *e = PlayerErrands_GetSlot(m, 0);
    void *t = SaveVillagers_PickRandomTalkPartner(gSaveVillagers, 0, 0);
    PlayerErrandSlot_Start(e, 0xe, 0, ((VillagerData *)(t))->getVillagerId());
    VillagerId_Copy(&m->jobVillagers[0], ((VillagerData *)(t))->getVillagerId());
}

extern "C" void Arbeit_StartLetterTask(PlayerErrands *m) {
    PlayerErrandSlot *e = PlayerErrands_GetSlot(m, 0);
    PlayerErrandSlot_GetRecord(e);
    VillagerIdCtorDtor *a[1];
    func_02133ef8(a, 4);
    s32 n = 0;
    if (((VillagerId *)(void *)(&m->jobVillagers[0]))->isValid()) {
        a[0] = &m->jobVillagers[0];
        n = 1;
    }
    void *t = SaveVillagers_PickRandomTalkPartner(gSaveVillagers, a, n);
    PlayerErrandSlot_Start(e, 0xf, 0, ((VillagerData *)(t))->getVillagerId());
    VillagerId_Copy(&m->jobVillagers[1], ((VillagerData *)(t))->getVillagerId());
}

extern "C" void Arbeit_OnLetterWritten(void *p) {
    ErrandRecord *y = PlayerErrandSlot_GetRecord(PlayerErrands_GetSlot((PlayerErrands *)p, 0));
    if ((y)->getKind() == 0xf) {
        if ((y)->getStep() == 0) {
            (y)->setStep(1);
        }
    }
}

extern "C" void Arbeit_NotifyLetterWritten() {
    void *r = (void *)PlayerData_GetCurrent();
    if (r) {
        Arbeit_OnLetterWritten(((PlayerData *)(r))->getErrands());
    }
}

extern "C" BOOL Arbeit_OnLetterSent(PlayerErrands *m, void *p) {
    if (p && ((LetterView *)(p))->getState()) {
        PlayerErrandSlot *e = PlayerErrands_GetSlot(m, 0);
        ErrandRecord *y = PlayerErrandSlot_GetRecord(e);
        if ((y)->getKind() == 0xf) {
            LetterParty *v = Letter_GetRecipientVillager((LetterView *)p);
            if (v) {
                VillagerIdCtorDtor tmp((VillagerId *)v); // (the party record starts with the villager's id)
                if (tmp.isValid()) {
                    VillagerIdCtorDtor *q = PlayerErrandSlot_GetVillager(e, 1);
                    if (q->townId == tmp.townId && memcmp(q->townName, tmp.townName, 8) == 0 && q->species == tmp.species) {
                        if ((y)->getStep() < 2) {
                            (y)->setStep(2);
                            return TRUE;
                        }
                        goto out;
                    }
                }
                if ((y)->getStep() == 0) {
                    (y)->setStep(1);
                    return TRUE;
                }
            out:;
            } else {
                if ((y)->getStep() == 0) {
                    (y)->setStep(1);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

extern "C" void Arbeit_StartCarpetDelivery(PlayerErrands *m) {
    PlayerErrandSlot *e = PlayerErrands_GetSlot(m, 0);
    VillagerIdCtorDtor *a[2];
    func_02133ef8(a, 8);
    s32 n = 0, i = n;
    for (; i < 2; i++) {
        VillagerIdCtorDtor *r = &m->jobVillagers[i];
        if (((VillagerId *)(void *)(r))->isValid()) {
            a[n] = r;
            n++;
        }
    }
    void *t = SaveVillagers_PickRandomTalkPartner(gSaveVillagers, a, n);
    PlayerErrandSlot_Start(e, 0x10, 0, ((VillagerData *)(t))->getVillagerId());
    VillagerId_Copy(&m->jobVillagers[2], ((VillagerData *)(t))->getVillagerId());
}

extern "C" void Arbeit_StartWateringCanDelivery(PlayerErrands *m) {
    PlayerErrandSlot_Start(PlayerErrands_GetSlot(m, 0), 0x11, 0, &m->jobVillagers[1]);
}

extern "C" void Arbeit_StartBbsTask(PlayerErrands *m) {
    PlayerErrandSlot_Start(PlayerErrands_GetSlot(m, 0), 0x12, 0, 0);
}

extern "C" void Arbeit_OnBbsPosted(PlayerErrands *m) {
    ErrandRecord *r = PlayerErrandSlot_GetRecord(PlayerErrands_GetSlot(m, 0));
    if ((r)->isActive()) {
        if ((r)->getKind() == 0x12) {
            if ((r)->getStep() == 0) {
                (r)->setStep(1);
            }
        }
    }
}

extern "C" void Arbeit_Finish(PlayerErrands *m) {
    if (PlayerErrands_IsJobActive(m)) {
        PlayerErrandSlot_Clear(PlayerErrands_GetSlot(m, 0));
    }
}

extern "C" BOOL Arbeit_IsLetterRecipient(PlayerErrands *m, VillagerId *r) {
    if (((VillagerId *)(void *)(r))->isValid() && PlayerErrands_IsJobActive(m) && r->townId == m->jobVillagers[1].townId &&
        memcmp(r->townName, m->jobVillagers[1].townName, 8) == 0 && r->species == m->jobVillagers[1].species) {
        return TRUE;
    }
    return FALSE;
}


// ======== unk_02098f90.cpp ========
extern "C" {
s32 Item_GetKind(u16 *);
void Item_FromPlacedForm(u16 *, u16 *);
s32 func_01ffcb0c(s32, s32);
s32 FX_InvSqrt(s32);
s32 Date_DaysBetween(u8 *a, u8 *b);
void Clock_GetDate(u8 *out);
}


static inline BOOL Unk_02099124_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}extern "C" {
}


extern "C" s32 Pocket_CountKind(PocketMatches *self, s32 v);
extern "C" void PocketMatches_Init(PocketMatches *p);
extern "C" s32 Pocket_FindEmpty();
extern "C" BOOL Pocket_AddItem(u16 *a, s32 b);
extern "C" u16 Pocket_GetItem(s32 i);
extern "C" void Pocket_RemoveItem(s32 c);
extern "C" void Pocket_SetItem(u16 *a, s32 b, s32 c);
extern "C" BOOL Pocket_AddFoundItem(u16 *a);
extern "C" void Pocket_SetFoundItem(u16 *a, s32 b);
extern "C" u16 Item_PickRandomPresent();
extern "C" s32 Inventory_FindEmptyLetter();
extern "C" void Main_EmptyInitStub();
extern "C" void Quat_Mul(s32 *a, s32 *b, s32 *out);
extern "C" void Quat_ToMtx43(s32 *a, s32 *b);
extern "C" void Quat_ToMtx33(s32 *a, s32 *m);
extern "C" void Quat_Normalize(s32 *q);
extern "C" s32 FishSpawn_GetWeightClass(u32 v);
extern "C" s32 InsectSpawn_GetWeightClass(u32 v);
extern "C" s32 Threshold_FindIndex(u32 v, const u8 *tbl, s32 n);

// (called with PlayerData::getErrands; the decomp had it on SickVillagerRecord, whose bytes at 0x78 it returned)
ParcelErrand *PlayerErrands::getParcelErrand() {
    return &parcelErrand;
}


SickVillagerRecord *SickVillagerRecord::constructRecord() {
    errand.init();
    VillagerId_Construct(&villagerId);
    for (s32 i = 0; i < 5; i++) new (&visitors[i]) PlayerId((void *)NULL); // (o unused by the constructor)
    resetRecord();
    return this;
}


SickVillagerRecord *SickVillagerRecord::destructRecord() {
    for (s32 i = 5; i-- > 0;) new (&visitors[i]) PlayerId(); // (the original's destructor runs the constructor's code)
    VillagerId_Destruct(&villagerId);
    errand.func_0209ada0();
    return this;
}


void SickVillagerRecord::resetRecord() {
    s32 i;
    ((ErrandRecord *)(this))->clear();
    VillagerId_Clear(&villagerId);
    for (i = 0; i < 5; i++) ((PlayerId *)(&visitors[i]))->clear();
    startDate[0] = 1;
    startDate[1] = 1;
    startDate[2] = 0;
    startDate[3] = 0;
    statusDate[0] = 1;
    statusDate[1] = 1;
    statusDate[2] = 0;
    statusDate[3] = 0;
    sickDay = 5;
}


ErrandRecord *SickVillagerRecord::getErrand() {
    return &errand;
}


VillagerId *SickVillagerRecord::getVillagerId() {
    return &villagerId;
}


void SickVillagerRecord::startSickness(VillagerId *a, u8 *b) {
    u16 v;
    resetRecord();
    VillagerId_Copy(&villagerId, a);
    v = 0x155e;
    ((ErrandRecord *)(this))->start(9, &v, 0);
    MI_CpuCopy8(b, startDate, 4);
    MI_CpuCopy8(b, statusDate, 4);
    statusDate[3] = 1;
    sickDay = Random_GlobalBelow(2) + 1;
}


PlayerId *SickVillagerRecord::getVisitor(u32 i) {
    if (i < 5) return &visitors[i];
    return NULL;
}


PlayerId *SickVillagerRecord::getTodaysVisitor() {
    return getVisitor(sickDay);
}


BOOL SickVillagerRecord::hasVisitor(PlayerId *e) {
    if (((PlayerId *)(e))->isValid()) {
        PlayerId *p = visitors;
        s32 i;
        for (i = 0; i < 5; i++) {
            if (e->townId == p->townId && memcmp(e->townName, p->townName, 8) == 0 && ((PlayerId *)(e))->equals((PlayerId *)(p))) return TRUE;
        }
    }
    return FALSE;
}


BOOL SickVillagerRecord::hasTodaysVisitor() {
    PlayerId *p = getTodaysVisitor();
    if (p && ((PlayerId *)(p))->isValid()) return TRUE;
    return FALSE;
}


void SickVillagerRecord::setTodaysVisitor(PlayerId *e) {
    PlayerId *p = getTodaysVisitor();
    if (p) ((PlayerId *)(p))->setRaw(e);
}


BOOL SickVillagerRecord::isRecovered() {
    if (sickDay == 5) return TRUE;
    return FALSE;
}


BOOL SickVillagerRecord::isRecentlyRecovered(ClockDate *d) {
    ClockDate local;
    if (d == NULL) {
        Clock_GetDate((u8 *)&local);
        d = &local;
    }
    if (isRecovered()) {
        s32 r = Date_DaysBetween((u8 *)d, statusDate);
        if (r >= 0 && r < 3) return TRUE;
        return FALSE;
    }
    return FALSE;
}


PlayerId *SickVillagerRecord::getTopVisitor() {
    u8 counts[5];
    s32 mask;
    s32 i, j, k;
    s32 grp, f1, f2;
    PlayerId *pi, *e;
    u8 *cnt;
    s32 off;
    if (!isRecovered()) goto ret0;
    mask = 0;
    MI_CpuFill8(counts, 0, 5);
    for (i = 0; i < 5; i++) {
        if ((mask >> i) & 1) continue;
        e = visitors + i;
        if (!((PlayerId *)(e))->isValid()) continue;
        grp = 0;
        mask |= 1 << i;
        mask = (u8)mask;
        cnt = &counts[i];
        counts[i] = 1;
        j = i + 1;
        pi = (PlayerId *)((u8 *)this + i * 0x16);
        for (; j < 5; j++) {
            f1 = 0;
            f2 = 0;
            off = j;
            off = off * 0x16;
            if (*(u16 *)((u8 *)pi + 0x18) == *(u16 *)((u8 *)this + off + 0x18)) {
                if (memcmp(e->townName, ((PlayerId *)((u8 *)visitors + off))->townName, 8) == 0) f2 = 1;
            }
            if (f2 && ((PlayerId *)(e))->equals((PlayerId *)((PlayerId *)((u8 *)visitors + off)))) f1 = 1;
            if (f1) {
                grp |= 1 << j;
                grp = (u8)grp;
                mask |= 1 << j;
                mask = (u8)mask;
                (*cnt)++;
            }
        }
        if (grp) {
            for (k = i + 1; k < 5; k++) {
                if ((grp >> k) & 1) counts[k] = *cnt;
            }
        }
    }
    if (mask) {
        u8 *cp = counts;
        s32 maxc = 0;
        s32 best = -1;
        for (i = 0; i < 5; cp++, i++) {
            s32 c = *cp;
            if (c > maxc) {
                maxc = c;
                best = i;
            } else if (c != 0) {
                if (c == maxc) best = i;
            }
        }
        if ((u32)best < 5) return &visitors[best];
    }
ret0:
    return NULL;
}


extern "C" s32 Threshold_FindIndex(u32 v, const u8 *tbl, s32 n) {
    s32 i;
    for (i = 0; i < n; i++) {
        if (v >= *tbl) return i;
        tbl++;
    }
    return n;
}

extern "C" s32 InsectSpawn_GetWeightClass(u32 v) {
    return Threshold_FindIndex(v, data_020d05a0, 4);
}

extern "C" s32 FishSpawn_GetWeightClass(u32 v) {
    return Threshold_FindIndex(v, data_020d0598, 4);
}

#pragma thumb off
extern "C" void Quat_Normalize(s32 *q) {
    s64 sum = (s64)q[0] * q[0];
    sum += (s64)q[1] * q[1];
    sum += (s64)q[2] * q[2];
    sum += (s64)q[3] * q[3];
    s32 len = FX_InvSqrt((s32)(sum >> 12));
    q[0] = (s32)(((s64)len * q[0] + 0x800) >> 12);
    q[1] = (s32)(((s64)len * q[1] + 0x800) >> 12);
    q[2] = (s32)(((s64)len * q[2] + 0x800) >> 12);
    q[3] = (s32)(((s64)len * q[3] + 0x800) >> 12);
}
#pragma thumb reset

extern "C" void Quat_ToMtx33(s32 *a, s32 *m) {
    s32 xx = func_01ffcb0c(a[0], a[0]);
    s32 xy = func_01ffcb0c(a[0], a[1]);
    s32 xz = func_01ffcb0c(a[0], a[2]);
    s32 zz = func_01ffcb0c(a[2], a[2]);
    s32 zy = func_01ffcb0c(a[2], a[1]);
    s32 yy = func_01ffcb0c(a[1], a[1]);
    s32 wx = func_01ffcb0c(a[3], a[0]);
    s32 wy = func_01ffcb0c(a[3], a[1]);
    s32 wz = func_01ffcb0c(a[3], a[2]);
    m[0] = 0x1000 - 2 * (yy + zz);
    m[1] = 2 * (xy + wz);
    m[2] = 2 * (xz - wy);
    m[3] = 2 * (xy - wz);
    m[4] = 0x1000 - 2 * (xx + zz);
    m[5] = 2 * (zy + wx);
    m[6] = 2 * (wy + xz);
    m[7] = 2 * (zy - wx);
    m[8] = 0x1000 - 2 * (xx + yy);
}

extern "C" void Quat_ToMtx43(s32 *a, s32 *b) {
    Quat_ToMtx33(a, b);
    b[9] = 0;
    b[10] = 0;
    b[11] = 0;
}

extern "C" void Quat_Mul(s32 *a, s32 *b, s32 *out) {
    s32 o0, o1, o2, o3;
    o3 = func_01ffcb0c(a[3], b[3]) - func_01ffcb0c(a[0], b[0]) - func_01ffcb0c(a[1], b[1]) - func_01ffcb0c(a[2], b[2]);
    o0 = func_01ffcb0c(a[1], b[2]) + (func_01ffcb0c(a[3], b[0]) + func_01ffcb0c(a[0], b[3])) - func_01ffcb0c(a[2], b[1]);
    o1 = func_01ffcb0c(a[2], b[0]) + (func_01ffcb0c(a[3], b[1]) + func_01ffcb0c(a[1], b[3])) - func_01ffcb0c(a[0], b[2]);
    o2 = func_01ffcb0c(a[0], b[1]) + (func_01ffcb0c(a[3], b[2]) + func_01ffcb0c(a[2], b[3])) - func_01ffcb0c(a[1], b[0]);
    out[0] = o0;
    out[1] = o1;
    out[2] = o2;
    out[3] = o3;
}

extern "C" void Main_EmptyInitStub() {
}

extern "C" s32 Inventory_FindEmptyLetter() {
    return ((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->findEmptyLetter();
}

extern "C" Letter *Inventory_GetEmptyLetter() {
    return (Letter *)((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getEmptyLetter();
}

extern "C" u16 Item_PickRandomPresent() {
    u16 out[2];
    ItemPickSpec obj;
    obj.set(0, 3);
    ItemPick_One(out, &obj, 0, 0, 1, 1, 0);
    return out[0];
}

extern "C" void Pocket_SetFoundItem(u16 *a, s32 b) {
    u16 v[2];
    BOOL r = FALSE;
    v[0] = 0xfff1;
    if (*a == 0x156b) {
        v[0] = Item_PickRandomPresent();
        r = TRUE;
    } else {
        Item_FromPlacedForm(&v[1], a);
        v[0] = v[1];
    }
    Pocket_SetItem(v, r, b);
}

#ifdef N3DS_DIRECT
// (n3ds_options.h)
extern "C" int Pocket_FindFruitStack(u16 *a) {
    PlayerInventory *inv = (PlayerInventory *)PlayerData_GetCurrent()->getInventory();
    s32 f = n3ds_fruit_kind(*a);
    if (!n3ds_stack_fruit() || n3ds_fruit_count(*a) != 1 || ((CommManager *)(gCommManager))->isOnline()) return -1;
    for (s32 i = 0; i < 15; i++) {
        u16 v = *inv->getPocket(i);
        if (n3ds_fruit_kind(v) == f && n3ds_fruit_count(v) < N3DS_FRUIT_MAX && inv->getPocketFlags(i) == 0) return i;
    }
    return -1;
}
#endif

extern "C" BOOL Pocket_AddFoundItem(u16 *a) {
#ifdef N3DS_DIRECT
    s32 k = Pocket_FindFruitStack(a);
    if (k != -1) {  // onto the stack (stacked fruit, n3ds_options.h)
        u16 v = *((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getPocket(k);
        v = n3ds_fruit_make(n3ds_fruit_kind(v), n3ds_fruit_count(v) + 1);
        Pocket_SetItem(&v, 0, k);
        return TRUE;
    }
#endif
    s32 r = Pocket_FindEmpty();
    if (r == -1) return FALSE;
    if (*a == 0xfff1) return TRUE;
    if (*a >= 0xa7 && *a <= 0xc6) return TRUE;
    Pocket_SetFoundItem(a, r);
    return TRUE;
}

extern "C" void Pocket_SetItem(u16 *a, s32 b, s32 c) {
    void *r6 = PlayerData_GetCurrent();
    if (Pocket_IsValidIndex(c)) {
        if (*a != 0xfff1 && b == 0) {
            Catalog_SetItem(((PlayerData *)(r6))->getCatalog(), a, 0, 1);
        }
        if (b == 0) {
            if (Unk_02099124_R(a, 0x151f, 0x151f)) {
                u16 t = 0x1033;
                Catalog_SetItem(((PlayerData *)(r6))->getCatalog(), &t, 0, 1);
            }
        }
        ((PlayerInventory *)(((PlayerData *)(r6))->getInventory()))->setPocket(a, c, b);
    }
}

extern "C" void Pocket_RemoveItem(s32 c) {
    void *r4 = PlayerData_GetCurrent();
    if (Pocket_IsValidIndex(c)) {
        u16 t = 0xfff1;
#ifdef N3DS_DIRECT
        // a fruit stack (n3ds_options.h): one fruit went (Pocket_GetItem gave one), the rest stays
        t = n3ds_fruit_rest(*((PlayerInventory *)(((PlayerData *)(r4))->getInventory()))->getPocket(c));
#endif
        ((PlayerInventory *)(((PlayerData *)(r4))->getInventory()))->setPocket(&t, c, 0);
    }
}

extern "C" u16 Pocket_GetItem(s32 i) {
#ifdef N3DS_DIRECT
    // the NPCs take a pocket's item with this and Pocket_RemoveItem: of a fruit stack one fruit (n3ds_options.h)
    return n3ds_fruit_one(*((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getPocket(i));
#endif
    return *((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->getPocket(i);
}

extern "C" BOOL Pocket_AddItem(u16 *a, s32 b) {
    u16 t;
    s32 r = Pocket_FindEmpty();
    if (r != -1) {
        Item_FromPlacedForm(&t, a);
        Pocket_SetItem(&t, b, r);
        return TRUE;
    }
    return FALSE;
}

extern "C" s32 Pocket_FindEmpty() {
    return ((PlayerInventory *)PlayerData_GetCurrent()->getInventory())->findEmptyPocket();
}

extern "C" void PocketMatches_Init(PocketMatches *p) {
    p->count = 0;
    p->pocketMask = 0;
}

extern "C" s32 Pocket_CountKind(PocketMatches *self, s32 v) {
    void *r7 = PlayerData_GetCurrent();
    s32 i;
    PocketMatches_Init(self);
    u16 *tbl = ((PlayerInventory *)(((PlayerData *)(r7))->getInventory()))->getPocket(0);
    for (i = 0; i < 15; i++) {
        if (((PlayerInventory *)(((PlayerData *)(r7))->getInventory()))->isPocketFlagsClear(i)) {
            if (v == Item_GetKind(tbl + i)) {
                self->pocketMask |= (1 << i);
                self->count++;
            }
        }
    }
    return self->count;
}


// ======== unk_0209865c.cpp (0x02098e90..0x02098f90) ========
extern "C" {
}

extern "C" s32 Pocket_CountMatching(PocketMatches *out, s32 (*fn)(u16 *)) {
    PlayerData *o = PlayerData_GetCurrent();
    PocketMatches_Init(out);
    u16 *p = ((PlayerInventory *)(o->getInventory()))->getPocket(0);
    s32 i;
    for (i = 0; i < 15; i++) {
        if (((PlayerInventory *)(o->getInventory()))->isPocketFlagsClear(i)) {
            if (fn(p + i)) {
                out->pocketMask |= 1 << i;
                out->count++;
            }
        }
    }
    return out->count;
}

extern "C" s32 Pocket_FindItem(u16 *a) {
    PlayerData *o = PlayerData_GetCurrent();
    u16 *p = ((PlayerInventory *)(o->getInventory()))->getPocket(0);
    s32 i;
    for (i = 0; i < 15; i++) {
        if (((PlayerInventory *)(o->getInventory()))->isPocketFlagsClear(i)) {
            u16 *e = &p[i];
            BOOL r;
            if (Item_IsFurniture(e)) {
                r = (Item_GetFurnitureIndex(e) == Item_GetFurnitureIndex(a)) ? TRUE : FALSE;
            } else {
                u32 x = p[i];
                u32 y = *a;
                r = (x == y) ? TRUE : FALSE;
            }
            if (r) return i;
        }
    }
    return -1;
}

extern "C" BOOL Player_GetDeliveryRecipientName(MsgString *a, u16 *b) {
    PlayerData *o = PlayerData_GetCurrent();
    return PlayerErrands_GetDeliveryRecipientName(o->getErrands(), a, b);
}

// ======== data (rest; this order gives the original layout) ========
const u32 sPlanErrandStepCounts[5] = {5, 5, 5, 7, 7};
void *data_020e21b8[2] = {(void *)ErrandSetup_WateringCan, 0};
const u32 sErrandClassStarts[2] = {0, 5};
char data_020e21f0[10] = "st_object";
char data_020e21d8[9] = "st_drink";
const u32 sErrandGroupStarts[4] = {0, 10, 0x14, 0x15};
char data_020e21e4[10] = "st_sports";
const u8 sTrendWeights[8] = {3, 3, 5, 5, 3, 3, 2, 1};
char data_020e21b0[8] = "st_food";
const u16 sParcelItems[2] = {0x155f, 0x1560};
void *data_020e21a8[2] = {(void *)ErrandSetup_LetterBundle, 0};
const u16 sParcelRecipients[5] = {0xd019, 0xd004, 0xd006, 0xd007, 0xd00c};
char data_020e21cc[9] = "st_music";
const u8 sPlanStateGroupStarts[3] = {0, 5, 8};
const TopicWordList sTopicWordLists[6] = {
    {data_020e21cc, 0x20}, {data_020e21e4, 0x20}, {data_020e21c0, 0x24},
    {data_020e21d8, 0x18}, {data_020e21f0, 0x28}, {data_020e21b0, 0x20},
};
const u8 sFurnitureGoalCounts[6] = {2, 4, 6, 8, 10, 13};
const u8 data_020d05a0[4] = {0x0f, 0x0a, 6, 3};
void *data_020e2198[2] = {(void *)ErrandSetup_FurnitureParcel, 0};
const u8 data_020d0598[4] = {0x11, 0x0b, 6, 3};
ErrandKindSetup sErrandKindSetups[10] = {
    {*(Unk_0209a4f4_Fn *)data_020e2190, 0, 0},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 0},
    {*(Unk_0209a4f4_Fn *)data_020e2198, 0, 0},
    {0, 0, 1},
    {*(Unk_0209a4f4_Fn *)data_020e21a0, 0, 0},
    {*(Unk_0209a4f4_Fn *)data_020e21b8, 0, 0},
    {0, 0, 0},
    {*(Unk_0209a4f4_Fn *)data_020e21a8, 0, 0},
};

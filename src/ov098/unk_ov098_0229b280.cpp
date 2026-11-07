#include "types.h"
#include "n3ds_options.h"
#include "host_labels.h"
#include "host_shared_calls.h"
#include "Unk_020d8c7c.h"
#include "net/CommManager.h"
#include "player/PlayerData.h"
#include "save/TownExchangeRecord.h"
#include "menu/MenuProc.h"
#include "player/PlayerData.h"
#include "menu/MenuErrorMessage.h"
#include "menu/PocketMenu.h"
#include "actor/Actor.h"
#include "gfx/VecFx32.h"
#include "town/BlockMap.h"
#include "item/LetterView.h"
#include "sys/Heap.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0205b7b0.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02065330.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_0208f0b0.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/ov002/unk_ov002_022013ac.h"
#include "proto/ov003/unk_ov003_02204ce8.h"
#include "proto/ov003/unk_ov003_0221ffb8.h"
#include "proto/ov003/unk_ov003_02225800.h"
#include "proto/ov094/unk_ov094_02292360.h"
#include "proto/ov096/unk_02294c40.h"
#include "proto/ov098/unk_ov098_0229b280.h"
#include "proto/main/unk_0204cc1c.h"

class TownBlockMap;

// Functions whose definitions use file-local types (not in the shared headers).
extern "C" {
s32 FieldAction_RequestTool(s32 a, s32 *unit, s32 sel, s32 d, u16 item);  // unit: {x, z} (Unk_02042588_Pos)
s32 FieldAction_RequestAtFreeUnit(s32 a, s32 b, u16 c);
s32 FieldAction_PollResult(s32 a);
void FieldAction_Release(s32 a);
BOOL Ground_FindWaterAhead(VecFx32 *out, VecFx32 *pos, s32 dir, u32 dist, u32 s5, s32 s6);
void MI_CpuCopy8(const void *src, void *dest, u32 size);
void BottleThrow_SetTarget(VecFx32 *v, s32 i);
s32 PlayerActor_LocalRequestBuryItem(VecFx32 *p, u16 *b);

extern BlockMap *gSceneBlockMap;
#define data_021e7f8c HOST_LABEL(u8 (*)[], data_021e7f8c)  // the town exchange record (TownExchangeRecord) of the save data
extern u16 gPad[];
extern "C" s16 sBuryHoleAngles[16] = {0, 0x1800, -0x1800, 0};
}

extern "C" CommManager *gCommManager;




// ---------------------------------------------------------------------------------------------

static inline BOOL Unk_ov098_RangeCheck(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_ov098_0229b2d8_Both() {
    if (gTouchHeld && gTouchChanged) {
        return TRUE;
    }
    return FALSE;
}

struct Unk_ov098_0229b790_Pt {
    s32 x, y;
    constexpr Unk_ov098_0229b790_Pt(s32 a, s32 b) : x(a), y(b) {}
};

extern "C" {
}

extern "C" s32 PocketMenu_CanDropOutdoor(PocketMenu *self, u32 b) {
    InvItemStackPad pad;
    if (PlayerData_GetCurrent()->testFlag(1) != 0) {
        if ((b >= 0x14fe && b <= 0x1517) || (b >= 0x151d && b <= 0x151e)) {
            return 0;
        }
    }
    return Shared_InvItem_IsNotFishInsectOrFlower(b);
}

void PocketMenu::addFieldOptions(u16 v) {
    struct {
        u16 a;
    } l;
    l.a = v;
    BOOL f1 = FALSE;
    u16 a = *(volatile u16 *)&l.a;
    u16 b = *(volatile u16 *)&l.a;
    if (b >= 0x12b0 && a <= 0x12e7) {
        f1 = TRUE;
    }
    if (f1 || (a >= 0x12e8 && a <= 0x131f)) {
        ChoiceIdList_Add(optionList, 0x1b, 0x21);
    }
    BOOL f2 = FALSE;
    u16 c = *(volatile u16 *)&l.a;
    u16 d = *(volatile u16 *)&l.a;
    if (d >= 0x12b0 && c <= 0x12e7) {
        f2 = TRUE;
    }
    if (f2) {
        ChoiceIdList_Add(optionList, 8, 0x15);
    } else if (c >= 0x12e8 && c <= 0x131f) {
        if (findWaterNearPlayer(0)) {
            ChoiceIdList_Add(optionList, 8, 0x18);
        }
    } else if ((c >= 0x137c && c <= 0x137c) || (c >= 0x1408 && c <= 0x1428) || (c >= 0x1471 && c <= 0x1491) ||
               (c >= 0x14fe && c <= 0x1517) || (c >= 0x151d && c <= 0x151e) || (c >= 0x1567 && c <= 0x1567)) {
        ChoiceIdList_Add(optionList, 6, 0x17);
    } else if (findBuryHole()) {
        ChoiceIdList_Add(optionList, 7, 0x16);
    }
}

void PocketMenu::addBottleOption() {
    clearFlags(8);
    if (!gCommManager->isOnline()) {
        if (findWaterNearPlayer(1)) {
            ChoiceIdList_Add(optionList, 0x11, 0x19);
            setFlags(8);
        }
    }
}

BOOL PocketMenu::findWaterNearPlayer(s32 flag) {
    Actor *p = (Actor *)PlayerActor_GetActor(4);
    s32 base;
    VecFx32 *q;
    q = &p->position;
    base = 0xa00;
    u32 f = BlockMap_GetBlockAttr(gSceneBlockMap, p->position.x >> 17, q->z >> 17);
    if (flag) {
        if ((f & 0x7f000) == 0 && (f & 8) == 0) {
            return FALSE;
        }
        base = 0x1b58;
    } else {
        if ((f & 0x7f000) != 0 || (f & 8) != 0) {
            base += 0xe10;
        } else {
            base += 0x3e8;
        }
    }
    return Ground_FindWaterAhead(&waterPos, q, p->rotY, 0x7800, base, 0xc);
}

void PocketMenu::actionReleaseFish() {
    setMainState(0x30);
    mainAct30();
}

void PocketMenu::mainAct30() {
    u32 r6 = Shared_PocketMenu_GetPlayerSlot();
    s32 a = PocketMenu_GetItem(this, actionTarget);
    if (FishCatch_StartRelease(r6, a, &waterPos)) {
        struct {
            u16 a;
        } l;
        l.a = a;
        BOOL ok = FALSE;
        volatile u16 *pv = &l.a;
        u16 v = *pv;
        u16 w = *pv;
        if (w >= 0x12e8 && v <= 0x131f) {
            ok = TRUE;
        }
        s32 r1;
        if (ok) {
            r1 = v - 0x12e8;
        } else {
            r1 = -1;
        }
        sendFishReleasePacket((u8)r1);
        PlayerActor_LocalReleaseCatch(1);
        Shared_PocketMenu_StartRemoveBlink(this);
    }
}

void PocketMenu::sendReleasePacket(u8 a, u8 b) {
    if (gCommManager->isOnline()) {
        u8 pkt[12];
        pkt[0] = b;
        pkt[1] = a;
        NetBuf_PackPair20(&pkt[7], waterPos.x, waterPos.z);
        MI_CpuCopy8(&pkt[7], &pkt[2], 5);
        CommManager *g = gCommManager;
        g->beginRecord();
        g->writeRecord(pkt, 7);
        g->endRecord(0x16, 4);
    }
}

void PocketMenu::sendFishReleasePacket(u8 a) {
    sendReleasePacket(a, 2);
}

void PocketMenu::actionReleaseInsect() {
    setMainState(0x2c);
    mainAct2C();
}

void PocketMenu::mainAct2C() {
    u32 r7 = Shared_PocketMenu_GetPlayerSlot();
    if (HeldInsect_GetStage(r7) == 0) {
        struct { u16 a; } l;
        l.a = PocketMenu_GetItem(this, actionTarget);
        BOOL ok = FALSE;
        volatile u16 *pv = &l.a;
        u16 a = *pv;
        u16 b = *pv;
        if (b >= 0x12b0 && a <= 0x12e7) {
            ok = TRUE;
        }
        s32 r5 = ok ? a - 0x12b0 : -1;
        u32 t = (u8)Random_GlobalBelow(0x3c);
        s16 x = (t - 0x1e) * 0xb6;
        x += ((Actor *)PlayerActor_GetActor(4))->rotY;
        HeldInsect_Start((u8)r5, r7);
        HeldInsect_Release(r7, x);
        PlayerActor_LocalReleaseCatch(0);
        sendInsectReleasePacket((u8)r5, t);
        Shared_PocketMenu_StartRemoveBlink(this);
    }
}

void PocketMenu::sendInsectReleasePacket(u8 a, u32 b) {
    if (gCommManager->isOnline()) {
        u8 buf[3];
        buf[0] = 1;
        buf[1] = a;
        buf[2] = b;
        CommManager *g = gCommManager;
        g->beginRecord();
        g->writeRecord(buf, 3);
        g->endRecord(0x16, 4);
    }
}

// PocketMenu::getDirOffset's static tbl
Unk_ov098_0229b790_Pt PocketMenu_getDirOffset_tbl[16] = {
        Unk_ov098_0229b790_Pt(0, 1),   Unk_ov098_0229b790_Pt(1, 1),   Unk_ov098_0229b790_Pt(1, 1),
        Unk_ov098_0229b790_Pt(1, 0),   Unk_ov098_0229b790_Pt(1, 0),   Unk_ov098_0229b790_Pt(1, -1),
        Unk_ov098_0229b790_Pt(1, -1),  Unk_ov098_0229b790_Pt(0, -1),  Unk_ov098_0229b790_Pt(0, -1),
        Unk_ov098_0229b790_Pt(-1, -1), Unk_ov098_0229b790_Pt(-1, -1), Unk_ov098_0229b790_Pt(-1, 0),
        Unk_ov098_0229b790_Pt(-1, 0),  Unk_ov098_0229b790_Pt(-1, 1),  Unk_ov098_0229b790_Pt(-1, 1),
        Unk_ov098_0229b790_Pt(0, 1),
    };
s32 *PocketMenu::getDirOffset(s16 a) {
    return &PocketMenu_getDirOffset_tbl[(a >> 12) & 15].x;
}

BOOL PocketMenu::findBuryHole() {
    u16 *pv = PlayerData_GetCurrent()->getHeldItem();
    BOOL r = FALSE;
    u32 v = *pv;
    if (v >= 0x1369 && v <= 0x1369) {
        r = TRUE;
    }
    if (!r) {
        if (v >= 0x136a && v <= 0x136a) {
        } else {
            return FALSE;
        }
    }
    Actor *q = (Actor *)PlayerActor_GetActor(4);
    s32 px = 0, py = 0;
    BlockMap *grid = TownBlockMap_Get();
    FieldPos_ToUnit(&px, &py, &q->position);
    s32 zero1 = 0, zero2 = 0;
    s16 base = q->rotY;
    for (s32 i = 0; i < 3; i++) {
        s32 *p = getDirOffset(base + sBuryHoleAngles[i]);
        s32 x = px + p[0];
        s32 y = py + p[1];
        s32 hx = x >> 4;
        s32 hy = y >> 4;
        u16 *c = BlockMap_GetItemPtr(grid, hx, hy, x - (hx << 4), y - (hy << 4), zero1);
        if (c) {
            BOOL ok = zero2;
            if (*c >= 0xfc && *c <= 0xfd) {
                ok = TRUE;
            }
            if (ok) {
                digUnitX = x;
                digUnitY = y;
                return TRUE;
            }
        }
    }
    return FALSE;
}

void PocketMenu::actionBuryItem() {
    if (findBuryHole() == 0) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 0xd, 0xff, 1);
    } else {
        s32 a = PocketMenu_GetItem(this, actionTarget);
        s32 pair[2];
        pair[0] = digUnitX;
        pair[1] = digUnitY;
#ifdef N3DS_DIRECT
        a = n3ds_fruit_one(a);  // of a fruit stack one fruit is buried (mainAct2E)
#endif
        fieldRequest = FieldAction_RequestTool(gCommManager->myAid, pair, 2, 0, a);
        if (fieldRequest == -1) {
            PocketMenu_ReturnToIdle(this);
            PocketMenu_ShowMessage(this, 0xd, 0xff, 1);
        } else {
            setMainState(0x2d);
        }
    }
}

void PocketMenu::mainAct2D() {
    switch (FieldAction_PollResult(fieldRequest)) {
    case 1:
        setMainState(0x2e);
        mainAct2E();
        goto done;
    case 2:
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 3, 0xff, 1);
    done:
        FieldAction_Release(fieldRequest);
        fieldRequest = -1;
    }
}

void PocketMenu::mainAct2E() {
    u16 v;
    VecFx32 out;
    FieldPos_FromUnitCenter(&out, digUnitX, digUnitY);
    v = PocketMenu_GetItem(this, actionTarget);
#ifdef N3DS_DIRECT
    v = n3ds_fruit_one(v);  // a fruit stack (n3ds_options.h): one fruit goes into the hole, the rest stays
#endif
    if (PlayerActor_LocalRequestBuryItem(&out, &v)) {
#ifdef N3DS_DIRECT
        if (n3ds_fruit_count(PocketMenu_GetItem(this, actionTarget)) > 1)
            PocketMenu_SetSlotItem(this, actionTarget, n3ds_fruit_rest(PocketMenu_GetItem(this, actionTarget)), 0);
        else
#endif
        PocketMenu_ClearSlotItem(this, actionTarget);
        setMainState(0x2f);
    }
}

void PocketMenu::actionPlantItem() {
    s32 a = PocketMenu_GetItem(this, actionTarget);
    if (findBuryHole()) {
        s32 pair[2];
        pair[0] = digUnitX;
        pair[1] = digUnitY;
        fieldRequest = FieldAction_RequestTool(gCommManager->myAid, pair, 2, 0, a);
        if (fieldRequest != -1) {
            setMainState(0x2d);
            return;
        }
    }
    fieldRequest = FieldAction_RequestAtFreeUnit(gCommManager->myAid, 0x18, a);
    if (fieldRequest == -1) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 8, 0xff, 0);
        Snd_PlaySe(0x73);
    } else {
        setMainState(0x29);
        setFlags(0x1000);
    }
}

void PocketMenu::actionThrowBottle() {
    sendBottleLetter();
    PlayerActor_LocalRequestThrowBottle();
    BottleThrow_SetTarget(&waterPos, Shared_PocketMenu_GetPlayerSlot());
    PocketMenu_ClearLetter(this, actionTarget);
    setMainState(0x33);
}

void PocketMenu::mainAct33() {
    if (BottleThrow_IsActive(Shared_PocketMenu_GetPlayerSlot()) == 1) {
        setMainState(0x34);
    }
}

void PocketMenu::mainAct34() {
    if (BottleThrow_IsActive(Shared_PocketMenu_GetPlayerSlot()) == 0) {
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::sendBottleLetter() {
    sendReleasePacket(0, 5);
    LetterView *p = PocketMenu_GetLetter(this, actionTarget);
    CommManager *g = gCommManager;
    if (!g->isOnline() || g->myAid == 0) {
        TownExchangeRecord *const d = (TownExchangeRecord *)data_021e7f8c;
        Letter_Copy((LetterView *)TownExchange_GetLetter(d), p);
        d->resetCounter();
        d->setUnkFlag(0);
    } else {
        Heap *heap = gMenuHeap;
        u8 *buf = (u8 *)Heap_AllocTail(heap, 0xf5);
        buf[0] = 6;
        MI_CpuCopy8(p, buf + 1, 0xf4);
        CommManager *g2 = gCommManager;
        g2->beginRecord();
        g2->writeRecord(buf, 0xf5);
        g2->endRecord(0x16, 0);
        Heap_Free(heap, buf);
    }
}

void PocketMenu::actionAct21() {
    setMainState(0x35);
    requestCameraPop();
    hideCursor();
}

void PocketMenu::mainAct35() {
    if (PlayerActor_LocalRequestReleaseCreature(PocketMenu_GetItem(this, actionTarget))) {
        setMainState(0x36);
    }
}

void PocketMenu::mainAct36() {
    if (PlayerActor_IsLocalReleaseWaiting()) {
        setMainState(0x37);
        errorMessage.showPromptOnly();
    }
}

void PocketMenu::mainAct37() {
    errorMessage.updatePromptBalloon();
    if (MenuCtrl_IsForceCloseDue() != 0 || Unk_ov098_0229b2d8_Both() || (gPad[1] & 1) || (gPad[1] & 2)) {
        setMainState(0x38);
        errorMessage.hidePromptBalloon();
    }
}

void PocketMenu::mainAct38() {
    errorMessage.updatePromptBalloon();
    if (PlayerActor_ConfirmReleaseCreature()) {
        setMainState(0x39);
    }
}

void PocketMenu::mainAct39() {
    errorMessage.updatePromptBalloon();
    if (PlayerActor_IsInAction(6, 4) == 0) {
        errorMessage.undim();
        PocketMenu_ReturnToIdle(this);
    }
}

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov098_0229b280) {
    HOST_OVERLAY_VAR(sBuryHoleAngles),
    HOST_OVERLAY_VAR(PocketMenu_getDirOffset_tbl),
    HOST_OVERLAY_VARS_END
};

// mwcc-version: 1.2/base
// The 14 functions of the translation unit 0x02004558-0x0201106c that only mwcc 1.2/base compiles
// to the original code (see unk_02004558.cpp; same declarations, nothing else is defined here).
#include "types.h"
#ifdef HOST
#include "host_input.h"
#endif
#include "proto/main/unk_020b4828.h"
#include "gfx/VecFx32.h"
#include "net/CommManager.h"
#include "talk/TalkWindowState.h"
#include "gfx/NNSG3dRS.h"
#include "player/PendingFieldActionFx.h"
#include "player/Unk_02005f50_Area.h"
#include "player/FieldActionFxEvent.h"
#include "player/Unk_02006d14_Pair.h"
#include "player/Unk_02006d14_V3.h"
#include "player/Unk_02006d14_Trip.h"
#include "player/PlayerPickUpWork.h"
#include "player/Unk_02006d14_Ptr.h"
#include "player/PlayerInitWork.h"
#include "player/PlayerActionRequestBase.h"
#include "game/Unk_0200f6d4_V2.h"
#include "player/Unk_0200f070_V3.h"
#include "player/Unk_020107c8_Blk.h"
#include "player/PlayerNetActionArgs.h"
#include "player/PlayerU16ArgsRequest.h"
#include "player/PlayerAct76Args.h"
#include "player/PlayerTurnToWork.h"
#include "player/Unk_020092c8_Loc.h"
#include "player/PlayerChangeHeldItemWork.h"
#include "player/PlayerAct32Locals.h"
#include "player/PlayerAct30Args.h"
#include "player/PlayerPickUpFanfareArgs.h"
#include "talk/TalkMsgRequest.h"
#include "snd/SndSeEmitterKind99.h"
#include "snd/SndSeEmitterKind1.h"
#include "talk/MsgRequest.h"
#include "player/PlayerActionWork.h"
#include "actor/Character.h"
#include "actor/Actor.h"
#include "gfx/MatTexVramTask.h"
#include "player/PM_020076f0.h"
#include "player/PM_020063a0.h"
#include "player/PlayerWalkToRequest.h"
#include "gfx/MatTexPatAnim.h"
#include "sys/ProcBase.h"
#include "player/Unk_02007694.h"
#include "player/PlayerWalkToWork.h"
#include "player/PlayerAct76Request.h"
#include "player/Unk_02005e7c.h"
#include "player/PlayerAct30Request.h"
#include "player/PlayerPickUpReachRequest.h"
#include "player/PlayerWalkToArgs.h"
#include "player/PlayerActionRequest.h"
#include "player/PMRaw.h"
#include "snd/SndSeEmitter.h"
#include "actor/BlinkTimer.h"
#include "player/PlayerGlassesModelRef.h"
#include "player/PlayerFaceTexRef.h"
#include "player/PlayerBodyWorkRef.h"
#include "player/PlayerBodyModelRef.h"
#include "actor/CharaFaceAnimWorkRef.h"
#include "actor/CharaFaceAnimRef.h"
#include "actor/CharaClothTexRef.h"
#include "player/PlayerPaletteRef.h"
#include "player/Unk_0200bc78_Vec.h"
#include "player/AnimSlotRef.h"
#include "game/Unk_0203d820_Ptr.h"
#include "gfx/Mtx43.h"
#include "snd/BgmManager.h"
#include "player/PlayerWalkWork.h"
#include "player/PlayerHead.h"
#include "player/PlayerChangeClothesArgs.h"
#include "player/PlayerAct10Args.h"
#include "player/Unk_0200b908_Obj.h"
#include "player/PlayerNoArgsRequest.h"
#include "player/PlayerPickUpReachArgs.h"
#include "player/PlayerPickUpArgs.h"

class PlayerActor;
class Unk_02007694;
class PlayerActor;
struct PlayerActionRequest;
struct Unk_02006d14_Pair;
struct Unk_0200b750_Pair;
struct PlayerActionRequest;
struct Unk_0200f6d4_V2;
struct Unk_020107c8_Blk;


// ---- member-function-pointer constants as named objects (see notes.txt)
typedef void (*PMF)();

// ---- library base class chain of the object (declarations only; vtables and code are in other units)






// ---- sound emitters of PlayerActor (snd/SndSeEmitterKind1.h, snd/SndSeEmitterKind99.h). SndSeEmitterKind1 has no key
// function: this unit emits its link-once vtable and D1 0x02004b48 / D0 0x02010fa4, so the destructor that the header
// declares out of line is defined inline here.
inline SndSeEmitterKind1::~SndSeEmitterKind1() {}










#include "player/PlayerActor.h"
#include "gfx/AnimFrameCtrl.h"
#include "gfx/Camera.h"
#include "gfx/Model.h"
#include "gfx/TwoLayerAnimModel.h"
#include "item/ItemName.h"
#include "player/FishBobber.h"
#include "player/PlayerData.h"
#include "talk/ChoiceList.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_020e9a08.h"
#include "proto/autoload_2/unk_020ef150.h"
#include "proto/autoload_2/unk_02128030.h"
#include "proto/itcm/unk_01ffb4e8.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02004558.h"
#include "proto/main/unk_02004558_extra.h"
#include "proto/main/unk_0201106c.h"
#include "proto/main/unk_020119cc.h"
#include "proto/main/unk_0202e840.h"
#include "proto/main/unk_02034438.h"
#include "proto/main/unk_0203c638.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_0203ecec.h"
#include "gfx/ScreenTransition.h"
#include "proto/main/unk_020411dc.h"
#include "proto/main/unk_020571f4.h"
#include "proto/main/unk_0205b7b0.h"
#include "proto/main/unk_0205c66c.h"
#include "proto/main/unk_0205c91c.h"
#include "proto/main/unk_0205d480.h"
#include "proto/main/unk_0205d7d8.h"
#include "proto/main/unk_0205dfa4.h"
#include "proto/main/unk_0205edfc.h"
#include "proto/main/unk_0205ef08.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_0206d988.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206fc44.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_020815f8.h"
#include "proto/main/unk_0208eeac.h"
#include "proto/main/unk_02090268.h"
#include "proto/main/unk_02093ff0.h"
#include "proto/main/unk_020943dc.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02095cdc.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02097d1c.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_0209c37c.h"
#include "proto/main/unk_0209c4a8.h"
#include "proto/main/unk_020abbcc.h"
#include "proto/main/unk_020af258.h"
#include "proto/main/unk_020b0a80.h"
#include "proto/main/unk_020b0e60.h"
#include "proto/main/unk_020b5e5c.h"
#include "proto/main/unk_020b7190.h"
#include "proto/main/unk_020b7d10.h"
#include "proto/ov003/unk_ov003_02204ce8.h"
#include "proto/ov003/unk_ov003_02225800.h"
#include "proto/ov004/unk_ov004_0222bed0.h"
#include "proto/ov004/unk_ov004_02233074.h"
#include "proto/main/unk_0204cc1c.h"
#include "unk_02004558_shared.h"



























// ---- unk_020044dc.cpp
namespace nA {
extern "C" {

extern CommManager *gCommManager;
union PM_02004ce8 {
    PMRaw raw;
    void (PlayerActor::*fn)();
};
}
}

// ---- unk_02004e0c.cpp
namespace nB {
extern "C" {

extern s16 data_02135f44[];
extern CommManager *gCommManager;
extern s16 data_02135f44[];
union PM_02005294 {
    PMRaw raw;
    void (PlayerActor::*fn)();
};
}
}

// ---- unk_02005e7c.cpp
namespace nC {
extern "C" {

void PlayerActor_RequestTreeShake(void *p, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void PlayerActor_RequestPluck(void *p, void *v, s32 a, s32 b);
void PlayerActor_RequestAxeBreak(void *p, s32 a, void *v, s32 b, s32 c);
void PlayerActor_RequestAxeChop(void *p, s32 a, s32 b, void *v, s32 c, s32 d);
void PlayerActor_RequestDig(void *p, s32 a, void *v, s32 b, s32 c);
void PlayerActor_RequestDigUpItem(void *p, void *v, s32 a, s32 b, s32 c);
void PlayerActor_RequestFillHole(void *p, s32 a, void *v, s32 b, s32 c, s32 d);
void PlayerActor_RequestShovelStrike(void *p, s32 a, void *v, s32 b, s32 c);
void PlayerActor_RequestAxeStrike(void *p, s32 a, s32 b, void *v, s32 c, s32 d);
}
}

// ---- unk_02006d14.cpp
namespace nD {
extern "C" {

typedef void (PlayerActor::*Unk_02006d14_Fn)(PlayerActionRequest*, u32);
extern CommManager* gCommManager;
union PM_02006d14 {
    PMRaw raw;
    Unk_02006d14_Fn fn;
};
}
}

// ---- unk_02007694.cpp
namespace nE {
extern "C" {

s32 FX_Div(s32 a, s32 b);
}
}

// ---- unk_02008040.cpp
namespace nF {
extern "C" {

extern CommManager *gCommManager;
void PlayerActor_ApplyHoldOffset(void *, void *);
void Fish_GetDisplayScale(void *, s32);
void FishCatch_SetDisplayPosScale(void *, void *, void *);
void PlayerActor_RequestFishShowCatch(PlayerActor *, u32, u32, s32);
void PlayerActor_RequestInsectShowCatch(PlayerActor *, u32, u32, u32, u32, s32);
void PlayerActor_RequestStowItem(PlayerActor *, u32, u32, u32, u32, u32, u32, s32);
void Camera_SetMode4();
void Camera_SetModeDefault();
static inline BOOL Unk_02008858_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}
}
}

// ---- unk_02008cc0.cpp
namespace nG {
extern "C" {

inline s32 Unk_0200905c_abs(s32 x) { return x < 0 ? -x : x; }
extern CommManager *gCommManager;
extern s16 data_02135f44[];
s32 FX_Div(s32, s32);
s32 func_01ffcb0c(s32, s32);
s32 Math_Atan2(s32, s32);
}
}

// ---- unk_020095f8.cpp
namespace nH {
extern "C" {

extern CommManager* gCommManager;
extern s16 data_02135f44[];
s32 func_01ffcb0c(s32 a, s32 b);
s32 Math_Atan2(s32 a, s32 b);
static inline BOOL Unk_02009624_Check()
{
    if (gFieldSceneKind == 0) {
        return TRUE;
    }
    return FALSE;
}
}
}

// ---- unk_02009f68.cpp
namespace nI {
extern "C" {

static inline BOOL Unk_0200a114_IsZero(u8* p)
{
    if (*p == 0) return TRUE;
    return FALSE;
}
extern CommManager* gCommManager;
extern BlockMap *gSceneBlockMap;
void Camera_SetMode4();
void Camera_SetModeDefault();
}
}

// ---- unk_0200abc8.cpp
namespace nJ {
extern "C" {

extern CommManager *gCommManager;
extern BlockMap *gSceneBlockMap;
static inline BOOL Unk_0200add8_R1(u16* p, u32 lo, u32 hi) { BOOL r = FALSE; if (*p >= lo && *p <= hi) r = TRUE; return r; }
static inline BOOL Unk_0200add8_InRange(u32 c, u32 lo, u32 hi) { BOOL r = FALSE; if (c >= lo && c <= hi) r = TRUE; return r; }
static inline BOOL Unk_0200add8_IsFFF1(u16* p, u16* t) {
    BOOL r;
    if (Item_IsFurniture(p)) {
        *t = 0xfff1;
        r = Item_GetFurnitureIndex(p) == Item_GetFurnitureIndex(t) ? TRUE : FALSE;
    } else {
        r = *p == 0xfff1 ? TRUE : FALSE;
    }
    return r;
}
}
}

// ---- unk_0200b510.cpp
namespace nK {
extern "C" {

static inline void func_0200bc78_sub(VecFx32Ctor *o, VecFx32Ctor *a, VecFx32Ctor *b) {
    o->x = a->x - b->x;
    o->z = a->z - b->z;
}
extern BlockMap *gSceneBlockMap;
extern CommManager *gCommManager;
s32 Math_Atan2(s32, s32);
}
}

// ---- unk_0200be2c.cpp
namespace nL {
extern "C" {

void VillagerStates_ClearUnk1dBit0();
s32 func_01ffcb0c(s32 a, s32 b);
extern CommManager *gCommManager;
extern s16 data_02135f44[];
}
}

// ---- unk_0200c778.cpp
namespace nM {
extern "C" {

extern s16 data_02135f44[];
extern CommManager *gCommManager;
extern BgmManager *data_021c1b3c;  // (the BGM manager)
s32 FX_Div(s32 a, s32 b);
s32 func_01ffcb0c(s32 a, s32 b);
s32 FX_Sqrt(s32 a);
s32 Scene_NoPlayerInUnsharedScene();
}
}

// ---- unk_0200d2b4.cpp
namespace nN {
extern "C" {

s32 Scene_NoPlayerInUnsharedScene();
BOOL TouchPick_GetGroundPos(TouchPickResult *obj, VecFx32 *out);
BOOL Camera_ProjectCurvedToScreen(s32 *a, s32 *b, VecFx32Ctor *p);
s32 Math_Atan2(s32 a, s32 b);
s32 func_01ffcb0c(s32 a, s32 b);
s32 FX_Div(s32 a, s32 b);
s32 FX_Sqrt(s32 a);
extern s32 gGfxMainOnTop;
extern BlockMap *gSceneBlockMap;
extern s16 data_02135f44[];
extern CommManager *gCommManager;
extern Camera *gCamera;  // (defined in data)
static inline BOOL Unk_0200d64c_InRange(u16 h) {
    if (h >= 0xfc && h <= 0xfd) {
        return TRUE;
    }
    return FALSE;
}
static inline BOOL Unk_0200d64c_IsTwo() {
    return gScreenTransition.state == 2;
}
static inline BOOL Unk_0200d64c_Both() {
    return gTouchPrevHeld && gTouchPrevChanged;
}
}
}

// ---- unk_0200dde0.cpp
namespace nO {
extern "C" {

s32 func_01ffcb0c(s32 a, s32 b);
s32 Math_Atan2(s32 a, s32 b);
extern BgmManager *data_021c1b3c;  // (the BGM manager)
extern u8 gTalkMsgIndexEnd;
}
}

// ---- unk_0200e758.cpp
namespace nP {
extern "C" {

extern CommManager *gCommManager;
extern s16 data_02135f44[];
s32 Math_Atan2(s32 a, s32 b);
s32 func_01ffcb0c(s32 a, s32 b);
static inline BOOL Unk_0200ec54_Bit(u32 f, u32 m)
{
    return (f & m) != 0 ? TRUE : FALSE;
}
}
}

// ---- unk_0200f070.cpp
namespace nQ {
extern "C" {

extern s16 data_02135f44[];
extern CommManager *gCommManager;
extern BlockMap *gSceneBlockMap;
s32 func_01ffcb0c(s32 a, s32 b);
static inline void Unk_0200f070_Set(VecFx32 *r, s32 x, s32 y, s32 z)
{
    r->x = x;
    r->y = y;
    r->z = z;
}
namespace Unk_0200f7a0_NS {
}
}
}

// ---- unk_0200f9bc.cpp
namespace nR {
extern "C" {

inline BOOL Unk_0200f9d4_IsOne(u8 v) { return v == 1 ? TRUE : FALSE; }
inline BOOL Unk_020102a0_IsZero(u8 v) { return v == 0 ? TRUE : FALSE; }
s32 Math_Atan2(s32, s32);
s32 func_01ffcb0c(s32, s32);
inline BOOL Unk_02010154_In(u16 *p) { BOOL r = FALSE; u16 v = *p; if (*p >= 0x1380 && v <= 0x139f) r = TRUE; return r; }
}
}

// ---- unk_020102ec.cpp
namespace nS {
extern "C" {

extern CommManager *gCommManager;
}
}

// ---- unk_02010c74.cpp
namespace nT {
extern "C" {

s32 func_01ffcb0c(s32 a, s32 b);
extern CommManager *gCommManager;
}
}



void PlayerActor::nudgeForward() {
    using namespace nP;
    switch (Math_AngleToDir4(this->rotY)) {
    case 2: this->position.z -= 1; break;
    case 0: this->position.z += 1; break;
    case 3: this->position.x -= 1; break;
    case 1: this->position.x += 1; break;
    }
}

void PlayerActor::offsetSpawnBySlot() {
    using namespace nP;
    s32 r = Math_AngleToDir4(this->rotY);
    s32 step = this->sessionSlot;
    switch (r) {
    case 2: this->position.z -= step; break;
    case 0: this->position.z += step; break;
    case 3: this->position.x -= step; break;
    case 1: this->position.x += step; break;
    }
}

void PlayerActor::onMessageEnd(u32 attr) {
    using namespace nO;
    u8 buf[12];
    if (this->msgStep >= 0xf) return;
    switch (this->msgStep) {
    case 0:
        buf[0] = 0x3d;
        window->setNextMessage(&buf[0], NULL);
        break;
    case 4:
        buf[1] = 0x3e;
        window->setNextMessage(&buf[1], NULL);
        break;
    case 2:
        buf[2] = 3;
        window->setNextMessage(&buf[2], NULL);
        break;
    case 1:
        buf[3] = 0x3c;
        window->setNextMessage(&buf[3], NULL);
        this->msgStep = 9;
        break;
    case 5:
    case 6:
        data_021c1b3c->mixer.talkDuckHold = 0xb;
        break;
    case 13: {
        buf[4] = 0x3f;
        window->setNextMessage(&buf[4], NULL);
        *(u16 *)&buf[6] = this->actionWorkRaw[8] + 0x12b0;
        {
            ItemName obj((u16 *)&buf[6]);
            window->setNamedSlot(0, &obj, 7);
        }
        this->msgStep = 8;
        break;
    }
    case 14: {
        buf[5] = 0x3f;
        window->setNextMessage(&buf[5], NULL);
        this->msgStep = 8;
        *(u16 *)&buf[8] = this->actionWorkRaw[3] + 0x12e8;
        ItemName obj((u16 *)&buf[8]);
        window->setNamedSlot(0, &obj, 7);
        break;
    }
    }
}

void PlayerActor::onChoice(u32 attr) {
    using namespace nO;
    s32 st = this->msgStep;
    if (st >= 0xf) return;
    switch (st) {
    case 0:
    case 2:
    case 3:
    case 4: {
        s32 r5;
        u8 b;
        r5 = ((ChoiceList *)(window->getChoiceList()))->getResult();
        b = gTalkMsgIndexEnd;
        window->setNextMessage(&b, NULL);
        if (r5 == 0) {
            this->msgStep = 5;
        } else {
            switch (this->msgStep) {
            case 0:
            case 4:
                this->msgStep = 9;
                break;
            case 2:
                this->msgStep = 0xb;
                break;
            default:
                this->msgStep = 0xf;
                break;
            }
        }
        break;
    }
    }
}

void PlayerActor::onWindowClose() {
    using namespace nO;
    s32 st = this->msgStep;
    s32 r5;
    if (st >= 0xf) return;
    r5 = 6;
    switch (st) {
    case 11:
        r5 = 0x23;
    case 10:
        Bgm_Release(0x39);
        Bgm_RequestSilence(0xc, r5, r5 + 5);
        this->msgStep = 0xf;
        break;
    case 7: {
        u16 v[2];
        u16 r6 = *(u16 *)((PlayerActor *)this)->actionWorkRaw;
        BOOL ok;
        Bgm_Release(r6);
        PlayerActor_GetHeldItem(v, this);
        if (Item_IsFurniture(v)) {
            v[1] = 0xfff1;
            s32 r7 = Item_GetFurnitureIndex(v);
            if (r7 == Item_GetFurnitureIndex(&v[1])) ok = TRUE; else ok = FALSE;
        } else {
            if (v[0] == 0xfff1) ok = TRUE; else ok = FALSE;
        }
        if (!ok && !this->getHeldHoldableIndex()) {
            BOOL b = (gFieldSceneKind == 0);
            if (b) r5 += 0x15;
        }
        if ((u16)(r6 + 0xffc4) <= 1) {
            Bgm_RequestSilence(5, r5, r5 + 5);
        } else {
            Bgm_RequestSilence(0xc, r5, r5 + 5);
        }
        this->msgStep = 0xf;
        break;
    }
    case 5:
    case 8: {
        u32 arg;
        r5 = 0xc;
        if (this->action == 0x57) {
            arg = *(u16 *)&this->actionWorkRaw[4];
            if (arg == 0x3b) r5 = 5;
        } else if (this->action == 0x54) {
            arg = *(u16 *)((PlayerActor *)this)->actionWorkRaw;
            if (arg == 0x3a) r5 = 5;
        } else if (this->action == 0x1c) {
            break;
        } else if (this->action == 0x19) {
            break;
        } else if (this->action == 0x5f) {
            arg = 0x39;
        } else {
            break;
        }
        Bgm_Release(arg);
        Bgm_RequestSilence(r5, 0x14, 0x19);
        if (this->msgStep != 5) this->msgStep = 0xf;
        break;
    }
    case 9: {
        u32 arg;
        if (this->action == 0x57) {
            arg = *(u16 *)&this->actionWorkRaw[4];
            r5 += 0x1e;
        } else {
            arg = *(u16 *)((PlayerActor *)this)->actionWorkRaw;
        }
        Bgm_Release(arg);
        Bgm_RequestSilence(0xc, r5, r5 + 5);
        this->msgStep = 0xf;
        break;
    }
    case 12:
        Bgm_Release(0x42);
        Bgm_RequestSilence(0xc, r5, 0xb);
        this->msgStep = 0xf;
        break;
    }
}

void PlayerActor::readInput() {
    using namespace nN;
    s32 flag = 0;
    s32 mode = this->inputMode;
    s32 v4 = 0;
    void *v8 = NULL;
    s32 vc = 0;
    s32 v10 = this->actionHeld;
    s32 v14 = 0;
    s32 sp18, sp1c;
    Camera *sp20;
    s32 sp24, sp28, sp2c;
    u8 sp30;
    s32 sp34;
    s32 ax, ay, bx, by;
    s32 sp48, sp4c, len;
    VecFx32Ctor p50;
    VecFx32Ctor r5c;
    PlayerActor_OffsetByAngle(&r5c, this, &this->position, &this->rotY, &data_020d5e50);
    VecFx32Ctor p68;
    VecFx32Ctor pv[4];
    BOOL got = FALSE;
    this->actionPressed = 0;
    this->toolTargetKind = 0;
    this->touchTargetId = 0xff;
    if (Unk_0200d64c_IsTwo()) {
    if (this->testActionFlag(0xb) || this->testActionFlag(0x1b)) {
        this->inputMagnitude = 0;
        this->inputRun = 0;
        this->interactPressed = 0;
        this->interactTarget = 0;
        this->actionHeld = 0;
        this->hasTargetPos = 0;
        *(VecFx32Ctor *)&this->targetPos = r5c;
        if (this->testActionFlag(0x1b)) {
            if (Scene_NoPlayerInUnsharedScene() == 1) {
                if (!PlayerActor_IsWaitingForSlots()) {
                    this->clearActionFlag(0x1b);
                    FieldInfoBalloon_ClearNetMsg();
                    if (gCommManager->myAid == 0) {
                        if ((u8)(Net_GetMode() + 0xfd) <= 1) {
                            Net_WifiHostKeepAlive();
                        }
                    }
                    if (this->action == 2 || this->action == 0x83 || this->action == 0x28 || this->action == 8) {
                        TalkRequest_FinishSceneEntry();
                    }
                }
            }
        }
    } else {
    if (gGfxMainOnTop == 0) {
        vc = TouchPick_GetGroundPos(Scene_GetTouchPicker(), &p50);
        if (TouchPickResult_GetTarget(Scene_GetTouchPicker(), &p68, &sp34, &sp30)) {
            got = TRUE;
            {
                switch (sp34) {
                case 0:
                    break;
                case 1:
                    if (Unk_0200d64c_Both()) {
                        flag = 1;
                        r5c = p50;
                    }
                    break;
                case 2: case 3: case 6: case 7: case 9: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
                    if (Unk_0200d64c_Both()) {
                        v4 = 1;
                        v8 = TouchPick_GetTargetObject(Scene_GetTouchPicker(), 0, 0);
                        r5c = p50;
                    }
                    break;
                case 5:
                    if (Unk_0200d64c_Both()) {
                        this->toolTargetKind = 1;
                        if (this->getHeldToolKind() != 1) {
                            r5c = *(VecFx32Ctor *)&this->toolTargetPos = p68;
                        } else {
                            u16 *hp = BlockMap_GetItemPtrAtPos(gSceneBlockMap, &p68, 0);
                            BOOL is = FALSE;
                            u16 h = *hp;
                            if (h < 0xfc || h > 0xfd) {
                            } else {
                                is = TRUE;
                            }
                            if (is) {
                                ax = 0; ay = 0; bx = 0; by = 0;
                                FieldPos_ToUnit(&ax, &ay, (VecFx32Ctor *)&this->position);
                                FieldPos_ToUnit(&bx, &by, &p50);
                                if (ax != bx || ay != by) {
                                    r5c = *(VecFx32Ctor *)&this->toolTargetPos = p50;
                                } else {
                                    r5c = *(VecFx32Ctor *)&this->toolTargetPos = p68;
                                }
                            } else {
                                r5c = *(VecFx32Ctor *)&this->toolTargetPos = p68;
                            }
                        }
                    }
                    break;
                case 4:
                    if (Unk_0200d64c_Both()) {
                        this->toolTargetKind = 2;
                        r5c = *(VecFx32Ctor *)&this->toolTargetPos = p68;
                    }
                    break;
                case 8:
                case 10:
                    if (Unk_0200d64c_Both()) {
                        v14 = 1;
                        FieldPos_SnapToUnitCenter(&p68, &p68);
                        r5c = p68;
                    }
                    this->touchTargetId = sp30;
                    break;
                }
            }
        }
    }
    if (flag != 0 || (Stub_ReturnFalse() && vc == 0)) {
        this->inputMagnitude = 0;
        this->inputRun = 0;
        v10 = 1;
        this->actionPressed = 1;
        PlayerActor_OffsetByAngle(&r5c, this, &this->position, &this->rotY, &data_020d5e34);
        mode = 2;
    } else {
        s32 fast = 0;
        this->actionPressed = fast;
        if (this->inputMode == 2) {
            if (vc) mode = 2; else mode = 1;
        } else if (gPad.cur & 0x2ff3) {
            mode = 1;
        } else if (vc) {
            mode = 2;
        }
        if (mode == 2) {
            if (this->action >= 0x1d && this->action <= 0x23) {
                sp20 = gCamera;
                pv[1].x = this->bodyPos.x;
                pv[1].y = this->bodyPos.y;
                pv[1].z = this->bodyPos.z;
                sp24 = ((u16)(s16)(0x4000 - sp20->getPitch()) >> 4) * 2;
                sp2c = FX_Div(func_01ffcb0c(0x1266, data_02135f44[sp24]), data_02135f44[sp24 + 1]);
                sp28 = ((u16)sp20->getYaw() >> 4) * 2;
                pv[2].x = pv[1].x - func_01ffcb0c(sp2c, data_02135f44[sp28]);
                s32 t = func_01ffcb0c(sp2c, data_02135f44[sp28 + 1]);
                pv[2].z = pv[1].z - t;
                pv[0].x = p50.x - pv[2].x;
                pv[0].z = p50.z - pv[2].z;
                len = Vec_MagXZ(&pv[0]);
            } else {
                pv[0].x = p50.x - this->bodyPos.x;
                pv[0].z = p50.z - this->bodyPos.z;
                Camera_ProjectCurvedToScreen(&sp48, &sp4c, (VecFx32Ctor *)&this->bodyPos);
                sp48 += 0x80;
                sp4c += 0x60;
                pv[3].x = ((u8)gTouchX - sp48) << 8;
                pv[3].y = 0;
                pv[3].z = ((u8)gTouchY - sp4c) << 8;
                len = Vec_MagXZ(&pv[3]);
            }
            s32 res = func_01ffcb0c(len, 0x4f4);
            if (res >= 0x1000) {
                res = 0x1000;
            } else if (res <= 0x19a) {
                res = 0;
            } else {
                s32 t = FX_Sqrt(FX_Div((res + 0x39a) << 12, 0x139a) >> 12);
                sp18 = t << 12;
                for (sp1c = 0; sp1c < 3; sp1c++) {
                    t = func_01ffcb0c(t, sp18) >> 12;
                }
                res = t;
                if (res > 0x1000) {
                    res = 0x1000;
                } else if (res < 0) {
                    res = 0;
                }
            }
            this->inputMagnitude = res;
            if (res > 0xc32) {
                fast = 1;
            }
            if (res > 0) {
                this->inputAngle = Math_Atan2(pv[0].x, pv[0].z);
            }
            if (!got) {
                if (Unk_0200d64c_Both()) {
                    v14 = 1;
                    r5c = p50;
                }
            }
        } else {
            s32 keymask;
            if (TalkRequestFlags_IsResetti() || this->testActionFlag(0x13)) {
                keymask = 0x2fff;
            } else {
                keymask = 0x2ff3;
            }
            u32 keys = gPad.cur;
            if (keymask & keys) {
                mode = 1;
            } else {
                mode = this->inputMode;
            }
            if ((keys & 2) || (keys & 0x100) || (keys & 0x200)) {
                fast = 1;
            } else {
                fast = 0;
            }
            u32 kb = *(volatile u16 *)&gPad.trig;
            v4 = 1;
            if (!(kb & 1)) {
                v4 = 0;
            }
            this->actionPressed = v4;
            if (gPad.cur & 1) {
                v10 = 1;
            } else {
                v10 = 0;
                if (gPad.trig & 2) {
                    v14 = 1;
                    PlayerActor_OffsetByAngle(&r5c, this, &this->position, &this->rotY, &data_020d5e48);
                }
            }
            keys = gPad.cur;
            if ((keys & 0x80) || (keys & 0x40) || (keys & 0x10) || (keys & 0x20)) {
                if (fast) {
                    this->inputMagnitude = 0x1000;
                } else {
                    this->inputMagnitude = 0xc32;
#ifdef HOST
                    // (the 3DS's circle pad: walking as fast as it is tilted, as the stylus's distance does)
                    if (gHostStickTilt) this->inputMagnitude = gHostStickTilt < 0x400 ? 0x30c : gHostStickTilt * 0xc32 >> 12;
#endif
                }
                this->inputAngle = gPad.dir;
            } else {
                this->inputMagnitude = 0;
            }
        }
        this->inputRun = fast;
    }
    this->interactPressed = v4;
    this->interactTarget = (Character *)v8;
    this->actionHeld = v10;
    this->hasTargetPos = v14;
    *(VecFx32Ctor *)&this->targetPos = r5c;
    this->inputMode = mode;
    }
    }
}

void Unk_02007694::endChangeClothes(u32 a) {
    using namespace nL;
    PlayerChangeClothesWork *p = &((Unk_02007694 *)this)->clothesWork;
    if (p->isApplied == 0) {
        switch (p->changeKind) {
        case 0:
            changeClothesShirt();
            break;
        case 1:
            changeClothesFaceItem();
            break;
        case 2:
            changeClothesHat();
            break;
        case 3: {
            void *r = PlayerData_GetBySessionSlot(((Unk_02007694 *)this)->sessionSlot);
            if (r != NULL) {
                PlayerData_SetStungFace(r, 0);
                (&((Unk_02007694 *)this)->faceTex)->load(((PlayerData *)(r))->getFaceType());
                ((PlayerActor *)(this))->replayAnim();
                VillagerStates_ClearUnk1dBit0();
            }
            break;
        }
        }
    }
    if (gCommManager->isLocalSlot(((Unk_02007694 *)this)->sessionSlot)) {
        u32 i;
        switch (p->changeKind) {
        case 0:
            i = 0;
            break;
        case 1:
            i = 1;
            break;
        case 2:
            i = 2;
            break;
        case 3:
            PlayerActor_NetSendFaceChange();
            return;
        default:
            return;
        }
        ((PlayerActor *)(this))->netSendClothesChange(i, p->unk_00);
    }
}

void Unk_02007694::changeClothesApply() {
    using namespace nL;
    ((PlayerActor *)(this))->clearActionFlag(0xf);
    if ((&static_cast<AnimFrameCtrl &>(((Unk_02007694 *)this)->bodyModel))->hasPassedFrame(8)) {
        ((PlayerActor *)(this))->setActionFlag(0xf);
        switch (((Unk_02007694 *)this)->clothesWork.changeKind) {
        case 0:
            changeClothesShirt();
            break;
        case 1:
            changeClothesFaceItem();
            break;
        case 2:
            changeClothesHat();
            break;
        case 3: {
            void *r = PlayerData_GetBySessionSlot(((Unk_02007694 *)this)->sessionSlot);
            if (r != NULL) {
                PlayerData_SetStungFace(r, 0);
                (&((Unk_02007694 *)this)->faceTex)->load(((PlayerData *)(r))->getFaceType());
                u8 buf[2];
                buf[0] = static_cast<AnimFrameCtrl &>(((Unk_02007694 *)this)->bodyModel).playMode;
                ((PlayerActor *)(this))->setEyeAnimForBody(&data_020d5e3c, &buf[0]);
                buf[1] = static_cast<AnimFrameCtrl &>(((Unk_02007694 *)this)->bodyModel).playMode;
                ((PlayerActor *)(this))->setMouthAnimForBody(&data_020d5e44, &buf[1]);
                VillagerStates_ClearUnk1dBit0();
            }
            break;
        }
        }
    }
}

void PlayerActor::pickUpReachUpdate() {
    using namespace nJ;
    PlayerPickUpWork* s = (PlayerPickUpWork *)((PlayerActor *)this)->actionWorkRaw;
    u8* st = &s->unk_04.b.unk_04;
    struct { u32 pad; Vec2 p[2]; } l;
    switch (*st) {
    case 1: {
        u8 a = s->unk_04.b.unk_05;
        u8 b = s->unk_04.b.unk_06;
        s32 v;
        u8 flag;
        if (gCommManager->isLocalSlot(this->sessionSlot)) {
            v = this->fieldAnswerKind;
        } else {
            v = s->unk_04.b.unk_07;
        }
        flag = 1;
        switch (v) {
        case 3:
            flag = 0;
        case 4: {
            l.p[0].x = a;
            l.p[0].y = b;
            requestPickUpAt(&l.p[0], -1, flag, 6, -1);
            break;
        }
        case 0x15:
            flag = 0;
        case 0x16: {
            l.p[1].x = a;
            l.p[1].y = b;
            this->requestPickUpFanfareAt((Unk_02006d14_Pair *)(void *)(&l.p[1]), flag, 6, -1);
            break;
        }
        }
        break;
    }
    case 2:
        this->actionPriority = ((Unk_02007694 *)(this))->getActionDonePriority(this->action);
        this->requestWait(3, 1, -1);
        break;
    case 3:
        if (TalkRequest_AddPlayerMessage()) {
            *st = 4;
            attachTalkRequest(this);
            this->setActionFlag(0x11);
            setFileName((const char *)sPlayerActorMsgFile);
            this->msgIndex = 0;
            this->window->nextState = 1;
        }
        break;
    case 4:
        if (this->window != NULL) {
            if (this->window->state != 0) {
                *st = 5;
            }
        }
        break;
    case 5:
        if (this->window != NULL) {
            if (this->window->state == 0) {
                detachTalkRequest(this);
                this->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                *st = 6;
            }
        }
        break;
    case 6:
        this->actionPriority = ((Unk_02007694 *)(this))->getActionDonePriority(this->action);
        this->requestWait(3, 1, -1);
        this->clearActionFlag(0xd);
        break;
    case 7:
        if (TalkRequest_AddPlayerMessage()) {
            *st = 8;
            attachTalkRequest(this);
            this->setActionFlag(0x11);
            setFileName((const char *)sPlayerActorErrorMsgFile);
            this->msgIndex = 1;
            this->window->nextState = 1;
        }
        break;
    case 8:
        if (this->window != NULL) {
            if (this->window->state != 0) {
                *st = 9;
            }
        }
        break;
    case 9:
        if (this->window != NULL) {
            if (this->window->state == 0) {
                detachTalkRequest(this);
                this->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                this->actionPriority = ((Unk_02007694 *)(this))->getActionDonePriority(this->action);
                this->requestWait(3, 1, -1);
            }
        }
        break;
    }
}

void PlayerActor::pickUpUpdateStore(u8* state, u8 flag) {
    using namespace nI;
    switch (*state) {
    case 0:
        if (!((AnimFrameCtrl *)(&(AnimFrameCtrl &)((PlayerActor *)this)->bodyModel))->isFinished()) break;
        if (this->actionItem == 0x1520) {
            if (BottleLetter_Open()) {
                *state = 6;
                break;
            }
            if (!TalkRequest_AddPlayerMessage()) break;
            this->startAnim(0x6c, 0, 0);
            s32 r = this->getHeldToolKind();
            if (r == 4) {
                HeldItemModel_PlayAnim(&this->heldItemModel, 0, 9, 0);
            } else if (r == 3) {
                HeldItemModel_PlayAnim(&this->heldItemModel, 0x13, 3, 0);
            }
            *state = 7;
            ((Character *)(this))->attachTalkRequest((PlayerActor *)this);
            this->setActionFlag(0x11);
            setFileName((const char *)sPlayerActorErrorMsgFile);
            this->msgIndex = 0x12;
            this->window->nextState = 1;
        } else if (flag) {
            if (!TalkRequest_IsPlayerMessage() && !TalkRequest_AddPlayerMessage()) break;
            this->startAnim(0x6c, 0, 0);
            s32 r = this->getHeldToolKind();
            if (r == 4) {
                HeldItemModel_PlayAnim(&this->heldItemModel, 0, 9, 0);
            } else if (r == 3) {
                HeldItemModel_PlayAnim(&this->heldItemModel, 0x13, 3, 0);
            }
            *state = 1;
            ((Character *)(this))->attachTalkRequest((PlayerActor *)this);
            this->setActionFlag(0x11);
            setFileName((const char *)sPlayerActorMsgFile);
            this->msgIndex = 2;
            this->window->nextState = 1;
        } else {
            u16 v[2];
            v[1] = this->actionItem;
            Pocket_AddFoundItem(&v[1]);
            *state = 6;
        }
        break;
    case 1:
        if (!this->window) break;
        if (!this->window->state) break;
        *state = 2;
        this->msgStep = 3;
        break;
    case 2:
        if (this->msgStep >= 15) {
            *state = 3;
        } else {
            if (!this->window) break;
            if (this->window->state) break;
            if (this->msgStep == 5) {
                if (!MenuCtrl_OpenPocketsFullPickUp(this->actionItem)) break;
                this->msgStep = 6;
                break;
            } else if (this->msgStep != 6) {
                break;
            }
            if (!MenuCtrl_IsFinished()) break;
            this->msgStep = 15;
            this->bodyModel.frameStep = 0x1000;
            if (MenuCtrl_IsResultOk()) {
                *state = 6;
                ((Character *)(this))->detachTalkRequest((PlayerActor *)this);
                this->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                break;
            }
            *state = 3;
        }
    case 3:
        if (this->dropQuery != -1) break;
        this->dropQuery = FieldAction_RequestPlaceAtPendingForAid(this->sessionSlot, this->actionItem);
        if (this->dropQuery == -1) break;
        this->startAnim(0, 6, 6);
        *state = 4;
        if (this->getHeldToolKind() != 4) break;
        HeldItemModel_PlayAnim(&this->heldItemModel, 0, 9, 0);
        break;
    case 4:
        if (this->dropQuery != -1) break;
        *state = 5;
    case 5:
        if (!this->window) break;
        if (this->window->state) break;
        ((Character *)(this))->detachTalkRequest((PlayerActor *)this);
        this->clearActionFlag(0x11);
        TalkRequest_FinishPlayerMessage();
        *state = 6;
        this->startAnim(0, 6, 6);
        if (this->getHeldToolKind() == 4) {
            HeldItemModel_PlayAnim(&this->heldItemModel, 0, 9, 0);
        }
        break;
    case 6:
        this->actionPriority = ((Unk_02007694 *)(this))->getActionDonePriority(this->action);
        this->requestWait(3, 1, -1);
        this->clearActionFlag(0xd);
        this->msgStep = 15;
        break;
    case 7:
        if (!this->window) break;
        if (!this->window->state) break;
        *state = 8;
        break;
    case 8:
        if (!this->window) break;
        if (this->window->state) break;
        *state = 3;
        if (this->dropQuery != -1) break;
        this->dropQuery = FieldAction_RequestPlaceAtPendingForAid(this->sessionSlot, this->actionItem);
        if (this->dropQuery == -1) break;
        *state = 5;
        this->startAnim(0, 6, 6);
        if (this->getHeldToolKind() == 4) {
            HeldItemModel_PlayAnim(&this->heldItemModel, 0, 9, 0);
        }
        break;
    }
}

void PlayerActor::setupAct76(PlayerActionRequest *item, u32 old) {
    using namespace nG;
    u8 *p = &((PlayerActionRequest *)item)->unk_0c_b[0];
    u8 a = p[0];
    u8 b = p[1];
    u8 c = p[2];
    PlayerActionWork *s;
    if ((u8)(a + 254) <= 1) TalkRequest_FinishSceneEntry();
    s = (PlayerActionWork *)((PlayerActor *)this)->actionWorkRaw;
    s->unk_02 = a;
    if (a == 0) {
        if (old != 0x57) s->unk_03 = 3;
        else s->unk_03 = 0;
    } else {
        s->unk_03 = 0;
    }
    s->unk_04 = 0;
    s->unk_05 = b;
    s->unk_06 = c;
    this->netData.writeAct76Net(a, b, c);
    this->startAnimOnce(sPlayerAct76Anims[a], 3, 0);
    this->pendingAct76Kind = 0;
    s32 r = this->getHeldToolKind();
    if (r != 3) {
        if (r == 4) HeldItemModel_PlayAnim(&this->heldItemModel, 9, 3, 1);
    } else {
        HeldItemModel_PlayAnim(&this->heldItemModel, 0x1f, 3, 1);
    }
    if (gCommManager->isLocalSlot(this->sessionSlot)) {
        u32 t = 0xd;
        switch (a) {
        case 0:
        case 1:
            t = 6;
            break;
        case 2:
        case 3:
            t = 6;
            Bgm_ReleasePriority(0x12);
            Bgm_RequestSilence(5, 0, 1);
            this->msgStep = 7;
            break;
        default:
            Bgm_RequestSilence(0xc, 0, 1);
            this->msgStep = 7;
            break;
        }
        u16 hv = data_020c61c0[a];
        s->unk_00 = hv;
        Bgm_Request(t, hv, 0x7f, 1);
    }
}

void PlayerActor::act76Update() {
    using namespace nF;
    u8 *p, *st;
    u8 kind, sub;
    s32 lim;
    void *g;
    u16 t[8];
    s16 handRot[3];
    Mtx43 blk;
    VecFx32 v1, v2, v3, v4, v5;
    BOOL r;

    this->turnToCamera(0x400);
    p = &actionWorkRaw[0];
    kind = p[2];
    sub = p[5];
    st = p + 3;
    blk = itemHandMtx;
    switch (kind) {
    case 0:
        if (*st == 3) {
            *st = 0;
            Insect_FinishCatch(p[6]);
            HeldInsect_Start(sub, (u8)sessionSlot);
        }
        if (sub == 9) {
            v1.x = 0x119a;
            v1.y = 0x4cd;
            v1.z = -0x4cd;
        } else {
            v1.x = 0xb33;
            v1.y = 0x19a;
            v1.z = -0x19a;
        }
        PlayerActor_ApplyHoldOffset(&blk, &v1);
        handRot[0] = 0x64;
        handRot[1] = 0x64;
        handRot[2] = 0x64;
        v1 = *(VecFx32 *)&blk.m[9];
        WorldCurve_FromCurved(&v1, &v1);
        HeldInsect_SetHandMatrix((u8)sessionSlot, handRot, &blk, 0);
        break;
    case 1:
        void *o = ((FishBobber *)(&heldItemModel.bobber))->getFish();
        if (o != NULL) {
            v1.x = 0xb33;
            v1.y = 0x19a;
            v1.z = -0x19a;
            PlayerActor_ApplyHoldOffset(&blk, &v1);
            v2 = *(VecFx32 *)&blk.m[9];
            WorldCurve_FromCurved(&v2, &v2);
            Fish_GetDisplayScale(&v3, *((s8 *)o + 0x7e));
            v4 = v2;
            v5 = v3;
            FishCatch_SetDisplayPosScale(o, &v4, &v5);
        }
        break;
    }
    switch (animId) {
    case 0x87:
        if (!((AnimFrameCtrl *)(&(AnimFrameCtrl &)bodyModel))->isFinished()) {
            return;
        }
        this->startAnimOnce(0x88, 0, 0);
        HeldItemModel_PlayAnim(&heldItemModel, 0xa, 0, 1);
        return;
    case 0x89:
        if (!((AnimFrameCtrl *)(&(AnimFrameCtrl &)bodyModel))->isFinished()) {
            return;
        }
        this->startAnimOnce(0x8a, 0, 0);
        HeldItemModel_PlayAnim(&heldItemModel, 0x20, 0, 1);
        return;
    case 0x86:
        lim = 0x19;
        if (gCommManager->isLocalSlot(sessionSlot)) {
            if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)bodyModel))->hasPassedFrame(0xc)) {
                Camera_SetMode4();
            }
        }
        break;
    default:
        lim = 5;
        break;
    }
    if ((s32)(((u32)bodyModel.curFrame << 4) >> 16) < lim) {
        return;
    }
    g = gCommManager;
    if (!((CommManager *)(g))->isLocalSlot(sessionSlot)) {
        actionPriority = ((Unk_02007694 *)(void *)(this))->getActionDonePriority(action);
        return;
    }
    switch (*st) {
    case 0:
        if (TalkRequest_AddPlayerMessage()) {
            *st = 1;
            attachTalkRequest(this);
            this->setActionFlag(0x11);
            switch (kind) {
            case 0:
                setFileName(sPlayerActorGetInsectMsgFile);
                msgIndex = 0x40;
                break;
            case 1:
                setFileName(sPlayerActorGetFishMsgFile);
                msgIndex = 0x40;
                break;
            case 2:
                setFileName(sPlayerActorMsgFile);
                msgIndex = 0x28;
                break;
            case 3:
                setFileName(sPlayerActorMsgFile);
                msgIndex = 0x29;
                break;
            case 7:
                setFileName(sPlayerActorMsgFile);
                msgIndex = 0x22;
                break;
            default:
                setFileName(sPlayerActorMsgFile);
                msgIndex = kind + 0x1a;
                break;
            }
            window->nextState = 1;
        }
    case 1:
        if (window != NULL) {
            if (window->state != 0) {
                *st = 2;
            }
        }
        break;
    case 2:
        if (kind < 2) {
            if (window == NULL) {
                break;
            }
            if (window->nextMsgIndex != 0xfe) {
                break;
            }
            if (kind == 1) {
                PlayerActor_RequestFishShowCatch(this, 1, 6, -1);
            } else {
                PlayerActor_RequestInsectShowCatch(this, 3, sub, sub, 6, -1);
            }
            break;
        }
        if (window == NULL) {
            break;
        }
        if (window->state != 0) {
            break;
        }
        detachTalkRequest(this);
        this->clearActionFlag(0x11);
        TalkRequest_FinishPlayerMessage();
        if (((CommManager *)(g))->isLocalSlot(sessionSlot)) {
            Camera_SetModeDefault();
        }
        actionPriority = ((Unk_02007694 *)(void *)(this))->getActionDonePriority(action);
        PlayerActor_GetHeldItem(&t[3], this);
        if (Item_IsFurniture(&t[3])) {
            t[4] = 0xfff1;
            r = Item_GetFurnitureIndex(&t[3]) == Item_GetFurnitureIndex(&t[4]) ? TRUE : FALSE;
        } else {
            r = t[3] == 0xfff1 ? TRUE : FALSE;
        }
        if (!r && !this->getHeldHoldableIndex() && Unk_02008858_IsZero(gFieldSceneKind)) {
            PlayerActor_RequestStowItem(this, 2, 2, 0, 0, 0, 6, -1);
            break;
        }
        if (Unk_02008858_IsZero(gFieldSceneKind)) {
            TalkRequest_FinishSceneEntry();
            this->setActionFlag(0);
        }
        this->requestWait(3, 1, -1);
        break;
    }
}

void Unk_02005e7c::handleNetEvent() {
    using namespace nC;
    FieldActionFxEvent pkt;
    volatile u16 pos;
    s32 ux, uy;
    s32 ux2, uy2;
    u8 sub;
    Unk_02005f50_Area *pa;
    s32 flag;
    u8 type;
    s32 x, y;
    s32 v34[2], v2122[2], v5[2], v6a[2], v6b[2], v19[2];
    Unk_0200f6d4_V2 v20;
    s32 v14a[2], v14b[2], v14c[2];
    VecFx32 w, q1, q8, q11, q10, q20;

    if (!FieldActionFx_Take(&pkt, sessionSlot) && !pendingEvent.set) {
        return;
    }
    ux = 0;
    uy = 0;
    if (pendingEvent.set) {
        pendingEvent.set = 0;
        x = pendingEventUnitX;
        y = pendingEventUnitZ;
        type = pendingEvent.type;
        sub = pendingEvent.sub;
    } else {
        pos = pkt.pos;
        y = pos;
        x = y >> 8;
        y &= 0xff;
        type = pkt.type;
        sub = pkt.sub;
    }
    FieldPos_FromUnitCenter(&w, x, y);
    switch (type) {
    case 17:
        if (action != 0) {
            break;
        }
        netPickUpDelay = 2;
        
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
    case 14: case 19: case 20: case 21: case 22:
        if (((PlayerActor *)(this))->testActionFlag(10) == 0) {
            pendingEvent.set = 1;
            pendingEventUnitX = x;
            pendingEventUnitZ = y;
            pendingEvent.type = type;
            pendingEvent.sub = sub;
            return;
        }
        ((PlayerActor *)(this))->clearRequests();
        break;
    case 12: case 13: case 15: case 16: case 18: case 23: case 24: case 25: case 26:
        return;
    }
    switch (type) {
    case 1:
    case 2: {
        if (action == 0x68) {
            pa = (Unk_02005f50_Area *)actionWorkRaw;
            s32 t = pa->a[1]; q1.x = pa->a[0]; q1.y = 0; q1.z = t;
            FieldPos_ToUnit(&ux, &uy, &q1);
            if (ux == x && uy == y) {
                pa->e[0] = 1;
                static_cast<AnimFrameCtrl &>(bodyModel).playMode = 1;  // (DS +0x2e0)
                return;
            }
        }
        PlayerActor_RequestTreeShake(this, w.x, w.z, type != 1, 1, 6, -1);
        break;
    }
    case 3:
    case 4:
    case 21:
    case 22:
        if (action == 0x18) {
            u8 *p = actionWorkRaw;
            ux = p[5];
            uy = p[5];
            if (ux == x && uy == y) {
                p[4] = 1;
                p[7] = type;
                return;
            }
        }
        flag = 1;
        switch (type) {
        case 3:
            flag = 0;
        case 4: {
            v34[0] = x; v34[1] = y;
            ((PlayerActor *)(this))->requestPickUpAt((Vec2 *)(v34), -1, flag, 6, -1);
            break;
        }
        case 21:
            flag = 0;
        case 22: {
            v2122[0] = x; v2122[1] = y;
            ((PlayerActor *)(this))->requestPickUpFanfareAt((Unk_02006d14_Pair *)(v2122), flag, 6, -1);
            break;
        }
        }
        break;
    case 5:
        if (action == 0x16) {
            u8 *p = actionWorkRaw;
            ux = p[0];
            uy = p[1];
            if (ux == x && uy == y) {
                p[2] = 1;
                return;
            }
        }
        {
            v5[0] = x; v5[1] = y;
            PlayerActor_RequestPluck(this, v5, 6, -1);
        }
        break;
    case 6:
    case 7:
        if (sub == 2) {
            v6a[0] = x; v6a[1] = y;
            PlayerActor_RequestAxeBreak(this, 1, v6a, 6, -1);
        } else {
            v6b[0] = x; v6b[1] = y;
            PlayerActor_RequestAxeChop(this, sub != 0, type != 6, v6b, 6, -1);
        }
        break;
    case 8: {
        q8 = w;
        PlayerActor_RequestDig(this, 0, &q8, 6, -1);
        break;
    }
    case 11: {
        q11 = w;
        PlayerActor_RequestDig(this, 1, &q11, 6, -1);
        break;
    }
    case 9:
        flag = 0;
    case 10: {
        q10 = w;
        PlayerActor_RequestDigUpItem(this, &q10, flag, 6, -1);
        break;
    }
    case 19: {
        v19[0] = x; v19[1] = y;
        PlayerActor_RequestFillHole(this, 0, v19, 0xfff1, 6, -1);
        break;
    }
    case 20:
        if (action == 0x66) {
            pa = (Unk_02005f50_Area *)actionWorkRaw;
            ux2 = 0;
            uy2 = 0;
            q20.x = pa->a[0]; q20.y = pa->a[1]; q20.z = pa->a[2];
            FieldPos_ToUnit(&ux2, &uy2, &q20);
            if (ux2 == x && uy2 == y) {
                ((PlayerActor *)(this))->setActionFlag(0x1c);
                pa->e[3] = 1;
                return;
            }
        }
        {
            v20.x = x; v20.y = y;
            PlayerActor_RequestAct66(this, &v20, 1, 6, -1);
        }
        break;
    case 14:
        toolHitKind = 0;
        if (sub == 2) {
            v14a[0] = x; v14a[1] = y;
            PlayerActor_RequestAxeBreak(this, 1, v14a, 6, -1);
        } else if (sub == 3) {
            v14b[0] = x; v14b[1] = y;
            PlayerActor_RequestShovelStrike(this, 1, v14b, 6, -1);
        } else {
            v14c[0] = x; v14c[1] = y;
            PlayerActor_RequestAxeStrike(this, sub != 0, 1, v14c, 6, -1);
        }
        break;
    case 17:
        if (action == 0) {
            ((PlayerActor *)(this))->requestWait(3, 1, -1);
            ((PlayerActor *)(this))->clearActionFlag(0x1c);
        }
        break;
    case 0: case 12: case 13: case 15: case 16: case 18: case 23: case 24: case 25: case 26:
        break;
    }
}

void PlayerActor::drawReady() {
    using namespace nB;
    Mtx43 a;
    VecFx32 t;
    VecFx32 t2;
    VecFx32 v;
    data_021cb69c = this->bodyModel.mtx;
    ((Model *)(&this->bodyModel))->setAlpha(this->alpha);
    ((TwoLayerAnimModel *)(&this->bodyModel))->drawLayered(NULL);
    Model_GetJointWorldMtx(&this->bodyModel, &this->headMtx, 0xf);
    Model_GetJointWorldMtx(&this->bodyModel, &this->toolHandMtx, 0xe);
    Model_GetJointWorldMtx(&this->bodyModel, &this->itemHandMtx, 0xb);
    Model_GetJointWorldMtx(&this->bodyModel, &a, 7);
    t.x = a.m[9];
    t.y = a.m[10];
    t.z = a.m[11];
    WorldCurve_FromCurved(&this->footPosA, &t);
    Model_GetJointWorldMtx(&this->bodyModel, &a, 4);
    t.x = a.m[9];
    t.y = a.m[10];
    t.z = a.m[11];
    WorldCurve_FromCurved(&this->footPosB, &t);
    Model_GetJointWorldMtx(&this->bodyModel, &a, 0x10);
    t.x = a.m[9];
    t.y = a.m[10];
    t.z = a.m[11];
    WorldCurve_FromCurved(&this->headTopPos, &t);
    *(Mtx43 *)&this->headModel0.mtx = this->headMtx;
    ((Model *)(&this->headModel0))->setAlpha(this->alpha);
    ((Model *)(&this->headModel0))->drawScaled((s32 *)(0));
    if (PlayerHead_GetModelId(&this->headRef.slot, 1) < 0x9e) {
        *(Mtx43 *)&this->headModel1.mtx = this->headMtx;
        ((Model *)(&this->headModel1))->setAlpha(this->alpha);
        ((Model *)(&this->headModel1))->drawScaled((s32 *)(0));
    }
    if (PlayerGlassesModelRef_GetModelId(&this->faceItemRef.slot) != 0x4b) {
        *(Mtx43 *)&this->faceItemModel.mtx = this->headMtx;
        ((Model *)(&this->faceItemModel))->setAlpha(this->alpha);
        ((Model *)(&this->faceItemModel))->drawScaled((s32 *)(0));
    }
    if (this->shadowSize > 0) {
        Model_GetJointWorldMtx(&this->bodyModel, &a, 1);
        t2.x = a.m[9];
        t2.y = a.m[10];
        t2.z = a.m[11];
        WorldCurve_FromCurved(&v, &t2);
        CharaShadow_DrawPlayer(&v, this->shadowSize);
    }
    if (this->testActionFlag(0)) {
        HeldItemModel_Draw(&this->heldItemModel, &this->toolHandMtx);
        u32 mode;
        switch (this->getHeldToolKind()) {
        case 4:
        case 10:
            mode = 2;
            break;
        case 1:
        case 5:
        case 9:
            mode = 1;
            break;
        default:
            mode = 0;
            break;
        }
        HeldItemModel_GetJointMtx(&this->heldItemJointMtx, &this->heldItemModel, mode);
        if (this->getHeldToolKind() == 4) {
            HeldItemModel_GetJointMtx(&this->heldItemJointMtx2, &this->heldItemModel, 3);
        }
    }
}

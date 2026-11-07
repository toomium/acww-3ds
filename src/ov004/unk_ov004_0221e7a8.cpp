#include "uptr.h"
#include "proto/main/unk_0202fa70.h"
#include "game/CollisionState.h"
// mwcc-version: 1.2/base
// mwcc-flags: -str reuse
#include "types.h"
#include "n3ds_options.h"
#include "room/FtrManager.h"
#include "sys/NNSFndArchive.h"
#include "gfx/VecFx32.h"
#include "actor/CharacterListNode.h"
#include "gfx/AnimFrameCtrl.h"
#include "room/RoomObjRes.h"
#include "room/RoomObjTex.h"
#include "gfx/CachedModel.h"
#include "sys/ProcBase.h"
#include "actor/Actor.h"
#include "actor/Character.h"
#include "gfx/AnimModel.h"
#include "room/RoomObjActor.h"
#include "talk/TalkWindowState.h"
#include "talk/TalkMsgRequest.h"
#include "player/PlayerActionRequest.h"
#include "player/PlayerActor.h"
#include "player/PlayerPickUpReachArgs.h"
#include "actor/ActorCollider.h"
#include "gfx/JointBlend.h"
#include "gfx/Model.h"
#include "gfx/TwoLayerAnimModel.h"
#include "net/CommManager.h"
#include "player/PlayerData.h"
#include "player/Unk_02007694.h"
#include "snd/SndSeEmitter.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02101048.h"
#include "proto/itcm/unk_01ffcfc0.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02004558.h"
#include "proto/main/unk_02034438.h"
#include "proto/main/unk_02051218.h"
#include "proto/main/unk_020514a4.h"
#include "proto/main/unk_020571f4.h"
#include "proto/main/unk_0205b7b0.h"
#include "proto/main/unk_0205dfa4.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_0209c37c.h"
#include "proto/main/unk_0209c3e0.h"
#include "proto/main/unk_0209c4a8.h"
#include "proto/main/unk_020b0e60.h"
#include "proto/ov004/unk_ov004_0221e7a8.h"
#include "proto/ov004/unk_ov004_02229660.h"
#include "proto/ov004/unk_ov004_02233074.h"
#include "proto/main/unk_0208f268.h"
#include "proto/main/unk_020411dc.h"
#include "gfx/ScreenTransition.h"
#include "talk/MsgRequest.h"
#include "proto/main/unk_020b4828.h"
#include "proto/main/unk_0204cc1c.h"

class FtrContact;

extern "C" {
}

extern "C" {
extern const s16 sSeatSideAngles[4] = {0, -0x4000, 0x4000, 0};
u32 data_ov004_0224d4a4 = 0x400;
char sRoomPlayerMsgFile[16] = "obj_etc_player";
u32 data_ov004_0224d4a8 = 0x400;
s32 sFtrPullDist = -0x2000;
s32 sSeatApproachDist = -0x2000;
u32 data_ov004_0224d4b0 = 0x400;
const s32 sAct12Pos[4] = {0x10800, 0x2200, 0x17100, 0x2000};
s32 sRoomInteractDist = 0xccd;
char sRoomErrorMsgFile[16] = "obj_etc_error";
u32 data_ov004_0224d4b4 = 0x400;
}







// ---- model resource sub-object at +0xec (see src/main/unk_02054190.cpp)





// ---- helper objects at +0x1a4, +0x248, +0x250 (their other methods live in ov004_054)



extern "C" {

void NNS_G3dBindMdlTex(void *a, void *b);
void NNS_G3dBindMdlPltt(void *a, void *b);
}


extern "C" {
}

typedef RoomObjActor M;
typedef VecFx32 Vec;


RoomObjActor::RoomObjActor() {
    syncSlot = 0xff;
}

RoomObjActor::~RoomObjActor() {
    RoomObjTex_Destruct(&tex);
}

BOOL RoomObjActor::preCreate() {
    if (Character::preCreate() == 0) {
        return FALSE;
    }
    setCharId(0);
    se.RoomObj_ActivateSe();
    return TRUE;
}

BOOL RoomObjActor::preExecute() {
    if (Character::preExecute() == 0) {
        return FALSE;
    }
    if (syncSlot != 0xff) {
        s32 v = getSyncState();
        if (tex.syncState != v) {
            changeSyncState(v);
        }
    }
    return TRUE;
}

BOOL RoomObjActor::postExecute(u32 a) {
    Vec out;
    getSoundPos(&out);
    se.RoomObj_SetSePos(&out);
    return ((Actor *)(this))->Actor::postExecute(a);
}

BOOL RoomObjActor::preDelete() {
    if (Character::preDelete() == 0) {
        return FALSE;
    }
    se.RoomObj_DeactivateSe();
    return TRUE;
}

void RoomObjActor::getSoundPos(Vec *out) {
    out->x = position.x;
    out->y = position.y;
    out->z = position.z;
}

BOOL RoomObjActor::changeSyncState(u32 v) {
    return TRUE;
}

extern "C" void RoomObj_LoadResources(char *a, char *b, AnimModel *m, RoomObjRes *aa, RoomObjTexHandle *bb) {
    aa->RoomObjRes_Load(a);
    bb->RoomObjTex_Load(b);
    ((Model *)(m))->setResource((NNSG3dResMdl *)(aa->RoomObjRes_GetModel()), 0);
    void *p = aa->RoomObjRes_GetModel();
    NNS_G3dBindMdlTex(p, bb->RoomObjTex_Get());
    void *q = aa->RoomObjRes_GetModel();
    NNS_G3dBindMdlPltt(q, bb->RoomObjTex_Get());
}

extern "C" void RoomObj_LoadResourcesByName(char *name, AnimModel *m, RoomObjRes *a, RoomObjTexHandle *b) {
    char x[0x28];
    char y[0x28];
    Str_SPrintf(x, "/roomObj/%s.arc", name);
    Str_SPrintf(y, "/roomObj/%s.nsbtx", name);
    RoomObj_LoadResources(x, y, m, a, b);
}

void RoomObjActor::loadResources(char *a, char *b) {
    RoomObj_LoadResources(a, b, &model, &res, &tex);
}

void RoomObjActor::loadResourcesByName(char *name) {
    char a[0x28];
    char b[0x28];
    Str_SPrintf(a, "/roomObj/%s.arc", name);
    Str_SPrintf(b, "/roomObj/%s.nsbtx", name);
    loadResources(a, b);
}

extern "C" void RoomObj_ReleaseResources(RoomObjRes *a, RoomObjTexHandle *b) {
    a->RoomObjRes_Free();
    b->RoomObjTex_Reset();
}

void RoomObjActor::releaseResources() {
    RoomObj_ReleaseResources(&res, &tex);
}

void RoomObjActor::setSyncSlot(u32 v) {
    syncSlot = v;
}

s32 RoomObjActor::getSyncState() {
    if (syncSlot != 0xff) {
        return RoomObjSync_GetState(syncSlot);
    }
    return 0;
}

s32 RoomObjActor::storeSyncState(u8 v) {
    if (syncSlot != 0xff) {
        return RoomObjSync_SetState(syncSlot, v);
    }
    return 0;
}

// 02224ee4 is the first function
RoomObjRes::RoomObjRes() {
    clear();
}

RoomObjRes::~RoomObjRes() {
    clear();
}

void RoomObjRes::clear() {
    archive = 0;
    model = 0;
    u32 i;
    for (i = 0; i < 13; i++) {
        bmas[i] = 0;
        bcas[i] = 0;
        btas[i] = 0;
    }
}


namespace ns_0222459c { struct GetOutOfBedCheckWork; }
namespace ns_02223314 { struct BedApproachWork; }
namespace ns_022215a8 { struct PlayerSitDownArgs; }

// the player actions defined in this file (and their helpers elsewhere), as defined
extern "C" {
extern u8 gFieldSceneKind;
s32 PlayerActor_RequestLieInBed(PlayerActor *o, u32 a, u32 b, u32 c);
void PlayerActor_GetOutOfBedCheckSetWork(ns_0222459c::GetOutOfBedCheckWork *r, VecFx32 *v, u32 f);
s32 PlayerActor_RequestBedRollCheck(PlayerActor *o, u32 a, s32 b, s32 c);
s32 PlayerActor_RequestGetOutOfBed(PlayerActor *o, u32 a, u32 b, s32 c, s32 d);
void PlayerActor_BedApproachSetArgs(ns_02223314::BedApproachWork *t, s32 x, s32 z, s16 h, u8 a, u8 b);
s32 PlayerActor_RequestAct12(PlayerActor *o, s32 a, s32 b);
s32 PlayerActor_RequestBedApproach(PlayerActor *o, s32 x, s32 z, s32 h, u8 a, s32 b, s32 c);
void PlayerActor_StepTowardPose(PlayerActor *o, s32 a, s32 b, s32 c);
s32 Item_GetFurnitureIndex(u16 *p);
void PlayerActor_PickUpItemUpdateAnim(PlayerActor *o);
void PlayerActor_PickUpItemUpdateScale(PlayerActor *o);
s32 PlayerActor_RequestFtrHold(PlayerActor *o, u32 a, u32 b, u32 c, s32 e);
s32 PlayerActor_RequestSeatApproach(PlayerActor *o, s32 x, s32 z, s16 h, u8 d, s32 a, s32 b);
void PlayerActor_SitDownFrontSetArgs(ns_022215a8::PlayerSitDownArgs *t, s32 x, s32 z, s16 h);
s32 PlayerActor_RequestSit(PlayerActor *o, u32 a, u32 b, u32 c);
s32 PlayerActor_RequestStandUpSide2(PlayerActor *o, u32 a, u32 b);
s32 PlayerActor_RequestStandUpCheck(PlayerActor *o, u32 a, s32 b, s32 c);
s32 PlayerActor_RequestStorageClose(PlayerActor *o, u32 a, s32 b, s32 c);
s32 PlayerActor_RequestAct42(PlayerActor *o, s32 a, s32 b);
s32 Vec_DistXZ(VecFx32 *a, VecFx32 *b);
void PlayerActor_ApproachValue(s32 *p, s32 target, s32 rate, s32 maxstep, s32 minstep);
s32 PlayerActor_RequestFtrGrabApproach(PlayerActor *o, s32 x, s32 z, s16 h, u32 a, s32 b);
s32 PlayerActor_RequestPickUpItem(PlayerActor *o, Vec2 *p, s32 c, u32 b, s16 d);
void PlayerActor_RoomWalkToward(PlayerActor *o, VecFx32 *tgt, s32 *out, s32 *lim);
}

namespace ns_0222459c {

struct GetOutOfBedCheckWork {
    VecFx32 pos;
    u8 flag;
};

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef PlayerActionRequest Msg;
typedef GetOutOfBedCheckWork Rec;
typedef RoomObjRes Res;

extern "C" {
extern void *gCommManager;

BOOL Scene_InUnk6To8(void);
s32 PlayerActor_GetGender(Obj *o);
s32 TalkRequest_FinishSceneEntry(void);
void PlayerActor_GetHat(u16 *p, Obj *o);
void PlayerActor_GetFaceItem(u16 *p, Obj *o);
s32 PlayerActor_GetHairStyle(Obj *o);
s32 PlayerActor_GetHairColor(Obj *o);
u8 *NNS_G3dGetTex(u8 *p);
void Gfx3d_LoadTexAndPltt(void *p, u32 a);
void *Gfx3d_CopyTex(void *p, void *g);
s32 NNS_FndMountArchive(void *buf, const char *name, void *data);
void NNS_FndUnmountArchive(void *buf);
void *NNS_G3dGetMdlSet(void *p);
void *func_021065dc(void *p);
void *func_021065f8(void *p, u32 a);
void *func_02106618(void *p);
void *func_02106634(void *p, u32 a);
void *func_02106654(void *p);
void *func_02106670(void *p, u32 a);

s32 PlayerActor_RequestDrinkCoffee(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestPhoneHangUp(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestPhonePickUp(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestHaircutStart(Obj *o, u32 a, u32 b, u32 c, s32 e);
s32 PlayerActor_RequestAct37(Obj *o, s32 a, s32 b);
s32 PlayerActor_RequestAct36(Obj *o, u16 *a, s32 b, s32 c, s32 d, Character *e, s32 f, s16 g);
s32 PlayerActor_RequestDoorWalkOut(Obj *o, V3 *v, u32 a, u32 b);
s32 PlayerActor_RequestLeaveRoom(Obj *o, s32 a, s32 b);
s32 PlayerActor_RequestStorageOpen(Obj *o, u8 b, u32 x, u32 y, s16 c, s32 d, s32 e);
s32 PlayerActor_RequestStandUpFront(Obj *o, s32 a, s32 b);
s32 PlayerActor_RequestStandUpSide1(Obj *o, s32 a, s32 b);

s32 PlayerActor_RequestGetOutOfBedCheck(Obj *o, u32 a, u32 b, u32 c);
void PlayerActor_MainLieInBed(Obj *o);
void PlayerActor_LieInBedReadInput(Obj *o);
void PlayerActor_EndLieInBed(Obj *o, s32 a);
void PlayerActor_NetLieInBed(Obj *o, s32 a);
void PlayerActor_SetupLieInBed(Obj *o, u8 *p, s32 c);
s32 PlayerActor_GetPosIfInAction(V3 *out, u32 idx, s32 st);
}

enum Unk_ov004_02224a80_Limit { Unk_ov004_02224a80_LIMIT_6 = 6 };

extern "C" s32 RoomObjRes_Load(Res *self, const char *id) {
    if (self->archive == 0) {
        char buf[0x24];
        NNSFndArchive res;
        self->archive = File_LoadAlloc(id, gBgHeap, 4, 0);
        if (self->archive) {
            if (NNS_FndMountArchive(&res, "RMO", self->archive)) {
                u32 i, z;
                void *h = NNS_FndGetArchiveFileByName("RMO:a/bmd/bmd0");
                if (h) {
                    u8 *r = (u8 *)NNS_G3dGetMdlSet(h);
                    self->model = r + *(u32 *)(r + *(u16 *)(r + 0xe) + 0xc);
                }
                for (i = 0, z = i; i < 13;) {
                    Str_SPrintf(buf, "RMO:a/bca/bca%d", i);
                    { void *h = NNS_FndGetArchiveFileByName(buf); if (h) {
                        self->bcas[i] = func_021065f8(func_021065dc(h), z);
                    } }
                    if (self->bcas[i] == 0) {
                        break;
                    }
                    i++;
                }
                for (i = 0, z = i; i < 13;) {
                    Str_SPrintf(buf, "RMO:a/bma/bma%d", i);
                    { void *h = NNS_FndGetArchiveFileByName(buf); if (h) {
                        self->bmas[i] = func_02106634(func_02106618(h), z);
                    } }
                    if (self->bmas[i] == 0) {
                        break;
                    }
                    i++;
                }
                for (i = 0, z = i; i < 13;) {
                    Str_SPrintf(buf, "RMO:a/bta/bta%d", i);
                    { void *h = NNS_FndGetArchiveFileByName(buf); if (h) {
                        self->btas[i] = func_02106670(func_02106654(h), z);
                    } }
                    if (self->btas[i] == 0) {
                        break;
                    }
                    i++;
                }
                NNS_FndUnmountArchive(&res);
                return 1;
            }
        }
    }
    return 0;
}

extern "C" void RoomObjRes_Free(Res *p) {
    if (p->archive) {
        Heap_Free((Heap *)gBgHeap, p->archive);
        p->archive = 0;
    }
}

extern "C" void *RoomObjRes_GetBca(Res *p, u32 i) {
    if (i < 13) {
        return p->bcas[i];
    }
    return 0;
}

extern "C" void *RoomObjRes_GetBma(Res *p, u32 i) {
    if (i < 13) {
        return p->bmas[i];
    }
    return 0;
}

extern "C" void *RoomObjRes_GetBta(Res *p, u32 i) {
    if (i < 13) {
        return p->btas[i];
    }
    return 0;
}

extern "C" void *RoomObjRes_GetModel(Res *p) {
    return p->model;
}

extern "C" void RoomObjTex_Construct(RoomObjTexHandle *p) {
    p->texture = 0;
}

extern "C" void RoomObjTex_Destruct(RoomObjTexHandle *p) {
}

extern "C" s32 RoomObjTex_Load(RoomObjTexHandle *self, const char *id) {
    void *r4 = File_LoadAlloc(id, gCurrentHeap, -4, 0);
    if (r4) {
        void *r6 = NNS_G3dGetTex((u8 *)r4);
        Gfx3d_LoadTexAndPltt(r6, 0);
        self->texture = Gfx3d_CopyTex(r6, gBgHeap);
        Mem_Free(r4);
        return 1;
    }
    return 0;
}

extern "C" void RoomObjTex_Reset(RoomObjTexHandle *p) {
    p->texture = 0;
}

extern "C" void *RoomObjTex_Get(RoomObjTexHandle *p) {
    return p->texture;
}

extern "C" RoomObjSe *RoomObj_ConstructSe(RoomObjSe *p) {
    new (&p->emitter) SndSeEmitter();
    return p;
}

extern "C" RoomObjSe *RoomObj_DestructSe(RoomObjSe *p) {
    p->emitter.SndSeEmitter::~SndSeEmitter();
    return p;
}

extern "C" void RoomObj_ActivateSe(void *a) {
    ((SndSeEmitter *)a)->callInit();
}

extern "C" void RoomObj_SetSePos(RoomObjSe *a, V3 *p) {
    V3 v;
    v.x = p->x;
    v.y = p->y;
    v.z = p->z;
    a->emitter.callUpdateRelative(&v);
}

extern "C" void RoomObj_DeactivateSe(void *a) {
    ((SndSeEmitter *)a)->callStop();
}

extern "C" void RoomObj_PlaySe(void *a, s32 b) {
    Snd_SeEmitterPlayOneShot((SndSeEmitter *)a, b, 0x7f, 0);
}

extern "C" void RoomObj_PlaySeHeld(void *a, s32 b) {
    Snd_SeEmitterPlayHeld((SndSeEmitter *)a, b, 0x7f, 0);
}

extern "C" s32 PlayerActor_TestLocalFlag0F(void) {
    return PlayerActor_TestSlotFlag(15, 4);
}

extern "C" s32 PlayerActor_TestLocalFlag02(void) {
    return PlayerActor_TestSlotFlag(2, 4);
}

extern "C" s32 PlayerActor_GetPosIfInAction(V3 *out, u32 idx, s32 st) {
    Obj *o = PlayerActor_Get(idx);
    if (o && o->action == st) {
        V3 *pv = (V3 *)&o->position;
        out->x = o->position.x;
        out->y = pv->y;
        out->z = pv->z;
        return 1;
    }
    return 0;
}

extern "C" s32 PlayerActor_GetPosIfSitting(V3 *out, u32 idx) {
    return PlayerActor_GetPosIfInAction(out, idx, 0x28);
}

extern "C" s32 PlayerActor_GetPosIfInBed(V3 *out, u32 idx) {
    return PlayerActor_GetPosIfInAction(out, idx, 8);
}

extern "C" s32 PlayerActor_LocalSetHeadwearHidden(u32 a) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        u16 v[2];
        u16 t[2];
        if (a) {
            if ((o)->testActionFlag(0xc)) {
                PlayerActor_GetHat(&t[0], o);
                v[0] = t[0];
                PlayerActor_GetFaceItem(&t[1], o);
                v[1] = t[1];
                (o)->clearActionFlag(0xc);
            } else {
                return 1;
            }
        } else {
            if (!(o)->testActionFlag(0xc)) {
                v[1] = 0xfff1;
                v[0] = 0xfff1;
                (o)->setActionFlag(0xc);
            } else {
                return 1;
            }
        }
        s32 r4 = PlayerActor_GetHairStyle(o);
        s32 r3 = PlayerActor_GetHairColor(o);
        if ((o)->requestHatChange(v, r4, r3, 1)) {
            (o)->requestFaceItemChange(&v[1]);
        }
        return 1;
    }
    return 0;
}

// e: passed by the callers (FtrBed), not used here
extern "C" s32 PlayerActor_LocalRequestBedApproach(const s32 *a, const s32 *b, const s16 *c, s32 d, s32 e) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        Unk_ov004_02224a80_Limit k = Unk_ov004_02224a80_LIMIT_6;
        if (k <= (o)->getEffectivePriority()) {
            return 0;
        }
        u32 f = Math_AngleToSide(*c - d) == 1 ? 1 : 0;
        return PlayerActor_RequestBedApproach(o, *a, *b, *c, f, k, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestGetOutOfBed(u32 a, u32 b) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        if (PlayerActor_RequestGetOutOfBed(o, a, b, 6, -1)) {
            (o)->loadInputMode();
            return 1;
        }
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestSeatApproach(const s32 *a, const s32 *b, const s16 *c, const u8 *d) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        Unk_ov004_02224a80_Limit k = Unk_ov004_02224a80_LIMIT_6;
        if (k <= (o)->getEffectivePriority()) {
            return 0;
        }
        return PlayerActor_RequestSeatApproach(o, *a, *b, *c + 0x8000, *d, k, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestStandUp(s32 t) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        switch (t) {
        case 0:
            PlayerActor_RequestStandUpFront(o, 6, -1);
            break;
        case 1:
            PlayerActor_RequestStandUpSide1(o, 6, -1);
            break;
        case 2:
            PlayerActor_RequestStandUpSide2(o, 6, -1);
            break;
        }
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestStorageOpen(const s32 *a, const s32 *b, const s32 *c, const s16 *d) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestStorageOpen(o, *a, *b, *c, *d, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestStorageClose(const s32 *p) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestStorageClose(o, *p, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestLeaveRoom(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        if (o->exitMode == 4) {
            V3 v;
            V3 *pv = (V3 *)&o->position;
            v.x = o->position.x;
            v.y = pv->y;
            v.z = pv->z;
            v.z += 0x6000;
            return PlayerActor_RequestDoorWalkOut(o, &v, 6, -1);
        }
        return PlayerActor_RequestLeaveRoom(o, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestAct36(u16 *a, u32 *b, u8 *c, u32 *d, Character *e) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestAct36(o, a, *b, *c, *d, e, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestAct37(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestAct37(o, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestHaircutStart(u8 *a, u8 *b) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestHaircutStart(o, *a, *b, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestPhonePickUp(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestPhonePickUp(o, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestPhoneHangUp(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestPhoneHangUp(o, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestSit(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestSit(o, 10, 6, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestAct12(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        return PlayerActor_RequestAct12(o, 5, -1);
    }
    return 0;
}

extern "C" s32 PlayerActor_LocalRequestDrinkCoffee(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        return PlayerActor_RequestDrinkCoffee(o, 6, -1);
    }
    return 0;
}

extern "C" BOOL PlayerActor_LocalPlayAnim98(void) {
    Obj *o = PlayerActor_Get(4);
    if (o) {
        (o)->startAnimOnce(0x98, 0, 0);
        return TRUE;
    }
    return FALSE;
}

extern "C" void PlayerActor_LieInBedSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestLieInBed(Obj *o, u32 a, u32 b, u32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(8, b, c);
    PlayerActor_LieInBedSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_LieInBedSetWork(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_SetupLieInBed(Obj *o, u8 *p, s32 c) {
    // (the request's payload: at 0xc on the DS, 0x10 on 64-bit hosts - read at 0xc, the padding before it, which the
    // request on the stack left as it was: whether the player wakes up with a yawn depended on stack contents)
    u8 *q = ((PlayerActionRequest *)(void *)p)->unk_0c_b;
    u8 *rec = o->actionWorkRaw;
    if (o->animId != 0x11) {
        if (c == 0xf || c == 0xd || !((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            (o)->startAnim(0x11, 0, 0);
        } else {
            (o)->startAnim(0x11, 3, 0);
        }
    }
    PlayerActor_LieInBedSetWork(rec, *q);
    if (*rec == 0 && ((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        TalkRequest_FinishSceneEntry();
    }
}

extern "C" void PlayerActor_NetLieInBed(Obj *o, s32 a) {
    if (o->action == 8) {
        o->netSeq = a;
    } else {
        PlayerActor_RequestLieInBed(o, 0, 6, a);
    }
}

extern "C" void PlayerActor_EndLieInBed(Obj *o, s32 a) {
    if (Scene_InUnk6To8() && a == 10 && o->actionWorkRaw[0] == 0) {
        if (PlayerActor_GetGender(o)) {
            (o)->playSe(0x4cd);
        } else {
            (o)->playSe(0x4cc);
        }
    }
}

extern "C" void PlayerActor_LieInBedReadInput(Obj *o) {
    s32 r;
    if (!Scene_InUnk6To8()) {
        if ((o)->getInputMagnitude() > 0) {
            s32 t = (o)->getInputSideRelative();
            if (o->lastInputSide != t + 1) {
                o->lastInputSide = t + 1;
                switch (t) {
                case 0:
                    r = FtrMgr_CheckBedStepFront(&o->position, o->rotY);
                    break;
                case 1:
                    r = FtrMgr_CheckBedStepBack(&o->position, o->rotY);
                    break;
                }
                if (r == 1) {
                    PlayerActor_RequestBedRollCheck(o, t == 0 ? 1 : 0, 6, -1);
                } else if (r == 2) {
                    PlayerActor_RequestGetOutOfBedCheck(o, t == 0 ? 1 : 0, 6, -1);
                }
            }
        }
    }
}

extern "C" void PlayerActor_MainLieInBed(Obj *o) {
    (o)->netFollowTransform();
    (o)->advanceAnim();
    (o)->submitSceneCollider();
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_LieInBedReadInput(o);
    }
}

extern "C" void PlayerActor_GetOutOfBedCheckSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestGetOutOfBedCheck(Obj *o, u32 a, u32 b, u32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(9, b, c);
    PlayerActor_GetOutOfBedCheckSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_GetOutOfBedCheckSetWork(Rec *r, V3 *v, u32 f) {
    r->pos.x = v->x;
    r->pos.y = v->y;
    r->pos.z = v->z;
    r->flag = f;
}


}  // namespace ns_0222459c

namespace ns_02223c38 {

struct GetOutOfBedWork {
    s32 x;
    s32 z;
    s16 h;
    u8 afterSleep;
    u8 unk_0b;
    u8 getOutSide;
};

struct Unk_ov004_02223c38_Bits16 {
    u16 lo : 7;
    u16 mid : 4;
    u16 hi : 5;
};

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef VecFx32CtorDtor V3c;
typedef GetOutOfBedWork Rec;
typedef PlayerActionRequest Msg;
typedef Unk_ov004_02223c38_Bits16 Bits16;

extern "C" {
extern void *gCommManager;
extern u8 gFieldSceneKind;
extern s16 data_02135f44[];

Bits16 *PlayerSession_GetLastPlayDate(void);
void *PlayerActor_GetPlayerData(void *p);
u32 PlayerActor_GetTan(Obj *o);
s32 PlayerActor_CompareLastPlayDateNow(Obj *o);
s32 func_01ffcb0c(s32 a, s32 b);


void PlayerActor_MainBedRoll(Obj *o);
void PlayerActor_BedRollCheckEnd(Obj *o);
void PlayerActor_EndBedRoll(Obj *o);
void PlayerActor_NetBedRoll(Obj *o, s32 a);
void PlayerActor_SetupBedRoll(Obj *o, Msg *m);
s32 PlayerActor_RequestBedRoll(Obj *o, u32 a, s32 b, s32 c);
void PlayerActor_MainBedRollBlocked(Obj *o);
void PlayerActor_BedRollBlockedCheckEnd(Obj *o);
void PlayerActor_NetBedRollBlocked(Obj *o, s32 a);
void PlayerActor_SetupBedRollBlocked(Obj *o, Msg *m);
s32 PlayerActor_RequestBedRollBlocked(Obj *o, u32 a, s32 b, s32 c);
void PlayerActor_MainBedRollCheck(Obj *o);
void PlayerActor_BedRollCheckResult(Obj *o);
void PlayerActor_NetBedRollCheck(Obj *o, s32 a);
void PlayerActor_SetupBedRollCheck(Obj *o, Msg *m);
void PlayerActor_MainGetOutOfBed(Obj *o);
void PlayerActor_GetOutOfBedCheckEnd(Obj *o);
void PlayerActor_GetOutOfBedUpdateHeight(Obj *o);
void PlayerActor_GetOutOfBedUpdateAnim(Obj *o);
void PlayerActor_EndGetOutOfBed(Obj *o);
void PlayerActor_NetGetOutOfBed(Obj *o, s32 a);
void PlayerActor_SetupGetOutOfBed(Obj *o, Msg *m);
void PlayerActor_GetOutOfBedSetWork(Rec *r, s32 x, s32 z, s16 h, u8 a);
void PlayerActor_MainGetOutOfBedCheck(Obj *o);
void PlayerActor_GetOutOfBedCheckResult(Obj *o);
void PlayerActor_GetOutOfBedCheckTurn(Obj *o);
void PlayerActor_SetupGetOutOfBedCheck(Obj *o, Msg *m);
}

static inline BOOL Unk_ov004_02224284_IsOne(u8 v) {
    return v == 1 ? TRUE : FALSE;
}

extern "C" void PlayerActor_SetupGetOutOfBedCheck(Obj *o, Msg *m) {
    u8 c = m->unk_0c_b[0];
    s32 ang = o->rotY;
    // the get-out-of-bed check's work (this setup belongs to that action)
    ns_0222459c::GetOutOfBedCheckWork *r = (ns_0222459c::GetOutOfBedCheckWork *)&o->actionWork;
    V3 v;
    V3 w;
    {
        V3 *pp = (V3 *)&o->position;
        v.x = o->position.x;
        v.y = pp->y;
        v.z = pp->z;
    }
    s32 k = 0x2000;
    if (c == 0) {
        k *= -1;
    }
    s32 t[2];
    t[0] = data_02135f44[((u16)ang >> 4) * 2];
    t[1] = data_02135f44[((u16)ang >> 4) * 2 + 1];
    s32 dz = func_01ffcb0c(t[1], 0x1000);
    dz -= func_01ffcb0c(t[0], k);
    s32 dx = func_01ffcb0c(t[0], 0x1000);
    dx += func_01ffcb0c(t[1], k);
    V3 *p2 = (V3 *)&o->position;
    v.x = p2->x + dx;
    v.z = p2->z + dz;
    w.x = v.x;
    w.y = v.y;
    w.z = v.z;
    PlayerActor_GetOutOfBedCheckSetWork(r, &w, c);
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        BedSpot_RequestGetOut((s32 *)&v);
    }
}

extern "C" void PlayerActor_NetGetOutOfBedCheck(void) {
}

extern "C" void PlayerActor_GetOutOfBedCheckTurn(Obj *o) {
    V3 v;
    V3 *s = (V3 *)&o->actionWork;
    v.x = s->x;
    v.y = s->y;
    v.z = s->z;
    (o)->setSubCollider(&v, 0x1000, 0x1000);
    ((ActorCollider *)(&o->subCollider))->submit();
}

extern "C" void PlayerActor_GetOutOfBedCheckResult(Obj *o) {
    switch (BedSpot_GetGetOutResult()) {
    case 0:
        return;
    case 1:
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        break;
    case 2:
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        PlayerActor_RequestLieInBed(o, 0, 6, -1);
        return;
    }
    Rec *r = (Rec *)&o->actionWork;
    if (o->subCollider.isHit != 0) {
        PlayerActor_RequestLieInBed(o, 0, 6, -1);
    } else {
        PlayerActor_RequestGetOutOfBed(o, r->getOutSide, 0, 6, -1);
    }
}

extern "C" void PlayerActor_MainGetOutOfBedCheck(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_GetOutOfBedCheckResult(o);
    PlayerActor_GetOutOfBedCheckTurn(o);
}

extern "C" void PlayerActor_GetOutOfBedSetArgs(u8 *p, u32 a, u32 b) {
    p[0] = a;
    p[1] = b;
}

extern "C" s32 PlayerActor_RequestGetOutOfBed(Obj *o, u32 a, u32 b, s32 c, s32 d) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xa, c, *(s16 *)&d);
    PlayerActor_GetOutOfBedSetArgs(m.unk_0c_b, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_GetOutOfBedSetWork(Rec *r, s32 x, s32 z, s16 h, u8 a) {
    r->x = x;
    r->z = z;
    r->h = h;
    r->afterSleep = a;
}

extern "C" void PlayerActor_GetOutOfBedSetNetData(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_GetOutOfBedGetNetData(u8 *p, u8 *out) {
    *out = *p;
}

extern "C" void PlayerActor_SetupGetOutOfBed(Obj *o, Msg *m) {
    u8 *q = m->unk_0c_b;
    u8 c = m->unk_0c_b[0];
    if (Unk_ov004_02224284_IsOne(gFieldSceneKind)) {
        FtrMgr_PlaySeatSound2At((V3 *)&o->position);
    }
    (o)->startAnimOnce(c ? 0xa : 9, 3, 0);
    s32 ang = o->rotY;
    Rec *r = (Rec *)&o->actionWork;
    s32 k = 0x2000;
    if (c == 0) {
        k *= -1;
    }
    s32 t[2];
    t[0] = data_02135f44[((u16)ang >> 4) * 2];
    t[1] = data_02135f44[((u16)ang >> 4) * 2 + 1];
    s32 dz = func_01ffcb0c(t[1], 0x1000);
    dz -= func_01ffcb0c(t[0], k);
    s32 dx = func_01ffcb0c(t[0], 0x1000);
    dx += func_01ffcb0c(t[1], k);
    V3 *p = (V3 *)&o->position;
    s32 x = p->x + dx;
    s32 z = p->z + dz;
    s16 h;
    if (c != 0) {
        h = ang + 0x4000;
    } else {
        h = ang - 0x4000;
    }
    PlayerActor_GetOutOfBedSetWork(r, x, z, h, q[1]);
    PlayerActor_GetOutOfBedSetNetData(o->netData.bytes, c);
}

extern "C" void PlayerActor_NetGetOutOfBed(Obj *o, s32 a) {
    u8 b;
    PlayerActor_GetOutOfBedGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestGetOutOfBed(o, b, 0, 6, a);
}

extern "C" void PlayerActor_EndGetOutOfBed(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->z;
    (o)->setAngleY(&r->h);
    o->prevPosition.x = o->position.x;
    o->prevPosition.y = o->position.y;
    o->prevPosition.z = o->position.z;
}

extern "C" void PlayerActor_GetOutOfBedUpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x15)) {
        (o)->playSe(0x4c6);
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            BedSpot_Release();
        }
    }
}

extern "C" void PlayerActor_GetOutOfBedUpdateHeight(Obj *o) {
    V3 v;
    Rec *r = (Rec *)&o->actionWork;
    v.x = r->x;
    v.y = Ground_GetDefaultY();
    v.z = r->z;
    (o)->updateCollidersAtDrawPos((u32 *)(void *)(&v));
}

extern "C" void PlayerActor_GetOutOfBedCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        if (((Rec *)&o->actionWork)->afterSleep != 0) {
            volatile u16 t[2];
            (o)->requestAct10(0, 5, -1);
            *PlayerSession_GetTanTimer() = 0x4650;
            t[0] = static_cast<PlayerData *>(PlayerActor_GetPlayerData(o))->getLastPlayDate();
            t[1] = t[0];
            *(u16 *)PlayerSession_GetLastPlayDate() = t[1];
            s32 r5 = PlayerActor_CompareLastPlayDateNow(o);
            u16 *r7 = DebugVar_GetPtr(0, 0x50);
            u32 r4 = PlayerActor_GetTan(o);
            Bits16 *r6 = PlayerSession_GetLastPlayDate();
            *r7 = r4 + r6->mid * 1000 + PlayerSession_GetLastPlayDate()->hi * 10;
            if (r5 == 0) {
                *PlayerSession_GetSessionFlags() = 0x11;
            } else {
                *PlayerSession_GetSessionFlags() = 0;
            }
        } else {
            (o)->requestWait(0, 1, -1);
        }
    }
}

extern "C" void PlayerActor_MainGetOutOfBed(Obj *o) {
    PlayerActor_GetOutOfBedUpdateAnim(o);
    PlayerActor_GetOutOfBedUpdateHeight(o);
    PlayerActor_GetOutOfBedCheckEnd(o);
}

extern "C" void PlayerActor_BedRollCheckSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestBedRollCheck(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xb, b, c);
    PlayerActor_BedRollCheckSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_BedRollCheckSetWork(u8 *p, u32 v) {
    p[0] = 0;
    p[1] = v;
}

extern "C" void PlayerActor_BedRollCheckSetNetData(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_BedRollCheckGetNetData(u8 *p, u8 *out) {
    *out = *p;
}

extern "C" void PlayerActor_SetupBedRollCheck(Obj *o, Msg *m) {
    u8 c = m->unk_0c_b[0];
    s32 k;
    s32 d;
    s16 ang;
    V3 w;
    V3 v;
    PlayerActor_BedRollCheckSetWork((u8 *)&o->actionWork, c);
    PlayerActor_BedRollCheckSetNetData(o->netData.bytes, c);
    if (c != 0) {
        k = 0xc;
    } else {
        k = 0xf;
    }
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        if (c == 0) {
            d = -0x2000;
        } else {
            d = 0x2000;
        }
        ang = o->rotY + 0x4000;
        PlayerActor_OffsetByAngle(&v, o, &o->position, &ang, &d);
        w.x = v.x;
        w.y = v.y;
        w.z = v.z;
        BedSpot_RequestRoll((s32 *)&w);
        (o)->startAnim(k, 3, 0);
    } else {
        (o)->startAnim(k, 3, 0);
    }
}

extern "C" void PlayerActor_NetBedRollCheck(Obj *o, s32 a) {
    u8 b;
    PlayerActor_BedRollCheckGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestBedRollCheck(o, b, 6, a);
}

extern "C" void PlayerActor_BedRollCheckResult(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        u8 *p = (u8 *)&o->actionWork;
        if (p[0] < 3) {
            p[0] = p[0] + 1;
        }
        switch (BedSpot_GetRollResult()) {
        case 0:
            break;
        case 1:
            if (p[0] >= 3) {
                PlayerActor_RequestBedRoll(o, p[1], 6, -1);
            }
            break;
        case 2:
            if (p[0] >= 3) {
                PlayerActor_RequestBedRollBlocked(o, p[1], 6, -1);
            }
            break;
        }
    }
}

extern "C" void PlayerActor_MainBedRollCheck(Obj *o) {
    (o)->netFollowTransform();
    (o)->advanceAnim();
    PlayerActor_BedRollCheckResult(o);
}

extern "C" void PlayerActor_BedRollBlockedSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestBedRollBlocked(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xc, b, c);
    PlayerActor_BedRollBlockedSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_BedRollBlockedSetNetData(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_BedRollBlockedGetNetData(u8 *p, u8 *out) {
    *out = *p;
}

extern "C" void PlayerActor_SetupBedRollBlocked(Obj *o, Msg *m) {
    u8 c = m->unk_0c_b[0];
    PlayerActor_BedRollBlockedSetNetData(o->netData.bytes, c);
    s32 k;
    if (c != 0) {
        k = 0xd;
    } else {
        k = 0x10;
    }
    (o)->startAnimOnce(k, 3, 0);
    (o)->playSe(0x4a4);
}

extern "C" void PlayerActor_NetBedRollBlocked(Obj *o, s32 a) {
    u8 b;
    PlayerActor_BedRollBlockedGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestBedRollBlocked(o, b, 6, a);
}

extern "C" void PlayerActor_BedRollBlockedCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestLieInBed(o, 0, 6, -1);
    }
}

extern "C" void PlayerActor_MainBedRollBlocked(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_BedRollBlockedCheckEnd(o);
}

extern "C" void PlayerActor_BedRollSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestBedRoll(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xd, b, c);
    PlayerActor_BedRollSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_BedRollSetWork(s32 *p, s32 x, s32 z) {
    p[0] = x;
    p[1] = z;
}

extern "C" void PlayerActor_BedRollSetNetData(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_BedRollGetNetData(u8 *p, u8 *out) {
    *out = *p;
}

extern "C" void PlayerActor_SetupBedRoll(Obj *o, Msg *m) {
    u8 c = m->unk_0c_b[0];
    s32 k;
    s32 d;
    s16 ang;
    V3c w;
    V3 v;
    if (c != 0) {
        k = 0xb;
    } else {
        k = 0xe;
    }
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        (o)->startAnimOnce(k, 3, 0);
    } else {
        (o)->startAnimOnce(k, 0, 0);
    }
    Rec *r = (Rec *)&o->actionWork;
    if (c == 0) {
        d = -0x2000;
    } else {
        d = 0x2000;
    }
    ang = o->rotY + 0x4000;
    PlayerActor_OffsetByAngle(&v, o, &o->position, &ang, &d);
    s32 tx = v.x;
    w.x = tx;
    w.y = v.y;
    s32 tz = v.z;
    w.z = tz;
    PlayerActor_BedRollSetWork((s32 *)r, tx, tz);
    PlayerActor_BedRollSetNetData(o->netData.bytes, c);
    (o)->playSe(0x435);
}

extern "C" void PlayerActor_NetBedRoll(Obj *o, s32 a) {
    u8 b;
    PlayerActor_BedRollGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestBedRoll(o, b, 6, a);
}

extern "C" void PlayerActor_EndBedRoll(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->z;
}

extern "C" void PlayerActor_BedRollCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        if (PlayerActor_RequestLieInBed(o, 0, 6, -1)) {
            o->lastInputSide = 0;
        }
    }
}

extern "C" void PlayerActor_MainBedRoll(Obj *o) {
    (o)->netFollowTransform();
    (o)->advanceAnim();
    PlayerActor_BedRollCheckEnd(o);
}



}  // namespace ns_02223c38

namespace ns_02223314 {

struct BedApproachWork {
    s32 targetX;
    s32 targetZ;
    s16 targetAngle;
    u8 unk_0a;
    u8 unk_0b;
};

struct PickUpItemWork {
    s32 ftrIndex;
    u8 answer;
    u8 step;
};

struct Unk_ov004_02223314_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

struct Unk_ov004_02223314_Pl {
    u8 a;
    u8 b;
    u8 pad_02[2];
    s32 c;
    s16 d;
    u8 e;
    u8 f;
};

typedef PlayerActor Obj;
typedef Unk_ov004_02223314_Bits Bits;
typedef VecFx32 V3;
typedef BedApproachWork Rec;
typedef PickUpItemWork Rec2;
typedef PlayerActionRequest Msg;
typedef Unk_ov004_02223314_Pl Pl;

extern "C" {
extern u8 gFieldSceneKind;
extern void *gCommManager;
extern BlockMap *gSceneBlockMap;

void PlayerActor_StepTowardXZ(Obj *o, s32 a, s32 b);
s32 Camera_SnapToFocus(V3 *v);
s32 Scene_InHouseRoom(void);
s32 Item_IsFurniture(void *p);
void PendingUnit_ApplyAt(void *p, s32 a);


void PlayerActor_SetupPickUpItem(Obj *o, Msg *m);
void PlayerActor_MainAct12(Obj *o);
void PlayerActor_Act12Move(Obj *o);
void PlayerActor_SetupAct12(Obj *o);
void PlayerActor_MainGetIntoBed(Obj *o);
void PlayerActor_GetIntoBedCheckEnd(Obj *o);
void PlayerActor_GetIntoBedUpdateAnim(Obj *o);
void PlayerActor_GetIntoBedSlide(Obj *o);
void PlayerActor_EndGetIntoBed(Obj *o);
void PlayerActor_NetGetIntoBed(Obj *o, u32 a);
void PlayerActor_SetupGetIntoBed(Obj *o, Msg *m);
void PlayerActor_GetIntoBedSetWork(Rec *r, s32 a, s32 b, s32 c, u8 d);
s32 PlayerActor_RequestGetIntoBed(Obj *o, u8 b, s32 x, s32 z, u8 e, s32 f, s32 g);
void PlayerActor_MainBedApproach(Obj *o);
void PlayerActor_BedApproachCheckArrive(Obj *o);
void PlayerActor_BedApproachMove(Obj *o);
void PlayerActor_EndBedApproach(Obj *o);
void PlayerActor_NetBedApproach(Obj *o, u32 a);
void PlayerActor_SetupBedApproach(Obj *o, Msg *m);
void PlayerActor_BedApproachSetWork(Rec *r, s32 a, s32 b, s16 c, u8 d, u8 e);
}

static inline BOOL Unk_ov004_02223314_IsOne(u8 v) {
    return v == 1 ? TRUE : FALSE;
}

extern "C" void PlayerActor_BedApproachSetArgs(Rec *t, s32 x, s32 z, s16 h, u8 a, u8 b) {
    t->targetX = x;
    t->targetZ = z;
    t->targetAngle = h;
    t->unk_0a = a;
    t->unk_0b = b;
}

extern "C" s32 PlayerActor_RequestBedApproach(Obj *o, s32 x, s32 z, s32 h, u8 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xe, b, *(s16 *)&c);
    PlayerActor_BedApproachSetArgs((Rec *)m.unk_0c_b, x, z, h, a, b == 6 ? 1 : 0);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_BedApproachSetWork(Rec *r, s32 a, s32 b, s16 c, u8 d, u8 e) {
    r->targetX = a;
    r->targetZ = b;
    r->targetAngle = c;
    r->unk_0a = d;
    r->unk_0b = e;
}

extern "C" void PlayerActor_BedApproachSetNetData(u8 *dst, s32 x, s32 z, s32 h, u8 b) {
    NetBuf_PackPair20(dst, x, z);
    NetBuf_WriteS16B(dst + 5, h);
    dst[7] = b;
}

extern "C" void PlayerActor_BedApproachGetNetData(u8 *src, s32 *x, s32 *z, s16 *h, u8 *b) {
    NetBuf_UnpackPair20(src, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B(src + 5);
    *b = src[7];
}

extern "C" void PlayerActor_SetupBedApproach(Obj *o, Msg *m) {
    Pl *pl = (Pl *)m->unk_0c_b;
    V3 v;
    s16 h;
    V3 tmp;
    void *g;
    u8 r6;
    Rec *r7;
    s32 c = pl->c;
    v.x = *(s32 *)pl;
    v.y = 0;
    v.z = c;
    h = pl->d;
    r6 = pl->e;
    r7 = (Rec *)&o->actionWork;
    g = gCommManager;
    if (((CommManager *)(g))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_OffsetByAngle(&tmp, o, &v, &h, (s32 *)&sAct12Pos[3]);
        v.x = tmp.x;
        v.y = tmp.y;
        v.z = tmp.z;
    }
    PlayerActor_BedApproachSetWork(r7, v.x, v.z, h, r6, pl->f);
    PlayerActor_BedApproachSetNetData(o->netData.bytes, v.x, v.z, h, r6);
    if (((CommManager *)(g))->isLocalSlot(o->sessionSlot)) {
        BedSpot_RequestApproach((s32 *)&v);
    }
    (o)->startAnim(0x1a, 3, 0);
}

extern "C" void PlayerActor_NetBedApproach(Obj *o, u32 a) {
    struct L {
        u8 b;
        s16 h;
    } l;
    s32 x, z;
    L *q = &l;
    PlayerActor_BedApproachGetNetData(o->netData.bytes, &x, &z, &l.h, &l.b);
    PlayerActor_RequestBedApproach(o, x, z, q->h, q->b, 6, a);
}

extern "C" void PlayerActor_EndBedApproach(Obj *o) {
    (o)->setAngleY((s16 *)&o->actionWorkRaw[8]); // BedApproachWork::targetAngle
}

extern "C" void PlayerActor_BedApproachMove(Obj *o) {
    PlayerActor_StepTowardPose(o, o->position.x, o->position.z, ((Rec *)&o->actionWork)->targetAngle);
}

extern "C" void PlayerActor_BedApproachCheckArrive(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
    } else {
        switch (BedSpot_GetApproachResult()) {
        case 0:
            break;
        case 2:
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
            (o)->requestWait(3, 1, -1);
            break;
        case 1:
        default:
            if (r->targetAngle == o->rotY) {
                PlayerActor_RequestGetIntoBed(o, r->unk_0a, r->targetX, r->targetZ, r->unk_0b, 6, -1);
            }
            break;
        }
    }
}

extern "C" void PlayerActor_MainBedApproach(Obj *o) {
    PlayerActor_BedApproachMove(o);
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_BedApproachCheckArrive(o);
}

extern "C" void PlayerActor_GetIntoBedSetArgs(void *out, u8 b, s32 x, s32 z, u8 e) {
    u8 *p = (u8 *)out;
    p[0] = b;
    *(s32 *)(p + 4) = x;
    *(s32 *)(p + 8) = z;
    p[0xc] = e;
}

extern "C" s32 PlayerActor_RequestGetIntoBed(Obj *o, u8 b, s32 x, s32 z, u8 e, s32 f, s32 g) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0xf, f, *(s16 *)&g);
    PlayerActor_GetIntoBedSetArgs((Pl *)m.unk_0c_b, b, x, z, e);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_GetIntoBedSetWork(Rec *r, s32 a, s32 b, s32 c, u8 d) {
    r->targetX = a;
    r->targetZ = b;
    r->targetAngle = c;
    r->unk_0a = d;
}

extern "C" void PlayerActor_GetIntoBedSetNetData(u8 *dst, u32 k, s32 a, s32 b) {
    dst[0] = k;
    NetBuf_PackPair20(dst + 1, a, b);
}

extern "C" void PlayerActor_GetIntoBedGetNetData(u8 *src, u8 *out, s32 *x, s32 *z) {
    *out = src[0];
    NetBuf_UnpackPair20(src + 1, (u32 *)x, (u32 *)z);
}

extern "C" void PlayerActor_SetupGetIntoBed(Obj *o, Msg *m) {
    Pl *pl = (Pl *)m->unk_0c_b;
    u8 k = pl->a;
    Rec *r;
    s32 t, h, x, z;
    (o)->startAnimOnce(k == 0 ? 8 : 7, 3, 0);
    (o)->playSe(0x4c5);
    r = (Rec *)&o->actionWork;
    t = Math_AngleToDir4(o->rotY);
    t = (t << 30) >> 16;
    x = pl->c;
    z = *(s32 *)&pl->d;  // (d, e and f as one word)
    if (k == 0) {
        h = (s16)(t + 0x4000);
    } else {
        h = (s16)(t - 0x4000);
    }
    PlayerActor_GetIntoBedSetWork(r, x, z, h, ((u8 *)pl)[0xc]);
    PlayerActor_GetIntoBedSetNetData(o->netData.bytes, k, x, z);
}

extern "C" void PlayerActor_NetGetIntoBed(Obj *o, u32 a) {
    if (o->action == 0xf) {
        o->netSeq = a;
    } else {
        u8 b;
        s32 x, z;
        PlayerActor_GetIntoBedGetNetData(o->netData.bytes, &b, &x, &z);
        PlayerActor_RequestGetIntoBed(o, b, x, z, 0, 6, a);
    }
}

extern "C" void PlayerActor_EndGetIntoBed(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->targetX;
    p->z = r->targetZ;
    (o)->setAngleY(&r->targetAngle);
}

extern "C" void PlayerActor_GetIntoBedSlide(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    s32 *p;
    s32 v;
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        p = &o->position.x;
        if ((r->targetAngle & 0x4000) != 0) {
            v = r->targetX;
        } else {
            p = &o->position.z;
            v = r->targetZ;
        }
        PlayerActor_ApproachValue(p, v, 0x399, 0x1ec, 0x31);
    } else {
        (o)->netFollowTransform();
    }
}

extern "C" void PlayerActor_GetIntoBedUpdateAnim(Obj *o) {
    V3 v;
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xd)) {
        if (Unk_ov004_02223314_IsOne(gFieldSceneKind)) {
            Rec *r = (Rec *)&o->actionWork;
            s32 z = r->targetZ;
            s32 y = o->position.y;
            s32 x = r->targetX;
            v.x = x;
            v.y = y;
            v.z = z;
            FtrMgr_PlaySeatSound1At(&v);
        }
    }
}

extern "C" void PlayerActor_GetIntoBedCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestLieInBed(o, ((Rec *)&o->actionWork)->unk_0a, 6, -1);
    }
}

extern "C" void PlayerActor_MainGetIntoBed(Obj *o) {
    PlayerActor_GetIntoBedSlide(o);
    PlayerActor_GetIntoBedUpdateAnim(o);
    (o)->updateBodyCollider();
    PlayerActor_GetIntoBedCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestAct12(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x12, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct12(Obj *o) {
    V3 v;
    (o)->startAnim(0x9b, 3, 0);
    v.x = sAct12Pos[0];
    v.y = sAct12Pos[1];
    v.z = sAct12Pos[2];
    Camera_SnapToFocus(&v);
}

extern "C" void PlayerActor_NetAct12(void) {
}

extern "C" void PlayerActor_Act12Move(Obj *o) {
    (o)->turnAwayFromCamera(0x400);
    PlayerActor_StepTowardXZ(o, sAct12Pos[0], sAct12Pos[2]);
}

extern "C" void PlayerActor_MainAct12(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_Act12Move(o);
    (o)->updateBodyCollider();
}

extern "C" void PlayerActor_PickUpItemSetArgs(void *out, s32 *in, s32 c) {
    ((u8 *)out)[0] = in[0];
    ((u8 *)out)[1] = in[1];
    *(s32 *)((u8 *)out + 4) = c;
}

extern "C" s32 PlayerActor_RequestPickUpItem(Obj *o, Vec2 *p, s32 c, u32 b, s16 d) {
    Msg m;
    struct {
        s32 x;
        s32 z;
    } v;
    ((PlayerActionRequest *)(&m))->assign(0x1c, b, d);
    v.x = p->x;
    v.z = p->y;
    PlayerActor_PickUpItemSetArgs((Pl *)m.unk_0c_b, &v.x, c);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupPickUpItem(Obj *o, Msg *m) {
    Pl *pl = (Pl *)m->unk_0c_b;
    s32 b = pl->c;
    Rec2 *r = (Rec2 *)&o->actionWork;
    u8 a;
    u32 c;
    u16 *p;
    r->answer = 0;
    r->step = 0;
    r->ftrIndex = b;
    a = pl->a;
    c = pl->b;
    FieldPos_FromUnitCenter(&o->shownItemPos, a, c);
    o->shownItemPos.y = FtrMgr_GetSurfaceHeightAtPos(&o->shownItemPos);
    o->shownItemScaleX = 0x1000;
    o->shownItemScaleY = 0x1000;
    o->shownItemScaleZ = 0x1000;
    if (b < 0) {
        p = BlockMap_GetItemPtrAtPos(gSceneBlockMap, &o->shownItemPos, 1);
        if (p == 0) {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
            (o)->requestWait(3, 1, -1);
            return;
        }
        o->shownItem = *p;
        o->actionItem = o->shownItem;
    }
    if (Pocket_FindEmpty() == -1 && Scene_InHouseRoom()) {
        BOOL ok;
        if (Item_IsFurniture(&o->actionItem)) {
            u16 t = 0xfff1;
            s32 x = Item_GetFurnitureIndex(&o->actionItem);
            if (x == Item_GetFurnitureIndex(&t)) {
                ok = TRUE;
            } else {
                ok = FALSE;
            }
        } else {
            if (o->actionItem == 0xfff1) {
                ok = TRUE;
            } else {
                ok = FALSE;
            }
        }
        if (!ok) {
            (o)->startAnim(0, 3, 0);
            r->step = 6;
            o->shownItem = 0xfff1;
            o->actionItem = o->shownItem;
            return;
        }
    }
    if (Room_CountOccupants() <= 1) {
        if (b < 0) {
            struct {
                s32 a;
                s32 c;
            } v;
            (o)->setActionFlag(0xd);
            v.a = a;
            v.c = c;
            PendingUnit_ApplyAt(&v, 1);
        } else {
            FtrMgr_RemoveActor(b, &o->actionItem, &o->shownItem);
        }
        (o)->startAnimOnce(0x19, 3, 0);
        (o)->playSe(0x52);
    } else {
        r->step = 10;
        (o)->startAnim(0, 3, 0);
    }
}

extern "C" void PlayerActor_NetPickUpItem(void) {
}

extern "C" void PlayerActor_PickUpItemUpdateAnim(Obj *o) {
    Rec2 *r = (Rec2 *)&o->actionWork;
    if (r->step >= 6) {
        (o)->advanceAnim();
        return;
    }
    if (r->answer == 2) {
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(7)) {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
            (o)->requestWait(3, 1, -1);
            return;
        }
    } else if (r->answer == 0) {
        if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid < 7) {
            if (o->fieldAnswer == 2) {
                r->answer = 2;
            } else if (o->fieldAnswer == 1) {
                r->answer = 1;
                (o)->setActionFlag(0xd);
            }
        }
    }
    (o)->advanceAnim();
}

extern "C" void PlayerActor_PickUpItemUpdateScale(Obj *o) {
    s32 *p;
    s32 r;
    if ((o)->testActionFlag(0xd) == 0) {
        p = &o->actionWork;
        if (*p < 0) return;
        if (FtrMgr_PollRemovedPos(*p) == 0) return;
        (o)->setActionFlag(0xd);
        *p = -1;
    }
    if (o->animId != 0x19) {
        o->shownItemScaleX = 0;
        o->shownItemScaleY = 0;
        o->shownItemScaleZ = 0;
        return;
    }
    if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid < 8) return;
    r = 0x1000 - (s32)((((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid - 8) << 12) / 7;
    if (r < 0) {
        r = 0;
        (o)->clearActionFlag(0xd);
    }
    o->shownItemScaleX = r;
    o->shownItemScaleY = r;
    o->shownItemScaleZ = r;
    (o)->updateShownItemPos(r);
}


}  // namespace ns_02223314

namespace ns_02222838 {

struct PlayerFtrGrabApproachArgs {
    s32 x, z;
    s16 h;
};

struct FtrGrabApproachWork {
    s32 x;
    s32 z;
    s16 h;
    u8 flag;
    u8 pad_b;
};

typedef PlayerActor Obj;
typedef TalkMsgRequest Sec;
typedef VecFx32 V3;
typedef PlayerFtrGrabApproachArgs Tgt;
typedef PlayerActionRequest Msg;
typedef FtrGrabApproachWork Rec;

extern "C" {
extern u8 gFieldSceneKind;
extern void *gCommManager;

s32 Scene_InHouseRoom(void);
s32 TalkRequest_AddPlayerMessage(void);
void TalkRequest_FinishPlayerMessage(void);
s32 Item_IsFurniture(void *p);
s32 FieldAction_RequestPlaceAtPendingForAid(u32 a, u32 b);

s32 PlayerActor_RequestFtrPush(Obj *o, s32 a, s32 b);
s32 PlayerActor_RequestFtrPull(Obj *o, s32 a, s32 b);

void PlayerActor_SetupFtrRotate(Obj *o, Obj *arg);
s32 PlayerActor_RequestFtrRotate(Obj *o, u32 a, s32 b, s32 c);
void PlayerActor_MainFtrHold(Obj *o);
void PlayerActor_FtrHoldUpdate(Obj *o);
void PlayerActor_NetFtrHold(Obj *o, s32 c);
void PlayerActor_SetupFtrHold(Obj *o, Obj *arg);
void PlayerActor_MainFtrGrabApproach(Obj *o);
void PlayerActor_FtrGrabApproachCheckArrive(Obj *o);
void PlayerActor_FtrGrabApproachMove(Obj *o);
void PlayerActor_EndFtrGrabApproach(Obj *o);
s32 PlayerActor_NetFtrGrabApproach(Obj *o, s32 c);
void PlayerActor_SetupFtrGrabApproach(Obj *o, Obj *arg);
void PlayerActor_FtrGrabApproachSetWork(Rec *r, s32 x, s32 z, s16 h);
void PlayerActor_FtrGrabApproachSetArgs(Tgt *t, s32 x, s32 z, s16 h);
void PlayerActor_MainPickUpItem(Obj *o);
void PlayerActor_PickUpItemUpdateState(Obj *o);
}

struct FtrHoldStackPad {
    s32 v[2];
    FtrHoldStackPad() {}
    ~FtrHoldStackPad() {}
};

static inline BOOL Unk_ov004_02222874_IsOne(u8 v) {
    return v == 1 ? TRUE : FALSE;
}

#define ECC_START_FAIL(c) \
    { \
        *p = 4; \
        ((PlayerActor *)(o))->startAnim(0, 6, 6); \
        o->dropQuery = FieldAction_RequestPlaceAtPendingForAid(o->sessionSlot, o->actionItem); \
    }

extern "C" void PlayerActor_PickUpItemUpdateState(Obj *o) {
    u8 *p = &o->actionWorkRaw[5];
    struct {
        u32 pad;
        u16 a;
        u16 b;
    } w;
    s32 t;
    switch (*p) {
    case 0:
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished() == 0) {
            break;
        }
        {
            BOOL ok;
#ifdef N3DS_DIRECT
            if (Pocket_FindEmpty() == -1 && Pocket_FindFruitStack(&o->actionItem) == -1) {  // (or onto a fruit stack)
#else
            if (Pocket_FindEmpty() == -1) {
#endif
                if (Item_IsFurniture(&o->actionItem) != 0) {
                    w.b = 0xfff1;
                    u32 r6 = Item_GetFurnitureIndex(&o->actionItem);
                    if (r6 == Item_GetFurnitureIndex(&w.b)) {
                        ok = TRUE;
                    } else {
                        ok = FALSE;
                    }
                } else {
                    if (o->actionItem == 0xfff1) {
                        ok = TRUE;
                    } else {
                        ok = FALSE;
                    }
                }
                if (ok == FALSE) {
                    if (TalkRequest_AddPlayerMessage() == 0) {
                        break;
                    }
                    (o)->startAnim(0x6c, 3, 0);
                    s32 r = (o)->getHeldToolKind();
                    if (r == 4) {
                        HeldItemModel_PlayAnim(&o->heldItemModel, 0, 9, 0);
                    } else if (r == 3) {
                        HeldItemModel_PlayAnim(&o->heldItemModel, 0x13, 3, 0);
                    }
                    *p = 1;
                    ((Character *)(o))->attachTalkRequest(o);
                    (o)->setActionFlag(0x11);
                    {
                        Sec &s = *o;
                        ((MsgRequest *)(void *)(&s))->setFileName((const char *)(sRoomPlayerMsgFile));
                    }
                    o->msgIndex = 2;
                    o->window->nextState = 1;
                    break;
                }
            }
        }
        w.a = o->actionItem;
        Pocket_AddFoundItem(&w.a);
        *p = 5;
        break;
    case 1:
        if (o->window != 0) {
            if (o->window->state != 0) {
                *p = 2;
                o->msgStep = 3;
            }
        }
        break;
    case 2:
        t = o->msgStep;
        if (t >= 0xf) {
            *p = 3;
            if (o->dropQuery == -1) {
                ECC_START_FAIL(0)
            }
        } else if (o->window != 0) {
            if (o->window->state == 0) {
                if (t == 5) {
                    if (MenuCtrl_OpenPocketsFullPickUp(o->actionItem) != 0) {
                        o->msgStep = 6;
                    }
                } else if (t == 6) {
                    if (MenuCtrl_IsFinished() != 0) {
                        o->msgStep = 0xf;
                        ((AnimFrameCtrl &)o->bodyModel).frameStep = 0x1000;
                        if (MenuCtrl_IsResultOk() != 0) {
                            *p = 5;
                            ((Character *)(o))->detachTalkRequest(o);
                            (o)->clearActionFlag(0x11);
                            TalkRequest_FinishPlayerMessage();
                        } else {
                            *p = 3;
                            if (o->dropQuery == -1) {
                                ECC_START_FAIL(0)
                            }
                        }
                    }
                }
            }
        }
        break;
    case 3:
        if (o->dropQuery == -1) {
            ECC_START_FAIL(0)
        }
        break;
    case 4:
        if (o->window != 0) {
            if (o->window->state == 0) {
                ((Character *)(o))->detachTalkRequest(o);
                (o)->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                *p = 5;
                (o)->startAnim(0, 6, 6);
            }
        }
        break;
    case 5:
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        ((PlayerActor *)(o))->requestWait(3, 1, -1);
        ((PlayerActor *)(o))->clearActionFlag(0xd);
        o->msgStep = 0xf;
        break;
    case 6:
        if (TalkRequest_AddPlayerMessage() != 0) {
            *p = 7;
            ((Character *)(o))->attachTalkRequest(o);
            (o)->setActionFlag(0x11);
            {
                Sec &s = *o;
                ((MsgRequest *)(void *)(&s))->setFileName((const char *)(sRoomPlayerMsgFile));
            }
            o->msgIndex = 0;
            o->window->nextState = 1;
        }
        break;
    case 7:
        if (o->window != 0) {
            if (o->window->state != 0) {
                *p = 8;
            }
        }
        break;
    case 8:
        if (o->window != 0) {
            if (o->window->state == 0) {
                ((Character *)(o))->detachTalkRequest(o);
                (o)->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                *p = 9;
            }
        }
        break;
    case 9:
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        ((PlayerActor *)(o))->requestWait(3, 1, -1);
        ((PlayerActor *)(o))->clearActionFlag(0xd);
        break;
    case 10:
        if (TalkRequest_AddPlayerMessage() != 0) {
            *p = 0xb;
            ((Character *)(o))->attachTalkRequest(o);
            (o)->setActionFlag(0x11);
            {
                Sec &s = *o;
                ((MsgRequest *)(void *)(&s))->setFileName((const char *)(sRoomErrorMsgFile));
            }
            o->msgIndex = 1;
            o->window->nextState = 1;
        }
        break;
    case 11:
        if (o->window != 0) {
            if (o->window->state != 0) {
                *p = 0xc;
            }
        }
        break;
    case 12:
        if (o->window != 0) {
            if (o->window->state == 0) {
                ((Character *)(o))->detachTalkRequest(o);
                (o)->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
                (o)->requestWait(3, 1, -1);
            }
        }
        break;
    }
}

extern "C" void PlayerActor_MainPickUpItem(Obj *o) {
    PlayerActor_PickUpItemUpdateAnim(o);
    (o)->updateBodyCollider();
    PlayerActor_PickUpItemUpdateScale(o);
    PlayerActor_PickUpItemUpdateState(o);
}

extern "C" void PlayerActor_FtrGrabApproachSetArgs(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" s32 PlayerActor_RequestFtrGrabApproach(Obj *o, s32 x, s32 z, s16 h, u32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x1d, a, *(s16 *)&b);
    PlayerActor_FtrGrabApproachSetArgs((PlayerFtrGrabApproachArgs *)m.unk_0c_b, x, z, h);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_FtrGrabApproachSetWork(Rec *r, s32 x, s32 z, s16 h) {
    r->x = x;
    r->z = z;
    r->h = h;
    r->flag = 0;
}

extern "C" void PlayerActor_FtrGrabApproachSetNetData(u8 *p, s32 x, s32 z, s16 h) {
    NetBuf_PackPair20(p, x, z);
    NetBuf_WriteS16B((u8 *)p + 5, h);
}

extern "C" void PlayerActor_FtrGrabApproachGetNetData(u8 *p, s32 *x, s32 *z, s16 *h) {
    NetBuf_UnpackPair20(p, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B((u8 *)p + 5);
}

extern "C" void PlayerActor_SetupFtrGrabApproach(Obj *o, Obj *arg) {
    (o)->startAnimOnce(0x1a, 3, 0);
    V3 *p = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    s32 x = p->x;
    s32 z = p->y;
    s16 h = (s16)p->z;
    PlayerActor_FtrGrabApproachSetWork((Rec *)&o->actionWork, x, z, h);
    PlayerActor_FtrGrabApproachSetNetData(o->netData.bytes, x, z, h);
}

extern "C" s32 PlayerActor_NetFtrGrabApproach(Obj *o, s32 c) {
    s32 x, z;
    s16 h;
    PlayerActor_FtrGrabApproachGetNetData(o->netData.bytes, &x, &z, &h);
    return PlayerActor_RequestFtrGrabApproach(o, x, z, h, 5, c);
}

extern "C" void PlayerActor_EndFtrGrabApproach(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->z;
    (o)->setAngleY(&r->h);
}

extern "C" void PlayerActor_FtrGrabApproachMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    PlayerActor_StepTowardPose(o, r->x, r->z, r->h);
}

extern "C" void PlayerActor_FtrGrabApproachCheckArrive(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *v = (V3 *)&o->position;
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished() && v->x == r->x && v->z == r->z && r->h == o->rotY) {
        PlayerActor_RequestFtrHold(o, 1, 0, 5, -1);
    } else if (o->actionHeld == 0) {
        if (r->flag == 0) {
            r->flag = 1;
            if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished() == 0) {
                ((FtrContactSet *)(FtrContactSet_GetInstance()))->requestToggle();
            }
        } else {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
            (o)->requestWait(3, 1, -1);
        }
    }
}

extern "C" void PlayerActor_MainFtrGrabApproach(Obj *o) {
    PlayerActor_FtrGrabApproachMove(o);
    (o)->advanceAnim();
    (o)->moveWithCollision();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_FtrGrabApproachCheckArrive(o);
    } else {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
    }
}

extern "C" void PlayerActor_FtrHoldSetArgs(u8 *p, u32 a, u32 b) {
    p[0] = a;
    p[1] = b;
}

extern "C" s32 PlayerActor_RequestFtrHold(Obj *o, u32 a, u32 b, u32 c, s32 e) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x1e, c, *(s16 *)&e);
    PlayerActor_FtrHoldSetArgs(m.unk_0c_b, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_FtrHoldSetWork(u8 *p, s32 v) {
    if (v > 0x333) {
        p[0] = 0;
    } else {
        p[0] = 1;
    }
    p[1] = 0;
}

extern "C" void PlayerActor_FtrHoldSetNetData(u8 *p, u32 a, u32 b) {
    p[0] = a;
    p[1] = b;
}

extern "C" void PlayerActor_FtrHoldGetNetData(u8 *src, u8 *a, u8 *b) {
    *a = src[0];
    *b = src[1];
}

extern "C" void PlayerActor_SetupFtrHold(Obj *o, Obj *arg) {
    u8 *a = ((PlayerActionRequest *)(void *)arg)->unk_0c_b;
    u8 *r = &o->actionWorkRaw[0];
    u32 x = a[0];
    u32 y = a[1];
    s32 t = (o)->getInputMagnitude();
    if (x != 0) {
        t = 0;
    }
    PlayerActor_FtrHoldSetWork(r, t);
    if (y != 0) {
        (o)->startAnim(0x1a, 3, 0);
    } else {
        (o)->startAnim(0x1a, 0, 0);
    }
    PlayerActor_FtrHoldSetNetData(o->netData.bytes, x, y);
}

extern "C" void PlayerActor_NetFtrHold(Obj *o, s32 c) {
    u8 l[2];
    PlayerActor_FtrHoldGetNetData(o->netData.bytes, &l[0], &l[1]);
    PlayerActor_RequestFtrHold(o, l[0], l[1], 5, c);
}

extern "C" void PlayerActor_FtrHoldUpdate(Obj *o) {
    FtrHoldStackPad pad;
    u8 *p = &o->actionWorkRaw[0];
    u8 *q = p + 1;
    switch (*q) {
    case 0:
        if (*p == 0) {
            if ((o)->getInputMagnitude() < 0x333) {
                *p = 1;
            }
        }
        if (o->actionHeld == 0) {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
            (o)->requestWait(3, 1, -1);
        } else if ((o)->getInputMagnitude() > 0x333) {
            if ((u32)Room_CountOccupants() <= 1) {
                o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
                switch ((o)->getInputDirRelative()) {
                case 2:
                    PlayerActor_RequestFtrPull(o, 6, -1);
                    break;
                case 0:
                    PlayerActor_RequestFtrPush(o, 6, -1);
                    break;
                case 3:
                    if (*p != 0) {
                        if (PlayerActor_RequestFtrRotate(o, 1, 6, -1) == 0) {
                            *p = 0;
                        }
                    }
                    break;
                case 1:
                    if (*p != 0) {
                        if (PlayerActor_RequestFtrRotate(o, 0, 6, -1) == 0) {
                            *p = 0;
                        }
                    }
                    break;
                }
            } else {
                if (Scene_InHouseRoom() != 0 && o->sessionSlot == 0) {
                    (*q)++;
                }
            }
        }
        break;
    case 1:
        if (TalkRequest_AddPlayerMessage() != 0) {
            (*q)++;
            ((Character *)(o))->attachTalkRequest(o);
            (o)->setActionFlag(0x11);
            {
                Sec &s = *o;
                ((MsgRequest *)(void *)(&s))->setFileName((const char *)(sRoomErrorMsgFile));
            }
            o->msgIndex = 1;
            o->window->nextState = 1;
        }
        break;
    case 2:
        if (o->window != 0) {
            if (o->window->state != 0) {
                *q = *q + 1;
            }
        }
        break;
    case 3:
        if (o->window != 0) {
            if (o->window->state == 0) {
                ((Character *)(o))->detachTalkRequest(o);
                (o)->clearActionFlag(0x11);
                TalkRequest_FinishPlayerMessage();
                *q = 0;
            }
        }
        break;
    }
}

extern "C" void PlayerActor_MainFtrHold(Obj *o) {
    (o)->advanceAnim();
    (o)->moveWithCollision();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_FtrHoldUpdate(o);
    } else {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
    }
}

extern "C" void PlayerActor_FtrRotateSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestFtrRotate(Obj *o, u32 a, s32 b, s32 c) {
    if (!Unk_ov004_02222874_IsOne(gFieldSceneKind)) {
        return 0;
    }
    if ((a != 0 && ((FtrContactSet *)(FtrContactSet_GetInstance()))->canRotatePlus90() != 0) || (a == 0 && ((FtrContactSet *)(FtrContactSet_GetInstance()))->canRotateMinus90() != 0)) {
        Msg m;
        ((PlayerActionRequest *)(&m))->assign(0x1f, b, c);
        PlayerActor_FtrRotateSetArgs(m.unk_0c_b, a);
        if (a != 0) {
            ((FtrContactSet *)(FtrContactSet_GetInstance()))->startRotatePlus90();
        } else {
            ((FtrContactSet *)(FtrContactSet_GetInstance()))->startRotateMinus90();
        }
        s32 r = (o)->pushRequest(&m);
        return r;
    }
    if (Scene_InHouseRoom() != 0 && o->sessionSlot == 0) {
        (o)->playSe(0x6d);
    }
    return 0;
}

extern "C" void PlayerActor_FtrRotateSetStickFlag(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_SetupFtrRotate(Obj *o, Obj *arg) {
    u8 *p = &o->actionWorkRaw[0];
    if (*(((PlayerActionRequest *)(void *)arg)->unk_0c_b) != 0) {
        (o)->startAnimOnce(0x1b, 3, 0);
    } else {
        (o)->startAnimOnce(0x1c, 3, 0);
    }
    PlayerActor_FtrRotateSetStickFlag(p, 0);
}

extern "C" void PlayerActor_NetFtrRotate(void) {
}


}  // namespace ns_02222838

namespace ns_02221ed0 {

struct SeatApproachWork {
    s32 x;
    s32 z;
    s16 h;
    u8 d;
};

struct Unk_ov004_02221ed0_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

typedef PlayerActor Obj;
typedef Unk_ov004_02221ed0_Bits Bits;
typedef VecFx32 V3;
typedef SeatApproachWork Rec;
typedef PlayerActionRequest Msg;

extern "C" {
extern void *gCommManager;


s32 Scene_InHouseRoom(void);

s32 PlayerActor_RequestSitDownFront(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestSitDownSide1(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestSitDownSide2(Obj *o, u32 a, u32 b);

void PlayerActor_MainSeatApproach(Obj *o);
void PlayerActor_SeatApproachCheckArrive(Obj *o);
void PlayerActor_SeatApproachMove(Obj *o);
void PlayerActor_EndSeatApproach(Obj *o);
void PlayerActor_NetSeatApproach(Obj *o, s32 a);
void PlayerActor_SetupSeatApproach(Obj *o, Msg *m);
void PlayerActor_SeatApproachSetWork(Rec *r, s32 x, s32 z, s16 h, u8 d);
void PlayerActor_SeatApproachSetArgs(Rec *r, s32 x, s32 z, s16 h, u8 d);
void PlayerActor_MainFtrPullMove(Obj *o);
void PlayerActor_FtrPullMoveCheckEnd(Obj *o);
void PlayerActor_FtrPullMoveCollide(Obj *o);
void PlayerActor_EndFtrPullMove(Obj *o);
void PlayerActor_SetupFtrPullMove(Obj *o);
void PlayerActor_FtrPullMoveSetWork(Rec *r, s32 x, s32 z);
s32 PlayerActor_RequestFtrPullMove(Obj *o, s32 a, s32 b);
void PlayerActor_MainFtrPushMove(Obj *o);
void PlayerActor_FtrPushMoveCheckEnd(Obj *o);
void PlayerActor_EndFtrPushMove(Obj *o);
void PlayerActor_SetupFtrPushMove(Obj *o);
void PlayerActor_FtrPushMoveSetWork(Rec *r, s32 x, s32 z);
s32 PlayerActor_RequestFtrPushMove(Obj *o, s32 a, s32 b);
void PlayerActor_MainFtrPull(Obj *o);
void PlayerActor_FtrPullTryMove(Obj *o);
void PlayerActor_NetFtrPull(Obj *o, s32 a);
void PlayerActor_SetupFtrPull(Obj *o);
s32 PlayerActor_RequestFtrPull(Obj *o, s32 a, s32 b);
void PlayerActor_MainFtrPush(Obj *o);
void PlayerActor_FtrPushTryMove(Obj *o);
void PlayerActor_NetFtrPush(Obj *o, s32 a);
void PlayerActor_SetupFtrPush(Obj *o);
s32 PlayerActor_RequestFtrPush(Obj *o, s32 a, s32 b);
void PlayerActor_MainFtrRotate(Obj *o);
void PlayerActor_FtrRotateCheckEnd(Obj *o);
}

extern "C" void PlayerActor_FtrRotateCheckEnd(Obj *o) {
    if (((AnimFrameCtrl &)o->bodyModel).playMode == 3) {
        u8 *f = (u8 *)&o->actionWork;
        s32 t;
        if (*f == 0) {
            if ((o)->getInputMagnitude() < 0x19a) {
                PlayerActor_FtrRotateSetStickFlag(f, 1);
            }
        }
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        t = (o)->getInputDirRelative();
        if (*f != 0 && (o)->getInputMagnitude() > 0x19a) {
            if (t == 3 || t == 1) {
                PlayerActor_RequestFtrHold(o, 1, 1, 5, -1);
                return;
            }
        }
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
            if (o->actionHeld) {
                PlayerActor_RequestFtrHold(o, 0, 1, 5, -1);
            } else {
                (o)->requestWait(3, 1, -1);
            }
        }
    } else {
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
            ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->setup(0, 3, 0x1000, ((Bits *)&((AnimFrameCtrl &)o->bodyModel).numFrames)->mid);
        }
    }
}

extern "C" void PlayerActor_MainFtrRotate(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    PlayerActor_FtrRotateCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestFtrPush(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x20, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupFtrPush(Obj *o) {
    (o)->startAnimOnce(0x1d, 3, 0);
    *(u8 *)&o->actionWork = 0;
}

extern "C" void PlayerActor_NetFtrPush(Obj *o, s32 a) {
    PlayerActor_RequestFtrPush(o, 6, a);
}

extern "C" void PlayerActor_FtrPushTryMove(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            u8 r4 = 0;
            if (((FtrContactSet *)(FtrContactSet_GetInstance()))->canPush() == 0) {
                r4 = 1;
            }
            if (Scene_InHouseRoom() == 0 || o->sessionSlot != 0) {
                r4 |= 2;
            }
            if (r4 != 0) {
                u8 *f = (u8 *)&o->actionWork;
                if (*f == 0) {
                    *f = 1;
                    if (r4 == 1) {
                        (o)->playSe(0x6d);
                    }
                }
                s32 t = (o)->getInputDirRelative();
                if ((o)->getInputMagnitude() > 0) {
                    if (t == 0) {
                        return;
                    }
                }
                o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
                PlayerActor_RequestFtrHold(o, 0, 1, 5, -1);
            } else {
                PlayerActor_RequestFtrPushMove(o, 6, -1);
                ((FtrContactSet *)(FtrContactSet_GetInstance()))->startPush();
            }
        } else {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        }
    }
}

extern "C" void PlayerActor_MainFtrPush(Obj *o) {
    (o)->advanceAnim();
    (o)->moveWithCollision();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    PlayerActor_FtrPushTryMove(o);
}

extern "C" s32 PlayerActor_RequestFtrPull(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x21, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupFtrPull(Obj *o) {
    (o)->startAnimOnce(0x1e, 3, 0);
    *(u8 *)&o->actionWork = 0;
}

extern "C" void PlayerActor_NetFtrPull(Obj *o, s32 a) {
    PlayerActor_RequestFtrPull(o, 6, a);
}

extern "C" void PlayerActor_FtrPullTryMove(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            u8 r4 = 0;
            if (((FtrContactSet *)(FtrContactSet_GetInstance()))->canPull() == 0) {
                r4 = 1;
            }
            if (Scene_InHouseRoom() == 0 || o->sessionSlot != 0) {
                r4 |= 2;
            }
            if (r4 != 0) {
                u8 *f = (u8 *)&o->actionWork;
                if (*f == 0) {
                    *f = 1;
                    if (r4 == 1) {
                        (o)->playSe(0x6d);
                    }
                }
                s32 t = (o)->getInputDirRelative();
                if ((o)->getInputMagnitude() > 0) {
                    if (t == 2) {
                        return;
                    }
                }
                o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
                PlayerActor_RequestFtrHold(o, 0, 1, 5, -1);
            } else {
                PlayerActor_RequestFtrPullMove(o, 6, -1);
                ((FtrContactSet *)(FtrContactSet_GetInstance()))->startPull();
            }
        } else {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        }
    }
}

extern "C" void PlayerActor_MainFtrPull(Obj *o) {
    (o)->advanceAnim();
    (o)->moveWithCollision();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    PlayerActor_FtrPullTryMove(o);
}

extern "C" s32 PlayerActor_RequestFtrPushMove(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x22, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_FtrPushMoveSetWork(Rec *r, s32 x, s32 z) {
    r->x = x;
    r->z = z;
}

extern "C" void PlayerActor_SetupFtrPushMove(Obj *o) {
    V3 v;
    (o)->startAnimOnce(0x1f, 3, 0);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
    PlayerActor_FtrPushMoveSetWork((Rec *)&o->actionWork, v.x, v.z);
}

extern "C" void PlayerActor_NetFtrPushMove(void) {
}

extern "C" void PlayerActor_EndFtrPushMove(Obj *o) {
    V3 *p = (V3 *)&o->position;
    Rec *r = (Rec *)&o->actionWork;
    p->x = r->x;
    p->z = r->z;
}

extern "C" void PlayerActor_FtrPushMoveCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        if (o->actionHeld) {
            PlayerActor_RequestFtrHold(o, 1, 0, 5, -1);
        } else {
            (o)->requestWait(3, 1, -1);
        }
    }
}

extern "C" void PlayerActor_MainFtrPushMove(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    PlayerActor_FtrPushMoveCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestFtrPullMove(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x23, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_FtrPullMoveSetWork(Rec *r, s32 x, s32 z) {
    r->x = x;
    r->z = z;
}

extern "C" void PlayerActor_SetupFtrPullMove(Obj *o) {
    V3 v;
    (o)->startAnimOnce(0x20, 3, 0);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, &sFtrPullDist);
    PlayerActor_FtrPullMoveSetWork((Rec *)&o->actionWork, v.x, v.z);
}

extern "C" void PlayerActor_NetFtrPullMove(void) {
}

extern "C" void PlayerActor_EndFtrPullMove(Obj *o) {
    V3 *p = (V3 *)&o->position;
    Rec *r = (Rec *)&o->actionWork;
    p->x = r->x;
    p->z = r->z;
    o->prevPosition.x = p->x;
    o->prevPosition.y = p->y;
    o->prevPosition.z = p->z;
}

extern "C" void PlayerActor_FtrPullMoveCollide(Obj *o) {
    volatile V3 old;
    V3 *pv = (V3 *)&o->position;
    s32 y;
    old.x = o->position.x;
    y = pv->y;
    old.y = y;
    old.z = pv->z;
    s32 nz = o->bodyPos.z;
    s32 nx = o->bodyPos.x;
    o->position.x = nx;
    o->position.y = y;
    o->position.z = nz;
    o->prevPosition.x = o->position.x;
    o->prevPosition.y = o->position.y;
    o->prevPosition.z = o->position.z;
    (o)->moveWithCollision();
    s32 ox = old.x;
    if (ox == o->bodyPos.x) {
        o->position.z = old.z;
    } else {
        o->position.x = ox;
    }
}

extern "C" void PlayerActor_FtrPullMoveCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        if (o->actionHeld) {
            PlayerActor_RequestFtrHold(o, 1, 0, 5, -1);
        } else {
            (o)->requestWait(3, 1, -1);
        }
    }
}

extern "C" void PlayerActor_MainFtrPullMove(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_FtrPullMoveCollide(o);
    (o)->updateBodyCollider();
    (o)->submitSceneCollider();
    PlayerActor_FtrPullMoveCheckEnd(o);
}

extern "C" void PlayerActor_SeatApproachSetArgs(Rec *r, s32 x, s32 z, s16 h, u8 d) {
    r->x = x;
    r->z = z;
    r->h = h;
    r->d = d;
}

extern "C" s32 PlayerActor_RequestSeatApproach(Obj *o, s32 x, s32 z, s16 h, u8 d, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x24, a, *(s16 *)&b);
    PlayerActor_SeatApproachSetArgs((SeatApproachWork *)m.unk_0c_b, x, z, h, d);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SeatApproachSetWork(Rec *r, s32 x, s32 z, s16 h, u8 d) {
    r->x = x;
    r->z = z;
    r->h = h;
    r->d = d;
}

extern "C" void PlayerActor_SeatApproachSetNetData(u8 *self, s32 x, s32 z, s16 h, u8 d) {
    NetBuf_PackPair20(self, x, z);
    NetBuf_WriteS16B(self + 5, h);
    self[7] = d;
}

extern "C" void PlayerActor_SeatApproachGetNetData(u8 *self, s32 *x, s32 *z, s16 *h, u8 *d) {
    NetBuf_UnpackPair20(self, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B(self + 5);
    *d = self[7];
}

extern "C" void PlayerActor_SetupSeatApproach(Obj *o, Msg *m) {
    Rec *p = (SeatApproachWork *)m->unk_0c_b;
    s16 h = p->h;
    u8 d = p->d;
    s32 z = p->z;
    s32 y = o->position.y;
    s32 x = p->x;
    V3 tmp;
    V3 v;
    s16 t;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    t = h + sSeatSideAngles[d];
    PlayerActor_OffsetByAngle(&v, o, &tmp, &t, &sSeatApproachDist);
    PlayerActor_SeatApproachSetWork((Rec *)&o->actionWork, v.x, v.z, t, d);
    PlayerActor_SeatApproachSetNetData(o->netData.bytes, tmp.x, tmp.z, h, d);
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        SpotSync_RequestReserve((s32 *)&tmp);
    }
    (o)->startAnim(0x1a, 3, 0);
}

extern "C" void PlayerActor_NetSeatApproach(Obj *o, s32 a) {
    struct {
        u8 d;
        u8 pad;
        s16 h;
    } l;
    s32 x, z;
    PlayerActor_SeatApproachGetNetData(o->netData.bytes, &x, &z, &l.h, &l.d);
    PlayerActor_RequestSeatApproach(o, x, z, l.h, l.d, 6, a);
}

extern "C" void PlayerActor_EndSeatApproach(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->z;
    (o)->setAngleY(&r->h);
}

extern "C" void PlayerActor_SeatApproachMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    PlayerActor_StepTowardPose(o, r->x, r->z, r->h);
}

extern "C" void PlayerActor_SeatApproachCheckArrive(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        return;
    }
    switch (SpotSync_GetReserveResult()) {
    case 0:
        return;
    case 1:
        break;
    case 2:
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(3, 1, -1);
        return;
    }
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    if (p->x == r->x) {
        if (p->z == r->z) {
            if (r->h == o->rotY) {
                switch (r->d) {
                case 0:
                    PlayerActor_RequestSitDownFront(o, 6, -1);
                    break;
                case 1:
                    PlayerActor_RequestSitDownSide1(o, 6, -1);
                    break;
                case 2:
                    PlayerActor_RequestSitDownSide2(o, 6, -1);
                    break;
                }
            }
        }
    }
}

extern "C" void PlayerActor_MainSeatApproach(Obj *o) {
    PlayerActor_SeatApproachMove(o);
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_SeatApproachCheckArrive(o);
}



}  // namespace ns_02221ed0

namespace ns_022215a8 {

struct PlayerSitDownArgs {
    s32 x, z;
    s16 h;
};

struct SitWork {
    s32 x;
    union {
        s32 z;
        u8 flag;
    } u;
    s16 h;
};

struct Unk_ov004_022215a8_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

typedef PlayerActor Obj;
typedef Unk_ov004_022215a8_Bits Bits;
typedef VecFx32 V3;
struct Unk_ov004_022215a8_V3c {
    volatile s32 x, y, z;
    Unk_ov004_022215a8_V3c(s32 a, s32 b, s32 c) {
        x = a;
        y = b;
        z = c;
    }
};
typedef Unk_ov004_022215a8_V3c V3c;
typedef PlayerSitDownArgs Tgt;
typedef PlayerActionRequest Msg;
typedef SitWork Rec;
typedef TwoLayerAnimModel Sub;

extern "C" {
extern u8 gFieldSceneKind;
extern void *gCommManager;

s32 Math_IsInRange(s32 a, s32 b, s32 c);


void PlayerActor_MainSit(Obj *o);
void PlayerActor_SitReadInput(Obj *o);
void PlayerActor_SitUpdateAnim(Obj *o);
void PlayerActor_EndSit(Obj *o);
s32 PlayerActor_NetSit(Obj *o, s32 a);
void PlayerActor_SetupSit(Obj *o, Obj *arg);
void PlayerActor_MainSitDownSide1(Obj *o);
void PlayerActor_SitDownSide1CheckEnd(Obj *o);
void PlayerActor_SitDownSide1Update(Obj *o);
void PlayerActor_EndSitDownSide1(Obj *o);
void PlayerActor_NetSitDownSide1(Obj *o, s32 a);
void PlayerActor_SetupSitDownSide1(Obj *o, Obj *arg);
void PlayerActor_SitDownSide1SetWork(Tgt *t, s32 x, s32 z, s16 h);
s32 PlayerActor_RequestSitDownSide1At(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b);
s32 PlayerActor_RequestSitDownSide1(Obj *o, u32 a, u32 b);
void PlayerActor_SitDownSide1SetArgs(Tgt *t, s32 x, s32 z, s16 h);
void PlayerActor_MainSitDownSide2(Obj *o);
void PlayerActor_SitDownSide2CheckEnd(Obj *o);
void PlayerActor_SitDownSide2Update(Obj *o);
void PlayerActor_EndSitDownSide2(Obj *o);
void PlayerActor_NetSitDownSide2(Obj *o, s32 a);
void PlayerActor_SetupSitDownSide2(Obj *o, Obj *arg);
void PlayerActor_SitDownSide2SetWork(Tgt *t, s32 x, s32 z, s16 h);
s32 PlayerActor_RequestSitDownSide2At(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b);
s32 PlayerActor_RequestSitDownSide2(Obj *o, u32 a, u32 b);
void PlayerActor_SitDownSide2SetArgs(Tgt *t, s32 x, s32 z, s16 h);
void PlayerActor_MainSitDownFront(Obj *o);
void PlayerActor_SitDownFrontCheckEnd(Obj *o);
void PlayerActor_SitDownFrontUpdate(Obj *o);
void PlayerActor_EndSitDownFront(Obj *o);
void PlayerActor_NetSitDownFront(Obj *o, s32 a);
void PlayerActor_SetupSitDownFront(Obj *o, Obj *arg);
void PlayerActor_SitDownFrontSetWork(Tgt *t, s32 x, s32 z, s16 h);
s32 PlayerActor_RequestSitDownFrontAt(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b);
s32 PlayerActor_RequestSitDownFront(Obj *o, u32 a, u32 b);
}

static inline BOOL Unk_ov004_022215a8_IsOne(u8 v) {
    return v == 1 ? TRUE : FALSE;
}

extern "C" void PlayerActor_SitDownFrontSetArgs(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" s32 PlayerActor_RequestSitDownFront(Obj *o, u32 a, u32 b) {
    Msg m;
    V3 v;
    ((PlayerActionRequest *)(&m))->assign(0x25, a, b);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
    PlayerActor_SitDownFrontSetArgs((PlayerSitDownArgs *)m.unk_0c_b, v.x, v.z, (s16)(o->rotY + 0x8000));
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" s32 PlayerActor_RequestSitDownFrontAt(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x25, a, *(s16 *)&b);
    PlayerActor_SitDownFrontSetArgs((PlayerSitDownArgs *)m.unk_0c_b, *x, *z, *h);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SitDownFrontSetWork(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" void PlayerActor_SitDownFrontSetNetData(u8 *p, s32 x, s32 z, s16 h) {
    NetBuf_PackPair20(p, x, z);
    NetBuf_WriteS16B((u8 *)p + 5, h);
}

extern "C" void PlayerActor_SitDownFrontGetNetData(u8 *p, s32 *x, s32 *z, s16 *h) {
    NetBuf_UnpackPair20(p, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B((u8 *)p + 5);
}

extern "C" void PlayerActor_SetupSitDownFront(Obj *o, Obj *arg) {
    s16 h;
    (o)->startAnimOnce(0x21, 3, 0);
    (o)->playSe(0x4c5);
    V3 *p = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    h = (s16)p->z;
    s32 z = p->y;
    s32 y = *(volatile s32 *)&o->position.y;
    s32 x = *(s32 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    V3c v(x, y, z);
    PlayerActor_SitDownFrontSetWork((Tgt *)&o->actionWork, x, z, h);
    PlayerActor_SitDownFrontSetNetData(o->netData.bytes, v.x, v.z, h);
}

extern "C" void PlayerActor_NetSitDownFront(Obj *o, s32 a) {
    s32 x, z;
    s16 h;
    PlayerActor_SitDownFrontGetNetData(o->netData.bytes, &x, &z, &h);
    PlayerActor_RequestSitDownFrontAt(o, &x, &z, &h, 6, a);
}

extern "C" void PlayerActor_EndSitDownFront(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->u.z;
    (o)->setAngleY(&r->h);
}

extern "C" void PlayerActor_SitDownFrontUpdate(Obj *o) {
    V3 v;
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xc)) {
        if (Unk_ov004_022215a8_IsOne(gFieldSceneKind)) {
            Rec *r = (Rec *)&o->actionWork;
            s32 z = r->u.z;
            s32 y = o->position.y;
            s32 x = r->x;
            v.x = x;
            v.y = y;
            v.z = z;
            FtrMgr_PlaySeatSound1At(&v);
        }
    }
}

extern "C" void PlayerActor_SitDownFrontCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestSit(o, 0, 6, -1);
    }
}

extern "C" void PlayerActor_MainSitDownFront(Obj *o) {
    PlayerActor_SitDownFrontUpdate(o);
    (o)->netFollowTransform();
    (o)->updateBodyCollider();
    PlayerActor_SitDownFrontCheckEnd(o);
}

extern "C" void PlayerActor_SitDownSide2SetArgs(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" s32 PlayerActor_RequestSitDownSide2(Obj *o, u32 a, u32 b) {
    Msg m;
    V3 v;
    ((PlayerActionRequest *)(&m))->assign(0x26, a, b);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
    PlayerActor_SitDownSide2SetArgs((PlayerSitDownArgs *)m.unk_0c_b, v.x, v.z, (s16)(o->rotY + 0x4000));
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" s32 PlayerActor_RequestSitDownSide2At(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x26, a, *(s16 *)&b);
    PlayerActor_SitDownSide2SetArgs((PlayerSitDownArgs *)m.unk_0c_b, *x, *z, *h);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SitDownSide2SetWork(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" void PlayerActor_SitDownSide2SetNetData(u8 *p, s32 x, s32 z, s16 h) {
    NetBuf_PackPair20(p, x, z);
    NetBuf_WriteS16B((u8 *)p + 5, h);
}

extern "C" void PlayerActor_SitDownSide2GetNetData(u8 *p, s32 *x, s32 *z, s16 *h) {
    NetBuf_UnpackPair20(p, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B((u8 *)p + 5);
}

extern "C" void PlayerActor_SetupSitDownSide2(Obj *o, Obj *arg) {
    s16 h;
    (o)->startAnimOnce(0x22, 0, 0);
    (o)->playSe(0x4c5);
    V3 *p = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    h = (s16)p->z;
    s32 z = p->y;
    s32 y = *(volatile s32 *)&o->position.y;
    s32 x = *(s32 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    V3c v(x, y, z);
    PlayerActor_SitDownSide2SetWork((Tgt *)&o->actionWork, x, z, h);
    PlayerActor_SitDownSide2SetNetData(o->netData.bytes, v.x, v.z, h);
}

extern "C" void PlayerActor_NetSitDownSide2(Obj *o, s32 a) {
    s32 x, z;
    s16 h;
    PlayerActor_SitDownSide2GetNetData(o->netData.bytes, &x, &z, &h);
    PlayerActor_RequestSitDownSide2At(o, &x, &z, &h, 6, a);
}

extern "C" void PlayerActor_EndSitDownSide2(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->u.z;
    (o)->setAngleY(&r->h);
}

extern "C" void PlayerActor_SitDownSide2Update(Obj *o) {
    V3 v;
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xc)) {
        if (Unk_ov004_022215a8_IsOne(gFieldSceneKind)) {
            Rec *r = (Rec *)&o->actionWork;
            s32 z = r->u.z;
            s32 y = o->position.y;
            s32 x = r->x;
            v.x = x;
            v.y = y;
            v.z = z;
            FtrMgr_PlaySeatSound1At(&v);
        }
    }
}

extern "C" void PlayerActor_SitDownSide2CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestSit(o, 0, 6, -1);
    }
}

extern "C" void PlayerActor_MainSitDownSide2(Obj *o) {
    PlayerActor_SitDownSide2Update(o);
    (o)->netFollowTransform();
    (o)->updateBodyCollider();
    PlayerActor_SitDownSide2CheckEnd(o);
}

extern "C" void PlayerActor_SitDownSide1SetArgs(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" s32 PlayerActor_RequestSitDownSide1(Obj *o, u32 a, u32 b) {
    Msg m;
    V3 v;
    ((PlayerActionRequest *)(&m))->assign(0x27, a, b);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
    PlayerActor_SitDownSide1SetArgs((PlayerSitDownArgs *)m.unk_0c_b, v.x, v.z, (s16)(o->rotY - 0x4000));
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" s32 PlayerActor_RequestSitDownSide1At(Obj *o, s32 *x, s32 *z, s16 *h, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x27, a, *(s16 *)&b);
    PlayerActor_SitDownSide1SetArgs((PlayerSitDownArgs *)m.unk_0c_b, *x, *z, *h);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SitDownSide1SetWork(Tgt *t, s32 x, s32 z, s16 h) {
    t->x = x;
    t->z = z;
    t->h = h;
}

extern "C" void PlayerActor_SitDownSide1SetNetData(u8 *p, s32 x, s32 z, s16 h) {
    NetBuf_PackPair20(p, x, z);
    NetBuf_WriteS16B((u8 *)p + 5, h);
}

extern "C" void PlayerActor_SitDownSide1GetNetData(u8 *p, s32 *x, s32 *z, s16 *h) {
    NetBuf_UnpackPair20(p, (u32 *)x, (u32 *)z);
    *h = NetBuf_ReadS16B((u8 *)p + 5);
}

extern "C" void PlayerActor_SetupSitDownSide1(Obj *o, Obj *arg) {
    s16 h;
    (o)->startAnimOnce(0x23, 0, 0);
    (o)->playSe(0x4c5);
    V3 *p = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    h = (s16)p->z;
    s32 z = p->y;
    s32 y = *(volatile s32 *)&o->position.y;
    s32 x = *(s32 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    V3c v(x, y, z);
    PlayerActor_SitDownSide1SetWork((Tgt *)&o->actionWork, x, z, h);
    PlayerActor_SitDownSide1SetNetData(o->netData.bytes, v.x, v.z, h);
}

extern "C" void PlayerActor_NetSitDownSide1(Obj *o, s32 a) {
    s32 x, z;
    s16 h;
    PlayerActor_SitDownSide1GetNetData(o->netData.bytes, &x, &z, &h);
    PlayerActor_RequestSitDownSide1At(o, &x, &z, &h, 6, a);
}

extern "C" void PlayerActor_EndSitDownSide1(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    V3 *p = (V3 *)&o->position;
    p->x = r->x;
    p->z = r->u.z;
    (o)->setAngleY(&r->h);
}

extern "C" void PlayerActor_SitDownSide1Update(Obj *o) {
    V3 v;
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xc)) {
        if (Unk_ov004_022215a8_IsOne(gFieldSceneKind)) {
            Rec *r = (Rec *)&o->actionWork;
            s32 z = r->u.z;
            s32 y = o->position.y;
            s32 x = r->x;
            v.x = x;
            v.y = y;
            v.z = z;
            FtrMgr_PlaySeatSound1At(&v);
        }
    }
}

extern "C" void PlayerActor_SitDownSide1CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestSit(o, 0, 6, -1);
    }
}

extern "C" void PlayerActor_MainSitDownSide1(Obj *o) {
    PlayerActor_SitDownSide1Update(o);
    (o)->netFollowTransform();
    (o)->updateBodyCollider();
    PlayerActor_SitDownSide1CheckEnd(o);
}

extern "C" s32 PlayerActor_RequestSit(Obj *o, u32 a, u32 b, u32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x28, b, c);
    m.unk_0c_h = a;
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupSit(Obj *o, Obj *arg) {
    u16 *r2 = (u16 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    if (o->animId != 0x24) {
        (o)->startAnim(0x24, *r2, 0);
    }
    Rec *r = (Rec *)&o->actionWork;
    r->x = o->actorFlags;
    r->u.flag = 0;
}

extern "C" s32 PlayerActor_NetSit(Obj *o, s32 a) {
    return PlayerActor_RequestSit(o, 0, 6, a);
}

extern "C" void PlayerActor_EndSit(Obj *o) {
    o->actorFlags = ((Rec *)&o->actionWork)->x;
}

extern "C" void PlayerActor_SitUpdateAnim(Obj *o) {
    u8 *p;
    if (Math_IsInRange(Bgm_GetCurrent(), 0x63, 0xab) != 0 && (p = (u8 *)Snd_GetBeatState()) != 0 && (s8)p[3] != 1) {
        Sub *sb = &o->bodyModel;
        Rec *r = (Rec *)&o->actionWork;
        if (r->u.flag == 0) {
            r->u.flag = 1;
            if (((Bits *)&((AnimFrameCtrl &)*sb).curFrame)->mid != 0) {
                ((JointBlend *)(&(JointBlend &)*sb))->start(0xa);
            }
        }
        *(u32 *)&((AnimFrameCtrl &)*sb).curFrame = 0;
        ((AnimFrameCtrl &)*sb).frameStep = *(s32 *)(p + 0xc);
        ((TwoLayerAnimModel *)(sb))->updateLayers();
        ((AnimFrameCtrl &)*sb).frameStep = 0x1000;
        o->actorFlags = 0;
    } else {
        (o)->advanceAnim();
    }
}

extern "C" void PlayerActor_SitReadInput(Obj *o) {
    if ((o)->getInputMagnitude() > 0) {
        s32 t = (o)->getInputAngle();
        switch (Math_AngleToDir4((s16)(o->rotY - t))) {
        case 0:
            if (FtrMgr_CanStepForward((V3 *)&o->position, o->rotY)) {
                PlayerActor_RequestStandUpCheck(o, 0, 6, -1);
            }
            break;
        case 3:
            if (FtrMgr_CanStepSidePlus90((V3 *)&o->position, o->rotY)) {
                PlayerActor_RequestStandUpCheck(o, 1, 6, -1);
            }
            break;
        case 1:
            if (FtrMgr_CanStepSideMinus90((V3 *)&o->position, o->rotY)) {
                PlayerActor_RequestStandUpCheck(o, 2, 6, -1);
            }
            break;
        }
    }
}

extern "C" void PlayerActor_MainSit(Obj *o) {
    if (Unk_ov004_022215a8_IsOne(gFieldSceneKind)) {
        FtrMgr_PlaySeatSound0At((V3 *)&o->position);
    }
    PlayerActor_SitUpdateAnim(o);
    (o)->submitSceneCollider();
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_SitReadInput(o);
    } else {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
    }
}

extern "C" s32 PlayerActor_RequestStandUpSide2(Obj *o, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x29, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}


}  // namespace ns_022215a8

namespace ns_02220c78 {

struct StorageOpenWork {
    u32 targetX;
    u32 targetZ;
    s16 targetAngle;
    u8 storageKind;
    u8 unk_0b;
    u8 seatSide;
};

struct Unk_ov004_02220c78_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

typedef PlayerActor Obj;
typedef Unk_ov004_02220c78_Bits Bits;
typedef VecFx32 V3;
typedef StorageOpenWork Rec;
typedef PlayerActionRequest Msg;

extern "C" {
extern void *gCommManager;



s32 PlayerActor_RequestStorageHold(Obj *o, u32 a, s32 b, s32 c);
void PlayerActor_StorageOpenCheckEnd(Obj *o);
void PlayerActor_StorageOpenStartAnim(Obj *o);
void PlayerActor_StorageOpenMove(Obj *o);
void PlayerActor_StorageOpenSetWork(Rec *r, u8 d, u32 a, u32 b, s16 c);
s32 PlayerActor_RequestStorageOpen(Obj *o, u8 b, u32 x, u32 y, s16 c, s32 d, s32 e);
void PlayerActor_StandUpFrontCheckEnd(Obj *o);
void PlayerActor_StandUpFrontMoveRemote(Obj *o);
void PlayerActor_StandUpFrontUpdateAnim(Obj *o);
void PlayerActor_StandUpFrontSetWork(V3 *d, V3 *s);
s32 PlayerActor_RequestStandUpFront(Obj *o, s32 a, s32 b);
void PlayerActor_StandUpCheckResult(Obj *o);
void PlayerActor_StandUpCheckTurn(Obj *o);
void PlayerActor_StandUpCheckSetWork(V3 *d, V3 *s, u8 b);
void PlayerActor_StandUpSide1CheckEnd(Obj *o);
s32 PlayerActor_RequestStandUpSide1(Obj *o, s32 a, s32 b);
void PlayerActor_StandUpSide2CheckEnd(Obj *o);
}

static inline BOOL Unk_ov004_02220e48_IsZero(u32 v) {
    return v == 0 ? TRUE : FALSE;
}

static inline BOOL Unk_ov004_022211ac_Flag() {
    if (gFieldSceneKind == 1) return TRUE;
    return FALSE;
}

extern "C" void PlayerActor_SetupStandUpSide2(Obj *o) {
    s32 t = o->rotY;
    t -= 0x4000;
    *(u16 *)&o->actionWork = t;
    (o)->startAnimOnce(0x26, 0, 0);
    (o)->playSe(0x4c9);
}

extern "C" s32 PlayerActor_NetStandUpSide2(Obj *o, s32 a) {
    return PlayerActor_RequestStandUpSide2(o, 6, a);
}

extern "C" void PlayerActor_EndStandUpSide2(Obj *o) {
    (o)->setAngleY((s16 *)&o->actionWork);
}

extern "C" void PlayerActor_StandUpSide2CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestStandUpFront(o, 6, -1);
    }
}

extern "C" void PlayerActor_MainStandUpSide2(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_StandUpSide2CheckEnd(o);
}

extern "C" s32 PlayerActor_RequestStandUpSide1(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2a, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupStandUpSide1(Obj *o) {
    s32 t = o->rotY;
    t += 0x4000;
    *(u16 *)&o->actionWork = t;
    (o)->startAnimOnce(0x25, 0, 0);
    (o)->playSe(0x4c9);
}

extern "C" s32 PlayerActor_NetStandUpSide1(Obj *o, s32 a) {
    return PlayerActor_RequestStandUpSide1(o, 6, a);
}

extern "C" void PlayerActor_EndStandUpSide1(Obj *o) {
    (o)->setAngleY((s16 *)&o->actionWork);
}

extern "C" void PlayerActor_StandUpSide1CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestStandUpFront(o, 6, -1);
    }
}

extern "C" void PlayerActor_MainStandUpSide1(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_StandUpSide1CheckEnd(o);
}

extern "C" void PlayerActor_StandUpCheckSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestStandUpCheck(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2b, b, c);
    PlayerActor_StandUpCheckSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_StandUpCheckSetWork(V3 *d, V3 *s, u8 b) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
    ((u8 *)d)[0xc] = b;
}

extern "C" void PlayerActor_SetupStandUpCheck(Obj *o, Msg *m) {
    V3 pos;
    V3 tmp;
    s16 t;
    u8 idx = m->unk_0c_b[0];
    t = o->rotY - sSeatSideAngles[idx];
    PlayerActor_OffsetByAngle(&pos, o, &o->position, &t, (s32 *)&sAct12Pos[3]);
    tmp.x = pos.x;
    tmp.y = pos.y;
    tmp.z = pos.z;
    PlayerActor_StandUpCheckSetWork((V3 *)&o->actionWork, &tmp, idx);
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        SpotSync_RequestStandUpSpot((s32 *)&pos);
    }
}

extern "C" void PlayerActor_NetStandUpCheck() {
}

extern "C" void PlayerActor_StandUpCheckTurn(Obj *o) {
    V3 v;
    V3 *s = (V3 *)&o->actionWork;
    v.x = s->x;
    v.y = s->y;
    v.z = s->z;
    (o)->setSubCollider(&v, 0x1000, 0x1000);
    ((ActorCollider *)(&o->subCollider))->submit();
}

extern "C" void PlayerActor_StandUpCheckResult(Obj *o) {
    Rec *r;
    switch (SpotSync_GetStandUpSpotResult()) {
    case 0:
        return;
    case 1:
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        break;
    case 2:
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        PlayerActor_RequestSit(o, 3, 6, -1);
        return;
    }
    r = (Rec *)&o->actionWork;
    if (o->subCollider.isHit != 0) {
        PlayerActor_RequestSit(o, 3, 6, -1);
        return;
    }
    switch (r->seatSide) {
    case 0:
        PlayerActor_RequestStandUpFront(o, 6, -1);
        break;
    case 1:
        PlayerActor_RequestStandUpSide1(o, 6, -1);
        break;
    case 2:
        PlayerActor_RequestStandUpSide2(o, 6, -1);
        break;
    }
}

extern "C" void PlayerActor_MainStandUpCheck(Obj *o) {
    if (Unk_ov004_022211ac_Flag()) {
        FtrMgr_PlaySeatSound0At((V3 *)&o->position);
    }
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_StandUpCheckResult(o);
    PlayerActor_StandUpCheckTurn(o);
}

extern "C" s32 PlayerActor_RequestStandUpFront(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2c, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_StandUpFrontSetWork(V3 *d, V3 *s) {
    d->x = s->x;
    d->y = s->y;
    d->z = s->z;
}

extern "C" void PlayerActor_SetupStandUpFront(Obj *o) {
    V3 v;
    (o)->startAnimOnce(0x27, 0, 0);
    PlayerActor_OffsetByAngle(&v, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
    PlayerActor_StandUpFrontSetWork((V3 *)&o->actionWork, &v);
    if (Unk_ov004_022211ac_Flag()) {
        FtrMgr_PlaySeatSound2At((V3 *)&o->position);
    }
}

extern "C" void PlayerActor_NetStandUpFront(Obj *o, s32 a) {
    s32 t = o->action;
    if ((u32)(t - 0x29) <= 1) return;
    if (t == 0x2c) {
        o->netSeq = a;
    } else {
        PlayerActor_RequestStandUpFront(o, 6, a);
    }
}

extern "C" void PlayerActor_EndStandUpFront(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        SpotSync_Release(0);
    }
    V3 *s = (V3 *)&o->actionWork;
    V3 *d = (V3 *)&o->position;
    o->position.x = s->x;
    d->y = s->y;
    d->z = s->z;
}

extern "C" void PlayerActor_StandUpFrontUpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xd)) {
        (o)->playSe(0x4c6);
    }
}

extern "C" void PlayerActor_StandUpFrontMoveRemote(Obj *o) {
    V3 v;
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        (o)->updateBodyCollider();
    } else {
        V3 *s = (V3 *)&o->actionWork;
        v.x = s->x;
        v.y = s->y;
        v.z = s->z;
        (o)->setBodyColliderAt((Unk_020107c8_Blk *)(void *)(&v), (u32 *)((u32 *)&o->action));
        ((ActorCollider *)(&o->bodyCollider))->submit();
    }
}

extern "C" void PlayerActor_StandUpFrontCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(0, 1, -1);
    }
}

extern "C" void PlayerActor_MainStandUpFront(Obj *o) {
    PlayerActor_StandUpFrontUpdateAnim(o);
    PlayerActor_StandUpFrontMoveRemote(o);
    PlayerActor_StandUpFrontCheckEnd(o);
}

extern "C" void PlayerActor_StorageOpenSetArgs(void *r, u8 d, u32 a, u32 b, s16 c) {
    Rec *p = (Rec *)r;
    p->storageKind = d;
    p->targetX = a;
    p->targetZ = b;
    p->targetAngle = c;
}

extern "C" s32 PlayerActor_RequestStorageOpen(Obj *o, u8 b, u32 x, u32 y, s16 c, s32 d, s32 e) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2d, d, *(s16 *)&e);
    PlayerActor_StorageOpenSetArgs(m.unk_0c_b, b, x, y, c);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_StorageOpenSetWork(Rec *r, u8 d, u32 a, u32 b, s16 c) {
    r->storageKind = d;
    r->targetX = a;
    r->targetZ = b;
    r->targetAngle = c;
}

extern "C" void PlayerActor_StorageOpenSetNetData(u8 *self, u8 b, s32 x, s32 y, s16 h) {
    self[7] = b;
    NetBuf_PackPair20(self, x, y);
    NetBuf_WriteS16B(self + 5, h);
}

extern "C" void PlayerActor_StorageOpenGetNetData(u8 *self, u8 *o1, s32 *o2, s32 *o3, u16 *o4) {
    u8 b = self[7];
    *o1 = b;
    NetBuf_UnpackPair20(self, (u32 *)o2, (u32 *)o3);
    *o4 = NetBuf_ReadS16B(self + 5);
}

extern "C" void PlayerActor_SetupStorageOpen(Obj *o, Msg *m) {
    Rec *p = (Rec *)m->unk_0c_b;
    u8 d = p->storageKind;
    u32 a = *(u32 *)m->unk_0c_b;
    u32 b = p->targetZ;
    s16 c = p->targetAngle;
    PlayerActor_StorageOpenSetWork((Rec *)&o->actionWork, d, a, b, c);
    PlayerActor_StorageOpenSetNetData(o->netData.bytes, d, a, b, c);
    ((AnimFrameCtrl &)o->bodyModel).playMode = 1;
}

extern "C" void PlayerActor_NetStorageOpen(Obj *o, s32 a) {
    u8 b;
    u16 h;
    s32 x, y;
    PlayerActor_StorageOpenGetNetData(o->netData.bytes, &b, &x, &y, &h);
    PlayerActor_RequestStorageOpen(o, b, x, y, (s16)h, 6, a);
}

extern "C" void PlayerActor_StorageOpenMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    PlayerActor_StepTowardPose(o, r->targetX, r->targetZ, r->targetAngle);
}

extern "C" void PlayerActor_StorageOpenStartAnim(Obj *o) {
    s32 k;
    if (o->animId == 0x1a || o->animId == 0) {
        if (Unk_ov004_02220e48_IsZero(((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid) < 6) {
            (o)->setActionFlag(2);
            switch (((Rec *)&o->actionWork)->storageKind) {
            case 0:
                k = 0x3b;
                break;
            case 1:
                k = 0x3c;
                break;
            case 2:
                k = 0x3d;
                break;
            }
            (o)->startAnimOnce(k, 3, 0);
            if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
                FtrMgr_OnRemoteStorageOpen();
            }
        }
    }
}

extern "C" void PlayerActor_StorageOpenCheckEnd(Obj *o) {
    if (o->animId == 0x1a) return;
    if (o->animId == 0) return;
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        (o)->clearActionFlag(2);
        PlayerActor_RequestStorageHold(o, ((Rec *)&o->actionWork)->storageKind, 6, -1);
    }
}

extern "C" void PlayerActor_MainStorageOpen(Obj *o) {
    PlayerActor_StorageOpenMove(o);
    PlayerActor_StorageOpenStartAnim(o);
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_StorageOpenCheckEnd(o);
}

extern "C" void PlayerActor_StorageHoldSetArgs(u8 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestStorageHold(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2e, b, c);
    PlayerActor_StorageHoldSetArgs(m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_StorageHoldSetWork(u32 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_StorageHoldSetNetData(u8 *p, u32 v) {
    *p = v;
}

extern "C" void PlayerActor_StorageHoldGetNetData(u8 *p, u8 *out) {
    *out = *p;
}

extern "C" void PlayerActor_SetupStorageHold(Obj *o, Msg *m) {
    u32 t = m->unk_0c_b[0];
    s32 k;
    PlayerActor_StorageHoldSetWork((u32 *)&o->actionWork, t);
    PlayerActor_StorageHoldSetNetData(o->netData.bytes, t);
    switch (m->unk_0c_b[0]) {
    case 0:
        k = 0x3b;
        break;
    case 1:
        k = 0x3c;
        break;
    case 2:
        k = 0x3d;
        break;
    }
    (o)->startAnim(k, 3, 0);
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
        FtrMgr_OnRemoteStorageHold();
    }
}

extern "C" void PlayerActor_NetStorageHold(Obj *o, s32 a) {
    u8 b;
    PlayerActor_StorageHoldGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestStorageHold(o, b, 6, a);
}

extern "C" void PlayerActor_MainStorageHold(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
}

extern "C" void PlayerActor_StorageCloseSetArgs(u32 *p, u32 v) {
    *p = v;
}

extern "C" s32 PlayerActor_RequestStorageClose(Obj *o, u32 a, s32 b, s32 c) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x2f, b, c);
    PlayerActor_StorageCloseSetArgs((u32 *)m.unk_0c_b, a);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_StorageCloseSetWork(u32 *a, u32 v) {
    *a = v;
}

extern "C" void PlayerActor_StorageCloseSetNetData(u8 *a, u32 v) {
    *a = v;
}

extern "C" void PlayerActor_StorageCloseGetNetData(u8 *a, u8 *b) {
    *b = *a;
}


}  // namespace ns_02220c78

namespace ns_02220314 {

struct LeaveRoomWork {
    s32 leaveMode;
    s16 targetAngle;
    s32 targetX;
    s32 targetZ;
    u8 entryStep;
};

struct PlayerLeaveRoomArgs {
    s32 kind;
    s16 angle;
};

typedef PlayerAct30Args PlayerAct36Args;  // (the same arguments)

struct Unk_ov004_02220314_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

static inline BOOL Unk_ov004_02220314_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef LeaveRoomWork Rec;
typedef PlayerActionRequest Msg;
typedef TalkMsgRequest Sec;
typedef Unk_ov004_02220314_Bits Bits;

extern "C" {
extern void *gCommManager;

s32 Scene_GetCurrent(void);
void Vec_Sub(V3 *o, V3 *a, V3 *b);
s32 Vec_MagXZ(V3 *v);
s32 func_01ffcb0c(s32 a, s32 b);
s32 FX_Div(s32 a, s32 b);
void PlayerActor_StepAlpha(Obj *o, u32 a, u32 b);

s32 PlayerActor_NetAct41(Obj *o, s32 a);
void PlayerActor_SetupAct41(Obj *o);
s32 PlayerActor_RequestAct41(Obj *o, s32 a, s32 b);
void PlayerActor_MainLeaveRoom(Obj *o);
void PlayerActor_LeaveRoomCheckArrive(Obj *o);
void PlayerActor_LeaveRoomUpdateHeight(Obj *o);
void PlayerActor_LeaveRoomUpdateAnim(Obj *o);
void PlayerActor_LeaveRoomMove(Obj *o);
void PlayerActor_NetLeaveRoom(Obj *o);
void PlayerActor_SetupLeaveRoom(Obj *o, PlayerActionRequest *m);
void PlayerActor_LeaveRoomSetWork(Rec *r, s32 a, s32 b, s32 c, s32 d, bool e);
s32 PlayerActor_RequestLeaveRoom(Obj *o, s32 a, s32 b);
void PlayerActor_MainAct37(Obj *o);
void PlayerActor_Act37CheckEnd(Obj *o);
void PlayerActor_NetAct37(Obj *o);
void PlayerActor_SetupAct37(Obj *o);
s32 PlayerActor_RequestAct37(Obj *o, s32 a, s32 b);
void PlayerActor_MainAct36(Obj *o);
void PlayerActor_Act36CheckEnd(Obj *o);
void PlayerActor_Act36UpdateAnim(Obj *o);
void PlayerActor_NetAct36(Obj *o);
void PlayerActor_SetupAct36(Obj *o, PlayerActionRequest *m);
s32 PlayerActor_RequestAct36(Obj *o, u16 *a, s32 b, s32 c, s32 d, Character *e, s32 f, s16 g);
void PlayerActor_MainStorageClose(Obj *o);
void PlayerActor_StorageCloseCheckEnd(Obj *o);
void PlayerActor_NetStorageClose(Obj *o, s32 a);
void PlayerActor_SetupStorageClose(Obj *o, PlayerActionRequest *m);
}

extern "C" void PlayerActor_SetupStorageClose(Obj *o, PlayerActionRequest *m) {
    s32 t = ((PlayerLeaveRoomArgs *)m->unk_0c_b)->kind;
    s32 k;
    PlayerActor_StorageCloseSetWork((u32 *)&o->actionWork, t);
    PlayerActor_StorageCloseSetNetData(o->netData.bytes, (u8)t);
    switch (((PlayerLeaveRoomArgs *)m->unk_0c_b)->kind) {
    case 0:
        k = 0x38;
        break;
    case 1:
        k = 0x39;
        break;
    case 2:
        k = 0x3a;
        break;
    }
    (o)->startAnimOnce(k, 3, 0);
    u32 mid = ((Bits *)&((AnimFrameCtrl &)o->bodyModel).numFrames)->mid;
    ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->setup(mid, 3, 0x1000, (u16)(mid - 1));
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
        FtrMgr_OnRemoteStorageClose();
    }
}

extern "C" void PlayerActor_NetStorageClose(Obj *o, s32 a) {
    u8 b;
    PlayerActor_StorageCloseGetNetData(o->netData.bytes, &b);
    PlayerActor_RequestStorageClose(o, b, 6, a);
}

extern "C" void PlayerActor_StorageCloseCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        (o)->startAnim(0, 3, 3);
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
            o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        }
    }
}

extern "C" void PlayerActor_MainStorageClose(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_StorageCloseCheckEnd(o);
}

extern "C" void PlayerActor_Act36SetArgs(PlayerAct36Args *p, s32 a, u32 b, s32 c, Character *d) {
    p->kind = a;
    p->nextMode = b;
    p->variant = c;
    p->partner = d;
}

extern "C" s32 PlayerActor_RequestAct36(Obj *o, u16 *a, s32 b, s32 c, s32 d, Character *e, s32 f, s16 g) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x36, f, g);
    u16 *p = &o->shownItem;
    *p = *a;
    o->actionItem = *p;
    PlayerActor_Act36SetArgs(&m.act30, b, c, d, e);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct36(Obj *o, PlayerActionRequest *m) {
    PlayerAct36Args *p = (PlayerAct36Args *)m->unk_0c_b;
    HandOverItem_Begin(&o->actionItem, ((PlayerAct36Args *)m->unk_0c_b)->kind, p->nextMode, p->variant, o, p->partner);
    (o)->startAnimOnce(0x2e, 3, 0);
    HandOverItem_RequestMode(1, o);
    if (Scene_GetCurrent() == 9) {
        (o)->playSe(0x4f);
    }
}

extern "C" void PlayerActor_NetAct36(Obj *o) {
}

extern "C" void PlayerActor_Act36UpdateAnim(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(8)) {
        if (Scene_GetCurrent() == 9) {
            (o)->playSe(0x63);
        }
    }
    (o)->advanceAnim();
}

extern "C" void PlayerActor_Act36CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestAct10(3, 5, -1);
    }
}

extern "C" void PlayerActor_MainAct36(Obj *o) {
    PlayerActor_Act36UpdateAnim(o);
    (o)->updateBodyCollider();
    PlayerActor_Act36CheckEnd(o);
}

extern "C" s32 PlayerActor_RequestAct37(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x37, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct37(Obj *o) {
    (o)->startAnimOnce(0x2e, 3, 0);
    u32 mid = ((Bits *)&((AnimFrameCtrl &)o->bodyModel).numFrames)->mid;
    ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->setup(mid, 3, 0x1000, (u16)(mid - 1));
    HandOverItem_RequestMode(HandOverItem_GetNextMode(), o);
    (o)->playSe(0x52);
}

extern "C" void PlayerActor_NetAct37(Obj *o) {
}

extern "C" void PlayerActor_Act37CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestAct10(3, 5, -1);
        u16 v;
        HandOverItem_GetItem(&v);
        if (v == 0x1379) {
            o->pendingAct76Kind = 4;
        }
        HandOverItem_End(o);
    }
}

extern "C" void PlayerActor_MainAct37(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_Act37CheckEnd(o);
}

extern "C" void PlayerActor_LeaveRoomSetArgs(void *p, s32 a, s16 b) {
    *(s32 *)p = a;
    *(s16 *)((u8 *)p + 4) = b;
}

extern "C" s32 PlayerActor_RequestLeaveRoom(Obj *o, s32 a, s32 b) {
    s32 h = o->exitIndex;
    if (h != -1) {
        s16 x;
        u32 y;
        if (SceneExit_GetDoor(Scene_GetWarpRequest(), h, &y, &x) == 0) {
            return 0;
        }
        Msg m;
        ((PlayerActionRequest *)(&m))->assign(0x40, a, b);
        PlayerActor_LeaveRoomSetArgs((VecFx32 *)m.unk_0c_b, y, x);
        s32 r = (o)->pushRequest(&m);
        return r;
    }
    return 0;
}

extern "C" void PlayerActor_LeaveRoomSetWork(Rec *r, s32 a, s32 b, s32 c, s32 d, bool e) {
    r->leaveMode = a;
    r->targetAngle = b;
    r->targetX = c;
    r->targetZ = d;
    r->entryStep = e;
}

extern "C" void PlayerActor_SetupLeaveRoom(Obj *o, PlayerActionRequest *m) {
    (o)->setActionFlag(3);
    PlayerLeaveRoomArgs *pay = (PlayerLeaveRoomArgs *)m->unk_0c_b;
    Rec *r = (Rec *)&o->actionWork;
    V3 v;
    SceneExit_SnapPos(Scene_GetWarpRequest(), o->exitIndex, &v, (V3 *)&o->position);
    s32 mode = pay->kind;
    s32 ang = pay->angle;
    if (mode == 2) {
        switch (Math_AngleToDir4(ang)) {
        case 2:
            v.z += 0x1000;
            break;
        case 0:
            v.z -= 0x1000;
            break;
        case 3:
            v.x += 0x1000;
            break;
        case 1:
            v.x -= 0x1000;
            break;
        }
    } else if (mode == 0) {
        ang = -0x8000;
        v.x = o->position.x;
        v.z = o->position.z;
    }
    bool flag = 0;
    if (RoomEntry_IsExclusiveExit(o->exitIndex)) {
        flag = 1;
    }
    PlayerActor_LeaveRoomSetWork(r, mode, ang, v.x, v.z, flag);
    if (flag == 0) {
        if (mode == 1) {
            (o)->startAnimOnce(0x3e, 3, 0);
        } else {
            (o)->startAnimOnce(0x3f, 3, 0);
        }
    } else {
        (o)->setActionFlag(0x18);
        (o)->startAnimOnce(1, 3, 0);
    }
}

extern "C" void PlayerActor_NetLeaveRoom(Obj *o) {
}

extern "C" void PlayerActor_LeaveRoomMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    PlayerActor_StepTowardPose(o, r->targetX, r->targetZ, r->targetAngle);
}

extern "C" void PlayerActor_LeaveRoomUpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (o->animId == 1) {
        V3 v;
        Vec_Sub(&v, (V3 *)&o->position, (V3 *)&o->prevPosition);
        s32 t = func_01ffcb0c(Vec_MagXZ(&v), 0x3ae1) << 2;
        if (t <= (s32)((AnimFrameCtrl &)o->bodyModel).numFrames) {
            ((AnimFrameCtrl &)o->bodyModel).frameStep = t;
        }
        (o)->updateFootstepFx();
        if (t == 0) {
            (o)->startAnim(0, 3, 0);
        }
    } else if (o->animId != 0) {
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xb) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x10)) {
            (o)->playFootstepSe();
        }
    }
}

extern "C" void PlayerActor_LeaveRoomUpdateHeight(Obj *o) {
    VecFx32 *p = &o->position;
    {
        CollisionState loc;
        s32 ang = o->rotY;
        CollisionState *sel;
        if (((Unk_02007694 *)(void *)(o))->keepsBgCheckWork(o->action) == 0) {
            sel = &loc;
        } else {
            sel = &o->bgCheckWork;
        }
        Collision_Move(sel, p, &o->prevPosition, ang, 0xfd7, o, 0xf);
    }
    p->y = Ground_GetDefaultY();
    Rec *r = (Rec *)&o->actionWork;
    s32 m = r->leaveMode;
    if ((u32)(m - 1) <= 1) {
        VecFx32Ctor v(r->targetX, p->y, r->targetZ);
        if (Vec_DistXZ((V3 *)&v, p) < 0x1000) {
            s32 d = FX_Div(0x1000 - Vec_DistXZ((V3 *)&v, p), 0x1000) * 6;
            if (m == 1) {
                p->y = p->y + (d >> 5);
            } else {
                p->y = p->y - (d >> 5);
            }
        }
    }
}

extern "C" void PlayerActor_LeaveRoomCheckArrive(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    u8 *st = &r->entryStep;
    if (*st == 0) {
        V3 *p = (V3 *)&o->position;
        if (p->x == r->targetX && p->z == r->targetZ && r->targetAngle == o->rotY) {
            if (r->leaveMode == 1) {
                PlayerActor_RequestAct41(o, 6, -1);
            } else {
                PlayerActor_RequestAct42(o, 6, -1);
            }
        }
        s32 t = o->animId;
        if (t == 1) return;
        if (t == 0x3e) {
            if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 6) {
                PlayerActor_StepAlpha(o, 0, 6);
            }
        }
        if (Unk_ov004_02220314_IsZero(gScreenTransition.state)) return;
        if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 5) {
            SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
        }
    } else {
        RoomEntryRequest *p = (RoomEntryRequest *)RoomEntry_GetRequest();
        switch (*st) {
        case 1:
            if (!RoomEntry_Request(o->exitIndex)) break;
            *st = *st + 1;
        case 2: {
            s32 c = RoomEntryRequest_GetResult(p);
            switch (c) {
            case 0:
                break;
            case 2:
                if (RoomEntryRequest_GetDoorKind(p) == 0) {
                    SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
                } else {
                    *st = 0;
                    (o)->startAnimOnce(0x3e, 3, 0);
                }
                break;
            case 1: {
                ((Character *)(void *)(o))->attachTalkRequest(o);
                (o)->setActionFlag(0x11);
                {
                    Sec &s = *o;
                    ((MsgRequest *)(void *)(&s))->setFileName((const char *)(sRoomErrorMsgFile));
                }
                o->msgIndex = 0x15;
                o->window->nextState = 1;
                *st = *st + 1;
                break;
            }
            }
            break;
        }
        case 3: {
            TalkWindowState *q = o->window;
            if (q != 0) {
                if (q->state != 0) {
                    *st = *st + 1;
                }
            }
            break;
        }
        case 4: {
            TalkWindowState *q = o->window;
            if (q != 0) {
                if (q->state == 0) {
                    struct { u32 pad; V3 a; V3 b; } l;
                    V3 *v = (V3 *)RoomEntryRequest_GetRetreatPos(p);
                    l.a.x = v->x;
                    l.a.y = v->y;
                    l.a.z = v->z;
                    Sec *s = o;
                    ((Character *)(void *)(o))->detachTalkRequest(s);
                    (o)->clearActionFlag(0x11);
                    o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
                    l.b.x = l.a.x;
                    l.b.y = l.a.y;
                    l.b.z = l.a.z;
                    (o)->requestWalkTo((VecFx32Ctor *)(void *)(&l.b), 0x333, 5, -1);
                }
            }
            break;
        }
        }
    }
}

extern "C" void PlayerActor_MainLeaveRoom(Obj *o) {
    PlayerActor_LeaveRoomMove(o);
    PlayerActor_LeaveRoomUpdateAnim(o);
    PlayerActor_LeaveRoomUpdateHeight(o);
    (o)->updateBodyCollider();
    PlayerActor_LeaveRoomCheckArrive(o);
}

extern "C" s32 PlayerActor_RequestAct41(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x41, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct41(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        if (o->animId != 0x3e) {
            (o)->startAnim(0x3e, 3, 0);
        }
    } else {
        (o)->startAnim(0, 3, 3);
    }
}

extern "C" s32 PlayerActor_NetAct41(Obj *o, s32 a) {
    return PlayerActor_RequestAct41(o, 9, a);
}


}  // namespace ns_02220314

namespace ns_0221fa00 {

struct HaircutWork {
    u8 hairStyle;
    u8 hairColor;
    u8 hairApplied;
};

struct PlayerHaircutArgs {
    u8 hairStyle;
    u8 hairColor;
};

struct Unk_ov004_0221fa00_Bits {
    u32 lo : 12;
    u32 mid : 16;
    u32 hi : 4;
};

static inline BOOL Unk_ov004_0221fa00_IsZero(u8 v) {
    return v == 0 ? TRUE : FALSE;
}

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef HaircutWork Rec;
typedef PlayerActionRequest Msg;
typedef Unk_ov004_0221fa00_Bits Bits;

extern "C" {
extern void *gCommManager;

void PlayerActor_GetHat(u16 *p, Obj *o);
void *PlayerActor_GetPlayerData(void *p);
s32 PlayerActor_GetHairStyle(Obj *o);
s32 PlayerActor_GetHairColor(Obj *o);
s32 TalkRequest_FinishSceneEntry(void);
void PlayerActor_StepAlpha(Obj *o, u32 a, u32 b);

void PlayerActor_HaircutFinishUpdate(Obj *o);
void PlayerActor_EndHaircutFinish(Obj *o);
void PlayerActor_NetHaircutFinish(Obj *o);
void PlayerActor_SetupHaircutFinish(Obj *o, PlayerActionRequest *m);
s32 PlayerActor_RequestHaircutFinish(Obj *o, u32 a, u32 b, u32 c, s32 e);
void PlayerActor_MainHaircutCut(Obj *o);
void PlayerActor_HaircutCutCheckEnd(Obj *o);
void PlayerActor_NetHaircutCut(Obj *o);
void PlayerActor_SetupHaircutCut(Obj *o, PlayerActionRequest *m);
s32 PlayerActor_RequestHaircutCut(Obj *o, u32 a, u32 b, u32 c, s32 e);
void PlayerActor_MainHaircutStart(Obj *o);
void PlayerActor_HaircutStartCheckEnd(Obj *o);
void PlayerActor_NetHaircutStart(Obj *o);
void PlayerActor_SetupHaircutStart(Obj *o, PlayerActionRequest *m);
s32 PlayerActor_RequestHaircutStart(Obj *o, u32 a, u32 b, u32 c, s32 e);
void PlayerActor_MainAct44(Obj *o);
void PlayerActor_Act44CheckEnd(Obj *o);
void PlayerActor_Act44UpdateAnim(Obj *o);
s32 PlayerActor_NetAct44(Obj *o, s32 a);
void PlayerActor_SetupAct44(Obj *o);
s32 PlayerActor_RequestAct44(Obj *o, s32 a, s32 b);
void PlayerActor_MainAct43(Obj *o);
void PlayerActor_Act43CheckEnd(Obj *o);
void PlayerActor_Act43UpdateAnim(Obj *o);
s32 PlayerActor_NetAct43(Obj *o, s32 a);
void PlayerActor_SetupAct43(Obj *o);
s32 PlayerActor_RequestAct43(Obj *o, s32 a, s32 b);
void PlayerActor_MainAct42(Obj *o);
void PlayerActor_Act42CheckSceneChange(Obj *o);
void PlayerActor_Act42FadeOut(Obj *o);
void PlayerActor_Act42UpdateRemote(Obj *o, s32 a);
void PlayerActor_Act42UpdateAnim(Obj *o);
s32 PlayerActor_NetAct42(Obj *o, s32 a);
void PlayerActor_SetupAct42(Obj *o);
void PlayerActor_MainAct41(Obj *o);
void PlayerActor_Act41CheckSceneChange(Obj *o);
void PlayerActor_Act41FadeOut(Obj *o);
void PlayerActor_Act41UpdateRemote(Obj *o, s32 a);
void PlayerActor_Act41UpdateAnim(Obj *o);
}

extern "C" void PlayerActor_Act41UpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xb) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x10)) {
        (o)->playFootstepSe();
    }
}

extern "C" void PlayerActor_Act41UpdateRemote(Obj *o, s32 a) {
    if (a != 0) {
        if (o->animId != 0x3e) {
            (o)->startAnimOnce(0x3e, 3, 0);
        }
    }
    (o)->advanceAnim();
    if (o->animId == 0x3e) {
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xb) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x10)) {
            (o)->playFootstepSe();
        }
    }
}

extern "C" void PlayerActor_Act41FadeOut(Obj *o) {
    if (o->animId == 0x3e) {
        if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 6) {
            PlayerActor_StepAlpha(o, 0, 6);
        }
    }
}

extern "C" void PlayerActor_Act41CheckSceneChange(Obj *o) {
    if (Unk_ov004_0221fa00_IsZero(gScreenTransition.state)) return;
    if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 5) {
        SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
    }
}

extern "C" void PlayerActor_MainAct41(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_Act41UpdateAnim(o);
        PlayerActor_Act41FadeOut(o);
        PlayerActor_Act41CheckSceneChange(o);
    } else {
        PlayerActor_Act41UpdateRemote(o, (o)->netApproachTransform());
        PlayerActor_Act41FadeOut(o);
    }
}

extern "C" s32 PlayerActor_RequestAct42(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x42, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct42(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
        (o)->startAnim(0, 3, 3);
    }
}

extern "C" s32 PlayerActor_NetAct42(Obj *o, s32 a) {
    return PlayerActor_RequestAct42(o, 9, a);
}

extern "C" void PlayerActor_Act42UpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xb) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x10)) {
        (o)->playFootstepSe();
    }
}

extern "C" void PlayerActor_Act42UpdateRemote(Obj *o, s32 a) {
    if (a != 0) {
        if (o->animId != 0x3f) {
            (o)->startAnimOnce(0x3f, 3, 0);
        }
    }
    (o)->advanceAnim();
    if (o->animId == 0x3e) {
        if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xb) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x10)) {
            (o)->playFootstepSe();
        }
    }
}

extern "C" void PlayerActor_Act42FadeOut(Obj *o) {
    if (o->animId == 0x3f) {
        if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 9) {
            PlayerActor_StepAlpha(o, 0, 6);
        }
    }
}

extern "C" void PlayerActor_Act42CheckSceneChange(Obj *o) {
    if (Unk_ov004_0221fa00_IsZero(gScreenTransition.state)) return;
    if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 5) {
        SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
    }
}

extern "C" void PlayerActor_MainAct42(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_Act42UpdateAnim(o);
        PlayerActor_Act42FadeOut(o);
        PlayerActor_Act42CheckSceneChange(o);
    } else {
        PlayerActor_Act42UpdateRemote(o, (o)->netApproachTransform());
        PlayerActor_Act42FadeOut(o);
    }
}

extern "C" s32 PlayerActor_RequestAct43(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x43, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct43(Obj *o) {
    (o)->startAnimOnce(0x40, 0, 0);
    o->alpha = 0;
}

extern "C" s32 PlayerActor_NetAct43(Obj *o, s32 a) {
    return PlayerActor_RequestAct43(o, 9, a);
}

extern "C" void PlayerActor_Act43UpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xc) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x14)) {
        (o)->playFootstepSe();
    }
}

extern "C" void PlayerActor_Act43CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        (o)->nudgeForward();
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(3, 1, -1);
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            TalkRequest_FinishSceneEntry();
        }
    }
}

extern "C" void PlayerActor_MainAct43(Obj *o) {
    PlayerActor_Act43UpdateAnim(o);
    PlayerActor_StepAlpha(o, 0x1f, 6);
    (o)->updateBodyCollider();
    PlayerActor_Act43CheckEnd(o);
}

extern "C" s32 PlayerActor_RequestAct44(Obj *o, s32 a, s32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x44, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupAct44(Obj *o) {
    (o)->startAnimOnce(0x41, 0, 0);
    o->alpha = 0;
}

extern "C" s32 PlayerActor_NetAct44(Obj *o, s32 a) {
    return PlayerActor_RequestAct44(o, 9, a);
}

extern "C" void PlayerActor_Act44UpdateAnim(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(1) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(6) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0xc) || ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(0x14)) {
        (o)->playFootstepSe();
    }
}

extern "C" void PlayerActor_Act44CheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        (o)->nudgeForward();
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(3, 1, -1);
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            TalkRequest_FinishSceneEntry();
        }
    }
}

extern "C" void PlayerActor_MainAct44(Obj *o) {
    PlayerActor_Act44UpdateAnim(o);
    PlayerActor_StepAlpha(o, 0x1f, 6);
    (o)->updateBodyCollider();
    PlayerActor_Act44CheckEnd(o);
}

extern "C" s32 PlayerActor_RequestHaircutStart(Obj *o, u32 a, u32 b, u32 c, s32 e) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7a, c, *(s16 *)&e);
    PlayerHaircutArgs &q = *(PlayerHaircutArgs *)m.unk_0c_b;
    q.hairStyle = a;
    q.hairColor = b;
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupHaircutStart(Obj *o, PlayerActionRequest *m) {
    (o)->startAnimOnce(0x6d, 3, 0);
    PlayerHaircutArgs *p = (PlayerHaircutArgs *)m->unk_0c_b;
    u8 b = p->hairColor;
    Rec *r = (Rec *)&o->actionWork;
    r->hairStyle = p->hairStyle;
    r->hairColor = b;
}

extern "C" void PlayerActor_NetHaircutStart(Obj *o) {
}

extern "C" void PlayerActor_HaircutStartCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        Rec *r = (Rec *)&o->actionWork;
        PlayerActor_RequestHaircutCut(o, r->hairStyle, r->hairColor, 6, -1);
    } else if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 3) {
        if (PlayerActor_GetHairStyle(o)) {
            u16 loc[2];
            PlayerActor_GetHat(&loc[1], o);
            loc[0] = loc[1];
            s32 t = PlayerActor_GetHairColor(o);
            if ((o)->requestHatChange(loc, 0, t, 0)) {
                static_cast<PlayerData *>(PlayerActor_GetPlayerData(o))->setHairStyle(0);
            }
        }
    }
}

extern "C" void PlayerActor_MainHaircutStart(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_HaircutStartCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestHaircutCut(Obj *o, u32 a, u32 b, u32 c, s32 e) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7b, c, *(s16 *)&e);
    PlayerHaircutArgs &q = *(PlayerHaircutArgs *)m.unk_0c_b;
    q.hairStyle = a;
    q.hairColor = b;
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupHaircutCut(Obj *o, PlayerActionRequest *m) {
    (o)->startAnimOnce(0x6e, 3, 0);
    PlayerHaircutArgs *p = (PlayerHaircutArgs *)m->unk_0c_b;
    u8 b = p->hairColor;
    Rec *r = (Rec *)&o->actionWork;
    r->hairStyle = p->hairStyle;
    r->hairColor = b;
}

extern "C" void PlayerActor_NetHaircutCut(Obj *o) {
}

extern "C" void PlayerActor_HaircutCutCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        Rec *r = (Rec *)&o->actionWork;
        PlayerActor_RequestHaircutFinish(o, r->hairStyle, r->hairColor, 6, -1);
    }
}

extern "C" void PlayerActor_MainHaircutCut(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_HaircutCutCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestHaircutFinish(Obj *o, u32 a, u32 b, u32 c, s32 e) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7c, c, *(s16 *)&e);
    PlayerHaircutArgs &q = *(PlayerHaircutArgs *)m.unk_0c_b;
    q.hairStyle = a;
    q.hairColor = b;
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupHaircutFinish(Obj *o, PlayerActionRequest *m) {
    (o)->startAnimOnce(0x6f, 3, 0);
    PlayerHaircutArgs *p = (PlayerHaircutArgs *)m->unk_0c_b;
    u8 b = p->hairColor;
    Rec *r = (Rec *)&o->actionWork;
    r->hairStyle = p->hairStyle;
    r->hairColor = b;
    r->hairApplied = 0;
}

extern "C" void PlayerActor_NetHaircutFinish(Obj *o) {
}

extern "C" void PlayerActor_EndHaircutFinish(Obj *o) {
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerData *r4 = PlayerData_GetBySessionSlot(o->sessionSlot);
        s32 r6 = r4->getHairStyle();
        s32 r2 = r4->getHairColor();
        PlayerActor_NetSendHair(o, r6, r2);
    }
}

extern "C" void PlayerActor_HaircutFinishUpdate(Obj *o) {
    (o)->advanceAnim();
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->hasPassedFrame(10)) {
        EffectSpl_CreateOneShot(0x60, &o->headTopPos, NULL, gEffectSplDefaultInitCbs);
    }
    if (((Bits *)&((AnimFrameCtrl &)o->bodyModel).curFrame)->mid >= 0xb) {
        Rec *r = (Rec *)&o->actionWork;
        if (r->hairApplied == 0) {
            u16 loc[2];
            PlayerActor_GetHat(&loc[1], o);
            loc[0] = loc[1];
            if ((o)->requestHatChange(loc, r->hairStyle, r->hairColor, 0)) {
                PlayerData *p = static_cast<PlayerData *>(PlayerActor_GetPlayerData(o));
                p->setHairStyle(r->hairStyle);
                p->setHairColor(r->hairColor);
                r->hairApplied = 1;
            }
        }
    }
}


}  // namespace ns_0221fa00

namespace ns_0221f0b8 {

struct DoorWalkWork {
    VecFx32 targetPos;
    s32 walkSpeed;
    s32 speedLimit;
    u8 doorTimer;
    u8 entranceType;
};

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef DoorWalkWork Rec;
typedef PlayerActionRequest Msg;

extern "C" {
extern void *gCommManager;

s32 func_01ffcb0c(s32 a, s32 b);
s32 TalkRequest_FinishSceneEntry();
s32 Scene_InHouseRoom();
s32 Scene_InVillagerHouse();
void PlayerActor_GetFrontUnitCenter(V3 *out, Obj *o);

s32 PlayerActor_RequestStandUpFront(Obj *o, s32 a, s32 b);
void PlayerActor_HaircutFinishUpdate(Obj *o);

void PlayerActor_DoorWalkOutSetWork(Rec *r, V3 v, s32 a, s32 b, u8 c);
s32 PlayerActor_RequestDoorWalkOut(Obj *o, V3 *v, u32 a, u32 b);
void PlayerActor_DoorWalkOutSetArgs(V3 *d, V3 v);
void PlayerActor_RoomWalkCheckEnd(Obj *o);
void PlayerActor_RoomWalkUpdateAnim(Obj *o);
void PlayerActor_DoorWalkInUpdate(Obj *o);
void PlayerActor_DoorWalkInSetWork(Rec *r, V3 v, s32 a, u32 b, u8 c);
s32 PlayerActor_RequestDoorWalkIn(Obj *o, V3 *v, u32 a, u32 b);
void PlayerActor_DoorWalkInSetArgs(V3 *d, V3 v);
void PlayerActor_DrinkCoffeeCheckEnd(Obj *o);
void PlayerActor_PhoneHangUpCheckEnd(Obj *o);
s32 PlayerActor_RequestPhoneHangUp(Obj *o, u32 a, u32 b);
s32 PlayerActor_RequestPhoneHold(Obj *o, u32 a, u32 b);
void PlayerActor_PhonePickUpCheckEnd(Obj *o);
void PlayerActor_PhonePickUpMove(Obj *o);
s32 PlayerActor_RequestPhonePickUp(Obj *o, u32 a, u32 b);
void PlayerActor_PhonePickUpSetWork(V3 *d, V3 v);
void PlayerActor_HaircutFinishCheckEnd(Obj *o);
}

extern "C" void PlayerActor_HaircutFinishCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        if ((o)->getInputMagnitude() > 0) {
            s32 t = (o)->getInputAngle() - o->rotY;
            if (Math_AngleToDir4((s16)t) == 0) {
                PlayerActor_RequestStandUpFront(o, 6, -1);
            }
        }
    }
}

extern "C" void PlayerActor_MainHaircutFinish(Obj *o) {
    PlayerActor_HaircutFinishUpdate(o);
    PlayerActor_HaircutFinishCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestPhonePickUp(Obj *o, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7d, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_PhonePickUpSetWork(V3 *d, V3 v) {
    d->x = v.x;
    d->y = v.y;
    d->z = v.z;
}

extern "C" void PlayerActor_SetupPhonePickUp(Obj *o) {
    V3 v;
    (o)->startAnimOnce(0x78, 3, 0);
    PlayerActor_GetFrontUnitCenter(&v, o);
    v.z = v.z + 0x2000;
    PlayerActor_PhonePickUpSetWork((V3 *)&o->actionWork, v);
    RoomTelephone_GetInstance();
    RoomTelephone_PlayAnimPickUp();
}

extern "C" void PlayerActor_NetPhonePickUp(Obj *o, u32 a) {
    PlayerActor_RequestPhonePickUp(o, 6, a);
}

extern "C" void PlayerActor_PhonePickUpMove(Obj *o) {
    Rec *r4 = (Rec *)&o->actionWork;
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        PlayerActor_StepTowardPose(o, r4->targetPos.x, r4->targetPos.z, -0x8000);
    } else {
        (o)->netFollowTransform();
    }
}

extern "C" void PlayerActor_PhonePickUpCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        PlayerActor_RequestPhoneHold(o, 6, -1);
    }
}

extern "C" void PlayerActor_MainPhonePickUp(Obj *o) {
    PlayerActor_PhonePickUpMove(o);
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_PhonePickUpCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestPhoneHold(Obj *o, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7e, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupPhoneHold(Obj *o) {
    (o)->startAnim(0x79, 3, 0);
    RoomTelephone_GetInstance();
    RoomTelephone_PlayAnimHold();
}

extern "C" void PlayerActor_NetPhoneHold(Obj *o, u32 a) {
    PlayerActor_RequestPhoneHold(o, 6, a);
}

extern "C" void PlayerActor_MainPhoneHold(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    (o)->netFollowTransform();
}

extern "C" s32 PlayerActor_RequestPhoneHangUp(Obj *o, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x7f, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupPhoneHangUp(Obj *o) {
    (o)->startAnimOnce(0x78, 3, 0);
    u32 t = (u32)((s32)((AnimFrameCtrl &)o->bodyModel).numFrames << 4) >> 16;
    ((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->setup(t, 3, 0x1000, (u16)(t - 1));
    RoomTelephone_GetInstance();
    RoomTelephone_PlayAnimHangUp();
}

extern "C" void PlayerActor_NetPhoneHangUp(Obj *o, u32 a) {
    PlayerActor_RequestPhoneHangUp(o, 6, a);
}

extern "C" void PlayerActor_PhoneHangUpCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(3, 1, -1);
    }
}

extern "C" void PlayerActor_MainPhoneHangUp(Obj *o) {
    (o)->advanceAnim();
    (o)->updateBodyCollider();
    PlayerActor_PhoneHangUpCheckEnd(o);
}

extern "C" s32 PlayerActor_RequestDrinkCoffee(Obj *o, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x8a, a, b);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_SetupDrinkCoffee(Obj *o) {
    (o)->startAnimOnce(0x95, 3, 0);
}

extern "C" void PlayerActor_NetDrinkCoffee() {
}

extern "C" void PlayerActor_DrinkCoffeeCheckEnd(Obj *o) {
    if (((AnimFrameCtrl *)(&(AnimFrameCtrl &)o->bodyModel))->isFinished()) {
        switch (o->animId) {
        case 0x95:
            (o)->startAnimOnce(0x96, 0, 0);
            break;
        case 0x96:
            (o)->startAnimOnce(0x97, 0, 0);
            break;
        case 0x97:
            break;
        case 0x98:
            PlayerActor_RequestSit(o, 0, 6, -1);
            break;
        }
    }
}

extern "C" void PlayerActor_MainDrinkCoffee(Obj *o) {
    (o)->advanceAnim();
    PlayerActor_DrinkCoffeeCheckEnd(o);
}

extern "C" void PlayerActor_DoorWalkInSetArgs(V3 *d, V3 v) {
    d->x = v.x;
    d->y = v.y;
    d->z = v.z;
}

extern "C" s32 PlayerActor_RequestDoorWalkIn(Obj *o, V3 *v, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x8b, a, b);
    PlayerActor_DoorWalkInSetArgs((VecFx32 *)m.unk_0c_b, *v);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_DoorWalkInSetWork(Rec *r, V3 v, s32 a, u32 b, u8 c) {
    r->targetPos.x = v.x;
    r->targetPos.y = v.y;
    r->targetPos.z = v.z;
    r->walkSpeed = 0;
    r->speedLimit = a;
    r->doorTimer = *(u8 *)&b;
    r->entranceType = c;
}

extern "C" void PlayerActor_DoorWalkInSetNetData(u8 *a, s32 b, s32 c) {
    NetBuf_PackPair20(a, b, c);
}

extern "C" void PlayerActor_DoorWalkInGetNetData(u8 *a, s32 *b, s32 *c) {
    NetBuf_UnpackPair20(a, (u32 *)b, (u32 *)c);
}

extern "C" void PlayerActor_SetupDoorWalkIn(Obj *o, Obj *arg) {
    V3 c;
    V3 *r6 = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    Rec *r0c = (Rec *)&o->actionWork;
    (o)->startAnim(1, 0, 0);
    s32 r7 = 0;
    ((AnimFrameCtrl &)o->bodyModel).frameStep = r7;
    u32 r5 = r7;
    s32 mode = Building_GetLastEntranceType();
    o->drawStep = 1;
    switch (mode) {
    case 2:
        if (((o)->isGuestInSession() && Scene_InHouseRoom()) || Scene_InVillagerHouse()) {
            r7 = 0x14;
            r5 = 0x4ca;
            o->drawStep = 0;
        } else {
            r5 = 0x4cb;
        }
        break;
    case 1:
        r5 = 0x4d2;
        r7 = 0x14;
        o->drawStep = 0;
        break;
    case 0:
    case 3:
        break;
    }
    u8 *r14 = o->netData.bytes;
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        V3 *pv = (V3 *)&o->position;
        c.x = pv->x;
        c.y = pv->y;
        c.z = pv->z;
        PlayerActor_DoorWalkInSetNetData(r14, c.x, c.z);
        o->position.z = o->position.z + 0x6000;
    } else {
        if (r5 != 0) {
            (o)->playSe(r5);
            if (r5 == 0x4cb) Building_PlayDoorChime();
        }
        c.x = r6->x;
        c.y = r6->y;
        c.z = r6->z;
        o->position.z = c.z + 0x6000;
    }
    PlayerActor_DoorWalkInSetWork(r0c, c, 0x400, r7, (u8)mode);
    if (o->drawStep == 1) {
        o->setSpeed(&data_ov004_0224d4b4);
        r0c->walkSpeed = 0x400;
    }
    (o)->setActionFlag(5);
}

extern "C" void PlayerActor_NetDoorWalkIn(Obj *o, u32 a) {
    V3 v;
    PlayerActor_DoorWalkInGetNetData(o->netData.bytes, &v.x, &v.z);
    PlayerActor_RequestDoorWalkIn(o, &v, 6, a);
}

extern "C" void PlayerActor_DoorWalkInUpdate(Obj *o) {
    Rec *r4 = (Rec *)&o->actionWork;
    V3 v;
    s32 lim;
    u8 *p = &r4->doorTimer;
    if (r4->doorTimer != 0) {
        *p = r4->doorTimer - 1;
        if (r4->entranceType == 2) {
            if (*p != 0) return;
            if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
                Building_PlayDoorChime();
                (o)->playSe(0x4cb);
            }
            o->drawStep = 1;
            o->setSpeed(&data_ov004_0224d4b0);
            r4->walkSpeed = 0x400;
        } else {
            u32 c = *p;
            if (c > 0xa) return;
            if (c == 0xa) {
                o->drawStep = 1;
                o->setSpeed(&data_ov004_0224d4a4);
                r4->walkSpeed = 0x400;
            } else if (c == 0) {
                if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
                    (o)->playSe(0x4d3);
                }
            }
        }
    }
    lim = r4->speedLimit;
    v.x = r4->targetPos.x;
    v.y = r4->targetPos.y;
    v.z = r4->targetPos.z;
    PlayerActor_RoomWalkToward(o, &v, &r4->walkSpeed, &lim);
}

extern "C" void PlayerActor_RoomWalkUpdateAnim(Obj *o) {
    s32 r4 = o->speed;
    s32 t = func_01ffcb0c(r4, 0x3ae1);
    if (t < 0x800) t = 0x800;
    if (t <= (s32)((AnimFrameCtrl &)o->bodyModel).numFrames) ((AnimFrameCtrl &)o->bodyModel).frameStep = t;
    if (r4 > 0x53f) {
        if (o->animId != 2) (o)->switchAnim(2, 3, 0);
    } else {
        if (o->animId != 1) (o)->switchAnim(1, 3, 0);
    }
    ((TwoLayerAnimModel *)(&o->bodyModel))->updateLayers();
    (o)->updateFootstepFx();
}

extern "C" void PlayerActor_RoomWalkCheckEnd(Obj *o) {
    if (o->speed == 0 && o->drawStep == 1) {
        o->actionPriority = ((Unk_02007694 *)(void *)(o))->getActionDonePriority(o->action);
        (o)->requestWait(3, 5, -1);
        (o)->clearActionFlag(5);
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            TalkRequest_FinishSceneEntry();
        }
    }
}

extern "C" void PlayerActor_MainDoorWalkIn(Obj *o) {
    PlayerActor_DoorWalkInUpdate(o);
    PlayerActor_RoomWalkUpdateAnim(o);
    (o)->moveNoCollision();
    (o)->updateBodyCollider();
    PlayerActor_RoomWalkCheckEnd(o);
}

extern "C" void PlayerActor_DoorWalkOutSetArgs(V3 *d, V3 v) {
    d->x = v.x;
    d->y = v.y;
    d->z = v.z;
}

extern "C" s32 PlayerActor_RequestDoorWalkOut(Obj *o, V3 *v, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x8c, a, b);
    PlayerActor_DoorWalkOutSetArgs((VecFx32 *)m.unk_0c_b, *v);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_DoorWalkOutSetWork(Rec *r, V3 v, s32 a, s32 b, u8 c) {
    r->targetPos.x = v.x;
    r->targetPos.y = v.y;
    r->targetPos.z = v.z;
    r->walkSpeed = a;
    r->speedLimit = b;
    r->doorTimer = 0;
    r->entranceType = c;
}

extern "C" void PlayerActor_DoorWalkOutSetNetData(u8 *a, s32 b, s32 c) {
    NetBuf_PackPair20(a, b, c);
}

extern "C" void PlayerActor_DoorWalkOutGetNetData(u8 *a, s32 *b, s32 *c) {
    NetBuf_UnpackPair20(a, (u32 *)b, (u32 *)c);
}

extern "C" void PlayerActor_SetupDoorWalkOut(Obj *o, Obj *arg) {
    V3 c;
    V3 *r6 = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    if (o->animId != 1) (o)->startAnim(1, 3, 0);
    u32 r5 = 0;
    s32 r7 = Building_GetLastEntranceType();
    switch (r7) {
    case 2:
        r5 = 0x4cb;
        break;
    case 1:
        r5 = 0x4d2;
        break;
    case 0:
    case 3:
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
            SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
        }
        break;
    }
    if (r5 != 0) {
        if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot) == 0) {
            (o)->playSe(r5);
            if (r5 == 0x4cb) Building_PlayDoorChime();
        }
    }
    c.x = r6->x;
    c.y = r6->y;
    c.z = r6->z;
    PlayerActor_DoorWalkOutSetWork((Rec *)&o->actionWork, c, o->speed, 0x400, (u8)r7);
    PlayerActor_DoorWalkOutSetNetData(o->netData.bytes, c.x, c.z);
}

extern "C" void PlayerActor_NetDoorWalkOut(Obj *o, u32 a) {
    V3 v;
    PlayerActor_DoorWalkOutGetNetData(o->netData.bytes, &v.x, &v.z);
    PlayerActor_RequestDoorWalkOut(o, &v, 6, a);
}


}  // namespace ns_0221f0b8

namespace ns_0221e7a8 {

struct ExitWalkWork {
    VecFx32 targetPos;
    s32 walkSpeed;
    s32 speedLimit;
    u8 doorTimer;
    u8 entranceType;
};

typedef PlayerActor Obj;
typedef VecFx32 V3;
typedef Vec2 Pair;
typedef ExitWalkWork Rec;
typedef PlayerActionRequest Msg;

extern "C" {
extern void *gCommManager;
extern s16 data_02135f44[];
extern BlockMap *gSceneBlockMap;

s32 Vec_MagXZ(V3 *v);
s32 Math_Atan2(s32 a, s32 b);
s32 func_01ffcb0c(s32 a, s32 b);
void VEC_Add(void *a, void *b, void *c);
s32 PlayerActor_Decelerate(s32 a, s32 b);
s32 PlayerActor_Accelerate(s32 a, s32 b);
void PlayerActor_TurnAngle(void *a, s32 b);
s32 Scene_InHouseRoom();
BOOL Item_IsNormalItem(u16 *p);

void PlayerActor_RoomWalkUpdateAnim(Obj *o);
void PlayerActor_RoomWalkCheckEnd(Obj *o);

void PlayerActor_ExitWalkInMove(Obj *o);
void PlayerActor_ExitWalkOutMove(Obj *o);
void PlayerActor_DoorWalkOutMove(Obj *o);
void PlayerActor_ExitWalkInSetWork(Rec *r, V3 v, s32 w);
s32 PlayerActor_RequestExitWalkIn(Obj *o, V3 *v, u32 a, u32 b);
void PlayerActor_ExitWalkInSetArgs(V3 *d, V3 v);
void PlayerActor_ExitWalkOutSetWork(Rec *r, V3 v, s32 a, s32 b);
s32 PlayerActor_RequestExitWalkOut(Obj *o, V3 *v, u32 a, u32 b);
void PlayerActor_ExitWalkOutSetArgs(V3 *d, V3 v);
}

extern "C" void PlayerActor_DoorWalkOutMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    s32 lim = r->speedLimit;
    V3 v;
    v.x = r->targetPos.x;
    v.y = r->targetPos.y;
    v.z = r->targetPos.z;
    PlayerActor_RoomWalkToward(o, &v, &r->walkSpeed, &lim);
}

extern "C" void PlayerActor_MainDoorWalkOut(Obj *o) {
    PlayerActor_DoorWalkOutMove(o);
    PlayerActor_RoomWalkUpdateAnim(o);
    (o)->moveNoCollision();
    (o)->updateBodyCollider();
    Rec *q = (Rec *)&o->actionWork;
    u8 *r4 = &q->doorTimer;
    u8 r6 = q->entranceType;
    void *r7 = gCommManager;
    if (((CommManager *)(r7))->isLocalSlot(o->sessionSlot) == 0 && *r4 == 0xf && r6 == 1) {
        (o)->playSe(0x4d3);
    }
    if (((CommManager *)(r7))->isLocalSlot(o->sessionSlot) != 0) {
        if ((u32)(r6 - 1) <= 1 && *r4 == 0xa) {
            SceneWarp_RequestExit(Scene_GetWarpRequest(), o->exitIndex);
        }
    }
    *r4 = *r4 + 1;
}

extern "C" void PlayerActor_ExitWalkOutSetArgs(V3 *d, V3 v) {
    d->x = v.x;
    d->y = v.y;
    d->z = v.z;
}

extern "C" s32 PlayerActor_RequestExitWalkOut(Obj *o, V3 *v, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x8d, a, b);
    PlayerActor_ExitWalkOutSetArgs((VecFx32 *)m.unk_0c_b, *v);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_ExitWalkOutSetWork(Rec *r, V3 v, s32 a, s32 b) {
    r->targetPos.x = v.x;
    r->targetPos.y = v.y;
    r->targetPos.z = v.z;
    r->walkSpeed = a;
    r->speedLimit = b;
}

extern "C" void PlayerActor_ExitWalkOutSetNetData(u8 *a, s32 b, s32 c) {
    NetBuf_PackPair20(a, b, c);
}

extern "C" void PlayerActor_ExitWalkOutGetNetData(u8 *a, s32 *b, s32 *c) {
    NetBuf_UnpackPair20(a, (u32 *)b, (u32 *)c);
}

extern "C" void PlayerActor_SetupExitWalkOut(Obj *o, Obj *arg) {
    V3 c;
    V3 *r4 = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    if (o->animId != 1) (o)->startAnim(1, 3, 0);
    c.x = r4->x;
    c.y = r4->y;
    c.z = r4->z;
    PlayerActor_ExitWalkOutSetWork((Rec *)&o->actionWork, c, o->speed, 0x400);
    PlayerActor_ExitWalkOutSetNetData(o->netData.bytes, c.x, c.z);
}

extern "C" void PlayerActor_NetExitWalkOut(Obj *o, u32 a) {
    V3 v;
    PlayerActor_ExitWalkOutGetNetData(o->netData.bytes, &v.x, &v.z);
    if ((v.x & 0x80000) != 0) v.x |= 0xfff00000;
    if ((v.z & 0x80000) != 0) v.z |= 0xfff00000;
    PlayerActor_RequestExitWalkOut(o, &v, 6, a);
}

extern "C" void PlayerActor_ExitWalkOutMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    s32 lim = r->speedLimit;
    V3 v;
    v.x = r->targetPos.x;
    v.y = r->targetPos.y;
    v.z = r->targetPos.z;
    PlayerActor_RoomWalkToward(o, &v, &r->walkSpeed, &lim);
}

extern "C" void PlayerActor_MainExitWalkOut(Obj *o) {
    PlayerActor_ExitWalkOutMove(o);
    PlayerActor_RoomWalkUpdateAnim(o);
    (o)->moveNoCollision();
    (o)->updateBodyCollider();
}

extern "C" void PlayerActor_ExitWalkInSetArgs(V3 *d, V3 v) {
    d->x = v.x;
    d->y = v.y;
    d->z = v.z;
}

extern "C" s32 PlayerActor_RequestExitWalkIn(Obj *o, V3 *v, u32 a, u32 b) {
    Msg m;
    ((PlayerActionRequest *)(&m))->assign(0x8e, a, b);
    PlayerActor_ExitWalkInSetArgs((VecFx32 *)m.unk_0c_b, *v);
    s32 r = (o)->pushRequest(&m);
    return r;
}

extern "C" void PlayerActor_ExitWalkInSetWork(Rec *r, V3 v, s32 w) {
    r->targetPos.x = v.x;
    r->targetPos.y = v.y;
    r->targetPos.z = v.z;
    r->walkSpeed = w;
    r->speedLimit = w;
}

extern "C" void PlayerActor_ExitWalkInSetNetData(u8 *a, s32 b, s32 c) {
    NetBuf_PackPair20(a, b, c);
}

extern "C" void PlayerActor_ExitWalkInGetNetData(u8 *a, s32 *b, s32 *c) {
    NetBuf_UnpackPair20(a, (u32 *)b, (u32 *)c);
}

extern "C" void PlayerActor_SetupExitWalkIn(Obj *o, Obj *arg) {
    V3 cur;
    V3 w;
    V3 tmp;
    V3 *r5 = (V3 *)(((PlayerActionRequest *)(void *)arg)->unk_0c_b);
    Rec *r6 = (Rec *)&o->actionWork;
    (o)->startAnim(1, 0, 0);
    ((AnimFrameCtrl &)o->bodyModel).frameStep = 0;
    w.x = 0;
    w.y = 0;
    w.z = 0;
    u8 *r7 = o->netData.bytes;
    switch (Math_AngleToDir4(o->rotY)) {
    case 2:
        w.z = w.z + 0x6000;
        break;
    case 0:
        w.z = w.z - 0x6000;
        break;
    case 3:
        w.x = w.x + 0x6000;
        break;
    case 1:
        w.x = w.x - 0x6000;
        break;
    }
    if (((CommManager *)(gCommManager))->isLocalSlot(o->sessionSlot)) {
        V3 *pv = (V3 *)&o->position;
        cur.x = pv->x;
        cur.y = pv->y;
        cur.z = pv->z;
        PlayerActor_ExitWalkInSetNetData(r7, cur.x, cur.z);
        VEC_Add((V3 *)&o->position, &w, (V3 *)&o->position);
    } else {
        cur.x = r5->x;
        cur.y = r5->y;
        cur.z = r5->z;
        Vec_Add(&tmp, &cur, &w);
        V3 *pv = (V3 *)&o->position;
        pv->x = tmp.x;
        pv->y = tmp.y;
        pv->z = tmp.z;
    }
    PlayerActor_ExitWalkInSetWork(r6, cur, 0x400);
    (o)->setSpeed(&data_ov004_0224d4a8);
    (o)->setActionFlag(5);
}

extern "C" void PlayerActor_NetExitWalkIn(Obj *o, u32 a) {
    V3 v;
    PlayerActor_ExitWalkInGetNetData(o->netData.bytes, &v.x, &v.z);
    PlayerActor_RequestExitWalkIn(o, &v, 6, a);
}

extern "C" void PlayerActor_ExitWalkInMove(Obj *o) {
    Rec *r = (Rec *)&o->actionWork;
    s32 lim = r->speedLimit;
    V3 v;
    v.x = r->targetPos.x;
    v.y = r->targetPos.y;
    v.z = r->targetPos.z;
    PlayerActor_RoomWalkToward(o, &v, &r->walkSpeed, &lim);
}

extern "C" void PlayerActor_MainExitWalkIn(Obj *o) {
    PlayerActor_ExitWalkInMove(o);
    PlayerActor_RoomWalkUpdateAnim(o);
    (o)->moveNoCollision();
    (o)->updateBodyCollider();
    PlayerActor_RoomWalkCheckEnd(o);
}

extern "C" void PlayerActor_StepAlpha(Obj *o, u32 lim, u32 step) {
    u8 *p = &o->alpha;
    u32 c = *p;
    if (c == lim) return;
    if (c < lim) {
        *p = c + step;
        if (*p > lim) *p = lim;
    } else {
        *p = c - step;
        if (*(s8 *)p < (s32)lim) *p = lim;
    }
}

extern "C" s32 PlayerActor_RoomInteractAt(Obj *o, s32 param) {
    s32 o1 = 0, o2 = 0;
    s32 u, v;
    Pair pa, pb, pc, pd, pe, pf;
    V3 tgt, tmp1, tmp2;
    s32 r5;
    u16 *item;
    BlockMap *r6;
    if (Scene_InHouseRoom() == 0) goto fail;
    if (o->sessionSlot != 0) goto fail;
    r5 = FtrMgr_FindFurnitureFacingPlayer(&u, &v, &o->actionItem, &o->shownItem);
    if (r5 >= 0) {
        if (r5 == o->touchTargetId || o->inputMode == 1) {
            if (FtrMgr_GetActorLayer(r5) == 0) {
                pa.x = 0;
                pa.y = 0;
                ((PlayerActor *)(o))->requestPickUpReach(Unk_0200b750_Pair(pa.x, pa.y), r5, 6, -1);
            } else {
                pb.x = (u8)u;
                pb.y = (u8)v;
                PlayerActor_RequestPickUpItem(o, &pb, r5, 6, -1);
            }
            return 1;
        }
    }
    r6 = gSceneBlockMap;
    if (r5 == -2) {
        if (o->inputMode == 1) {
            PlayerActor_OffsetByAngle(&tmp1, o, &o->position, &o->rotY, (s32 *)&sAct12Pos[3]);
            tgt.x = tmp1.x;
            tgt.y = tmp1.y;
            tgt.z = tmp1.z;
        } else {
            tgt.x = o->targetPos.x;
            tgt.y = o->targetPos.y;
            tgt.z = o->targetPos.z;
            if ((o)->isPosInReach((s32 *)(void *)(&tgt), 0xe) == 0) goto second;
        }
        item = BlockMap_GetItemPtrAtPos(r6, &tgt, 1);
        if (Pocket_FindEmpty() == -1) {
            if (item == 0) goto second;
            if (Item_IsNormalItem(item) == 0) goto second;
            FieldPos_ToUnit(&o1, &o2, &tgt);
            pc.x = o1;
            pc.y = o2;
            ((PlayerActor *)(o))->requestPickUpReach(Unk_0200b750_Pair(pc.x, pc.y), -1, 6, -1);
            return 1;
        }
        if (item == 0) goto second;
        if (Item_IsNormalItem(item) == 0) goto second;
        if ((u32)Room_CountOccupants() <= 1) {
            if ((o)->startFieldQuery(&tgt, 0, param) == 0) goto second;
            if ((o)->requestByFieldAnswer(&tgt, 1) == 0) goto second;
            return 1;
        }
        FieldPos_ToUnit(&o1, &o2, &tgt);
        pd.x = o1;
        pd.y = o2;
        ((PlayerActor *)(o))->requestPickUpReach(Unk_0200b750_Pair(pd.x, pd.y), -3, 6, -1);
        return 1;
    }
second:
    if (o->inputMode == 1) {
        PlayerActor_OffsetByAngle(&tmp2, o, &o->position, &o->rotY, &sRoomInteractDist);
        tgt.x = tmp2.x;
        tgt.y = tmp2.y;
        tgt.z = tmp2.z;
    } else {
        tgt.x = o->targetPos.x;
        tgt.y = o->targetPos.y;
        tgt.z = o->targetPos.z;
        if ((o)->isPosInReach((s32 *)(void *)(&tgt), 0xc) == 0) return 0;
    }
    {
        u16 *q = BlockMap_GetItemPtrAtPos(r6, &tgt, 0);
        if (q == 0) goto fail;
        if (Item_IsNormalItem(q) == 0) goto fail;
    }
    if (Pocket_FindEmpty() == -1) {
        FieldPos_ToUnit(&o1, &o2, &tgt);
        pe.x = o1;
        pe.y = o2;
        ((PlayerActor *)(o))->requestPickUpReach(Unk_0200b750_Pair(pe.x, pe.y), -1, 6, -1);
        return 1;
    }
    if ((u32)Room_CountOccupants() <= 1) {
        if ((o)->startFieldQuery(&tgt, 0, param) == 0) goto fail;
        if ((o)->requestByFieldAnswer(&tgt, 0) == 0) goto fail;
        return 1;
    }
    FieldPos_ToUnit(&o1, &o2, &tgt);
    pf.x = o1;
    pf.y = o2;
    ((PlayerActor *)(o))->requestPickUpReach(Unk_0200b750_Pair(pf.x, pf.y), -3, 6, -1);
    return 1;
fail:
    return 0;
}

extern "C" void PlayerActor_NetSendHair(void *unused, s32 a, s32 b) {
    u8 buf[4];
    if (((CommManager *)(gCommManager))->isOnline()) {
        PlayerActor_PackHair(buf, a, b);
        void *g = gCommManager;
        ((CommManager *)(g))->beginRecord();
        ((CommManager *)(g))->writeRecord(buf, 1);
        ((CommManager *)(g))->endRecord(0x2c, 4);
    }
}

extern "C" s32 PlayerActor_RoomUseTool(Obj *o) {
    if (o->actionHeld != 0) {
        FtrContactSet *a = FtrContactSet_GetInstance();
        FtrContact *r4 = ((FtrContactSet *)(a))->getContact(0);
        if (r4 == 0) return 0;
        volatile V3 loc;
        V3 *p = ((FtrContact *)(r4))->getClampedContactPoint();
        loc.x = p->x;
        loc.y = p->y;
        loc.z = p->z;
        return PlayerActor_RequestFtrGrabApproach(o, loc.x, loc.z, ((FtrContact *)(r4))->getPushAngle(), 5, -1);
    }
    if (o->hasTargetPos != 0) {
        return (o)->interactAt((o)->getHeldToolKind());
    }
    return 0;
}

extern "C" void PlayerActor_RoomWalkToward(Obj *o, V3 *tgt, s32 *out, s32 *lim) {
    V3 d;
    V3 saved;
    s32 t;
    s32 ang;
    s16 h;
    s32 v;
    V3 *pv = (V3 *)&o->position;
    saved = *pv;
    d.x = tgt->x - o->position.x;
    d.z = tgt->z - o->position.z;
    if (Vec_MagXZ(&d) < 0x1000) {
        if (*out <= *lim) {
            PlayerActor_ApproachValue(&o->position.x, tgt->x, 0x800, *lim, 0x31);
            PlayerActor_ApproachValue(&o->position.z, tgt->z, 0x800, *lim, 0x31);
            if (tgt->x == o->position.x && tgt->z == o->position.z) {
                *out = 0;
            } else {
                *out = Vec_DistXZ((V3 *)&o->position, &saved);
                V3 *pw = (V3 *)&o->position;
                *pw = saved;
            }
        } else {
            *out = PlayerActor_Decelerate(*out, *lim);
        }
    } else {
        *out = PlayerActor_Accelerate(*out, *lim);
    }
    ang = Math_Atan2(d.x, d.z);
    h = o->rotY;
    if (*out != 0) {
        PlayerActor_TurnAngle(&h, ang);
        (o)->setAngleY(&h);
    }
    s32 vt = func_01ffcb0c(*out, data_02135f44[(((u16)(s16)(h - ang)) >> 4) * 2 + 1]);
    if (vt < 0) vt = -vt;
    v = vt;
    (o)->setSpeed((u32 *)(void *)(&v));
}


}  // namespace ns_0221e7a8

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov004_0221e7a8) {
    HOST_OVERLAY_VAR(data_ov004_0224d4a4),
    HOST_OVERLAY_VAR(sRoomPlayerMsgFile),
    HOST_OVERLAY_VAR(data_ov004_0224d4a8),
    HOST_OVERLAY_VAR(sFtrPullDist),
    HOST_OVERLAY_VAR(sSeatApproachDist),
    HOST_OVERLAY_VAR(data_ov004_0224d4b0),
    HOST_OVERLAY_VAR(sRoomInteractDist),
    HOST_OVERLAY_VAR(sRoomErrorMsgFile),
    HOST_OVERLAY_VAR(data_ov004_0224d4b4),
    HOST_OVERLAY_VARS_END
};

#include "uptr.h"
#include "host_labels.h"
#include "proto/main/unk_0202fa70.h"
// mwcc-flags: -str reuse
#include "types.h"
#include "gfx/VecFx32.h"
#include "game/Unk_0201acf8.h"
#include "actor/BlinkTimer.h"
#include "gfx/HudObjGfx.h"
#include "player/HeldToolModel.h"
#include "player/HeldItemModel.h"
#include "npc/NpcAnimCtrl.h"
#include "npc/NpcSpeechState.h"
#include "npc/NpcTalkCtrl.h"
#include "gfx/TwoLayerAnimModel.h"
#include "npc/NpcLookAt.h"
#include "npc/NpcObstacleProbe.h"
#include "npc/NpcMoveCtrl.h"
#include "npc/NpcMoveAnimSet.h"
#include "npc/Unk_0201ad18.h"
#include "game/FxVec3.h"
#include "sys/ProcBase.h"
#include "npc/NpcResHandle.h"
#include "gfx/MatTexPatAnim.h"
#include "npc/NpcActionCtrl.h"
#include "game/GroundInfo.h"
#include "talk/MsgString9B.h"
#include "talk/MsgString9C.h"
#include "talk/MsgString33.h"
#include "talk/MsgString25.h"
#include "npc/NpcFaceAnim.h"
#include "item/ItemName.h"
#include "actor/Actor.h"
#include "actor/Character.h"
#include "npc/NpcEmotionFx.h"
#include "talk/MsgRequest.h"
#include "talk/TalkMsgRequest.h"
#include "talk/ActorTalkRequest.h"
#include "talk/ActorTalkRequest.h"
#include "talk/TalkWindowState.h"
#include "npc/NpcFootstepFx.h"
#include "actor/NpcActor.h"
#include "nitro/mtx.h"
#include "gfx/NNSG3dRS.h"
#include "net/CommManager.h"
#include "actor/ActorCollider.h"
#include "actor/CharaFaceAnimRef.h"
#include "actor/CharaFaceAnimWorkRef.h"
#include "game/GroundInfoBase.h"
#include "gfx/AnimFrameCtrl.h"
#include "gfx/AnimModel.h"
#include "gfx/CachedModel.h"
#include "gfx/Model.h"
#include "gfx/ThreeLayerAnimModel.h"
#include "npc/VillagerDataItemView.h"
#include "npc/VillagerId.h"
#include "player/PlayerData.h"
#include "player/PlayerId.h"
#include "snd/SndSeEmitter.h"
#include "talk/MsgString.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02119434.h"
#include "proto/autoload_2/unk_02133d04.h"
#include "proto/main/unk_02002b1c.h"
#include "proto/main/unk_02060b7c.h"
#include "proto/main/unk_020b0a80.h"
#include "proto/itcm/unk_01ffb4e8.h"
#include "proto/itcm/unk_01ffcfc0.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_020116e0.h"
#include "proto/main/unk_020119cc.h"
#include "proto/main/unk_02038474.h"
#include "proto/main/unk_0203ecec.h"
#include "proto/main/unk_02055c38.h"
#include "proto/main/unk_020571f4.h"
#include "proto/main/unk_0205b7b0.h"
#include "proto/main/unk_0205dfa4.h"
#include "proto/main/unk_020637e4.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_0206d988.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_02082d74.h"
#include "proto/main/unk_0208eeac.h"
#include "proto/main/unk_02090268.h"
#include "proto/main/unk_02093ff0.h"
#include "proto/main/unk_020943dc.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_0209eb94.h"
#include "proto/main/unk_020a427c.h"
#include "proto/main/unk_020abbcc.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/ov004/unk_ov004_0221e7a8.h"
#include "proto/ov004/unk_ov004_02233074.h"
#include "save/VillagerData.h"
#include "town/BlockMap.h"
#include "town/MapBlock.h"
#include "npc/VillagerRoute.h"
#include "proto/main/unk_0204cc1c.h"








// unk_02011ec0.cpp



// unk_02011ec0.cpp




// unk_020131a4.cpp: picks the route's target unit (the villager's position and the villager)
typedef void (*VillagerRoutePickFn)(Vec2 *out, VillagerRoute *self, VecFx32 *pos, NpcActor *owner);

// unk_02012810.cpp
typedef s32 (VillagerRoute::*Unk_02012810_Fn)(VecFx32 *);

// unk_02012810.cpp
typedef s32 (VillagerRoute::*Unk_02012810_Fn0)();

// unk_02012810.cpp
struct VillagerRouteType {
    Unk_02012810_Fn arrivedCheck;
    VillagerRoutePickFn pickTarget;
    Unk_02012810_Fn tbl[2][3];
};

// unk_02012810.cpp
struct RouteGridPosCopy {
    u32 x, z;
    RouteGridPosCopy(const RouteGridPos &o) : x(o.x), z(o.z) {}
    RouteGridPosCopy(const RouteGridPosCopy &o) : x(o.x), z(o.z) {}
};

// unk_02012810.cpp
struct RouteJunction {
    u8 x, z;
    union {
        u8 b;
        struct { u8 lo : 4; u8 hi : 4; } f;
    };
};



#ifdef HOST64
static_assert(sizeof(VillagerRoute) == VILLAGER_ROUTE_HOST_SIZE, "VILLAGER_ROUTE_HOST_SIZE (include/uptr.h)");
#endif


// unk_020131a4.cpp
typedef VecFx32 Unk_020133cc_Vec;

// unk_020131a4.cpp
struct Unk_02013474_Half { s16 a; s16 b; };

// unk_020131a4.cpp
struct Unk_02013778_Vec : Unk_020133cc_Vec {
    Unk_02013778_Vec() {}
};

// unk_020131a4.cpp
typedef void (NpcTalkCtrl::*Unk_02013474_Fn)(NpcActor*);


// unk_02013b10.cpp


class NpcTalkCtrl;


// unk_02013b10.cpp
struct NpcTalkCtrlState {
    void (NpcTalkCtrl::*setupFn)(NpcActor *);
    void (NpcTalkCtrl::*mainFn)(NpcActor *);
};


// unk_02014420.cpp
typedef BOOL (ActorTalkRequest::*Unk_02014420_Fn)();

struct Actor;




// unk_02014d90.cpp
typedef BOOL (ActorTalkRequest::*ActorTalkTaskFn)();









// unk_02015fe0.cpp
typedef void (NpcActionCtrl::*Unk_02016360_Fn)(NpcActor *);




// unk_02017d74.cpp


// unk_02017d74.cpp
typedef void (NpcActionCtrl::*Unk_02017d74_Fn)(NpcActor *);


struct NpcEmotionEntry;



// unk_02018698.cpp

// unk_02018698.cpp
typedef VecFx32Ctor V;


struct NpcActionCtrl;

// unk_02019020.cpp
typedef s32 (NpcActionCtrl::*Unk_02019858_FnA)(NpcActor *owner);

// unk_02019020.cpp
typedef void (NpcActionCtrl::*Unk_02019858_FnB)(NpcActor *arg);

// unk_02019020.cpp
typedef void (NpcActionCtrl::*Unk_02019858_FnC)(NpcActor *arg);

// unk_02019020.cpp
struct NpcActionEntry {
    Unk_02019858_FnA setupFn;
    Unk_02019858_FnB mainFn;
    Unk_02019858_FnC postFn;
};













// unk_02019998.cpp
typedef void (NpcLookAt::*Unk_0201a1e0_Fn)(Character *);







// unk_0201a334.cpp
struct NpcMoveModeEntry {
    s32 animKind;
    s16 defaultTurnSpeed;
    s16 pad_06;
    s32 speedPresetIdx;
    s32 arriveDistance;
    u8 keepAnimFrame;
    u8 pad_11[3];
};

class NpcLookAt;

class NpcMoveCtrl;

class NpcObstacleProbe;





// unk_0201ac80.cpp
struct Unk_0201b2b8_Bits { u32 lo : 12; u32 mid : 16; u32 hi : 4; };

// unk_0201ac80.cpp
struct NpcNetActionHalves { u8 b0; u8 pad; s16 h2; s16 h4; u16 h6; u16 h8; u16 ha; };




// unk_0201ac80.cpp
typedef VecFx32 V3;






// the direction table sRouteDirs (filled by __sinit)
struct Unk_021be028_Dir : VecXZ {
    Unk_021be028_Dir(s32 a, s32 b) { x = a; z = b; }
};
extern "C" Unk_021be028_Dir sRouteDirs[4];
// An emotion: its intro and loop phases (NpcEmotionFx::startEntry), then how the intro plays and the sound mode.
struct NpcEmotionEntry {
    /* 0x00 */ NpcEmotionPhase phases[2];
    /* 0x18 */ u8 introMode;
    /* 0x19 */ u8 seMode;
    /* 0x1a */ u8 pad_1a[2];
};
typedef void (NpcTalkCtrl::*Unk_02014040_Fn)(NpcActor *);


// ---- unk_02011580.cpp
// the unit's data tables (defined below)
extern "C" {
extern const u16 data_020c6cbc[2];
extern const u16 data_020c6cc0[2];
extern const u16 data_020c6cc4[2];
extern const u16 data_020c6cc8[2];  // {blend frames, 0}: the first is read
extern const u16 data_020c6ccc[2];
extern const u16 data_020c6cd0[2];
extern const u16 data_020c6cd4[2];
extern const u16 data_020c6cd8[2];
extern const u16 data_020c6cdc[2];
extern const u16 data_020c6ce0[2];
extern const u16 data_020c6ce4[2];
extern const u16 data_020c6ce8[2];
extern const u16 data_020c6cec[2];
extern const u16 data_020c6cf0[2];
extern const u16 data_020c6cf4[2];
extern const u16 data_020c6cf8[2];
extern const u16 data_020c6cfc[2];
extern const u16 data_020c6d00[2];
extern const u16 data_020c6d04[2];
extern const u16 data_020c6d08[2];
extern const u16 data_020c6d0c[2];
extern const u16 data_020c6d10[2];
extern const u16 data_020c6d14[2];
extern const u16 data_020c6d18[2];
extern const u16 data_020c6d1c[2];
extern const s32 data_020c6d20;
extern const u16 data_020c6d24[2];
extern const u16 data_020c6d28[2];
extern const u16 data_020c6d2c[2];
extern const u16 data_020c6d30[2];
extern const u16 data_020c6d34[2];
extern const u16 data_020c6d38[2];
extern const u16 data_020c6d3c[2];
extern const s32 sNpcTalkMouthAnims[2];
extern const s32 data_020c6d48[2];
extern const u16 data_020c6d50[4];
extern const u16 data_020c6d58[4];
extern const s32 data_020c6d60[2];
extern const u32 sNpcGiveItemAnims[2];
extern const u16 data_020c6d70[4];
extern const u16 sNpcAct07Ses[5];
extern const u32 data_020c6d84[4];
extern const u32 sNpcAct07Anims[5];
extern const u32 sNpcMoveSpeedPresets[9];
extern const VecFx32 data_020c6dcc[4];
extern const NpcMoveModeEntry sNpcMoveModeTable[5];
extern const NpcEmotionEntry sEmotionTable[60];
extern FxVec3 sNpcObstacleProbeOffsets[2];
}

// declarations shared by the merged units below (the real signatures)
extern "C" {
extern VecFx32 gVec3Zero;
void MI_CpuFill8(void *dst, u32 v, u32 n);
BOOL Model_GetJointWorldMtx(Model *p, void *a, s32 b);
BOOL Math_StepAngle(s16 *p, s16 target, s16 step);
s32 Vec_DistXZ(VecFx32 *a, VecFx32 *b);
s32 Vec_Equal(VecFx32 *a, VecFx32 *b);
void Camera_FocusOnPair(VecFx32 *a, VecFx32 *b);
BOOL Camera_FocusOnPoint(VecFx32 *a);
void ThreeLayerAnimModel_AssignJointsToLayer2(void *a, u32 b, u32 c);
BOOL TownMap_IsUnitWalkable(s32 x, s32 y, void *grid);
s32 Item_GetFurnitureIndex(u16 *p);
}

namespace nA {
extern "C" {

extern const char data_020d6f7c[];
extern const char data_020d6f9c[];
extern const char data_020d6fbc[];
extern const char data_020d6fdc[];
extern const char data_020d6ff8[];
extern const char data_020d7018[];
extern const char data_020d7038[];
extern const char data_020d7058[];
extern u8 data_021bddc0[];
extern u8 sSceneHudKinds[];
extern u32 data_021bdd80[];
extern u32 data_020d6f54[];
s32 Scene_GetCurrent(void);
inline BOOL Unk_02011c44_InRange(u32 id, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (id >= lo && id <= hi) r = TRUE;
    return r;
}
}
}

// ---- unk_02011ec0.cpp
namespace nB {
extern "C" {

extern BlockMap *gSceneBlockMap;
#define gSaveVillagers HOST_LABEL(u8 (*)[], gSaveVillagers)
extern s16 data_02135f44[];
s32 MapBlock_HasAnyAttr(void *, u32);
s32 Random_PickSetBit(u32, s32, s32);
void *SaveVillagers_PickRandomExcept(void *, void *, u32);
s32 Random_Next(void *);
s32 func_01ffcb0c(s32, s32);
s32 TownMap_IsPosWalkable(VecFx32 *, s32);
static inline MapBlock *GetCell(BlockMap *w, u32 x, u32 y)
{
    if (x < (u32)w->width && (u32)w->height > y && w->blocks != NULL) {
        return (MapBlock *)w->blocks + (x + w->width * y);
    }
    return NULL;
}
static inline MapBlock *GetCellD(BlockMap *w, u32 x, u32 y, MapBlock *volatile *dflt)
{
    if (x < (u32)w->width && (u32)w->height > y && w->blocks != NULL) {
        return (MapBlock *)w->blocks + (x + w->width * y);
    }
    return *dflt;
}
}
}

// ---- unk_02012810.cpp
namespace nC {
extern "C" {
extern BlockMap *gSceneBlockMap;
s32 Random_PickSetBit(u32 mask, s32 n, s32 max);
void MI_CpuCopy8(const void *src, void *dst, u32 size);
}
}

// ---- unk_020131a4.cpp
namespace nD {
extern "C" {

static inline BOOL Unk_02013568_IsSet(u32 v, u32 m)
{
    if (v & m) return TRUE;
    return FALSE;
}
}
}

// ---- unk_02013b10.cpp
namespace nE {
extern "C" {

u32 Camera_IsBlending(void);
u32 Camera_GetBlendFramesLeft(void);
void Camera_RetargetFocus(VecFx32 *v);
void Camera_SetModeDefault(void);
extern NpcTalkCtrlState sNpcTalkCtrlStates[5];
}
}

// ---- unk_02014420.cpp
namespace nF {
extern "C" {

}
}

// ---- unk_02014d90.cpp
namespace nG {
extern "C" {

}
}

// ---- unk_020156ac.cpp
namespace nH {
extern "C" {

s32 Scene_GetCurrent(void);
u32 Npc_GetVoiceType(u16 *p);
s32 func_01ffcb0c(s32 a, s32 b);
void *func_021065dc(void *a);
void *func_021065f8(void *a, s32 b);
}
}

// ---- unk_02015fe0.cpp
namespace nI {
extern "C" {

BOOL Item_IsHoldable(u16 *p);
void BlendAnimModel_Play2(void *p, void *anm, s32 blend, s32 mode, s32 speed, u16 frame, u16 numFrames);
void *func_021065dc(void *a);
void *func_021065f8(void *a, s32 b);
void NpcActor_JointCalcLayer3Cb(NNSG3dRS *p);
s32 func_01ffcb0c(s32 a, s32 b);
s32 Math_Atan2(s32 a, s32 b);
static inline BOOL Unk_02015fe0_R(u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    if (*p >= lo && *p <= hi) r = TRUE;
    return r;
}
}
}

// ---- unk_02016a44.cpp
namespace nJ {
extern "C" {

s32 func_01ffcb0c(s32 a, s32 b);
s32 Math_Atan2(s32 a, s32 b);
}
}

// ---- unk_0201745c.cpp
namespace nK {
extern "C" {
}
}

// ---- unk_02017d74.cpp
namespace nL {
extern "C" {

BOOL Item_IsFurniture(void *p);
s32 Scene_GetCurrent(void);
static inline BOOL Unk_02017d74_Is(u16 *p, u16 v) {
    if (Item_IsFurniture(p)) {
        u16 t = v;
        s32 x = Item_GetFurnitureIndex(p);
        return x == Item_GetFurnitureIndex(&t) ? TRUE : FALSE;
    }
    return *p == v ? TRUE : FALSE;
}
}
}

// ---- unk_02018698.cpp
namespace nM {
extern "C" {

}
}

// ---- unk_02019020.cpp
namespace nN {
extern "C" {

void MI_CpuCopy8(const void *src, void *dst, u32 size);
extern NpcActionEntry sNpcActionTable[];
}
}

// ---- unk_02019998.cpp
namespace nO {
extern "C" {

u8 *NNS_G3dGetAnmByIdx(void *p, s32 v);
void NpcTexPatBufRef_LoadFile(s32 *p, void *path);
void *NpcTexPatBufRef_GetBuffer(s32 *p);
void MI_CpuCopy8(const void *src, void *dst, u32 size);
s32 Vec_Distance(void *a, void *b);
extern Unk_0201a1e0_Fn sNpcLookAtTypes[6];
}
}

// ---- unk_0201a334.cpp
namespace nP {
extern "C" {

extern u8 gFieldSceneKind;
extern s16 data_02135f44[];
s32 Vec_Distance(void *a, void *b);
s32 Vec_NotEqual(void *a, void *b);
s32 Math_Atan2(s32 a, s32 b);
s32 Math_AngleDiffAbs(s32 a, s32 b);
s32 Math_StepS32Alt(s32 *p, s32 a, s32 b);
s32 func_01ffcb0c(s32 a, s32 b);
s32 Ground_GetExitAtPos(VecFx32 *p);
s32 Collision_HasUnitShapeAt(VecFx32 *p);
void GroundInfo_Destruct(void *buf);
static inline BOOL Unk_0201a834_IsOne(u8 v) {
    return v == 1 ? TRUE : FALSE;
}
}
}

// ---- unk_0201ac80.cpp
namespace nQ {
extern "C" {

extern BlockMap *gSceneBlockMap;
extern CommManager *gCommManager;
void MI_CpuCopy8(const void *src, void *dst, u32 size);
s32 TownMap_IsPosWalkable(VecFx32 *v, s32 a);
void NpcActor_FindFreeUnitNear(V3 *out, NpcActor *self, V3 *in);
}
}

// ---- unk_0201b690.cpp
namespace nR {
extern "C" {

extern CommManager *gCommManager;
extern FxVec3 sNpcAvoidOffsets[2];
extern s16 data_02135f44[];
void MI_CpuCopy8(const void *src, void *dst, u32 size);
u8 Npc_GetInfoByte2(void *p);
s32 Scene_GetCurrent();
s32 Vec_Distance(void *a, void *b);
BOOL Item_IsFurniture(void *p);
BOOL Scene_InMuseumRoom();
s32 Scene_GetMuseumRoom();
void JointCb_UseRestTranslation(void *self, u32 flags);
void MTX_RotY33_(MtxFx33 *m, s32 a, s32 b);
void MTX_RotX33_(MtxFx33 *m, s32 a, s32 b);
void NpcActor_JointCalcLayer3Cb(NNSG3dRS *p);
void NpcActor_SetJointCallbackNext(NNSG3dRS *self);
void NpcActor_SetJointCallbackNext(NNSG3dRS *self);
void NpcActor_OnJointCalc(NNSG3dRS *self);
}
}

extern "C" {
}

namespace nR {
extern "C" void NpcActor_OnJointCalc(NNSG3dRS *self) {
    u32 idx = self->c[1];
    NpcActor *p = (NpcActor *)self->pRenderObj->ptrUser;
    u16 tmp[2];
    MtxFx33 mB;
    MtxFx33 mA;
    VecFx32 va;
    MtxFx33 mC;
    VecFx32 vb;
    if (p != NULL) {
        BOOL isX;
        if (Item_IsFurniture(&p->npcHandle) != 0) {
            tmp[0] = 0xd011;
            isX = Item_GetFurnitureIndex(&p->npcHandle) == Item_GetFurnitureIndex(&tmp[0]) ? TRUE : FALSE;
        } else {
            isX = p->npcHandle == 0xd011 ? TRUE : FALSE;
        }
        if (isX && (Scene_InMuseumRoom() == 0 || Scene_GetMuseumRoom() != 1)) {
            if (idx == 0x10) {
                u8 *base = self->pResNodeInfo;
                u16 off = *(u16 *)(base + 6);
                u8 *t = base + off;
                u32 n = *(u16 *)t;
                s32 o = *(s32 *)(t + n * idx + 4);
                s32 *v = (s32 *)(base + o);
                s32 *w = v + 1;
                NNSG3dJntAnmResult *d = self->pJntAnmResult;
                d->trans.x = v[1];
                d->trans.y = w[1];
                d->trans.z = w[2];
            }
        } else {
            JointCb_UseRestTranslation(self, 0x800);
        }
    }
    if (idx == (u32)data_020c6d20 && p != NULL && p->lookAt.lookType < 6) {
        MtxFx33 *m = (MtxFx33 *)&self->pJntAnmResult->rot;
        if (p->lookAt.pitch != 0) {
            u32 a = (u16)p->lookAt.pitch >> 4;
            MTX_RotY33_(&mA, data_02135f44[a * 2], data_02135f44[a * 2 + 1]);
            MTX_Concat33(m, &mA, m);
        }
        if (p->lookAt.yaw != 0) {
            u32 a = (u16)p->lookAt.yaw >> 4;
            MTX_RotX33_(&mB, data_02135f44[a * 2], data_02135f44[a * 2 + 1]);
            MTX_Concat33(m, &mB, m);
        }
    }
    if (idx == 0 && p != NULL) {
        BOOL isY;
        if (Item_IsFurniture(&p->npcHandle) != 0) {
            tmp[1] = 0xd016;
            isY = Item_GetFurnitureIndex(&p->npcHandle) == Item_GetFurnitureIndex(&tmp[1]) ? TRUE : FALSE;
        } else {
            isY = p->npcHandle == 0xd016 ? TRUE : FALSE;
        }
        if (isY) {
            NNSG3dJntAnmResult *d = self->pJntAnmResult;
            MtxFx33 *m = (MtxFx33 *)&d->rot;
            VecFx32 *pv = &d->trans;
            s32 r;
            va = *pv;
            vb = *pv;
            r = WorldCurve_Apply(&va, &vb);
            pv->z = va.z;
            pv->y = va.y - WorldCurve_GetRadius();
            if (r != 0) {
                u32 a = (u16)r >> 4;
                MTX_RotX33_(&mC, data_02135f44[a * 2], data_02135f44[a * 2 + 1]);
                MTX_Concat33(m, &mC, m);
            }
        }
    }
    if (p != NULL) {
        ((ThreeLayerAnimModel *)(&p->model))->onJointCalcPostLayer3((NNSG3dRS *)(self));
    }
    self->cbVecFunc[6] = (void *)NpcActor_SetJointCallbackNext;
    self->cbVecTiming[6] = 3;
}
}

namespace nR {
extern "C" void NpcActor_SetJointCallbackNext(NNSG3dRS *self) {
    self->cbVecFunc[6] = (void *)NpcActor_JointCalcLayer3Cb;
    self->cbVecTiming[6] = 1;
}
}

BOOL NpcActor::loadModel() {
    using namespace nR;
    u8 *t = getModelPath();
    BOOL r = FALSE;
    if (((CachedModel *)(&model))->load(t, gNpcModelHeap) != 0) {
        r = TRUE;
    }
    return r;
}

void NpcActor::releaseModel() {
    using namespace nR;
    ((CachedModel *)(&model))->release();
}

u16 NpcActor::getNpcIndex() {
    using namespace nR;
    return npcIndex;
}

void NpcActor::setNpcIndex(u16 v) {
    using namespace nR;
    npcIndex = v;
}

void NpcActor::setNpcHandle(u16 *p) {
    using namespace nR;
    npcHandle = *p;
    setNpcIndex(npcHandle & 0xfff);
    ((Character *)(this))->setCharId(getNpcIndex());
}

void NpcActor::setCollisionRadius(s32 v) {
    using namespace nR;
    collisionRadius = v;
}

namespace nR {
extern "C" BOOL NpcActor_IsFrontAngle(s32 x) {
    if (x < 0) {
        x = -x;
    }
    if (x < 0x4000) {
        return TRUE;
    }
    return FALSE;
}
}

BOOL NpcActor::isPosInFront(s16 *out, VecFx32 *pos) {
    using namespace nR;
    *out = Math_AngleXZ(&position, pos);
    return NpcActor_IsFrontAngle((s16)(*out - rotY));
}

s32 NpcActor::getDistanceTo(NpcActor *other) {
    using namespace nR;
    s32 r = 0;
    if (other != NULL) {
        r = Vec_Distance(&other->position, &position);
    }
    return r;
}

s32 NpcActor::getDistanceToPlayer(u32 id) {
    using namespace nR;
    return getDistanceTo((NpcActor *)PlayerActor_GetCharacter(id));
}

BOOL NpcActor::isNear(NpcActor *other, s32 n) {
    using namespace nR;
    BOOL r = FALSE;
    if (other != NULL) {
        s32 d = getDistanceTo(other);
        if (d < 0) {
            d = -d;
        }
        if (d < n) {
            r = TRUE;
        }
    }
    return r;
}

BOOL NpcActor::isPlayerNear(s32 n, u32 id) {
    using namespace nR;
    return isNear((NpcActor *)PlayerActor_GetCharacter(id), n);
}

s32 NpcActor::getAngleTo(NpcActor *other) {
    using namespace nR;
    s32 r = 0;
    if (other != NULL) {
        r = Math_AngleXZ(&position, &other->position);
    }
    return r;
}

s32 NpcActor::getAngleToPlayer(u32 id) {
    using namespace nR;
    s32 result = 0;
    u8 flag;
    VecFx32 vec;
    flag = 0;
    if (PlayerActor_GetSlotPosXZ(&flag, &vec.x, &vec.z, -1, id) != 0) {
        result = Math_AngleXZ(&position, &vec);
    } else {
        NpcActor *p = (NpcActor *)PlayerActor_GetCharacter(id);
        if (p != NULL) {
            result = getAngleTo(p);
        }
    }
    return result;
}

s16 NpcActor::getRelativeAngleTo(NpcActor *other) {
    using namespace nR;
    return getAngleTo(other) - rotY;
}

Character *NpcActor::getPlayerActor(u32 id) {
    using namespace nR;
    return (Character *)PlayerActor_GetCharacter(id);
}

void NpcActor::setTalkRequest(ActorTalkRequest *p) {
    using namespace nR;
    talkRequest = p;
    if (talkRequest != NULL) {
        (talkRequest)->setOwnerActor((NpcActor *)(this));
    }
}

ActorTalkRequest *NpcActor::getTalkRequest() {
    using namespace nR;
    return talkRequest;
}

s32 NpcActor::getSpeakerGender() {
    using namespace nR;
    s32 r = 2;
    s32 v = getGender();
    switch (v) {
    case 0:
        r = 0;
        break;
    case 1:
        r = 1;
        break;
    }
    return r;
}

void NpcActor::setShirt(u16 *, BOOL) {
    using namespace nR;}

void NpcActor::onJoinTalk() {
    using namespace nR;}

void NpcActor::onLeaveTalk() {
    using namespace nR;}

BOOL NpcActor::getFreeOffsetPos(VecFx32 *out, VecFx32 *p) {
    using namespace nR;
    BOOL result = FALSE;
    VecFx32 v;
    Npc_RotateOffsetXZ(&v, &position, p, moveAngleY);
    if (Npc_IsPosBlocked(&v) != 1) {
        out->x = v.x;
        out->y = v.y;
        out->z = v.z;
        result = TRUE;
    }
    return result;
}

s32 NpcActor::findAvoidPos(VecFx32 *out) {
    using namespace nR;
    s32 r = ((NpcLookAt *)(&obstacleProbe))->getObstacleBits();
    s32 result = 0;
    VecFx32 *pv = (VecFx32 *)&position;
    *out = *pv;
    switch (r) {
    case 3:
        result = 1;
        break;
    case 1:
        if (getFreeOffsetPos(out, &sNpcAvoidOffsets[1]) != 0) {
            result = 2;
        } else {
            result = 1;
        }
        break;
    case 2:
        if (getFreeOffsetPos(out, &sNpcAvoidOffsets[0]) != 0) {
            result = 2;
        } else {
            result = 1;
        }
        break;
    }
    return result;
}

s32 NpcActor::getAct0BAnimA() {
    using namespace nR;
    s32 i = getGender();
    if (i >= 2) {
        i = 0;
    }
    return data_020c6d48[i];
}

s32 NpcActor::getAct0BAnimB() {
    using namespace nR;
    s32 i = getGender();
    if (i >= 2) {
        i = 0;
    }
    return data_020c6d60[i];
}

u16 NpcActor::getEmotion0BlendFrames() {
    using namespace nR;
    return data_020c6cc8[0];
}

void NpcActor::addMood(u32, s32) {
    using namespace nR;}

BOOL NpcActor::isNetOwner() {
    using namespace nR;
    CommManager *g = gCommManager;
    if ((g)->isOnline() != 0 && netSyncOff == 0) {
        u8 *p = NpcNetRecord_GetVar(&npcHandle);
        if (p != NULL && p[0] != 0) {
            if ((p[1] == 4 && NetArea_IsLocalOwner() != 0) || p[1] == g->myAid) {
                return TRUE;
            }
            return FALSE;
        }
        return FALSE;
    }
    return TRUE;
}

void NpcActor::netSetSlots(s32 a, s32 b, s32 c) {
    using namespace nR;
    NpcNetRecord_SetSlotsAndSync(a, b, c, &npcHandle);
}

void NpcActor::netSetSlotsIfOwner(u32 a, u32 b, u32 c, ...) {
    using namespace nR;
    NpcNetRecord_SetSlots(&npcHandle, b, c);
    if (gCommManager->isSlotActive(gCommManager->myAid) != 0) {
        if (isNetOwner() != 0) {
            s32 t = (npcHandle & 0xf000) >> 12;
            if (t == 0xe) {
                CommSyncVar_SetVar(getNpcIndex() + 0xc, &a, 0, 0);
            } else if (t == 0xd) {
                CommSyncVar_SetVar(getNpcIndex() + 0x20, &a, 0, 0);
            }
        }
    }
}

s32 NpcActor::netGetSlots(s32 *a, s32 *b) {
    using namespace nR;
    return NpcNetRecord_GetSlots(a, b, &npcHandle);
}

BOOL NpcActor::netIsTalkLocked() {
    using namespace nR;
    s32 a = 4;
    s32 b = 4;
    if (netGetSlots(&a, &b) != 0) {
        if (b < 4) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL NpcActor::getNetUserBytes(u8 *src, u32 n) {
    using namespace nR;
    if (netSyncOff == 0) {
        u8 *p = NpcNetRecord_GetVar(&npcHandle);
        if (n > 4) {
            n = 4;
        }
        if (p != NULL) {
            MI_CpuCopy8(p + 0xb, src, n);
            return TRUE;
        }
    }
    return FALSE;
}

void NpcActor::setNetUserBytes(void *dst, s32 n) {
    using namespace nR;
    if (n > 4) {
        n = 4;
    }
    MI_CpuCopy8(dst, netUserBytes, n);
}

void NpcActor::netSendState(u32 a, ...) {
    using namespace nR;
    NpcNetRecord_SetState(&npcHandle, Scene_GetCurrent(), &position, rotY, &actionCtrl.netAction, netUserBytes);
    if (gCommManager->isSlotActive(gCommManager->myAid) != 0) {
        if (isNetOwner() != 0) {
            s32 t = (npcHandle & 0xf000) >> 12;
            if (t == 0xe) {
                CommSyncVar_SetVar(getNpcIndex() + 0xc, &a, 0, 0);
            } else if (t == 0xd) {
                CommSyncVar_SetVar(getNpcIndex() + 0x20, &a, 0, 0);
            }
        }
    }
}

BOOL NpcActor::netReadPosition(VecFx32 *pos, s16 *angle) {
    using namespace nR;
    if (netSyncOff == 0) {
        u8 *p = NpcNetRecord_GetVar(&npcHandle);
        if (p != NULL) {
            NetBuf_UnpackPair20(p + 4, (u32 *)&pos->x, (u32 *)&pos->z);
            MI_CpuCopy8(p + 9, angle, 2);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL NpcActor::netReadAction(s32 *a, s32 *b, u8 *c) {
    using namespace nR;
    u8 *p = NpcNetRecord_GetVar(&npcHandle);
    if (p != NULL) {
        *a = p[0xf];
        *b = p[0x10];
        MI_CpuCopy8(p + 0x11, c, 0xd);
        return TRUE;
    }
    return FALSE;
}

u8 NpcActor::isUpdating() {
    using namespace nR;
    return updateEnabled;
}

BOOL NpcActor::preCreate() {
    using namespace nR;
    u16 tmp;
    if (((Character *)(this))->Character::preCreate() == 0) {
        return FALSE;
    }
    tmp = param;
    setNpcHandle(&tmp);
    talkRequest = NULL;
    setCollisionRadius(0xd00);
    ((NpcSpeechState *)(&speechState))->reset();
    ((NpcTalkCtrl *)(&talkCtrl))->reset();
    ((NpcEmotionFx *)(&emotionFx))->reset();
    partnerPlayer = 0;
    ((NpcMoveCtrl *)(&moveCtrl))->reset();
    talkLockHeld = 0;
    MI_CpuFill8(netUserBytes, 0, 4);
    ((SndSeEmitter *)(&seEmitter))->callInitAlt();
    emotionFx.seEmitter = &seEmitter;
    ((NpcFootstepFx *)(&footstepFx))->resetFootsteps();
    collisionEnabled = 1;
    shadowEnabled = 1;
    ((NpcActor *)(this))->resetLastTaughtEmotion();
    updateEnabled = 1;
    drawEnabled = 1;
    curHeldTool = 0;
    ((Character *)(this))->setAreaSynced();
    return TRUE;
}

BOOL NpcActor::onCreate() {
    using namespace nR;
    if (NetArea_IsLocalOwner() != 0) {
        netSetSlots(1, gCommManager->myAid, 4);
    }
    if (loadModel() == 0) {
        return FALSE;
    }
    maxFallSpeed = 0xffffec00;
    gravity = 0;
    ((NpcMoveCtrl *)(&moveCtrl))->setTargetAngle(rotY);
    emotionFx.effectParam = Npc_GetInfoByte2(&npcHandle);
    return TRUE;
}

void NpcActor::postCreate(s32 x) {
    using namespace nR;
    if (x == 2) {
        if (NetArea_IsLocalOwner() == 0) {
            if ((collider.groups & 2) == 0) {
                collider.groups |= 2;
                talkLockHeld = 1;
            }
        }
        if (netSyncOff == 0) {
            netSendState(1);
        }
    }
    ((Character *)(this))->Character::postCreate(x);
}

BOOL NpcActor::updateAct() {
    using namespace nR;
    return TRUE;
}

BOOL NpcActor::onExecute() {
    using namespace nQ;
    NpcNetActionHalves s;
    struct NpcNetActionWords { s32 a, b, x, y, z; } L;
    Mtx43 t;
    u8 buf[0x10];
    VecFx32 v;
    s32 cur;

    NpcActor_UpdateMovement(&moveCtrl, this);
    updateAct();
    if (updateEnabled == 0) {
        return TRUE;
    }
    if (!isNetOwner()) {
        L.a = 0x16;
        L.b = 0;
        if (netReadAction(&L.a, &L.b, buf)) {
            if (L.b == 1 && (u32)L.a <= 2) {
                if (((NpcActionCtrl *)(&actionCtrl))->getAction() != 0x15) {
                    ((NpcActionCtrl *)(&actionCtrl))->requestAction(0x15, 1, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
                }
                if ((u32)(L.a - 1) <= 1) {
                    actionCtrl.netMoveMode = L.a;
                } else {
                    actionCtrl.netMoveMode = 0;
                }
            } else {
                cur = L.a;
                if (cur != ((NpcActionCtrl *)(&actionCtrl))->getAction()) {
                    L.x = 0;
                    s.h2 = 0;
                    s.h4 = 0;
                    L.y = 0;
                    s.h6 = 0;
                    s.h8 = 0;
                    L.b = 3;
                    switch (cur) {
                    case 0:
                        ((NpcActionCtrl *)(&actionCtrl))->requestStand(L.b, data_020c6cc8[0]);
                        break;
                    case 1:
                    case 2:
                        NpcAction_UnpackMove(&L.x, &L.x, &L.y, &L.y, &s.h2, &s.h6, buf);
                        ((NpcActionCtrl *)(&actionCtrl))->requestAction(L.a, L.b, L.x, 0, s.h2, s.h4, L.y, 0, s.h6, 0);
                        break;
                    case 3:
                        NpcAction_UnpackTurn(&s.h2, (u16 *)&s.h4, &s.h6, buf);
                        ((NpcActionCtrl *)(&actionCtrl))->requestAction(L.a, L.b, L.x, 0, s.h2, s.h4, L.y, 0, s.h6, 0);
                        break;
                    case 12:
                        L.z = 0x137;
                        s.b0 = 0;
                        NpcAction_UnpackAnim((u32 *)&L.z, &s.b0, &s.h6, &s.h8, buf);
                        ((NpcActionCtrl *)(&actionCtrl))->requestPlayAnim(L.b, L.z, s.b0, s.h6, s.h8);
                        break;
                    case 20:
                        s.ha = 0xfff1;
                        NpcAction_UnpackItem(&s.ha, buf);
                        ((NpcActionCtrl *)(&actionCtrl))->requestAct14(L.b, &s.ha);
                        break;
                    }
                }
            }
        }
    }
    ((NpcTalkCtrl *)(&talkCtrl))->update((NpcActor *)(this));
    actionCtrl.update(this);
    ((NpcEmotionFx *)(&emotionFx))->update(&jointPos[0], rotY, ((NpcAnimCtrl *)(&animCtrl))->getAnimId(0), ((Unk_0201b2b8_Bits *)&model.curFrame)->mid);
    ((NpcMoveCtrl *)(&moveCtrl))->updateTurn((Character *)(this));
    if (NetArea_IsLocalOwner() && collisionEnabled && collisionRadius > 0) {
        Collision_Move(&collisionState, &position, &prevPosition, rotY, collisionRadius, this, 0xf);
    }
    position.y = Ground_GetDefaultY();
    ((NpcLookAt *)(&lookAt))->update((NpcActor *)(this));
    ((NpcObstacleProbe *)(&obstacleProbe))->clear();
    ((NpcObstacleProbe *)(&obstacleProbe))->probe((Character *)(this));
    drawTilt = WorldCurve_ToCurved(&drawPos, &position);
    ((Actor *)(this))->calcModelMatrix(&t);
    (*(Mtx43 *)&model.mtx) = t;
    if (netIsTalkLocked()) {
        if ((collider.groups & 2) == 0 && NetArea_IsLocalOwner()) {
            collider.groups |= 2;
            talkLockHeld = 1;
        }
    } else if (talkLockHeld == 1 && NetArea_IsLocalOwner()) {
        collider.groups &= ~2;
        talkLockHeld = 0;
    }
    if (collisionEnabled) {
        ((ActorCollider *)(&collider))->submit();
    } else {
        ((ActorCollider *)(&collider))->resetHit();
    }
    ((NpcAnimCtrl *)(&animCtrl))->update((NpcActor *)(this));
    ((ThreeLayerAnimModel *)(&model))->updateLayers3();
    VecFx32 *pp = (VecFx32 *)&position;
    v.x = position.x;
    v.y = pp->y;
    v.z = pp->z;
    ((SndSeEmitter *)(&seEmitter))->callUpdateRelativeAlt(&v);
    if (curHeldTool) {
        ((HeldToolModel *)curHeldTool)->update(this);
    }
    ((NpcFootstepFx *)(&footstepFx))->updateFootsteps((NpcActor *)(this));
    faceAnim.update(this);
    actionCtrl.postUpdate(this);
    if (netSyncOff == 0) {
        netSendState(1);
    }
    return TRUE;
}

BOOL NpcActor::onDraw() {
    using namespace nQ;
    Mtx43 buf;
    VecFx32 t0, t1, t2;
    if (updateEnabled == 0) {
        VecFx32 *p = (VecFx32 *)&position;
        jointPos[0].x = position.x;
        jointPos[0].y = p->y;
        jointPos[0].z = p->z;
        jointPos[1].x = position.x;
        jointPos[1].y = p->y;
        jointPos[1].z = p->z;
        jointPos[2].x = position.x;
        jointPos[2].y = p->y;
        jointPos[2].z = p->z;
        return TRUE;
    }
    if (drawEnabled == 0) {
        VecFx32 *p = (VecFx32 *)&position;
        jointPos[0].x = position.x;
        jointPos[0].y = p->y;
        jointPos[0].z = p->z;
        jointPos[1].x = position.x;
        jointPos[1].y = p->y;
        jointPos[1].z = p->z;
        jointPos[2].x = position.x;
        jointPos[2].y = p->y;
        jointPos[2].z = p->z;
        return TRUE;
    }
    ((TwoLayerAnimModel *)(&model))->drawLayered(0);
    lookAt.storeHeadMtx(this);
    Model_GetJointWorldMtx(&model, &jointMtx, 0xb);
    Model_GetJointWorldMtx(&model, &buf, 0x10);
    t0 = *(VecFx32 *)&buf.m[9];
    WorldCurve_FromCurved(&jointPos[0], &t0);
    Model_GetJointWorldMtx(&model, &buf, 0x7);
    t1 = *(VecFx32 *)&buf.m[9];
    WorldCurve_FromCurved(&jointPos[1], &t1);
    Model_GetJointWorldMtx(&model, &buf, 0x4);
    t2 = *(VecFx32 *)&buf.m[9];
    WorldCurve_FromCurved(&jointPos[2], &t2);
    if (shadowEnabled != 0) {
        CharaShadow_Draw(&position, 0xb00, 0x4000, 0x1000);
    }
    return TRUE;
}

BOOL NpcActor::onDeleteRequest() {
    using namespace nQ;
    return 0; // the original returns whatever r0 holds; callers ignore it
}

BOOL NpcActor::onDelete() {
    using namespace nQ;
    releaseModel();
    ((NpcFaceAnim *)(&faceAnim))->release();
    ((NpcAnimCtrl *)(&animCtrl))->release();
    ((SndSeEmitter *)(&seEmitter))->callStopAlt();
    if (netSyncOff == 0) {
        netSendState(1);
    }
    return TRUE;
}

BOOL NpcActor::getHeldItemPos(VecFx32 *out) {
    using namespace nQ;
    s32 a = jointMtx.m[9];
    if (a == 0 && jointMtx.m[10] == 0 && jointMtx.m[11] == 0) {
        return FALSE;
    }
    out->x = a;
    out->y = jointMtx.m[10];
    out->z = jointMtx.m[11];
    WorldCurve_FromCurved(out, out);
    return TRUE;
}

void NpcActor::onInteractionEvent(u32 v, u8) {
    using namespace nQ;
    if (v == 8) {
        PlayerActor_SetNoFaceTalkTarget(0, 4);
    }
}

BOOL NpcActor::onToolHit(u16 *) {
    using namespace nQ; return 0; }

void *NpcActor::getVillagerData() {
    using namespace nQ; return 0; }

namespace nQ {
extern "C" void NpcActor_FindFreeUnitNear(V3 *out, NpcActor *self, V3 *in) {
    V3 a, b, cand1, cand2, arr[4], tmp, t1, t2, t3, t4;
    u32 m0, m1;
    s32 best, bestDist, d1, i1, dir0, dir1, best2, d, d0, i, j;
    BlockMap *g;
    s32 z0, z1, z2, z3;
    *out = *in;
    g = gSceneBlockMap;
    m0 = g->getWalkLinksAtPos(&self->position);
    m1 = g->getWalkLinksAtPos(in);
    FieldPos_SnapToUnitCenter(&a, &self->position);
    FieldPos_SnapToUnitCenter(&b, in);
    best = 0;
    a.y = 0;
    b.y = 0;
    if (m0 == 0) {
        bestDist = best;
        i1 = z0 = best;
        for (i1 = i1; i1 < 4; i1++) {
            Vec_Add(&t1, &a, &data_020c6dcc[i1]);
            cand1 = t1;
            d1 = Vec_DistXZ(&self->position, &cand1);
            if (g->getWalkLinksAtPos(&cand1) && TownMap_IsPosWalkable(&cand1, z0)) {
                if (bestDist == 0 || bestDist > d1) {
                    bestDist = d1;
                    best = i1;
                }
            }
        }
        if (bestDist != 0) {
            Vec_Add(&t2, &a, &data_020c6dcc[best]);
            *out = t2;
        } else {
            *out = *in;
        }
    } else {
        best2 = best;
        d0 = Vec_DistXZ(in, &a);
        func_02133ef8(arr, 0x30);
        u32 ang = (u16)self->rotY;
        if (ang >= 0xe000 || ang < 0x2000) {
            dir0 = 2;
            dir1 = 0;
        } else if (ang < 0x6000) {
            dir0 = 3;
            dir1 = 1;
        } else if (ang < 0xa000) {
            dir0 = 0;
            dir1 = 2;
        } else {
            dir0 = 1;
            dir1 = 3;
        }
        if (((CommManager *)(gCommManager))->isOnline()) {
            u32 k;
            z1 = 0;
            for (k = 0; k < 4; k++) {
                V3 *p = (V3 *)PlayerActor_GetBodyPos(k);
                if (p) {
                    FieldPos_SnapToUnitCenter(&tmp, p);
                    tmp.y = z1;
                    arr[k] = tmp;
                }
            }
        }
        z2 = z3 = 0;
        for (i = z2; i < 4; i++) {
            Vec_Add(&t3, &a, &data_020c6dcc[i]);
            cand2 = t3;
            d = Vec_DistXZ(in, &cand2);
            if (i == dir0) {
                d -= 0x2000;
            } else if (i == dir1) {
                d += 0x2000;
            }
            for (j = z2; j < 4; j++) {
                Vec_Equal(&cand2, &arr[j]);
            }
            if (TownMap_IsPosWalkable(&cand2, z3) && (m0 & data_020c6d84[i])) {
                if (best2 == 0 || best2 > d) {
                    best2 = d;
                    best = i;
                }
            }
        }
        if (m1 == 0 && best2 > d0) {
            *out = self->position;
        } else {
            Vec_Add(&t4, &a, &data_020c6dcc[best]);
            *out = t4;
        }
    }
}
}

namespace nQ {
extern "C" BOOL NpcActor_CanPlayerPay(void *self, s32 v) {
    return PlayerInventory_CanAddBells((u8 *)PlayerData_GetCurrent()->getInventory(), -v, 1, 0);
}
}

namespace nQ {
extern "C" void NpcActor_ChargePlayer(void *self, s32 v) {
    PlayerInventory_AddBells((u8 *)PlayerData_GetCurrent()->getInventory(), -v, 1);
}
}

namespace nQ {
extern "C" s32 NpcActor_CheckPayoutFits(void *self, s32 v, s32 w) {
    u8 *p = PlayerData_GetCurrent() ? (u8 *)PlayerData_GetCurrent()->getInventory() : 0;
    s32 lo = p ? PlayerInventory_GetBellsRoom(p, 0, 0) : 0;
    s32 hi = p ? PlayerInventory_GetBellsRoom(p, 1, w) : 0;
    s32 r = 2;
    if (v <= lo) {
        r = 0;
    } else if (v <= hi) {
        r = 1;
    }
    return r;
}
}

namespace nQ {
extern "C" void NpcActor_PayPlayer(void *self, s32 v) {
    PlayerInventory_AddBells((u8 *)PlayerData_GetCurrent()->getInventory(), v, 1);
}
}

NpcMoveAnimSet::NpcMoveAnimSet() {
    using namespace nQ;
    standAnim = 0;
    walkAnim = 1;
    runAnim = 2;
}

void NpcMoveAnimSet::func_0201ad38() {
    using namespace nQ;}

void NpcMoveAnimSet::setStandAnim(s32 v) {
    using namespace nQ; standAnim = v; }

void NpcMoveAnimSet::setWalkAnim(s32 v) {
    using namespace nQ; walkAnim = v; }

void NpcMoveAnimSet::setRunAnim(s32 v) {
    using namespace nQ; runAnim = v; }

s32 NpcMoveAnimSet::getStandAnim() {
    using namespace nQ; return standAnim; }

s32 NpcMoveAnimSet::getWalkAnim() {
    using namespace nQ; return walkAnim; }

s32 NpcMoveAnimSet::getRunAnim() {
    using namespace nQ; return runAnim; }

Unk_0201ad18::Unk_0201ad18() {
    using namespace nQ;
    unk_00 = 0;
    unk_02 = 0;
}

s32 Unk_0201acf8::getLevel() {
    using namespace nQ;
    s32 r = 2;
    u32 v = unk_00;
    if (v < 100) {
        r = 0;
    } else if (v < 0x320) {
        r = 1;
    }
    return r;
}

void Unk_0201acf8::func_0201acf8(u16 v) {
    using namespace nQ; unk_02 = v; }

NpcMoveCtrl::NpcMoveCtrl() {
    using namespace nQ;
    curSpeedPreset.x = 0x1000;
    curSpeedPreset.y = 0;
    curSpeedPreset.z = 0;
    moveMode = 5;
    waypoint.x = 0;
    waypoint.y = 0;
    waypoint.z = 0;
    destination.x = 0;
    destination.y = 0;
    destination.z = 0;
    keepAnimFrame = 0;
    turnMode = 0;
}

void NpcMoveCtrl::func_0201acc8() {
    using namespace nQ;}

void NpcMoveCtrl::reset() {
    using namespace nQ;
    curSpeedPreset.x = 0x1000;
    curSpeedPreset.y = 0;
    curSpeedPreset.z = 0;
    moveMode = 5;
    waypoint.x = 0;
    waypoint.y = 0;
    waypoint.z = 0;
    destination.x = 0;
    destination.y = 0;
    destination.z = 0;
    keepAnimFrame = 0;
    turnMode = 0;
    MI_CpuCopy8(sNpcMoveSpeedPresets, speedPresets, 0x24);
}

namespace nQ {
extern "C" void NpcActor_UpdateMovement(void *a, void *b) { ((NpcMoveCtrl *)(a))->applyMovement((Character *)(b)); }
}

void NpcMoveCtrl::applyMovement(Character *scene) {
    using namespace nP;
    s32 lo = curSpeedPreset.x;
    if (lo == 0) {
        scene->speed = 0;
    } else {
        s32 hi = curSpeedPreset.y;
        if (hi >= lo) {
            hi = curSpeedPreset.z;
        }
        Math_StepS32Alt(&scene->speed, lo, hi);
        s32 ang = Math_AngleXZ(&scene->position, &destination);
        if (ang == scene->moveAngleY) {
            s32 d = Vec_DistXZ(&destination, (VecFx32 *)&scene->position);
            if (d < scene->speed) {
                scene->speed = d;
            }
        }
    }
    scene->updatePosition(&static_cast<NpcActor *>(scene)->collider);
}

void NpcMoveCtrl::setMoveMode(Character *scene, s32 mode, s16 ang, u16 extra) {
    using namespace nP;
    if (mode < 0 || mode >= 5) {
        mode = 0;
    }
    const NpcMoveModeEntry *e = &sNpcMoveModeTable[mode];
    VecFx32 *src = &speedPresets[e->speedPresetIdx];
    curSpeedPreset.x = src->x;
    curSpeedPreset.y = src->y;
    curSpeedPreset.z = src->z;
    if (ang == 0) {
        turnSpeed = e->defaultTurnSpeed;
    } else {
        turnSpeed = ang;
    }
    arriveDistance = e->arriveDistance;
    if (mode != 4) {
        if ((keepAnimFrame == 1 && e->keepAnimFrame == 1) || static_cast<NpcActor *>(scene)->animCtrl.isPlayingAnim(e->animKind, &static_cast<NpcActor *>(scene)->moveAnimSet)) {
            static_cast<NpcActor *>(scene)->animCtrl.playAnimKeepFrame((NpcActor *)(scene), e->animKind, extra, 0);
        } else {
            static_cast<NpcActor *>(scene)->animCtrl.playAnim((NpcActor *)(scene), e->animKind, extra, 0, 0x1000, 0, 0);
        }
    }
    moveMode = mode;
    keepAnimFrame = e->keepAnimFrame;
}

s32 NpcMoveCtrl::getMoveMode() {
    using namespace nP;
    return moveMode;
}

void NpcMoveCtrl::aimAtDestination(Character *scene) {
    using namespace nP;
    targetAngle = Math_AngleXZ(&scene->position, &destination);
}

s32 NpcMoveCtrl::stepAngle(s16 *p, s16 target, s16 step, u8 mode) {
    using namespace nP;
    s32 lim;
    s32 r;
    s16 t;
    if (step < 0) {
        lim = (s16)-step;
    } else {
        lim = step;
    }
    r = 0;
    if (step != 0) {
        switch (mode) {
        case 1:
        case 2:
            if (Math_AngleDiffAbs(target, *p) <= lim) {
                if ((s16)(*p - target) > 0) {
                    step = (s16)-step;
                }
            } else {
                if (mode == 1) {
                    step = (s16)lim;
                } else {
                    step = (s16)-lim;
                }
            }
            break;
        default:
            if ((s16)(*p - target) > 0) {
                step = (s16)-step;
            }
            break;
        }
        *p = *p + step;
        s32 x = *p - target;
        s32 d = (s16)x;
        if (d < 0) { d = (s16)-d; }
        s32 m = step < 0 ? (s32)(s16)-step : step;
        if (d <= m && (s16)x * step >= 0) {
            *p = target;
            r = 1;
        }
    } else if (*p == target) {
        r = 1;
    }
    return r;
}

void NpcMoveCtrl::updateTurn(Character *scene) {
    using namespace nP;
    if (getMoveMode() == 4) {
        scene->moveAngleY = targetAngle;
    } else {
        stepAngle(&scene->moveAngleY, targetAngle, turnSpeed, turnMode);
        scene->rotY = scene->moveAngleY;
    }
}

void NpcMoveCtrl::setWaypoint(VecFx32 *v) {
    using namespace nP;
    if (Vec_Equal(&waypoint, &destination)) {
        setDestination(v);
    }
    waypoint.x = v->x;
    waypoint.y = v->y;
    waypoint.z = v->z;
}

VecFx32 *NpcMoveCtrl::getDestinationB() {
    using namespace nP;
    return &destination;
}

BOOL NpcMoveCtrl::hasArrived(Character *scene, s32 which) {
    using namespace nP;
    VecFx32 v;
    BOOL r;
    VecFx32 *sp = (VecFx32 *)&scene->position;
    v.x = sp->x;
    v.y = sp->y;
    v.z = sp->z;
    r = FALSE;
    v.y = 0;
    s32 d;
    if (which != 0) {
        d = Vec_Distance(&v, &waypoint);
    } else {
        d = Vec_Distance(&v, &destination);
    }
    if (d < arriveDistance) {
        r = TRUE;
    }
    return r;
}

void NpcMoveCtrl::setTargetAngle(s16 v) {
    using namespace nP;
    targetAngle = v;
}

s32 NpcMoveCtrl::getTargetAngle() {
    using namespace nP;
    return targetAngle;
}

s32 NpcMoveCtrl::getTurnSpeed() {
    using namespace nP;
    return turnSpeed;
}

void NpcMoveCtrl::setDestination(VecFx32 *v) {
    using namespace nP;
    destination.x = v->x;
    destination.y = v->y;
    destination.z = v->z;
}

VecFx32 *NpcMoveCtrl::getDestination() {
    using namespace nP;
    return &destination;
}

s32 NpcMoveCtrl::hasNextLeg() {
    using namespace nP;
    return Vec_NotEqual(&waypoint, &destination);
}

namespace nP {
extern "C" void Npc_RotateOffsetXZ(VecFx32 *out, VecFx32 *base, VecFx32 *off, u32 ang) {
    s32 i = ((u16)ang >> 4) << 1;
    s32 s = data_02135f44[i];
    s32 c = data_02135f44[i + 1];
    s32 a = func_01ffcb0c(off->x, c);
    out->x = base->x + a + func_01ffcb0c(off->z, s);
    out->y = 0;
    s32 d = func_01ffcb0c(off->x, s);
    out->z = base->z - d + func_01ffcb0c(off->z, c);
}
}

void NpcMoveCtrl::resetDestination() {
    using namespace nP;
    destination.x = waypoint.x;
    destination.y = waypoint.y;
    destination.z = waypoint.z;
}

void NpcMoveCtrl::setSpeedPreset(s32 idx, s32 x, s32 y, s32 z) {
    using namespace nP;
    if (idx >= 1 && idx < 3) {
        VecFx32 *e = &speedPresets[idx];
        e->x = x;
        e->y = y;
        e->z = z;
    }
}

void NpcMoveCtrl::getCurSpeedPreset() {
    using namespace nP;
}

void NpcMoveCtrl::setTurnMode(u8 v) {
    using namespace nP;
    turnMode = v;
}

NpcObstacleProbe::NpcObstacleProbe() {
    using namespace nP;
    blockedBits = 0;
}

void NpcObstacleProbe::clear() {
    using namespace nP;
    blockedBits = 0;
}

namespace nP {
extern "C" BOOL Npc_IsPosBlocked(VecFx32 *pos) {
    u32 buf[16];
    if (Unk_0201a834_IsOne(gFieldSceneKind)) {
        if (Ground_GetExitAtPos(pos) != -1) {
            return TRUE;
        }
        if (FtrMgr_GetSurfaceHeightAtPos(pos)) {
            return TRUE;
        }
        return FALSE;
    }
    ((GroundInfo *)(buf))->initAtPos(pos, 0, 0);
    s32 r;
    if (buf[12] != 0 || ((GroundInfoBase *)(buf))->getHeight(1) > 0 || Collision_HasUnitShapeAt(pos)) {
        r = 1;
    } else {
        r = 0;
    }
    GroundInfo_Destruct(buf);
    return r;
}
}

void NpcObstacleProbe::probe(Character *scene) {
    using namespace nP;
    for (s32 i = 0; i < 2; i++) {
        VecFx32 v;
        Npc_RotateOffsetXZ(&v, (VecFx32 *)&scene->position, &sNpcObstacleProbeOffsets[i], scene->moveAngleY);
        if (Npc_IsPosBlocked(&v)) {
            blockedBits |= 1 << i;
        }
    }
}

u8 NpcLookAt::getObstacleBits() {
    using namespace nP;
    return lookType;
}

NpcLookAt::NpcLookAt() {
    using namespace nP;
    lookType = 1;
    targetActor = 0;
    targetPos.x = 0;
    targetPos.y = 0;
    targetPos.z = 0;
    priority = 0;
    pitch = 0;
    pitchStep = 0x200;
    manualPitch = 0;
    pitchLimit = 0x1a00;
    yaw = 0;
    yawStep = 0x400;
    manualYaw = 0;
    yawLimit = 0x3000;
    disabled = 0;
    onTarget = 0;
    maxDistance = 0x8000;
    useYawLimit = 1;
    targetPlayer = 4;
}

void NpcLookAt::clearTargetActor() {
    using namespace nP;
    targetActor = 0;
}

void NpcLookAt::disable() {
    using namespace nP;
    disabled = 1;
}

void NpcLookAt::storeHeadMtx(NpcActor *npc) {
    using namespace nP;
    if (npc != 0) {
        Model_GetJointWorldMtx(&npc->model, &headMtx, 0xf);
    }
}

namespace nP {
extern "C" s32 NpcLookAt_GetHeadPos(NpcLookAt *self, VecFx32 *out) {
    s32 r = 0;
    s32 x = self->headMtx.m[9];
    if (x != 0 || self->headMtx.m[10] != 0 || self->headMtx.m[11] != 0) {
        out->x = x;
        out->y = self->headMtx.m[10];
        out->z = self->headMtx.m[11];
        WorldCurve_FromCurved(out, out);
        r = 1;
    }
    return r;
}
}

void NpcLookAt::setPitchLimit(s16 v) {
    using namespace nP;
    pitchLimit = v;
}

void NpcLookAt::setTargetPos(VecFx32 *v) {
    using namespace nP;
    targetPos.x = v->x;
    targetPos.y = v->y;
    targetPos.z = v->z;
}

void NpcLookAt::setTarget(u8 type, s32 pri, Character *tgt, VecFx32 *v, s32 h, s32 lim, u8 flag) {
    using namespace nP;
    if (disabled == 0 && priority != 4 && pri >= priority) {
        lookType = type;
        targetActor = tgt;
        targetPos.x = v->x;
        targetPos.y = v->y;
        targetPos.z = v->z;
        priority = pri;
        onTarget = 0;
        maxDistance = lim;
        useYawLimit = flag;
        targetPlayer = h;
        manualPitch = 0;
        manualYaw = 0;
        pitchStep = 0x200;
        yawStep = 0x400;
    }
}

void NpcLookAt::setManualAngles(s32 pri, s16 a, s16 b, s16 c, s16 d) {
    using namespace nP;
    if (disabled == 0 && priority != 4 && pri >= priority) {
        lookType = 5;
        targetActor = 0;
        targetPos.x = 0;
        targetPos.y = 0;
        targetPos.z = 0;
        priority = pri;
        onTarget = 0;
        maxDistance = 0x8000;
        useYawLimit = 1;
        targetPlayer = 4;
        manualPitch = a;
        manualYaw = b;
        pitchStep = c;
        yawStep = d;
    }
}

BOOL NpcLookAt::canSeeTarget(Character *scene) {
    using namespace nP;
    volatile VecFx32 z;
    VecFx32 *p = 0;
    z.x = 0;
    z.y = 0;
    z.z = 0;
    switch (lookType) {
    case 1: {
        Character *t = (Character *)PlayerActor_GetCharacter(targetPlayer);
        if (t != 0) {
            p = (VecFx32 *)&t->position;
        }
        break;
    }
    case 3:
        p = &targetPos;
        break;
    }
    if (p != 0 && Vec_NotEqual(p, &gVec3Zero) != 0) {
        s32 d = Vec_Distance(p, (VecFx32 *)&scene->position);
        if (maxDistance == 0 || (d < 0 ? -d : d) < maxDistance) {
            s32 ang = calcYaw(p, (VecFx32 *)&scene->position, scene->rotY);
            if (useYawLimit == 0 || (this)->isWithinYawLimit(ang) != 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

s32 NpcLookAt::calcYaw(VecFx32 *a, VecFx32 *b, s32 c) {
    using namespace nP;
    return (s16)(Math_AngleXZ(b, a) - c);
}

s32 NpcLookAt::calcPitch(VecFx32 *a, VecFx32 *b, s32 c) {
    using namespace nP;
    s32 d = Vec_Distance(a, b);
    return (s16)(Math_Atan2(a->y - b->y, d) - c);
}

s32 NpcLookAt::calcClampedYaw(VecFx32 *a, VecFx32 *b, s32 c) {
    using namespace nP;
    return clampYaw(calcYaw(a, b, c));
}

s32 NpcLookAt::clampYaw(s32 v) {
    using namespace nP;
    s32 m = v < 0 ? -v : v;
    s32 lim = yawLimit;
    if (m > lim) {
        if (v > 0) {
            v = lim;
        } else {
            v = (s16)-lim;
        }
    }
    return v;
}

s32 NpcLookAt::calcClampedPitch(VecFx32 *a, VecFx32 *b, s32 c) {
    using namespace nP;
    return clampPitch(calcPitch(a, b, c));
}

s32 NpcLookAt::clampPitch(s32 v) {
    using namespace nP;
    s32 m = v < 0 ? -v : v;
    s32 lim = pitchLimit;
    if (m > lim) {
        if (v >= 0) {
            v = lim;
        } else {
            v = (s16)-lim;
        }
    }
    return v;
}

void NpcLookAt::lookAtActor(Character *scene, Character *tgt, VecFx32 *v, s32 limit, u8 flag) {
    using namespace nP;
    VecFx32 *tp = (VecFx32 *)&tgt->position;
    VecFx32 *sp = (VecFx32 *)&scene->position;
    s32 a = 0;
    s32 b = 0;
    onTarget = 0;
    if (tp != 0) {
        s32 d = Vec_Distance(tp, sp);
        if (limit == 0 || (d < 0 ? -d : d) < limit) {
            s32 ang = calcYaw(tp, sp, scene->rotY);
            if (!flag || (this)->isWithinYawLimit(ang)) {
                VecFx32 w;
                if (NpcLookAt_GetHeadPos((NpcLookAt *)this, &w)) {
                    a = calcClampedPitch(v, &w, 0);
                }
                b = clampYaw(ang);
                onTarget = 1;
            }
        }
    }
    if (yaw != b) {
        Math_StepAngle(&yaw, b, yawStep);
    }
    if (pitch != a) {
        Math_StepAngle(&pitch, a, pitchStep);
    }
    if (yaw != b || pitch != a) {
        onTarget = 0;
    }
}

void NpcLookAt::relax(Character *) {
    using namespace nP;
    if (yaw != 0) {
        Math_StepAngle(&yaw, 0, yawStep);
    }
    if (pitch != 0) {
        Math_StepAngle(&pitch, 0, pitchStep);
    }
}

void NpcLookAt::lookAtPlayer(Character *scene, s32 h, s32 limit, u8 flag) {
    using namespace nP;
    Character *t = (Character *)PlayerActor_GetCharacter(h);
    if (t != 0) {
        VecFx32Ctor v;
        if (PlayerActor_GetHeadPos(&v, h)) {
            VecFx32 w;
            w.x = v.x;
            w.y = v.y;
            w.z = v.z;
            lookAtActor(scene, t, &w, limit, flag);
        }
    }
}

void NpcLookAt::lookAtTargetPlayer(Character *scene) {
    using namespace nP;
    lookAtPlayer(scene, targetPlayer, maxDistance, useYawLimit);
}

void NpcLookAt::lookAtLocalPlayer(Character *scene) {
    using namespace nP;
    lookAtPlayer(scene, 4, maxDistance, useYawLimit);
}

void NpcLookAt::lookAtTargetActor(Character *scene) {
    using namespace nP;
    if (targetActor != 0) {
        VecFx32 v;
        if (NpcLookAt_GetHeadPos(&static_cast<NpcActor *>(targetActor)->lookAt, &v)) {
            VecFx32 w;
            w.x = v.x;
            w.y = v.y;
            w.z = v.z;
            lookAtActor(scene, (Character *)targetActor, &w, maxDistance, useYawLimit);
        }
    }
}

void NpcLookAt::lookAtPoint(Character *o) {
    using namespace nO;
    s32 r6 = 0;
    s32 r7 = 0;
    VecFx32 sp;
    s32 d = Vec_Distance(&targetPos, (VecFx32 *)&o->position);
    s32 lim;
    onTarget = 0;
    lim = maxDistance;
    if (lim == 0 || (d < 0 ? -d : d) < lim) {
        s32 t = ((NpcLookAt *)(this))->calcYaw((VecFx32 *)(&targetPos), (VecFx32 *)((VecFx32 *)&o->position), o->rotY);
        if (useYawLimit == 0 || isWithinYawLimit(t)) {
            r6 = ((NpcLookAt *)(this))->calcClampedYaw((VecFx32 *)(&targetPos), (VecFx32 *)((VecFx32 *)&o->position), o->rotY);
            if (NpcLookAt_GetHeadPos(this, &sp)) {
                r7 = ((NpcLookAt *)(this))->calcClampedPitch((VecFx32 *)(&targetPos), &sp, o->rotX);
                onTarget = 1;
            }
        }
    }
    if (yaw != r6) {
        Math_StepAngle(&yaw, r6, yawStep);
    }
    if (pitch != r7) {
        Math_StepAngle(&pitch, r7, pitchStep);
    }
    if (yaw != r6 || pitch != r7) {
        onTarget = 0;
    }
}

void NpcLookAt::approachManualAngles(Character *) {
    using namespace nO;
    if (yaw != manualYaw) {
        Math_StepAngle(&yaw, manualYaw, yawStep);
    }
    if (pitch != manualPitch) {
        Math_StepAngle(&pitch, manualPitch, pitchStep);
    }
}

void NpcLookAt::update(NpcActor *base) {
    using namespace nO;
    if (disabled == 0 && lookType < 6) {
        Unk_0201a1e0_Fn *pf = &sNpcLookAtTypes[lookType];
        NpcLookAt *t = &base->lookAt;
        (t->**pf)(base);
    }
}

namespace nO {
extern "C" BOOL NpcLookAt_IsWithin(s32 a, s32 b) {
    if (a < 0) {
        a = -a;
    }
    if (a < b) {
        return TRUE;
    }
    return FALSE;
}
}

BOOL NpcLookAt::isWithinYawLimit(s32 v) {
    using namespace nO;
    return NpcLookAt_IsWithin(v, yawLimit);
}

BOOL NpcLookAt::isOnTarget() {
    using namespace nO;
    if (disabled == 0 && onTarget == 1) {
        return TRUE;
    }
    return FALSE;
}

NpcSpeechState::NpcSpeechState() {
    using namespace nO;
    reset();
}

NpcSpeechState::~NpcSpeechState() {
    using namespace nO;}

void NpcSpeechState::reset() {
    using namespace nO;
    speaking = 0;
    mouthType = 2;
}

void NpcSpeechState::startSpeaking() {
    using namespace nO;
    speaking = 1;
}

void NpcSpeechState::stopSpeaking() {
    using namespace nO;
    speaking = 0;
}

BOOL NpcSpeechState::isSpeaking() {
    using namespace nO;
    if (speaking == 1) {
        return TRUE;
    }
    return FALSE;
}

void NpcSpeechState::setMouthType(s32 v) {
    using namespace nO;
    mouthType = v;
}

s32 NpcSpeechState::getMouthType() {
    using namespace nO;
    return mouthType;
}

namespace nO {
extern "C" s32 NpcFace_PickTalkMouth() {
    s32 r = Random_GlobalBelow(4);
    s32 v = 1;
    if (r & v) {
        v = 0;
    }
    return v;
}
}

NpcEmotionFx::NpcEmotionFx() {
    using namespace nO;}

NpcEmotionFx::~NpcEmotionFx() {
    using namespace nO;}

void NpcEmotionFx::reset() {
    using namespace nO;
    MI_CpuFill8(this, 0xff, 0x10);
    clearSlots(slots, 2);
    slotAnimId = 0x137;
    seMode = 2;
    emotionId = 0x3c;
    effectParam = 0;
    useGlobalSe = 0;
}

void NpcEmotionFx::clearSlots(void *p, s32 n) {
    using namespace nO;
    NpcEmotionFxSlot *q = (NpcEmotionFxSlot *)p;
    s32 zero = 0;
    for (s32 i = 0; i < n; i++) {
        MI_CpuFill8(q, zero, 4);
        q->effectId = ~zero;
        q++;
    }
}

void NpcEmotionFx::copySlots(void *dst, const void *src, s32 n) {
    using namespace nO;
    MI_CpuCopy8(src, dst, n << 2);
}

s32 NpcEmotionFx::findFreeHandle() {
    using namespace nO;
    s32 r, i;
    i = 0;
    r = ~i;
    for (; i < 4; i++) {
        if (effects[i] == r) {
            r = i;
            break;
        }
    }
    return r;
}

void NpcEmotionFx::setSlots(const void *a, u32 b, s32 c) {
    using namespace nO;
    clearSlots(slots, 2);
    copySlots(slots, a, b);
    slotAnimId = c;
}

void NpcEmotionFx::startEntry(const NpcEmotionPhase *tbl, s32 idx) {
    using namespace nO;
    const NpcEmotionPhase *e = &tbl[idx];
    if (e->killPrevFx != 0) {
        killEffects();
    }
    if (e->fxSlots != 0) {
        setSlots(e->fxSlots, e->numFxSlots, e->animId);
        entryPart = idx;
    }
}

void NpcEmotionFx::playSound(NpcEmotionFxSlot *s) {
    using namespace nO;
    const NpcEmotionEntry *bank = Emotion_GetEntry(emotionId);
    if (s->repeat == 1 || seMode == 2) {
        seMode = bank->seMode;
        if (seMode == 1) {
            if (useGlobalSe == 0) {
                Snd_SeEmitterPlayHeld(seEmitter, emotionId + 0x84, 0x7f, 0);
            } else {
                Snd_PlayAuxSeHeld(emotionId + 0x84);
            }
        } else {
            if (useGlobalSe == 0) {
                Snd_SeEmitterPlayOneShot(seEmitter, emotionId + 0x84, 0x7f, 0);
            } else {
                Snd_PlayAuxSe(emotionId + 0x84);
            }
        }
    }
}

void NpcEmotionFx::keepSound() {
    using namespace nO;
    if (seMode == 1 && useGlobalSe == 0) {
        Snd_SeEmitterPlayHeld(seEmitter, emotionId + 0x84, 0x7f, 0);
    }
}

void NpcEmotionFx::stopSound() {
    using namespace nO;
    if (seMode == 1 && useGlobalSe != 0) {
        Snd_StopAuxSe();
    }
    seMode = 2;
}

void NpcEmotionFx::updateSlot(VecFx32 *a, s16 b, s32 c, u16 d, s32 j) {
    using namespace nO;
    NpcEmotionFxSlot *s = &slots[j];
    s32 id = s->effectId;
    if (id >= 0 && id < 0x66) {
        if (slotAnimId == c && slotAnimId != 0x137) {
            u16 dd = d;
            if (s->triggerFrame == dd) {
                s32 k = findFreeHandle();
                if (k != ~0) {
                    effects[k] = Effect_CreateWithParam((u16)s->effectId, effectParam, a, &b);
                    if (s->repeat == 0) {
                        clearSlots(s, 1);
                    }
                }
            }
            const NpcEmotionEntry *bank = Emotion_GetEntry(emotionId);
            s32 bv = bank->phases[entryPart].seFrame;
            if (bv == dd) {
                playSound(s);
            }
        }
    }
}

void NpcEmotionFx::killEffects() {
    using namespace nO;
    s32 zero = 0;
    for (s32 i = 0; i < 4; i++) {
        if (effects[i] != ~zero) {
            Effect_End(effects[i]);
            effects[i] = ~zero;
        }
    }
    stopSound();
}

void NpcEmotionFx::update(VecFx32 *a, s16 b, s32 c, u16 d) {
    using namespace nO;
    keepSound();
    s32 zero = 0;
    for (s32 i = 0; i < 4; i++) {
        if (effects[i] != ~zero) {
            Effect_SetPosition(effects[i], a, &b, NULL);
        }
    }
    for (s32 j = 0; j < 2; j++) {
        updateSlot(a, b, c, d, j);
    }
}

void NpcEmotionFx::stop() {
    using namespace nO;
    killEffects();
}

NpcFaceAnim::NpcFaceAnim() {
    using namespace nO;
    loaded = 0;
    eyeAnimId = 0x16f;
    mouthAnimId = 0x16f;
    savedMouthAnimId = 0x16f;
    talkMouthVariant = 2;
}

NpcFaceAnim::~NpcFaceAnim() {
    using namespace nO;}

BOOL NpcFaceAnim::isLoaded() {
    using namespace nO;
    if (loaded) {
        return TRUE;
    }
    return FALSE;
}

s32 NpcFaceAnim::getMouthAnim() {
    using namespace nO;
    return mouthAnimId;
}

BOOL NpcFaceAnim::load(NpcActor *o) {
    using namespace nO;
    u8 *r6 = o->getTexturePath();
    if (texPatBuf.getTexPatBufRef() == 0 && r6 != 0) {
        if (!texPatBuf.acquire()) {
            return FALSE;
        }
        s32 *a = (s32 *)texPatBuf.getTexPatBufRef();
        NpcTexPatBufRef_LoadFile(a, r6);
        if (!texPatHeap.acquire()) {
            return FALSE;
        }
        void *h = texPatHeap.getTexPatHeapRef();
        NNSG3dResMdl *fl = o->model.resMdl;
        void *x = NpcTexPatBufRef_GetBuffer(a);
        if (!((MatTexPatAnim *)(&eyeTexAnim))->init(fl, x, 1, (void *)(((CharaFaceAnimWorkRef *)(h))->getHeap()))) {
            return FALSE;
        }
        NNSG3dResMdl *fl2 = o->model.resMdl;
        void *y = NpcTexPatBufRef_GetBuffer(a);
        if (!((MatTexPatAnim *)(&mouthTexAnim))->init(fl2, y, 1, (void *)(((CharaFaceAnimWorkRef *)(h))->getHeap()))) {
            return FALSE;
        }
        if (!faceAnimRef.acquire()) {
            return FALSE;
        }
        eyeAnimId = 0x16f;
        mouthAnimId = 0x16f;
        BlinkTimer_Clear((u8 *)(BlinkTimer *)this);
        loaded = 1;
    }
    return TRUE;
}

BOOL NpcFaceAnim::isTalkMouthAnim(s32 v) {
    using namespace nO;
    switch (v) {
    case 0x137:
    case 0x138:
        return TRUE;
    }
    return FALSE;
}

void NpcFaceAnim::pickTalkMouthVariant() {
    using namespace nO;
    talkMouthVariant = NpcFace_PickTalkMouth();
}

s32 NpcFaceAnim::getTalkMouthStartFrame(s32 v) {
    using namespace nO;
    s32 r = 0;
    if (v == 1) {
        r = 5;
    }
    return r;
}

BOOL NpcFaceAnim::isTalkMouthCycleDone(s32 a, s32 b, s32 c) {
    using namespace nO;
    s32 r = 0;
    if (a == 0) {
        if (b == 5) {
            return 1;
        }
    } else if (b >= c - 0x1000) {
        r = 1;
    }
    return r;
}

void NpcFaceAnim::randomizeTalkMouth() {
    using namespace nO;
    if (isTalkMouthAnim(mouthAnimId)) {
        pickTalkMouthVariant();
        mouthTexAnim.curFrame = getTalkMouthStartFrame(talkMouthVariant) << 12;
    }
}

BOOL NpcFaceAnim::isMouthCycleDone() {
    using namespace nO;
    BOOL r = FALSE;
    if (isTalkMouthAnim(mouthAnimId)) {
        r = isTalkMouthCycleDone(talkMouthVariant, (u32)(mouthTexAnim.curFrame << 4) >> 16, (u32)(mouthTexAnim.numFrames << 4) >> 16);
    } else if (((AnimFrameCtrl *)(&mouthTexAnim))->isFinished()) {
        r = TRUE;
    }
    return r;
}

void NpcFaceAnim::setMouthAnim(s32 v, u32 w) {
    using namespace nO;
    if (isLoaded()) {
        void *h = faceAnimRef.getFaceAnim();
        ((CharaFaceAnimRef *)(h))->loadAnim(v, 1, 0);
        mouthTexAnim.setAnim(((CharaFaceAnimRef *)(h))->getMouthAnimBuffer(), 0, 0, w, 0x1000);
        mouthTexAnim.curFrame = 0;
        mouthAnimId = v;
        randomizeTalkMouth();
        ((MatTexPatAnim *)(&mouthTexAnim))->applyFrame();
    }
}

void NpcFaceAnim::startTalkMouth(s32 i) {
    using namespace nO;
    if (i >= 0 && i < 2) {
        if (!isTalkMouthAnim(mouthAnimId)) {
            savedMouthAnimId = mouthAnimId;
        }
        setMouthAnim(sNpcTalkMouthAnims[i], 0);
    }
}

void NpcFaceAnim::restoreMouthAnim() {
    using namespace nO;
    if (savedMouthAnimId == 0x16f) {
        savedMouthAnimId = 0xba;
    }
    setMouthAnim(savedMouthAnimId, 0);
    savedMouthAnimId = 0x16f;
}

void NpcFaceAnim::setFaceAnims(s32 t, s32 u, s32 x, s32 mode) {
    using namespace nO;
    if (isLoaded()) {
        void *h = faceAnimRef.getFaceAnim();
        if (t != 0 || eyeAnimId != t) {
            ((CharaFaceAnimRef *)(h))->loadAnim(t, 1, 0);
            NNS_G3dGetAnmByIdx(((CharaFaceAnimRef *)(h))->getEyeAnimBuffer(), 0);
            eyeTexAnim.setAnim(((CharaFaceAnimRef *)(h))->getEyeAnimBuffer(), 0, 0, x, 0x1000);
            eyeAnimId = t;
            eyeTexAnim.curFrame = 0;
            ((MatTexPatAnim *)(&eyeTexAnim))->applyFrame();
        }
        if (mode == 2 || !isTalkMouthAnim(mouthAnimId)) {
            setMouthAnim(u, 0);
        } else {
            savedMouthAnimId = u;
        }
    }
}

void NpcFaceAnim::setFaceAnimsFrom(u32 a, s32 b, s32 c) {
    using namespace nO;
    if (isLoaded()) {
        s32 t = CharaAnim_GetEyeAnim(a);
        s32 u = CharaAnim_GetMouthAnim(a);
        if (t == 0x16f) {
            t = 0;
        }
        if (u == 0x16f) {
            u = 0xba;
        }
        setFaceAnims(t, u, b, c);
    }
}

BOOL NpcFaceAnim::setMaterialTex(MatTexPatAnim *m, const char *q, const char *r) {
    using namespace nO;
    if (isLoaded()) {
        m->pauseMaterial(q);
        if (m->setMaterialTex(q, r)) {
            return TRUE;
        }
        MatTexPatAnim_ResumeMaterial(m, (void *)q);
    }
    return FALSE;
}

BOOL NpcFaceAnim::setMouthTexture(const char *a) {
    using namespace nO;
    return setMaterialTex(&mouthTexAnim, "m", a);
}

void NpcFaceAnim::resumeMouthMaterial() {
    using namespace nO;
    MatTexPatAnim_ResumeMaterial(&mouthTexAnim, ((u8 *)"m"));
}

void NpcFaceAnim::blinkNow() {
    using namespace nO;
    BlinkTimer_BlinkNow((u8 *)(BlinkTimer *)this);
}

void NpcFaceAnim::release() {
    using namespace nO;
    ((MatTexPatAnim *)(&eyeTexAnim))->release();
    ((MatTexPatAnim *)(&mouthTexAnim))->release();
    texPatBuf.release();
    faceAnimRef.release();
    texPatHeap.release();
}

void NpcFaceAnim::update(NpcActor *o) {
    using namespace nN;
    if (isLoaded()) {
        if (eyeAnimId == 0) {
            if (!((AnimFrameCtrl *)(&eyeTexAnim))->hasPassedFrame(0) || BlinkTimer_Update((u8 *)(BlinkTimer *)this)) {
                ((MatTexPatAnim *)(&eyeTexAnim))->update();
            }
        } else {
            ((MatTexPatAnim *)(&eyeTexAnim))->update();
        }
        if (o->speechState.getMouthType() < 2) {
            if (o->speechState.isSpeaking()) {
                if (!isTalkMouthAnim(mouthAnimId)) {
                    startTalkMouth(o->speechState.getMouthType());
                    randomizeTalkMouth();
                } else if (isMouthCycleDone()) {
                    randomizeTalkMouth();
                }
            } else if (isTalkMouthAnim(mouthAnimId)) {
                if (isMouthCycleDone()) {
                    restoreMouthAnim();
                }
            }
        } else if (isTalkMouthAnim(mouthAnimId)) {
            restoreMouthAnim();
        }
        ((MatTexPatAnim *)(&mouthTexAnim))->update();
    }
}

NpcActionCtrl::NpcActionCtrl() {
    using namespace nN;
    pendingParams.item = 0xfff1;
    curParams.item = 0xfff1;
    action = 0x16;
    actionEntry = NULL;
    priority = 0;
    clearPendingAction();
    setActionDone(1);
    unk_99 = 0;
    moveMode = 0;
    unk_a0 = 0;
    emotionEntry = 0;
    emotionIntro = 1;
    emotionId = 0;
    act07Variant = 5;
    itemEffect = -1;
}

void NpcActionCtrl::func_02019854() {
    using namespace nN;
}

void NpcActionParams::clear() {
    using namespace nN;
    MI_CpuFill8(this, 0, sizeof(NpcActionParams));
}

void NpcActionParams::copyFrom(NpcActionParams *src) {
    using namespace nN;
    MI_CpuCopy8(src, this, sizeof(NpcActionParams));
}

void NpcActionCtrl::startAction(NpcActor *o, s32 a, s32 b, s32 s0, s32 s1, s16 s2, s32 s3, s32 s4) {
    using namespace nN;
    NpcActionParams *p = &pendingParams;
    setPendingAction(a, b);
    p->clear();
    p->waypointX = s0;
    p->waypointZ = s1;
    if (s3 == 0 && s4 == 0) {
        p->destX = s0;
        p->destZ = s1;
    } else {
        p->destX = s3;
        p->destZ = s4;
    }
    p->targetAngle = s2;
    p->blendFrames = data_020c6cc8[0];
    curParams.copyFrom(&pendingParams);
    MI_CpuFill8(&netAction, 0, 0xf);
    isTalking = 0;
    talkingRequest = 0;
    changeAction(o, pendingAction, pendingPriority);
}

s32 NpcActionCtrl::getAction() {
    using namespace nN;
    return action;
}

u8 NpcActionCtrl::getEmotionId() {
    using namespace nN;
    return emotionId;
}

BOOL NpcActionCtrl::isActionDone() {
    using namespace nN;
    if (actionDone == 1) {
        return TRUE;
    }
    return FALSE;
}

NpcActionParams *NpcActionCtrl::getCurParams() {
    using namespace nN;
    return &curParams;
}

void NpcActionCtrl::changeAction(NpcActor *o, s32 idx, s32 state) {
    using namespace nN;
    action = idx;
    if (idx < 0 || idx >= 0x16) {
        action = 0;
    }
    priority = state;
    actionEntry = &sNpcActionTable[action];
    actStep = 0;
    if (actionEntry != NULL) {
        (this->*(actionEntry->setupFn))(o);
    }
}

void NpcActionCtrl::clearPendingAction() {
    using namespace nN;
    setPendingAction(0x16, 0);
    pendingParams.clear();
}

void NpcActionCtrl::setPendingAction(s32 a, s32 b) {
    using namespace nN;
    pendingAction = a;
    pendingPriority = b;
}

BOOL NpcActionCtrl::requestAction(u32 a, s32 b, s32 c, s32 s0, s16 s1, s16 s2, s32 s3, s32 s4, u16 s5, u16 s6) {
    using namespace nN;
    BOOL r = FALSE;
    if (b >= pendingPriority || pendingPriority == 3) {
        if (a != 0xc) {
            NpcActionParams *p = &pendingParams;
            setPendingAction(a, b);
            p->clear();
            p->waypointX = c;
            p->waypointZ = s0;
            if (s3 == 0 && s4 == 0) {
                p->destX = c;
                p->destZ = s0;
            } else {
                p->destX = s3;
                p->destZ = s4;
            }
            p->targetAngle = s2;
            p->turnSpeed = s1;
            p->blendFrames = s5;
            p->animStartFrame = s6;
            r = TRUE;
        }
    }
    return r;
}

BOOL NpcActionCtrl::requestAct07(s32 a, s32 b, s32 c, s32 d, u16 e) {
    using namespace nN;
    BOOL r = FALSE;
    if (requestAction(7, a, 0, 0, b, c, 0, 0, e, 0)) {
        pendingParams.act07Variant = d;
        r = TRUE;
    }
    return r;
}

BOOL NpcActionCtrl::requestEmotion(s32 a, u8 b, u16 c) {
    using namespace nN;
    BOOL r = FALSE;
    if (requestAction(8, a, 0, 0, 0, 0, 0, 0, c, 0)) {
        pendingParams.emotionId = b;
        r = TRUE;
    }
    return r;
}

BOOL NpcActionCtrl::requestStand(u32 a, u16 b) {
    using namespace nN;
    return requestAction(0, a, 0, 0, 0, 0, 0, 0, b, 0);
}

BOOL NpcActionCtrl::requestPlayAnim(s32 a, s32 b, u32 c, u16 s0, u16 s1) {
    using namespace nN;
    BOOL r = FALSE;
    if (a >= pendingPriority || pendingPriority == 3) {
        NpcActionParams *p = &pendingParams;
        setPendingAction(0xc, a);
        p->clear();
        p->animId = b;
        p->animPlayMode = c;
        p->blendFrames = s0;
        p->animStartFrame = s1;
        r = TRUE;
    }
    return r;
}

BOOL NpcActionCtrl::requestGiveItem(s32 a, u16 *b, u32 c, u8 s0, u32 s1, Character *s2) {
    using namespace nN;
    BOOL r = FALSE;
    if (a >= pendingPriority || pendingPriority == 3) {
        NpcActionParams *p = &pendingParams;
        setPendingAction(0xd, a);
        p->clear();
        p->item = *b;
        p->handOverKind = c;
        p->handOverMode = s0;
        p->handOverVariant = s1;
        p->handOverPartner = s2;
        r = TRUE;
    }
    return r;
}

BOOL NpcActionCtrl::requestTakeItem(s32 a, u16 *b, u32 c, u8 s0, u32 s1, Character *s2) {
    using namespace nN;
    BOOL r = FALSE;
    if (a >= pendingPriority || pendingPriority == 3) {
        NpcActionParams *p = &pendingParams;
        setPendingAction(0xe, a);
        p->clear();
        p->item = *b;
        p->handOverKind = c;
        p->handOverMode = s0;
        p->handOverVariant = s1;
        p->handOverPartner = s2;
        r = TRUE;
    }
    return r;
}

void NpcActionCtrl::requestTalkingOn() {
    using namespace nN; talkingRequest = 1; }

void NpcActionCtrl::requestTalkingOff() {
    using namespace nN; talkingRequest = 2; }

void NpcActionCtrl::setTalking() {
    using namespace nN; isTalking = 1; }

void NpcActionCtrl::clearTalking() {
    using namespace nN; isTalking = 0; }

void NpcActionCtrl::applyPendingAction(NpcActor *arg) {
    using namespace nN;
    if (pendingPriority != 0) {
        if (pendingPriority >= priority || isActionDone() || priority == 3) {
            curParams.copyFrom(&pendingParams);
            changeAction(arg, pendingAction, pendingPriority);
        }
    }
    u8 t = talkingRequest;
    if (t == 1) {
        setTalking();
        talkingRequest = 0;
    } else if (t == 2) {
        clearTalking();
        talkingRequest = 0;
    }
    clearPendingAction();
}

void NpcActionCtrl::setActionDone(s32 v) {
    using namespace nN;
    actionDone = v;
}

namespace nN {
extern "C" void NpcAction_PackMove(u8 *out, u32 a, u32 b, u32 c, u32 s0, s16 s1, u16 s2) {
    NetBuf_PackPair20(out, a, b);
    NetBuf_PackPair20(out + 5, c, s0);
    MI_CpuCopy8(&s1, out + 10, 2);
    out[12] = s2;
}
}

namespace nN {
extern "C" void NpcAction_UnpackMove(s32 *a, s32 *b, s32 *c, s32 *d, void *s0, u16 *s1, u8 *out) {
    NetBuf_UnpackPair20(out, (u32 *)a, (u32 *)b);
    NetBuf_UnpackPair20(out + 5, (u32 *)c, (u32 *)d);
    MI_CpuCopy8(out + 10, s0, 2);
    *s1 = out[12];
}
}

namespace nN {
extern "C" void NpcAction_PackTurn(u8 *out, u16 a, u16 b, u32 c) {
    MI_CpuCopy8(&a, out, 2);
    MI_CpuCopy8(&b, out + 2, 2);
    out[4] = c;
}
}

namespace nN {
extern "C" void NpcAction_UnpackTurn(void *a, void *b, u16 *c, u8 *d) {
    MI_CpuCopy8(d, a, 2);
    MI_CpuCopy8(d + 2, b, 2);
    *c = d[4];
}
}

namespace nN {
extern "C" void NpcAction_PackAnim(u8 *p, u32 a, u32 b, u32 c, u16 d) {
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
}
}

namespace nN {
extern "C" void NpcAction_UnpackAnim(u32 *a, u8 *b, u16 *c, u16 *d, u8 *src) {
    *a = src[0];
    *b = src[1];
    *c = src[2];
    *d = src[3];
}
}

namespace nN {
extern "C" void NpcAction_PackItem(void *unused, void *p, u16 *v) {
    NetBuf_WriteU16(p, *v);
}
}

namespace nN {
extern "C" void NpcAction_UnpackItem(u16 *out, void *in) {
    *out = NetBuf_ReadU16(in);
}
}

void NpcActionCtrl::update(NpcActor *arg) {
    using namespace nN;
    applyPendingAction(arg);
    NpcActionEntry *e = actionEntry;
    if (e != NULL && e->mainFn != NULL) {
        (this->*(e->mainFn))(arg);
    }
    if (isActionDone()) {
        priority = 0;
    }
}

void NpcActionCtrl::postUpdate(NpcActor *arg) {
    using namespace nN;
    if (actionEntry != NULL && actionEntry->postFn != NULL) {
        (this->*(actionEntry->postFn))(arg);
    }
    if (isActionDone()) {
        priority = 0;
    }
}

s32 NpcActionCtrl::setupAct00(NpcActor *o) {
    using namespace nN;
    NpcActionParams *c = getCurParams();
    o->moveCtrl.setMoveMode(o, 0, 0, c->blendFrames);
    o->moveCtrl.setWaypoint(&gVec3Zero);
    o->moveCtrl.setDestination(&gVec3Zero);
    o->moveCtrl.setTargetAngle(o->rotY);
    ((Unk_0201acf8 *)(&o->unk_3aa))->func_0201acf8(-2);
    if (o->curHeldTool != 0) {
        ((HeldToolModel *)o->curHeldTool)->playIdleAnim(data_020c6cc8[0], 0);
    }
    netAction = action;
    netPriority = priority;
    setActionDone(0);
    return 0;
}

void NpcActionCtrl::mainAct00(NpcActor *) {
    using namespace nN;
    setActionDone(1);
}

s32 NpcActionCtrl::setupAct01(NpcActor *o) {
    using namespace nN;
    NpcActionParams *c = getCurParams();
    VecFx32Ctor v0(c->waypointX, 0, c->waypointZ);
    VecFx32Ctor v1(c->destX, 0, c->destZ);
    o->moveCtrl.setWaypoint((VecFx32 *)(&v0));
    o->moveCtrl.setDestination((VecFx32 *)(&v1));
    o->moveCtrl.setMoveMode(o, 1, c->turnSpeed, c->blendFrames);
    ((Unk_0201acf8 *)(&o->unk_3aa))->func_0201acf8(1);
    if (o->curHeldTool != 0) {
        ((HeldToolModel *)o->curHeldTool)->playWalkAnim(data_020c6cc8[0], 0);
    }
    netAction = action;
    netPriority = priority;
    NpcAction_PackMove(&netArgs[0], c->waypointX, c->waypointZ, c->destX, c->destZ, c->turnSpeed, c->blendFrames);
    setActionDone(0);
    return 0;
}

void NpcActionCtrl::mainAct01(NpcActor *o) {
    using namespace nN;
    o->moveCtrl.aimAtDestination(o);
    if (o->moveCtrl.hasArrived(o, 0)) {
        if (o->moveCtrl.hasNextLeg()) {
            NpcActionParams *c = getCurParams();
            requestAction(1, 1, c->waypointX, c->waypointZ, 0, 0, 0, 0, data_020c6cc8[0], 0);
        } else {
            setActionDone(1);
        }
    }
}

s32 NpcActionCtrl::setupAct02(NpcActor *o) {
    using namespace nN;
    NpcActionParams *c = getCurParams();
    VecFx32Ctor v0(c->waypointX, 0, c->waypointZ);
    VecFx32Ctor v1(c->destX, 0, c->destZ);
    o->moveCtrl.setWaypoint((VecFx32 *)(&v0));
    o->moveCtrl.setDestination((VecFx32 *)(&v1));
    o->moveCtrl.setMoveMode(o, 2, c->turnSpeed, c->blendFrames);
    ((Unk_0201acf8 *)(&o->unk_3aa))->func_0201acf8(1);
    if (o->curHeldTool != 0) {
        ((HeldToolModel *)o->curHeldTool)->playWalkAnim(data_020c6cc8[0], 0);
    }
    netAction = action;
    netPriority = priority;
    NpcAction_PackMove(&netArgs[0], c->waypointX, c->waypointZ, c->destX, c->destZ, c->turnSpeed, c->blendFrames);
    setActionDone(0);
    return 0;
}

void NpcActionCtrl::mainAct02(NpcActor *o) {
    using namespace nN;
    o->moveCtrl.aimAtDestination(o);
    if (o->moveCtrl.hasArrived(o, 0)) {
        if (o->moveCtrl.hasNextLeg()) {
            NpcActionParams *c = getCurParams();
            requestAction(1, 1, c->waypointX, c->waypointZ, 0, 0, 0, 0, data_020c6cc8[0], 0);
        } else {
            setActionDone(1);
        }
    }
}

s32 NpcActionCtrl::setupAct03(NpcActor *c) {
    using namespace nM;
    NpcActionParams *d = (this)->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 3, d->turnSpeed, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setTargetAngle(d->targetAngle);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    if (c->curHeldTool) {
        ((HeldToolModel *)(void *)(c->curHeldTool))->playWalkAnim(data_020c6cc8[0], 0);
    }
    netAction = action;
    netPriority = priority;
    NpcAction_PackTurn(netArgs, d->turnSpeed, d->targetAngle, d->blendFrames);
    (this)->setActionDone(0);
    return 0;
}

void NpcActionCtrl::mainAct03(NpcActor *c) {
    using namespace nM;
    if (c->rotY == ((NpcMoveCtrl *)(&c->moveCtrl))->getTargetAngle()) {
        (&c->moveCtrl)->setMoveMode((Character *)(void *)(c), 0, 0, (this)->getCurParams()->blendFrames);
        (this)->setActionDone(1);
    }
}

s32 NpcActionCtrl::setupAct04(NpcActor *c) {
    using namespace nM;
    NpcActionParams *d = (this)->getCurParams();
    V v1(d->waypointX, 0, d->waypointZ);
    V v2(d->destX, 0, d->destZ);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 3, d->turnSpeed, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint((VecFx32 *)(&v1));
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination((VecFx32 *)(&v2));
    ((NpcMoveCtrl *)(&c->moveCtrl))->setTargetAngle(d->targetAngle);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    if (c->curHeldTool) {
        ((HeldToolModel *)(void *)(c->curHeldTool))->playWalkAnim(data_020c6cc8[0], 0);
    }
    (this)->setActionDone(0);
    return 0;
}

void NpcActionCtrl::act04Step0(NpcActor *c) {
    using namespace nM;
    if (c->rotY == ((NpcMoveCtrl *)(&c->moveCtrl))->getTargetAngle()) {
        setupAct01(c);
        actStep = 1;
    }
}

void NpcActionCtrl::act04Step1(NpcActor *c) {
    using namespace nM;
    mainAct01(c);
}

namespace nZ {
extern "C" {
extern const u16 data_020c6d70[4] = {0x49, 0x13, 0x4a, 0x1d};
extern const u16 data_020c6ce0[2] = {0x4b, 0x1};
FxVec3 sNpcAvoidOffsets[2] = {FxVec3(0x800, 0, 0x1000), FxVec3(-0x800, 0, 0x1000)};
FxVec3 sNpcObstacleProbeOffsets[2] = {FxVec3(0x700, 0, 0xf00), FxVec3(-0x700, 0, 0xf00)};
extern const u16 data_020c6cd4[2] = {0x63, 0x12};
extern const VecFx32 data_020c6dcc[4] = {{0, 0, -0x2000}, {-0x2000, 0, 0}, {0, 0, 0x2000}, {0x2000, 0, 0}};
extern const u16 data_020c6cbc[2] = {0x400, 0x0};
extern const u16 data_020c6cec[2] = {0x59, 0x0};
extern const u16 data_020c6d08[2] = {0x57, 0x103};
extern const u16 data_020c6cc8[2] = {0x4, 0x0};
extern const u16 data_020c6d00[2] = {0x3b, 0x0};
extern const u16 data_020c6cd8[2] = {0x46, 0x0};
extern const u16 data_020c6d04[2] = {0x63, 0x100};
extern const u16 data_020c6cf0[2] = {0xeb85, 0x0};
extern const u16 data_020c6d24[2] = {0x50, 0x9};
extern const u16 data_020c6cc0[2] = {0x3000, 0x0};
extern const u32 data_020c6d84[4] = {0x1, 0x2, 0x4, 0x8};
extern const u16 data_020c6d28[2] = {0x53, 0x0};
extern const u32 sNpcMoveSpeedPresets[9] = {0x0, 0x0, 0x0, 0x100, 0x19, 0x33, 0x199, 0x66, 0x99};
extern const u16 data_020c6cd0[2] = {0x5e, 0x0};
}
}

void NpcActionCtrl::mainAct04(NpcActor *c) {
    using namespace nM;
    static void (NpcActionCtrl::*tbl[])(NpcActor *) = {&NpcActionCtrl::act04Step0, &NpcActionCtrl::act04Step1};
    if (actStep < 2) {
        (this->*tbl[actStep])(c);
    }
}

s32 NpcActionCtrl::setupMoveTurnFirst(NpcActor *c, s32 a, s16 b) {
    using namespace nM;
    s16 out;
    NpcActionParams *d = (this)->getCurParams();
    V v1(d->waypointX, 0, d->waypointZ);
    V v2(d->destX, 0, d->destZ);
    moveMode = a;
    unk_a0 = b;
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint((VecFx32 *)(&v1));
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination((VecFx32 *)(&v2));
    if (((NpcActor *)(c))->isPosInFront(&out, (VecFx32 *)(((NpcMoveCtrl *)(&c->moveCtrl))->getDestinationB()))) {
        ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), moveMode, d->turnSpeed, d->blendFrames);
        actStep = 1;
    } else {
        ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 3, d->turnSpeed, d->blendFrames);
        ((NpcMoveCtrl *)(&c->moveCtrl))->setTargetAngle(out);
        c->speed = 0;
        actStep = 0;
    }
    if (c->curHeldTool) {
        ((HeldToolModel *)(void *)(c->curHeldTool))->playWalkAnim(data_020c6cc8[0], 0);
    }
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(unk_a0);
    (this)->setActionDone(0);
    return 0;
}

void NpcActionCtrl::act05Step0(NpcActor *c) {
    using namespace nM;
    if (NpcActor_IsFrontAngle(((NpcMoveCtrl *)(&c->moveCtrl))->getTargetAngle() - c->rotY)) {
        s32 t = ((NpcMoveCtrl *)(&c->moveCtrl))->getTurnSpeed();
        c->moveCtrl.setMoveMode(c, moveMode, t, data_020c6cc8[0]);
        actStep = 1;
    }
}

void NpcActionCtrl::act05Step1(NpcActor *c) {
    using namespace nM;
    s16 out;
    VecFx32 *r = c->moveCtrl.getDestinationB();
    ((NpcMoveCtrl *)(&c->moveCtrl))->aimAtDestination((Character *)(c));
    if (((NpcMoveCtrl *)(&c->moveCtrl))->hasArrived((Character *)(c), 0)) {
        if (((NpcMoveCtrl *)(&c->moveCtrl))->hasNextLeg()) {
            c->moveCtrl.setDestination(r);
        } else {
            (this)->setActionDone(1);
        }
    } else if (!c->isPosInFront(&out, r)) {
        u16 v = data_020c6cc8[0];
        s32 t = ((NpcMoveCtrl *)(&c->moveCtrl))->getTurnSpeed();
        c->moveCtrl.setMoveMode(c, 3, t, data_020c6cc8[0]);
        ((NpcMoveCtrl *)(&c->moveCtrl))->setTargetAngle(out);
        actStep = 0;
        c->speed = 0;
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playWalkAnim(v, 0);
        }
    }
}

namespace nZ {
extern "C" {
extern const u16 data_020c6d3c[2] = {0x54, 0x5};
}
}

void NpcActionCtrl::mainAct05(NpcActor *c) {
    using namespace nM;
    static void (NpcActionCtrl::*tbl[])(NpcActor *) = {&NpcActionCtrl::act05Step0, &NpcActionCtrl::act05Step1};
    if (actStep < 2) {
        (this->*tbl[actStep])(c);
    }
}

s32 NpcActionCtrl::setupAct05(NpcActor *c) {
    using namespace nM;
    return setupMoveTurnFirst(c, 1, 1);
}

s32 NpcActionCtrl::setupAct06(NpcActor *c) {
    using namespace nM;
    return setupMoveTurnFirst(c, 2, 2);
}

s32 NpcActionCtrl::setupAct07(NpcActor *c) {
    using namespace nM;
    NpcActionParams *d = (this)->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 3, d->turnSpeed, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setTargetAngle(d->targetAngle);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    if (c->curHeldTool) {
        ((HeldToolModel *)(c->curHeldTool))->playWalkAnim(d->blendFrames, 0);
    }
    act07Variant = d->act07Variant;
    (this)->setActionDone(0);
    return 0;
}

void NpcActionCtrl::act07Step0(NpcActor *c) {
    using namespace nM;
    if (c->rotY == ((NpcMoveCtrl *)(&c->moveCtrl))->getTargetAngle()) {
        if (act07Variant >= 5) {
            act07Variant = 0;
        }
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, sNpcAct07Anims[act07Variant], data_020c6cc8[0], 1, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, sNpcAct07Ses[act07Variant], 0x7f, 0);
        actStep = 1;
    }
}

namespace nZ {
extern "C" {
extern const u32 sNpcAct07Anims[5] = {0xda, 0xdb, 0xdc, 0xdd, 0xde};
extern const u16 sNpcAct07Ses[5] = {0x45, 0x46, 0x47, 0x48, 0x49};
}
}

void NpcActionCtrl::mainAct07(NpcActor *c) {
    using namespace nM;
    static void (NpcActionCtrl::*tbl[])(NpcActor *) = {&NpcActionCtrl::act07Step0};
    if (actStep < 1) {
        (this->*tbl[actStep])(c);
    }
}

void NpcActionCtrl::postAct07(NpcActor *c) {
    using namespace nM;
    if (actStep >= 1) {
        if (!(this)->isActionDone()) {
            if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
                (this)->setActionDone(1);
            }
        }
    }
}

namespace nM {
extern "C" const NpcEmotionEntry *Emotion_GetEntry(u32 i) {
    if (i < 0x3c) {
        return &sEmotionTable[i];
    }
    return 0;
}
}

s32 NpcActionCtrl::setupAct08(NpcActor *c) {
    using namespace nM;
    u32 i = (this)->getCurParams()->emotionId;
    u32 v = data_020c6cc8[0];
    if (i >= 0x3c) {
        i = 0;
    }
    emotionId = i;
    if (i == 0) {
        ((NpcFaceAnim *)(&c->faceAnim))->blinkNow();
        v = c->getEmotion0BlendFrames();
    }
    emotionEntry = &sEmotionTable[i];
    emotionIntro = emotionEntry->introMode;
    ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), emotionEntry->phases[0].animId, v, emotionIntro, 0x1000, 0, 0);
    if (c->curHeldTool) {
        ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
    }
    c->emotionFx.emotionId = i;
    ((NpcEmotionFx *)(&c->emotionFx))->startEntry(emotionEntry->phases, 0);
    (this)->setActionDone(0);
    return 1;
}

void NpcActionCtrl::mainAct08(NpcActor *c) {
    using namespace nM;
    const NpcEmotionEntry *t = emotionEntry;
    if (t != 0) {
        if (emotionIntro == 1) {
            if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
                const NpcEmotionEntry *r = emotionEntry;
                const s32 *pv = &r->phases[1].animId;
                if (r->phases[1].animId < 0x137) {
                    u16 v = data_020c6cc8[0];
                    c->animCtrl.playAnim(c, *pv, data_020c6cc8[0], 0, 0x1000, 0, 0);
                    if (c->curHeldTool) {
                        ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
                    }
                    c->emotionFx.emotionId = emotionId;
                    ((NpcEmotionFx *)(&c->emotionFx))->startEntry(emotionEntry->phases, 1);
                    emotionIntro = 0;
                } else if (r->phases[1].animId == 0x137) {
                    (this)->setActionDone(1);
                }
            }
        } else {
            if (t->phases[0].animId == 0) {
                if ((((u32)c->model.curFrame << 4) >> 16) == 0) {
                    (this)->setActionDone(1);
                }
            }
        }
    }
}

s32 NpcActionCtrl::setupAct09(NpcActor *c) {
    using namespace nM;
    NpcActionParams *d = (this)->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, d->blendFrames);
    ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), 0xd8, d->blendFrames, 0, 0x1000, 0, 0);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    (this)->setActionDone(0);
    return 0;
}

void NpcActionCtrl::mainAct09(NpcActor *c) {
    using namespace nM;
    if ((((u32)c->model.curFrame << 4) >> 16) == 0) {
        if (!((Unk_0201acf8 *)(&c->unk_3aa))->getLevel()) {
            (this)->setActionDone(1);
        }
    }
}

s32 NpcActionCtrl::setupAct0A(NpcActor *c) {
    using namespace nM;
    NpcActionParams *d = (this)->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), 0x77, d->blendFrames, 1, 0x1000, 0, 0);
    (this)->setActionDone(0);
    Snd_PlayAuxSe(0xa0);
    actStep = 0;
    return 0;
}

void NpcActionCtrl::act0AStep0(NpcActor *c) {
    using namespace nL;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        c->animCtrl.playAnim(c, 0x78, data_020c6cc8[0], 1, 0x1000, 0, 0);
        actStep = 1;
    }
}

namespace nZ {
extern "C" {
}
}

void NpcActionCtrl::mainAct0A(NpcActor *c) {
    using namespace nL;
    static Unk_02017d74_Fn tbl[1] = {&NpcActionCtrl::act0AStep0};
    if (actStep < 1) {
        (this->*tbl[actStep])(c);
    }
}

void NpcActionCtrl::postAct0A(NpcActor *c) {
    using namespace nL;
    if (actStep >= 1) {
        if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
            ((NpcActionCtrl *)(this))->setActionDone(1);
        }
    }
}

BOOL NpcActionCtrl::setupAct0B(NpcActor *c) {
    using namespace nL;
    u32 r = c->getAct0BAnimA();
    NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    u16 v = data_020c6cc8[0];
    c->animCtrl.playAnim(c, r, data_020c6cc8[0], 1, 0x1000, 0, 0);
    if (c->curHeldTool) {
        ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
    }
    actStep = 0;
    ((NpcActionCtrl *)(this))->setActionDone(0);
    return FALSE;
}

void NpcActionCtrl::act0BStep0(NpcActor *c) {
    using namespace nL;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        u32 r = c->getAct0BAnimB();
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, r, data_020c6cc8[0], 0, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        actStep = 1;
    }
}

namespace nZ {
extern "C" {
extern const u16 data_020c6d10[2] = {0x61, 0x0};
extern const u16 data_020c6d1c[2] = {0x8000, 0x0};
extern const u16 data_020c6d2c[2] = {0x5a, 0x0};
}
}

void NpcActionCtrl::mainAct0B(NpcActor *c) {
    using namespace nL;
    static Unk_02017d74_Fn tbl[1] = {&NpcActionCtrl::act0BStep0};
    if (actStep < 1) {
        (this->*tbl[actStep])(c);
    }
}

void NpcActionCtrl::postAct0B(NpcActor *c) {
    using namespace nL;
    if (actStep >= 1) {
        if (!((NpcActionCtrl *)(this))->isActionDone()) {
            s32 a = ((u32)c->model.curFrame << 4) >> 16;
            s32 b = (c->model.numFrames << 4) >> 16;
            if (a >= b - 0x1000) {
                ((NpcActionCtrl *)(this))->setActionDone(1);
            }
        }
    }
}

BOOL NpcActionCtrl::setupAct0C(NpcActor *c) {
    using namespace nL;
    NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
    c->moveCtrl.setMoveMode(c, 0, 0, data_020c6cc8[0]);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), d->animId, d->blendFrames, d->animPlayMode, 0x1000, d->animStartFrame, 0);
    netAction = action;
    netPriority = priority;
    NpcAction_PackAnim(netArgs, d->animId, d->animPlayMode, d->blendFrames, d->animStartFrame);
    ((NpcActionCtrl *)(this))->setActionDone(0);
    return TRUE;
}

void NpcActionCtrl::postAct0C(NpcActor *c) {
    using namespace nL;
    BOOL r = FALSE;
    switch (c->model.playMode) {
    case 0:
    case 2:
        if ((((u32)c->model.curFrame << 4) >> 16) == 0) {
            r = TRUE;
        }
        break;
    case 1:
    case 3:
        r = (&c->animCtrl)->isAnimFinished(c);
        break;
    }
    if (r) {
        ((NpcActionCtrl *)(this))->setActionDone(1);
    }
}

BOOL NpcActionCtrl::setupAct0D(NpcActor *c) {
    using namespace nL;
    NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    u16 v = data_020c6cc8[0];
    c->animCtrl.playAnim(c, sNpcGiveItemAnims[d->handOverVariant], data_020c6cc8[0], 1, 0x1000, 0, 0);
    if (c->curHeldTool) {
        ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
    }
    Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x4f, 0x7f, 0);
    HandOverItem_Begin(&d->item, d->handOverKind, d->handOverMode, d->handOverVariant, c, d->handOverPartner);
    HandOverItem_RequestMode(1, c);
    ((NpcActionCtrl *)(this))->setActionDone(0);
    if (d->handOverVariant == 1) {
        actStep = 4;
    } else {
        actStep = 0;
    }
    return TRUE;
}

void NpcActionCtrl::act0DStep0(NpcActor *c) {
    using namespace nL;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        if (PlayerActor_RequestAct32()) {
            u16 v = data_020c6cc8[0];
            c->animCtrl.playAnim(c, 0x23, data_020c6cc8[0], 0, 0x1000, 0, 0);
            if (c->curHeldTool) {
                ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
            }
            actStep = 1;
        }
    } else if ((((u32)c->model.curFrame << 4) >> 16) == 8) {
        Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x63, 0x7f, 0);
    }
}

void NpcActionCtrl::act0DStep1(NpcActor *c) {
    using namespace nL;
    if (!HandOverItem_IsMaster(c)) {
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        actStep = 2;
    }
}

void NpcActionCtrl::act0DStep2(NpcActor *c) {
    using namespace nL;
    if (!HandOverItem_IsActive()) {
        ((NpcActionCtrl *)(this))->setActionDone(1);
        actStep = 5;
    }
}

void NpcActionCtrl::act0DStep3(NpcActor *c) {
    using namespace nL;
    if ((((u32)c->model.curFrame << 4) >> 16) == 0x17) {
        HandOverItem_RequestMode(2, c);
        actStep = 4;
    }
}

void NpcActionCtrl::act0DStep4(NpcActor *c) {
    using namespace nL;
    if (!HandOverItem_IsModeActive(1)) {
        NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
        HandOverItem_SetNextMode(9, c);
        if (HandOverItem_SwitchMaster(d->handOverPartner)) {
            if (PlayerActor_LocalRequestAct37()) {
                actStep = 2;
            }
        }
    } else if ((((u32)c->model.curFrame << 4) >> 16) == 8) {
        Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x63, 0x7f, 0);
    }
}

namespace nZ {
extern "C" {
extern const u16 data_020c6d38[2] = {0x45, 0x2};
}
}

void NpcActionCtrl::mainAct0D(NpcActor *c) {
    using namespace nL;
    static Unk_02017d74_Fn tbl[5] = {&NpcActionCtrl::act0DStep0, &NpcActionCtrl::act0DStep1, &NpcActionCtrl::act0DStep2, &NpcActionCtrl::act0DStep3, &NpcActionCtrl::act0DStep4};
    if (actStep < 5) {
        (this->*tbl[actStep])(c);
    }
}

void NpcActionCtrl::postAct0D(NpcActor *c) {
    using namespace nL;
}

BOOL NpcActionCtrl::setupAct0E(NpcActor *c) {
    using namespace nL;
    NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
    ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, d->blendFrames);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
    ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
    ((Unk_0201acf8 *)(&c->unk_3aa))->func_0201acf8(-2);
    if (d->handOverVariant == 1) {
        PlayerActor_LocalRequestAct36(&d->item, &d->handOverKind, &d->handOverMode, &d->handOverVariant, c);
        actStep = 0xd;
    } else {
        PlayerActor_RequestAct30(&d->item, &d->handOverKind, &d->handOverMode, &d->handOverVariant, c);
        actStep = 0;
    }
    ((NpcActionCtrl *)(this))->setActionDone(0);
    return TRUE;
}

void NpcActionCtrl::act0EStep00(NpcActor *c) {
    using namespace nL;
    if (HandOverItem_IsModeActive(2)) {
        NpcActionParams *d = ((NpcActionCtrl *)(this))->getCurParams();
        if (Unk_02017d74_Is(&c->npcHandle, 0xd00c) || HandOverItem_CanTake(c)) {
            u16 v = data_020c6cc8[0];
            c->animCtrl.playAnim(c, 0x25, data_020c6cc8[0], 1, 0x1000, 0, 0);
            if (c->curHeldTool) {
                ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
            }
            actStep = 2;
        } else if (Unk_02017d74_Is(&d->item, 0x1565) && d->handOverKind == 0 && (Scene_GetCurrent() == 9 || Scene_GetCurrent() == 0x10)) {
            if (HandOverItem_SwitchMaster(c)) {
                u16 v = data_020c6cc8[0];
                c->animCtrl.playAnim(c, 0x25, data_020c6cc8[0], 1, 0x1000, 0, 0);
                if (c->curHeldTool) {
                    ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
                }
                actStep = 2;
            }
        } else {
            VecFx32 buf;
            if (HandOverItem_GetPos(&buf)) {
                buf.y = 0;
                ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&buf);
                ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&buf);
                c->moveCtrl.setMoveMode(c, 1, 0, data_020c6cc8[0]);
                actStep = 1;
            }
        }
    }
}

void NpcActionCtrl::act0EStep01(NpcActor *c) {
    using namespace nK;
    if (HandOverItem_CanTake(c)) {
        u16 v = data_020c6cc8[0];
        ((NpcMoveCtrl *)(&c->moveCtrl))->setMoveMode((Character *)(c), 0, 0, v);
        ((NpcMoveCtrl *)(&c->moveCtrl))->setWaypoint(&gVec3Zero);
        ((NpcMoveCtrl *)(&c->moveCtrl))->setDestination(&gVec3Zero);
        ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), 0x25, v, 1, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        actStep = 2;
    }
}

void NpcActionCtrl::act0EStep02(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        if (HandOverItem_SwitchMaster(c)) {
            if (HandOverItem_RequestMode(3, c)) {
                actStep = 3;
            }
        }
    }
}

void NpcActionCtrl::act0EStep03(NpcActor *c) {
    using namespace nK;
    if (!HandOverItem_IsModeActive(3)) {
        if (HandOverItem_RequestMode(4, c)) {
            u16 v = data_020c6cc8[0];
            c->animCtrl.playAnim(c, 0x26, data_020c6cc8[0], 1, 0x1000, 0, 0);
            if (c->curHeldTool) {
                ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
            }
            actStep = 4;
        }
    }
}

void NpcActionCtrl::act0EStep04(NpcActor *c) {
    using namespace nK;
    s16 t;
    VecFx32 v;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        switch (HandOverItem_GetNextMode()) {
        case 6:
            HandOverItem_End(c);
            c->animCtrl.playAnim(c, 0x28, data_020c6cc8[0], 1, 0x1000, 0, 0);
            Effect_PlayById2(0x61, &c->position, 0, 0);
            Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x76, 0x7f, 0);
            actStep = 6;
            break;
        case 7:
            if (HandOverItem_RequestMode(7, c)) {
                c->animCtrl.playAnim(c, 0x26, data_020c6cc8[0], 3, 0x1000, 0, 0);
                actStep = 7;
            }
            break;
        case 5:
            if (HandOverItem_RequestMode(5, c)) {
                c->animCtrl.playAnim(c, 0x27, data_020c6cc8[0], 1, 0x1000, 0, 0);
                Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x4f, 0x7f, 0);
                actStep = 5;
            }
            break;
        case 10:
            if (HandOverItem_RequestMode(10, c)) {
                c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
                ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), 0x139, 0, 0, 0x1000, 0, 1);
                ThreeLayerAnimModel_AssignJointsToLayer2(&c->model, 9, 14);
                setActionDone(1);
                actStep = 0x14;
            }
            break;
        case 11:
            if (HandOverItem_RequestMode(11, c)) {
                v.x = c->jointPos[0].x;
                v.y = c->jointPos[0].y;
                v.z = c->jointPos[0].z;
                t = c->rotY;
                c->animCtrl.playAnim(c, 0x29, data_020c6cc8[0], 1, 0x1000, 0, 0);
                itemEffect = Effect_Create(0x40, &v, &t, 0);
                actStep = 0xb;
            }
            break;
        case 8:
        case 9:
        default:
            c->animCtrl.playAnim(c, 0x24, data_020c6cc8[0], 0, 0x1000, 0, 0);
            setActionDone(1);
            actStep = 0x14;
            break;
        }
    }
}

void NpcActionCtrl::act0EStep05(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        HandOverItem_End(c);
        if (itemEffect != -1) {
            Effect_End(itemEffect);
        }
        setActionDone(1);
        actStep = 0x14;
    } else if (itemEffect != -1) {
        Effect_SetPosition(itemEffect, &c->jointPos[0], &c->rotY, NULL);
    }
}

BOOL NpcActionCtrl::waitItemAnimEnd(NpcActor *c, u16 *p, u32 a, u32 b) {
    using namespace nK;
    if (a == (u32)(&c->animCtrl)->getAnimId(0)) {
        if (c->animCtrl.isAnimFinished(c)) {
            u16 v = data_020c6cc8[0];
            c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
            if (c->curHeldTool) {
                ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
            }
            return TRUE;
        } else {
            switch (((u32)c->model.curFrame << 4) >> 16) {
            case 8:
                c->setShirt((u16 *)p, b);
                break;
            case 0xd:
                Effect_Create(0x22, &c->position, 0, 0);
                break;
            case 3: {
                s16 t = c->rotY;
                Effect_Create(0x28, &c->jointPos[2], &t, 0);
                Effect_Create(0x28, &c->jointPos[1], &t, 0);
                break;
            }
            }
        }
    }
    return FALSE;
}

void NpcActionCtrl::act0EStep06(NpcActor *c) {
    using namespace nK;
    void *r = getCurParams();
    if (waitItemAnimEnd(c, &((NpcActionParams *)r)->item, 0x28, 1)) {
        setActionDone(1);
        actStep = 0x14;
    }
}

void NpcActionCtrl::act0EStep07(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        if (PlayerActor_RequestAct32()) {
            u16 v = data_020c6cc8[0];
            c->animCtrl.playAnim(c, 0x23, data_020c6cc8[0], 0, 0x1000, 0, 0);
            if (c->curHeldTool) {
                ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
            }
            actStep = 8;
        }
    }
}

void NpcActionCtrl::act0EStep08(NpcActor *c) {
    using namespace nK;
    if (!HandOverItem_IsMaster(c)) {
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        actStep = 9;
    }
}

void NpcActionCtrl::act0EStep09(NpcActor *c) {
    using namespace nK;
    if (!HandOverItem_IsActive()) {
        setActionDone(1);
        actStep = 0x14;
    }
}

void NpcActionCtrl::act0EStep10(NpcActor *c) {
    using namespace nK;
    if ((((u32)c->model.curFrame << 4) >> 16) == 0) {
        setActionDone(1);
        actStep = 0x14;
    }
}

void NpcActionCtrl::act0EStep11(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        ((NpcAnimCtrl *)(&c->animCtrl))->playAnim((NpcActor *)(c), 0x2a, 0, 1, 0x1000, 0, 0);
        if (itemEffect != -1) {
            Effect_End(itemEffect);
        }
        actStep = 0xc;
    }
}

void NpcActionCtrl::act0EStep12(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        u16 v = data_020c6cc8[0];
        c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        if (c->curHeldTool) {
            ((HeldToolModel *)(c->curHeldTool))->playIdleAnim(v, 0);
        }
        HandOverItem_End(c);
        setActionDone(1);
        actStep = 0x14;
    }
}

void NpcActionCtrl::act0EStep13(NpcActor *c) {
    using namespace nK;
    if (HandOverItem_IsModeActive(1)) {
        actStep = 0xe;
    }
}

void NpcActionCtrl::act0EStep14(NpcActor *c) {
    using namespace nK;
    if (!HandOverItem_IsModeActive(1)) {
        c->animCtrl.playAnim(c, 0x2c, data_020c6cc8[0], 1, 0x1000, 0, 0);
        actStep = 0xf;
    }
}

void NpcActionCtrl::act0EStep15(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        if (HandOverItem_SwitchMaster(c)) {
            if (HandOverItem_RequestMode(3, c)) {
                actStep = 0x10;
            }
        }
    }
}

void NpcActionCtrl::act0EStep16(NpcActor *c) {
    using namespace nK;
    if (!HandOverItem_IsModeActive(3)) {
        if (HandOverItem_RequestMode(4, c)) {
            c->animCtrl.playAnim(c, 0x2d, data_020c6cc8[0], 1, 0x1000, 0, 0);
            actStep = 0x11;
        }
    }
}

void NpcActionCtrl::act0EStep17(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        u32 r = HandOverItem_GetNextMode();
        switch (r) {
        case 7:
            if (HandOverItem_RequestMode(7, c)) {
                c->animCtrl.playAnim(c, 0x2d, data_020c6cc8[0], 3, 0x1000, 0, 0);
                actStep = 0x12;
            }
            break;
        case 5:
            if (HandOverItem_RequestMode(5, c)) {
                c->animCtrl.playAnim(c, 0x2e, data_020c6cc8[0], 1, 0x1000, 0, 0);
                Snd_SeEmitterPlayOneShotAlt(&c->seEmitter, 0x4f, 0x7f, 0);
                actStep = 5;
            }
            break;
        default:
            c->animCtrl.playAnim(c, 0x24, data_020c6cc8[0], 0, 0x1000, 0, 0);
            setActionDone(1);
            actStep = 0x14;
            break;
        }
    }
}

void NpcActionCtrl::act0EStep18(NpcActor *c) {
    using namespace nK;
    if (((NpcAnimCtrl *)(&c->animCtrl))->isAnimFinished((NpcActor *)(c))) {
        if (HandOverItem_RequestMode(2, c)) {
            c->animCtrl.playAnim(c, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
            actStep = 0x13;
        }
    }
}

void NpcActionCtrl::act0EStep19(NpcActor *c) {
    using namespace nK;
    void *r = getCurParams();
    if (!HandOverItem_IsModeActive(2)) {
        HandOverItem_SetNextMode(9, c);
        if (HandOverItem_SwitchMaster(((NpcActionParams *)r)->handOverPartner)) {
            if (PlayerActor_LocalRequestAct37()) {
                actStep = 9;
            }
        }
    }
}

namespace nZ {
extern "C" {
extern const NpcMoveModeEntry sNpcMoveModeTable[5] = {
    {0, 0x100, 0, 0, 0x100, 0, {0}},
    {1, 0x200, 0, 1, 0x100, 1, {0}},
    {2, 0x400, 0, 2, 0x400, 1, {0}},
    {1, 0x800, 0, 0, 0x100, 1, {0}},
    {0, 0x100, 0, 1, 0x100, 0, {0}},
};
extern const u16 data_020c6d14[2] = {0x48, 0x0};
extern const u16 data_020c6d30[2] = {0x63, 0x0};
}
}

void NpcActionCtrl::mainAct0E(NpcActor *o) {
    using namespace nJ;
    static void (NpcActionCtrl::*tbl[20])(NpcActor *) = {&NpcActionCtrl::act0EStep00, &NpcActionCtrl::act0EStep01, &NpcActionCtrl::act0EStep02, &NpcActionCtrl::act0EStep03, &NpcActionCtrl::act0EStep04, &NpcActionCtrl::act0EStep05, &NpcActionCtrl::act0EStep06, &NpcActionCtrl::act0EStep07, &NpcActionCtrl::act0EStep08, &NpcActionCtrl::act0EStep09, &NpcActionCtrl::act0EStep10, &NpcActionCtrl::act0EStep11, &NpcActionCtrl::act0EStep12, &NpcActionCtrl::act0EStep13, &NpcActionCtrl::act0EStep14, &NpcActionCtrl::act0EStep15, &NpcActionCtrl::act0EStep16, &NpcActionCtrl::act0EStep17, &NpcActionCtrl::act0EStep18, &NpcActionCtrl::act0EStep19};
    if (actStep < 20) {
        (this->*tbl[actStep])(o);
    }
}

void NpcActionCtrl::postAct0E(NpcActor *) {
    using namespace nJ;
}

BOOL NpcActionCtrl::setupAct0F(NpcActor *o) {
    using namespace nJ;
    NpcActionParams *s = ((NpcActionCtrl *)(this))->getCurParams();
    u16 v;
    HandOverItem_GetItem(&v);
    s->item = v;
    if (HandOverItem_IsActive()) {
        HandOverItem_End((Character *)o);
    }
    o->animCtrl.playAnim(o, 0x28, data_020c6cc8[0], 1, 0x1000, 0, 0);
    Effect_PlayById2(0x61, (VecFx32 *)&o->position, 0, 0);
    Snd_SeEmitterPlayOneShotAlt(&o->seEmitter, 0x76, 0x7f, 0);
    ((NpcActionCtrl *)(this))->setActionDone(0);
    netAction = 0x14;
    netPriority = priority;
    NpcAction_PackItem(this, netArgs, &s->item);
    return TRUE;
}

void NpcActionCtrl::postAct0F(NpcActor *o) {
    using namespace nJ;
    NpcActionParams *s = ((NpcActionCtrl *)(this))->getCurParams();
    if (waitItemAnimEnd(o, &s->item, 0x28, 1)) {
        ((NpcActionCtrl *)(this))->setActionDone(1);
    }
}

BOOL NpcActionCtrl::setupAct10(NpcActor *o) {
    using namespace nJ;
    if (HandOverItem_IsActive()) {
        if (HandOverItem_RequestMode(5, (Character *)o)) {
            o->animCtrl.playAnim(o, 0x27, data_020c6cc8[0], 1, 0x1000, 0, 0);
            Snd_SeEmitterPlayOneShotAlt(&o->seEmitter, 0x4f, 0x7f, 0);
            if ((&o->animCtrl)->getAnimId(1) == 0x139) {
                ((TwoLayerAnimModel *)(void *)(&o->model))->playLayer2FromBase(0, 0);
            }
            ((NpcActionCtrl *)(this))->setActionDone(0);
        }
    }
    return TRUE;
}

void NpcActionCtrl::postAct10(NpcActor *o) {
    using namespace nJ;
    if ((&o->animCtrl)->isAnimFinished((NpcActor *)(void *)(o))) {
        HandOverItem_End((Character *)o);
        o->animCtrl.playAnim(o, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        ((NpcActionCtrl *)(this))->setActionDone(1);
    }
}

BOOL NpcActionCtrl::setupAct11(NpcActor *o) {
    using namespace nJ;
    if (HandOverItem_IsActive()) {
        if (HandOverItem_RequestMode(7, (Character *)o)) {
            o->animCtrl.playAnim(o, 0x26, data_020c6cc8[0], 3, 0x1000, 0, 0);
            if ((&o->animCtrl)->getAnimId(1) == 0x139) {
                ((TwoLayerAnimModel *)(void *)(&o->model))->playLayer2FromBase(0, 0);
            }
            HandOverItem_SetNextMode(5, (Character *)o);
            ((NpcActionCtrl *)(this))->setActionDone(0);
        }
    }
    return TRUE;
}

void NpcActionCtrl::act11Step0(NpcActor *o) {
    using namespace nJ;
    if ((&o->animCtrl)->isAnimFinished((NpcActor *)(void *)(o))) {
        if (PlayerActor_RequestAct32()) {
            o->animCtrl.playAnim(o, 0x23, data_020c6cc8[0], 0, 0x1000, 0, 0);
            actStep = 1;
        }
    }
}

void NpcActionCtrl::act11Step1(NpcActor *o) {
    using namespace nJ;
    if (!HandOverItem_IsMaster((Character *)o)) {
        o->animCtrl.playAnim(o, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
        actStep = 2;
    }
}

void NpcActionCtrl::act11Step2(NpcActor *) {
    using namespace nJ;
    if (!HandOverItem_IsActive()) {
        ((NpcActionCtrl *)(this))->setActionDone(1);
        actStep = 3;
    }
}

namespace nZ {
extern "C" {
}
}

void NpcActionCtrl::mainAct11(NpcActor *o) {
    using namespace nJ;
    static void (NpcActionCtrl::*tbl[3])(NpcActor *) = {&NpcActionCtrl::act11Step0, &NpcActionCtrl::act11Step1, &NpcActionCtrl::act11Step2};
    if (actStep < 3) {
        (this->*tbl[actStep])(o);
    }
}

BOOL NpcActionCtrl::setupAct12(NpcActor *o) {
    using namespace nJ;
    if (HandOverItem_IsActive()) {
        if (HandOverItem_RequestMode(8, (Character *)o)) {
            HandOverItem_SetNextMode(8, (Character *)o);
            actStep = 0;
            ((NpcActionCtrl *)(this))->setActionDone(0);
            PlayerActor_PlayLocalSe(0x72);
        }
    }
    return TRUE;
}

void NpcActionCtrl::act12Step0(NpcActor *) {
    using namespace nJ;
    if (HandOverItem_IsModeActive(8) != 0) {
        actStep = 1;
    }
}

void NpcActionCtrl::act12Step1(NpcActor *) {
    using namespace nJ;
    if (HandOverItem_IsModeActive(8) == 0) {
        actStep = 2;
        ((NpcActionCtrl *)(this))->setActionDone(1);
    }
}

namespace nZ {
extern "C" {
}
}

void NpcActionCtrl::mainAct12(NpcActor *o) {
    using namespace nJ;
    static void (NpcActionCtrl::*tbl[2])(NpcActor *) = {&NpcActionCtrl::act12Step0, &NpcActionCtrl::act12Step1};
    if (actStep < 2) {
        (this->*tbl[actStep])(o);
    }
}

BOOL NpcActionCtrl::setupAct13(NpcActor *o) {
    using namespace nJ;
    VecFx32 v;
    s16 ang;
    v.x = o->jointPos[0].x;
    v.y = o->jointPos[0].y;
    v.z = o->jointPos[0].z;
    ang = o->rotY;
    HandOverItem_RequestMode(0xb, (Character *)o);
    o->animCtrl.playAnim(o, 0x29, data_020c6cc8[0], 1, 0x1000, 0, 0);
    itemEffect = Effect_Create(0x40, &v, &ang, 0);
    Snd_SeEmitterPlayOneShotAlt(&o->seEmitter, 0x6f, 0x7f, 0);
    ((NpcActionCtrl *)(this))->setActionDone(0);
    return TRUE;
}

void NpcActionCtrl::act13Step0(NpcActor *o) {
    using namespace nJ;
    if ((&o->animCtrl)->isAnimFinished((NpcActor *)(void *)(o))) {
        (&o->animCtrl)->playAnim((NpcActor *)(void *)(o), 0x2a, 0, 1, 0x1000, 0, 0);
        if (itemEffect != -1) {
            Effect_End(itemEffect);
        }
        actStep = 1;
    } else {
        if (itemEffect != -1) {
            Effect_SetPosition(itemEffect, &o->jointPos[0], &o->rotY, NULL);
        }
    }
}

void NpcActionCtrl::mainAct13(NpcActor *o) {
    using namespace nJ;
    static void (NpcActionCtrl::*tbl[1])(NpcActor *) = {&NpcActionCtrl::act13Step0};
    if (actStep < 1) {
        (this->*tbl[actStep])(o);
    }
}

void NpcActionCtrl::postAct13(NpcActor *o) {
    using namespace nJ;
    if ((&o->animCtrl)->getAnimId(0) == 0x2a) {
        if ((&o->animCtrl)->isAnimFinished((NpcActor *)(void *)(o))) {
            HandOverItem_End((Character *)o);
            o->animCtrl.playAnim(o, 0, data_020c6cc8[0], 0, 0x1000, 0, 0);
            ((NpcActionCtrl *)(this))->setActionDone(1);
        }
    }
}

s32 NpcActionCtrl::requestAct14(s32 a, u16 *p) {
    using namespace nJ;
    s32 r = 0;
    if (a >= pendingPriority || pendingPriority == 3) {
        NpcActionParams *q = &pendingParams;
        ((NpcActionCtrl *)(this))->setPendingAction(0x14, a);
        (q)->clear();
        q->item = *p;
        r = 1;
    }
    return r;
}

BOOL NpcActionCtrl::setupAct14(NpcActor *o) {
    using namespace nJ;
    ((NpcActionCtrl *)(this))->getCurParams();
    u16 t = data_020c6cc8[0];
    o->animCtrl.playAnim(o, 6, data_020c6cc8[0], 1, 0x1000, 0, 0);
    if (o->curHeldTool) {
        ((HeldToolModel *)(o->curHeldTool))->playIdleAnim(t, 0);
    }
    Effect_PlayById2(0x61, (VecFx32 *)&o->position, 0, 0);
    Snd_SeEmitterPlayOneShotAlt(&o->seEmitter, 0x76, 0x7f, 0);
    ((NpcActionCtrl *)(this))->setActionDone(0);
    return TRUE;
}

void NpcActionCtrl::postAct14(NpcActor *o) {
    using namespace nJ;
    NpcActionParams *s = ((NpcActionCtrl *)(this))->getCurParams();
    if (waitItemAnimEnd(o, &s->item, 6, 0)) {
        ((NpcActionCtrl *)(this))->setActionDone(1);
    }
}

BOOL NpcActionCtrl::setupAct15(NpcActor *o) {
    using namespace nJ;
    VecFx32 v;
    s16 ang;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    ang = o->rotY;
    if (o->netReadPosition(&v, &ang)) {
        s32 dx = v.x - o->position.x;
        s32 dz = v.z - o->position.z;
        s32 d = func_01ffcb0c(dx, dx) + func_01ffcb0c(dz, dz);
        if (d < 0x29) {
            if (ang != o->rotY) {
                o->moveCtrl.setMoveMode(o, 3, 0, data_020c6cc8[0]);
                (&o->moveCtrl)->setTargetAngle(ang);
                actStep = 2;
            } else {
                o->moveCtrl.setMoveMode(o, 0, 0, data_020c6cc8[0]);
                actStep = 0;
            }
            (&o->moveCtrl)->setWaypoint(&gVec3Zero);
            (&o->moveCtrl)->setDestination(&gVec3Zero);
        } else {
            s32 a = Math_Atan2(dx, dz);
            if (NpcActor_IsFrontAngle((s16)(a - ang))) {
                if (netMoveMode == 2) {
                    o->moveCtrl.setMoveMode(o, 2, 0, data_020c6cc8[0]);
                } else {
                    o->moveCtrl.setMoveMode(o, 1, 0, data_020c6cc8[0]);
                }
                actStep = 1;
            } else {
                o->moveCtrl.setMoveMode(o, 4, 0, data_020c6cc8[0]);
                (&o->moveCtrl)->setTargetAngle(a);
                actStep = 3;
            }
            o->moveCtrl.setWaypoint(&v);
            o->moveCtrl.setDestination(&v);
        }
    } else {
        o->moveCtrl.setMoveMode(o, 0, 0, data_020c6cc8[0]);
        (&o->moveCtrl)->setWaypoint(&gVec3Zero);
        (&o->moveCtrl)->setDestination(&gVec3Zero);
        actStep = 0;
    }
    return TRUE;
}

void NpcActionCtrl::act15Step0(NpcActor *o) {
    using namespace nI;
    VecFx32 pos;
    s16 ang;
    s32 dx, dz, d;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    ang = o->rotY;
    if (o->netReadPosition(&pos, &ang)) {
        dx = pos.x - o->position.x;
        dz = pos.z - o->position.z;
        d = func_01ffcb0c(dx, dx) + func_01ffcb0c(dz, dz);
        if (d < 0x29) {
            if (ang != o->rotY) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 3, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 2;
            } else if (((NpcMoveCtrl *)(&o->moveCtrl))->getMoveMode()) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
                o->moveCtrl.setWaypoint(&gVec3Zero);
                o->moveCtrl.setDestination(&gVec3Zero);
            }
        } else {
            s32 a = Math_Atan2(dx, dz);
            if (NpcActor_IsFrontAngle(a - ang)) {
                if (netMoveMode == 2) {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 2, 0, data_020c6cc8[0]);
                } else {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 1, 0, data_020c6cc8[0]);
                }
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 1;
            } else {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 4, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(a);
                actStep = 3;
            }
            ((NpcMoveCtrl *)(&o->moveCtrl))->setWaypoint(&pos);
            ((NpcMoveCtrl *)(&o->moveCtrl))->setDestination(&pos);
        }
    }
}

void NpcActionCtrl::act15Step1(NpcActor *o) {
    using namespace nI;
    VecFx32 pos;
    s16 ang;
    s32 dx, dz, d;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    ang = o->rotY;
    ((NpcMoveCtrl *)(&o->moveCtrl))->aimAtDestination(o);
    if (o->netReadPosition(&pos, &ang)) {
        dx = pos.x - o->position.x;
        dz = pos.z - o->position.z;
        d = func_01ffcb0c(dx, dx) + func_01ffcb0c(dz, dz);
        if (d < 0x29) {
            if (ang != o->rotY) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 3, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 2;
            } else {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                o->moveCtrl.setWaypoint(&gVec3Zero);
                o->moveCtrl.setDestination(&gVec3Zero);
                actStep = 0;
            }
        } else {
            if (!NpcActor_IsFrontAngle(Math_Atan2(dx, dz) - ang)) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 3, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 2;
            } else {
                if (((NpcMoveCtrl *)(&o->moveCtrl))->getMoveMode() == 1) {
                    if (netMoveMode == 2) {
                        ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 2, 0, data_020c6cc8[0]);
                    }
                } else if (((NpcMoveCtrl *)(&o->moveCtrl))->getMoveMode() == 2) {
                    if (netMoveMode == 1) {
                        ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 1, 0, data_020c6cc8[0]);
                    }
                }
                ((NpcMoveCtrl *)(&o->moveCtrl))->setWaypoint(&pos);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setDestination(&pos);
            }
        }
    } else {
        ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
        ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
        o->moveCtrl.setWaypoint(&gVec3Zero);
        o->moveCtrl.setDestination(&gVec3Zero);
        actStep = 0;
    }
}

void NpcActionCtrl::act15Step2(NpcActor *o) {
    using namespace nI;
    VecFx32 pos;
    s16 ang;
    s32 dx, dz, d;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    ang = o->rotY;
    if (o->netReadPosition(&pos, &ang)) {
        dx = pos.x - o->position.x;
        dz = pos.z - o->position.z;
        d = func_01ffcb0c(dx, dx) + func_01ffcb0c(dz, dz);
        if (d < 0x29) {
            s16 a = ang;
            if (a != ((NpcMoveCtrl *)(&o->moveCtrl))->getTargetAngle()) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(a);
            } else {
                s32 c = ((NpcMoveCtrl *)(&o->moveCtrl))->getTargetAngle();
                if (c == o->rotY) actStep = 0;
            }
        } else {
            s32 a = Math_Atan2(dx, dz);
            if (NpcActor_IsFrontAngle(a - ang)) {
                if (netMoveMode == 2) {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 2, 0, data_020c6cc8[0]);
                } else {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 1, 0, data_020c6cc8[0]);
                }
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 1;
            } else {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 4, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(a);
                actStep = 3;
            }
            ((NpcMoveCtrl *)(&o->moveCtrl))->setWaypoint(&pos);
            ((NpcMoveCtrl *)(&o->moveCtrl))->setDestination(&pos);
        }
    } else {
        ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
        ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
        o->moveCtrl.setWaypoint(&gVec3Zero);
        o->moveCtrl.setDestination(&gVec3Zero);
        actStep = 0;
    }
}

void NpcActionCtrl::act15Step3(NpcActor *o) {
    using namespace nI;
    VecFx32 pos;
    s16 ang;
    s32 dx, dz, d;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    ang = o->rotY;
    ((NpcMoveCtrl *)(&o->moveCtrl))->aimAtDestination(o);
    if (o->netReadPosition(&pos, &ang)) {
        dx = pos.x - o->position.x;
        dz = pos.z - o->position.z;
        d = func_01ffcb0c(dx, dx) + func_01ffcb0c(dz, dz);
        if (d == 0) {
            o->moveAngleY = o->rotY;
            if (ang != o->rotY) {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 3, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 2;
            } else {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                o->moveCtrl.setWaypoint(&gVec3Zero);
                o->moveCtrl.setDestination(&gVec3Zero);
                actStep = 0;
            }
        } else if (d >= 0x29) {
            s32 a = Math_Atan2(dx, dz);
            if (NpcActor_IsFrontAngle(a - ang)) {
                if (netMoveMode == 2) {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 2, 0, data_020c6cc8[0]);
                } else {
                    ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 1, 0, data_020c6cc8[0]);
                }
                o->moveAngleY = o->rotY;
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
                actStep = 1;
            } else {
                ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(a);
            }
            ((NpcMoveCtrl *)(&o->moveCtrl))->setWaypoint(&pos);
            ((NpcMoveCtrl *)(&o->moveCtrl))->setDestination(&pos);
        }
    } else {
        ((NpcMoveCtrl *)(&o->moveCtrl))->setMoveMode(o, 0, 0, data_020c6cc8[0]);
        ((NpcMoveCtrl *)(&o->moveCtrl))->setTargetAngle(ang);
        o->moveCtrl.setWaypoint(&gVec3Zero);
        o->moveCtrl.setDestination(&gVec3Zero);
        actStep = 0;
    }
}

namespace nZ {
extern "C" {
}
}

void NpcActionCtrl::mainAct15(NpcActor *o) {
    using namespace nI;
    static Unk_02016360_Fn tbl[4] = {&NpcActionCtrl::act15Step0, &NpcActionCtrl::act15Step1, &NpcActionCtrl::act15Step2, &NpcActionCtrl::act15Step3};
    if (actStep < 4) {
        (this->*tbl[actStep])(o);
    }
}

NpcAnimCtrl::NpcAnimCtrl() {}

NpcAnimCtrl::~NpcAnimCtrl() {}

BOOL NpcAnimCtrl::initForActor(NpcActor *o, s32 a) {
    using namespace nI;
    ((NpcAnimCtrl *)(this))->setAnimSpeedFixed(0);
    animSpeedScale = a;
    if (!bodyAnim.acquire()) return FALSE;
    playAnim(o, 0, 0, 0, 0x1000, 0, 0);
    ((AnimModel *)(&o->model))->attachAnim();
    ((Model *)(&o->model))->setCallback(NpcActor_JointCalcLayer3Cb, 6, 1, o, 0);
    talkGestureVariant = 2;
    if (!((NpcAnimCtrl *)(this))->hasTalkGesture()) ((NpcAnimCtrl *)(this))->loadTalkGesture();
    talkGestureActive = 0;
    return TRUE;
}

void *NpcAnimCtrl::getAnimResource(s32 a, s32 b) {
    using namespace nI;
    AnimSlotRef *p = bodyAnim.getBodyAnimLayer(b);
    void *r = 0;
    if (p) {
        AnimSlotRef_Load(p, a, 0, 0);
        void *q = AnimSlotRef_GetData(p);
        q = func_021065dc(q);
        r = func_021065f8(q, 0);
    }
    return r;
}

s32 NpcAnimCtrl::resolveAnimId(s32 mode, void *p) {
    using namespace nI;
    switch (mode) {
    case 0:
        mode = ((NpcMoveAnimSet *)(p))->getStandAnim();
        break;
    case 1:
        mode = ((NpcMoveAnimSet *)(p))->getWalkAnim();
        break;
    case 2:
        mode = ((NpcMoveAnimSet *)(p))->getRunAnim();
        break;
    }
    return mode;
}

BOOL NpcAnimCtrl::isPlayingAnim(s32 mode, void *p) {
    using namespace nI;
    s32 a = resolveAnimId(mode, p);
    s32 b = ((NpcAnimCtrl *)(this))->getAnimId(0);
    if (a == b) return TRUE;
    return FALSE;
}

void NpcAnimCtrl::playAnim(NpcActor *o, s32 kind, s32 a3, s32 a4, s32 a5, u16 a6, s32 mode) {
    using namespace nI;
    u32 v;
    if (a6 == 0) {
        switch (a4) {
        case 2:
        case 3:
            v = 0xffff;
            goto vdone;
        }
    }
    v = a6;
vdone:
    if (mode >= 0 && mode < 3)
    switch (mode) {
    case 0: {
        s32 size = resolveAnimId(kind, &o->moveAnimSet);
        if (size != ((NpcMoveAnimSet *)(&o->moveAnimSet))->getStandAnim() || !isPlayingAnim(size, &o->moveAnimSet)) {
            void *r = getAnimResource(size, mode);
            if (r) {
                BlendAnimModel_Play2(&o->model, r, a3, a4, a5, v, 0);
            }
        }
        ((NpcAnimCtrl *)(this))->syncMouthType((NpcActor *)(o));
        s32 t = ((NpcAnimCtrl *)(this))->getAnimId(0);
        s32 u = ((NpcSpeechState *)(&o->speechState))->getMouthType();
        ((NpcFaceAnim *)(&o->faceAnim))->setFaceAnimsFrom(t, a4, u);
        break;
    }
    case 1: {
        void *r = getAnimResource(kind, mode);
        if (r) {
            ((TwoLayerAnimModel *)(&o->model))->playLayer2(r, a3, a4, a5, v, 0, 0);
        }
        break;
    }
    case 2: {
        void *r = getAnimResource(kind, mode);
        if (r) {
            ((ThreeLayerAnimModel *)(&o->model))->playLayer3(r, a3, a4, a5, v, 0, 0);
        }
        break;
    }
    }
}

void NpcAnimCtrl::playHoldItemPose(NpcActor *o, u16 *p, u32 q, u16 x) {
    using namespace nI;
    if (Item_IsHoldable(p)) {
        if (CharaAnim_GetHoldPoseMode(q) != 3) {
            if (Unk_02015fe0_R(p, 0x1369, 0x1369)) {
                void *r = getAnimResource(0x13f, 1);
                if (r) {
                    ((TwoLayerAnimModel *)(&o->model))->playLayer2(r, x, 0, 0x1000, 0, 0, 0);
                    ThreeLayerAnimModel_AssignJointsToLayer2(&o->model, 0xc, 0xe);
                }
            } else {
                s32 id = HeldItem_GetHandPose(p);
                if (id != 0x144) {
                    void *r = getAnimResource(id, 1);
                    if (r) {
                        ((TwoLayerAnimModel *)(&o->model))->playLayer2(r, x, 0, 0x1000, 0, 0, 0);
                        s32 t = CharaAnim_GetJointGroup(id);
                        if (t < 4) {
                            u32 n = JointGroup_GetRangeCount(t);
                            for (u32 i = 0; i < n; i++) {
                                s32 a = JointGroup_GetRangeFirst(t, i);
                                ThreeLayerAnimModel_AssignJointsToLayer2(&o->model, a, JointGroup_GetRangeLast(t, i));
                            }
                        } else {
                            ((TwoLayerAnimModel *)(&o->model))->playLayer2FromBase(0, 0);
                        }
                    }
                } else {
                    ((TwoLayerAnimModel *)(&o->model))->playLayer2FromBase(0, 0);
                }
            }
        } else {
            ((TwoLayerAnimModel *)(&o->model))->playLayer2FromBase(0, 0);
        }
    } else {
        ((TwoLayerAnimModel *)(&o->model))->playLayer2FromBase(0, 0);
    }
}

BOOL NpcAnimCtrl::hasTalkGesture() {
    using namespace nH;
    if (getAnimId(2) == 0x143) {
        return TRUE;
    }
    return FALSE;
}

void NpcAnimCtrl::loadTalkGesture() {
    using namespace nH;
    AnimSlotRef *p = bodyAnim.getBodyAnimLayer(2);
    if (p != NULL) {
        AnimSlotRef_Load(p, 0x143, 0, 0);
    }
}

void *NpcAnimCtrl::getTalkGestureData() {
    using namespace nH;
    void *r = NULL;
    if (hasTalkGesture()) {
        AnimSlotRef *p = bodyAnim.getBodyAnimLayer(2);
        if (p != NULL) {
            r = func_021065f8(func_021065dc(AnimSlotRef_GetData(p)), 0);
        }
    }
    return r;
}

void NpcAnimCtrl::playTalkGesture(NpcActor *scene, u32 a, u32 b) {
    using namespace nH;
    void *r1 = getTalkGestureData();
    if (r1 != NULL) {
        scene->model.playLayer3(r1, 0, 1, 0x1000, a, b, 0);
        s32 t = CharaAnim_GetJointGroup(0x143);
        if (t < 4) {
            u32 n = JointGroup_GetRangeCount(t);
            for (u32 i = 0; i < n; i++) {
                u32 x = JointGroup_GetRangeFirst(t, i);
                scene->model.assignJointsToLayer3(x, JointGroup_GetRangeLast(t, i));
            }
        } else {
            stopTalkGesture(scene);
        }
    } else {
        stopTalkGesture(scene);
    }
}

void NpcAnimCtrl::stopTalkGesture(NpcActor *scene) {
    using namespace nH;
    scene->model.playLayer3FromBase(0, 0);
}

void NpcAnimCtrl::setAnimSpeedFixed(u8 v) {
    using namespace nH;
    animSpeedFixed = v;
}

void NpcAnimCtrl::playAnimKeepFrame(NpcActor *scene, u32 c, u32 d, u32 e) {
    using namespace nH;
    ((NpcAnimCtrl *)(this))->playAnim((NpcActor *)(scene), c, d, 0, (u32)scene->model.frameStep, ((u32)scene->model.curFrame << 4) >> 16, e);
}

BOOL NpcAnimCtrl::isAnimFinished(NpcActor *scene) {
    using namespace nH;
    if (static_cast<AnimFrameCtrl &>(scene->model).isFinished() != 0) {
        return TRUE;
    }
    return FALSE;
}

s32 NpcAnimCtrl::getAnimId(u32 idx) {
    using namespace nH;
    if (bodyAnim.getBodyAnimLayer(idx) != NULL) {
        return AnimSlotRef_GetAnimId(bodyAnim.getBodyAnimLayer(idx));
    }
    return 0x144;
}

void NpcAnimCtrl::updateAnimSpeed(NpcActor *scene) {
    using namespace nH;
    s32 v = scene->speed;
    if (v == 0) {
        animSpeed = 0x1000;
    // (getCurSpeedPreset is empty: on the DS its "result" is its argument, still in r0; not so on hosts)
    } else if (scene->moveCtrl.curSpeedPreset.x == 0) {
        animSpeed = 0x1000;
    } else {
        animSpeed = func_01ffcb0c(v, animSpeedScale);
    }
    if (animSpeed <= (s32)scene->model.numFrames) {
        scene->model.frameStep = animSpeed;
    }
}

s32 NpcAnimCtrl::getTalkGestureStart(u32 k) {
    using namespace nH;
    s32 r = 0;
    if (k == 1) {
        r = 6;
    }
    return r;
}

s32 NpcAnimCtrl::getTalkGestureEnd(u32 k) {
    using namespace nH;
    s32 r = 7;
    if (k == 1) {
        r = 0xc;
    }
    return r;
}

void NpcAnimCtrl::update(NpcActor *scene) {
    using namespace nH;
    if (hasTalkGesture()) {
        if (scene->speechState.getMouthType() < 2) {
            if (scene->speechState.isSpeaking()) {
                if (talkGestureActive != 0) {
                    if (scene->model.checkLayer3Finished() == 0) {
                        goto tail;
                    }
                }
                talkGestureVariant = NpcFace_PickTalkMouth();
                s32 a = getTalkGestureStart(talkGestureVariant);
                s32 b = getTalkGestureEnd(talkGestureVariant);
                playTalkGesture(scene, a, b);
                talkGestureActive = 1;
            } else if (talkGestureActive == 1) {
                if (scene->model.checkLayer3Finished() != 0) {
                    stopTalkGesture(scene);
                    talkGestureActive = 0;
                }
            }
        } else if (talkGestureActive == 1) {
            stopTalkGesture(scene);
            talkGestureActive = 0;
        }
    }
tail:
    if (animSpeedFixed == 0) {
        updateAnimSpeed(scene);
    }
}

void NpcAnimCtrl::release() {
    using namespace nH;
    bodyAnim.release();
}

void NpcAnimCtrl::syncMouthType(NpcActor *scene) {
    using namespace nH;
    s32 r = getAnimId(0);
    s32 k = 0;
    switch (r) {
    case 0x45:
    case 0x47:
    case 0x49:
    case 0x4b:
    case 0x4d:
    case 0x4f:
    case 0x51:
    case 0x53:
    case 0x55:
    case 0x57:
    case 0x59:
    case 0x5b:
    case 0x5d:
    case 0x5f:
    case 0x61:
    case 0x63:
    case 0x65:
    case 0x67:
    case 0x69:
    case 0x6e:
    case 0x70:
    case 0x72:
    case 0x75:
    case 0x77:
    case 0x79:
    case 0x7b:
    case 0x7c:
    case 0x7f:
    case 0x80:
    case 0x81:
    case 0x84:
    case 0x86:
    case 0x88:
    case 0x8a:
    case 0x8c:
    case 0x8d:
    case 0x8f:
    case 0x91:
    case 0x93:
    case 0x94:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9c:
    case 0x9f:
    case 0xa1:
    case 0xa3:
    case 0xa5:
    case 0xa7:

    case 0x144:
        k = 2;
        break;
    case 0x46:
    case 0x48:
    case 0x4c:
    case 0x4e:
    case 0x50:
    case 0x52:
    case 0x54:
    case 0x56:
    case 0x5a:
    case 0x60:
    case 0x62:
    case 0x64:
    case 0x68:
    case 0x6c:
    case 0x6d:
    case 0x71:
    case 0x76:
    case 0x78:
    case 0x83:
    case 0x85:
    case 0x87:
    case 0x8b:
    case 0x96:
    case 0x9b:
    case 0x9d:
    case 0x9e:
    case 0xa0:
    case 0xa2:
    case 0xa4:
    case 0xa8:
    case 0xe6:
    case 0xea:
        k = 1;
        break;
    }
    scene->speechState.setMouthType(k);
}

ActorTalkRequest::ActorTalkRequest() {
    using namespace nH;
    subSceneItem = 0xfff1;
    talkPlayer = NULL;
    ownerActor = NULL;
    partnerActor = NULL;
    lastEmotion = 0;
    taskId = 0xc;
    ((ActorTalkRequest *)(this))->resetTasks();
}

ActorTalkRequest::~ActorTalkRequest() {
    using namespace nH;
}

void ActorTalkRequest::resetMsg() {
    using namespace nH;
    TalkMsgRequest::resetMsg();
    talkPlayer = NULL;
    partnerActor = NULL;
    actionActorIndex = 0;
    speakerIndex = 0;
    lastEmotion = 0;
    clearItemActionBusy();
    ((ActorTalkRequest *)(this))->resetTasks();
}

void ActorTalkRequest::update() {
    using namespace nH;
}

void ActorTalkRequest::tick() {
    using namespace nH;
    update();
    ((ActorTalkRequest *)(this))->runTask();
}

void ActorTalkRequest::runDeferred() {
    using namespace nH;
}

void ActorTalkRequest::setTalkPlayer(Character *player) {
    using namespace nH;
    talkPlayer = player;
}

Character *ActorTalkRequest::getTalkPlayer() {
    using namespace nH;
    return talkPlayer;
}

void ActorTalkRequest::setPartnerActor(NpcActor *p) {
    using namespace nH;
    partnerActor = p;
    if (partnerActor != NULL) {
        ActorTalkRequest *q = ((NpcActor *)(partnerActor))->getTalkRequest();
        if (q != NULL) {
            q->resetMsg();
            partnerActor->onJoinTalk();
        }
    }
}

NpcActor *ActorTalkRequest::getPartnerActor() {
    using namespace nH;
    return partnerActor;
}

void ActorTalkRequest::setOwnerActor(NpcActor *p) {
    using namespace nH;
    ownerActor = p;
}

ChoiceList *ActorTalkRequest::getChoiceList() {
    using namespace nH;
    ChoiceList *r = 0;
    if (window != 0) {
        r = window->getChoiceList();
    }
    return r;
}

s32 ActorTalkRequest::getVoiceType() {
    using namespace nH;
    switch (speakerIndex) {
    case 0:
        if (ownerActor != NULL) {
            return Npc_GetVoiceType(&ownerActor->npcHandle);  // (was the DS offset 0xea)
        }
        break;
    case 1:
        if (partnerActor != NULL) {
            return Npc_GetVoiceType(&partnerActor->npcHandle);
        }
        break;
    }
    return 5;
}

void ActorTalkRequest::playEmotion(u32 a, u32 b) {
    using namespace nH;
    NpcActor *o = getActor(b);
    if (o != NULL) {
        ActorTalkRequest *p = ((NpcActor *)(o))->getTalkRequest();
        if (p != NULL) {
            if (!p->isItemActionBusy()) {
                if (p->lastEmotion == a) {
                    if (((NpcActionCtrl *)(&o->actionCtrl))->getAction() == 8) {
                        return;
                    }
                }
                ((NpcActionCtrl *)(&o->actionCtrl))->requestEmotion(2, a, data_020c6cc8[0]);
                p->lastEmotion = a;
            }
        }
    }
}

void ActorTalkRequest::onEventTag(u32 a) {
    using namespace nH;
    playEmotion(a, actionActorIndex);
}

void ActorTalkRequest::setItemActionBusy() {
    using namespace nH;
    itemActionBusy = 1;
}

void ActorTalkRequest::clearItemActionBusy() {
    using namespace nH;
    itemActionBusy = 0;
}

BOOL ActorTalkRequest::isItemActionBusy() {
    using namespace nH;
    if (itemActionBusy == 1) {
        return TRUE;
    }
    return FALSE;
}

void ActorTalkRequest::setNumberSlot(s32 a, u32 b, s32 c, s32 d, s32 e) {
    using namespace nH;
    if (a >= 0) {
        MsgString25 loc;
        String_FormatNumber(&loc, a, c, d, e, 0);
        (window)->setSlot(b, &loc);
    }
}

void ActorTalkRequest::setNumberNamedSlot(s32 a, u32 b, s32 c, u8 d, s32 e, s32 f) {
    using namespace nH;
    if (a >= 0) {
        MsgString25 loc;
        u32 flag = 7;
        String_FormatNumber(&loc, a, c, e, f, 0);
        if (d != 0) {
            MsgString33 str;
            u8 ch = 0x1c;
            String_Load(&str, &ch, "st_general");
            ((MsgString *)(&loc))->appendString((MsgString *)(&str));
            flag = 0;
        }
        (window)->setNamedSlot(b, &loc, flag);
    }
}

void ActorTalkRequest::setFixedPointSlot(s32 a, u32 b, s32 c) {
    using namespace nH;
    if (a >= 0) {
        MsgString25 local;
        String_FormatFixedPoint(&local, a, c);
        (window)->setSlot(b, &local);
    }
}

void ActorTalkRequest::setMonthSlot(u32 a, u32 b) {
    using namespace nH;
    MsgString33 local;
    String_GetMonthName(&local, a);
    (window)->setSlot(b, &local);
}

void ActorTalkRequest::setDaySlot(u32 a, u32 b) {
    using namespace nH;
    MsgString33 local;
    String_GetDayOrdinal(&local, a);
    (window)->setSlot(b, &local);
}

void ActorTalkRequest::setTownNameSlot(void *a, u32 b) {
    using namespace nH;
    MsgString9C local;
    TownId_GetNameString(a, &local);
    (window)->setSlot(b, &local);
}

void ActorTalkRequest::setPlayerNameSlot(PlayerId *a, u32 b) {
    using namespace nH;
    MsgString9B local;
    a->getNameString(&local);
    (window)->setSlot(b, &local);
}

void ActorTalkRequest::setVillagerNameSlot(VillagerId *a, u32 b) {
    using namespace nH;
    MsgString9B local;
    a->getName(&local);
    (window)->setSlot(b, &local);
}

void ActorTalkRequest::setItemNameSlot(u16 *a, u32 b, u32 c) {
    using namespace nH;
    ItemName local(a);
    (window)->setNamedSlot(b, &local, c);
}

void ActorTalkRequest::setSlotFromString(u32 a, u8 *b, const char *c) {
    using namespace nH;
    (window)->setSlotFromString(a, b, c);
}

void ActorTalkRequest::onConditionTag(u32 condition, u32 branchCount) {
    using namespace nH;
}

void ActorTalkRequest::onTag09_0() {
    using namespace nH;
    actionActorIndex = 2;
}

void ActorTalkRequest::onTag09_1() {
    using namespace nH;
    actionActorIndex = 0;
}

void ActorTalkRequest::onTag09_2() {
    using namespace nH;
    actionActorIndex = 1;
}

NpcActor *ActorTalkRequest::getActor(u32 idx) {
    using namespace nH;
    switch (idx) {
    case 0:
        return ownerActor;
    case 1:
        return partnerActor;
    }
    return NULL;
}

NpcActor *ActorTalkRequest::getActionActor() {
    using namespace nH;
    return getActor(actionActorIndex);
}

NpcActor *ActorTalkRequest::getActorB(u32 idx) {
    using namespace nH;
    switch (idx) {
    case 0:
        return ownerActor;
    case 1:
        return partnerActor;
    }
    return NULL;
}

NpcActor *ActorTalkRequest::getSpeakerActor() {
    using namespace nH;
    return getActorB(speakerIndex);
}

u8 ActorTalkRequest::getSpeakerIndex() {
    using namespace nH;
    return speakerIndex;
}

namespace nH {
extern "C" s32 TalkRequest_GetPlayerId(void) {
    if (Scene_GetCurrent() == 0x2f) {
        return Net_GetJoiningAid();
    }
    return PlayerActor_GetLocalSessionSlot();
}
}

void ActorTalkRequest::onTag09_3() {
    using namespace nH;
    NpcActor *p = getActor(actionActorIndex);
    if (p != NULL) {
        p->lookAt.setTarget(4, 0, NULL, (VecFx32 *)&gVec3Zero, TalkRequest_GetPlayerId(), 0, 0);
    }
}

void ActorTalkRequest::onTag09_4() {
    using namespace nG;
    NpcActor *p5 = getActor(actionActorIndex);
    if (p5 != NULL) {
        ActorTalkRequest *h = p5->getTalkRequest();
        u32 u = TalkRequest_GetPlayerId();
        s32 v = p5->getAngleToPlayer(u);
        if (h != NULL) {
            h->onEventTag(0);
        }
        p5->lookAt.setTarget(4, 0, NULL, &gVec3Zero, u, 0, 0);
        p5->actionCtrl.requestAction(3, 2, 0, 0, 0, v, 0, 0, data_020c6cc8[0], 0);
    }
}

void ActorTalkRequest::makePlayerLookAt(NpcActor *p) {
    using namespace nG;
    u32 a = TalkRequest_GetPlayerId();
    VecFx32 *b = (VecFx32 *)PlayerActor_GetBodyPos(a);
    if (b != NULL) {
        s32 s = Math_AngleXZ(b, &p->position);
        s16 d = s - ((Actor *)PlayerActor_GetCharacter(4))->rotY;
        PlayerActor_SetHeadTilt(0, d, a);
    }
}

void ActorTalkRequest::makePlayerTurnTo(NpcActor *p) {
    using namespace nG;
    u32 a = TalkRequest_GetPlayerId();
    VecFx32 *b = (VecFx32 *)PlayerActor_GetBodyPos(a);
    if (b != NULL) {
        s32 s = Math_AngleXZ(b, &p->position);
        PlayerActor_SetHeadTilt(0, 0, a);
        PlayerActor_RequestTurnTo(s, a);
        PlayerActor_SetNoFaceTalkTarget(1, a);
    }
}

void ActorTalkRequest::onTag09_5() {
    using namespace nG;
    NpcActor *p4 = getActor(0);
    if (p4 != NULL) {
        u8 s = actionActorIndex;
        switch (s) {
        case 1: {
            NpcActor *p = getActor(s);
            if (p != NULL) {
                p->lookAt.setTarget(2, 0, p4, &gVec3Zero, 4, 0, 0);
            }
            break;
        }
        case 2:
            makePlayerLookAt(p4);
            break;
        }
    }
}

void ActorTalkRequest::onTag09_6() {
    using namespace nG;
    NpcActor *p7 = getActor(0);
    if (p7 != NULL) {
        u8 s = actionActorIndex;
        switch (s) {
        case 1: {
            NpcActor *p4 = getActor(s);
            if (p4 != NULL) {
                ActorTalkRequest *h = p4->getTalkRequest();
                s32 v = p4->getAngleTo(p7);
                onTag09_5();
                if (h != NULL) {
                    h->onEventTag(0);
                }
                p4->actionCtrl.requestAction(3, 2, 0, 0, 0, v, 0, 0, data_020c6cc8[0], 0);
            }
            break;
        }
        case 2:
            makePlayerTurnTo(p7);
            break;
        }
    }
}

void ActorTalkRequest::onTag09_7() {
    using namespace nG;
    NpcActor *p4 = getActor(1);
    if (p4 != NULL) {
        u8 s = actionActorIndex;
        switch (s) {
        case 0: {
            NpcActor *p = getActor(s);
            if (p != NULL) {
                p->lookAt.setTarget(2, 0, p4, &gVec3Zero, 4, 0, 0);
            }
            break;
        }
        case 2:
            makePlayerLookAt(p4);
            break;
        }
    }
}

void ActorTalkRequest::onTag09_8() {
    using namespace nG;
    NpcActor *p7 = getActor(1);
    if (p7 != NULL) {
        u8 s = actionActorIndex;
        switch (s) {
        case 0: {
            NpcActor *p4 = getActor(s);
            if (p4 != NULL) {
                ActorTalkRequest *h = p4->getTalkRequest();
                s32 v = p4->getAngleTo(p7);
                onTag09_7();
                if (h != NULL) {
                    h->onEventTag(0);
                }
                p4->actionCtrl.requestAction(3, 2, 0, 0, 0, v, 0, 0, data_020c6cc8[0], 0);
            }
            break;
        }
        case 2:
            makePlayerTurnTo(p7);
            break;
        }
    }
}

void ActorTalkRequest::onTag09_9() {
    using namespace nG;
    ((ActorTalkRequest *)(this))->requestSwitchSpeaker(1);
}

void ActorTalkRequest::resetTasks() {
    using namespace nG;
    taskId = 12;
    taskStep = 0;
    taskRunning = 0;
    initSubSceneParams();
}

void ActorTalkRequest::initSubSceneParams() {
    using namespace nG;
    subSceneType = 8;
    menuPtrArg0 = NULL;
    menuPtrArg1 = NULL;
    menu12Arg = NULL;
    pocketMask = 0;
    pocketFilter = NULL;
    pocketSelectMode = 13;
    subSceneItem = 0xfff1;
    handOverKind = 0;
    launcherMenu = 0x45;
    launcherIndex = 0;
    keepWindowClosed = 2;
    handOverMode = 12;
    handOverVariant = 2;
    focusNewSpeaker = 1;
    launcherText = NULL;
    launcherTextSize = 0;
    randomMelody = 0;
    closeMode = 0;
}

BOOL ActorTalkRequest::startTask(s32 x) {
    using namespace nG;
    BOOL result = FALSE;
    if (taskId == 12 || isTaskRunning() == 0) {
        taskId = x;
        taskStep = 0;
        result = TRUE;
        taskRunning = result;
    }
    return result;
}

BOOL ActorTalkRequest::isTaskRunning() {
    using namespace nG;
    if (taskRunning != 0) {
        return TRUE;
    }
    return FALSE;
}

namespace nZ {
extern "C" {
}
}

void ActorTalkRequest::runTask() {
    using namespace nG;
    static ActorTalkTaskFn tbl[12] = {&ActorTalkRequest::taskSubScene, &ActorTalkRequest::taskReopenWindow, &ActorTalkRequest::taskCloseWindow, &ActorTalkRequest::taskGiveItem, &ActorTalkRequest::taskTakeItem, &ActorTalkRequest::taskItemAct0F, &ActorTalkRequest::taskKeepItem, &ActorTalkRequest::taskReturnItem, &ActorTalkRequest::taskItemAct12, &ActorTalkRequest::taskEatItem, &ActorTalkRequest::taskMelody, &ActorTalkRequest::taskSwitchSpeaker};
    if ((s32)taskId < 12) {
        if ((this->*tbl[taskId])()) {
            s32 old = taskId;
            taskRunning = 0;
            taskId = 12;
            onTaskDone(old);
        }
    }
}

void ActorTalkRequest::onTaskDone(u32 id) {
    using namespace nG;
}

BOOL ActorTalkRequest::openSubScene(s32 x) {
    using namespace nG;
    BOOL result = FALSE;
    if (startTask(0)) {
        subSceneType = x;
        result = TRUE;
    }
    return result;
}

void ActorTalkRequest::setPocketItem(u32 a, u32 b, u32 c) {
    using namespace nG;
    pocketMask = a;
    pocketFilter = 0;
    pocketSelectMode = b;
    subSceneItem = 0xfff1;
    keepWindowClosed = c;
}

void ActorTalkRequest::setPocketFilter(BOOL (*a)(u16 *, s32), u32 b, u32 c) {
    using namespace nG;
    pocketMask = 0;
    pocketFilter = a;
    pocketSelectMode = b;
    subSceneItem = 0xfff1;
    keepWindowClosed = c;
}

void ActorTalkRequest::setSubSceneKind(u32 a, u32 b) {
    using namespace nG;
    launcherMenu = a;
    keepWindowClosed = b;
}

void ActorTalkRequest::setSubSceneKindArg(u32 a, u32 b, u32 c) {
    using namespace nG;
    launcherMenu = a;
    launcherIndex = b;
    keepWindowClosed = c;
}

void ActorTalkRequest::setSelectionList(void *a, void *b, u32 c) {
    using namespace nG;
    menuPtrArg0 = a;
    menuPtrArg1 = b;
    keepWindowClosed = c;
}

void ActorTalkRequest::setMenu12Arg(void *a, u32 b) {
    using namespace nG;
    menu12Arg = a;
    keepWindowClosed = b;
}

void ActorTalkRequest::setSubSceneKind2(u32 a, u8 *b, u32 c, u8 d) {
    using namespace nG;
    launcherMenu = a;
    launcherText = b;
    launcherTextSize = c;
    keepWindowClosed = d;
}

BOOL ActorTalkRequest::subSceneCloseWindow() {
    using namespace nG;
    if (window != NULL) {
        window->openMode = 1;
        taskStep = 1;
    }
    return FALSE;
}

BOOL ActorTalkRequest::subSceneOpen() {
    using namespace nG;
    if (window != NULL && window->state == 5) {
        BOOL r = FALSE;
        switch (subSceneType) {
        case 0: {
            u16 v = pocketMask;
            if (pocketFilter != 0) {
                v = MenuCtrl_BuildPocketMask(pocketFilter);
            }
            r = MenuCtrl_OpenPocketSelect(v, pocketSelectMode);
            break;
        }
        case 1:
            keepWindowClosed = 1;
            r = MenuCtrl_OpenPostOffice();
            break;
        case 2:
            r = MenuCtrl_OpenLauncher(launcherMenu);
            break;
        case 3:
            r = MenuCtrl_OpenLauncherWithIndex(launcherMenu, launcherIndex);
            break;
        case 4:
            r = MenuCtrl_OpenNearbyTowns(menuPtrArg0, menuPtrArg1);
            break;
        case 5:
            r = MenuCtrl_RequestOpenMenu12(menu12Arg);
            break;
        case 6:
            r = MenuCtrl_OpenLauncherWithText(launcherMenu, launcherText, launcherTextSize);
            break;
        case 7:
            if (keepWindowClosed != 1) {
                window->nextState = 1;
            }
            taskStep = 3;
            return TRUE;
        }
        if (r) {
            taskStep = 2;
        }
    }
    return FALSE;
}

BOOL ActorTalkRequest::subSceneWait() {
    using namespace nG;
    BOOL result = FALSE;
    if (MenuCtrl_IsFinished()) {
        if (keepWindowClosed != 1) {
            window->nextState = 1;
        }
        taskStep = 3;
        result = TRUE;
    }
    return result;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskSubScene() {
    using namespace nG;
    static ActorTalkTaskFn tbl[3] = {&ActorTalkRequest::subSceneCloseWindow, &ActorTalkRequest::subSceneOpen, &ActorTalkRequest::subSceneWait};
    BOOL r = FALSE;
    u8 i = taskStep;
    if (i < 3) {
        r = (this->*tbl[i])();
    }
    return r;
}

BOOL ActorTalkRequest::requestReopenWindow() {
    using namespace nG;
    return startTask(1);
}

BOOL ActorTalkRequest::taskReopenWindow() {
    using namespace nG;
    BOOL result = FALSE;
    if (window != NULL && window->state == 5) {
        result = TRUE;
        window->nextState = result;
    }
    return result;
}

BOOL ActorTalkRequest::requestCloseWindow(u32 x) {
    using namespace nG;
    if (startTask(2)) {
        closeMode = x;
        return TRUE;
    }
    return FALSE;
}

BOOL ActorTalkRequest::closeWindowStart() {
    using namespace nG;
    if (window != NULL) {
        window->openMode = closeMode;
        taskStep = 1;
    }
    return FALSE;
}

BOOL ActorTalkRequest::closeWindowWait() {
    using namespace nG;
    BOOL result = FALSE;
    if (window != NULL && window->state == 5) {
        result = TRUE;
    }
    return result;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskCloseWindow() {
    using namespace nG;
    static ActorTalkTaskFn tbl[2] = {&ActorTalkRequest::closeWindowStart, &ActorTalkRequest::closeWindowWait};
    BOOL r = FALSE;
    u8 i = taskStep;
    if (i < 2) {
        r = (this->*tbl[i])();
    }
    return r;
}

BOOL ActorTalkRequest::requestGiveItem(u16 *p, u32 b, u32 c, u32 d) {
    using namespace nG;
    BOOL result = FALSE;
    if (startTask(3)) {
        subSceneItem = *p;
        handOverKind = b;
        handOverMode = c;
        handOverVariant = d;
        result = TRUE;
    }
    return result;
}

BOOL ActorTalkRequest::giveItemStart() {
    using namespace nG;
    if (ownerActor != NULL) {
        if (window != NULL) {
            ((TalkWindowState *)(window))->lockAdvance();
        }
        if (ownerActor->actionCtrl.getAction() == 8 && ownerActor->actionCtrl.isActionDone() == 0) {
            onEventTag(0);
        } else if (window != NULL) {
            if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestGiveItem(4, &subSceneItem, handOverKind, handOverMode, handOverVariant, talkPlayer)) {
                taskStep = 1;
            }
        }
    }
    return FALSE;
}

BOOL ActorTalkRequest::giveItemWait() {
    using namespace nG;
    BOOL result = FALSE;
    if (ownerActor != NULL && window != NULL) {
        if (ownerActor->actionCtrl.getAction() == 13 && ownerActor->actionCtrl.isActionDone() != 0) {
            ((TalkWindowState *)(window))->unlockAdvance();
            taskStep = 2;
            result = TRUE;
        }
    }
    return result;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskGiveItem() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::giveItemStart, &ActorTalkRequest::giveItemWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestTakeItem(u16 *a, u32 b, u32 c, u32 d) {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(4)) {
        subSceneItem = *a;
        handOverKind = b;
        handOverMode = c;
        handOverVariant = d;
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::takeItemStart() {
    using namespace nF;
    if (ownerActor != NULL) {
        if (window != NULL) {
            ((TalkWindowState *)(window))->lockAdvance();
        }
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 8 && !((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            onEventTag(0);
        } else if (window != NULL) {
            if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestTakeItem(4, &subSceneItem, handOverKind, handOverMode, handOverVariant, talkPlayer)) {
                ((ActorTalkRequest *)(this))->setItemActionBusy();
                taskStep = 1;
            }
        }
    }
    return 0;
}

BOOL ActorTalkRequest::takeItemWait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0xe && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            if (handOverMode != 4) {
                ((ActorTalkRequest *)(this))->clearItemActionBusy();
            }
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
extern const s32 data_020c6d48[2] = {0xe2, 0xe0};
extern const u16 data_020c6d0c[2] = {0x5f, 0x19};
}
}

BOOL ActorTalkRequest::taskTakeItem() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::takeItemStart, &ActorTalkRequest::takeItemWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestItemAct0F() {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(5)) {
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::itemAct0FStart() {
    using namespace nF;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestAction(0xf, 4, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0)) {
            ((TalkWindowState *)(window))->lockAdvance();
            taskStep = 1;
        }
    }
    return 0;
}

BOOL ActorTalkRequest::itemAct0FWait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0xf && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            ((ActorTalkRequest *)(this))->clearItemActionBusy();
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
extern const u16 data_020c6cdc[2] = {0x4c, 0x0};
}
}

BOOL ActorTalkRequest::taskItemAct0F() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::itemAct0FStart, &ActorTalkRequest::itemAct0FWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestKeepItem() {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(6)) {
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::keepItemStart() {
    using namespace nF;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestAction(0x10, 4, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0)) {
            ((ActorTalkRequest *)(this))->setItemActionBusy();
            ((TalkWindowState *)(window))->lockAdvance();
            taskStep = 1;
        }
    }
    return 0;
}

BOOL ActorTalkRequest::keepItemWait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0x10 && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            ((ActorTalkRequest *)(this))->clearItemActionBusy();
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskKeepItem() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::keepItemStart, &ActorTalkRequest::keepItemWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestReturnItem() {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(7)) {
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::returnItemStart() {
    using namespace nF;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestAction(0x11, 4, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0)) {
            ((ActorTalkRequest *)(this))->setItemActionBusy();
            ((TalkWindowState *)(window))->lockAdvance();
            taskStep = 1;
        }
    }
    return 0;
}

BOOL ActorTalkRequest::returnItemWait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0x11 && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            ((ActorTalkRequest *)(this))->clearItemActionBusy();
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
extern const u16 data_020c6cf8[2] = {0x5d, 0x0};
}
}

BOOL ActorTalkRequest::taskReturnItem() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::returnItemStart, &ActorTalkRequest::returnItemWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestItemAct12() {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(8)) {
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::itemAct12Start() {
    using namespace nF;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestAction(0x12, 4, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0)) {
            ((TalkWindowState *)(window))->lockAdvance();
            taskStep = 1;
        }
    }
    return 0;
}

BOOL ActorTalkRequest::itemAct12Wait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0x12 && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
extern const u16 data_020c6ccc[2] = {0x4d, 0x0};
extern const u16 data_020c6d18[2] = {0x44, 0x0};
}
}

BOOL ActorTalkRequest::taskItemAct12() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::itemAct12Start, &ActorTalkRequest::itemAct12Wait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestEatItem() {
    using namespace nF;
    BOOL r = 0;
    if ((this)->startTask(9)) {
        r = 1;
    }
    return r;
}

BOOL ActorTalkRequest::eatItemStart() {
    using namespace nF;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->requestAction(0x13, 4, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0)) {
            ((TalkWindowState *)(window))->lockAdvance();
            taskStep = 1;
        }
    }
    return 0;
}

BOOL ActorTalkRequest::eatItemWait() {
    using namespace nF;
    BOOL r = 0;
    if (ownerActor != NULL && window != NULL) {
        if (((NpcActionCtrl *)(&ownerActor->actionCtrl))->getAction() == 0x13 && ((NpcActionCtrl *)(&ownerActor->actionCtrl))->isActionDone()) {
            ((TalkWindowState *)(window))->unlockAdvance();
            ((ActorTalkRequest *)(this))->clearItemActionBusy();
            taskStep = 2;
            r = 1;
        }
    }
    return r;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskEatItem() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::eatItemStart, &ActorTalkRequest::eatItemWait};
    BOOL r = 0;
    if (taskStep < 2) {
        r = (this->*tbl[taskStep])();
    }
    return r;
}

BOOL ActorTalkRequest::requestPlayMelody(Vec2 *p) {
    using namespace nF;
    if ((this)->startTask(10)) {
        *(long long *)&melodyPattern = *(long long *)p;
        randomMelody = 0;
        return TRUE;
    }
    return FALSE;
}

BOOL ActorTalkRequest::requestPlayRandomMelody() {
    using namespace nF;
    if ((this)->startTask(10)) {
        randomMelody = 1;
        return TRUE;
    }
    return FALSE;
}

BOOL ActorTalkRequest::melodyStart() {
    using namespace nF;
    u32 v;
    u8 *tbl = gMelodyEditPattern;
    if (ownerActor != NULL) {
        v = ownerActor->getSpecies();
    } else {
        v = 0;
    }
    if (v != 0xffff) {
        if (window != NULL) {
            ((TalkWindowState *)(window))->lockAdvance();
        }
        if (randomMelody == 0) {
            Melody_Unpack(melodyPattern, tbl);
            Melody_PlayEditPattern((u16)v);
        } else {
            Melody_PlayRandom((u16)v);
        }
        taskStep = 1;
        return 0;
    }
    taskStep = 3;
    return 1;
}

BOOL ActorTalkRequest::melodyWait() {
    using namespace nF;
    if (data_020ddf8c != -1) {
        taskStep = 2;
    }
    return 0;
}

BOOL ActorTalkRequest::melodyEnd() {
    using namespace nF;
    if (data_020ddf8c != -1) {
        return 0;
    }
    if (window != NULL) {
        ((TalkWindowState *)(window))->unlockAdvance();
    }
    taskStep = 3;
    return 1;
}

namespace nZ {
extern "C" {
extern const s32 data_020c6d20 = 0xf;
extern const u16 data_020c6ce8[2] = {0x43, 0x0};
extern const s32 sNpcTalkMouthAnims[2] = {0x137, 0x138};
extern const u16 data_020c6cfc[2] = {0x4e, 0x0};
extern const u16 data_020c6d58[4] = {0x55, 0x0, 0x56, 0x0};
extern const s32 data_020c6d60[2] = {0xe3, 0xe1};
extern const u16 data_020c6ce4[2] = {0x47, 0x6};
}
}

BOOL ActorTalkRequest::taskMelody() {
    using namespace nF;
    static Unk_02014420_Fn tbl[] = {&ActorTalkRequest::melodyStart, &ActorTalkRequest::melodyWait, &ActorTalkRequest::melodyEnd};
    if (taskStep < 3) {
        return (this->*tbl[taskStep])();
    }
    return 0;
}

BOOL ActorTalkRequest::requestSwitchSpeaker(u8 v) {
    using namespace nE;
    if ((this)->startTask(0xb)) {
        focusNewSpeaker = v;
        return TRUE;
    }
    return FALSE;
}

BOOL ActorTalkRequest::switchSpeakerClose() {
    using namespace nE;
    TalkWindowState *q = window;
    if (q != 0) {
        q->openMode = 2;
        if (focusNewSpeaker != 0) taskStep = 1;
        else taskStep = 2;
    }
    return FALSE;
}

BOOL ActorTalkRequest::switchSpeakerFocus() {
    using namespace nE;
    TalkWindowState *q = window;
    if (q != 0 && q->state == 5) {
        NpcActor *e = (this)->getActorB((speakerIndex + 1) & 1);
        if (e != 0) {
            VecFx32 v;
            VecFx32 *pv = (VecFx32 *)&e->position;
            v.x = pv->x;
            v.y = pv->y;
            v.z = pv->z;
            v.y += 0x2000;
            Camera_RetargetFocus(&v);
        }
        taskStep = 2;
    }
    return FALSE;
}

BOOL ActorTalkRequest::switchSpeakerSwap() {
    using namespace nE;
    TalkWindowState *q = window;
    if (q != 0 && q->state == 5) {
        if (Camera_IsBlending() == 0 || Camera_GetBlendFramesLeft() < 0x11) {
            NpcActor *e = (this)->getActorB(speakerIndex);
            if (e) e->speechState.stopSpeaking();
            speakerIndex = (speakerIndex + 1) & 1;
            NpcActor *n = (this)->getActorB(speakerIndex);
            if (n != 0) {
                MsgString9B x;
                n->getName(&x);
                ((TalkMsgRequest *)(void *)(this))->changeSpeakerName((MsgString *)(void *)(&x), (u32)(((NpcActor *)(n))->getSpeakerGender()));
                window->nextState = 1;
                taskStep = 3;
                return TRUE;
            }
        }
    }
    return FALSE;
}

namespace nZ {
extern "C" {
}
}

BOOL ActorTalkRequest::taskSwitchSpeaker() {
    using namespace nE;
    static BOOL (ActorTalkRequest::*tbl[3])() = {&ActorTalkRequest::switchSpeakerClose, &ActorTalkRequest::switchSpeakerFocus, &ActorTalkRequest::switchSpeakerSwap};
    if (taskStep < 3) return (this->*tbl[taskStep])();
    return FALSE;
}

NpcTalkCtrl::NpcTalkCtrl() {}

namespace nE {
extern "C" void NpcTalkCtrl_Destroy(void) {}
}

void NpcTalkCtrl::reset() {
    using namespace nE;
    state = 5;
    requestedState = 5;
    step = 0;
    turnSpeed = 0;
    turnAngle = 0;
    clearActorFlagOnEnd = 0;
    act07Variant = 5;
    stopAction = 1;
    keepCamera = 0;
    unk_0e = 0;
}

BOOL NpcTalkCtrl::isBusy() {
    using namespace nE;
    if (requestedState < 5 || state < 5) return TRUE;
    return FALSE;
}

void NpcTalkCtrl::updateSpeakerMouth(NpcActor *ctx) {
    using namespace nE;
    ActorTalkRequest *o = ctx->talkRequest;
    if (o != 0 && o->window != 0) {
        NpcActor *e = o->getSpeakerActor();
        if (e != 0) {
            if ((o->window)->isVoicePlaying()) e->speechState.startSpeaking();
            else e->speechState.stopSpeaking();
        }
    }
}

BOOL NpcTalkCtrl::requestTurnAndTalk(s16 d, s16 e, u8 g) {
    using namespace nE;
    return request(0, 0, d, e, 1, g);
}

BOOL NpcTalkCtrl::requestTalk(u8 f, u8 g) {
    using namespace nE;
    return request(1, 0, 0, 0, f, g);
}

BOOL NpcTalkCtrl::requestState4(u32 c, s32 d, s16 e, u8 g) {
    using namespace nE;
    return request(4, c, d, e, 1, g);
}

BOOL NpcTalkCtrl::request(u8 b, u32 c, s16 d, s16 e, u8 f, u8 g) {
    using namespace nE;
    BOOL r = FALSE;
    if (requestedState == 5) {
        requestedState = b;
        turnSpeed = d;
        turnAngle = e;
        act07Variant = c;
        stopAction = f;
        keepCamera = g;
        r = TRUE;
    }
    return r;
}

void NpcTalkCtrl::startTalkMessage(NpcActor *ctx) {
    using namespace nE;
    ActorTalkRequest *o = ctx->talkRequest;
    if (o != 0) {
        struct { s32 pad; TalkStartMsg msg; } out;
        MsgString9B x;
        ctx->getName(&x);
        MsgString9B *px = &x;
        u8 *r = px->data();
        ((TalkMsgRequest *)(void *)(o))->setSpeakerName(r, (u32)(((NpcActor *)(ctx))->getSpeakerGender()));
        ((Character *)(void *)(ctx))->attachTalkRequest(o);
        o->start(&out.msg);
        o->setFileName(out.msg.msgKey);
        o->msgIndex = out.msg.msgIndex;
        o->window->nextState = 1;
    }
}

void NpcTalkCtrl::applyRequest(NpcActor *ctx) {
    using namespace nE;
    u8 b = requestedState;
    if (b < 5 && state == 5) {
        state = b;
        step = 0;
        unk_0e = 0;
        (this->*sNpcTalkCtrlStates[state].setupFn)(ctx);
        requestedState = 5;
    }
}

void NpcTalkCtrl::update(NpcActor *ctx) {
    using namespace nE;
    applyRequest(ctx);
    if (state < 5) (this->*sNpcTalkCtrlStates[state].mainFn)(ctx);
}

void NpcTalkCtrl::endTalk(NpcActor *ctx) {
    using namespace nE;
    ActorTalkRequest *o = ctx->talkRequest;
    if (o != 0 && (o)->getPartnerActor() != 0) (o)->getPartnerActor()->onLeaveTalk();
    if (keepCamera == 0) Camera_SetModeDefault();
    if (clearActorFlagOnEnd == 1) ctx->collider.groups &= ~2;
    PlayerActor_SetHeadTilt(0, 0, 4);
}

void NpcTalkCtrl::setupState0(NpcActor *ctx) {
    using namespace nE;
    volatile VecFx32 v;
    VecFx32 w;
    VecFx32 *pv = (VecFx32 *)&ctx->position;
    v.x = pv->x;
    v.y = pv->y;
    v.z = pv->z;
    ((NpcActionCtrl *)(&ctx->actionCtrl))->requestAction(3, 2, 0, 0, turnSpeed, turnAngle, 0, 0, data_020c6cc8[0], 0);
    if (keepCamera == 0) {
        ActorTalkRequest *o = ctx->talkRequest;
        if (o != 0 && (o)->getPartnerActor() != 0) {
            VecFx32 *pw = (VecFx32 *)&(o)->getPartnerActor()->position;
            w.x = pw->x;
            w.y = pw->y;
            w.z = pw->z;
            v.y += 0x2000;
            w.y += 0x2000;
            Camera_FocusOnPair((VecFx32 *)&v, &w);
        } else {
            v.y += 0x2000;
            Camera_FocusOnPoint((VecFx32 *)&v);
        }
    }
    if (!(ctx->collider.groups & 2)) clearActorFlagOnEnd = 1;
    ctx->collider.groups |= 2;
    if (ctx->canPlayTalkMelody()) {
        u16 species = ctx->getSpecies();
        if (species != 0xffff) Melody_Play(species);
        ctx->onTalkMelodyPlayed();
    }
    step = 0;
}

void NpcTalkCtrl::state0Step0(NpcActor *ctx) {
    using namespace nE;
    if (turnAngle == ctx->rotY) {
        if (((NpcActionCtrl *)(&ctx->actionCtrl))->getAction() == 3) {
            if (((NpcActionCtrl *)(&ctx->actionCtrl))->isActionDone()) {
                ((NpcActionCtrl *)(&ctx->actionCtrl))->requestAction(0, 2, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
                startTalkMessage(ctx);
                step = 1;
            }
        }
    }
}

void NpcTalkCtrl::state0Step1(NpcActor *ctx) {
    using namespace nE;
    ActorTalkRequest *o = ctx->talkRequest;
    s32 a, b;
    if (o != 0 && o->window != 0) {
        if (o->window->state == 0) {
            o->runDeferred();
            ((Character *)(void *)(ctx))->detachTalkRequest(o);
            NpcActor *e = o->getSpeakerActor();
            if (e) e->speechState.stopSpeaking();
            a = 0;
            if ((o)->getPartnerActor() != 0 && (&(o)->getPartnerActor()->actionCtrl)->getAction() == 8 && (&(o)->getPartnerActor()->actionCtrl)->isActionDone() == 0) a = 1;
            b = 0;
            if (((NpcActionCtrl *)(&ctx->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&ctx->actionCtrl))->isActionDone() == 0) b = 1;
            if (a != 0 || b != 0) {
                if (b != 0) (o)->playEmotion(0, 0);
                if (a != 0) (o)->playEmotion(0, 1);
                step = 2;
            } else {
                endTalk(ctx);
                step = 3;
                state = 5;
            }
        } else {
            (o)->tick();
            updateSpeakerMouth(ctx);
        }
    }
}

void NpcTalkCtrl::state0Step2(NpcActor *ctx) {
    using namespace nE;
    ActorTalkRequest *o = ctx->talkRequest;
    if (o != 0 && (o)->getPartnerActor() != 0 && (&(o)->getPartnerActor()->actionCtrl)->getAction() == 8 && (&(o)->getPartnerActor()->actionCtrl)->isActionDone() == 0) return;
    if (((NpcActionCtrl *)(&ctx->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&ctx->actionCtrl))->isActionDone() == 0) return;
    endTalk(ctx);
    step = 3;
    state = 5;
}

namespace nZ {
extern "C" {
extern const u16 data_020c6cf4[2] = {0x4f, 0x0};
extern const u16 data_020c6d34[2] = {0x58, 0x6};
Unk_0201a1e0_Fn sNpcLookAtTypes[6] = {
    &NpcLookAt::relax,
    &NpcLookAt::lookAtLocalPlayer,
    &NpcLookAt::lookAtTargetActor,
    &NpcLookAt::lookAtPoint,
    &NpcLookAt::lookAtTargetPlayer,
    &NpcLookAt::approachManualAngles,
};
}
}

void NpcTalkCtrl::mainState0(NpcActor *ctx) {
    using namespace nE;
    static void (NpcTalkCtrl::*tbl[3])(NpcActor *) = {&NpcTalkCtrl::state0Step0, &NpcTalkCtrl::state0Step1, &NpcTalkCtrl::state0Step2};
    if (step < 3) (this->*tbl[step])(ctx);
}

void NpcTalkCtrl::setupState2(NpcActor *ctx) {
    using namespace nE;
    volatile VecFx32 v;
    VecFx32 *pv = (VecFx32 *)&ctx->position;
    v.x = pv->x;
    v.y = pv->y;
    v.z = pv->z;
    ((NpcActionCtrl *)(&ctx->actionCtrl))->requestAction(3, 2, 0, 0, turnSpeed, turnAngle, 0, 0, data_020c6cc8[0], 0);
    if (!(ctx->collider.groups & 2)) clearActorFlagOnEnd = 1;
    ctx->collider.groups |= 2;
    if (ctx->canPlayTalkMelody()) {
        u16 species = ctx->getSpecies();
        if (species != 0xffff) Melody_Play(species);
        ctx->onTalkMelodyPlayed();
    }
    step = 0;
}

void NpcTalkCtrl::state2Step0(NpcActor *ctx) {
    using namespace nE;
    if (((NpcActionCtrl *)(&ctx->actionCtrl))->getAction() == 3) {
        if (((NpcActionCtrl *)(&ctx->actionCtrl))->isActionDone()) {
            ((NpcActionCtrl *)(&ctx->actionCtrl))->requestAction(0, 2, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
            ((NpcActionCtrl *)(&ctx->actionCtrl))->requestTalkingOn();
            step = 1;
        }
    }
}

void NpcTalkCtrl::state2Step1(NpcActor *ctx) {
    using namespace nE;
    if (unk_0e) {
        ((NpcActionCtrl *)(&ctx->actionCtrl))->requestTalkingOff();
        step = 2;
        state = 5;
    }
}

void NpcTalkCtrl::mainState2(NpcActor *ctx) {
    using namespace nE;
    static void (NpcTalkCtrl::*tbl[2])(NpcActor *) = {&NpcTalkCtrl::state2Step0, &NpcTalkCtrl::state2Step1};
    if (step < 2) (this->*tbl[step])(ctx);
}

void NpcTalkCtrl::setupState1(NpcActor* p) {
    using namespace nD;
    Unk_020133cc_Vec pos;
    Unk_020133cc_Vec* pv = (Unk_020133cc_Vec *)&p->position;
    pos = *pv;
    if (stopAction != 0) {
        ((NpcActionCtrl *)(&p->actionCtrl))->requestAction(0, 2, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
    }
    step = 0;
    if (keepCamera == 0) {
        ActorTalkRequest* r6 = p->talkRequest;
        if (r6 != NULL && ((ActorTalkRequest *)(r6))->getPartnerActor() != NULL) {
            Unk_020133cc_Vec w;
            w = *(Unk_020133cc_Vec *)&((ActorTalkRequest *)(r6))->getPartnerActor()->position;
            pos.y += 0x2000;
            w.y += 0x2000;
            Camera_FocusOnPair(&pos, &w);
        } else {
            pos.y += 0x2000;
            Camera_FocusOnPoint(&pos);
        }
    }
    if (!(p->collider.groups & 2)) {
        clearActorFlagOnEnd = 1;
    }
    p->collider.groups |= 2;
    if (p->canPlayTalkMelody()) {
        u16 species = p->getSpecies();
        if (species != 0xffff) {
            Melody_Play(species);
        }
        p->onTalkMelodyPlayed();
    }
}

void NpcTalkCtrl::state1Step0(NpcActor* p) {
    using namespace nD;
    ((NpcTalkCtrl *)(this))->startTalkMessage(p);
    step = 1;
}

void NpcTalkCtrl::state1Step1(NpcActor* p) {
    using namespace nD;
    ActorTalkRequest* r4 = p->talkRequest;
    if (r4 != NULL) {
        if (r4->window != NULL) {
            if (r4->window->state == 0) {
                r4->runDeferred();
                ((Character *)(p))->detachTalkRequest(r4);
                NpcActor* t = ((ActorTalkRequest *)(r4))->getSpeakerActor();
                if (t != NULL) {
                    ((NpcSpeechState *)(&t->speechState))->stopSpeaking();
                }
                BOOL a = FALSE;
                if (((ActorTalkRequest *)(r4))->getPartnerActor() != NULL && ((NpcActionCtrl *)(&((ActorTalkRequest *)(r4))->getPartnerActor()->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&((ActorTalkRequest *)(r4))->getPartnerActor()->actionCtrl))->isActionDone() == 0) {
                    a = TRUE;
                }
                BOOL b = FALSE;
                if (((NpcActionCtrl *)(&p->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&p->actionCtrl))->isActionDone() == 0) {
                    b = TRUE;
                }
                if (a || b) {
                    if (b) ((ActorTalkRequest *)(r4))->playEmotion(0, 0);
                    if (a) ((ActorTalkRequest *)(r4))->playEmotion(0, 1);
                    step = 2;
                } else {
                    ((NpcTalkCtrl *)(this))->endTalk(p);
                    step = 1;
                    state = 5;
                }
            } else {
                ((ActorTalkRequest *)(r4))->tick();
                ((NpcTalkCtrl *)(this))->updateSpeakerMouth(p);
            }
        }
    }
}

void NpcTalkCtrl::state1Step2(NpcActor* p) {
    using namespace nD;
    ActorTalkRequest* r6 = p->talkRequest;
    if (r6 != NULL && ((ActorTalkRequest *)(r6))->getPartnerActor() != NULL && ((NpcActionCtrl *)(&((ActorTalkRequest *)(r6))->getPartnerActor()->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&((ActorTalkRequest *)(r6))->getPartnerActor()->actionCtrl))->isActionDone() == 0) {
        return;
    }
    if (((NpcActionCtrl *)(&p->actionCtrl))->getAction() == 8 && ((NpcActionCtrl *)(&p->actionCtrl))->isActionDone() == 0) {
        return;
    }
    ((NpcTalkCtrl *)(this))->endTalk(p);
    step = 1;
    state = 5;
}

namespace nZ {
extern "C" {
}
}

void NpcTalkCtrl::mainState1(NpcActor* p) {
    using namespace nD;
    static Unk_02013474_Fn tbl[3] = {&NpcTalkCtrl::state1Step0, &NpcTalkCtrl::state1Step1, &NpcTalkCtrl::state1Step2};
    if (step < 3) {
        (this->*tbl[step])(p);
    }
}

void NpcTalkCtrl::setupState3(NpcActor* p) {
    using namespace nD;
    Unk_02013778_Vec pos;
    Unk_020133cc_Vec* pv = (Unk_020133cc_Vec *)&p->position;
    *(Unk_020133cc_Vec*)&pos = *pv;
    if (stopAction != 0) {
        ((NpcActionCtrl *)(&p->actionCtrl))->requestAction(0, 2, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
    }
    ((NpcActionCtrl *)(&p->actionCtrl))->requestTalkingOn();
    if (!(p->collider.groups & 2)) {
        clearActorFlagOnEnd = 1;
    }
    p->collider.groups |= 2;
    if (p->canPlayTalkMelody()) {
        u16 species = p->getSpecies();
        if (species != 0xffff) {
            Melody_Play(species);
        }
        p->onTalkMelodyPlayed();
    }
    step = 0;
}

void NpcTalkCtrl::mainState3(NpcActor* p) {
    using namespace nD;
    static Unk_02013474_Fn tbl[1] = {&NpcTalkCtrl::state2Step1};
    if (step < 1) {
        (this->*tbl[step])(p);
    }
}

void NpcTalkCtrl::setupState4(NpcActor* p) {
    using namespace nD;
    ((NpcActionCtrl *)(&p->actionCtrl))->requestAct07(2, (u16)turnSpeed, (u16)turnAngle, act07Variant, data_020c6cc8[0]);
    if (!(p->collider.groups & 2)) {
        clearActorFlagOnEnd = 1;
    }
    p->collider.groups |= 2;
    step = 0;
}

void NpcTalkCtrl::state4Step0(NpcActor* p) {
    using namespace nD;
    if (((NpcActionCtrl *)(&p->actionCtrl))->getAction() == 7) {
        if (((NpcActionCtrl *)(&p->actionCtrl))->isActionDone() != 0) {
            ((NpcActionCtrl *)(&p->actionCtrl))->requestAction(0, 1, 0, 0, 0, 0, 0, 0, data_020c6cc8[0], 0);
            if (clearActorFlagOnEnd == 1) {
                p->collider.groups &= ~2;
            }
            step = 1;
            state = 5;
        }
    }
}

namespace nZ {
extern "C" {
extern const u32 sNpcGiveItemAnims[2] = {0x22, 0x2b};
NpcActionEntry sNpcActionTable[22] = {
    &NpcActionCtrl::setupAct00, &NpcActionCtrl::mainAct00, 0,
    &NpcActionCtrl::setupAct01, &NpcActionCtrl::mainAct01, 0,
    &NpcActionCtrl::setupAct02, &NpcActionCtrl::mainAct02, 0,
    &NpcActionCtrl::setupAct03, &NpcActionCtrl::mainAct03, 0,
    &NpcActionCtrl::setupAct04, &NpcActionCtrl::mainAct04, 0,
    &NpcActionCtrl::setupAct05, &NpcActionCtrl::mainAct05, 0,
    &NpcActionCtrl::setupAct06, &NpcActionCtrl::mainAct05, 0,
    &NpcActionCtrl::setupAct07, &NpcActionCtrl::mainAct07, &NpcActionCtrl::postAct07,
    &NpcActionCtrl::setupAct08, &NpcActionCtrl::mainAct08, 0,
    &NpcActionCtrl::setupAct09, &NpcActionCtrl::mainAct09, 0,
    &NpcActionCtrl::setupAct0A, &NpcActionCtrl::mainAct0A, &NpcActionCtrl::postAct0A,
    &NpcActionCtrl::setupAct0B, &NpcActionCtrl::mainAct0B, &NpcActionCtrl::postAct0B,
    &NpcActionCtrl::setupAct0C, 0, &NpcActionCtrl::postAct0C,
    &NpcActionCtrl::setupAct0D, &NpcActionCtrl::mainAct0D, &NpcActionCtrl::postAct0D,
    &NpcActionCtrl::setupAct0E, &NpcActionCtrl::mainAct0E, &NpcActionCtrl::postAct0E,
    &NpcActionCtrl::setupAct0F, 0, &NpcActionCtrl::postAct0F,
    &NpcActionCtrl::setupAct10, 0, &NpcActionCtrl::postAct10,
    &NpcActionCtrl::setupAct11, &NpcActionCtrl::mainAct11, 0,
    &NpcActionCtrl::setupAct12, &NpcActionCtrl::mainAct12, 0,
    &NpcActionCtrl::setupAct13, &NpcActionCtrl::mainAct13, &NpcActionCtrl::postAct13,
    &NpcActionCtrl::setupAct14, 0, &NpcActionCtrl::postAct14,
    &NpcActionCtrl::setupAct15, &NpcActionCtrl::mainAct15, 0,
};
NpcTalkCtrlState sNpcTalkCtrlStates[5] = {
    &NpcTalkCtrl::setupState0, &NpcTalkCtrl::mainState0,
    &NpcTalkCtrl::setupState1, &NpcTalkCtrl::mainState1,
    &NpcTalkCtrl::setupState2, &NpcTalkCtrl::mainState2,
    &NpcTalkCtrl::setupState3, &NpcTalkCtrl::mainState3,
    &NpcTalkCtrl::setupState4, &NpcTalkCtrl::mainState4,
};
extern const NpcEmotionEntry sEmotionTable[60] = {
    {{{0x0, 0, 0x0, 0x1, -1}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x45, data_020c6d34, 0x1, 0x1, 6}, {0x46, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x47, data_020c6ce8, 0x1, 0x1, 0}, {0x48, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x49, data_020c6d18, 0x1, 0x1, 0}, {0x4a, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x4b, data_020c6d38, 0x1, 0x1, 2}, {0x4c, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x4d, 0, 0x0, 0x1, -1}, {0x4e, data_020c6d08, 0x1, 0x1, 3}}, 0x1, 0x0},
    {{{0x4f, data_020c6cdc, 0x1, 0x1, 0}, {0x50, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x51, data_020c6d14, 0x1, 0x1, 0}, {0x52, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x53, data_020c6d3c, 0x1, 0x1, 5}, {0x54, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x55, data_020c6cec, 0x1, 0x1, 0}, {0x56, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x57, data_020c6d28, 0x1, 0x1, 0}, {0x58, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x59, data_020c6cd8, 0x1, 0x1, 0}, {0x5a, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x5b, data_020c6d24, 0x1, 0x1, 9}, {0x5c, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x5d, data_020c6d2c, 0x1, 0x1, 0}, {0x5e, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x5f, data_020c6d58, 0x2, 0x1, 0}, {0x60, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x61, data_020c6d10, 0x1, 0x1, 0}, {0x62, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x63, data_020c6cf8, 0x1, 0x1, 0}, {0x64, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x65, data_020c6ce4, 0x1, 0x1, 6}, {0x66, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x67, data_020c6d70, 0x2, 0x1, 0}, {0x68, data_020c6ce0, 0x1, 0x0, -1}}, 0x1, 0x0},
    {{{0x69, data_020c6cd0, 0x1, 0x1, 0}, {0x6a, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x6b, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x6c, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x6d, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x6e, data_020c6cfc, 0x1, 0x1, 0}, {0x6f, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x70, data_020c6d50, 0x2, 0x1, 3}, {0x71, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x72, data_020c6d30, 0x1, 0x1, 0}, {0x73, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x74, data_020c6d04, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x0, -1}}, 0x0, 0x0},
    {{{0x75, data_020c6d30, 0x1, 0x1, 0}, {0x76, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x77, data_020c6d0c, 0x1, 0x1, 0}, {0x78, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x79, data_020c6cf4, 0x1, 0x1, 0}, {0x7a, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x7b, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x7c, data_020c6d30, 0x1, 0x1, 0}, {0x7d, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x7e, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x7f, data_020c6cd4, 0x1, 0x1, 18}, {0x80, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x81, data_020c6d30, 0x1, 0x1, 0}, {0x82, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x83, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, 0}}, 0x0, 0x0},
    {{{0x84, data_020c6d38, 0x1, 0x1, 2}, {0x85, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x86, data_020c6cd8, 0x1, 0x1, 0}, {0x87, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x88, data_020c6d30, 0x1, 0x1, 0}, {0x89, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x8a, data_020c6d00, 0x1, 0x1, 0}, {0x8b, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x8c, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x9c, 0, 0x0, 0x1, 0}, {0x9d, data_020c6d08, 0x1, 0x1, 3}}, 0x1, 0x0},
    {{{0x9e, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, 0}}, 0x0, 0x0},
    {{{0x9f, data_020c6cec, 0x1, 0x1, 0}, {0xa0, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0xa1, data_020c6cf4, 0x1, 0x1, 0}, {0xa2, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0xa3, data_020c6d30, 0x1, 0x1, 0}, {0xa4, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0xa5, data_020c6d30, 0x1, 0x1, 0}, {0xa6, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0xa7, data_020c6d30, 0x1, 0x1, 0}, {0xa8, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x8d, data_020c6d30, 0x1, 0x1, 0}, {0x8e, 0, 0x0, 0x1, 0}}, 0x1, 0x0},
    {{{0x8f, data_020c6d30, 0x1, 0x1, 0}, {0x90, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0x91, data_020c6d30, 0x1, 0x1, 0}, {0x92, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x93, data_020c6d30, 0x1, 0x1, 0}, {0x94, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x95, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x96, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0x97, data_020c6d30, 0x1, 0x1, 0}, {0x98, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x99, data_020c6d30, 0x1, 0x1, 0}, {0x9a, 0, 0x0, 0x1, -1}}, 0x1, 0x0},
    {{{0x9b, data_020c6d30, 0x1, 0x1, 0}, {0x137, 0, 0x0, 0x1, -1}}, 0x0, 0x0},
    {{{0xa9, data_020c6d38, 0x1, 0x1, 2}, {0xaa, 0, 0x0, 0x0, -1}}, 0x1, 0x0},
    {{{0xab, data_020c6d30, 0x1, 0x1, 0}, {0xac, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
    {{{0x4f, data_020c6ccc, 0x1, 0x1, 0}, {0x50, 0, 0x0, 0x0, -1}}, 0x1, 0x1},
};
extern const u16 data_020c6cc4[2] = {0x200, 0x0};
extern const u16 data_020c6d50[4] = {0x51, 0x6, 0x52, 0x3};
}
}

void NpcTalkCtrl::mainState4(NpcActor* p) {
    using namespace nD;
    static Unk_02013474_Fn tbl[1] = {&NpcTalkCtrl::state4Step0};
    if (step < 3) {
        (this->*tbl[step])(p);
    }
}

NpcFootstepFx::NpcFootstepFx() {
    using namespace nD;
    footstepsEnabled = 0;
}

void NpcFootstepFx::func_020135e0() {
    using namespace nD;
}

void NpcFootstepFx::resetFootsteps() {
    using namespace nD;
    disableFootsteps();
    prevMoveMode = 5;
}

void NpcFootstepFx::enableFootsteps() {
    using namespace nD;
    footstepsEnabled = 1;
}

void NpcFootstepFx::disableFootsteps() {
    using namespace nD;
    footstepsEnabled = 0;
}

void NpcFootstepFx::playFootstepSe(NpcActor* p) {
    using namespace nD;
    u32 f = p->actorFlags;
    if (!(Unk_02013568_IsSet(f, 4) && Unk_02013568_IsSet(f, 2))) {
        if (footstepsEnabled != 0) {
            Snd_SeEmitterPlayAlternate(&p->seEmitter, Footstep_GetSeAtPos(&p->position));
        }
    }
}

void NpcFootstepFx::updateFootsteps(NpcActor* p) {
    using namespace nD;
    s32 st = ((NpcMoveCtrl *)(&p->moveCtrl))->getMoveMode();
    if (footstepsEnabled != 0 && (u32)(st - 1) <= 2) {
        Unk_02013474_Half h;
        h.a = p->rotY;
        if (prevMoveMode == 0) {
            Effect_Create(0x28, &p->jointPos[2], &h.a, 0);
            Effect_Create(0x28, &p->jointPos[1], &h.a, 0);
        } else {
            BOOL r4 = ((AnimFrameCtrl *)(&(AnimFrameCtrl &)p->model))->hasPassedFrame(1);
            BOOL r0 = ((AnimFrameCtrl *)(&(AnimFrameCtrl &)p->model))->hasPassedFrame(9);
            if (r4 != 0 || r0 != 0) {
                VecFx32 v;
                v.x = (r4 ? &p->jointPos[2] : &p->jointPos[1])->x;
                v.y = (r4 ? &p->jointPos[2] : &p->jointPos[1])->y;
                v.z = (r4 ? &p->jointPos[2] : &p->jointPos[1])->z;
                h.b = p->rotY + 0x8000;
                v.y = p->position.y;
                if (st == 2) Effect_Create(0, &v, &h.b, 0);
                Effect_Create(0x28, &v, &h.a, 0);
                playFootstepSe(p);
            }
        }
    }
    prevMoveMode = st;
}

void NpcActor::resetLastTaughtEmotion() {
    using namespace nD;
    lastTaughtEmotion = -1;
}

void NpcActor::setLastTaughtEmotion(s16 v) {
    using namespace nD;
    lastTaughtEmotion = v;
}

s16 NpcActor::getLastTaughtEmotion() {
    using namespace nD;
    return lastTaughtEmotion;
}

s32 NpcActor::getNewEmotionToLearn() {
    using namespace nD;
    s32 result = -1;
    if (((NpcTalkCtrl *)(&talkCtrl))->isBusy()) {
        if (((NpcActionCtrl *)(&actionCtrl))->getAction() == 8) {
            if (((NpcActor *)(this))->getTalkRequest() != NULL && ((ActorTalkRequest *)(((NpcActor *)(this))->getTalkRequest()))->getPartnerActor() == NULL) {
                s32 v = ((NpcActionCtrl *)(&actionCtrl))->getEmotionId();
                if (v > 0 && v < 0x3c && v != getLastTaughtEmotion()) {
                    if (Emotion_FindSlot((u8)v) == -1) {
                        result = v;
                        setLastTaughtEmotion((u8)v);
                    }
                }
            }
        }
    }
    return result;
}

s32 NpcActor::getTeachableEmotion() {
    using namespace nD;
    return -1;
}

// The route code keeps unit / block coordinates unsigned (RouteGridPos, VillagerRoute::lastUnitX/Z); the field
// position helpers (src/main/unk_0204cc1c.cpp) write them as s32.
static inline void RouteUnit_FromPos(u32 *x, u32 *z, VecFx32 *v) {
    s32 ux, uz;
    FieldPos_ToUnit(&ux, &uz, v);
    *x = ux;
    *z = uz;
}

static inline void RouteUnit_FromBlockUnit(u32 *x, u32 *z, s32 bx, s32 bz, s32 ux, s32 uz) {
    s32 ox, oz;
    FieldUnit_FromBlockUnit(&ox, &oz, bx, bz, ux, uz);
    *x = ox;
    *z = oz;
}

static inline void RouteGridPos_FromPos(RouteGridPos *block, RouteGridPos *unit, VecFx32 *v) {
    s32 b[2], u[2];
    FieldPos_ToBlockUnit2(b, u, v);
    block->x = b[0];
    block->z = b[1];
    unit->x = u[0];
    unit->z = u[1];
}

VillagerRoute* VillagerRoute::resetTarget() {
    using namespace nD;
    targetUnit.x = 0;
    targetUnit.y = 0;
    targetBlockX = 0;
    targetBlockZ = 0;
    waypoint.x = 0;
    waypoint.z = 0;
    lastUnitX = 0;
    lastUnitZ = 0;
    return this;
}

namespace nD {
extern "C" void VillagerRoute_Destruct() {
}
}

void VillagerRoute::reset() {
    using namespace nD;
    MI_CpuFill8(this, 0, sizeof(VillagerRoute));
    stepMode = 2;
    routeType = 7;
    unk_24 = -1;
    unk_26 = -1;
    pathMode = 2;
    approachDir = 4;
}

void VillagerRoute::start(VecFx32 *pos, u32 idx, u32 b, NpcActor *owner) {
    using namespace nD;
    if (idx < 7) {
        reset();
        routeType = idx;
        stepMode = b;
        typeTable = &sVillagerRouteTypes[routeType];
        if (typeTable->pickTarget) {
            typeTable->pickTarget(&targetUnit, this, pos, owner);
            s32 y = targetUnit.y;
            targetBlockX = targetUnit.x >> 4;
            targetBlockZ = y >> 4;
            (this)->planStep(pos);
        }
    }
}

void VillagerRoute::rememberUnit(VecFx32 *v) {
    using namespace nD;
    RouteUnit_FromPos(&lastUnitX, &lastUnitZ, v);
}

void VillagerRoute::markUnitUnset() {
    using namespace nD;
    if (lastUnitX == 0 && lastUnitZ == 0) {
        lastUnitX = 0xff;
        lastUnitZ = 0xff;
    }
}

void VillagerRoute::checkUnitChanged(VecFx32 *v) {
    using namespace nD;
    u32 a = 0;
    u32 b = 0;
    RouteUnit_FromPos(&a, &b, v);
    if (lastUnitX != 0 && lastUnitZ != 0 && (lastUnitX != a || lastUnitZ != b)) {
        stepPhase = 3;
        ((VillagerRoute *)(this))->clearJunctions();
        pathDir = 0;
        pathMode = 2;
        planStep(v);
        approachDir = 4;
    }
}

void VillagerRoute::setStepMode(u32 v) {
    using namespace nD;
    stepMode = v;
}

BOOL VillagerRoute::isActive() {
    using namespace nD;
    if (routeType < 7) return TRUE;
    return FALSE;
}

s32 VillagerRoute::scanForPath(VecFx32 *cand, RouteGridPos *pos, VecXZ *step, s32 n, RouteGridPos *bound, s32 flag,
                               BlockMap *map) {
    using namespace nD;
    s32 i;
    u32 x = pos->x;
    u32 y = pos->z;
    s32 cnt = 0;
    s32 zero = 0;
    for (i = 0; i < n; i++) {
        x += step->x;
        y += step->z;
        if (x >= bound->x || y >= bound->z) continue;
        if (flag != 0) {
            GroundInfo o(x, y, zero, zero);
            if (o.waterKind != 0) cnt++; else cnt = zero;
            if (cnt >= 4) break;
        }
        if (map->getWalkLinks(x, y)) {
            FieldPos_FromUnitCenter(cand, x, y);
            return TRUE;
        }
    }
    return FALSE;
}

u32 VillagerRoute::pickAdjacentUnit(VecFx32 *v, BlockMap *o) {
    using namespace nC;
    u8 mask = 0;
    RouteGridPos a, c, out;
    s32 n, i;
    u32 dir;
    a.x = 0;
    a.z = 0;
    c.x = 0;
    c.z = 0;
    out.x = 0;
    out.z = 0;
    n = 0;
    RouteGridPos_FromPos(&a, &c, v);
    for (i = 0; i < 4; i++) {
        u32 x = c.x + sRouteDirs[i].x;
        u32 z = c.z + sRouteDirs[i].z;
        if (x < 16 && z < 16) {
            RouteUnit_FromBlockUnit(&out.x, &out.z, a.x, a.z, x, z);
            if (TownMap_IsUnitWalkable(out.x, out.z, o) != 0) {
                mask = mask | (1 << i);
                n++;
            }
        }
    }
    dir = Random_PickSetBit(mask, n, 4);
    if (dir < 4) {
        FieldPos_FromBlockUnitCenter(v, a.x, a.z, c.x + sRouteDirs[dir].x, c.z + sRouteDirs[dir].z);
        return dir;
    }
    return 4;
}

u32 VillagerRoute::findNearestPath(RouteGridPos *out, VecFx32 *pos) {
    using namespace nC;
    VecFx32 best;
    VecFx32 cand[4];
    u8 mask;
    BlockMap *o;
    RouteGridPos p, q;
    s32 dir;
    s32 x0, z0, x1, z1, flag, off, bestd;
    o = gSceneBlockMap;
    best.x = pos->x;
    best.y = pos->y;
    best.z = pos->z;
    mask = 0;
    p.x = 0;
    p.z = 0;
    q.x = 0;
    q.z = 0;
    dir = 4;
    if (o == 0) {
        return 4;
    }
    RouteUnit_FromPos(&p.x, &p.z, pos);
    RouteGridPos *pq = (RouteGridPos *)&o->unitsX;
    q.x = pq->x;
    q.z = pq->z;
    if (o->getWalkLinksAtPos(pos) != 0) {
        return 4;
    }
    for (s32 i = 0; i < 4; i++) {
        cand[i].x = pos->x;
        cand[i].y = pos->y;
        cand[i].z = pos->z;
    }
    RouteUnit_FromPos(&p.x, &p.z, pos);
    flag = (BlockMap_GetBlockAttr(o, pos->x >> 17, pos->z >> 17) & 0x7f000) != 0 ? 1 : 0;
    x0 = p.x & 0xfff0;
    z0 = p.z & 0xfff0;
    x1 = x0 + 16;
    z1 = z0 + 16;
    if (scanForPath(&cand[0], &p, &sRouteDirs[0], p.z - z0, &q, flag, o)) {
        mask |= 1;
    }
    if (scanForPath(&cand[2], &p, &sRouteDirs[2], z1 - p.z, &q, flag, o)) {
        mask |= 4;
    }
    if (scanForPath(&cand[1], &p, &sRouteDirs[1], p.x - x0, &q, flag, o)) {
        mask |= 2;
    }
    if (scanForPath(&cand[3], &p, &sRouteDirs[3], x1 - p.x, &q, flag, o)) {
        mask |= 8;
    }
    if (mask != 0) {
        bestd = -4096;
        for (s32 i = 0; i < 4; i++) {
            s32 d;
            VecFx32 *pc = cand;
            d = Vec_DistXZ(pos, &pc[i]);
            if (((mask >> i) & 1) != 0) {
                if (bestd < 0 || d < bestd) {
                    best = pc[i];
                    bestd = d;
                    dir = i;
                }
            }
        }
    } else {
        u32 r = pickAdjacentUnit(&best, o);
        if (r < 4) {
            dir = r;
        }
    }
    RouteUnit_FromPos(&out->x, &out->z, &best);

    return dir;
}

s32 VillagerRoute::isArrived(VecFx32 *v) {
    using namespace nC;
    VillagerRouteType *t = typeTable;
    if (t != 0) {
        if (t->arrivedCheck != 0) {
            return (this->*(t->arrivedCheck))(v);
        }
    }
    return 0;
}

s32 VillagerRoute::firstDir(s32 v) {
    using namespace nC;
    if (v != 0) {
        for (s32 i = 0; i < 4; i++) {
            if (((v >> i) & 1) != 0) {
                return i;
            }
        }
    }
    return 4;
}

s32 VillagerRoute::scanDir(RouteGridPos *out, RouteGridPosCopy pos, u32 mask, RouteGridPos *lim, BlockMap *obj) {
    using namespace nC;
    s32 m, dx, dz;
    s32 d = firstDir(mask);
    if (d < 4) {
        dx = sRouteDirs[d].x;
        dz = sRouteDirs[d].z;
        m = ~(mask | oppositeDirs(mask));
        s32 cnt = 0;
        while (pos.x < lim->x && pos.z < lim->z) {
            pos.x += dx;
            pos.z += dz;
            cnt++;
            s32 r = obj->getWalkLinks(pos.x, pos.z);
            if (r == 0) {
                u32 z = pos.z - dz;
                out->x = pos.x - dx;
                out->z = z;
                return -(cnt - 1);
            }
            if ((r & m) != 0) {
                out->x = pos.x;
                out->z = pos.z;
                return cnt;
            }
        }
    }
    return 0;
}

void VillagerRoute::clearJunctions() {
    using namespace nC;
    MI_CpuFill8(junctions, 0, 90);
}

s32 VillagerRoute::countJunctions() {
    using namespace nC;
    u8 *e = junctions;
    for (s32 i = 0; i < 29; e += 3, i++) {
        if (((u32)(e[2] << 24) >> 28) == 0) {
            return i;
        }
    }
    return 29;
}

void VillagerRoute::pushJunction(u32 hi, u32 lo, RouteGridPos *p) {
    using namespace nC;
    s32 n = countJunctions();
    u8 *e = junctions + n * 3;
    for (; n > 0; n--) {
        MI_CpuCopy8(e - 3, e, 3);
        e -= 3;
    }
    e[2] = (e[2] & ~0xf0) | (((u8)(hi & 0xf) & 0xf) << 4);
    e[2] = (e[2] & ~0xf) | ((u8)(lo & 0xf) & 0xf);
    e[0] = p->x;
    e[1] = p->z;
}

s32 VillagerRoute::findJunction(RouteGridPos *p) {
    using namespace nC;
    u8 *e = junctions;
    for (s32 i = 0; i < 30; e += 3, i++) {
        if (p->x == e[0] && p->z == e[1]) {
            return i;
        }
    }
    return -1;
}

RouteJunction VillagerRoute::popJunction(RouteGridPos *p) {
    using namespace nC;
    s32 idx = findJunction(p);
    RouteJunction out;
    MI_CpuFill8(&out, 0, 3);
    if (idx != -1) {
        u8 *a = junctions;
        u8 *b = a + idx * 3;
        MI_CpuCopy8(b, &out, 3);
        b += 3;
        for (s32 i = idx; i < 29; i++) {
            MI_CpuCopy8(b, a, 3);
            b += 3;
            a += 3;
        }
        MI_CpuFill8(junctions + (29 - idx) * 3, 0, (idx + 1) * 3);
    }
    return out;
}

u32 VillagerRoute::getStage() {
    using namespace nC;
    return routeType;
}

s32 VillagerRoute::runStep(VecFx32 *v) {
    using namespace nC;
    VillagerRouteType *t = typeTable;
    if (t != 0 && stepMode < 2 && stepPhase < 3) {
        Unk_02012810_Fn *pf = &t->tbl[stepMode][stepPhase];
        if (*pf != 0) {
            s32 r = (this->*(*pf))(v);
            if (stepMode == 1) {
                ((VillagerRoute *)(this))->rememberUnit(v);
            }
            return r;
        }
    }
    return 0;
}

namespace nC {
extern "C" void VillagerRoute_PickPathUnitInBlock(Vec2 *out, VillagerRoute *unused, Vec2 *p, BlockMap *obj) {
    u32 bx = 0, bz = 0;
    u16 mask[16];
    s32 cnt = 0;
    s32 z, x;
    MI_CpuFill8(mask, 0, 32);
    RouteUnit_FromBlockUnit(&bx, &bz, p->x, p->y, 0, 0);
    for (z = 0; z < 16; z++) {
        for (x = 0; x < 16; x++) {
            if (obj->getWalkLinks(bx + x, bz + z) != 0) {
                mask[z] |= 1 << x;
                cnt++;
            }
        }
    }
    if (cnt > 0) {
        s32 k = Random_GlobalBelow(cnt);
        u16 *row;
        s32 xx, zz;
        for (zz = 0; zz < 16; zz++) {
            for (xx = 0, row = &mask[zz]; xx < 16; xx++) {
                if (((*row >> xx) & 1) != 0) {
                    if (k == 0) {
                        out->x = 0;
                        out->y = 0;
                        out->x = bx + xx;
                        out->y = bz + zz;
                        return;
                    }
                    k--;
                }
            }
        }
    }
    out->x = 0;
    out->y = 0;
}
}

s32 VillagerRoute::oppositeDirs(s32 v) {
    using namespace nC;
    if (v != 0) {
        v = v << 2;
        if ((v & 0xf) == 0) {
            v = (v >> 4) & 0xf;
        }
        return v;
    }
    return 0;
}

void VillagerRoute::planStep(VecFx32 *pos) {
    using namespace nC;
    BlockMap *o = gSceneBlockMap;
    s32 dirs, found;
    s32 lo2, present, dirs2, cand, bestd, i, hi, lo, w, bx, bz;
    RouteGridPos q, p, r;
    VecFx32 uv, tv;

    if (o == 0) {
        return;
    }
    present = o->getWalkLinksAtPos(pos);
    if (present == 0) {
        approachDir = findNearestPath(&waypoint, pos);
        stepPhase = 0;
        clearJunctions();
        pathDir = 0;
        pathMode = 2;
        return;
    }
    dirs2 = dirs = oppositeDirs(pathDir);
    RouteGridPos *pq = (RouteGridPos *)&o->unitsX;
    q.x = pq->x;
    q.z = pq->z;
    bx = 0;
    p.x = 0;
    p.z = 0;
    bz = 0;
    found = 0;
    r.x = 0;
    r.z = 0;
    bestd = -4096;
    RouteUnit_FromPos(&p.x, &p.z, pos);
    if ((present & ~dirs) == 0) {
        if (scanDir(&r, p, dirs, &q, o)) {
            waypoint.x = r.x;
            waypoint.z = r.z;
            pathMode = 1;
            pathDir = dirs;
        }
    } else if (pathMode != 1) {
        hi = found;
        lo = pathDir;
        if (findJunction(&p) != -1) {
            RouteJunction e1 = popJunction(&p);
            hi = e1.f.hi;
            lo = e1.f.lo;
            dirs2 = dirs2 | hi;
        }
        FieldPos_FromUnitCenter(&tv, targetUnit.x, targetUnit.y);
        for (i = 0; i < 4; i++) {
            if (((dirs2 >> i) & 1) == 0) {
                cand = present & (1 << i);
                if (cand != 0) {
                    if (scanDir(&r, p, cand, &q, o)) {
                        s32 d;
                        FieldPos_FromUnitCenter(&uv, r.x, r.z);
                        d = Vec_DistXZ(&tv, &uv);
                        if (bestd < 0 || d < bestd) {
                            bx = r.x;
                            bz = r.z;
                            bestd = d;
                            found = cand;
                        }
                    }
                }
            }
        }
        if (found != 0) {
            if (pathMode == 0) {
                pushJunction(hi | found, lo, &p);
            }
            waypoint.x = bx;
            waypoint.z = bz;
            pathMode = 0;
            pathDir = found;
        } else {
                if (scanDir(&r, p, dirs, &q, o)) {
                waypoint.x = r.x;
                waypoint.z = r.z;
                pathMode = 1;
                pathDir = dirs;
            }
        }
        stepPhase = 1;
    } else if (pathMode == 1) {
        s32 cand2, j;
        RouteJunction e2 = popJunction(&p);
        lo2 = e2.f.lo;
        w = oppositeDirs(lo2);
        FieldPos_FromUnitCenter(&tv, targetUnit.x, targetUnit.y);
        dirs = dirs | w;
        for (j = 0; j < 4; j++) {
            if (((dirs >> j) & 1) == 0) {
                cand2 = present & (1 << j);
                if (cand2 != 0) {
                    if (scanDir(&r, p, cand2, &q, o)) {
                        s32 d;
                        FieldPos_FromUnitCenter(&uv, r.x, r.z);
                        d = Vec_DistXZ(&tv, &uv);
                        if (bestd < 0 || d < bestd) {
                            bx = r.x;
                            bz = r.z;
                            bestd = d;
                            found = cand2;
                        }
                    }
                }
            }
        }
        if (found != 0) {
            if (lo2 == 0) {
                lo2 = pathDir;
            }
            waypoint.x = bx;
            waypoint.z = bz;
            pushJunction(e2.f.hi | found, lo2, &p);
            pathMode = 0;
            pathDir = found;
        } else {
                if (scanDir(&r, p, w, &q, o)) {
                waypoint.x = r.x;
                waypoint.z = r.z;
                pathMode = 1;
                pathDir = w;
            }
        }
        stepPhase = 1;
    }
    approachDir = 4;
}

BOOL VillagerRoute::followPathDir(VecFx32 *p, Vec2 *lim, BlockMap *w) {
    using namespace nB;
    checkUnitChanged(p);
    if ((u32)approachDir < 4) {
        Unk_021be028_Dir *tbl = sRouteDirs;
        Unk_021be028_Dir *e = &tbl[approachDir];
        s32 dx = tbl[approachDir].x;
        s32 dz = e->z;
        u32 a = 0;
        u32 b = 0;
        FieldPos_ToUnit((s32 *)&a, (s32 *)&b, p);
        for (; a < (u32)lim->x && b < (u32)lim->y;) {
            a += dx;
            b += dz;
            BOOL eq = FALSE;
            if (waypoint.x == a && waypoint.z == b) eq = TRUE;
            if (eq || w->getWalkLinks(a, b)) {
                FieldPos_FromUnitCenter(p, a, b);
                ((VillagerRoute *)(this))->planStep(p);
                break;
            }
            if (TownMap_IsUnitWalkable(a, b, w)) {
                FieldPos_FromUnitCenter(p, a, b);
                break;
            }
        }
    } else {
        ((VillagerRoute *)(this))->clearJunctions();
        pathDir = 0;
        pathMode = 2;
        ((VillagerRoute *)(this))->planStep(p);
    }
    return FALSE;
}

BOOL VillagerRoute::stepFollowPath(VecFx32 *p) {
    using namespace nB;
    BlockMap *world = gSceneBlockMap;
    if (world) {
        Vec2 lim;
        Vec2 *sz = (Vec2 *)&world->unitsX;
        lim = *sz;
        followPathDir(p, &lim, world);
        if (((VillagerRoute *)(this))->isArrived(p)) {
            stepPhase = 2;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerRoute::advanceOnPath(VecFx32 *p, BlockMap *w) {
    using namespace nB;
    checkUnitChanged(p);
    s32 flag = pathDir;
    if (flag != 0) {
        s32 k = ((VillagerRoute *)(this))->firstDir(flag);
        s32 a = 0;
        s32 b = 0;
        FieldPos_ToUnit(&a, &b, p);
        a += sRouteDirs[k].x;
        b += sRouteDirs[k].z;
        if (!w->getWalkLinks(a, b)) {
            FieldPos_FromUnitCenter(p, a, b);
            ((VillagerRoute *)(this))->clearJunctions();
            pathDir = 0;
            pathMode = 2;
            ((VillagerRoute *)(this))->planStep(p);
            approachDir = 4;
            return FALSE;
        }
        FieldPos_FromUnitCenter(p, a, b);
        if (a == waypoint.x && b == waypoint.z) {
            ((VillagerRoute *)(this))->planStep(p);
        }
        return FALSE;
    }
    ((VillagerRoute *)(this))->clearJunctions();
    pathDir = 0;
    pathMode = 2;
    ((VillagerRoute *)(this))->planStep(p);
    return FALSE;
}

BOOL VillagerRoute::stepAlongPath(VecFx32 *p) {
    using namespace nB;
    BlockMap *world = gSceneBlockMap;
    if (world) {
        advanceOnPath(p, world);
        if (((VillagerRoute *)(this))->isArrived(p)) {
            stepPhase = 2;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerRoute::pickAdjacent(VecFx32 *p, BlockMap *w) {
    using namespace nB;
    ((VillagerRoute *)(this))->pickAdjacentUnit(p, w);
    return FALSE;
}

BOOL VillagerRoute::stepAdjacentUnit(VecFx32 *p) {
    using namespace nB;
    BlockMap *world = gSceneBlockMap;
    if (world) {
        pickAdjacent(p, world);
        if (!((VillagerRoute *)(this))->isArrived(p)) {
            ((VillagerRoute *)(this))->clearJunctions();
            pathDir = 0;
            pathMode = 2;
            ((VillagerRoute *)(this))->planStep(p);
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL VillagerRoute::stepWander(VecFx32 *p) {
    using namespace nB;
    u16 mag = 0x7fff;
    s32 mode = Random_GlobalBelow(4);
    s32 base;
    volatile s32 i;
    volatile s32 len;
    volatile s32 zero0;
    volatile s32 zero1;
    VecFx32 tmp;
    FieldPos_FromUnitCenter(&tmp, targetUnit.x, targetUnit.y);
    if (mode == 0) {
        base = Math_AngleXZ(p, &tmp);
        mag = 0x1000;
    } else if (mode == 1) {
        base = (s16)Random_Next(&gRandom);
    } else {
        base = (s16)Random_Next(&gRandom);
        mag >>= 1;
    }
    i = 0;
    zero0 = 0;
    zero1 = 0;
    for (; i < 10; i++) {
        len = (Random_GlobalBelow(5) + 3) << 13;
        s32 r6 = (s16)Random_GlobalBelow(mag);
        if (Random_GlobalBelow(2)) {
            r6 = (s16)(r6 * ~zero1);
        }
        r6 = ((u16)(s16)(r6 + base) >> 4) << 1;
        tmp.x = p->x + func_01ffcb0c(len, data_02135f44[r6]);
        tmp.z = p->z + func_01ffcb0c(len, data_02135f44[r6 + 1]);
        FieldPos_SnapToUnitCenter(&tmp, &tmp);
        if (TownMap_IsPosWalkable(&tmp, zero0)) {
            p->x = tmp.x;
            p->z = tmp.z;
            break;
        }
        if ((i & 1) && mag != 0xffff) {
            mag = (u16)(mag + 0x1000);
        }
    }
    ((VillagerRoute *)(this))->markUnitUnset();
    if (((VillagerRoute *)(this))->isArrived(p)) {
        stepPhase = 2;
        return TRUE;
    }
    return FALSE;
}

namespace nB {
extern "C" void VillagerRoute_PickAttr200Target(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    BlockMap *world = gSceneBlockMap;
    out->x = 0;
    out->y = 0;
    MapBlock *block;
    if (world && (block = BlockMap_FindBlockAllAttr(world, 0x200))) {
        Vec2 t;
        Vec2 blk = {block->blockX, block->blockZ};
        VillagerRoute_PickPathUnitInBlock(&t, self, &blk, world);
        *out = t;
    }
}
}

BOOL VillagerRoute::isInAttr200Block(VecFx32 *v) {
    using namespace nB;
    BlockMap *world = gSceneBlockMap;
    BOOL r = FALSE;
    if (world != NULL) {
        s32 x = v->x >> 17;
        s32 z = v->z >> 17;
        BOOL c = BlockMap_BlockHasAllAttr(world, x, z, 0x200) != NULL;
        if (c) {
            r = TRUE;
        }
    }
    return r;
}

namespace nB {
extern "C" void VillagerRoute_PickOtherHouseDoor(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    out->x = 0;
    out->y = 0;
    if (obj->getVillagerData()) {
        uptr key;  // (a one-entry array of pointers: SaveVillagers_PickRandomExcept)
        func_02133ef8(&key, 4);
        key = (uptr)((VillagerData *)obj->getVillagerData())->getVillagerId();
        void *v;
        if ((v = SaveVillagers_PickRandomExcept(gSaveVillagers, &key, 1)) != 0) {
            u8 *p = ((VillagerDataItemView *)(v))->getHousePos();
            s32 z = p[1] + 1;
            out->x = p[0];
            out->y = z;
        }
    }
}
}

BOOL VillagerRoute::isAtDoorT1(VecFx32 *p) {
    using namespace nB;
    s32 a = 0;
    s32 b = 0;
    FieldPos_ToUnit(&a, &b, p);
    if (a == targetUnit.x && b == targetUnit.y) return TRUE;
    return FALSE;
}

BOOL VillagerRoute::stepCheckArrived(VecFx32 *p) {
    using namespace nB;
    if (!((VillagerRoute *)(this))->isArrived(p)) {
        ((VillagerRoute *)(this))->clearJunctions();
        pathDir = 0;
        pathMode = 2;
        ((VillagerRoute *)(this))->planStep(p);
        return FALSE;
    }
    return TRUE;
}

namespace nB {
extern "C" void VillagerRoute_PickOtherHouseBlock(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    BlockMap *world = gSceneBlockMap;
    out->x = 0;
    out->y = 0;
    if (world && obj->getVillagerData()) {
        uptr key;  // (a one-entry array of pointers: SaveVillagers_PickRandomExcept)
        func_02133ef8(&key, 4);
        key = (uptr)((VillagerData *)obj->getVillagerData())->getVillagerId();
        void *v;
        if ((v = SaveVillagers_PickRandomExcept(gSaveVillagers, &key, 1)) != 0) {
            u8 *p = ((VillagerDataItemView *)(v))->getHousePos();
            Vec2 pos;
            pos.x = 0;
            pos.y = 0;
            s32 y = p[1];
            pos.x = p[0] >> 4;
            pos.y = y >> 4;
            Vec2 t;
            VillagerRoute_PickPathUnitInBlock(&t, self, &pos, world);
            *out = t;
        }
    }
}
}

BOOL VillagerRoute::isInBlockT2(VecFx32 *v) {
    using namespace nB;
    s32 z = v->z >> 17;
    s32 x = v->x >> 17;
    if (x == targetBlockX && z == targetBlockZ) return TRUE;
    return FALSE;
}

namespace nB {
extern "C" void VillagerRoute_PickOwnHouseDoor(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    out->x = 0;
    out->y = 0;
    if (obj->getVillagerData()) {
        u8 *p = ((VillagerDataItemView *)(obj->getVillagerData()))->getHousePos();
        s32 z = p[1] + 1;
        out->x = p[0];
        out->y = z;
    }
}
}

BOOL VillagerRoute::isAtDoorT3(VecFx32 *p) {
    using namespace nB;
    s32 a = 0;
    s32 b = 0;
    FieldPos_ToUnit(&a, &b, p);
    if (a == targetUnit.x && b == targetUnit.y) return TRUE;
    return FALSE;
}

BOOL VillagerRoute::stepToDoor(VecFx32 *p) {
    using namespace nB;
    s32 a = 0;
    s32 b = 0;
    FieldPos_ToUnit(&a, &b, p);
    if (a == targetUnit.x && b >= targetUnit.y && b <= targetUnit.y + 1) {
        FieldPos_FromUnitCenter(p, ((volatile Vec2 &)targetUnit).x, ((volatile Vec2 &)targetUnit).y);
    } else if (a >= targetUnit.x - 3 && a <= targetUnit.x + 4 && b >= targetUnit.y && b <= targetUnit.y + 5) {
        a = targetUnit.x;
        b = targetUnit.y + Random_GlobalBelow(2);
        FieldPos_FromUnitCenter(p, a, b);
    } else {
        stepWander(p);
    }
    ((VillagerRoute *)(this))->markUnitUnset();
    if (((VillagerRoute *)(this))->isArrived(p)) {
        stepPhase = 2;
        return TRUE;
    }
    return FALSE;
}

namespace nB {
extern "C" void VillagerRoute_PickOwnHouseBlock(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    BlockMap *world = gSceneBlockMap;
    out->x = 0;
    out->y = 0;
    if (world && obj->getVillagerData()) {
        u8 *p = ((VillagerDataItemView *)(obj->getVillagerData()))->getHousePos();
        Vec2 pos;
        pos.x = 0;
        pos.y = 0;
        s32 y = p[1];
        pos.x = p[0] >> 4;
        pos.y = y >> 4;
        Vec2 t;
        VillagerRoute_PickPathUnitInBlock(&t, self, &pos, world);
        *out = t;
    }
}
}

BOOL VillagerRoute::isInBlockT4(VecFx32 *v) {
    using namespace nB;
    s32 z = v->z >> 17;
    s32 x = v->x >> 17;
    if (x == targetBlockX && z == targetBlockZ) return TRUE;
    return FALSE;
}

namespace nB {
extern "C" void VillagerRoute_PickRandomBlock(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    BlockMap *world = gSceneBlockMap;
    Vec2 pos;
    out->x = 0;
    out->y = 0;
    if (world) {
        Vec2 *size = (Vec2 *)&world->width;
        Vec2 szcopy;
        szcopy = *size;
        s32 w = szcopy.x;
        s32 h = size->y;
        pos.x = 0;
        pos.y = 0;
        if (w >= 3) {
            pos.x = Random_GlobalBelow(w - 2) + 1;
        }
        if (h >= 3) {
            pos.y = Random_GlobalBelow(h - 2) + 1;
            Vec2 t;
            VillagerRoute_PickPathUnitInBlock(&t, self, &pos, world);
            *out = t;
        }
    }
}
}

BOOL VillagerRoute::isInBlock(VecFx32 *v) {
    using namespace nB;
    s32 z = v->z >> 17;
    s32 x = v->x >> 17;
    if (x == targetBlockX && z == targetBlockZ) return TRUE;
    return FALSE;
}

namespace nB {
extern "C" void VillagerRoute_PickCoastBlock(Vec2 *out, VillagerRoute *self, VecFx32 *v, NpcActor *obj) {
    BlockMap *world = gSceneBlockMap;
    u8 mask = 0;
    out->x = 0;
    out->y = 0;
    if (world) {
        Vec2 *size = (Vec2 *)&world->width;
        Vec2 szcopy;
        s32 count;
        s32 total;
        s32 sz;
        szcopy = *size;
        sz = szcopy.x;
        count = 0;
        total = 0;
        if (size->y > 4 && sz <= 8) {
            s32 px = v->x >> 17;
            s32 pz = v->z >> 17;
            s32 i;
            for (i = 1; i < sz - 1; i++) {
                if (i != px || pz != 4) {
                    MapBlock *c = GetCell(world, i, 4);
                    if (c && MapBlock_HasAnyAttr(c, 8)) {
                        mask |= 1 << (i - 1);
                        count++;
                    }
                }
                total++;
            }
        }
        u32 idx = Random_PickSetBit(mask, count, total);
        if (idx < (u32)total) {
            MapBlock *c = GetCell(world, idx + 1, 4);
            if (c) {
                Vec2 t;
                Vec2 blk = {c->blockX, c->blockZ};
                VillagerRoute_PickPathUnitInBlock(&t, self, &blk, world);
                *out = t;
            }
        }
    }
}
}

HeldToolModel *HeldToolModel::init() {
    using namespace nB;
    heldItem = 0xfff1;
    new (&modelHandle) NpcHeldItemModelHandle();
    heldItem = 0xfff1;
    return this;
}

HeldToolModel *HeldToolModel::destroy() {
    using namespace nB;
    ((NpcHeldItemModelHandle *)(&modelHandle))->NpcHeldItemModelHandle::~NpcHeldItemModelHandle();
    return this;
}

BOOL HeldToolModel::load(u16 *a) {
    using namespace nB;
    if (!((NpcHeldItemModelHandle *)modelHandle)->acquire()) return FALSE;
    if (!((NpcHeldItemModelHandle *)modelHandle)->loadHeldItem(a)) return FALSE;
    heldItem = 0xfff1;
    return TRUE;
}

BOOL HeldToolModel::attach(NpcActor *a, u16 *b, u32 c, u16 d) {
    using namespace nB;
    HeldItemModel *p = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (p) {
        HeldItemModel_SetItem(p, b, 0);
        a->animCtrl.playHoldItemPose(a, b, c, d);
        heldItem = *b;
        return TRUE;
    }
    return FALSE;
}

void HeldToolModel::playAnim(u32 a, u32 b, u32 c) {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) HeldItemModel_PlayAnim(r, a, b, c);
}

void HeldToolModel::playIdleAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) {
        playAnim(0, a, b);
    } else if (Unk_02011c44_InRange(id, 0x1374, 0x1374) || Unk_02011c44_InRange(id, 0x1375, 0x1375)) {
        playAnim(0x13, 0, 0);
    } else if (Unk_02011c44_InRange(id, 0x1380, 0x139f)) {
        playAnim(0x25, 0, 0);
    }
}

void HeldToolModel::playIdleAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playIdleAnimFor(this, a, b);
}

void HeldToolModel::playWalkAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) playAnim(1, a, b);
}

void HeldToolModel::playWalkAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playWalkAnimFor(this, a, b);
}

void HeldToolModel::playPitfallFallAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) playAnim(0xf, a, b);
}

void HeldToolModel::playPitfallFallAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playPitfallFallAnimFor(this, a, b);
}

void HeldToolModel::playPitfallHoleAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) playAnim(0x10, a, b);
}

void HeldToolModel::playPitfallHoleAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playPitfallHoleAnimFor(this, a, b);
}

void HeldToolModel::playPitfallStuckAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) playAnim(0x11, a, b);
}

void HeldToolModel::playPitfallStuckAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playPitfallStuckAnimFor(this, a, b);
}

void HeldToolModel::playPitfallClimbOutAnimFor(HeldToolModel *p, u32 a, u32 b) {
    using namespace nA;
    u16 id = p->heldItem;
    if (Unk_02011c44_InRange(id, 0x1376, 0x1376) || Unk_02011c44_InRange(id, 0x1377, 0x1377)) playAnim(0x12, a, b);
}

void HeldToolModel::playPitfallClimbOutAnim(u32 a, u32 b) {
    using namespace nA;
    if (heldItem != 0xfff1) playPitfallClimbOutAnimFor(this, a, b);
}

void HeldToolModel::release() {
    using namespace nA; ((NpcHeldItemModelHandle *)modelHandle)->release(); }

void HeldToolModel::update(NpcActor *p) {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) {
        u32 v = p->model.frameStep;
        AnimFrameCtrl &s = *(BlendAnimModel *)HeldItemModel_GetModel((u8 *)r);
        s.frameStep = v;
        HeldItemModel_Update(r);
    }
}

void HeldToolModel::draw(NpcActor *p) {
    using namespace nA;
    if (visible != 0) {
        HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
        if (p) {
            Model_GetJointWorldMtx(&p->model, &handMtx, 0xe);
        }
        if (r) HeldItemModel_Draw(r, &handMtx);
    }
}

u32 HeldToolModel::getAnimSpeed() {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) return r->scale;
    return 0;
}

void HeldToolModel::setAnimSpeed(u32 v) {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) r->scale = v;
}

BlendAnimModel *HeldToolModel::getModel() {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) return (BlendAnimModel *)HeldItemModel_GetModel((u8 *)r);
    return 0;
}

void HeldToolModel::setModelAnimSpeed(u32 v) {
    using namespace nA;
    HeldItemModel *r = (HeldItemModel *)((NpcHeldItemModelHandle *)modelHandle)->getHeldItemModel();
    if (r) HeldItemModel_SetAnimSpeed(r, v);
}

inline SndSeEmitterKind1::~SndSeEmitterKind1() {}

inline NpcActor::~NpcActor() {}

namespace nZ {
extern "C" {
VillagerRouteType sVillagerRouteTypes[7] = {
    {&VillagerRoute::isInAttr200Block, VillagerRoute_PickAttr200Target, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepAdjacentUnit}}},
    {&VillagerRoute::isAtDoorT1, VillagerRoute_PickOtherHouseDoor, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepCheckArrived}}},
    {&VillagerRoute::isInBlockT2, VillagerRoute_PickOtherHouseBlock, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepAdjacentUnit}}},
    {&VillagerRoute::isAtDoorT3, VillagerRoute_PickOwnHouseDoor, {{&VillagerRoute::stepToDoor, &VillagerRoute::stepToDoor, &VillagerRoute::stepToDoor}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepCheckArrived}}},
    {&VillagerRoute::isInBlockT4, VillagerRoute_PickOwnHouseBlock, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepAdjacentUnit}}},
    {&VillagerRoute::isInBlock, VillagerRoute_PickRandomBlock, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepAdjacentUnit}}},
    {&VillagerRoute::isInBlock, VillagerRoute_PickCoastBlock, {{&VillagerRoute::stepWander, &VillagerRoute::stepWander, &VillagerRoute::stepWander}, {&VillagerRoute::stepFollowPath, &VillagerRoute::stepAlongPath, &VillagerRoute::stepAdjacentUnit}}},
};
Unk_021be028_Dir sRouteDirs[4] = {Unk_021be028_Dir(0, -1), Unk_021be028_Dir(-1, 0), Unk_021be028_Dir(0, 1), Unk_021be028_Dir(1, 0)};
}
}


// mwcc-flags: -nothumb -O4,p
// G001a part 3: the key-pad file, autoload_2 .text 0x020e8208-0x020e82bc (Pad_Update and an empty function).
// mwcc 1.2/base, C++, ARM, -O4,p. Owns .rodata 0x02135914-0x02135934 and autoload_3 .bss 0x021f47d0-0x021f47e0.
#include "types.h"
#include "host_mem.h"
#ifdef HOST
#include "host_input.h"
#endif
#include "sys/TreeNode.h"
#include "gfx/VecFx32.h"
#include "sys/ListNode.h"
#include "nitro/io_access.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8208.h"
#include "proto/autoload_2/unk_0211bcdc.h"
#include "proto/main/unk_0206d49c.h"

struct Tree {
    TreeNode *root;
};
struct MtxFx43 {
    s32 m[4][3];
};
struct TPData {
    u16 x;
    u16 y;
    u16 touch;
    u16 validity;
};
struct TPCalibrateParam {
    s16 x0, xDotSize, y0, yDotSize;
};
struct PadState {
    u16 cur;
    u16 trig;
    s16 dir;
};

extern "C" {
s32 func_02133150(s32, s32); // _s32_div_f (called by the compiler for the s16 division)
s32 FX_Div(s32, s32); // FX_Div
s32 VEC_Mag(const VecFx32 *v); // VEC_Mag
void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst); // VEC_Add
void MTX_RotZ43_(MtxFx43 *m, s32 sin, s32 cos); // MTX_RotZ43_ (Thumb)
void MTX_RotY43_(MtxFx43 *m, s32 sin, s32 cos); // MTX_RotY43_ (Thumb)
void MTX_RotX43_(MtxFx43 *m, s32 sin, s32 cos); // MTX_RotX43_ (Thumb)
void MTX_Scale43_(MtxFx43 *m, s32 x, s32 y, s32 z); // MTX_Scale43_ (Thumb)
void MTX_Concat43(const MtxFx43 *a, const MtxFx43 *b, MtxFx43 *ab); // MTX_Concat43
void Vec_Sub(VecFx32 *out, const VecFx32 *a, const VecFx32 *b); // out = a - b
s32 Vec_MagXZ(const VecFx32 *v); // length in the XZ plane
void Vec_Scale(VecFx32 *v, s32 s); // scale
u16 TP_GetLatestIndexInAuto(void); // TP_GetLatestIndexInAuto
void TP_GetCalibratedPoint(TPData *dst, const TPData *src); // TP_GetCalibratedPoint
BOOL TP_GetUserInfo(TPCalibrateParam *p); // TP_GetUserInfo
void TP_SetCalibrateParam(const TPCalibrateParam *p); // TP_SetCalibrateParam
void TP_RequestSetStabilityAsync(u32, u32);
void TP_WaitBusy(u32); // TP_WaitBusy
u32 TP_CheckError(u32); // TP_CheckError
void TP_RequestAutoSamplingStartAsync(u32, u32, TPData *, u32); // TP_RequestAutoSamplingStartAsync

extern u16 data_0213a748[]; // atan table (.data of autoload_2)
extern const s16 kPadDirAngleTable[]; // direction (angle) by D-pad bits, const s16[16] at the start of .rodata
extern const s16 data_02135f44[]; // FX_SinCosTable_
extern s32 gFrameCounter;
extern s32 data_021f476c;
extern TPData sTouchPoint;
extern TPData sTouchSampleBuf[9]; // auto-sampling buffer
extern PadState gPad;

BOOL List_PushFront(List *list, ListNode *node);
void TreeNode_Init(TreeNode *n);
u32 Random_Next(u32 *seed);
void Mtx43_SetRotZ(MtxFx43 *m, s32 angle);
void Mtx43_SetRotY(MtxFx43 *m, s32 angle);
void Mtx43_SetRotX(MtxFx43 *m, s32 angle);
void Mtx43_SetTranslate(MtxFx43 *m, s32 x, s32 y, s32 z);
}

// FX_Mul of the SDK
static inline s32 FX_Mul(s32 a, s32 b) {
    return (s32)(((s64)a * b + 0x800) >> 12);
}

// PAD_Read / PAD_DetectFold of the SDK (REG_KEYINPUT 0x04000130, shared work X/Y/fold word 0x027fffa8)
static inline u16 PAD_Read(void) {
    return (u16)(((IO_R((vu16 *)0x04000130) | *HW_SYSWORK(vu16, buttonXY)) ^ 0x2fff) & 0x2fff);
}
static inline BOOL PAD_DetectFold(void) {
    return (*HW_SYSWORK(vu16, buttonXY) & 0x8000) >> 15;
}

#define FX_SinIdx(a) data_02135f44[((a) >> 4) * 2]
#define FX_CosIdx(a) data_02135f44[((a) >> 4) * 2 + 1]
extern "C" void Gfx_InitNop(void) {
}

extern "C" void Pad_Update(void) {
    u32 keys;

    if (PAD_DetectFold()) {
        keys = 0;
    } else {
        keys = PAD_Read();
    }
    data_021f47d0 = 0;
    gPad.trig = keys & (keys ^ sPadPrevHeld);
    sPadPrevHeld = keys;
    gPad.cur = keys;
    gPad.dir = kPadDirAngleTable[(keys & 0xf0) >> 4];
#ifdef HOST
    if (gHostStickTilt) gPad.dir = gHostStickAngle;  // (the 3DS's circle pad: any angle, include/host_input.h)
#endif
}

// ---- file-scope objects (defined after their users)
const s16 kPadDirAngleTable[16] = {
    0, 16384, -16384, 16384, -32768, 24576, -24576, 24576, 0, 8192, -8192, 8192, -32768, 24576, -24576, 24576,
};
u8 data_021f47d0;
u16 sPadPrevHeld; // keys of the previous frame
PadState gPad;

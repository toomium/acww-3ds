#include "types.h"
#include "text/Unk_02050288.h"
#include "Unk_020d8c7c.h"
#include "sys/StackPad.h"
#include "talk/MsgStringBase.h"
#include "talk/MsgStringAttr.h"
#include "talk/MsgTag.h"
#include "talk/MsgRunner.h"
#include "talk/BmgReader.h"
#include "talk/BmgMsgAttr.h"
#include "talk/MsgParser.h"
#include "talk/EncodedStringBase.h"
#include "talk/Flag18.h"
#include "talk/MsgTextLabel.h"
#include "talk/MsgString.h"
#include "talk/MsgRequest.h"
#include "talk/MsgProcessor.h"
#include "talk/MsgWalker.h"
#include "talk/EncodedString.h"
#include "talk/MsgString33.h"
#include "talk/MsgRenderProcessor.h"
#include "talk/MsgQuery.h"
#include "talk/MsgCopyProcessor.h"
#include "talk/MsgUiProc.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02119434.h"
#include "proto/autoload_2/unk_0212a120.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020029e8.h"
#include "proto/main/unk_020116e0.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02065e88.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_0208eeac.h"
#include "proto/main/unk_020a8ba0.h"
#include "proto/main/unk_020b7190.h"
#include "proto/main/unk_020b7d84.h"
#include "n3ds_ui.h"


extern "C" {
// Other files
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
void NNS_FndRemoveListObject(void *list, void *obj);
}

extern "C" {
void NNS_FndAppendListObject(void *list, void *obj);
}

extern "C" {
void MI_CpuFill8(void *dst, u32 value, u32 size);
}

extern "C" {
void MI_CpuCopy8(const void *src, void *dst, u32 size);
}

extern "C" {
void FS_CloseFile(void *file);
}

extern "C" {
}

extern "C" {
BOOL FS_OpenFile(void *file, const char *path);
}

extern "C" {
}

extern "C" {
}

extern "C" {
char *strncpy(char *dst, const char *src, u32 n);
}

extern "C" {
}

extern "C" {
u32 strlen(const char *s);
}

extern "C" {
// Script stack helpers (another file)
void func_020a8c9c(void *list);
}

extern "C" {
void func_020a8cb0(void *list);
}

extern "C" {
BOOL func_020a8cb4(void *list);
}

extern "C" {
void func_020a8cc4(void *list, u8 **p);
}

extern "C" {
u8 **func_020a8cd8(void *list);
}

extern "C" {
void func_020a8ce4(void *list);
}

extern "C" {
// This file
BOOL Msg_EncodeGameChar(u8 *out, const u8 *src);
}

extern "C" {
BOOL Msg_DecodeGameChar(char *out, u8 c);
}

extern "C" {
u32 Msg_GetCharAt(u32 a, u32 b);
}

extern "C" {
BOOL Input_IsTouchBlocked(void);
}

extern "C" {
BOOL Input_IsKeyBlocked(void);
}

extern "C" {
void Input_ResetMode(void);
}

extern "C" {
u16 Msg_ReadU16(u8 *p, u32 i);
}

extern "C" {
u8 Msg_ReadU8(u8 *p, u32 i);
}

extern "C" {
u8 Bmg_ReadU8(void *p);
}

extern "C" {
u16 Bmg_ReadU16(void *p);
}

extern "C" {
u32 Bmg_ReadU32(void *p);
}

extern "C" {
u32 Bmg_ReadMagic(void *p);
}

extern "C" {
void MsgUiProc_HideObjPlane(void);
}

extern "C" {
void MsgUiProc_ShowObjPlane(void);
}

extern "C" {
}

extern "C" {
}

extern "C" {
extern u32 sBmgMsgAttrTableA[];
}

extern "C" {
extern u32 sBmgMsgAttrTableB[];
}

extern "C" {
extern u32 sBmgMsgAttrTableC[];
}

extern "C" {
extern u8 sColorTags[][7];
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
extern u16 gPad[2];
}

extern "C" {
extern u8 sInputButtonMode;
}

extern "C" {
extern u8 gU8None;
}

extern "C" {
extern u8 sInputLocked;
}

extern "C" {
extern u8 data_021edd14;
}

class MsgString;














class MsgProcessor;













extern "C" {
extern BmgInfHeader sBmgInfHeader;
}

extern "C" {
extern BmgDatHeader sBmgDatHeader;
}

extern "C" {
extern BmgFileHeader sBmgFileHeader;
}

extern MsgProcessor gMsgRenderProcessor;
extern MsgCopyProcessor gMsgCopyProcessor;
extern MsgQuery gMsgQuery;
extern Flag18 data_021edcfc;
// prototypes (test harness)


// mwcc 1.2 emits functions in reverse order, so they are defined here from highest to lowest address

// ---- MsgUiProc
extern "C" MsgUiProc *MsgUiProc_Create(void) {
    return new MsgUiProc;
}

MsgUiProc::MsgUiProc() {}

MsgUiProc::~MsgUiProc() {}

BOOL MsgUiProc::onCreate() {
    InputModeIcon_Init();
    Input_ResetMode();
    AbAllObjGfx_Upload();
    HudObjGfx_LoadForScene();
    HudObjGfx_SetMsgUiActive();
    TalkWindow_CreateAll();
    FieldInfoBalloon_Init();
    MsgUiProc_ShowObjPlane();
    return TRUE;
}

BOOL MsgUiProc::onDelete() {
    MsgUiProc_HideObjPlane();
    FieldInfoBalloon_Release();
    TalkWindow_DestroyAll();
    HudObjGfx_ClearMsgUiActive();
    InputModeIcon_Exit();
    return TRUE;
}

BOOL MsgUiProc::onExecute() {
    InputModeIcon_Update();
    TalkWindow_UpdateAll();
    FieldInfoBalloon_Update();
    return TRUE;
}

BOOL MsgUiProc::onDraw() {
    FieldInfoBalloon_Draw();
    TalkWindow_DrawAll();
#ifdef N3DS_DIRECT  // (n3ds_ui.h: to the wide screen's edges it is near; mode 3: the screen that isn't on top)
    int n = Oam_Count(3);
    InputModeIcon_Draw();
    Oam_Anchor(3, n, N3DS_UI_NEAR(7));
#else
    InputModeIcon_Draw();
#endif
    return TRUE;
}


// ---- MsgTag

// ---- BmgMsgAttr

extern "C" {
void BmgMsgAttr_Clear(BmgMsgAttr *s);
}

// ---- Touch input
static inline BOOL IsTouching() {
    return gTouchHeld != 0 && gTouchChanged != 0;
}

// ---- MsgQuery
// MsgTag::MsgTag(), which func_020a6a0c calls to re-construct its token in place. A placement new adds a
// null check, and C++ has no other way to call a constructor on an existing object.
extern "C" void func_020a776c(MsgTag *token);

extern "C" {
u32 Msg_CountChars(u32 a);
}


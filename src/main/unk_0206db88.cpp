#include "uptr.h"
#include "gfx/HBlankTask.h"
// mwcc-flags: -str reuse
#include "types.h"
#include "gfx/BgVramTask.h"
#include "nitro/io_access.h"
#include "n3ds_ui.h"
#include "player/PlayerData.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/dtcm/unk_027e0434.h"
#include "proto/itcm/unk_01ffcc60.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_020116e0.h"
#include "proto/main/unk_0203c638.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_0206d988.h"
#include "proto/main/unk_0205b69c.h"
#include "proto/main/unk_0206db88.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_02097444.h"


extern "C" {
extern u8 gSaveTownTune[];
extern s32 gGfxMainOnTop;
extern u8 gFieldSceneKind;

s64 func_02133540(u32, u32, u32);
BOOL Camera_RestorePrevMode();
void Sky_SetEngine(u32);
s32 func_01ffcb0c(s32, s32);
BOOL Camera_IsViewPushed();
void Camera_PopView();
void Sky_Disable();
BOOL Camera_SetMode1();

}

// 4-byte bss slots accessed as u8/u16/s16
union Slot8 { u8 v; u32 pad; };
union Slot16 { u16 v; u32 pad; };
union SlotS16 { s16 v; u32 pad; };

Slot8 sMenuScreenFlags;
s32 sMenuScreenProgress;
s32 sMenuScreenProgressStep;
Slot16 sMenuScreenState;
SlotS16 sMenuWipePos;
Slot16 sMenuScreenScroll;
Slot8 sMenuScreenWipeFlags;
Slot8 sMenuScreenBgKind;
HBlankTask sMenuWipeHBlankTask;
#ifdef N3DS_DIRECT  // (the host draws the wipe's zigzag, gpu2d_menu_wipe: no window changed every line, the V-blank's part kept)
#define MENU_WIPE_HBLANK NULL
#else
#define MENU_WIPE_HBLANK MenuScreen_WipeHBlank
#endif

BOOL (*sMenuScreenSteps[])() = {
    MenuScreen_StepAct00, MenuScreen_StepAct01, MenuScreen_StepAct02, MenuScreen_StepAct03, MenuScreen_StepAct04,
    MenuScreen_StepAct05, MenuScreen_StepAct06, MenuScreen_StepNop, MenuScreen_StepAct08, MenuScreen_StepAct09,
    MenuScreen_StepAct0A, MenuScreen_StepAct0B, MenuScreen_StepNop,
};

static inline BOOL Unk_0206dc9c_IsZero(u8 v) { return v == 0 ? TRUE : FALSE; }

extern "C" void MenuScreen_WipeInVBlank(void) {
    s32 i, b, t;
    if (sMenuWipePos.v > 20) {
        sMenuWipePos.v -= 20;
    } else {
        HBlank_Remove(&sMenuWipeHBlankTask);
        sMenuScreenWipeFlags.v &= ~1;
        Gfx2d_SetMainWin1Rect(0, 0, 255, 192);
        IO_W((volatile u16 *)0x4000042, 0xff);
        sMenuWipePos.v = 0;
    }
    i = 0;
    b = sMenuWipePos.v;
    for (; i < 24; i++) {
        t = b - i;
        if (t < 0) {
            sMenuWipeEdge[i] = 0;
        } else if (t > 0xfe) {
            sMenuWipeEdge[i] = 0xfe;
        } else {
            sMenuWipeEdge[i] = t;
        }
    }
    IO_W((volatile u16 *)0x4000042, ((sMenuWipeEdge[0] << 8) & 0xff00) | 0xff);
#ifdef N3DS_DIRECT
    gpu2d_menu_wipe(sMenuWipePos.v);
#endif
}

extern "C" void MenuScreen_WipeOutVBlank(void) {
    s32 i, b, t;
    if (sMenuWipePos.v < 0x103) {
        sMenuWipePos.v += 20;
    } else {
        HBlank_Remove(&sMenuWipeHBlankTask);
        sMenuScreenWipeFlags.v &= ~1;
        sMenuWipePos.v = 0xfe;
        IO_W((volatile u16 *)0x4000042, 0xfeff);
    }
    i = 0;
    b = sMenuWipePos.v;
    for (; i < 24; i++) {
        t = b - i;
        if (t < 0) {
            sMenuWipeEdge[i] = 0;
        } else if (t > 0xfe) {
            sMenuWipeEdge[i] = 0xfe;
        } else {
            sMenuWipeEdge[i] = t;
        }
    }
#ifdef N3DS_DIRECT
    gpu2d_menu_wipe(sMenuWipePos.v);
#endif
}

extern "C" void MenuScreen_StopWipe(void) {
    if (sMenuScreenWipeFlags.v & 1) {
        HBlank_Remove(&sMenuWipeHBlankTask);
        sMenuScreenWipeFlags.v &= ~1;
    }
}

extern "C" void MenuScreen_StartWipeIn(void) {
    s32 i;
    sMenuWipePos.v = 0x117;
    if (HBlank_Add(&sMenuWipeHBlankTask, MENU_WIPE_HBLANK, MenuScreen_WipeInVBlank, 0)) {
        sMenuScreenWipeFlags.v |= 1;
    }
    for (i = 0; i < 24; i++) {
        sMenuWipeEdge[i] = 0xfe;
    }
    Gfx2d_SetMainWin1Rect(sMenuWipePos.v, 0, 255, 192);
    sMenuWipeLine = 0;
}

extern "C" void MenuScreen_StartWipeOut(void) {
    s32 i;
    sMenuWipePos.v = 0;
    if (HBlank_Add(&sMenuWipeHBlankTask, MENU_WIPE_HBLANK, MenuScreen_WipeOutVBlank, 0)) {
        sMenuScreenWipeFlags.v |= 1;
    }
    for (i = 0; i < 24; i++) {
        sMenuWipeEdge[i] = 0;
    }
    Gfx2d_SetMainWin1Rect(1, 0, 255, 192);
    sMenuWipeLine = 0;
}

extern "C" BOOL MenuScreen_IsWiping(void) {
    if (sMenuScreenWipeFlags.v & 1) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL MenuScreen_IsOpen(void) {
    if ((u16)(sMenuScreenState.v + 0xfffb) <= 2) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL MenuScreen_IsClosed(void) {
    if (sMenuScreenState.v == 12) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void MenuScreen_UploadClothPattern(u16 *p, BgVramTaskPair *x, u8 *img, u16 *pal) {
    PlayerData *r = PlayerData_GetCurrent();
    BOOL in1 = FALSE;
    u32 v = *p;
    if (v >= 0x11a8 && v <= 0x12a7) in1 = TRUE;
    if (in1 || (v >= 0x12a8 && v <= 0x12af)) {
        Heap *heap = gCurrentHeap;
        void *o = Heap_AllocTail(heap, 0x2c4);
        if (o != NULL) {
            ClothTex_LoadItem(o, p, r);
            Gfx_LightenPalette16((u16 *)ClothTex_GetPlttData(o), pal);
            Gfx2d_LinearToTiles4bppBytes((u8 *)ClothTex_GetTexData(o), img, 4, 4);
            Heap_Free(heap, o);
            if (x->requestCharsAndPalette(img, 5, 0, 0, 0xf, pal, 0) != 0) {
                r->setInventoryBackground(p);
            }
        }
    }
}

extern "C" void MenuScreen_SetupMainBg() {
    Gfx2d_SetLayerPriority(1, 1);
    Gfx2d_SetLayerControl(1, 0, 0, 0);
}

extern "C" void MenuScreen_SetupSubBg() {
    Gfx2d_SetLayerPriority(5, 3);
    Gfx2d_SetLayerControl(5, 0, 0, 0);
}

extern "C" BOOL MenuScreen_LoadStdBackground(u32 arg) {
    char buf[0x24];
    Heap *heap = gCurrentHeap;
    if (!Gfx2d_LoadScreenFile("menu/inventory/b_itm_back.bsc", heap, arg)) {
        return FALSE;
    }
    Str_SPrintf(buf, "menu/bas/b_bas_%d.bpl", sMenuScreenBgKind.v);
    if (!Gfx2d_LoadPaletteFile(buf, heap, arg, 0, 0, 0)) {
        return FALSE;
    }
    Str_SPrintf(buf, "menu/bas/b_bas_%d.bch", sMenuScreenBgKind.v);
    Gfx2d_LoadCharFile(buf, heap, arg, 0, 0, 0xf);
    return TRUE;
}

extern "C" BOOL MenuScreen_LoadBackground(u32 arg) {
    if (sMenuScreenBgKind.v != 4) {
        return MenuScreen_LoadStdBackground(arg);
    }
    BOOL ok;
    Heap *heap = gCurrentHeap;
    PlayerData *r = PlayerData_GetCurrent();
    u16 tmp = *(u16 *)r->getInventoryBackground();
    u8 *a = (u8 *)Heap_AllocTail(heap, 0x200);
    if (a == NULL) {
        return FALSE;
    }
    u8 *b = (u8 *)Heap_AllocTail(heap, 0x20);
    if (b == NULL) {
        Heap_Free(heap, a);
        return FALSE;
    }
    void *c = Heap_AllocTail(heap, 0x2c4);
    if (c == NULL) {
        Heap_Free(heap, a);
        Heap_Free(heap, b);
        return FALSE;
    }
    ClothTex_LoadItem(c, &tmp, r);
    Gfx_LightenPalette16((u16 *)ClothTex_GetPlttData(c), (u16 *)b);
    ok = Gfx2d_LoadPaletteRange(b, arg, 0, 0, 0);
    Gfx2d_LinearToTiles4bppBytes((u8 *)ClothTex_GetTexData(c), a, 4, 4);
    ok &= Gfx2d_LoadCharRange(a, arg, 0, 0, 0xf);
    Heap_Free(heap, a);
    Heap_Free(heap, b);
    Heap_Free(heap, c);
    ok &= Gfx2d_LoadScreenFile("menu/inventory/b_itm_back.bsc", heap, arg);
    return ok;
}

extern "C" void MenuScreen_BeginClose() {
    sMenuScreenState.v = 8;
    MenuCtrl_SetScreenChanging();
}

extern "C" void MenuScreen_BeginOpen() {
    MenuScreen_SetupMainBg();
    sMenuScreenState.v = 1;
    sMenuScreenBgKind.v = 4;
    MenuCtrl_SetScreenChanging();
}

extern "C" void MenuScreen_ReleaseCloseHold() { MenuScreen_ClearFlags(1); }

extern "C" void MenuScreen_ClearState() {
    sMenuScreenScroll.v = 0;
    sMenuScreenState.v = 0;
    sMenuWipePos.v = 0;
}

extern "C" void MenuScreen_Reset() {
    if (!Stub_ReturnFalse()) {
        Gfx2d_ResetLayer(1);
        Gfx2d_ResetLayer(5);
    }
    Gfx2d_DisableMainWindows(2);
    gGfxMainOnTop = 0;
    MenuCtrl_ClearScreenChanging();
    MenuScreen_ClearState();
    MenuScreen_StopWipe();
}// Declarations for data defined further down (definition order sets the data layout)




extern "C" void MenuScreen_Update() {
    if (sMenuScreenSteps[sMenuScreenState.v]()) {
        sMenuScreenScroll.v = (sMenuScreenScroll.v + 1) & 0x1ff;
        u16 s = sMenuScreenState.v;
        if (s == 2 || s == 4 || (u16)(s + 0xfff6) <= 1) {
            Gfx2d_SetLayerOffset(1, sMenuScreenScroll.v, sMenuScreenScroll.v);
        } else {
            Gfx2d_SetLayerOffset(5, sMenuScreenScroll.v, sMenuScreenScroll.v);
        }
    }
}

extern "C" BOOL MenuScreen_StepAct00() { return FALSE; }

extern "C" BOOL MenuScreen_StepNop() { return TRUE; }

extern "C" BOOL MenuScreen_StepAct01() {
    if (MenuScreen_LoadBackground(1)) {
        sMenuScreenState.v = 2;
        Gfx2d_ShowLayer(1);
        Gfx2d_EnableMainWindows(2);
        Gfx2d_SetMainWin1Planes(0x1f);
        Gfx2d_SetMainWinOutPlanes(0x1b);
        MenuScreen_StartWipeIn();
        return MenuScreen_StepAct02();
    }
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct02() {
    if (!MenuScreen_IsWiping()) {
        Gfx2d_SetMainWin1Rect(1, 0, 0xff, 0xc0);
        sMenuScreenState.v = 3;
        MenuScreen_SetupSubBg();
    }
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct03() {
    if (Camera_SetMode1()) {
        MenuCtrl_SetTransitionActive();
        MenuCtrl_SetTransitionProgress(0);
        sMenuScreenProgress = 0x1000;
        sMenuScreenProgressStep = 0x200;
        sMenuScreenState.v = 4;
    }
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct04() {
    if (MenuScreen_LoadBackground(5)) {
        Gfx2d_DisableMainWindows(2);
        Gfx2d_HideSubPlanes(0xf);
        Gfx2d_ShowLayer(5);
        gGfxMainOnTop = 1;
        MenuCtrl_ClearScreenChanging();
        MenuCtrl_SetMenuOnTop();
        Gfx2d_ResetLayer(1);
        sMenuScreenState.v = 5;
        if (Unk_0206dc9c_IsZero(gFieldSceneKind)) {
            Sky_SetEngine(1);
        }
    }
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct05() {
    if (sMenuScreenProgress == 0) {
        sMenuScreenState.v = 6;
        if (Unk_0206dc9c_IsZero(gFieldSceneKind)) {
            Sky_Disable();
        }
        Gfx2d_SetMainBgModeState(0);
        return TRUE;
    }
    if (sMenuScreenProgress > sMenuScreenProgressStep) {
        sMenuScreenProgress -= sMenuScreenProgressStep;
    } else {
        sMenuScreenProgress = 0;
    }
    MenuCtrl_SetTransitionProgress(0x1000 - func_01ffcb0c(sMenuScreenProgress, sMenuScreenProgress));
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct06() {
    Gfx2d_DisableMainWindows(1);
    Gfx2d_DisableMainWindows(2);
    Gfx2d_HideMainPlanes(0xe);
    sMenuScreenState.v = 7;
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct08() {
    if (Camera_IsViewPushed()) {
        Camera_PopView();
    }
    if (Unk_0206dc9c_IsZero(gFieldSceneKind)) {
        ScreenLayers_AcquireStub();
        Sky_SetEngine(1);
        Gfx2d_HideMainPlanes(8);
    }
    sMenuScreenState.v = 9;
    sMenuScreenProgress = 0x1000;
    sMenuScreenProgressStep = 0x200;
    MenuScreen_SetFlags(1);
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct09() {
    if (sMenuScreenProgress == 0) {
        if (!MenuScreen_HasFlags(1)) {
            Gfx2d_SetLayerPriority(5, 1);
            MenuScreen_LoadBackground(1);
            MenuCtrl_ClearTransitionActive();
            sMenuScreenState.v = 10;
            gGfxMainOnTop = 0;
            Gfx2d_HideMainPlanes(0xe);
            Gfx2d_ShowLayer(1);
            Gfx2d_ResetLayer(5);
            HudObjGfx_LoadForSceneSub();
            if (Unk_0206dc9c_IsZero(gFieldSceneKind)) {
                Sky_SetEngine(0);
            }
            MenuCtrl_ClearMenuOnTop();
            ScreenLayers_ReleaseStub();
        }
    }
    if (sMenuScreenProgress > sMenuScreenProgressStep) {
        sMenuScreenProgress -= sMenuScreenProgressStep;
    } else {
        sMenuScreenProgress = 0;
    }
    MenuCtrl_SetTransitionProgress(func_01ffcb0c(sMenuScreenProgress, sMenuScreenProgress));
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct0A() {
    Gfx2d_EnableMainWindows(2);
    Gfx2d_DisableMainWindows(1);
    Gfx2d_SetMainWin1Planes(0x1f);
    Gfx2d_SetMainWinOutPlanes(0x1b);
    Gfx2d_HideMainPlanes(10);
    MenuScreen_StartWipeOut();
    if (Camera_RestorePrevMode()) {
        sMenuScreenState.v = 0xb;
    }
    return TRUE;
}

extern "C" BOOL MenuScreen_StepAct0B() {
    if (!MenuScreen_IsWiping()) {
        sMenuScreenState.v = 0xc;
        Gfx2d_DisableMainWindows(2);
        Gfx2d_ResetLayer(1);
        MenuCtrl_ClearScreenChanging();
    }
    return TRUE;
}

extern "C" void Gfx_LightenPalette16(u16 *src, u16 *dst) {
    s32 i;
    src[0] = 0;
    for (i = 1; i < 16; i++) {
        u16 v = src[i];
        u16 r = v & 0x1f;
        u16 g = (v & 0x3e0) >> 5;
        u16 b = (v & 0x7c00) >> 10;
        r = (r + 0x1f) >> 1;
        g = (g + 0x1f) >> 1;
        b = (b + 0x1f) >> 1;
        dst[i] = r | (g << 5) | (b << 10);
    }
}

extern "C" void MenuScreen_SetFlags(u32 a) { sMenuScreenFlags.v |= a; }

extern "C" void MenuScreen_ClearFlags(u32 a) { sMenuScreenFlags.v &= ~a; }

extern "C" BOOL MenuScreen_HasFlags(u32 a) {
    if (a == (a & sMenuScreenFlags.v)) {
        return TRUE;
    }
    return FALSE;
}

extern "C" void MenuScreen_SetBackgroundKind(u32 a) { sMenuScreenBgKind.v = a; }







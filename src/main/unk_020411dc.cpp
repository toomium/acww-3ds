#include "types.h"
#include "host_mem.h"
#include "gfx/HBlankTask.h"
#include "gfx/ScreenTransition.h"
#include "snd/BgmManager.h"
#include "nitro/io_access.h"
#include "proto/autoload_2/unk_020e7500.h"
#include "proto/autoload_2/unk_021104ac.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0203cbc0.h"
#include "proto/main/unk_02037734.h"
#include "proto/main/unk_020407fc.h"
#include "proto/main/unk_020411dc.h"
#include "proto/main/unk_0205b69c.h"
#include "proto/main/unk_0208e6d8.h"

// A transition kind's handlers: begin, end, update (per frame), at VBlank
struct ScreenTransitionType {
    void (*handlers[4])();
};

extern const ScreenTransitionType sTransitionTypeTable[4];
const ScreenTransitionType sTransitionTypeTable[4] = {
    { { ScreenFade_Begin, ScreenFade_End, ScreenFade_Update, ScreenFade_VBlank } },
    { { ScreenFade_Begin, ScreenFade_End, ScreenFade_Update, ScreenFade_VBlank } },
    { { IrisWipe_Begin, IrisWipe_End, IrisWipe_Update, IrisWipe_SwapTables } },
    { { CutTransition_Begin, CutTransition_End, CutTransition_Update, CutTransition_VBlank } },
};

u8 sIrisWipeFlags;
u8 data_021c3cb8;
u16 *sIrisWipeActiveTable;
ScreenTransition gScreenTransition;
HBlankTask sIrisWipeHBlankTask;
u16 sIrisWipeTableA[0x60];
u16 sIrisWipeTableB[0x60];

extern BgmManager *data_021c1b3c;

#ifdef N3DS_DIRECT
// The enhanced port draws the iris itself, over the whole wide screens: the progress
// given at V-blank for the next frame (0 open, 0x1000 closed; -1 none). Window 0 changed every line is left out: it
// made the 2D a piece a line (slow) and left the margins black.
extern "C" void gpu2d_iris(int progress);  // (modern/host/gpu2d)
#endif

extern "C" {
void MIi_CpuClear16(u16 v, void *dst, u32 size);
s32 FX_Div(s32, s32);
s32 FX_Sqrt(s32);
}
static inline void Unk_02041104_Fill(u16 v, void *dst, u32 size) {
    volatile u16 t = v;
    MIi_CpuClear16(t, dst, size);
}

extern "C" void ScreenFade_Begin() {
    ScreenTransition *s = &gScreenTransition;
    if (s->state == 3) {
        s->brightness = 0;
    } else if (s->type == 0) {
        s->brightness = -16;
    } else {
        s->brightness = 16;
    }
    Gfx2d_SetBrightness(s->brightness);
}

extern "C" void ScreenFade_End() {}

extern "C" void ScreenFade_Update() {
    ScreenTransition *s = &gScreenTransition;
    if (s->type == 0) {
        s->brightness = -(s->progress << 4) >> 12;
    } else {
        s->brightness = (s->progress << 4) >> 12;
    }
}

extern "C" void ScreenFade_VBlank() {
    Gfx2d_SetBrightness(gScreenTransition.brightness);
}

extern "C" void IrisWipe_HBlank() {
    volatile u16 *r = (volatile u16 *)0x4000000;
    u32 line = IO_R(&r[3]);
    if ((s32)line < 0xc0) {
        if ((s32)line >= 0x60) line = 0xbf - line;
        u16 v = sIrisWipeActiveTable[line];
        if (IO_R((volatile u16 *)0x4000004) & 2) {
            IO_W((volatile u16 *)((u8 *)r + 0x40), v);
            IO_W((volatile u16 *)((u8 *)r + 0x1040), v);
        }
    }
}

extern "C" void IrisWipe_VBlankRegs() {
    IO_W((volatile u16 *)0x4000040, *sIrisWipeActiveTable);
    IO_W((volatile u16 *)0x4001040, *sIrisWipeActiveTable);
    IO_W((volatile u16 *)0x4000044, 0xc0);
    IO_W((volatile u16 *)0x4001044, 0xc0);
}

extern "C" void IrisWipe_SetupLayers() {
    volatile u16 *r;
    ScreenLayers_AcquireStub();
    Gfx2d_EnableMainWindows(1);
    Gfx2d_EnableSubWindows(1);
    Gfx2d_SetMainWin0Planes(0x1b);
    Gfx2d_SetSubWin0Planes(0x1b, 1);
    Gfx2d_SetMainWinOutPlanes(4);
    Gfx2d_SetSubWinOutPlanes(4);
    if (gScreenTransition.progress == 0) {
        Gfx2d_SetMainWin0Rect(0, 0, 0xff, 0xc0);
        Gfx2d_SetSubWin0Rect(0, 0, 0xff, 0xc0);
    } else {
        Gfx2d_SetMainWin0Rect(0, 0, 0, 0);
        Gfx2d_SetSubWin0Rect(0, 0, 0, 0);
    }
    r = (volatile u16 *)0x400000c;
    IO_AND(r, ~3);
    IO_W(r, (IO_R(r) & 0x43) | 0x600);
    Unk_02041104_Fill(0, G2_GetBG2ScrPtr(), 0x800);
    Unk_02041104_Fill(0x1111, G2_GetBG2CharPtr(), 0x20);
    Unk_02041104_Fill(0x8000, (void *)HW_PLTT, 4);  // the main screen's backdrop colour
    Gfx2d_ShowMainPlanes(4);
    r = (volatile u16 *)0x400100c;
    IO_AND(r, ~3);
    IO_W(r, (IO_R(r) & 0x43) | 0xe04);
    Unk_02041104_Fill(0, G2S_GetBG2ScrPtr(), 0x800);
    Unk_02041104_Fill(0x1111, G2S_GetBG2CharPtr(), 0x20);
    Unk_02041104_Fill(0x8000, (void *)(HW_PLTT + 0x400), 4);  // the sub screen's
    Gfx2d_ShowSubPlanes(4);
#ifdef N3DS_DIRECT  // (no window; the black BG2 hidden, as inside the window)
    Gfx2d_DisableMainWindows(1);
    Gfx2d_DisableSubWindows(1);
    Gfx2d_HideMainPlanes(4);
    Gfx2d_HideSubPlanes(4);
#endif
}

extern "C" void IrisWipe_Begin() {
    volatile u16 z = 0;
    // the two tables are cleared together (they are adjacent in the original)
    MIi_CpuClear16(z, sIrisWipeTableA, sizeof sIrisWipeTableA);
    MIi_CpuClear16(z, sIrisWipeTableB, sizeof sIrisWipeTableB);
    sIrisWipeActiveTable = sIrisWipeTableA;
    IrisWipe_SetupLayers();
#ifndef N3DS_DIRECT
    if (HBlank_Add(&sIrisWipeHBlankTask, IrisWipe_HBlank, IrisWipe_VBlankRegs, NULL) != 0) {
        sIrisWipeFlags |= 1;
    }
#endif
    if (gScreenTransition.progress == 0x1000) Gfx2d_SetBrightness(0);
}

extern "C" void IrisWipe_End() {
#ifdef N3DS_DIRECT
    gpu2d_iris(-1);
#endif
    Gfx2d_DisableMainWindows(1);
    Gfx2d_DisableSubWindows(1);
    if (sIrisWipeFlags & 1) {
        HBlank_Remove(&sIrisWipeHBlankTask);
        sIrisWipeFlags &= ~1;
    }
    Gfx2d_HideMainPlanes(4);
    Gfx2d_HideSubPlanes(4);
    ScreenLayers_ReleaseStub();
}

extern "C" void IrisWipe_Update() {
    u16 *p;
    u16 t;
    ScreenTransition *s = &gScreenTransition;
    if (sIrisWipeFlags & 2) p = sIrisWipeTableA; else p = sIrisWipeTableB;
    s32 v = s->progress;
    if (v == 0) {
        volatile u16 c = 0xff;
        MIi_CpuClear16(c, p, 0xc0);
    } else if (v == 0x1000) {
        volatile u16 c = 0x8080;
    // the two tables are cleared together (they are adjacent in the original)
    MIi_CpuClear16(c, sIrisWipeTableA, sizeof sIrisWipeTableA);
    MIi_CpuClear16(c, sIrisWipeTableB, sizeof sIrisWipeTableB);
    } else {
        s32 h = ((0x1000 - v) * 160) >> 12;
        s32 h2 = (h * h) << 12;
        for (s32 i = 0; i < 0x60; p++, i++) {
            s32 d = 0x60 - i;
            if (d > h) {
                *p = 0x8080;
            } else {
                s32 r = FX_Sqrt(h2 - ((d * d) << 12));
                if (r < 0x80000) {
                    s32 x = (0x80 - (r >> 12)) & 0xffff;
                    *p = ((x << 8) & 0xff00) | ((0x100 - x) & 0xff);
                } else {
                    *p = 0xff;
                }
            }
        }
    }
}

extern "C" void IrisWipe_SwapTables() {
#ifdef N3DS_DIRECT
    gpu2d_iris(gScreenTransition.progress);
#endif
    u32 v = sIrisWipeFlags;
    if (v & 2) {
        sIrisWipeActiveTable = sIrisWipeTableA;
        v &= ~2;
        sIrisWipeFlags = v;
    } else {
        sIrisWipeActiveTable = sIrisWipeTableB;
        v |= 2;
        sIrisWipeFlags = v;
    }
}

extern "C" void CutTransition_Begin() {
    Gfx2d_SetBrightness(0);
}

extern "C" void CutTransition_End() {}

extern "C" void CutTransition_Update() {}

extern "C" void CutTransition_VBlank() {}

extern "C" void ScreenTransition_Init() {
    gScreenTransition.state = 0;
    gScreenTransition.type = 0;
    gScreenTransition.brightness = -16;
    gScreenTransition.progress = 0x1000;
    gScreenTransition.step = 0;
    sIrisWipeActiveTable = 0;
    sIrisWipeFlags = 0;
}

extern "C" BOOL ScreenTransition_StartFadeOut(u32 a, u32 b) {
    u32 old = gScreenTransition.type;
    BOOL ok;
    if (gScreenTransition.state == 2) ok = TRUE; else ok = FALSE;
    if (!ok && data_021c3cb8 == 0) return FALSE;
    gScreenTransition.state = 3;
    data_021c3cb8 = 0;
    void (*fn)() = sTransitionTypeTable[old].handlers[1];
    if (fn) fn();
    gScreenTransition.type = a;
    gScreenTransition.progress = 0;
    fn = sTransitionTypeTable[a].handlers[0];
    if (fn) {
        fn();
        data_021c1b3c->sceneFade.onFadeOut();
        TransitionCommIcon_RequestShow(a);
        if (a == 2) Snd_FadeOutScene();
    }
    if (b == 0 || a == 3) {
        gScreenTransition.step = 0x1000;
    } else {
        gScreenTransition.step = FX_Div(0x1000, b << 12);
    }
    return TRUE;
}

extern "C" BOOL ScreenTransition_StartFadeIn(u32 a, u32 b, u32 c) {
    BOOL ok;
    if (gScreenTransition.state == 0) ok = TRUE; else ok = FALSE;
    if (!ok) return FALSE;
    ScreenTransition_HideCover();
    gScreenTransition.state = 1;
    gScreenTransition.type = a;
    gScreenTransition.progress = 0x1000;
    void (*fn)() = sTransitionTypeTable[a].handlers[0];
    if (fn) fn();
    if (b == 0 || a == 3) {
        gScreenTransition.step = -0x1000;
    } else {
        gScreenTransition.step = FX_Div(-0x1000, b << 12);
    }
    if (c == 0) {
        data_021c1b3c->sceneFade.onFadeIn();
        TransitionCommIcon_RequestHide(a);
    }
    return TRUE;
}

extern "C" void ScreenTransition_Update() {
    if (CommCaution_ArePlanesHidden() != 0) return;
    u32 idx = gScreenTransition.type;
    u32 st = gScreenTransition.state;
    if (st == 0 || st == 2) return;
    s32 v = gScreenTransition.step;
    if (v != 0) {
        u32 a = v >= 0 ? 0x1000 : 0;
        if (v < 0) v = -v;
        if (Math_StepS32Alt(&gScreenTransition.progress, a, v) != 0) {
            gScreenTransition.step = 0;
        }
    }
    void (*fn)() = sTransitionTypeTable[idx].handlers[2];
    if (fn) fn();
}

extern "C" void ScreenTransition_VBlank() {
    if (CommCaution_ArePlanesHidden() != 0) return;
    ScreenTransition *s = &gScreenTransition;
    u32 idx = s->type;
    u32 st = s->state;
    if (st == 2 || st == 0) return;
    void (*fn)() = sTransitionTypeTable[idx].handlers[3];
    if (fn) fn();
    if (gScreenTransition.state == 1) {
        if (gScreenTransition.step != 0) return;
        gScreenTransition.state = 2;
        fn = sTransitionTypeTable[idx].handlers[1];
        if (fn) fn();
    } else if (gScreenTransition.state == 3) {
        if (gScreenTransition.step != 0) return;
        gScreenTransition.state = 0;
        fn = sTransitionTypeTable[idx].handlers[1];
        if (fn) fn();
        ScreenTransition_OnHidden();
    }
}

extern "C" void ScreenTransition_OnHidden() {
    ScreenLayers_AcquireStub();
    Gfx2d_SetMainPlanes(0x10);
    Gfx2d_SetSubPlanes(0);
    ScreenTransition_ShowCover();
    Gfx2d_SetBrightness(-16);
}

extern "C" void ScreenTransition_HideCover() {
    Gfx2d_HideMainPlanes(4);
    Gfx2d_HideSubPlanes(4);
    ScreenLayers_ReleaseStub();
}


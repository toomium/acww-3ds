#include "ds_div.h"
#include "item/ItemName.h"
#include "types.h"
#include "nitro/gxoam.h"
#include "item/ItemIconCache.h"
#include "talk/MsgStringBase.h"
#include "talk/MsgStringAttr.h"
#include "talk/MsgString.h"
#include "ui/LabelString.h"
#include "gfx/BgVramTask.h"
#include "menu/MenuScrollKnob.h"
#include "menu/CreatureBookPanel.h"
#include "player/PlayerData.h"
#include "ui/ScrollKnob.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0203c23c.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_02085940.h"
#include "proto/main/unk_02087e0c.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov094/unk_ov094_02292da8.h"
#include "proto/ov114/unk_ov114_02294c40.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/main/unk_02087e70.h"
#include "proto/ov002/unk_ov002_022025cc.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
// the options button as a focus in the tab row, left of the pockets' tab (0-0xc are taken, 0xd on the list's entries,
// at most 0xd + 0x38): a tab focus whose tab is N3DS_OPT_TAB
#define BOOK_OPT_FOCUS 0xff
#endif


extern "C" {
extern const u8 data_ov114_0229654c[];
extern const u8 data_ov114_02296550[];
extern const s32 data_ov114_02296554[];
extern const s32 data_ov114_02296564[];
extern u16 gPad;

s32 _s32_div_f(s32 a, s32 b);
s32 Item_GetInfoUnk02(u16 *item);
void *MI_CpuCopy8(void *dst, void *src, u32 n);

u8 *Msg_SkipLines(u8 *p, s32 i);
void Math_StepS32(void *p, s32 a, s32 b);

void func_020b8800(void *p);
void func_020b85f8(void *p);
void MIi_CpuCopy16(void *, void *, u32);
void func_0206fcc8(void *p);
void func_0206fca8(void *p);
void func_02135714(void *, s32, s32, void (*)(void *), void (*)(void *));
void func_021355f0(void *, s32, s32, void (*)(void *));

}











CreatureBookPanel::CreatureBookPanel() {}

CreatureBookPanel::~CreatureBookPanel() {}

void CreatureBookPanel::init(u8 a, u8 b, u8 c, u8 d) {
    flags = 0;
    mainLayer = a;
    listLayer = b;
    pictureLayer = c;
    listBlendMask = Gfx2d_GetLayerBlendMask(b);
    pictureBlendMask = Gfx2d_GetLayerBlendMask(c);
    mode = d;
    iconCache.invalidate();
    CreatureBook_InitRows(this);
    switch (d) {
    case 0:
        listLength = 0x38;
        ownTabFocus = 7;
        break;
    case 1:
        listLength = 0x38;
        ownTabFocus = 8;
        break;
    default:
        listLength = 0x38;
        ownTabFocus = 0;
        break;
    }
    CreatureBook_BuildCaughtMask(this);
    focus = 0x10;
    scrollMax = (listLength - 8) * 0x1b;
    pendingScroll = 0;
    scrollKnob.show();
    u8 *p = &pageButtonFlashTimers[1];
    *p = 0;
    pageButtonFlashTimers[0] = *p;
    ((MsgString *)(&descText))->clear();
    touchTarget = 4;
}

void CreatureBookPanel::cleanup() {
    paletteTask.cancel();
    CreatureBook_CancelIconTasks(this);
    CreatureBook_ClearLabels(this);
}

void CreatureBookPanel::preUpdate() {
    paletteTask.cancel();
    CreatureBook_CancelIconTasks(this);
    scrollKnob.update();
    CreatureBook_ClearLabels(this);
}

void CreatureBookPanel::postUpdate() {
    CreatureBook_ApplyScrollInertia(this);
    scrollKnob.updateRelease();
    CreatureBook_UpdatePictureFade(this);
    if (CreatureBook_HasFlags(this, 1)) {
        if (paletteTask.requestPalette(paletteBuf, mainLayer, 4)) {
            CreatureBook_ClearFlags(this, 1);
        }
    }
}

void CreatureBookPanel::loadBgGraphics() {
    Heap *h = gCurrentHeap;
    Gfx2d_LoadCharFile("menu/fish/bg0.bch", h, mainLayer, 0x109, 0x109, 0x153);
    Gfx2d_LoadCharFile("menu/fish/bg1.bch", h, mainLayer, 0x154, 0x154, 0x19f);
    if (mode == 1) {
        Gfx2d_LoadCharFile("menu/fish/bug_bg.bch", h, mainLayer, 0x150, 0x150, 0x164);
    }
    switch (mode) {
    case 0:
        sCreatureBgPaletteName = "menu/fish/bg.bpl";
        break;
    case 1:
        sCreatureBgPaletteName = "menu/fish/bug_bg.bpl";
        break;
    }
    Gfx2d_LoadPaletteFile(sCreatureBgPaletteName, h, mainLayer, 1, 1, 6);
    File_LoadToBuffer("menu/fish/bg4.bpl", paletteFile, 0x20);
    MIi_CpuCopy16(paletteFile, paletteBuf, 0x20);
    Gfx2d_LoadScreenFile("menu/fish/a_bg.bsc", h, mainLayer);
    Gfx2d_LoadScreenFile("menu/fish/b_bg.bsc", h, listLayer);
    Gfx2d_LoadScreenFile("menu/fish/c_bg.bsc", h, pictureLayer);
}

void CreatureBookPanel::loadObjGraphics() {
    Heap *h = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/icon/b_obj_itm.bpl", h, 8, 7, 7, 0xe);
    Gfx2d_LoadPaletteFile("menu/fish/obj.bpl", h, 8, 4, 4, 6);
    Gfx2d_LoadCharFile("menu/fish/obj0.bch", h, 8, 0xc0, 0xc0, 0x140);
    Gfx2d_LoadCharFile("menu/fish/obj1.bch", h, 8, 0x141, 0x141, 0x1bf);
}

void CreatureBookPanel::drawButtons(s32 y) {
    s32 py = y + 0x60;
    s32 i = 0;
    do {
        s32 pal = pageButtonPalettes[i];
        u8 *q = &pageButtonFlashTimers[i];
        u32 v = *q;
        if (v != 0) {
            *q = v - 1;
            pal = 6;
        }
        // the next / previous page buttons' cells
        GXOamAttr *cell = i == 0 ? &data_ov114_02296588 : &data_ov114_02296590;
        Oam_DrawObj(1, cell, 0x80, py, pal, 1, NULL);
        i++;
    } while (i < 2);
    s32 py1 = py;
    if (touchTarget == 0) py1 = py + 2;
    Oam_DrawObj(1, &data_ov114_022965c0[0], 0x80, py1, -1, 1, NULL);
    s32 py2 = py;
    if (touchTarget == 1) py2 = py + 2;
    Oam_DrawObj(1, &data_ov114_022965c0[1], 0x80, py2, -1, 1, NULL);
    Oam_DrawCell(1, &data_ov114_022965c0[2], 0x80, py, -1, 1, 0x1000, 0x1000, 0, -1, 0, 0);
}

BOOL CreatureBookPanel::hitDescPageButtons(s32 x, s32 y) {
    s32 xs = x - 0x80;
    s32 ys = y - 0x60;
    if (Cell_HitTest(&data_ov114_02296588, xs, ys, 2, 2)) {
        return CreatureBook_NextDescPage(this);
    }
    if (Cell_HitTest(&data_ov114_02296590, xs, ys, 2, 2)) {
        return CreatureBook_PrevDescPage(this);
    }
    return FALSE;
}

void CreatureBookPanel::drawScrollKnob() {
    scrollKnob.draw();
}

void CreatureBookPanel::placeScrollKnob(s32 x) {
    ((ScrollKnob *)(&scrollKnob))->moveTo(knobPos - 0x4e, x + 0x4a);
}

void CreatureBookPanel::syncKnobToScroll() {
    knobPos = _s32_div_f(scroll * 0x8c, scrollMax);
}

void CreatureBookPanel::syncScrollToKnob() {
    if (CreatureBook_SetScroll(this, _s32_div_f(knobPos * scrollMax, 0x8c))) {
        CreatureBook_RefreshRowIcons(this);
    }
    pendingScroll = 0;
}

BOOL CreatureBookPanel::beginScrollTouch(s32 x, s32 y) {
    if (scrollKnob.hitTest(x, y)) {
        knobGrabOffset = knobPos - x;
        scrollKnob.grab();
        touchTarget = 2;
        knobLastTickPos = knobPos;
        return TRUE;
    }
    s32 xs = x - 0x80;
    s32 ys = y - 0x60;
    if (Cell_HitTest(&data_ov114_022965c0[0], xs, ys, 2, 2)) {
        touchTarget = 0;
        return TRUE;
    }
    if (Cell_HitTest(&data_ov114_022965c0[1], xs, ys, 2, 2)) {
        touchTarget = 1;
        return TRUE;
    }
    if (x > 0x3a && x < 0xc6 && y > 0xac && y < 0xb8) {
        scrollKnob.grab();
        touchTarget = 3;
        knobLastTickPos = knobPos;
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_UpdateScrollTouch(CreatureBookPanel *s, s32 a)
{
    s32 *p = &s->knobPos;
    s32 old = *p;
    switch (s->touchTarget) {
    case 2:
        *p = a + s->knobGrabOffset;
        break;
    case 0:
        *p = old - 2;
        break;
    case 1:
        *p = old + 2;
        break;
    case 3:
        Math_StepS32(p, a - 0x3a, 4);
        break;
    }
    if (s->knobPos < 0) {
        s->knobPos = 0;
    }
    if (s->knobPos > 0x8c) {
        s->knobPos = 0x8c;
    }
    s->syncScrollToKnob();
    if (s->touchTarget == 2) {
        s32 d = s->knobLastTickPos - s->knobPos;
        if (d >= 4 || d <= -4) {
            Menu_PlayScrollTickSe();
            s->knobLastTickPos = s->knobPos;
        }
    } else if (s->knobPos != old) {
        Menu_PlayScrollTickSe();
    }
}

void CreatureBook_EndScrollTouch(CreatureBookPanel *s)
{
    if (s->touchTarget == 2) {
        ((MenuScrollKnob *)(&s->scrollKnob))->release();
    }
    s->touchTarget = 4;
}

s32 CreatureBook_WaitScrollHoldStart(CreatureBookPanel *s)
{
    if (s->touchTarget == 2) {
        return ((ScrollKnob *)(&s->scrollKnob))->areAnimsDone();
    }
    CreatureBook_UpdateScrollHold(s);
    return TRUE;
}

void CreatureBook_UpdateScrollHold(CreatureBookPanel *s)
{
    s32 old = s->knobPos;
    switch (s->touchTarget) {
    case 2: {
        u16 k = gPad;
        if (k & 0x20) {
            s->knobPos = old - 2;
        } else if (k & 0x10) {
            s->knobPos = old + 2;
        }
        break;
    }
    case 0:
        s->knobPos = old - 2;
        break;
    case 1:
        s->knobPos = old + 2;
        break;
    }
    if (s->knobPos < 0) {
        s->knobPos = 0;
    }
    if (s->knobPos > 0x8c) {
        s->knobPos = 0x8c;
    }
    if (s->knobPos != old) {
        Menu_PlayScrollTickSe();
    }
    s->syncScrollToKnob();
}

void CreatureBook_ReleaseKnob(CreatureBookPanel *s)
{
    if (s->touchTarget == 2) {
        ((MenuScrollKnob *)(&s->scrollKnob))->release();
    }
}

BOOL CreatureBook_FinishScrollHold(CreatureBookPanel *s)
{
    if (s->touchTarget == 2) {
        if (((ScrollKnob *)(&s->scrollKnob))->areAnimsDone()) {
            ((MenuScrollKnob *)(&s->scrollKnob))->show();
            s->touchTarget = 4;
            return TRUE;
        }
    } else {
        s->touchTarget = 4;
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_ApplyScrollInertia(CreatureBookPanel *s)
{
    s32 pos = s->scroll;
    s32 r = s->pendingScroll;
    if (r > 0) {
        if (r < 0xb) {
            pos += r;
            s->pendingScroll = 0;
        } else {
            pos += 0xb;
            r -= 0xb;
            s->pendingScroll = r;
        }
    } else if (r < 0) {
        if (r > -11) {
            pos += r;
            s->pendingScroll = 0;
        } else {
            pos -= 0xb;
            r += 0xb;
            s->pendingScroll = r;
        }
    }
    if (pos != s->scroll) {
        s->syncKnobToScroll();
        if (CreatureBook_SetScroll(s, pos)) {
            CreatureBook_RefreshRowIcons(s);
        }
    }
}

MsgString406::MsgString406() { clear(); }

MsgString406::~MsgString406() {}

u32 MsgString406::capacity() { return 0x196; }

u8 *MsgString406::data() { return text; }

void CreatureBook_LoadDescription(CreatureBookPanel *s)
{
    u8 v = s->shownEntry;
    const char *name;
    switch (s->mode) {
    case 0:
        name = (const char *)"obj_etc_fish";
        break;
    case 1:
        name = (const char *)"obj_etc_insect";
        break;
    }
    if (v == 0xff) {
        ((MsgString *)(&s->descText))->clear();
        s->descPageCount = 1;
    } else {
        u8 key = v;
        String_Load(&s->descText, &key, name);
        s->descPageCount = CreatureBook_CountDescPages(s);
    }
    s->descPage = 0;
    CreatureBook_ShowDescPage(s);
}

u8 CreatureBook_CountDescPages(CreatureBookPanel *s)
{
    s32 n = 0;
    s32 k = n;
    for (; n < 5; n++) {
        u8 *a = Msg_SkipLines(s->descText.text, k);
        u8 *b = Msg_SkipLines(s->descText.text, k + 1);
        u8 *c = Msg_SkipLines(s->descText.text, k + 2);
        k += 3;
        if (a == 0 && b == 0 && c == 0) {
            return (u8)n;
        }
    }
    return 5;
}

void CreatureBook_ShowDescPage(CreatureBookPanel *s)
{
    LabelString *o[3];
    s32 z10 = 0;
    s32 z14 = 0;
    s32 k = (u8)s->descPage * 3;
    s32 y;
    s32 i;
    o[0] = CreatureBook_AllocLabel(s);
    o[1] = CreatureBook_AllocLabel(s);
    o[2] = CreatureBook_AllocLabel(s);
    y = 0xbb;
    i = 0;
    do {
        u8 *t = Msg_SkipLines(s->descText.text, k);
        LabelString *ob;
        if (t != NULL) {
            ob = o[i];
            ob->setLine(t);
        } else {
            ob = o[i];
            ob->clear();
        }
        ob->createLabel(s->mainLayer, y, 0xd, 2, 3, z10);
        ob->redrawAligned(z14, z14);
        k++;
        y += 0x1a;
        i++;
    } while (i < 3);
    if (s->descPage == 0) {
        s->pageButtonPalettes[1] = 4;
    } else {
        s->pageButtonPalettes[1] = 5;
    }
    if (s->descPage + 1 >= s->descPageCount) {
        s->pageButtonPalettes[0] = 4;
    } else {
        s->pageButtonPalettes[0] = 5;
    }
}

BOOL CreatureBook_NextDescPage(CreatureBookPanel *s)
{
    s32 n = s->descPage + 1;
    if (n < s->descPageCount) {
        s->descPage = n;
        s->pageButtonFlashTimers[0] = 5;
        CreatureBook_ShowDescPage(s);
        Snd_PlaySe(0x39);
        return TRUE;
    }
    return FALSE;
}

BOOL CreatureBook_PrevDescPage(CreatureBookPanel *s)
{
    u32 c = s->descPage;
    if (c != 0) {
        s->descPage = c - 1;
        s->pageButtonFlashTimers[1] = 5;
        CreatureBook_ShowDescPage(s);
        Snd_PlaySe(0x39);
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_SetBgFadeColor(CreatureBookPanel *s, s32 a, s32 b)
{
    s32 c1 = s->paletteFile[3];
    u8 r = c1 & 0x1f;
    u8 g = (c1 & 0x3e0) >> 5;
    u8 bl = (c1 & 0x7c00) >> 10;
    s32 d = b - a;
    s32 c2 = s->paletteFile[2];
    r = DS_DIV(((u8)(c2 & 0x1f) * a + r * d), b);
    g = DS_DIV(((u8)((c2 & 0x3e0) >> 5) * a + g * d), b);
    bl = DS_DIV(((u8)((c2 & 0x7c00) >> 10) * a + bl * d), b);
    s->paletteBuf[2] = r | (g << 5) | (bl << 10);
    CreatureBook_SetFlags(s, 1);
}

void CreatureBook_CancelIconTasks(CreatureBookPanel *s)
{
    s32 i;
    for (i = 0; i < 9; i++) {
        s->rowIconTasks[i].BgVramTask::cancel();
    }
    s->iconTaskCount = 0;
}

u32 CreatureBook_AllocIconTask(CreatureBookPanel *s)
{
    u32 c = s->iconTaskCount;
    if (c >= 9) {
        return 8;
    }
    s->iconTaskCount = c + 1;
    return c;
}

void CreatureBook_ClearLabels(CreatureBookPanel *s)
{
    s32 i;
    s->labelCount = 0;
    for (i = 0; i < 4; i++) {
        s->labels[i].destroyLabel();
    }
}

extern "C" LabelString *CreatureBook_AllocLabel(CreatureBookPanel *s)
{
    u8 *p = &s->labelCount;
    u32 c = *p;
    if (c >= 4) {
        return &s->labels[3];
    }
    *p = c + 1;
    return &s->labels[*p - 1];
}

void CreatureBook_SetNameLabel(CreatureBookPanel *s, u32 a)
{
    ItemName obj;
    u16 col;
    u32 v;
    if (a == 0xff) {
        obj.clear();
    } else {
        switch (s->mode) {
        case 0:
            v = a < 0x38 ? (u16)(a + 0x12e8) : 0x12e8;
            break;
        case 1:
            v = a < 0x38 ? (u16)(a + 0x12b0) : 0x12b0;
            break;
        default:
            return;
        }
        col = v;
        obj.setFromItem(&col);
    }
    LabelString *t = CreatureBook_AllocLabel(s);
    t->copy(&obj);
    t->createLabel(s->mainLayer, 0xa1, 0xd, 1, 3, 0);
    t->redrawAligned(1, 0);
}

void CreatureBook_DrawRows(CreatureBookPanel *s, s32 a)
{
    s32 z24 = 0;
    s32 i;
    s32 rowY = s->firstIconX;
    s32 x = s->iconRowY + a;
    s32 idx = s->firstIconSlot;
    s32 m1;
    s32 t14;
    GXOamAttr *src;
    i = 0;
    m1 = -1;
    do {
        if (CreatureBook_IsCaught(s, s->firstVisibleEntry + i)) {
            src = &s->rowIcons[idx].cell;
            t14 = m1;
        } else {
            src = &data_ov114_02296580;
            t14 = 4;
        }
        Oam_DrawObj(1, src, rowY, x, m1, 2, NULL);
        if (s->selectedEntry == s->firstVisibleEntry + i) {
            Oam_DrawObj(1, &data_ov114_022965a0, rowY, x, m1, 2, NULL);
        }
        Oam_DrawObj(1, &data_ov114_02296598, rowY, x, t14, 2, NULL);
        rowY += 0x1b;
        idx++;
        if (idx >= 9) {
            idx = z24;
        }
        i++;
    } while (i < 9);
}

BOOL CreatureBook_SetScroll(CreatureBookPanel *s, s32 v)
{
    if (v < 0) {
        v = 0;
    }
    if (v >= s->scrollMax) {
        v = s->scrollMax;
    }
    s->scroll = v;
    s32 old = s->firstVisibleEntry;
    s->firstIconX = 0x22 - v;
    s->firstVisibleEntry = 0;
    while (s->firstIconX < 8) {
        s->firstIconX += 0x1b;
        s->firstVisibleEntry++;
    }
    if (old != s->firstVisibleEntry) {
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_RefreshRowIcons(CreatureBookPanel *s)
{
    s32 zero;
    u16 col;
    u32 pos;
    s32 i;
    s32 idx;
    s->firstIconSlot = s->firstVisibleEntry % 9;
    pos = s->firstVisibleEntry;
    idx = s->firstIconSlot;
    col = 0xfff1;
    zero = 0;
    i = 0;
    goto test0;
loop0:
    switch (s->mode) {
    case 0:
        col = pos < 0x38 ? (u16)(pos + 0x12e8) : 0x12e8;
        break;
    case 1:
        col = pos < 0x38 ? (u16)(pos + 0x12b0) : 0x12b0;
        break;
    default:
        return;
    }
    CreatureBook_SetRowIcon(s, &col, idx);
    idx++;
    if (idx >= 9) {
        idx = zero;
    }
    pos++;
    if (pos >= 0x38) {
        return;
    }
    i++;
test0:
    if (i < 9) goto loop0;
}

BOOL CreatureBook_TouchRow(CreatureBookPanel *s, s32 x, s32 y) {
    if (y < 0x90 || y >= 0xb0) {
        return FALSE;
    }
    if (x < 0x10 || x >= 0xf0) {
        return FALSE;
    }
    if (x < 0x18) {
        x = 0x18;
    }
    if (x > 0xe8) {
        x = 0xe8;
    }
    s32 t = x - (0x15 - s->scroll);
    if (t < 0) {
        t = 0;
    }
    s32 q = t / 0x1b;
    if (q >= s->listLength) {
        q = s->listLength - 1;
    }
    if (CreatureBook_IsCaught(s, q)) {
        if (s->selectedEntry != q) {
            Snd_PlaySe(0x29);
        }
        CreatureBook_SelectEntry(s, (u8)q);
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_InitRows(CreatureBookPanel *s) {
    s32 i;
    for (i = 0; i < 9; i++) {
        CreatureBookIconObj *e = &s->rowIcons[i];
        e->cell = data_ov114_02296580;
        e->cell.charNo = i * 2 + 0xc0;
        e->iconId = -1;
    }
    s->iconRowY = (s32)Oam_GetObjY((s8 *)&data_ov114_022965e0[0]) + 0x70;
    CreatureBook_SetScroll(s, 0);
}

extern "C" void CreatureBook_SetRowIcon(CreatureBookPanel *s, u16 *item, s32 idx) {
    CreatureBookIconObj *e = &s->rowIcons[idx];
    s32 key = Item_GetInfoUnk02(item);
    if (key != e->iconId) {
        e->iconId = key;
        u32 lo = e->cell.charNo;
        void *dst = s->iconCache.getIconChars(key);
        s32 n = CreatureBook_AllocIconTask(s);
        u8 *src = s->iconUploadChars[n].top;
        u8 *src2 = src + 0x40;
        MI_CpuCopy8(dst, src, 0x40);
        MI_CpuCopy8((u8 *)dst + 0x400, src2, 0x40);
        ((BgVramTaskPair *)(&s->rowIconTasks[n]))->requestCharPair(src, src2, 8, lo, lo + 1, lo + 0x20, lo + 0x21);
        e->cell.cParam = InventoryItemGrid_GetIconPalette(s, key);
    }
}

void CreatureBook_InitPictureView(CreatureBookPanel *s) {
    s->selectedEntry = 0xff;
    s->pictureFade = 0;
    CreatureBook_SetNameLabel(s, 0xff);
    CreatureBook_ShowSelected(s);
    CreatureBook_SetBgFadeColor(s, 0, 0x10);
    Gfx2d_SetSubAlphaBlend(s->pictureBlendMask, s->listBlendMask, 0);
}

void CreatureBook_StopPictureFade(CreatureBookPanel *s) {
    s->selectedEntry = s->shownEntry;
    Gfx2d_ResetSubBlend();
    s->pictureFade = 0x10;
}

void CreatureBook_ShowSelected(CreatureBookPanel *s) {
    s->shownEntry = s->selectedEntry;
    CreatureBook_LoadPicture(s);
    CreatureBook_LoadDescription(s);
}

void CreatureBook_LoadPicture(CreatureBookPanel *s) {
    u32 c = s->shownEntry;
    if (c == 0xff) {
        return;
    }
    s32 q = (s32)c / 12;
    Heap *font = gCurrentHeap;
    switch (s->mode) {
    case 0:
        sCreaturePicDir = "menu/fish_pic";
        break;
    case 1:
        sCreaturePicDir = "menu/bug_pic";
        break;
    default:
        return;
    }
    Str_SPrintf(sCreaturePicPath, "%s/%d/%d_%02d.bch", sCreaturePicDir, q, q, c);
    Gfx2d_LoadCharFile(sCreaturePicPath, font, s->pictureLayer, 0x11, 0x11, 0xa0);
    Str_SPrintf(sCreaturePicPath, "%s/%d/%d.bpl", sCreaturePicDir, q, q, c);
    Gfx2d_LoadPaletteFileSlot(sCreaturePicPath, font, s->pictureLayer, (s32)c % 12, 5);
}

void CreatureBook_UpdatePictureFade(CreatureBookPanel *s) {
    u8 old = s->pictureFade;
    u32 a = s->shownEntry;
    if (a == s->selectedEntry) {
        if (a != 0xff && old != 0x10) {
            if (old < 0xc) {
                if (old == 0) {
                    Gfx2d_ShowLayer(s->pictureLayer);
                }
                s->pictureFade = s->pictureFade + 4;
                Gfx2d_SetSubAlphaBlend(s->pictureBlendMask, s->listBlendMask, s->pictureFade);
            } else {
                Gfx2d_ResetSubBlend();
            }
        }
    } else if (old == 0) {
        CreatureBook_ShowSelected(s);
    } else if (old > 4) {
        s->pictureFade = old - 4;
        Gfx2d_SetSubAlphaBlend(s->pictureBlendMask, s->listBlendMask, s->pictureFade);
    } else {
        s->pictureFade = 0;
        Gfx2d_HideLayer(s->pictureLayer);
    }
    if (old != s->pictureFade) {
        CreatureBook_SetBgFadeColor(s, s->pictureFade, 0x10);
    }
}

void CreatureBook_SelectEntry(CreatureBookPanel *s, u8 v) {
    s->selectedEntry = v;
    if (s->selectedEntry != s->shownEntry) {
        CreatureBook_SetNameLabel(s, v);
    }
}

s32 CreatureBook_GetFocusX(CreatureBookPanel *s) {
    s32 r;
    if (CreatureBook_GetFocusedTab(s) != -1) {
#ifdef N3DS_DIRECT
        r = MenuTabBar_GetTabX(CreatureBook_GetFocusedTab(s));
#else
        r = MenuTabBar_GetTabX(s->focus - 5);
#endif
    } else {
        u32 t = s->focus;
        if (t == 4) {
            r = ((MenuScrollKnob *)(&s->scrollKnob))->getGripX();
        } else if (t < 4) {
            r = data_ov114_02296554[t];
        } else {
            r = (t - 0xd) * 0x1b + 0x22 - s->scroll - s->pendingScroll;
        }
    }
    return r;
}

s32 CreatureBook_GetFocusY(CreatureBookPanel *s) {
    s32 r;
    if (CreatureBook_GetFocusedTab(s) != -1) {
        return 8;
    }
    u32 t = s->focus;
    if (t == 4) {
        return ((MenuScrollKnob *)(&s->scrollKnob))->getGripY();
    }
    if (t < 4) {
        r = data_ov114_02296564[t];
        if (t == 2 && s->touchTarget == 0) {
            r += 2;
        } else if (t == 3 && s->touchTarget == 1) {
            r += 2;
        }
    } else {
        r = 0x9e;
    }
    return r;
}

s32 CreatureBook_GetFocusedTab(CreatureBookPanel *s) {
    u8 v = s->focus;
    if (v >= 5 && v <= 0xc) {
        return v - 5;
    }
#ifdef N3DS_DIRECT
    if (v == BOOK_OPT_FOCUS) {
        return N3DS_OPT_TAB;
    }
#endif
    return -1;
}

BOOL CreatureBook_IsFocusOnPageArrow(CreatureBookPanel *s) {
    BOOL r = TRUE;
    u8 v = s->focus;
    if (v != 0 && v != 1) {
        r = FALSE;
    }
    return r;
}

BOOL CreatureBook_MoveFocus(CreatureBookPanel *s, u32 pad) {
    if (pad == 0) {
        return FALSE;
    }
    u8 old = s->focus;
#ifdef N3DS_DIRECT
    // the options button: Left from the pockets' tab, Right back to it, Down as from the tabs
    if (s->focus == BOOK_OPT_FOCUS) {
        if (MenuKeys_HasDown(pad)) {
            s->focus = 0;
        } else if (MenuKeys_HasRight(pad)) {
            s->focus = 5;
        }
    } else if (s->focus == 5 && MenuKeys_HasLeft(pad) && !MenuKeys_HasDown(pad)) {
        s->focus = BOOK_OPT_FOCUS;
    } else
#endif
    if (CreatureBook_GetFocusedTab(s) != -1) {
        if (MenuKeys_HasDown(pad)) {
            s->focus = 0;
        } else if (MenuKeys_HasLeft(pad)) {
            if (s->focus > 5) {
                s->focus = s->focus - 1;
            }
        } else if (MenuKeys_HasRight(pad)) {
            if (s->focus < 0xc) {
                s->focus = s->focus + 1;
            }
        }
    } else {
        s32 c = s->focus;
        if ((u32)c <= 1) {
            if (MenuKeys_HasUp(pad)) {
                s->focus = s->ownTabFocus;
            } else if (MenuKeys_HasDown(pad)) {
                s->focus = CreatureBook_GetRowFocusAtCursor(s);
            } else if (MenuKeys_HasLeft(pad)) {
                s->focus = 1;
            } else {
                if (MenuKeys_HasRight(pad)) {
                    s->focus = 0;
                }
            }
        } else if ((u32)c >= 0xd) {
            s32 d = c - 0xd;
            if (MenuKeys_HasUp(pad)) {
                if (CreatureBook_GetFocusX(s) < 0x48) {
                    s->focus = 1;
                } else {
                    s->focus = 0;
                }
            } else if (MenuKeys_HasDown(pad)) {
                s32 r = CreatureBook_GetFocusX(s);
                if (r < 0x40) {
                    s->focus = 2;
                } else if (r >= 0xc0) {
                    s->focus = 3;
                } else {
                    s->focus = 4;
                }
            } else if (MenuKeys_HasRight(pad)) {
                if (d < s->listLength - 1) {
                    s->focus = s->focus + 1;
                    s32 r = CreatureBook_GetFocusX(s);
                    if (r > 0xe0) {
                        s->pendingScroll = r - 0xe0;
                    }
                }
            } else if (MenuKeys_HasLeft(pad)) {
                if (d > 0) {
                    s->focus = s->focus - 1;
                    s32 r = CreatureBook_GetFocusX(s);
                    if (r < 0x20) {
                        s->pendingScroll = r - 0x20;
                    }
                }
            }
        } else {
            s32 d = c - 2;
            if (MenuKeys_HasUp(pad)) {
                s->focus = CreatureBook_GetRowFocusAtCursor(s);
            } else if (MenuKeys_HasLeft(pad)) {
                s->focus = data_ov114_02296550[d];
            } else if (MenuKeys_HasRight(pad)) {
                s->focus = data_ov114_0229654c[d];
            }
        }
    }
    if (old != s->focus) {
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_ClampFocusToView(CreatureBookPanel *s) {
    if (s->focus >= 0xd) {
        s32 r = CreatureBook_GetFocusX(s);
        if (r < 0x20) {
            s->focus = s->firstVisibleEntry + 0xd;
            if (CreatureBook_GetFocusX(s) < 0x20) {
                s->focus = s->focus + 1;
            }
        } else if (r > 0xe0) {
            s->focus = s->firstVisibleEntry + 0x14;
            if (CreatureBook_GetFocusX(s) > 0xe0) {
                s->focus = s->focus - 1;
            }
        }
    }
}

u8 CreatureBook_GetRowFocusAtCursor(CreatureBookPanel *s) {
    s32 t = CreatureBook_GetFocusX(s) - (0x15 - s->scroll);
    if (t < 0) {
        t = 0;
    }
    s32 q = t / 0x1b;
    if (q >= s->listLength) {
        q = s->listLength - 1;
    }
    return q + 0xd;
}

BOOL CreatureBook_ActivateFocus(CreatureBookPanel *s) {
    u32 t = s->focus;
    if (t >= 0xd) {
        u32 k = t - 0xd;
        if (CreatureBook_IsCaught(s, k)) {
            if (s->selectedEntry != k) {
                Snd_PlaySe(0x29);
            }
            CreatureBook_SelectEntry(s, (u8)k);
        }
        return FALSE;
    } else {
        switch (t) {
        case 1:
            CreatureBook_PrevDescPage(s);
            return FALSE;
        case 0:
            CreatureBook_NextDescPage(s);
            return FALSE;
        case 2:
            s->touchTarget = 0;
            return TRUE;
        case 3:
            s->touchTarget = 1;
            return TRUE;
        case 4:
            s->touchTarget = 2;
            ((MenuScrollKnob *)(&s->scrollKnob))->grab();
            Menu_PlayScrollGrabSe();
            return TRUE;
        default:
            return FALSE;
        }
    }
}

void CreatureBook_BuildCaughtMask(CreatureBookPanel *s) {
    s32 i;
    u16 v;
    for (i = 0; i < 3; i++) {
        s->caughtMask[i] = 0;
    }
    PlayerData *p = PlayerData_GetCurrent();
    v = 0xfff1;
    if (s->mode == 0) {
        for (i = 0; i < s->listLength; i++) {
            v = (u32)i < 0x38 ? (u16)(i + 0x12e8) : 0x12e8;
            if (Catalog_HasItem(p->getCatalog(), &v)) {
                s->caughtMask[i >> 5] |= 1 << (i & 0x1f);
            }
        }
    } else {
        for (i = 0; i < s->listLength; i++) {
            v = (u32)i < 0x38 ? (u16)(i + 0x12b0) : 0x12b0;
            if (Catalog_HasItem(p->getCatalog(), &v)) {
                s->caughtMask[i >> 5] |= 1 << (i & 0x1f);
            }
        }
    }
}

BOOL CreatureBook_IsCaught(CreatureBookPanel *s, s32 i) {
    BOOL r = TRUE;
    if (((1 << (i & 0x1f)) & s->caughtMask[i >> 5]) == 0) {
        r = FALSE;
    }
    return r;
}

BOOL CreatureBook_HasFlags(CreatureBookPanel *s, u32 m) {
    if (s->flags & m) {
        return TRUE;
    }
    return FALSE;
}

void CreatureBook_SetFlags(CreatureBookPanel *s, u32 m) { s->flags = s->flags | m; }

void CreatureBook_ClearFlags(CreatureBookPanel *s, u32 m) { s->flags = s->flags & ~m; }

// Declarations for data defined further down (definition order sets the data layout)
extern "C" const u8 data_ov114_02296550[4];
extern "C" const s32 data_ov114_02296554[4];
extern "C" const s32 data_ov114_02296564[4];
extern "C" GXOamAttr data_ov114_02296580;
extern "C" GXOamAttr data_ov114_02296590;
extern "C" const u8 data_ov114_0229654c[4];
extern "C" GXOamAttr data_ov114_022965e0[9];
extern "C" char *sCreaturePicDir;
extern "C" char *sCreatureBgPaletteName;
extern "C" char sCreaturePicPath[0x38];

extern "C" const u8 data_ov114_02296550[4] = {0x02, 0x04, 0x02, 0x00};

extern "C" const s32 data_ov114_02296554[4] = {0x62, 0x34, 0x2d, 0xd4};

extern "C" const s32 data_ov114_02296564[4] = {0x71, 0x71, 0xb0, 0xb0};

extern "C" GXOamAttr data_ov114_02296580 = {0x41f800f8, 0x104, 0, 0x4, 0xffff};

extern "C" GXOamAttr data_ov114_02296588 = {0x51dc0007, 0x100, 0, 0x5, 0x0};

extern "C" GXOamAttr data_ov114_02296590 = {0x41ac0007, 0x100, 0, 0x4, 0xffff};

extern "C" GXOamAttr data_ov114_02296598 = {0x81f000f0, 0x144, 1, 0x5, 0xffff};

extern "C" GXOamAttr data_ov114_022965a0 = {0x81f000f0, 0x14c, 1, 0x4, 0xffff};

extern "C" const u8 data_ov114_0229654c[4] = {0x04, 0x03, 0x03, 0x00};

extern "C" GXOamAttr data_ov114_022965c0[4] = {
    {0x41a40049, 0x102, 0, 0x5, 0x0},
    {0x504d0049, 0x102, 0, 0x5, 0x0},
    {0x41a4004b, 0x102, 0, 0x1, 0x0},
    {0x504d004b, 0x102, 0, 0x1, 0xffff},
};

extern "C" GXOamAttr data_ov114_022965e0[9] = {
    {0x8184002e, 0x144, 1, 0x5, 0x0},
    {0x819f002e, 0x144, 1, 0x5, 0x0},
    {0x81ba002e, 0x144, 1, 0x4, 0x0},
    {0x81d5002e, 0x144, 1, 0x5, 0x0},
    {0x81f0002e, 0x144, 1, 0x4, 0x0},
    {0x800b002e, 0x144, 1, 0x5, 0x0},
    {0x8026002e, 0x144, 1, 0x5, 0x0},
    {0x8041002e, 0x144, 1, 0x4, 0x0},
    {0x805c002e, 0x144, 1, 0x5, 0xffff},
};

extern "C" char *sCreaturePicDir = 0;

extern "C" char *sCreatureBgPaletteName = 0;

extern "C" char sCreaturePicPath[0x38] = {0};

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov114_02294c40) {
    HOST_OVERLAY_VAR(data_ov114_02296580),
    HOST_OVERLAY_VAR(data_ov114_02296588),
    HOST_OVERLAY_VAR(data_ov114_02296590),
    HOST_OVERLAY_VAR(data_ov114_02296598),
    HOST_OVERLAY_VAR(data_ov114_022965a0),
    HOST_OVERLAY_VAR(data_ov114_022965c0),
    HOST_OVERLAY_VAR(data_ov114_022965e0),
    HOST_OVERLAY_VAR(sCreaturePicDir),
    HOST_OVERLAY_VAR(sCreatureBgPaletteName),
    HOST_OVERLAY_VAR(sCreaturePicPath),
    HOST_OVERLAY_VARS_END
};

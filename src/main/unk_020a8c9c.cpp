#include "nitro/fs.h"
#include "uptr.h"
#include "types.h"
#include "talk/MsgCallStack.h"
#include "text/Unk_02050288.h"
#include "talk/MsgStringBase.h"
#include "talk/MsgStringAttr.h"
#include "gfx/SpriteAnim.h"
#include "ui/UiWidget.h"
#include "talk/BmgReader.h"
#include "talk/BmgMsgAttr.h"
#include "talk/MsgString.h"
#include "ui/HandCursor.h"
#include "ui/ScrollKnob.h"
#include "ui/LabelButton.h"
#include "talk/ChoiceString.h"
#include "talk/ChoiceList.h"
#include "talk/ChoiceMenu.h"
#include "nitro/io_access.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_02119434.h"
#include "proto/main/unk_0209c37c.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_020a8c9c.h"
#include "proto/main/unk_020b2c50.h"
#include "n3ds_ui.h"

// ---------------------------------------------------------------------------------------------------------------------
// Classes from other files








// ---------------------------------------------------------------------------------------------------------------------
// Widget classes defined elsewhere






// ---------------------------------------------------------------------------------------------------------------------
// Classes of this file

// Vtable 0x020e2cb0: BMG reader over a caller-provided buffer (the global sChoiceBmgReader)
class BufferBmgReader : public BmgReader {
public:
    BufferBmgReader();
    virtual ~BufferBmgReader();
    virtual u8 *getBuffer();
    virtual u32 getBufferSize();

    void clearBuffer();
    void setBuffer(u8 *data, u32 size);

    /* 0xa4 */ u8 *buffer;
    /* 0xa8 */ u32 bufferSize;
};















// ---------------------------------------------------------------------------------------------------------------------
// Free functions and globals

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
BOOL Input_IsButtonMode(void);
}

extern "C" {
BOOL Input_IsTouchMode(void);
}

extern "C" {
void Input_SetButtonMode(void);
}

extern "C" {
void Input_SetTouchMode(void);
}

extern "C" {
void Input_Unlock(void);
}

extern "C" {
void Input_Lock(void);
}

extern "C" {
BOOL Input_IsDownHeld(void);
}

extern "C" {
BOOL Input_IsUpHeld(void);
}

extern "C" {
BOOL Input_IsAHeld(void);
}

extern "C" {
BOOL Input_IsStartTrig(void);
}

extern "C" {
BOOL Input_IsDownTrig(void);
}

extern "C" {
BOOL Input_IsUpTrig(void);
}

extern "C" {
BOOL Input_IsATrig(void);
}

extern "C" {
BOOL Input_IsAnyKeyTrig(void);
}

extern "C" {
BOOL Input_GetTouchHeldPos(s32 *x, s32 *y);
}

extern "C" {
BOOL Input_GetTouchTrigPos(s32 *x, s32 *y);
}


extern "C" {
BOOL Input_IsTouchTrigInRect(s32 x0, s32 x1, s32 y0, s32 y1);
}

extern "C" {
BOOL Input_IsTouchTrig(void);
}

extern "C" {
BmgMsgAttr *BmgMsgAttr_Get(BmgMsgAttr *p);
}

extern "C" {
void BmgMsgAttr_Clear(BmgMsgAttr *p);
}

extern "C" {
void BmgMsgAttr_Copy(BmgMsgAttr *dst, BmgMsgAttr *src);
}

extern "C" {
void MsgTextLabel_Destroy(TextLabel *p);
}

extern "C" {
TextLabel *MsgTextLabel_CreateVram(u32 a, s32 b, s32 c);
}

extern "C" {
BmgMsgAttr *Bmg_GetMsgAttr(BmgReader *p);
}

extern "C" {
}

extern "C" {
}

extern "C" {
}

extern "C" {
void GX_LoadBG1Scr(void *p, u32 a, u32 size);
}

extern "C" {
void GX_LoadBGPltt(void *p, u32 a, u32 size);
}

extern "C" {
void DC_FlushRange(void *p, u32 size);
}

extern "C" {
void MI_CpuFill8(void *dst, u32 value, u32 size);
}

extern "C" {
void MI_CpuCopy8(const void *src, void *dst, u32 size);
}

extern "C" {
s32 FS_ReadFile(void *file, void *buf, u32 size);
}

extern "C" {
s32 FS_CloseFile(void *file);
}

extern "C" {
s32 FS_OpenFile(void *file, const char *path);
}

extern "C" {
}

extern "C" {
char *strncpy(char *dst, const char *src, u32 n);
}

extern "C" {
s32 _s32_div_f(s32 a, s32 b);
}

extern "C" {
// This file
}

extern "C" {
}

extern "C" {
}

extern "C" {
extern const u8 sChoiceLabelTilesMode2[4];
}

extern "C" {
extern const u8 sChoiceLabelTilesMode3[4];
}

extern "C" {
extern const u8 sChoiceLabelTilesMode4[4];
}

extern "C" {
extern const void *const kChoiceBmgNames[2];
}

extern "C" {
extern const u16 kChoiceBlinkColors[6];
}

extern "C" {

// A debug variable (id, index) tunes the window's motion: the release game's stub (DebugVar_GetStub) gives 0
static inline s32 DebugVar(s32 id, s32 idx) { return (s32)DebugVar_GetStub(); }
extern SpriteAnimSeq data_020d467c[];  // (sprite sequence records)
}

extern "C" {
extern u8 gU8None;
}

extern "C" {
}

extern "C" {
extern u16 gPad[2];
}

extern "C" {
}

// ---- data ----
const u8 sChoiceLabelTilesMode2[4] = {0, 4, 0, 0};
const u8 sChoiceLabelTilesMode3[4] = {0, 3, 4, 0};
const u8 sChoiceLabelTilesMode4[4] = {0, 2, 3, 4};
const void *const kChoiceBmgNames[2] = {"select", "select2"};
const u16 kChoiceBlinkColors[6] = {0x7d5f, 0x7d5f, 0x7d5f, 0x7d5f, 0x7d5f, 0};
BufferBmgReader sChoiceBmgReader;


// ---------------------------------------------------------------------------------------------------------------------
// Functions, from the highest address to the lowest

ChoiceSlider::ChoiceSlider() : value(0x800), knob(1) {
    knob.setPriority(0);
}

ChoiceSlider::~ChoiceSlider() {}

void ChoiceSlider::draw() {
    if (knob.getState()) {
        knob.draw();
        GXOamAttr *r = (GXOamAttr *)data_020d467c[0].frames[0].cell;  // (the DS read the first record's first word)
        s32 a = getOriginY();
        s32 b = getOriginX();
        Oam_DrawCell(0, r, b, a, -1, 0, 0x1000, 0x1000, 0, -1, 0, 0);
    }
}

void ChoiceSlider::update() {
    s32 a = getKnobX();
    s32 b = getKnobY();
    knob.moveTo(a, b);
    knob.update();
}

void ChoiceSlider::setOrigin(s32 a, s32 b) {
    UiWidget::setOrigin(a, b);
    knob.setOrigin(a, b);
}

void ChoiceSlider::setPos(s32 v) { value = v; }

s32 ChoiceSlider::getKnobOffsetY() { return -((value - 0x1000) * 32) >> 12; }

s32 ChoiceSlider::getKnobX() { return 6; }

s32 ChoiceSlider::getKnobY() { return getKnobOffsetY(); }

s32 ChoiceSlider::getTrackBottom() { return 0x20; }

s32 ChoiceSlider::getTrackTop() { return 0; }

void ChoiceSlider::getKnobAnimOffset(s32 *x, s32 *y) { knob.getAnimOffset(x, y); }

BOOL ChoiceSlider::isKnobAnimDone() { return knob.areAnimsDone(); }

void ChoiceSlider::showKnob() { knob.setState(1); }

void ChoiceSlider::hideKnob() { knob.setState(0); }

void ChoiceSlider::startKnobGrab() { knob.setState(2); }

void ChoiceSlider::startKnobRelease() { knob.setState(3); }

ChoiceHandCursor::ChoiceHandCursor() : HandCursor(1) {
    priority = 0;
}

ChoiceHandCursor::~ChoiceHandCursor() {}

ChoiceWindow::ChoiceWindow() : bgScreen(0), rowColorsDirty(0), width(0), scrollX(0), scrollY(0), topY(0), numRows(0) {
    u32 i;
    for (i = 0; i < 5; i++) {
        rowColors[i] = 0;
    }
}

ChoiceWindow::~ChoiceWindow() {
    freeBg();
}

BOOL ChoiceWindow::loadBgFile(void *file) {
    s32 opened = FS_OpenFile(file, "/a_mes/a_mes1a_bg_nsc.bin");
    BOOL ok;
    bgScreen = (u8 *)Mem_AllocTail(0x800);
    if (bgScreen) {
        ok = FS_ReadFile(file, bgScreen, 0x800) != -1;
    } else {
        ok = FALSE;
    }
    s32 closed = FS_CloseFile(file);
    if (opened && ok && closed && bgScreen) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceWindow::loadBg() {
    BOOL r = FALSE;
    FSFile file;
    FS_InitFile(&file);
    if (loadBgFile(&file)) {
        r = TRUE;
    }
    return r;
}

void ChoiceWindow::freeBg() {
    if (bgScreen) {
        Heap_Free(gCurrentHeap, bgScreen);
        bgScreen = 0;
    }
}

void ChoiceWindow_BlankTiles(void *p, u32 n) {
    MI_CpuFill8(p, 0x10, n);
}

void ChoiceWindow::layout1Row() {
    MI_CpuCopy8(bgScreen + 0x2a2, bgScreen + 0x3a2, 0x1a);
    MI_CpuCopy8(bgScreen + 0x2e2, bgScreen + 0x3e2, 0x1a);
    MI_CpuCopy8(bgScreen + 0x25e, bgScreen + 0x35e, 0x22);
    MI_CpuCopy8(bgScreen + 0x51e, bgScreen + 0x41e, 0x22);
    *(u16 *)(bgScreen + 0x39e) = *(u16 *)(bgScreen + 0x29e);
    *(u16 *)(bgScreen + 0x3a0) = *(u16 *)(bgScreen + 0x2a0);
    *(u16 *)(bgScreen + 0x3be) = *(u16 *)(bgScreen + 0x2be);
    *(u16 *)(bgScreen + 0x3bc) = *(u16 *)(bgScreen + 0x2bc);
    *(u16 *)(bgScreen + 0x3de) = *(u16 *)(bgScreen + 0x4de);
    *(u16 *)(bgScreen + 0x3e0) = *(u16 *)(bgScreen + 0x4e0);
    *(u16 *)(bgScreen + 0x3fe) = *(u16 *)(bgScreen + 0x4fe);
    *(u16 *)(bgScreen + 0x3fc) = *(u16 *)(bgScreen + 0x4fc);
    ChoiceWindow_BlankTiles(bgScreen + 0x25e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x29e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x2de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x31e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x45e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x49e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x4de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x51e, 0x22);
    fitWidth(0xd, 0x10);
    setTopRow(0xd);
    numRows = 1;
}

void ChoiceWindow::layout2Rows() {
    MI_CpuCopy8(bgScreen + 0x29e, bgScreen + 0x39e, 0x22);
    MI_CpuCopy8(bgScreen + 0x2de, bgScreen + 0x3de, 0x22);
    MI_CpuCopy8(bgScreen + 0x25e, bgScreen + 0x35e, 0x22);
    MI_CpuCopy8(bgScreen + 0x49e, bgScreen + 0x41e, 0x22);
    MI_CpuCopy8(bgScreen + 0x4de, bgScreen + 0x45e, 0x22);
    MI_CpuCopy8(bgScreen + 0x51e, bgScreen + 0x49e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x25e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x29e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x2de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x31e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x4de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x51e, 0x22);
    fitWidth(0xd, 0x12);
    setTopRow(0xd);
    numRows = 2;
}

void ChoiceWindow::layout3Rows() {
    MI_CpuCopy8(bgScreen + 0x29e, bgScreen + 0x39e, 0x22);
    MI_CpuCopy8(bgScreen + 0x2de, bgScreen + 0x3de, 0x22);
    MI_CpuCopy8(bgScreen + 0x25e, bgScreen + 0x35e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x25e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x29e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x2de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x31e, 0x22);
    fitWidth(0xd, 0x14);
    setTopRow(0xd);
    numRows = 3;
}

void ChoiceWindow::layout4Rows() {
    MI_CpuCopy8(bgScreen + 0x29e, bgScreen + 0x31e, 0x22);
    MI_CpuCopy8(bgScreen + 0x2de, bgScreen + 0x35e, 0x22);
    MI_CpuCopy8(bgScreen + 0x25e, bgScreen + 0x2de, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x25e, 0x22);
    ChoiceWindow_BlankTiles(bgScreen + 0x29e, 0x22);
    fitWidth(0xb, 0x14);
    setTopRow(0xb);
    numRows = 4;
}

void ChoiceWindow::layout5Rows() {
    fitWidth(9, 0x14);
    setTopRow(9);
    numRows = 5;
}

void ChoiceWindow::fitWidth(u32 start, u32 end) {
    if ((u32)width < 13) {
        s32 n = 13 - width;
        s32 off1 = (30 - n) * 2;
        s32 off2 = (31 - n) * 2;
        u32 row;
        for (row = start; row <= end; row++) {
            s32 i;
            *(u16 *)(bgScreen + row * 0x40 + off1) = *(u16 *)(bgScreen + row * 0x40 + 0x3c);
            *(u16 *)(bgScreen + row * 0x40 + off2) = *(u16 *)(bgScreen + row * 0x40 + 0x3e);
            for (i = n - 1; i >= 0; i--) {
                ChoiceWindow_BlankTiles(bgScreen + row * 0x40 + (31 - i) * 2, 2);
            }
        }
    }
}

void ChoiceWindow::setTopRow(s32 v) {
    topY = (v - 9) << 3;
}

void ChoiceWindow::setWidth(s32 v) {
    width = v;
}

void ChoiceWindow::setupBgControl() {
    u16 *reg = (u16 *)0x400000a;
    IO_AND(reg, ~3);
    IO_W(reg, (IO_R(reg) & 0x43) | 0x500);
    IO_AND(reg, ~0x40);
}

void ChoiceWindow::uploadBg() {
    DC_FlushRange(bgScreen, 0x800);
    GX_LoadBG1Scr(bgScreen, 0, 0x800);
}

void ChoiceWindow::flushRowColors() {
    if (rowColorsDirty) {
        DC_FlushRange(rowColors, 10);
        GX_LoadBGPltt(rowColors, 0x82, 10);
        rowColorsDirty = 0;
    }
}

void ChoiceWindow::showLayer() {
    Gfx2d_ShowMainPlanes(2);
#ifdef N3DS_DIRECT  // (n3ds_ui.h: the choices, BG1, with the text box they belong to - TalkFrame_ShowBg - and their cursor)
    if (gN3dsTalkBgShown) gpu2d_bg_anchor(0, 6, 1, N3DS_UI_BOTTOM);
#endif
}

void ChoiceWindow::hideLayer() {
    Gfx2d_HideMainPlanes(2);
#ifdef N3DS_DIRECT
    if (gN3dsTalkBgShown) gpu2d_bg_anchor(0, 4, 1, N3DS_UI_BOTTOM);
#endif
}

void ChoiceWindow::setScroll(s32 a, s32 b) {
    s32 x = DebugVar(0x66, 2);
    s32 y = DebugVar(0x66, 3) - 8;
    s32 n = 13 - width;
    if (n < 0) {
        n = 0;
    }
    s32 pos = a + (x + y * n);
    Gfx2d_SetMainBg1Offset(pos, b);
    scrollX = pos;
    scrollY = b;
}

void ChoiceWindow::setAllRowColors() {
    s32 i;
    for (i = 0; i < 5; i++) {
        rowColors[i] = 12;
    }
    rowColorsDirty = 1;
}

void ChoiceWindow::setRowColor(s32 idx, u16 val) {
    s32 i;
    for (i = 0; i < 5; i++) {
        if (i == idx) {
            rowColors[i] = val;
        } else {
            rowColors[i] = 0;
        }
    }
    rowColorsDirty = 1;
}

s32 ChoiceWindow_GetLabelTile(s32 a, s32 mode) {
    s32 r = 0;
    if (mode == 1) {
    } else if (mode == 2) {
        r = sChoiceLabelTilesMode2[a];
    } else if (mode == 3) {
        r = sChoiceLabelTilesMode3[a];
    } else if (mode == 4) {
        r = sChoiceLabelTilesMode4[a];
    } else if (mode == 5) {
        r = a;
    }
    return r * 26 + 0x99;
}

ChoiceListWindow::ChoiceListWindow() {
    cursorRow = 0;
    blinkCount = 0;
    cursorVisible = 0;
}

ChoiceListWindow::~ChoiceListWindow() {}

void ChoiceListWindow::update() {
    cursor.setPos(5, topY + (cursorRow * 16 - 4));
    cursor.update();
}

void ChoiceListWindow::draw() {
    if (cursorVisible) {
        cursor.draw();
    }
}

BOOL ChoiceListWindow::isCursorPointing() {
    if (cursor.getAnim() == 7) return TRUE;
    return FALSE;
}

BOOL ChoiceListWindow::isCursorSelectDone() {
    if (cursor.getAnim() == 8 && cursor.isAnimDone()) return TRUE;
    return FALSE;
}

void ChoiceListWindow::startCursorAppear() {
    cursorRow = 0;
    cursor.setOrigin(-scrollX, -scrollY);
    cursor.setAnim(7);
}

void ChoiceListWindow::hideCursor() { cursor.setAnim(0); }

void ChoiceListWindow::startCursorSelect() { cursor.setAnim(8); }

void ChoiceListWindow::setCursorRow(s32 v) { cursorRow = v; }

void ChoiceListWindow::resetBlink() { blinkCount = 0; }

BOOL ChoiceListWindow::blinkStep(s32 a) {
    u16 v = kChoiceBlinkColors[blinkCount];
    blinkCount++;
    BOOL r;
    if (blinkCount >= 5) r = TRUE; else r = FALSE;
    setRowColor(a, v);
    return r;
}

s32 ChoiceWindow::hitTestRow(s32 x, s32 y) {
    s32 res = -1;
    s32 i = 0;
    s32 left = 0x88 - scrollX;
    s32 right = left + width * 8;
    if (x >= left && x < right) {
        s32 top = topY + 0x50 - scrollY;
        for (; i < numRows; i++) {
            s32 bot = top + 0x10;
            if (y >= top && y < bot) {
                res = i;
                break;
            }
            top = bot;
        }
    }
    return res;
}

void ChoiceListWindow::setCursorVisible(u8 v, BOOL flag) {
    if (flag != 0 && cursorVisible == 0 && v != 0) Snd_PlaySe(0x3b);
    cursorVisible = v;
}

ChoiceSliderWindow::ChoiceSliderWindow() : okButton(1, 0) {
    state = 0;
    sliderFocusRequest = 0;
    knobGrabbed = 0;
    buttonFocusRequest = 0;
    pressRequest = 0;
    closeRequest = 0;
    cursorVisible = 0;
}

ChoiceSliderWindow::~ChoiceSliderWindow() {}

void ChoiceSliderWindow::update() {
    if (sliderFocusRequest) { sliderFocusRequest = 0; enterSliderFocus(); }
    if (buttonFocusRequest) { buttonFocusRequest = 0; enterButtonFocus(); }
    if (closeRequest) { closeRequest = 0; enterHidden(); }
    static void (ChoiceSliderWindow::*table[12])() = {
        &ChoiceSliderWindow::updateHidden, &ChoiceSliderWindow::updateSliderFocus, &ChoiceSliderWindow::updateGrabbing,
        &ChoiceSliderWindow::updateGrabbed, &ChoiceSliderWindow::updateReleasing, &ChoiceSliderWindow::updateReleased,
        &ChoiceSliderWindow::updateButtonFocus, &ChoiceSliderWindow::updatePressing, &ChoiceSliderWindow::updateButtonDown,
        &ChoiceSliderWindow::updateButtonUp, &ChoiceSliderWindow::updateCursorReturn, &ChoiceSliderWindow::updateDecided,
    };
    (this->*table[state])();
    s32 a, b, c, d;
    if (isSliderFocused()) {
        slider.getKnobAnimOffset(&a, &b);
        s32 e = slider.getKnobOffsetY();
        cursor.setPos(a + 0x10, b + 7 + e);
    } else if (isButtonFocused()) {
        okButton.getAnimOffset(&c, &d);
        cursor.setPos(c + 0x46, d + 0x4c);
    }
    okButton.setPos(0x3c, 0x44);
    cursor.update();
    okButton.update();
    slider.update();
}

void ChoiceSliderWindow::draw() {
    if (cursorVisible) cursor.draw();
    okButton.draw();
    slider.draw();
}

void ChoiceSliderWindow::openWidgets() {
    s32 y = -scrollY;
    s32 x = -scrollX;
    cursor.setOrigin(x, y);
    okButton.setOrigin(0, y);
    slider.setOrigin(x, y);
    LabelButtonText l;
    u8 v = 0x12;
    String_Load2d(&l, &v, 0);
    (&okButton)->setLabelText((MsgString *)(void *)(&l));
    slider.setPos(0x800);
    requestSliderFocus();
}

void ChoiceSliderWindow::close() { requestClose(); }

BOOL ChoiceSliderWindow::isKnobHeld() {
    if (knobGrabbed && slider.isKnobAnimDone()) return TRUE;
    return FALSE;
}

void ChoiceSliderWindow::setCursorVisible(u8 v, BOOL flag) {
    if (flag != 0 && cursorVisible == 0 && v != 0) Snd_PlaySe(0x3b);
    cursorVisible = v;
}

void ChoiceSliderWindow::setSliderPos(s32 v) { slider.setPos(v); }

s32 ChoiceSliderWindow::getKnobX() { return slider.getKnobX() + 0x80 - scrollX; }

s32 ChoiceSliderWindow::getKnobY() { return slider.getKnobY() + 0x60 - scrollY; }

s32 ChoiceSliderWindow::getTrackTopY() { return slider.getTrackTop() + 0x60 - scrollY; }

s32 ChoiceSliderWindow::getTrackBottomY() { return slider.getTrackBottom() + 0x60 - scrollY; }

BOOL ChoiceSliderWindow::isSliderFocused() {
    if ((u32)(state - 1) <= 4) return TRUE;
    return FALSE;
}

BOOL ChoiceSliderWindow::isButtonFocused() {
    if ((u32)(state - 6) <= 5) return TRUE;
    return FALSE;
}

s32 ChoiceSliderWindow::getState() { return state; }

void ChoiceSliderWindow::requestSliderFocus() { sliderFocusRequest = 1; }

void ChoiceSliderWindow::grabKnob() { knobGrabbed = 1; }

void ChoiceSliderWindow::releaseKnob() { knobGrabbed = 0; }

void ChoiceSliderWindow::requestButtonFocus() { buttonFocusRequest = 1; }

void ChoiceSliderWindow::requestPress() { pressRequest = 1; }

void ChoiceSliderWindow::requestClose() { closeRequest = 1; }

void ChoiceSliderWindow::setCursorAnim(s32 v) { cursor.setAnim(v); }

void ChoiceSliderWindow::enterHidden() {
    state = 0;
    setCursorAnim(0);
    slider.hideKnob();
    okButton.setState(0);
}

void ChoiceSliderWindow::updateHidden() {}

void ChoiceSliderWindow::enterSliderFocus() {
    state = 1;
    cursor.setOrigin(-scrollX, -scrollY);
    setCursorAnim(1);
    slider.showKnob();
    okButton.setState(1);
}

void ChoiceSliderWindow::updateSliderFocus() {
    if (knobGrabbed) enterGrabbing();
    else if (pressRequest) enterPressing();
}

void ChoiceSliderWindow::enterGrabbing() {
    state = 2;
    return setCursorAnim(2);
}

void ChoiceSliderWindow::updateGrabbing() {
    if (cursor.isAnimDone()) {
        if (cursorVisible) Snd_PlaySe(0x33);
        enterGrabbed();
    }
}

void ChoiceSliderWindow::enterGrabbed() {
    state = 3;
    return slider.startKnobGrab();
}

void ChoiceSliderWindow::updateGrabbed() {
    if (knobGrabbed == 0) enterReleasing();
}

void ChoiceSliderWindow::enterReleasing() {
    state = 4;
    return slider.startKnobRelease();
}

void ChoiceSliderWindow::updateReleasing() {
    if (slider.isKnobAnimDone()) enterReleased();
}

void ChoiceSliderWindow::enterReleased() {
    state = 5;
    slider.showKnob();
    setCursorAnim(3);
}

void ChoiceSliderWindow::updateReleased() {
    if (cursor.isAnimDone()) enterSliderFocus();
}

void ChoiceSliderWindow::enterButtonFocus() {
    state = 6;
    cursor.setOrigin(0, -scrollY);
    cursor.setAnim(7);
    slider.showKnob();
    okButton.setState(1);
}

void ChoiceSliderWindow::updateButtonFocus() {
    if (pressRequest) enterPressing();
}

void ChoiceSliderWindow::enterPressing() {
    state = 7;
    cursor.setOrigin(0, -scrollY);
    setCursorAnim(8);
}

void ChoiceSliderWindow::updatePressing() {
    if (cursor.isAnimDone()) {
        Snd_PlaySe(0x27);
        enterButtonDown();
    }
}

void ChoiceSliderWindow::enterButtonDown() {
    state = 8;
    return okButton.setState(2);
}

void ChoiceSliderWindow::updateButtonDown() {
    if (okButton.isAnimDone()) enterDecided();
}

void ChoiceSliderWindow::updateButtonUp() {
    if (okButton.isAnimDone()) enterCursorReturn();
}

void ChoiceSliderWindow::enterCursorReturn() {
    state = 10;
    setCursorAnim(9);
    okButton.setState(1);
}

void ChoiceSliderWindow::updateCursorReturn() {
    if (cursor.isAnimDone()) enterDecided();
}

void ChoiceSliderWindow::enterDecided() {
    state = 11;
    pressRequest = 0;
}

void ChoiceSliderWindow::updateDecided() { pressRequest = 0; }

BufferBmgReader::BufferBmgReader() : BmgReader(1) {
    buffer = 0;
    bufferSize = 0;
}

BufferBmgReader::~BufferBmgReader() {}

void BufferBmgReader::setBuffer(u8 *data, u32 size) {
    buffer = data;
    bufferSize = size;
}

void BufferBmgReader::clearBuffer() {
    buffer = 0;
    bufferSize = 0;
}

u8 *BufferBmgReader::getBuffer() { return buffer; }

u32 BufferBmgReader::getBufferSize() { return bufferSize; }

ChoiceString::ChoiceString() { clear(); }

ChoiceString::~ChoiceString() {}

u32 ChoiceString::capacity() { return 0x21; }

u8 *ChoiceString::data() { return unk_14; }

BOOL ChoiceString::loadFromBmg(const char *path, void *entry, BmgMsgAttr *out) {
    u8 *d = data();
    u32 s = capacity();
    BOOL r;
    sChoiceBmgReader.setBuffer(d, s);
    sChoiceBmgReader.open(path);
    r = sChoiceBmgReader.loadMessage((u8 *)entry);
    BmgMsgAttr_Copy(out, Bmg_GetMsgAttr(&sChoiceBmgReader));
    sChoiceBmgReader.close();
    sChoiceBmgReader.clearBuffer();
    return r;
}

ChoiceEntry::ChoiceEntry() : msgIndex(gU8None), value(gU8None) {
    clear();
}

ChoiceEntry::~ChoiceEntry() {}

void ChoiceEntry::clear() {
    msgIndex = gU8None;
    bmgName = NULL;
    text.clear();
    BmgMsgAttr_Clear(&attr);
    value = gU8None;
    MI_CpuFill8(name, 0, 0x1a);
    weight = 0;
    seType = 0;
}

ChoiceString *ChoiceEntry::getText() { return &text; }

u8 *ChoiceEntry::getValuePtr() { return &value; }

char *ChoiceEntry::getName() { return name; }

BmgMsgAttr *ChoiceEntry::getAttr() { return &attr; }

u8 *ChoiceEntry::getWeightPtr() { return &weight; }

s32 ChoiceEntry::getSeType() { return seType; }

void ChoiceEntry::setMsgIndex(const u8 *p) { msgIndex = *p; }

void ChoiceEntry::setBmgName(const void *p) { bmgName = p; }

void ChoiceEntry::setValue(const u8 *p) { value = *p; }

void ChoiceEntry::setName(const char *src) {
    name[0x19] = 0;
    strncpy(name, src, 0x19);
}

void ChoiceEntry::setSeType(s32 v) { seType = v; }

void ChoiceEntry::loadText() {
    char buf[0x40];
    Str_SPrintf(buf, "/script/%s/select/%s.bmg", bmgName ? "ENG" : "ENG", bmgName);
    text.loadFromBmg(buf, this, &attr);
}

ChoiceList::ChoiceList() : resultValue(gU8None) {
    clear();
}

ChoiceList::~ChoiceList() {}

void ChoiceList::reset(s32 a, s32 b) {
    clear();
    count = a;
    cancelIndex = b;
}

void ChoiceList::setEntry(s32 idx, const u8 *a, s32 b, const u8 *c, const char *d, s32 e) {
    ChoiceEntry *p = &entries[idx];
    p->setMsgIndex(a);
    p->setBmgName(Choice_GetBmgName(b));
    p->setValue(c);
    p->setSeType(e);
    if (d) {
        p->setName(d);
    }
}

void ChoiceList::loadTexts() {
    s32 i;
    for (i = 0; i < count; i++) {
        entries[i].loadText();
    }
}

void ChoiceList::clear() {
    clearResult();
    clearEntries();
}

void ChoiceList::clearResult() {
    result = -1;
    resultValue = gU8None;
    MI_CpuFill8(resultName, 0, 0x1a);
    resultText.clear();
    BmgMsgAttr_Clear(&resultAttr);
    sliderValue = 0;
}

void ChoiceList::clearEntries() {
    s32 i;
    for (i = 0; (u32)i < 5; i++) {
        entries[i].clear();
    }
    count = 0;
    cancelIndex = -1;
}

ChoiceEntry *ChoiceList::getEntry(s32 i) { return &entries[i]; }

ChoiceString *ChoiceList::getFirstText() { return getEntry(0)->getText(); }

ChoiceString *ChoiceList::getLastText() { return getEntry(4)->getText(); }

s32 ChoiceList::getCount() { return count; }

s32 ChoiceList::getCancelIndex() { return cancelIndex; }

s32 ChoiceList::getResult() { return result; }

s32 ChoiceList::getSliderValue() { return sliderValue; }

u8 *ChoiceList::getResultValue() { return &resultValue; }

char *ChoiceList::getResultName() { return resultName; }

ChoiceString *ChoiceList::getResultText() { return &resultText; }

BmgMsgAttr *ChoiceList::getResultAttr() { return &resultAttr; }

void ChoiceList::setCount(s32 v) { count = v; }

void ChoiceList::setCancelToLast() { cancelIndex = count - 1; }

void ChoiceList::pick(s32 idx) {
    ChoiceEntry *e = &entries[idx];
    result = idx;
    resultValue = *e->getValuePtr();
    resultText.copy(e->getText());
    char *src = e->getName();
    resultName[0x19] = 0;
    strncpy(resultName, src, 0x19);
    BmgMsgAttr_Copy(&resultAttr, BmgMsgAttr_Get(e->getAttr()));
}

void ChoiceList::pickBySliderPos(s32 arg) {
    u32 i;
    s32 v = (arg << 4) >> 12;
    s32 sum;
    s32 sel;
    ChoiceEntry *e;
    if (v >= 16) {
        v = 15;
    }
    sum = 0;
    sel = 3;
    for (i = 0; i < 4; i++) {
        sum += *entries[i].getWeightPtr();
        if (v < sum) {
            sel = i;
            break;
        }
    }
    e = &entries[sel];
    result = sel;
    resultValue = *e->getValuePtr();
    BmgMsgAttr_Copy(&resultAttr, BmgMsgAttr_Get(e->getAttr()));
    sliderValue = v;
}

const void *Choice_GetBmgName(u32 i) { return kChoiceBmgNames[i]; }

ChoiceListCursor::ChoiceListCursor(ChoiceWindow *window) {
    u32 i;
    row = 0;
    listWindow = window;
    maxWidth = 0;
    list = NULL;
    confirmDelay = 0;
    repeatTimer = 0;
    repeatDir = 0;
    for (i = 0; i < 5; i++) {
        labels[i] = NULL;
    }
}

ChoiceListCursor::~ChoiceListCursor() { freeLabels(); }

void ChoiceListCursor::setList(ChoiceList *p) {
    p->getCount();
    list = p;
}

s32 ChoiceListCursor::getCount() { return list->getCount(); }

s32 ChoiceListCursor::getRow() { return row; }

s32 ChoiceListCursor::getResult() { return list->getResult(); }

u32 ChoiceListCursor::getMaxWidth() { return maxWidth; }

void ChoiceListCursor::open() {
    list->clearResult();
    createLabels();
    row = 0;
    confirmDelay = 0;
    resetKeyRepeat();
    calcMaxWidth();
}

void ChoiceListCursor::showText() { redrawLabels(); }

void ChoiceListCursor::hideText() { clearLabels(); }

void ChoiceListCursor::close() {
    freeLabels();
    list->clearEntries();
    maxWidth = 0;
    list = NULL;
}

void ChoiceListCursor::update() {
    if (list) {
        if (list->getResult() < 0) {
            if (checkDeviceSwitch()) {
                resetKeyRepeat();
            } else {
                BOOL r = FALSE;
                if (Input_IsTouchMode()) {
                    if (updateTouch()) {
                        r = TRUE;
                    }
                } else if (Input_IsButtonMode()) {
                    if (updateKeys()) {
                        r = TRUE;
                    }
                }
                if (r) {
                    list->pick(row);
                }
            }
        }
    }
}

BOOL ChoiceListCursor::isTouchMode() { return Input_IsTouchMode(); }

void ChoiceListCursor::calcMaxWidth() {
    u32 count = list->getCount();
    u32 max = 0;
    u32 i;
    for (i = 0; i < count; i++) {
        if (labels[i]) {
            u32 v = labels[i]->getWidthInTiles();
            if (v > max) {
                max = v;
            }
        }
    }
    maxWidth = max;
}

void ChoiceListCursor::resetKeyRepeat() {
    repeatTimer = 0;
    repeatDir = 0;
}

void ChoiceListCursor::createLabels() {
    u32 count = list->getCount();
    u32 i;
    for (i = 0; i < count; i++) {
        ChoiceEntry *e = list->getEntry(i);
        u32 size = ChoiceWindow_GetLabelTile(i, count);
        ChoiceString *buf = e->getText();
        TextLabel *t = MsgTextLabel_CreateVram(size, 0xd, 2);
        if (t) {
            t->vramLoader = 1;
            t->textStart = buf->data();
            t->copyMode = 2;
            t->bgColor = 0xe;
            t->fgColor = i + 1;
            t->group = 2;
            t->requestClear(0);
            t->group = 0;
            labels[i] = t;
        }
    }
}

void ChoiceListCursor::redrawLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            labels[i]->requestRedraw();
        }
    }
}

void ChoiceListCursor::clearLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            labels[i]->requestClear(0);
        }
    }
}

void ChoiceListCursor::freeLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            MsgTextLabel_Destroy(labels[i]);
            labels[i] = NULL;
        }
    }
}

BOOL ChoiceListCursor::checkDeviceSwitch() {
    BOOL r = FALSE;
    if (Input_IsTouchMode()) {
        if (Input_IsAnyKeyTrig()) {
            Input_SetButtonMode();
            r = TRUE;
        }
    } else if (Input_IsButtonMode()) {
        if (Input_IsTouchTrig()) {
            Input_SetTouchMode();
            r = TRUE;
        }
    }
    return r;
}

BOOL ChoiceListCursor::updateTouch() {
    s32 a, b;
    BOOL r = FALSE;
    if (Input_GetTouchTrigPos(&a, &b)) {
        s32 res = listWindow->hitTestRow(a, b);
        if (res >= 0) {
            row = res;
            r = TRUE;
            s32 cur = list->getCancelIndex();
            if (cur >= 0 && cur == res) {
                Snd_PlaySe(0x2a);
            } else {
                playDecideSe();
            }
        }
    }
    return r;
}

BOOL ChoiceListCursor::updateKeys() {
    BOOL result = FALSE;
    s32 cur = list->getCancelIndex();
    u16 pressed, held;
    if (confirmDelay > 0) {
        confirmDelay--;
        if (confirmDelay <= 0) {
            result = TRUE;
            Input_Unlock();
            goto end;
        }
    }
    pressed = gPad[1];
    if (pressed & 1) {
        if (cur >= 0 && cur == row) {
            Snd_PlaySe(0x2a);
        } else {
            playDecideSe();
        }
        result = TRUE;
        Input_Unlock();
        goto end;
    }
    if (cur >= 0 && (pressed & 2)) {
        Snd_PlaySe(0x2a);
        row = list->getCancelIndex();
        confirmDelay = 1;
        Input_Lock();
        goto end;
    }
    held = gPad[0];
    BOOL heldUp;
    heldUp = (held & 0x40) != 0;
    BOOL heldDown;
    heldDown = (held & 0x80) != 0;
    BOOL pressUp;
    pressUp = (pressed & 0x40) != 0;
    BOOL pressDown;
    pressDown = (pressed & 0x80) != 0;
    s32 last;
    last = list->getCount() - 1;
    s32 old;
    old = row;
    BOOL moved;
    moved = FALSE;
    s32 state;
    state = repeatDir;
    if (state == 0 && (heldUp || heldDown) && (pressUp || pressDown)) {
        moved = TRUE;
        Input_Lock();
        repeatTimer = 9;
        repeatDir = heldUp ? -1 : 1;
    } else if ((state > 0 && heldDown) || (state < 0 && heldUp)) {
        if (repeatTimer > 0) {
            repeatTimer--;
            if (repeatTimer <= 0) {
                moved = TRUE;
                repeatTimer = 3;
            }
        }
    } else {
        if (state != 0) {
            Input_Unlock();
        }
        repeatDir = 0;
        repeatTimer = 0;
    }
    if (moved) {
        row += repeatDir;
    }
    if (row < 0) {
        row = last;
    } else if (row > last) {
        row = 0;
    }
    if (old != row) {
        Snd_PlaySe(11);
    }
end:
    return result;
}

void ChoiceListCursor::playDecideSe() {
    s32 t = list->getEntry(row)->getSeType();
    s32 id = 0x29;
    if (t == 1) {
        id = 0x3a;
    } else if (t == 2) {
        id = 0x6a;
    } else if (t == 3) {
        id = 0x6c;
    }
    Snd_PlaySe(id);
}

ChoiceSliderCursor::ChoiceSliderCursor(ChoiceSliderWindow *window) {
    u32 i;
    sliderWindow = window;
    speed = 0;
    sliderPos = 0x800;
    prevPos = 0x800;
    grabOffsetY = 0;
    maxWidth = 0;
    focus = 0;
    grabbing = 0;
    confirmedByStart = 0;
    list = NULL;
    for (i = 0; i < 5; i++) {
        labels[i] = NULL;
    }
}

ChoiceSliderCursor::~ChoiceSliderCursor() {
    freeLabels();
}

void ChoiceSliderCursor::setList(ChoiceList *p) {
    p->getCount();
    list = p;
}

s32 ChoiceSliderCursor::getPos() {
    return sliderPos;
}

u32 ChoiceSliderCursor::getMaxWidth() {
    return maxWidth;
}

s32 ChoiceSliderCursor::getResult() {
    return list->getResult();
}

void ChoiceSliderCursor::open() {
    list->clearResult();
    createLabels();
    speed = 0;
    sliderPos = 0x800;
    prevPos = 0x800;
    focusSlider();
    calcMaxWidth();
    confirmedByStart = 0;
}

void ChoiceSliderCursor::showText() {
    redrawLabels();
}

void ChoiceSliderCursor::hideText() {
    clearLabels();
}

void ChoiceSliderCursor::close() {
    freeLabels();
    list->clearEntries();
    maxWidth = 0;
    list = NULL;
}

void ChoiceSliderCursor::update() {
    if (!checkDeviceSwitch()) {
        static void (ChoiceSliderCursor::*tbl[2])() = {
            &ChoiceSliderCursor::updateSlider,
            &ChoiceSliderCursor::updateOkButton,
        };
        updateGrab();
        updateFocus();
        (this->*tbl[focus])();
    }
}

BOOL ChoiceSliderCursor::isSliderFocused() {
    if (focus == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceSliderCursor::isOkButtonFocused() {
    if (focus == 1) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceSliderCursor::isGrabbing() {
    if (isSliderFocused() && grabbing) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceSliderCursor::isCursorHidden() {
    if (Input_IsTouchMode() || confirmedByStart) {
        return TRUE;
    }
    return FALSE;
}

void ChoiceSliderCursor::calcMaxWidth() {
    u32 m = 0;
    u32 w;
    if (labels[4]) {
        w = labels[4]->getWidthInTiles();
        if (w > m) {
            m = w;
        }
    }
    if (labels[0]) {
        w = labels[0]->getWidthInTiles();
        if (w > m) {
            m = w;
        }
    }
    maxWidth = m;
}

void ChoiceSliderCursor::createLabels() {
    u32 i;
    s32 zero = 0;
    for (i = 0; i < 5; i++) {
        ChoiceEntry *a = list->getEntry(i);
        s32 font = ChoiceWindow_GetLabelTile(i, 5);
        ChoiceString *text = a->getText();
        TextLabel *o = MsgTextLabel_CreateVram(font, 13, 2);
        if (o) {
            o->vramLoader = 1;
            o->textStart = text->data();
            o->copyMode = 2;
            o->bgColor = 14;
            o->fgColor = i + 1;
            o->group = 2;
            o->requestClear(zero);
            o->group = 0;
            labels[i] = o;
        }
    }
}

void ChoiceSliderCursor::redrawLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            labels[i]->requestRedraw();
        }
    }
}

void ChoiceSliderCursor::clearLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            labels[i]->requestClear(0);
        }
    }
}

void ChoiceSliderCursor::freeLabels() {
    u32 i;
    for (i = 0; i < 5; i++) {
        if (labels[i]) {
            MsgTextLabel_Destroy(labels[i]);
            labels[i] = NULL;
        }
    }
}

BOOL ChoiceSliderCursor::checkDeviceSwitch() {
    BOOL r = FALSE;
    if (Input_IsTouchMode()) {
        if (Input_IsAnyKeyTrig()) {
            Input_SetButtonMode();
            r = TRUE;
        }
    } else if (Input_IsButtonMode()) {
        if (Input_IsTouchTrig()) {
            Input_SetTouchMode();
            r = TRUE;
        }
    }
    return r;
}

void ChoiceSliderCursor::updateFocus() {
    if (Input_IsTouchMode()) {
        if (focus == 0) {
            if (isOkButtonTapped()) {
                focusOkButton();
            }
        } else if (focus == 1) {
            if (grabbing) {
                focusSlider();
            }
        }
    } else if (Input_IsButtonMode()) {
        if (focus == 0) {
            if (!grabbing) {
                BOOL a = Input_IsDownTrig();
                BOOL b = Input_IsStartTrig();
                if (a || b) {
                    if (a) {
                        Snd_PlaySe(11);
                    }
                    focusOkButton();
                }
            }
        } else if (focus == 1) {
            if (Input_IsUpTrig()) {
                Snd_PlaySe(11);
                focusSlider();
            }
        }
    }
}

void ChoiceSliderCursor::updateGrab() {
    BOOL r;
    if (grabbing) {
        r = FALSE;
        if (Input_IsTouchMode()) {
            if (isTouchReleased()) {
                r = TRUE;
            }
        } else if (Input_IsButtonMode()) {
            if (isGrabKeyReleased()) {
                r = TRUE;
            }
        }
        if (r) {
            grabbing = 0;
            Input_Unlock();
        }
    } else {
        r = FALSE;
        if (Input_IsTouchMode()) {
            if (isKnobTouched()) {
                r = TRUE;
            }
        } else if (Input_IsButtonMode()) {
            if (isGrabKeyPressed()) {
                r = TRUE;
            }
        }
        if (r) {
            grabbing = 1;
            Input_Lock();
        }
    }
}

BOOL ChoiceSliderCursor::isKnobTouched() {
    BOOL r = FALSE;
    s32 a = sliderWindow->getKnobX();
    s32 b = sliderWindow->getKnobY();
    s32 x, y;
    if (Input_IsTouchTrigInRect(a - 5, a + 21, b, b + 16)) {
        Input_GetTouchTrigPos(&x, &y);
        grabOffsetY = y - b;
        r = TRUE;
    }
    return r;
}

BOOL ChoiceSliderCursor::isGrabKeyPressed() {
    if (focus == 0 && Input_IsATrig()) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceSliderCursor::isTouchReleased() {
    if (Input_GetTouchHeldPos(0, 0)) {
        return FALSE;
    }
    return TRUE;
}

BOOL ChoiceSliderCursor::isGrabKeyReleased() {
    if (Input_IsAHeld()) {
        return FALSE;
    }
    return TRUE;
}

BOOL ChoiceSliderCursor::isOkButtonTapped() {
    BOOL r = FALSE;
    if (Input_IsTouchTrigInRect(184, 256, 160, 176)) {
        r = TRUE;
    }
    return r;
}

BOOL ChoiceSliderCursor::isConfirmPressed() {
    BOOL r = FALSE;
    if (focus == 1) {
        if (Input_IsStartTrig()) {
            r = TRUE;
            confirmedByStart = TRUE;
        } else if (Input_IsATrig()) {
            r = TRUE;
        }
    }
    return r;
}

void ChoiceSliderCursor::focusSlider() {
    focus = 0;
}

void ChoiceSliderCursor::updateSlider() {
    s32 x, y, v, hi, lo;
    if (grabbing) {
        if (sliderWindow->isKnobHeld()) {
            if (Input_IsTouchMode()) {
                if (Input_GetTouchHeldPos(&x, &y)) {
                    lo = sliderWindow->getTrackBottomY();
                    hi = sliderWindow->getTrackTopY();
                    v = y - grabOffsetY;
                    if (v < hi) {
                        v = hi;
                    } else if (v > lo) {
                        v = lo;
                    }
                    sliderPos = _s32_div_f((v - lo) << 12, hi - lo);
                }
                speed = 0;
            } else if (Input_IsButtonMode()) {
                BOOL up = Input_IsUpHeld();
                BOOL down = Input_IsDownHeld();
                if (!up && !down) {
                    speed = 40;
                } else {
                    speed += 81;
                    s32 sp = speed;
                    if (sp < 0) {
                        sp = 0;
                    } else if (sp > 409) {
                        sp = 409;
                    }
                    speed = sp;
                }
                if (up) {
                    sliderPos += speed;
                }
                if (down) {
                    sliderPos -= speed;
                }
                s32 pos = sliderPos;
                if (pos < 0) {
                    pos = 0;
                } else if (pos > 0x1000) {
                    pos = 0x1000;
                }
                sliderPos = pos;
            }
        }
    }
    s32 d = sliderPos - prevPos;
    if (d < 0) {
        d = -d;
    }
    if (d >= 81) {
        BOOL inc = sliderPos > prevPos;
        s32 hi = inc ? sliderPos : prevPos;
        s32 lo = inc ? prevPos : sliderPos;
        BOOL found = FALSE;
        s32 i;
        for (i = 0; i < 10; i++) {
            s32 v = i * 409;
            if (v >= lo && v < hi) {
                found = TRUE;
                break;
            }
        }
        if (hi == 0x1000) {
            found = TRUE;
        }
        if (found) {
            Snd_PlaySe(25);
        }
    }
    prevPos = sliderPos;
}

void ChoiceSliderCursor::focusOkButton() {
    focus = 1;
}

void ChoiceSliderCursor::updateOkButton() {
    if (list) {
        if (list->getResult() < 0) {
            if ((Input_IsTouchMode() && isOkButtonTapped()) || (Input_IsButtonMode() && isConfirmPressed())) {
                list->pickBySliderPos(sliderPos);
            }
        }
    }
}

ChoiceDialog::ChoiceDialog() {
    openRequest = 0;
}

ChoiceDialog::~ChoiceDialog() {}

void ChoiceDialog::requestOpen() {
    openRequest = 1;
}

void ChoiceListDialog::setChoices(ChoiceList *p) {
    cursor.setList(p);
}

ChoiceListDialog::ChoiceListDialog() : state(0), cursor(&window) {
    scrollX = 0;
    scrollY = 0;
    slideTimer = 0;
    decided = 0;
}

ChoiceListDialog::~ChoiceListDialog() {}

void ChoiceListDialog::update() {
    static void (ChoiceListDialog::*tbl[4])() = {
        &ChoiceListDialog::updateClosed,
        &ChoiceListDialog::updateOpening,
        &ChoiceListDialog::updateSelecting,
        &ChoiceListDialog::updateClosing,
    };
    (this->*tbl[state])();
    window.update();
}

void ChoiceListDialog::draw() {
    window.draw();
    window.flushRowColors();
}

BOOL ChoiceListDialog::isClosed() {
    if (state == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL ChoiceListDialog::justDecided() {
    return decided;
}

void ChoiceListDialog::enterClosed() {
    state = 0;
}

void ChoiceListDialog::updateClosed() {
    if (openRequest) {
        openRequest = 0;
        enterOpening();
    }
}

void ChoiceListDialog::enterOpening() {
    state = 1;
    window.loadBg();
    cursor.open();
    layoutWindow();
    window.setupBgControl();
    window.uploadBg();
    window.showLayer();
    window.freeBg();
    slideTimer = DebugVar(100, 2) + 3;
    scrollX = DebugVar(100, 4) + 5;
    scrollY = DebugVar(100, 5) - 5;
    window.setScroll(scrollX, scrollY);
    window.setAllRowColors();
}

void ChoiceListDialog::updateOpening() {
    BOOL done;
    if (slideTimer > DebugVar(0x64, 3) + 2) {
        scrollX += DebugVar(0x64, 6) - 6;
        scrollY += DebugVar(0x64, 7) + 6;
    } else {
        scrollX += DebugVar(0x64, 8) + 1;
        scrollY += DebugVar(0x64, 9) - 1;
    }
    done = --slideTimer <= 0;
    if (done) {
        scrollX = 0;
        scrollY = 0;
    }
    window.setScroll(scrollX, scrollY);
    if (done) {
        cursor.showText();
        enterSelecting();
    }
}

void ChoiceListDialog::enterSelecting() {
    state = 2;
    window.startCursorAppear();
    window.setCursorVisible(!cursor.isTouchMode(), FALSE);
}

void ChoiceListDialog::updateSelecting() {
    s32 v;
    cursor.update();
    window.setCursorVisible(!cursor.isTouchMode(), TRUE);
    window.setCursorRow(cursor.getRow());
    v = cursor.getResult();
    if (v >= 0) {
        if (window.isCursorPointing()) {
            window.startCursorSelect();
            window.resetBlink();
        } else if (window.isCursorSelectDone()) {
            if (window.blinkStep(v)) {
                window.hideCursor();
                cursor.hideText();
                enterClosing();
            }
        }
    }
}

void ChoiceListDialog::enterClosing() {
    state = 3;
    decided = 1;
    slideTimer = DebugVar(0x65, 2) + 2;
    Snd_PlaySe(0x14);
}

void ChoiceListDialog::updateClosing() {
    BOOL done;
    scrollX += DebugVar(0x65, 3) + 0x11;
    scrollY += DebugVar(0x65, 4) - 0x11;
    done = --slideTimer <= 0;
    window.setScroll(scrollX, scrollY);
    decided = 0;
    if (done) {
        cursor.close();
        window.hideLayer();
        enterClosed();
    }
}

void ChoiceListDialog::layoutWindow() {
    s32 t = cursor.getCount();
    window.setWidth(cursor.getMaxWidth());
    if (t == 1) {
        window.layout1Row();
    } else if (t == 2) {
        window.layout2Rows();
    } else if (t == 3) {
        window.layout3Rows();
    } else if (t == 4) {
        window.layout4Rows();
    } else if (t == 5) {
        window.layout5Rows();
    }
}

void ChoiceSliderDialog::setChoices(ChoiceList *p) {
    cursor.setList(p);
}

ChoiceSliderDialog::ChoiceSliderDialog() : state(0), cursor(&window) {
    scrollX = 0;
    scrollY = 0;
    slideTimer = 0;
    decided = 0;
}

ChoiceSliderDialog::~ChoiceSliderDialog() {}

void ChoiceSliderDialog::update() {
    static void (ChoiceSliderDialog::*const table[4])() = {
        &ChoiceSliderDialog::updateClosed,
        &ChoiceSliderDialog::updateOpening,
        &ChoiceSliderDialog::updateSelecting,
        &ChoiceSliderDialog::updateClosing,
    };
    (this->*table[state])();
    window.update();
}

void ChoiceSliderDialog::draw() {
    window.draw();
    window.flushRowColors();
}

BOOL ChoiceSliderDialog::isClosed() {
    return state == 0;
}

BOOL ChoiceSliderDialog::justDecided() {
    return decided;
}

void ChoiceSliderDialog::enterClosed() {
    state = 0;
    window.hideLayer();
}

void ChoiceSliderDialog::updateClosed() {
    if (openRequest) {
        openRequest = 0;
        enterOpening();
    }
}

void ChoiceSliderDialog::enterOpening() {
    state = 1;
    window.loadBg();
    cursor.open();
    layoutWindow();
    window.setupBgControl();
    window.uploadBg();
    window.showLayer();
    window.freeBg();
    slideTimer = DebugVar(0x64, 2) + 3;
    scrollX = DebugVar(0x64, 4) + 5;
    scrollY = DebugVar(0x64, 5) - 5;
    window.setScroll(scrollX, scrollY + 4);
    window.setAllRowColors();
    Snd_PlaySe(0x13);
}

void ChoiceSliderDialog::updateOpening() {
    BOOL done;
    if (slideTimer > DebugVar(0x64, 3) + 2) {
        scrollX += DebugVar(0x64, 6) - 6;
        scrollY += DebugVar(0x64, 7) + 6;
    } else {
        scrollX += DebugVar(0x64, 8) + 2;
        scrollY += DebugVar(0x64, 9) - 2;
    }
    done = --slideTimer <= 0;
    if (done) {
        scrollX = 0;
        scrollY = 0;
    }
    window.setScroll(scrollX, scrollY + 4);
    if (done) {
        window.openWidgets();
        window.setCursorVisible(!cursor.isCursorHidden(), FALSE);
        cursor.showText();
        enterSelecting();
    }
}

void ChoiceSliderDialog::enterSelecting() {
    state = 2;
}

void ChoiceSliderDialog::updateSelecting() {
    s32 v;
    s32 w;
    cursor.update();
    window.setCursorVisible(!cursor.isCursorHidden(), TRUE);
    window.setSliderPos(cursor.getPos());
    v = cursor.getResult();
    w = window.getState();
    if (v >= 0) {
        if (w == 0xb) {
            window.close();
            cursor.hideText();
            enterClosing();
        } else {
            window.requestPress();
        }
    } else if (cursor.isSliderFocused()) {
        if (window.isButtonFocused()) {
            window.requestSliderFocus();
        } else if (cursor.isGrabbing()) {
            window.grabKnob();
        } else {
            window.releaseKnob();
        }
    } else if (cursor.isOkButtonFocused()) {
        if (window.isSliderFocused()) {
            window.requestButtonFocus();
        }
    }
}

void ChoiceSliderDialog::enterClosing() {
    state = 3;
    decided = 1;
    slideTimer = DebugVar(0x65, 2) + 2;
    Snd_PlaySe(0x14);
}

void ChoiceSliderDialog::updateClosing() {
    BOOL done;
    decided = 0;
    scrollX += DebugVar(0x65, 3) + 0xb;
    scrollY += DebugVar(0x65, 4) - 0xb;
    done = --slideTimer <= 0;
    window.setScroll(scrollX, scrollY);
    if (done) {
        cursor.close();
        enterClosed();
    }
}

void ChoiceSliderDialog::layoutWindow() {
    window.setWidth(cursor.getMaxWidth());
    window.layout5Rows();
}

void ChoiceMenu::setListChoices(ChoiceList *p) {
    listDialog.setChoices(p);
    active = &listDialog;
}

void ChoiceMenu::setSliderChoices(ChoiceList *p) {
    sliderDialog.setChoices(p);
    active = &sliderDialog;
}

ChoiceMenu::ChoiceMenu() : active(0) {
    closedThisFrame = 0;
}

ChoiceMenu::~ChoiceMenu() {}

void ChoiceMenu::update() {
    if (active != 0) {
        active->update();
        if (active->isClosed()) {
            active = 0;
            closedThisFrame = 1;
        }
    } else {
        closedThisFrame = 0;
    }
}

void ChoiceMenu::open() {
    if (active != 0) {
        active->requestOpen();
    }
}

BOOL ChoiceMenu::isIdle() {
    return active == 0;
}

BOOL ChoiceMenu::justDecided() {
    BOOL r = FALSE;
    if (active != 0) {
        r = active->justDecided();
    }
    return r;
}

void ChoiceMenu::draw() {
    if (active != 0) {
        active->draw();
    }
}

void MsgCallStack::pop() {
    if (depth != 0) {
        depth--;
        entries[depth] = NULL;
    }
}

u8 **MsgCallStack::top() {
    return &entries[depth - 1];
}

void MsgCallStack::push(u8 *const &value) {
    if (depth < 6) {
        u32 i = depth++;
        entries[i] = value;
    }
}

BOOL MsgCallStack::isEmpty() {
    return depth == 0;
}

MsgCallStack::~MsgCallStack() {}

MsgCallStack::MsgCallStack() {
    depth = 0;
    for (u32 i = 0; i < 6; i++) {
        entries[i] = NULL;
    }
}


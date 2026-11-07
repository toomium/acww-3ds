// ov121: scene overlay (class DesignTab, vtable 0x02294d68, size 0x107c). Linked unit.
#include "types.h"
#include "host_shared_calls.h"
#include "Unk_020d8c7c.h"
#include "ui/UiWidget.h"
#include "menu/MenuProc.h"
#include "ui/HandCursor.h"
#include "ui/LabelBalloon.h"
#include "gfx/BgVramTask.h"
#include "ui/TouchPromptBalloon.h"
#include "menu/MenuCursor.h"
#include "menu/PopupChoiceMenu.h"
#include "menu/MenuErrorMessage.h"
#include "sys/ProcProfile.h"
#include "net/CommManager.h"
#include "nitro/gxoam.h"
#include "menu/MenuTabBar.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
// the options button as a hand target in the tab row, left of the pockets' tab (0x18 is "none"): a tab target whose tab
// is N3DS_OPT_TAB
#define DESIGN_OPT_SLOT 0x19
#endif
#include "player/PlayerData.h"
#include "save/Pattern.h"
#include "save/PatternOrder.h"
#include "talk/EncodedString16Buf.h"
#include "talk/LabelBalloonText.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02034048.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206db88.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_020501d4.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_020b0e60.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov004/unk_ov004_0222a500.h"
#include "proto/ov004/unk_ov004_02233074.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov121/unk_ov121_02292360.h"


extern "C" {
extern CommManager *gCommManager;
extern u8 gU8None;
extern u8 gFieldSceneKind;
extern u16 gPad[];

s32 FieldAction_RequestDrop(s32 a, u16 b);
s32 FieldAction_PollDrop(s32 idx);
void FieldAction_Release(s32 i);
void MI_CpuCopy8(const void *a, void *b, u32 n);
BOOL Camera_IsViewPushed();
BOOL Camera_PopView();
BOOL Camera_PushView();
void MIi_CpuCopy16(const void *src, void *dst, u32 n);
BOOL Scene_InHouseRoom();
// FtrMgr_FindPlacementMyDesign* (src/ov004/unk_ov004_02233074.cpp, defined by a macro)
s32 FtrMgr_FindPlacementMyDesignA(u32 *out, u32 a, u32 b);
s32 FtrMgr_FindPlacementMyDesignD(u32 *out, u32 a, u32 b);
s32 FtrMgr_FindPlacementMyDesignC(u32 *out, u32 a, u32 b);
s32 FtrMgr_FindPlacementMyDesignB(u32 *out, u32 a, u32 b);

// the popup choice menu's functions (src/ov002/unk_ov002_022013ac.cpp; they take the menu first)
u8 PopupChoice_DecideCancel(PopupChoiceMenu *p, s32 v);
void PopupChoice_DecideRow(PopupChoiceMenu *p, s32 a, s32 b);
void PopupChoice_ForceClose(PopupChoiceMenu *p);
void PopupChoice_Draw(PopupChoiceMenu *p);
void PopupChoice_Update(PopupChoiceMenu *p);
void PopupChoice_LoadChoiceBg(PopupChoiceMenu *p);
void PopupChoice_Close(PopupChoiceMenu *p, s32 a);
void PopupChoice_Open(PopupChoiceMenu *p, s32 a);
BOOL PopupChoice_TickDecideDelay(PopupChoiceMenu *p);
BOOL PopupChoice_MoveCursor(PopupChoiceMenu *p, s32 a, u8 *pos, u32 n);

extern const u8 sDesignTabTabDownSlot[];
extern const u16 sDesignTabTargetTileX[];
extern const u16 sDesignTabTargetTileY[];
extern const u8 sDesignTabSlotY[];
extern const u8 sDesignTabSlotX[];
extern const u8 sDesignTabPopupChoices[][0xb];
}

// ---- external classes (real names from symbols.txt), sized for the sub-objects ----













// Vtable 0x02294d68, size 0x107c
class DesignTab : public MenuProc {
public:
    DesignTab() : nameBalloon(), cursor(), errorMessage(), popup(), bgTasks() {}

    virtual BOOL onCreate();
    virtual BOOL onDelete();
    virtual BOOL onDraw();
    virtual BOOL execTransition();
    virtual BOOL execMain();
    virtual BOOL execPhase3();
    virtual BOOL execPhase4();
    virtual BOOL execClosed();

    void clearFlags(u32 mask);
    void setFlags(u32 mask);
    BOOL testFlags(u32 mask);
    BOOL isRoomEditAllowed();
    BOOL isWearDone(s32 k);
    BOOL requestWear(s32 k, u32 v);
    u32 swapWornItem(s32 k, u32 v);
    u32 getWornItem(s32 k);
    void updateTargetBlink();
    void startTargetBlink(u32 v);
    BOOL moveCursorByPad(u32 pad, u32 f);
    void moveCursorVertical(u32 pad, u32 f);
    void moveCursorFromTab(u32 idx);
    void moveCursorToTabs();
    BOOL moveCursorHorizontal(u32 pad, u32 f);
    s32 getCursorTab();
    BOOL cursorRowDown(u32 lo, u32 hi, u32 v);
    BOOL cursorRowUp(u32 lo, u32 hi, u32 v);
    BOOL stepCursorLeft(u32 lo, u32 hi);
    BOOL stepCursorRight(u32 lo, u32 hi);
    void moveDragIconToCursor();
    void cancelCarry();
    void dropCarry();
    void startCarry();
    void releaseCursor();
    void pressCursor();
    void refreshCursor();
    void showCursorAtTarget();
    void cursorToPopupTop();
    void cancelPopup();
    void moveCursorToPopupRow();
    void moveCursorToTarget();
    void hideCursor();
    s32 getCursorTargetY();
    s32 getCursorTargetX();
    void showCursor();
    void updateNameLabel();
    void refreshNameLabel();
    void restoreCamera();
    void pushCamera();
    void abortPopup();
    void openPopup();
    void onPopupChoice();
    void placeDesignItem();
    void placeDesignInRoom();
    void equipDesign(u32 m);
    void applyRoomDesignB(u32 m);
    void applyRoomDesignA(u32 m);
    void dropOnPlayerFigure();
    u32 getPopupRowValue(u32 i);
    void setPopupChoices(u32 i);
    u32 getSlotPattern(u32 i);
    void swapPatternSlots(u32 a, u32 b);
    BOOL dropHeldOnTarget();
    void moveDragIconToTouch();
    BOOL hasDragStarted();
    BOOL isTargetDisabled(u32 i);
    void disableTarget(u32 i);
    u32 findDropTarget();
    u32 findTouchedSlot();
    u32 findSlotAt(u32 x, u32 y, u32 n);
    void highlightTarget(u32 idx);
    void setTargetPalette(u32 id, u32 s);
    u32 getSlotY(u32 i);
    s32 getSlotX(u32 i);
    void openMessageWindow(u32 a, u32 b);
    void resumeInput();
    void startButtonInput();
    void startTouchInput();
    void updateItemPlace();
    void updateMessage();
    void updateWearWait();
    void updateWearRequest();
    void updatePopupDone();
    void updatePopupClose();
    void updatePopupOpen();
    void updatePopupPress();
    void updatePopupButtons();
    void updateDrop();
    void updateCarryMove();
    void updateGrab();
    void updateCursorRelease();
    void updateCursorPress();
    void updateCursorMove();
    void updateCarry();
    void updateButtons();
    void updatePopupTouch();
    void updateTouchDrag();
    void updateTouchHold();
    void updateTouch();
    void loadPatternIcons();
    void loadBgGfx();
    void setupBgLayers();
    void postStateUpdate();
    void preStateUpdate();
    void postInputUpdate();
    void preInputUpdate();
    void releaseResources();
    BOOL canDecorateRoom();
    void initDesignTab();
    void stateClosing();
    void stateClose();
    void stateOpening();
    void stateOpen();
    BOOL requestTab(s32 a);
    BOOL handleTabSwitch();
    void runMainState();

    /* 0x91 */ u8 unk_91[3];
    /* 0x94 */ s32 slideY;
    /* 0x98 */ s32 dropRequest;
    /* 0x9c */ u32 disabledTargets;
    /* 0xa0 */ u16 designItem;
    /* 0xa2 */ u16 flags;
    /* 0xa4 */ s16 handX;
    /* 0xa6 */ s16 handY;
    /* 0xa8 */ s16 dragOffsetX;
    /* 0xaa */ s16 dragOffsetY;
    /* 0xac */ u8 roomMode;
    /* 0xad */ u8 balloonSlot;
    /* 0xae */ u8 heldSlot;
    /* 0xaf */ u8 targetSlot;
    /* 0xb0 */ u8 highlightedTarget;
    /* 0xb1 */ u8 cursorSlot;
    /* 0xb2 */ u8 popupChoice;
    /* 0xb3 */ u8 popupRow;
    /* 0xb4 */ u8 returnState;
    /* 0xb5 */ u8 blinkTarget;
    /* 0xb6 */ u8 blinkTimer;
    /* 0xb7 */ u8 equipPart;
    /* 0xb8 */ u16 clothPalette[0x10];
    /* 0xd8 */ u8 clothImage[0x2d8 - 0xd8];
    /* 0x2d8 */ TouchPromptBalloon nameBalloon;
    /* 0x398 */ MenuCursorBuf0 cursor;
    /* 0x3fc */ MenuErrorMessage errorMessage;
    /* 0x504 */ PopupChoiceMenu popup;
    /* 0x7f8 */ PopupChoiceIdList choiceList;
    /* 0x803 */ u8 pad_803[1];
    /* 0x804 */ BgVramTaskPair bgTasks[2];
    /* 0x874 */ u16 mainScreen[0x400];
    /* 0x1074 */ u8 slotIcons[8];
};

typedef void (DesignTab::*Unk_ov121_02294d68_Fn)();


static inline BOOL Unk_ov121_02293188_InRange(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 a = *p;
    u32 b = *p;
    if (b >= lo && a <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_ov121_022924e0_Range(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 a = *p;
    u32 b = *p;
    if (b >= lo && a <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_ov121_02293f34_Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) {
        return TRUE;
    }
    return FALSE;
}

extern "C" DesignTab *DesignTab_Create() { return new DesignTab(); }

BOOL DesignTab::onCreate() {
    initDesignTab();
    setTransitionState(0);
    setPhase(1);
    return TRUE;
}

BOOL DesignTab::onDelete() {
    ((MenuTabBar *)ProcBase_GetParent(this))->onTabMenuClosed();
    releaseResources();
    return TRUE;
}

BOOL DesignTab::onDraw() {
    s32 p = slideY;
    u32 i;
    s32 j;
    s32 *zero = 0;
    if (!testFlags(1)) {
        return TRUE;
    }
    nameBalloon.draw();
    if (MenuCtrl_IsButtons()) {
        ((MenuCursorBase *)(&cursor))->drawWrapped();
    }
    PopupChoice_Draw(&popup);
    if (testFlags(4)) {
        if (MenuCtrl_IsButtons()) {
            moveDragIconToCursor();
        } else {
            moveDragIconToTouch();
        }
        u8 *q = slotIcons;
        u8 s = heldSlot;
        sDesignTabIconCell.attr2 = (sDesignTabIconCell.attr2 & 0xfffffc00) | (u16)(q[s] * 4 + 0xc0) & 0x3ff;
        Oam_DrawObj(1, &sDesignTabIconCell, handX, handY, q[s] + 4, 2, 0);
    }
    if (testFlags(8)) {
        u8 k = targetSlot;
        s32 y = p + getSlotY(k);
        s32 x = getSlotX(k);
        Oam_DrawCell(1, (GXOamAttr *)sDesignTabTargetFrameCells, x, y, -1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
    }
    i = 0;
    for (j = 0; j < 8; j++) {
        if (testFlags(4) && i == heldSlot) {
        } else {
            u8 icon = slotIcons[j];
            sDesignTabIconCell.attr2 = (sDesignTabIconCell.attr2 & 0xfffffc00) | (u16)(icon * 4 + 0xc0) & 0x3ff;
            s32 y, pal;
            pal = icon + 4;
            y = p + getSlotY(i);
            s32 x = getSlotX(i);
            Oam_DrawObj(1, &sDesignTabIconCell, x, y, pal, 2, zero);
        }
        i = (u8)(i + 1);
    }
    return TRUE;
}

// Scene registration entry read by main: factory, then two ids
extern "C" ProcProfile sDesignTabProfile = {(void *(*)())DesignTab_Create, 0xa4, 0xa8};

extern "C" GXOamAttr sDesignTabIconCell = {0x81f000f0, 0xc0, 0, 0x4, 0xffff};

extern "C" u32 sDesignTabTargetFrameCells[8] = {0x41ee00ee, 0x0000c140, 0x500200ee, 0x0000c140, 0x70020002, 0x0000c140, 0x61ee0002, 0xffffc140};

BOOL DesignTab::execTransition() {
    static Unk_ov121_02294d68_Fn tbl[4] = {
        &DesignTab::stateOpen, &DesignTab::stateOpening,
        &DesignTab::stateClose, &DesignTab::stateClosing};
    preStateUpdate();
    (this->*tbl[transitionState])();
    postStateUpdate();
    return TRUE;
}

void DesignTab::runMainState() {
    static Unk_ov121_02294d68_Fn tbl[21] = {
        &DesignTab::updateTouch, &DesignTab::updateTouchHold,
        &DesignTab::updateTouchDrag, &DesignTab::updatePopupTouch,
        &DesignTab::updateButtons, &DesignTab::updateCarry,
        &DesignTab::updateCursorMove, &DesignTab::updateCursorPress,
        &DesignTab::updateCursorRelease, &DesignTab::updateGrab,
        &DesignTab::updateCarryMove, &DesignTab::updateDrop,
        &DesignTab::updatePopupButtons, &DesignTab::updatePopupPress,
        &DesignTab::updatePopupOpen, &DesignTab::updatePopupClose,
        &DesignTab::updatePopupDone, &DesignTab::updateWearRequest,
        &DesignTab::updateWearWait, &DesignTab::updateMessage,
        &DesignTab::updateItemPlace};
    (this->*tbl[mainState])();
}

BOOL DesignTab::execMain() {
    if (handleTabSwitch()) {
        return TRUE;
    }
    preInputUpdate();
    runMainState();
    postInputUpdate();
    return TRUE;
}

BOOL DesignTab::execPhase3() {
    return TRUE;
}

BOOL DesignTab::execPhase4() {
    return TRUE;
}

BOOL DesignTab::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

BOOL DesignTab::handleTabSwitch() {
    MenuCtrl_TickForceClose();
    if (MenuCtrl_IsForceCloseDue()) {
        switch (mainState) {
        case 0:
        case 1:
        case 4:
        case 7:
        case 8:
            return requestTab(7);
        case 2:
        case 3:
        case 5:
        case 6:
            break;
        }
    }
    if (mainState != 0 && mainState != 4) {
        return FALSE;
    }
    s32 r5 = -1;
    if (MenuCtrl_IsTouch()) {
        r5 = MenuTabBar_HitTestTouch();
    } else {
        u32 v = gPad[1];
        if ((v & 0x800) != 0) {
            r5 = 0;
        } else if ((v & 0x400) != 0) {
            r5 = 5;
        } else if ((v & 4) != 0) {
            r5 = 4;
        }
    }
    return requestTab(r5);
}

BOOL DesignTab::requestTab(s32 a) {
    MenuTabBar *r = (MenuTabBar *)ProcBase_GetParent(this);
    s32 m = -1;
    if (a == m) goto fail;
    if (a == 1) goto fail;
    r->selectTab((u8)a);
    transitionState = 2;
    setPhase(1);
    ((TouchPromptBalloon *)(&nameBalloon))->hide(1);
    if (a != 7) {
        restoreCamera();
    }
    return TRUE;
fail:
    return FALSE;
}

void DesignTab::stateOpen() {
    setupBgLayers();
    loadBgGfx();
    loadPatternIcons();
    PopupChoice_LoadChoiceBg(&popup);
    beginSubSlideIn(8, 0, 0, 0x30);
    Gfx2d_ShowLayer(6);
    setTransitionState(1);
    setFlags(1);
    applySlideOffset(6, 0, 0);
    slideY = getSlideOffsetY();
}

void DesignTab::stateOpening() {
    if (stepSlideIn(0)) {
        setPhase(2);
        resumeInput();
    }
    applySlideOffset(6, 0, 0);
    slideY = getSlideOffsetY();
}

void DesignTab::stateClose() {
    hideCursor();
    beginSubSlideOut(8, 0, 0, 0x30);
    setTransitionState(3);
    applySlideOffset(6, 0, 0);
    slideY = getSlideOffsetY();
}

void DesignTab::stateClosing() {
    if (stepSlideOut(0)) {
        Gfx2d_ResetLayer(6);
        setPhase(5);
    } else {
        applySlideOffset(6, 0, 0);
        slideY = getSlideOffsetY();
    }
}

void DesignTab::initDesignTab() {
    u32 i = 0;
    do {
        slotIcons[i] = i;
        i = (u8)(i + 1);
    } while (i < 8);
    u32 z = 0;
    flags = z;
    disabledTargets = z;
    if (gFieldSceneKind == 0) {
        z = 1;
    }
    if (z != 0) {
        roomMode = 0;
        disableTarget(9);
        disableTarget(0xa);
        disableTarget(0xb);
        CommManager *g = gCommManager;
        if (((CommManager *)(g))->isOnline() && g->myAid != 0) {
            disableTarget(0xf);
        }
    } else if (canDecorateRoom()) {
        roomMode = 1;
        disableTarget(0xf);
    } else {
        roomMode = 2;
        disableTarget(9);
        disableTarget(0xa);
        disableTarget(0xb);
        disableTarget(0xe);
        disableTarget(0xf);
    }
    ((PopupChoiceMenu *)(&popup))->init(3, 1, 0);
    highlightedTarget = 0x18;
    cursorSlot = 0;
}

BOOL DesignTab::canDecorateRoom() {
    if (Scene_InHouseRoom()) {
        CommManager *g = gCommManager;
        if (((CommManager *)(g))->isOnline() && g->myAid != 0) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

void DesignTab::releaseResources() {
    PopupChoice_ForceClose(&popup);
    bgTasks[0].cancel();
    bgTasks[1].cancel();
}

void DesignTab::preInputUpdate() {
    preStateUpdate();
    cursor.update();
}

void DesignTab::postInputUpdate() {
    postStateUpdate();
}

void DesignTab::preStateUpdate() {}

void DesignTab::postStateUpdate() {
    updateTargetBlink();
    if (testFlags(2)) {
        if (((BgVramTask *)(&bgTasks[0]))->requestScreen(mainScreen, 6, 0x800, 0)) {
            clearFlags(2);
        }
    }
    if (((TouchPromptBalloon *)(&nameBalloon))->updatePrompt()) {
        refreshNameLabel();
    }
    Shared_PopupChoice_Update(&popup);
}

void DesignTab::setupBgLayers() {
    Gfx2d_SetLayerPriority(6, 2);
    Gfx2d_SetLayerControl(6, 0, 0, 0);
}

void DesignTab::loadBgGfx() {
    Heap *r4 = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/desi/b_myd_bg.bpl", r4, 6, 1, 1, 7);
    Gfx2d_LoadCharFile("menu/desi/b_myd_bg.bch", r4, 6, 0x10, 0x10, 0xc5);
    File_LoadToBuffer("menu/desi/b_myd_a_bg.bsc", mainScreen, 0x800);
    u32 i = 9;
    do {
        if (isTargetDisabled(i)) {
            setTargetPalette(i, 7);
        }
        i = (u8)(i + 1);
    } while (i <= 0xf);
    setFlags(2);
}

void DesignTab::loadPatternIcons() {
    Heap *r6;
    u32 *r5;
    u32 n;
    u32 i;
    PlayerPatterns *v;
    u32 k;
    r6 = gCurrentHeap;
    Gfx2d_LoadCharFile("menu/desi/b_myd_obj.bch", r6, 8, 0x140, 0x140, 0x17f);
    r5 = (u32 *)Heap_AllocTail(r6, 0x1000);
    v = (PlayerPatterns *)PlayerData_GetCurrent()->getPatterns();
    i = 0;
    k = 4;
    do {
        Gfx2d_LinearToTilesInRow32((u32 *)v->getPatternByOrder(i)->getPixels(), r5, i << 2, k, k);
        i = (u8)(i + 1);
    } while (i < 8);
    Gfx2d_LoadCharRange((u8 *)r5, 8, 0xc0, 0xc0, 0x13f);
    Heap_Free(r6, r5);
    u8 *r7 = (u8 *)Heap_AllocTail(r6, 0x120);
    i = 0;
    n = i;
    do {
        MIi_CpuCopy16(v->getPatternByOrder(n)->getInfo()->getPaletteData(), r7 + i * 2, 0x20);
        i += 0x10;
        n = (u8)(n + 1);
    } while (n < 8);
    File_LoadToBuffer("menu/desi/b_myd_obj.bpl", r7 + i * 2, 0x20);
    Gfx2d_LoadPaletteRange(r7, 8, 4, 4, 0xc);
    Heap_Free(r6, r7);
}

void DesignTab::updateTouch() {
    if (checkSwitchToButtons(1)) {
        startButtonInput();
    } else if (Unk_ov121_02293f34_Both()) {
        u32 r4 = findTouchedSlot();
        if (r4 != 0x18) {
            setMainState(1);
            balloonSlot = r4;
            ((TouchPromptBalloon *)(&nameBalloon))->queueOpen();
            Snd_PlaySe(0xc);
        }
    }
}

void DesignTab::updateTouchHold() {
    if (gTouchHeld == 0) {
        setMainState(0);
        ((TouchPromptBalloon *)(&nameBalloon))->setAutoCloseTimer(0x3c);
    } else if (hasDragStarted()) {
        heldSlot = balloonSlot;
        setFlags(4);
        targetSlot = 0x18;
        setMainState(2);
        ((TouchPromptBalloon *)(&nameBalloon))->hide(1);
        Snd_PlaySe(0xd);
    } else {
        ((TouchPromptBalloon *)(&nameBalloon))->commitOpen();
    }
}

void DesignTab::updateTouchDrag() {
    clearFlags(8);
    if (MenuCtrl_IsForceCloseDue()) {
        clearFlags(4);
        highlightTarget(0x18);
        setMainState(0);
    } else if (gTouchHeld == 0) {
        clearFlags(4);
        highlightTarget(0x18);
        if (dropHeldOnTarget() == 0) {
            setMainState(0);
        }
    } else {
        targetSlot = findDropTarget();
        u32 v = targetSlot;
        if (v >= 9 && v <= 0xf) {
            highlightTarget(v);
            setFlags(8);
            return;
        }
        if (v <= 7) goto b;
        if (v == 8) {
b:
            setFlags(8);
        }
        highlightTarget(0x18);
    }
}

void DesignTab::updatePopupTouch() {
    if (MenuCtrl_IsForceCloseDue()) {
        abortPopup();
        return;
    }
    if (checkSwitchToButtons(1)) {
        cursorToPopupTop();
        setMainState(0xc);
        cursorSlot = targetSlot;
        return;
    }
    if (Unk_ov121_02293f34_Both()) {
        s32 r6 = ((PopupChoiceMenuBody *)(&popup))->hitTestRowOrLast(gTouchCurX, gTouchCurY);
        if (r6 >= 0) {
            s32 r5 = 1;
            popupChoice = getPopupRowValue(r6);
            switch (popupChoice) {
            case 0:
            case 1:
            case 2:
            case 3:
                Snd_PlaySe(0x50);
                break;
            case 4:
            case 5:
                break;
            case 6:
                r5 = 0;
                break;
            }
            PopupChoice_DecideRow(&popup, r6, r5);
            setMainState(0xf);
        }
    }
}

void DesignTab::updateButtons() {
    if (checkSwitchToTouch()) {
        startTouchInput();
        ((TouchPromptBalloon *)(&nameBalloon))->hide(1);
    } else if (moveCursorByPad(takeRepeatedKeys(), 0)) {
        updateNameLabel();
        moveCursorToTarget();
        ((TouchPromptBalloon *)(&nameBalloon))->hide(0);
    } else {
        if (gPad[1] & 1) {
            ((TouchPromptBalloon *)(&nameBalloon))->hide(1);
            if (getCursorTab() != -1) {
                pressCursor();
                return;
            }
            if (cursorSlot <= 7) {
                startCarry();
                return;
            }
        }
        u32 k = gPad[1];
        if (k & 0x100) {
            requestTab(MenuTabBar_NextTab(1));
        } else if (k & 0x200) {
            requestTab(MenuTabBar_PrevTab(1));
        } else if (k & 2) {
            requestTab(7);
        } else {
            ((TouchPromptBalloon *)(&nameBalloon))->commitOpen();
        }
    }
}

void DesignTab::updateCarry() {
    if (MenuCtrl_IsForceCloseDue()) {
        clearFlags(4);
        requestTab(7);
    } else if (moveCursorByPad(takeRepeatedKeys(), 1)) {
        moveCursorToTarget();
    } else {
        if (gPad[1] & 1) {
            if (!isTargetDisabled(cursorSlot)) dropCarry();
        }
        if (gPad[1] & 2) cancelCarry();
    }
}

void DesignTab::updateCursorMove() {
    if (!((MenuCursorBase *)(&cursor))->isMoving()) {
        setMainState(returnState);
        runMainState();
    }
}

void DesignTab::updateCursorPress() {
    if (((HandCursor *)(&cursor))->isAnimDone()) {
        s32 r = getCursorTab();
        if (r == -1 || !requestTab(r)) releaseCursor();
    }
}

void DesignTab::updateCursorRelease() {
    if (((HandCursor *)(&cursor))->isAnimDone()) {
        refreshCursor();
        setMainState(4);
    }
}

void DesignTab::updateGrab() {
    if (((MenuCursorBase *)(&cursor))->isGripping()) {
        setMainState(0xa);
        heldSlot = cursorSlot;
        setFlags(4);
        Snd_PlaySe(0xd);
    }
}

void DesignTab::updateCarryMove() {
    if (((HandCursor *)(&cursor))->isAnimDone()) setMainState(5);
}

void DesignTab::updateDrop() {
    if (!((MenuCursorBase *)(&cursor))->isGripping()) {
        clearFlags(4);
        if (testFlags(0x80)) {
            updateNameLabel();
            setMainState(4);
            clearFlags(0x80);
            Snd_PlaySe(0xe);
        } else {
            targetSlot = cursorSlot;
            clearFlags(4);
            if (!dropHeldOnTarget()) {
                updateNameLabel();
                setMainState(4);
            }
        }
    }
}

void DesignTab::updatePopupButtons() {
    if (MenuCtrl_IsForceCloseDue()) {
        abortPopup();
    } else if (checkSwitchToTouch()) {
        hideCursor();
        setMainState(3);
    } else if (PopupChoice_MoveCursor(&popup, takeRepeatedKeys(), &popupRow, 0)) {
        moveCursorToPopupRow();
    } else {
        u32 k = gPad[1];
        if (k & 1) {
            ((MenuCursor *)(&cursor))->setPosePress();
            setMainState(0xd);
        } else if (k & 2) {
            cancelPopup();
        }
    }
}

void DesignTab::updatePopupPress() {
    if (MenuCtrl_IsForceCloseDue()) {
        abortPopup();
    } else if (((HandCursor *)(&cursor))->isAnimDone()) {
        popupChoice = getPopupRowValue(popupRow);
        BOOL r5 = TRUE;
        switch (popupChoice) {
        case 0:
        case 1:
        case 2:
        case 3:
            Snd_PlaySe(0x50);
            break;
        case 4:
        case 5:
            break;
        case 6:
            r5 = FALSE;
            break;
        }
        PopupChoice_DecideRow(&popup, popupRow, r5);
        setMainState(0xf);
    }
}

void DesignTab::updatePopupOpen() {
    if (((PopupChoiceMenuBody *)(&popup))->isOpen()) {
        if (MenuCtrl_IsButtons()) {
            cursorToPopupTop();
            setMainState(0xc);
        } else {
            setMainState(3);
        }
    }
}

void DesignTab::updatePopupClose() {
    if (PopupChoice_TickDecideDelay(&popup)) {
        PopupChoice_Close(&popup, 0);
        if (((HandCursor *)(&cursor))->getAnim()) showCursorAtTarget();
        setMainState(0x10);
    }
}

void DesignTab::updatePopupDone() {
    if (((PopupChoiceMenuBody *)(&popup))->isClosed()) onPopupChoice();
}

void DesignTab::updateWearRequest() {
    if (requestWear(equipPart, designItem)) setMainState(0x12);
}

void DesignTab::updateWearWait() {
    if (isWearDone(equipPart)) resumeInput();
}

void DesignTab::updateMessage() {
    if (((MenuErrorMessage *)(&errorMessage))->update(1)) resumeInput();
}

void DesignTab::updateItemPlace() {
    switch (FieldAction_PollDrop(dropRequest)) {
    case 1:
        restoreCamera();
        startTargetBlink(targetSlot);
        resumeInput();
        break;
    case 2:
        openMessageWindow(3, 0);
        Snd_PlaySe(0x73);
        break;
    default:
        return;
    }
    FieldAction_Release(dropRequest);
    dropRequest = -1;
}

void DesignTab::startTouchInput() {
    hideCursor();
    setMainState(0);
}

void DesignTab::startButtonInput() {
    u8 v = 0x18;
    heldSlot = v;
    balloonSlot = v;
    showCursor();
    restartKeyRepeat();
    updateNameLabel();
    setMainState(4);
}

void DesignTab::resumeInput() {
    if (MenuCtrl_IsTouch()) startTouchInput();
    else startButtonInput();
}

void DesignTab::openMessageWindow(u32 a, u32 b) {
    u8 buf[1];
    buf[0] = gU8None;
    buf[0] = a;
    ((MenuErrorMessage *)(&errorMessage))->open(buf, b, 0);
    setMainState(0x13);
    hideCursor();
}

s32 DesignTab::getSlotX(u32 i) {
    if (i >= 0x10 && i <= 0x17) return MenuTabBar_GetTabX(i - 0x10);
#ifdef N3DS_DIRECT
    if (i == DESIGN_OPT_SLOT) return MenuTabBar_GetTabX(N3DS_OPT_TAB);
#endif
    return sDesignTabSlotX[i];
}

u32 DesignTab::getSlotY(u32 i) {
    if (i >= 0x10 && i <= 0x17) return 8;
#ifdef N3DS_DIRECT
    if (i == DESIGN_OPT_SLOT) return 8;
#endif
    return sDesignTabSlotY[i];
}

void DesignTab::setTargetPalette(u32 id, u32 s) {
    u32 k = id - 9;
    u32 xv = sDesignTabTargetTileY[k];
    u32 yv = sDesignTabTargetTileX[k];
    BgScreen_SetRectPalette(mainScreen, yv, xv, yv + 3, xv + 3, s);
    setFlags(2);
}

void DesignTab::highlightTarget(u32 idx) {
    u32 old = highlightedTarget;
    if (idx != old) {
        if (old != 0x18) setTargetPalette(old, 2);
        highlightedTarget = idx;
        u32 n = *(volatile u8 *)&highlightedTarget;
        if (n != 0x18) setTargetPalette(n, 6);
    }
}

u32 DesignTab::findSlotAt(u32 x, u32 y, u32 n) {
    s32 xl = x - 0x10;
    s32 xh = x + 0x10;
    s32 yl = y - 0x10;
    s32 yh = y + 0x10;
    u8 i;
    for (i = 0; i <= n; i++) {
        s32 px = getSlotX(i);
        if (xl < px && px < xh) {
            s32 py = getSlotY(i);
            if (yl < py && py < yh) return i;
        }
    }
    return 0x18;
}

u32 DesignTab::findTouchedSlot() {
    u32 x = gTouchCurX;
    u32 y = gTouchCurY;
    u32 r = findSlotAt(x, y, 7);
    if (r != 0x18) {
        dragOffsetX = getSlotX(r) - x;
        dragOffsetY = getSlotY(r) - y;
    }
    return r;
}

u32 DesignTab::findDropTarget() {
    u32 x = *(volatile u8 *)&gTouchCurX;
    u32 y = *(volatile u8 *)&gTouchCurY;
    u32 r = findSlotAt(x + dragOffsetX, y + dragOffsetY, 0xf);
    if (isTargetDisabled(r)) r = 0x18;
    return r;
}

void DesignTab::disableTarget(u32 i) {
    disabledTargets = disabledTargets | (1 << i);
}

BOOL DesignTab::isTargetDisabled(u32 i) {
    if ((disabledTargets & (1 << i)) != 0) return TRUE;
    return FALSE;
}

BOOL DesignTab::hasDragStarted() {
    s32 d = gTouchPressX - gTouchCurX;
    if (d < 0) d = -d;
    if (d > 0x10) return TRUE;
    d = gTouchPressY - gTouchCurY;
    if (d < 0) d = -d;
    if (d > 0x10) return TRUE;
    return FALSE;
}

void DesignTab::moveDragIconToTouch() {
    handX = dragOffsetX + gTouchCurX;
    handY = dragOffsetY + gTouchCurY;
}

BOOL DesignTab::dropHeldOnTarget() {
    u32 v = targetSlot;
    if (v <= 7) {
        Snd_PlaySe(0xe);
        swapPatternSlots(targetSlot, heldSlot);
        return FALSE;
    }
    switch (v) {
    case 8:
        dropOnPlayerFigure();
        return TRUE;
    case 10:
        if (isRoomEditAllowed()) {
            setPopupChoices(0);
        } else {
            openMessageWindow(0x17, 1);
            return TRUE;
        }
        openPopup();
        return TRUE;
    case 11:
        if (isRoomEditAllowed()) {
            setPopupChoices(1);
        } else {
            openMessageWindow(0x18, 1);
            return TRUE;
        }
        openPopup();
        return TRUE;
    case 13:
        switch (roomMode) {
        case 0:
        case 2:
            equipDesign(0);
            break;
        case 1:
            setPopupChoices(3);
            openPopup();
            break;
        }
        return TRUE;
    case 12:
        switch (roomMode) {
        case 0:
        case 2:
            equipDesign(1);
            break;
        case 1:
            setPopupChoices(4);
            openPopup();
            break;
        }
        return TRUE;
    case 14:
        switch (roomMode) {
        case 0:
            equipDesign(2);
            break;
        case 1:
            placeDesignInRoom();
            return TRUE;
        default:
            return FALSE;
        }
        return TRUE;
    case 9:
        placeDesignInRoom();
        return TRUE;
    case 15:
        placeDesignItem();
        return TRUE;
    default:
        return FALSE;
    }
}

void DesignTab::swapPatternSlots(u32 a, u32 b) {
    if (a != b) {
        ((PatternOrder *)(((PlayerPatterns *)(PlayerData_GetCurrent()->getPatterns()))->getPatternOrder()))->swap(a, b);
        u8 t = slotIcons[a];
        slotIcons[a] = slotIcons[b];
        slotIcons[b] = t;
    }
}

u32 DesignTab::getSlotPattern(u32 i) {
    return ((PatternOrder *)(((PlayerPatterns *)(PlayerData_GetCurrent()->getPatterns()))->getPatternOrder()))->getSlot(i);
}

void DesignTab::setPopupChoices(u32 i) {
    MI_CpuCopy8(sDesignTabPopupChoices[i], &choiceList, 0xb);
}

u32 DesignTab::getPopupRowValue(u32 i) {
    return choiceList.values[i];
}

void DesignTab::dropOnPlayerFigure() {
    volatile u16 v;
    u16 w;
    void *r6 = PlayerData_GetCurrent();
    s32 s = Pocket_FindEmpty();
    v = *(u16 *)((PlayerData *)(r6))->getInventoryBackground();
    if (Unk_ov121_02293188_InRange(&v, 0x11a8, 0x12a7) && s == -1) {
        openMessageWindow(4, 1);
        return;
    }
    restoreCamera();
    u32 u = getSlotPattern(heldSlot);
    u16 x;
    if (u < 8) {
        x = u + 0x12a8;
    } else {
        x = 0x12a8;
    }
    w = x;
    MenuScreen_UploadClothPattern(&w, &bgTasks[1], clothImage, clothPalette);
    if (Unk_ov121_02293188_InRange(&v, 0x11a8, 0x12a7)) {
        Pocket_AddItem((u16 *)&v, 0);
    }
    resumeInput();
    Snd_PlaySe(0x6b);
}

void DesignTab::applyRoomDesignA(u32 m) {
    volatile u16 v;
    if (!isRoomEditAllowed()) {
        openMessageWindow(0x17, 1);
        return;
    }
    PlayerData_GetCurrent();
    s32 s = Pocket_FindEmpty();
    v = *RoomShell_GetCarpet();
    if (!Unk_ov121_02293188_InRange(&v, 0x1188, 0x11a7) && s == -1) {
        openMessageWindow(4, 1);
        return;
    }
    restoreCamera();
    RoomWallFloor_SetCarpetDesign(getSlotPattern(heldSlot), m, 1, 1);
    if (!Unk_ov121_02293188_InRange(&v, 0x1188, 0x11a7)) {
        Pocket_AddItem((u16 *)&v, 0);
    }
    resumeInput();
    startTargetBlink(targetSlot);
}

void DesignTab::applyRoomDesignB(u32 m) {
    volatile u16 v;
    if (!isRoomEditAllowed()) {
        openMessageWindow(0x18, 1);
        return;
    }
    s32 s = Pocket_FindEmpty();
    v = *RoomShell_GetWallpaper();
    if (!Unk_ov121_02293188_InRange(&v, 0x1188, 0x11a7) && s == -1) {
        openMessageWindow(4, 1);
        return;
    }
    restoreCamera();
    RoomWallFloor_SetWallpaperDesign(getSlotPattern(heldSlot), m, 1, 1);
    if (!Unk_ov121_02293188_InRange(&v, 0x1188, 0x11a7)) {
        Pocket_AddItem((u16 *)&v, 0);
    }
    resumeInput();
    startTargetBlink(targetSlot);
}

void DesignTab::equipDesign(u32 m) {
    volatile u16 v;
    u16 w;
    u32 t;
    equipPart = m;
    t = getWornItem(equipPart);
    v = t;
    switch (equipPart) {
    case 0:
        if (Unk_ov121_02293188_InRange(&v, 0x12a8, 0x12af)) {
            t = 0xfff1;
        }
        break;
    case 1:
        if (Unk_ov121_02293188_InRange(&v, 0x1429, 0x1430)) {
            t = 0xfff1;
        }
        break;
    case 2:
        if (Unk_ov121_02293188_InRange(&v, 0x13a0, 0x13a7)) {
            t = 0xfff1;
        }
        break;
    }
    if (t != 0xfff1) {
        if (Pocket_FindEmpty() == -1) {
            openMessageWindow(4, 1);
            return;
        }
        w = t;
        Pocket_AddItem(&w, 0);
    }
    pushCamera();
    u32 u;
    u16 x;
    switch (equipPart) {
    case 0:
        u = getSlotPattern(heldSlot);
        if (u < 8) {
            x = u + 0x12a8;
        } else {
            x = 0x12a8;
        }
        designItem = x;
        break;
    case 1:
        u = getSlotPattern(heldSlot);
        if (u < 8) {
            x = u + 0x1429;
        } else {
            x = 0x1429;
        }
        designItem = x;
        break;
    case 2:
        u = getSlotPattern(heldSlot);
        if (u < 8) {
            x = u + 0x13a0;
        } else {
            x = 0x13a0;
        }
        designItem = x;
        break;
    }
    swapWornItem(equipPart, designItem);
    setMainState(0x11);
    startTargetBlink(targetSlot);
}

void DesignTab::placeDesignInRoom() {
    u32 buf;
    s32 r;
    if (Room_CountOccupants() > 1) {
        openMessageWindow(9, 0);
        Snd_PlaySe(0x73);
        return;
    }
    switch (targetSlot - 9) {
    case 4:
        r = FtrMgr_FindPlacementMyDesignA(&buf, (u8)getSlotPattern(heldSlot), 1);
        break;
    case 3:
        r = FtrMgr_FindPlacementMyDesignD(&buf, (u8)getSlotPattern(heldSlot), 1);
        break;
    case 0:
        r = FtrMgr_FindPlacementMyDesignC(&buf, (u8)getSlotPattern(heldSlot), 1);
        break;
    case 5:
        r = FtrMgr_FindPlacementMyDesignB(&buf, (u8)getSlotPattern(heldSlot), 1);
        break;
    case 1:
    case 2:
    default:
        resumeInput();
        return;
    }
    switch (r) {
    case 0:
    case 1:
        openMessageWindow(5, 0);
        Snd_PlaySe(0x73);
        break;
    case 2:
        openMessageWindow(3, 0);
        Snd_PlaySe(0x73);
        break;
    default:
        restoreCamera();
        FtrMgr_SpawnFromArg(buf);
        resumeInput();
        startTargetBlink(targetSlot);
        break;
    }
}

void DesignTab::placeDesignItem() {
    s32 t = PlayerData_GetCurrent()->getIndex();
    u32 u = getSlotPattern(heldSlot);
    u32 x = sDesignItemBase[t];
    x += u;
    designItem = x;
    dropRequest = FieldAction_RequestDrop(gCommManager->myAid, designItem);
    if (dropRequest == -1) {
        openMessageWindow(3, 0);
        Snd_PlaySe(0x73);
    } else {
        setMainState(0x14);
    }
}

void DesignTab::onPopupChoice() {
    switch (popupChoice) {
    case 0:
        applyRoomDesignA(0);
        break;
    case 1:
        applyRoomDesignA(1);
        break;
    case 2:
        applyRoomDesignB(0);
        break;
    case 3:
        applyRoomDesignB(1);
        break;
    case 4:
        dropOnPlayerFigure();
        break;
    case 5:
        equipDesign(0);
        break;
    case 6:
        placeDesignInRoom();
        break;
    case 7:
        equipDesign(1);
        break;
    default:
        resumeInput();
        break;
    }
}

void DesignTab::openPopup() {
    ((PopupChoiceMenuBody *)(&popup))->setRowsFromIds((PopupChoiceIdList *)(&choiceList), 0);
    s32 a = getSlotX(targetSlot) - 0x18;
    s32 b = getSlotY(targetSlot) + 0x10;
    ((PopupChoiceMenu *)(&popup))->placeAt(a, b);
    PopupChoice_Open(&popup, 0);
    setMainState(0xe);
}

void DesignTab::abortPopup() {
    popupChoice = 8;
    showCursorAtTarget();
    PopupChoice_Close(&popup, 0);
    setMainState(0x10);
}

void DesignTab::pushCamera() {
    if (!Camera_IsViewPushed()) {
        Camera_PushView();
    }
}

void DesignTab::restoreCamera() {
    if (Camera_IsViewPushed()) {
        Camera_PopView();
    }
}

void DesignTab::refreshNameLabel() {
    s32 x = getSlotY(balloonSlot);
    x -= 0x84;
    if (MenuCtrl_IsButtons()) {
        x -= 0xa;
    }
    ((LabelBalloon *)(&nameBalloon))->setPos(getSlotX(balloonSlot) - 0x78, x);
    EncodedString16Buf a;
    ((PlayerPatterns *)PlayerData_GetCurrent()->getPatterns())->getPatternByOrder(balloonSlot)->getInfo()->getTitleEncoded(&a);
    LabelBalloonText b;
    StrBuf_GameToAscii((StrBuf *)&b, (StrBuf *)&a);
    ((LabelBalloon *)(&nameBalloon))->setText((StrBuf *)&b);
}

void DesignTab::updateNameLabel() {
    u32 t = cursorSlot;
    if (t <= 7) {
        balloonSlot = t;
        ((TouchPromptBalloon *)(&nameBalloon))->queueOpen();
    } else {
        ((TouchPromptBalloon *)(&nameBalloon))->cancelQueuedOpen();
    }
}

void DesignTab::showCursor() {
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    ((MenuCursorBase *)(&cursor))->warpTo(a, b);
    if (getCursorTab() != -1) {
        ((MenuCursor *)(&cursor))->setAnimIfChanged(0xd);
    } else {
        ((MenuCursor *)(&cursor))->setAnimIfChanged(1);
    }
    refreshCursor();
}

s32 DesignTab::getCursorTargetX() {
    s32 t = getSlotX(cursorSlot);
    if (testFlags(0x40)) {
        t += 0x100;
    } else if (testFlags(0x20)) {
        t -= 0x100;
    }
    if (getCursorTab() == -1) {
        t += 0xb;
    }
    return t;
}

s32 DesignTab::getCursorTargetY() {
    s32 t = getSlotY(cursorSlot);
    if (getCursorTab() == -1) {
        t -= 0xb;
    }
    return t;
}

void DesignTab::hideCursor() {
    ((MenuCursor *)(&cursor))->setAnimIfChanged(0);
    cursor.update();
}

void DesignTab::moveCursorToTarget() {
    if (testFlags(0x10)) {
        s32 a = getCursorTargetX();
        s32 b = getCursorTargetY();
        ((MenuCursorBase *)(&cursor))->warpTo(a, b);
        clearFlags(0x10);
    } else {
        s32 a = getCursorTargetX();
        s32 b = getCursorTargetY();
        ((MenuCursorBase *)(&cursor))->moveToEase(a, b, 3, 1);
        returnState = mainState;
        setMainState(6);
    }
}

void DesignTab::moveCursorToPopupRow() {
    s32 a = ((PopupChoiceMenuBody *)(&popup))->getRowX();
    s32 b = ((PopupChoiceMenuBody *)(&popup))->getRowY(popupRow);
    ((MenuCursorBase *)(&cursor))->moveToLinear(a, b, 2);
    returnState = mainState;
    setMainState(6);
}

void DesignTab::cancelPopup() {
    popupChoice = 8;
    popupRow = PopupChoice_DecideCancel(&popup, 1);
    s32 a = ((PopupChoiceMenuBody *)(&popup))->getRowX();
    s32 b = ((PopupChoiceMenuBody *)(&popup))->getRowY(popupRow);
    ((MenuCursorBase *)(&cursor))->warpTo(a, b);
    ((HandCursor *)(&cursor))->setAnimAtEnd(8);
    setMainState(0xf);
}

void DesignTab::cursorToPopupTop() {
    popupRow = 0;
    s32 a = ((PopupChoiceMenuBody *)(&popup))->getRowX();
    s32 b = ((PopupChoiceMenuBody *)(&popup))->getRowY(popupRow);
    ((MenuCursorBase *)(&cursor))->warpTo(a, b);
    ((MenuCursor *)(&cursor))->setAnimIfChanged(7);
}

void DesignTab::showCursorAtTarget() {
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    ((MenuCursorBase *)(&cursor))->warpTo(a, b);
    ((MenuCursor *)(&cursor))->setAnimIfChanged(1);
}

void DesignTab::refreshCursor() {
    ((MenuCursorBase *)(&cursor))->setPoseIdle();
    cursor.update();
}

void DesignTab::pressCursor() {
    ((MenuCursor *)(&cursor))->setPosePress();
    setMainState(7);
}

void DesignTab::releaseCursor() {
    ((MenuCursorBase *)(&cursor))->setPoseRelease();
    setMainState(8);
}

void DesignTab::startCarry() {
    ((MenuCursor *)(&cursor))->setAnimIfChanged(4);
    setMainState(9);
}

void DesignTab::dropCarry() {
    ((MenuCursor *)(&cursor))->setAnimIfChanged(5);
    clearFlags(0x80);
    setMainState(0xb);
}

void DesignTab::cancelCarry() {
    ((MenuCursor *)(&cursor))->setAnimIfChanged(5);
    setFlags(0x80);
    setMainState(0xb);
}

void DesignTab::moveDragIconToCursor() {
    handX = ((MenuCursorBase *)(&cursor))->getFrameScreenX() - 3;
    handY = ((MenuCursorBase *)(&cursor))->getFrameScreenY() + 9;
}

BOOL DesignTab::stepCursorRight(u32 lo, u32 hi) {
    u32 t = cursorSlot;
    if (t >= lo && t <= hi) {
        if (t == hi) {
            setFlags(0x40);
            cursorSlot = lo;
        } else {
            cursorSlot = *(volatile u8 *)&cursorSlot + 1;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL DesignTab::stepCursorLeft(u32 lo, u32 hi) {
    u32 t = cursorSlot;
    if (t >= lo && t <= hi) {
        if (t == lo) {
            cursorSlot = hi;
            setFlags(0x20);
        } else {
            cursorSlot = *(volatile u8 *)&cursorSlot - 1;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL DesignTab::cursorRowUp(u32 lo, u32 hi, u32 v) {
    u32 t = cursorSlot;
    if (t >= lo && t <= hi) {
        cursorSlot = *(volatile u8 *)&cursorSlot + (v - lo);
        return TRUE;
    }
    return FALSE;
}

BOOL DesignTab::cursorRowDown(u32 lo, u32 hi, u32 v) {
    u32 t = cursorSlot;
    if (t >= lo && t <= hi) {
        cursorSlot = *(volatile u8 *)&cursorSlot + (v - lo);
        return TRUE;
    }
    return FALSE;
}

s32 DesignTab::getCursorTab() {
    u32 t = cursorSlot;
    if (t >= 0x10 && t <= 0x17) {
        return t - 0x10;
    }
#ifdef N3DS_DIRECT
    if (t == DESIGN_OPT_SLOT) {
        return N3DS_OPT_TAB;
    }
#endif
    return -1;
}

BOOL DesignTab::moveCursorHorizontal(u32 pad, u32 f) {
    if (MenuKeys_HasLeft(pad)) {
        if (cursorSlot == 8) {
            cursorSlot = 7;
            setFlags(0x20);
        } else if (cursorSlot == 4 && f) {
            cursorSlot = 8;
        } else if (!stepCursorLeft(0, 3) && !stepCursorLeft(4, 7) && !stepCursorLeft(9, 0xb) &&
                   !stepCursorLeft(0xc, 0xf)) {
            u32 t = cursorSlot;
            if (t > 0x10 && t <= 0x17) {
                cursorSlot = *(volatile u8 *)&cursorSlot - 1;
            }
#ifdef N3DS_DIRECT
            else if (t == 0x10) {
                cursorSlot = DESIGN_OPT_SLOT;  // (from the pockets' tab onto the options button)
            }
#endif
        }
    } else if (MenuKeys_HasRight(pad)) {
        if (cursorSlot == 8) {
            cursorSlot = 4;
        } else if (cursorSlot == 7 && f) {
            cursorSlot = 8;
            setFlags(0x40);
        } else if (cursorSlot == 0xb && MenuKeys_HasDown(pad)) {
            cursorSlot = 0xf;
            return TRUE;
        } else if (!stepCursorRight(0, 3) && !stepCursorRight(4, 7) && !stepCursorRight(9, 0xb) &&
                   !stepCursorRight(0xc, 0xf)) {
            u32 t = cursorSlot;
            if (t >= 0x10 && t < 0x17) {
                cursorSlot = *(volatile u8 *)&cursorSlot + 1;
            }
#ifdef N3DS_DIRECT
            else if (t == DESIGN_OPT_SLOT) {
                cursorSlot = 0x10;  // (back onto the pockets' tab)
            }
#endif
        }
    }
    return testFlags(0x60);
}

void DesignTab::moveCursorToTabs() {
    s32 t = ((MenuCursorBase *)(&cursor))->getScreenX();
    cursorSlot = MenuTabBar_TabFromX(t) + 0x10;
    ((MenuCursor *)(&cursor))->switchToAnim0D();
}

void DesignTab::moveCursorFromTab(u32 idx) {
    cursorSlot = sDesignTabTabDownSlot[idx];
    ((MenuCursor *)(&cursor))->switchToAnim01();
}

void DesignTab::moveCursorVertical(u32 pad, u32 f) {
    if (MenuKeys_HasUp(pad)) {
        if (cursorSlot == 8) {
            cursorSlot = 0;
        } else if (cursorSlot <= 3) {
            if (f) {
                cursorSlot = *(volatile u8 *)&cursorSlot + 0xc;
            } else {
                moveCursorToTabs();
            }
        } else if (!cursorRowUp(4, 7, 0)) {
            u32 t = cursorSlot;
            if (t >= 9 && t <= 0xb) {
                if (!f) {
                    moveCursorToTabs();
                }
            } else if (t >= 0xc && t <= 0xf) {
                if (MenuKeys_HasRight(pad)) {
                    cursorSlot = *(volatile u8 *)&cursorSlot - 1;
                }
                cursorSlot = *(volatile u8 *)&cursorSlot - 3;
                if (cursorSlot > 0xb) {
                    cursorSlot = 0xb;
                }
            }
        }
    } else if (MenuKeys_HasDown(pad)) {
        if (!cursorRowDown(0, 3, 4)) {
            s32 t = getCursorTab();
            if (t != -1) {
#ifdef N3DS_DIRECT
                if (t == N3DS_OPT_TAB) t = 0;  // (down from the options button as from the pockets' tab)
#endif
                moveCursorFromTab(t);
            } else if (!cursorRowDown(0xc, 0xf, 0)) {
                u32 b = cursorSlot;
                if (b >= 9 && b <= 0xb) {
                    if (MenuKeys_HasLeft(pad)) {
                        cursorSlot = *(volatile u8 *)&cursorSlot + 1;
                    }
                    cursorSlot = *(volatile u8 *)&cursorSlot + 3;
                }
            }
        }
    }
}

BOOL DesignTab::moveCursorByPad(u32 pad, u32 f) {
    u32 old = cursorSlot;
    clearFlags(0x60);
    if (!moveCursorHorizontal(pad, f)) {
        moveCursorVertical(pad, f);
    }
    if (old != cursorSlot) {
        return TRUE;
    }
    return FALSE;
}

void DesignTab::startTargetBlink(u32 v) {
    if (testFlags(0x200)) {
        setTargetPalette(blinkTarget, 2);
    }
    blinkTarget = v;
    blinkTimer = 0xf;
    setFlags(0x200);
}

void DesignTab::updateTargetBlink() {
    if (testFlags(0x200)) {
        if (blinkTimer != 0) {
            blinkTimer = *(volatile u8 *)&blinkTimer - 1;
            switch (blinkTimer % 5) {
            case 0:
                setTargetPalette(blinkTarget, 2);
                break;
            case 3:
                setTargetPalette(blinkTarget, 6);
                break;
            }
        } else {
            clearFlags(0x200);
            setTargetPalette(blinkTarget, 2);
        }
    }
}

u32 DesignTab::getWornItem(s32 k) {
    void *p = PlayerData_GetCurrent();
    switch (k) {
    case 0:
        return *((PlayerData *)(p))->getShirt();
    case 1:
        return *((PlayerData *)(p))->getHat();
    case 2:
        return *((PlayerData *)(p))->getHeldItem();
    }
    return 0xfff1;
}

u32 DesignTab::swapWornItem(s32 k, u32 v) {
    u32 res = getWornItem(k);
    void *p = PlayerData_GetCurrent();
    volatile u16 t = v;
    switch (k) {
    case 0:
        ((PlayerData *)(p))->setShirt((u16 *)&t);
        t = res;
        if (!Unk_ov121_022924e0_Range(&t, 0x11a8, 0x12a7)) {
            res = 0xfff1;
        }
        break;
    case 1:
        ((PlayerData *)(p))->setHat((u16 *)&t);
        t = res;
        if (Unk_ov121_022924e0_Range(&t, 0x1429, 0x1430)) {
            res = 0xfff1;
        }
        break;
    case 2:
        t = res;
        if (Unk_ov121_022924e0_Range(&t, 0x13a0, 0x13a7)) {
            res = 0xfff1;
        }
        break;
    }
    return res;
}

BOOL DesignTab::requestWear(s32 k, u32 v) {
    u16 t = v;
    switch (k) {
    case 0:
        if (PlayerActor_RequestWearShirt(&t)) {
            return TRUE;
        }
        break;
    case 1:
        if (PlayerActor_RequestWearHat(&t)) {
            return TRUE;
        }
        break;
    case 2:
        if (PlayerActor_RequestChangeHeldItem(&t)) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL DesignTab::isWearDone(s32 k) {
    if (k == 2) {
        if (PlayerActor_IsChangingHeldItem() == 0) {
            return TRUE;
        }
        return FALSE;
    }
    if (PlayerActor_IsChangingClothes() == 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL DesignTab::isRoomEditAllowed() {
    CommManager *c = gCommManager;
    if (((CommManager *)(c))->isOnline()) {
        if (c->myAid != 0 || Room_CountOccupants() > 1) {
            return FALSE;
        }
    } else if (Room_CountOccupants() > 1) {
        return FALSE;
    }
    return TRUE;
}

BOOL DesignTab::testFlags(u32 mask) {
    if (flags & mask) {
        return TRUE;
    }
    return FALSE;
}

void DesignTab::setFlags(u32 mask) { flags = flags | mask; }

void DesignTab::clearFlags(u32 mask) { flags = flags & ~mask; }

extern "C" const u8 sDesignTabPopupChoices[6][0xb] = {
    {0x31, 0x32, 0x33, 0xff, 0xff, 0x00, 0x01, 0x08, 0x08, 0x08, 0x00},
    {0x31, 0x32, 0x33, 0xff, 0xff, 0x02, 0x03, 0x08, 0x08, 0x08, 0x00},
    {0x28, 0x33, 0xff, 0xff, 0xff, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00},
    {0x2a, 0x2f, 0x33, 0xff, 0xff, 0x05, 0x06, 0x08, 0x08, 0x08, 0x00},
    {0x28, 0x29, 0x33, 0xff, 0xff, 0x07, 0x06, 0x08, 0x08, 0x08, 0x00},
    {0x7c, 0xff, 0xff, 0xff, 0xff, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00},
};

extern "C" u16 sDesignItemBase[4] = {0xa7, 0xaf, 0xb7, 0xbf};

extern "C" const u16 sDesignTabTargetTileX[7] = {8, 0xe, 0x14, 5, 0xb, 0x11, 0x17};

extern "C" const u8 sDesignTabTabDownSlot[8] = {1, 1, 1, 2, 2, 3, 3, 3};

extern "C" const u8 sDesignTabSlotX[16] = {0x28, 0x60, 0x98, 0xd0, 0x38, 0x70, 0xa8, 0xe0, 0x10, 0x50, 0x80, 0xb0, 0x38, 0x68, 0x98, 0xc8};

extern "C" const u8 sDesignTabSlotY[16] = {0x7c, 0x7c, 0x7c, 0x7c, 0xa4, 0xa4, 0xa4, 0xa4, 0xb0, 0x30, 0x30, 0x30, 0x50, 0x50, 0x50, 0x50};

extern "C" const u16 sDesignTabTargetTileY[7] = {4, 4, 4, 8, 8, 8, 8};

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov121_02292360) {
    HOST_OVERLAY_VAR(sDesignTabProfile),
    HOST_OVERLAY_VAR(sDesignTabIconCell),
    HOST_OVERLAY_VAR(sDesignTabTargetFrameCells),
    HOST_OVERLAY_VAR(sDesignItemBase),
    HOST_OVERLAY_VARS_END
};

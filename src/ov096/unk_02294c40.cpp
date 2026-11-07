#include "uptr.h"
#include "host_shared_calls.h"
#include "types.h"
#include "n3ds_options.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
// the options button as a hand target in the tab row, left of the pockets' tab (0x26 and 0x27 are taken): a tab target
// whose tab is N3DS_OPT_TAB
#define POCKET_OPT_TARGET 0x28
#define POCKET_TAB_OF(id) ((id) == POCKET_OPT_TARGET ? N3DS_OPT_TAB : (id) - 0x19)
#else
#define POCKET_TAB_OF(id) ((id) - 0x19)
#endif
#include "Unk_020d8c7c.h"
#include "net/CommManager.h"
#include "ui/UiWidget.h"
#include "menu/MenuProc.h"
#include "menu/MenuCursor.h"
#include "menu/InventoryItemGrid.h"
#include "menu/LetterGrid.h"
#include "menu/InventoryBg.h"
#include "ui/TouchPromptBalloon.h"
#include "ui/CursorMotion.h"
#include "ui/LetterRenderer.h"
#include "item/Letter.h"
#include "menu/MenuLabelButton.h"
#include "menu/PopupChoiceMenu.h"
#include "menu/MenuErrorMessage.h"
#include "menu/PocketMenu.h"
#include "sys/ProcProfile.h"
#include "item/LetterView.h"
#include "menu/MenuTabBar.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "ui/HandCursor.h"
#include "ui/HudController.h"
#include "ui/LabelBalloon.h"
#include "ui/LabelString.h"
#include "ui/LabelButton.h"
#include "ui/AddresseePageArg.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02065330.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206db88.h"
#include "proto/main/unk_0206f4f0.h"
#include "proto/main/unk_02089fbc.h"
#include "proto/main/unk_02094810.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_02098e90.h"
#include "proto/main/unk_020b7d10.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov002/unk_ov002_022013ac.h"
#include "proto/ov002/unk_ov002_022025cc.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov094/unk_ov094_02292360.h"
#include "proto/ov094/unk_ov094_02292da8.h"
#include "proto/ov094/unk_ov094_02293b90.h"
#include "proto/ov096/unk_02294c40.h"
#include "proto/ov097/unk_0229b280.h"
#include "proto/ov098/unk_ov098_0229b280.h"

class PocketMenu;
typedef void (PocketMenu::*Unk_ov096_0229aea8_Fn)();

struct Unk_ov096_02297fb8_Msg {
    u8 a;
    u8 b;
};



static inline BOOL IsZero(u8 v)
{
    return v == 0 ? TRUE : FALSE;
}

static inline BOOL Unk_ov096_0229590c_IsZero(u8 v) { return v == 0 ? TRUE : FALSE; }

static inline BOOL Unk_ov096_02295a44_Range(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 a = *p;
    u32 b = *p;
    if (b >= lo && a <= hi) r = TRUE;
    return r;
}

static inline BOOL Unk_ov096_022968bc_InRange(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 x = *p;
    u32 y = *p;
    if (y >= lo && x <= hi) {
        r = TRUE;
    }
    return r;
}

extern "C" {
extern CommManager *gCommManager;
extern u8 gFieldSceneKind;
extern u8 gU8None;
extern u16 gPad[];
void Camera_PopView();
void Camera_PushView();
BOOL Camera_IsViewPushed();
void FieldAction_Release(s32 a);
s32 FieldAction_PollDrop(s32 a);
s32 FieldAction_RequestDrop(s32 a, s32 b);
s32 FieldAction_PollResult(s32 a);
s32 Item_MakePaper(s32 a, s32 b);
u32 Item_GetPaperCount(u16 *p);
s32 Item_GetPaperIndex(u16 *p);
u32 Item_FindMoneyBagForAmount(u32 v, s32 a, s32 *out);
BOOL func_0204bab8(u16 *p);
BOOL Item_IsHoldable(u16 *p);
u32 Item_GetPrice(u16 *p);
s32 Scene_InHouseRoom();
// ov002's popup choice menu functions (their definitions use a file-local typedef: not in the shared headers)
BOOL PopupChoice_MoveCursor(PopupChoiceMenuBody *self, s32 p, u8 *pos, u32 n);
BOOL PopupChoice_TickDecideDelay(PopupChoiceMenuBody *self);
void PopupChoice_DecideAddressee(PopupChoiceMenuBody *self, u32 x);
u8 PopupChoice_DecideCancel(PopupChoiceMenuBody *self, s32 x);
void PopupChoice_DecideRow(PopupChoiceMenuBody *self, s32 a, s32 b);
void PopupChoice_ForceClose(PopupChoiceMenuBody *self);
void PopupChoice_Draw(PopupChoiceMenuBody *self);
void PopupChoice_Update(PopupChoiceMenuBody *self);
void PopupChoice_Close(PopupChoiceMenuBody *self, s32 x);
void PopupChoice_Open(PopupChoiceMenuBody *self, s32 x);
void PopupChoice_OpenAddresseePage(PopupChoiceMenuBody *self, AddresseePageArg a, s32 x);
}

// The popup choice menu's methods are declared under PopupChoiceMenuBody (the same object; no fields of its own).
static inline PopupChoiceMenuBody *PocketMenu_Popup(PocketMenu *s) {
    return static_cast<PopupChoiceMenuBody *>(&s->m_24fc);
}

static inline BOOL Unk_ov096_02299778_Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) return TRUE;
    return FALSE;
}






extern "C" ProcProfile sPocketMenuProfile = {(void *(*)())PocketMenu_Create, 0x91, 0x95};

static inline BOOL Unk_ov096_0229619c_Range(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 b = *p;
    u32 a = *p;
    if (a >= lo && b <= hi) {
        r = TRUE;
    }
    return r;
}

static inline BOOL Unk_ov096_0229652c_IsZero(u8 v) { return v == 0 ? TRUE : FALSE; }

static inline s32 Unk_ov096_022969bc_Idx(u32 v) {
    if (v >= 0x1531 && v <= 0x153a) {
        return v - 0x1531;
    }
    return -1;
}

static inline u16 Unk_ov096_022969bc_Ch(s32 n) {
    if ((u32)n < 10) {
        return n + 0x1531;
    }
    return 0x1531;
}

// ===== unit 022971dc =====
static inline BOOL Unk_ov096_022979f0_InRange(volatile u16 *p, u32 lo, u32 hi) {
    BOOL r = FALSE;
    u32 x = *p;
    u32 y = *p;
    if (y >= lo && x <= hi) {
        r = TRUE;
    }
    return r;
}

inline PocketMenu::PocketMenu()
    : bgTasks(), m_358(), m_db8(), m_de0(), m_23c0(), m_2480(), m_2498(), m_24fc(), errorMessage(), m_2904(), m_2b14(),
      digUnitX(0), digUnitY(0), m_2b98(), m_2c8c() {}

extern "C" PocketMenu *PocketMenu_Create() {
    return new PocketMenu;
}

BOOL PocketMenu::onCreate() {
    initPocketMenu();
    setTransitionState(0);
    setPhase(1);
    return TRUE;
}

BOOL PocketMenu::onDelete() {
    ((MenuTabBar *)(ProcBase_GetParent(this)))->onTabMenuClosed();
    releaseResources();
    MenuCtrl_SetHandBells(0);
    return TRUE;
}

// ===== unit 0229a94c =====
BOOL PocketMenu::onDraw() {
    PopupChoice_Draw(PocketMenu_Popup(this));
    if (!testFlags(1)) {
        return TRUE;
    }
    (&m_23c0)->draw();
    if (MenuCtrl_IsButtons()) {
        m_2498.drawWrapped();
    }
    drawHand();
    if (testFlags(2)) {
        InventoryItemGrid_DrawPockets(&m_358, 0, slideY);
        Shared_InventoryItemGrid_DrawExtraMarks(&m_358, 0, slideY);
        m_db8.drawPocketLetters(0, slideY);
        Shared_InventoryBg_DrawSprite(&m_de0, slideY);
    }
    if (testFlags(0x100)) {
        m_2b14.setPos(0, getSlideOffsetY());
        (&m_2b14)->draw();
    }
    return TRUE;
}

BOOL PocketMenu::execTransition() {
    static Unk_ov096_0229aea8_Fn tbl[11] = {
        &PocketMenu::stateLoadBg,
        &PocketMenu::stateLoadBgChars,
        &PocketMenu::stateLoadObj,
        &PocketMenu::stateSlideIn,
        &PocketMenu::stateWaitSlideIn,
        &PocketMenu::stateSlideOut,
        &PocketMenu::stateWaitSlideOut,
        &PocketMenu::stateOpenLetterView,
        &PocketMenu::stateWaitLetterView,
        &PocketMenu::stateCloseLetterView,
        &PocketMenu::stateWaitCloseLetterView};
    preStateUpdate();
    (this->*tbl[transitionState])();
    postStateUpdate();
    return TRUE;
}

void PocketMenu::runMainState() {
    static Unk_ov096_0229aea8_Fn tbl[58] = {
        &PocketMenu::mainAct00,
        &PocketMenu::mainAct01,
        &PocketMenu::mainAct02,
        &PocketMenu::mainAct03,
        &PocketMenu::mainAct04,
        &PocketMenu::mainAct05,
        &PocketMenu::mainAct06,
        &PocketMenu::mainAct07,
        &PocketMenu::mainAct08,
        &PocketMenu::mainAct09,
        &PocketMenu::mainAct0A,
        &PocketMenu::mainAct0B,
        &PocketMenu::mainAct0C,
        &PocketMenu::mainAct0D,
        &PocketMenu::mainAct0E,
        &PocketMenu::mainAct0F,
        &PocketMenu::mainAct10,
        &PocketMenu::mainAct11,
        &PocketMenu::mainAct12,
        &PocketMenu::mainAct13,
        &PocketMenu::mainAct14,
        &PocketMenu::mainAct15,
        &PocketMenu::mainAct16,
        &PocketMenu::mainAct17,
        &PocketMenu::mainAct18,
        &PocketMenu::mainAct19,
        &PocketMenu::mainAct1A,
        &PocketMenu::mainAct1B,
        &PocketMenu::mainAct1C,
        &PocketMenu::mainAct1D,
        &PocketMenu::mainAct1E,
        &PocketMenu::mainAct1F,
        &PocketMenu::mainAct20,
        &PocketMenu::mainAct21,
        &PocketMenu::mainAct22,
        &PocketMenu::mainAct23,
        &PocketMenu::mainAct24,
        &PocketMenu::mainAct25,
        &PocketMenu::mainAct26,
        &PocketMenu::mainAct27,
        &PocketMenu::mainAct28,
        &PocketMenu::mainAct29,
        &PocketMenu::mainAct2A,
        &PocketMenu::mainAct2B,
        &PocketMenu::mainAct2C,
        &PocketMenu::mainAct2D,
        &PocketMenu::mainAct2E,
        &PocketMenu::mainAct2F,
        &PocketMenu::mainAct30,
        &PocketMenu::mainAct31,
        &PocketMenu::mainAct32,
        &PocketMenu::mainAct33,
        &PocketMenu::mainAct34,
        &PocketMenu::mainAct35,
        &PocketMenu::mainAct36,
        &PocketMenu::mainAct37,
        &PocketMenu::mainAct38,
        &PocketMenu::mainAct39};
    (this->*tbl[mainState])();
}

BOOL PocketMenu::execMain() {
    if (handleTabSwitch()) {
        return TRUE;
    }
    preInputUpdate();
    runMainState();
    postInputUpdate();
    return TRUE;
}

BOOL PocketMenu::execPhase3() { return TRUE; }

BOOL PocketMenu::execPhase4() { return TRUE; }

BOOL PocketMenu::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

BOOL PocketMenu::handleTabSwitch() {
    MenuCtrl_TickForceClose();
    if (MenuCtrl_IsForceCloseDue()) {
        if (mainState == 0 || mainState == 1 || mainState == 0xa) {
            return requestTab(7);
        }
    }
    if (mainState != 0 && mainState != 0xa) {
        return FALSE;
    }
    s32 t = -1;
    if (MenuCtrl_IsTouch()) {
        t = MenuTabBar_HitTestTouch();
    } else {
        u16 v = gPad[1];
        if (v & 0x800) {
            t = 7;
        } else if (v & 0x400) {
            t = 5;
        } else if (v & 4) {
            t = 4;
        }
    }
    return requestTab(t);
}

BOOL PocketMenu::requestTab(s32 a) {
    void *o = ProcBase_GetParent(this);
    if (a != -1) {
        if (a != 0) {
            ((MenuTabBar *)(o))->selectTab((u8)a);
            transitionState = 5;
            setPhase(1);
            if (a != 7) {
                requestCameraPop();
            }
            clearFlags(0x80);
            return TRUE;
        }
    }
    return FALSE;
}

void PocketMenu::stateLoadBg() {
    setupBgLayer();
    Shared_InventoryBg_LoadBg(&m_de0, 1);
    setTransitionState(1);
    if (testFlags(0x80000) == 0) {
        stateLoadBgChars();
    }
}

void PocketMenu::stateLoadBgChars() {
    InventoryBg_LoadGraphics(&m_de0);
    setTransitionState(2);
    if (testFlags(0x80000) == 0) {
        stateLoadObj();
    }
}

void PocketMenu::stateLoadObj() {
    loadObjGraphics();
    setTransitionState(3);
    if (testFlags(0x80000) == 0) {
        stateSlideIn();
        clearFlags(0x80000);
    }
}

void PocketMenu::stateSlideIn() {
    Shared_InventoryItemGrid_LoadPockets(&m_358);
    Shared_LetterGrid_LoadPocketLetters(&m_db8);
    beginSubSlideIn(8, 3, 0, 0x30);
    Gfx2d_ShowLayer(6);
    applySlideOffset(6, 0, 0);
    setTransitionState(4);
    setFlags(1);
    setFlags(2);
    slideY = getSlideOffsetY();
}

void PocketMenu::stateWaitSlideIn() {
    if (stepSlideIn(0)) {
        setPhase(2);
        PocketMenu_ReturnToIdle(this);
    }
    applySlideOffset(6, 0, 0);
    slideY = getSlideOffsetY();
}

void PocketMenu::stateSlideOut() {
    m_23c0.hide(1);
    hideCursor();
    beginSubSlideOut(8, 0, 0, 0x30);
    applySlideOffset(6, 0, 0);
    setTransitionState(6);
    slideY = getSlideOffsetY();
}

void PocketMenu::stateWaitSlideOut() {
    if (stepSlideOut(0)) {
        Gfx2d_ResetLayer(6);
        clearFlags(2);
        if (testFlags(0x80)) {
            setTransitionState(7);
        } else {
            setPhase(5);
            clearFlags(1);
        }
    } else {
        applySlideOffset(6, 0, 0);
    }
    slideY = getSlideOffsetY();
}

void PocketMenu::stateOpenLetterView() {
    LetterView *r4 = getActionLetter();
    Letter_MarkRead(r4);
    m_2904.show(r4, 3, 4, 1);
    beginSubSlideIn(3, 0, 0, 0x30);
    Gfx2d_ShowLayer(3);
    applySlideOffset(3, 0, 0);
    Gfx2d_ShowLayer(4);
    applySlideOffset(4, 0, 0);
    setTransitionState(8);
    m_2b14.showDefault(0x88);
    setFlags(0x100);
}

void PocketMenu::stateWaitLetterView() {
    if (stepSlideIn(0)) {
        setPhase(2);
        if (MenuCtrl_IsTouch()) {
            setMainState(7);
        } else {
            setMainState(0x17);
        }
    } else {
        applySlideOffset(3, 0, 0);
        applySlideOffset(4, 0, 0);
    }
}

void PocketMenu::stateCloseLetterView() {
    beginSubSlideOut(3, 0, 0, 0x30);
    applySlideOffset(3, 0, 0);
    applySlideOffset(4, 0, 0);
    setTransitionState(0xa);
}

void PocketMenu::stateWaitCloseLetterView() {
    if (stepSlideOut(0)) {
        Gfx2d_ResetLayer(3);
        Gfx2d_ResetLayer(4);
        clearFlags(0x100);
        void *r4 = ProcBase_GetParent(this);
        if (testFlags(0x20000)) {
            setPhase(5);
            clearFlags(1);
        } else {
            ((MenuTabBar *)(r4))->showTabs();
            stateLoadBg();
        }
    } else {
        applySlideOffset(3, 0, 0);
        applySlideOffset(4, 0, 0);
    }
}

// ===== unit 0229a000 =====

void PocketMenu::hideTabBar() {
    ((MenuTabBar *)(ProcBase_GetParent(this)))->hideTabs();
}

void PocketMenu::initPocketMenu() {
    stateFlags = 0;
    InventoryItemGrid_Init(&m_358, 2);
    m_db8.init(2);
    Shared_InventoryBg_Init(&m_de0, 6);
    balloonTarget = 0x26;
    m_23c0.setPriority(2);
    m_2480.reset();
    clearHand();
    cursorTarget = MenuCtrl_GetSavedSlot();
    m_24fc.init(3, 1, (const char *)(0));
    PocketMenu_Popup(this)->buildAddresseeList();
    m_2904.setLayer(3);
    optionsOpenDelay = 0;
    if (((MenuTabBar *)(ProcBase_GetParent(this)))->isJustOpened()) {
        setFlags(0x80000);
    }
}

void PocketMenu::releaseResources() {
    PocketMenu_CancelVramTasks(this);
    InventoryBg_Exit(&m_de0);
    Shared_InventoryItemGrid_Exit(&m_358);
    PopupChoice_ForceClose(PocketMenu_Popup(this));
    m_2904.release();
}

void PocketMenu::preInputUpdate() {
    preStateUpdate();
    (&m_2498)->update();
}

void PocketMenu::postInputUpdate() {
    postStateUpdate();
}

void PocketMenu::preStateUpdate() {
    PocketMenu_CancelVramTasks(this);
    Shared_InventoryBg_PreUpdate(&m_de0);
    InventoryItemGrid_PreUpdate(&m_358);
    m_db8.updateCursorLift();
}

void PocketMenu::postStateUpdate() {
    updateCameraView();
    Shared_PopupChoice_Update(PocketMenu_Popup(this));
    InventoryBg_Update(&m_de0);
    if (m_23c0.updatePrompt()) {
        positionLabelBalloon();
    }
}

void PocketMenu::setupBgLayer() {
    Gfx2d_SetLayerPriority(6, 2);
    Gfx2d_SetLayerControl(6, 0, 0, 0);
}

void PocketMenu::loadObjGraphics() {
    InventoryBg_LoadObjGraphics();
}

void PocketMenu::mainAct00() {
    if (checkSwitchToButtons(1)) {
        cursorTarget = 0;
        PocketMenu_EnterButtonIdle(this);
    } else if (Unk_ov096_02299778_Both()) {
        u8 a = gTouchCurX;
        u8 b = gTouchCurY;
        s32 t = PocketMenu_HitPocket(this, a, b, 1);
        if (t != 0x26) {
            PocketMenu_BeginTouchOnTarget(this, t);
            return;
        }
        t = PocketMenu_HitLetter(this, a, b, 1);
        if (t != 0x26) {
            PocketMenu_BeginTouchOnTarget(this, t);
            return;
        }
        if (PocketMenu_HitWalletIcon(this, a, b)) {
            openTargetOptions(0x25, 0);
            return;
        }
        switch (PocketMenu_HitWalletAmount(this, a, b)) {
        case 1:
            openTargetOptions(0x27, 0);
            break;
        case 2:
            setMainState(9);
            Shared_InventoryBg_SetBellsPanelMode(&m_de0, bellsPanelMode);
            break;
        default:
            if (PocketMenu_HitPlayer(this, a, b)) {
                openTargetOptions(0x24, 0);
            }
            break;
        }
    }
}

void PocketMenu::mainAct01() {
    if (gTouchHeld == 0) {
        if (testFlags(0x10000)) {
            setMainState(0);
            m_23c0.setAutoCloseTimer(0x3c);
        } else {
            setMainState(3);
            runMainState();
        }
        return;
    }
    if (testFlags(4) == 0) goto stop;
    if (hasTouchMoved()) {
        PocketMenu_StartTouchDrag(this, touchedTarget);
        return;
    }
    if (testFlags(0x10000) != 0) goto stop;
    if (m_23c0.isOpenOrOpening() == 0) goto stop;
    if (optionsOpenDelay != 0) {
        optionsOpenDelay = optionsOpenDelay - 1;
    } else {
        openTargetOptions(touchedTarget, 1);
        setMainState(2);
    }
    return;
stop:
    m_23c0.commitOpen();
}

void PocketMenu::mainAct02() {
    if (MenuCtrl_IsForceCloseDue()) {
        setMainState(4);
    } else if (gTouchHeld == 0) {
        setMainState(4);
    } else if (testFlags(4)) {
        if (hasTouchMoved()) {
            PocketMenu_StartTouchDrag(this, touchedTarget);
            PopupChoice_Close(PocketMenu_Popup(this), 0);
            m_23c0.hide(1);
        }
    }
}

void PocketMenu::mainAct03() {
    if (m_23c0.isOpenOrOpening()) {
        if (optionsOpenDelay != 0) {
            optionsOpenDelay = optionsOpenDelay - 1;
        } else {
            openTargetOptions(touchedTarget, 1);
            setMainState(2);
        }
    }
}

void PocketMenu::mainAct04() {
    if (PocketMenu_Popup(this)->isOpen()) {
        if (MenuCtrl_IsForceCloseDue()) {
            cancelOptions();
        } else if (checkSwitchToButtons(1)) {
            cancelOptions();
        } else if (Unk_ov096_02299778_Both()) {
            s32 t = PocketMenu_Popup(this)->hitTestRowOrLast(gTouchCurX, gTouchCurY);
            if (t >= 0) {
                if (testFlags(0x40000) == 0 || t != 0) {
                    chosenAction = optionList[5 + t];
                    s32 u = playActionSe();
                    PopupChoice_DecideRow(PocketMenu_Popup(this), t, u);
                    setMainState(0x1e);
                }
            }
        }
    }
}

void PocketMenu::mainAct05() {
    if (MenuCtrl_IsForceCloseDue()) {
        putHandBack(handSource, 1);
        requestTab(7);
        return;
    }
    syncHandFromTouch();
    clearHighlights();
    s32 x = handX + 8;
    s32 y = handY + 8;
    s32 t = PocketMenu_HitPocket(this, x, y, 0);
    if (t == 0x26) {
        t = PocketMenu_HitLetter(this, x, y, 0);
    }
    if (t != 0x26) {
        if (gTouchHeld == 0) {
            if (checkSwap(t, handSource) == 0) {
                if (PocketMenu_IsLetterTarget(this, t)) {
                    requestCameraPop();
                }
                PocketMenu_DropItemAt(this, t);
                if (mainState == 5) {
                    PocketMenu_ReturnToIdle(this);
                }
                if (PocketMenu_IsPocketTarget(this, t) == 0 && PocketMenu_IsLetterTarget(this, t) == 0) {
                    return;
                }
                Inventory_PlayPutDownSe();
            } else {
                PocketMenu_FlyHandTo(this, handSource, 4);
            }
        } else {
            highlightTarget(t);
        }
    } else {
        t = PocketMenu_HitSpecialTarget(this, x, y);
        if (t != 0x26) {
            if (gTouchHeld == 0) {
                if ((u8)(t + 0xde) <= 1) {
                    if (canDropHeldItem() == 0) {
                        PocketMenu_FlyHandTo(this, handSource, 4);
                    } else if (dropHeldItem()) {
                        PocketMenu_ReturnToIdle(this);
                    }
                } else if (t == 0x24) {
                    useOnPlayerKind = getUseOnPlayerKind();
                    u32 v = useOnPlayerKind;
                    if (v < 1) {
                        cancelUseOnPlayer();
                    } else if (v == 1) {
                        PocketMenu_FlyHandTo(this, handSource, 4);
                    } else {
                        startUseOnPlayer();
                    }
                } else if (t == 0x25) {
                    if (isHoldingMoneyBag(0)) {
                        depositHeldBag();
                    } else {
                        PocketMenu_FlyHandTo(this, handSource, 4);
                    }
                } else {
                    PocketMenu_FlyHandTo(this, handSource, 4);
                }
            } else {
                highlightTarget(t);
            }
        } else if (PocketMenu_HitShirtBox(this, x, y)) {
            if (gTouchHeld == 0) {
                if (isHoldingShirt()) {
                    wearHeldShirt();
                    PocketMenu_ReturnToIdle(this);
                } else {
                    PocketMenu_FlyHandTo(this, handSource, 4);
                }
            } else {
                highlightTarget(0x21);
            }
        } else if (gTouchHeld == 0) {
            PocketMenu_FlyHandTo(this, handSource, 4);
        }
    }
}

void PocketMenu::mainAct06() {
    if (MenuCtrl_IsForceCloseDue()) {
        putHandBack(handSource, 1);
        requestTab(7);
    } else {
        syncHandFromTouch();
        clearHighlights();
        s32 t = PocketMenu_HitLetter(this, handX + 8, handY + 8, 0);
        if (t != 0x26) {
            if (gTouchHeld == 0) {
                if (PocketMenu_DropLetterAt(this, t) == 0) {
                    PocketMenu_FlyHandTo(this, handSource, 4);
                }
                Inventory_PlayPutDownSe();
                PocketMenu_ReturnToIdle(this);
            } else {
                highlightTarget(t);
            }
        } else if (gTouchHeld == 0) {
            PocketMenu_FlyHandTo(this, handSource, 4);
        }
    }
}

void PocketMenu::mainAct07() {
    if (MenuCtrl_IsForceCloseDue()) {
        closeLetterViewAndMenu();
    } else if (checkSwitchToButtons(1)) {
        setMainState(0x17);
    } else if (m_2b14.isTouched()) {
        closeLetterView();
    }
}

void PocketMenu::mainAct08() {
    if (MenuCtrl_IsForceCloseDue()) {
        closeAddresseeList();
    } else if (checkSwitchToButtons(1)) {
        PocketMenu_EnterAddresseeButtons(this);
    } else if (Unk_ov096_02299778_Both()) {
        s32 t = PocketMenu_Popup(this)->hitTestRow(gTouchCurX, gTouchCurY);
        if (t >= 0) {
            PopupChoice_DecideAddressee(PocketMenu_Popup(this), t);
            addressee = PocketMenu_Popup(this)->pickAddressee(addresseePage, (u8)t);
            setMainState(0x23);
        } else {
            closeAddresseeList();
        }
    }
}

// ===== unit 022996e8 =====

void PocketMenu::mainAct09() {
    if (MenuCtrl_IsForceCloseDue()) {
        requestTab(7);
        Shared_InventoryBg_SetBellsPanelMode(&m_de0, 0);
    } else if (gTouchHeld == 0) {
        setMainState(0);
        Shared_InventoryBg_SetBellsPanelMode(&m_de0, 0);
    } else if (isTouchHeldFor(4)) {
        PocketMenu_BeginTouchOnTarget(this, 0x25);
        grabOffsetX = -8;
        grabOffsetY = -8;
        PocketMenu_StartTouchDrag(this, 0x25);
        setFlags(0x4000);
        Shared_InventoryBg_SetBellsPanelMode(&m_de0, bellsPanelMode);
    }
}

void PocketMenu::mainAct0A() {
    if (checkSwitchToTouch()) {
        PocketMenu_EnterTouchIdle(this);
        m_23c0.hide(1);
    } else {
        s32 t = takeRepeatedKeys();
        if (moveCursorByPad(t, 0)) {
            updateLabelBalloon();
            moveCursorToTarget();
            m_23c0.hide(0);
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                if (PocketMenu_IsPocketTarget(this, cursorTarget) || PocketMenu_IsLetterTarget(this, cursorTarget)) {
                    if (PocketMenu_IsSlotEmpty(this, cursorTarget) == 0) {
                        openTargetOptions(cursorTarget, 0);
                    }
                } else if (PocketMenu_IsTabTarget(this, cursorTarget)) {
                    pressTab();
                } else if (cursorTarget == 0x25) {
                    openTargetOptions(0x25, 0);
                } else if (cursorTarget == 0x24) {
                    openTargetOptions(0x24, 0);
                }
            } else if (k & 0x100) {
                requestTab(MenuTabBar_NextTab(0));
            } else if (k & 0x200) {
                requestTab(MenuTabBar_PrevTab(0));
            } else if (k & 2) {
                requestTab(7);
            } else {
                m_23c0.commitOpen();
            }
        }
    }
}

void PocketMenu::mainAct0B() {
    if (MenuCtrl_IsForceCloseDue()) {
        putHandBack(handSource, 1);
        requestTab(7);
    } else {
        s32 t = takeRepeatedKeys();
        if (moveCursorByPad(t, 1)) {
            updateLabelBalloon();
            moveCursorToTarget();
            m_23c0.hide(0);
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                if (cursorTarget == 0x21) {
                    if (isHoldingShirt() == 0) {
                        return;
                    }
                    requestCameraPop();
                    beginPutDownAt(cursorTarget);
                } else if (PocketMenu_IsSpecialTarget(this, cursorTarget)) {
                    u32 b = cursorTarget;
                    if (b == 0x24) {
                        useOnPlayerKind = getUseOnPlayerKind();
                        beginPutDownAt(cursorTarget);
                    } else if (b == 0x22) {
                        requestCameraPop();
                        beginPutDownAt(cursorTarget);
                    } else if (b == 0x25) {
                        if (isHoldingMoneyBag(1) == 0 && handSource == 0x25) {
                            Snd_PlaySe(0x2a);
                        } else {
                            requestCameraPop();
                            beginPutDownAt(cursorTarget);
                        }
                    }
                } else if (checkPlace(cursorTarget, handItem, handItemFlags) == 0) {
                    if (PocketMenu_IsLetterTarget(this, cursorTarget)) {
                        requestCameraPop();
                    }
                    if (PocketMenu_IsPocketTarget(this, cursorTarget)) {
                        u16 v;
                        u32 w;
                        v = PocketMenu_GetItem(this, cursorTarget);
                        w = PocketMenu_GetItemFlags(this, cursorTarget);
                        if (mergeItems((u16 *)(&handItem), handItemFlags, &v, w) == 0) {
                            InventoryItemGrid_SetSlotItem(&m_358, PocketMenu_TargetToGridIndex(this, cursorTarget), v, w);
                        }
                    }
                    if (PocketMenu_GetItem(this, cursorTarget) == 0xfff1) {
                        beginPutDownAt(cursorTarget);
                    } else {
                        if (swapTarget != 0x26 && isHoldingMoneyBag(1)) {
                            swapTarget = cursorTarget;
                            auxItem = handItem;
                        }
                        beginSwapAt(cursorTarget);
                    }
                }
            } else {
                if (k & 2) {
                    if (checkPlaceHand(handSource) == 0) {
                        beginPutDownAt(handSource);
                    } else {
                        u32 v = swapTarget;
                        if (v != 0x26) {
                            u32 o = handSource;
                            handSource = v;
                            beginPutDownAt(handSource);
                            swapTarget = o;
                            setFlags(0x2000);
                        }
                    }
                }
                syncHandFromCursor();
                m_23c0.commitOpen();
            }
        }
    }
}

void PocketMenu::mainAct0C() {
    if (MenuCtrl_IsForceCloseDue()) {
        putHandBack(handSource, 1);
        requestTab(7);
    } else {
        s32 t = takeRepeatedKeys();
        if (moveCursorByPad(t, 2)) {
            updateLabelBalloon();
            moveCursorToTarget();
            m_23c0.hide(0);
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                if (PocketMenu_IsSlotEmpty(this, cursorTarget)) {
                    beginPutDownAt(cursorTarget);
                } else {
                    beginSwapAt(cursorTarget);
                }
            } else if (k & 2) {
                beginPutDownAt(handSource);
            } else {
                syncHandFromCursor();
                m_23c0.commitOpen();
            }
        }
    }
}

void PocketMenu::mainAct0D() {
    if (MenuCtrl_IsForceCloseDue()) {
        cancelOptions();
    } else if (checkSwitchToTouch()) {
        cancelOptions();
    } else {
        s32 t = takeRepeatedKeys();
        u8 f = testFlags(0x40000);
        if (PopupChoice_MoveCursor(PocketMenu_Popup(this), t, &popupRow, f)) {
            moveCursorToPopupRow();
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                m_2498.setPosePress();
                setMainState(0xe);
            } else if (k & 2) {
                cancelPopup();
            }
        }
    }
}

void PocketMenu::mainAct0E() {
    if (m_2498.isAnimDone()) {
        chosenAction = optionList[5 + popupRow];
        s32 u = playActionSe();
        PopupChoice_DecideRow(PocketMenu_Popup(this), popupRow, u);
        setMainState(0x1e);
    }
}

void PocketMenu::mainAct0F() {
    if (m_2498.isMoving() == 0) {
        setMainState(returnState);
        if ((u8)(returnState + 0xf6) <= 2) {
            selectTarget(cursorTarget);
        }
        runMainState();
    }
    syncHandFromCursor();
}

void PocketMenu::mainAct10() {
    if (m_2498.isAnimDone()) {
        if (requestTab(POCKET_TAB_OF(cursorTarget)) == 0) {
            releaseCursor();
        }
    }
}

void PocketMenu::mainAct11() {
    if (m_2498.isAnimDone()) {
        refreshCursor();
        setMainState(0xa);
    }
}

void PocketMenu::mainAct12() {
    if (m_2498.isGripping()) {
        PocketMenu_StartButtonDrag(this, cursorTarget);
        setMainState(0x13);
    }
}

void PocketMenu::mainAct13() {
    if (m_2498.isAnimDone()) {
        setMainState(returnState);
    }
    syncHandFromCursor();
}

void PocketMenu::mainAct14() {
    if (m_2498.isGripping() == 0) {
        u32 a = placeTarget;
        u32 b = cursorTarget;
        if (b == a) {
            placeHandAt(a);
            if (testFlags(0x2000)) {
                setHandItem(auxItem, 0);
                PocketMenu_FlyHandTo(this, swapTarget, 4);
                clearFlags(0x2000);
            }
        } else {
            PocketMenu_FlyHandTo(this, a, 4);
        }
        if (mainState == 0x14) {
            updateLabelBalloon();
            setMainState(0xa);
            Inventory_PlayPutDownSe();
        }
    } else {
        syncHandFromCursor();
    }
}

void PocketMenu::mainAct15() {
    if (m_2498.func_ov002_022028fc() == 0) {
        swapHandWith(placeTarget);
        setFlags(0x40);
        setMainState(0x16);
        updateLabelBalloon();
    } else {
        setMainState(0xa);
    }
}

void PocketMenu::mainAct16() {
    if (m_2498.isAnimDone()) {
        setMainState(returnState);
    }
    if (m_2498.isGripping()) {
        if (testFlags(0x40)) {
            clearFlags(0x40);
            Shared_Inventory_PlayPickUpSe();
        }
        syncHandFromCursor();
    }
}

void PocketMenu::mainAct17() {
    if (MenuCtrl_IsForceCloseDue()) {
        closeLetterViewAndMenu();
    } else if (MenuCtrl_IsForceCloseDue()) {
        closeLetterView();
        setFlags(0x20000);
    } else {
        if (m_2498.getAnim() == 0) {
            s32 a = m_2b14.getAnchorX(1);
            s32 b = m_2b14.getAnchorY(1);
            m_2498.warpTo(a, b);
            m_2498.setAnimIfChanged(1);
        }
        if (checkSwitchToTouch()) {
            hideCursor();
            setMainState(7);
        } else {
            u32 k = gPad[1];
            if ((k & 1) || (k & 2)) {
                m_2498.setPosePress();
                setMainState(0x18);
            }
        }
    }
}

void PocketMenu::mainAct18() {
    if (m_2498.isAnimDone()) {
        closeLetterView();
    }
}

void PocketMenu::mainAct19() {
    if (MenuCtrl_IsForceCloseDue()) {
        closeAddresseeList();
    } else if (checkSwitchToTouch()) {
        PocketMenu_EnterAddresseeTouch(this);
    } else {
        s32 t = takeRepeatedKeys();
        if (PopupChoice_MoveCursor(PocketMenu_Popup(this), t, &popupRow, 0)) {
            moveCursorToPopupRow();
        }
        u32 k = gPad[1];
        if (k & 1) {
            m_2498.setPosePress();
            setMainState(0x1a);
        } else if (k & 2) {
            closeAddresseeList();
        }
    }
}

// ===== unit 02298dac =====

void PocketMenu::mainAct1A() {
    if (m_2498.isAnimDone()) {
        PopupChoice_DecideAddressee(PocketMenu_Popup(this), popupRow);
        addressee = PocketMenu_Popup(this)->pickAddressee(addresseePage, popupRow);
        setMainState(0x23);
    }
}

void PocketMenu::mainAct1B()
{
    if (m_2480.update()) {
        if (PocketMenu_IsSpecialTarget(this, handSource) || testFlags(0x2000)) {
            handX = PocketMenu_GetTargetX(this, handSource);
            handY = PocketMenu_GetTargetY(this, handSource);
            setMainState(0x1c);
        } else {
            mainAct1C();
        }
    } else {
        syncHandFromMover();
    }
}

void PocketMenu::mainAct1C()
{
    putHandBack(handSource, 1);
    if (testFlags(0x2000)) {
        PocketMenu_FlyItemTo(this, handSource, swapTarget, auxItem, 0);
        clearFlags(0x2000);
    } else {
        Inventory_PlayPutDownSe();
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::mainAct1D()
{
    if (PocketMenu_Popup(this)->isOpen()) {
        if (MenuCtrl_IsButtons()) {
            cursorToPopupDefault();
            setMainState(0xd);
        } else {
            setMainState(4);
        }
    }
}

extern "C" s32 PocketMenu_TickOptionDecide(PocketMenu *s)
{
    if (PopupChoice_TickDecideDelay(PocketMenu_Popup(s))) {
        PopupChoice_Close(PocketMenu_Popup(s), 0);
        s->m_23c0.hide(1);
        if (s->m_2498.getAnim()) {
            s->placeCursorOnTarget();
        }
        return 1;
    }
    return 0;
}

void PocketMenu::mainAct1E()
{
    if (PocketMenu_TickOptionDecide(this)) {
        setMainState(0x1f);
    }
}

void PocketMenu::mainAct1F()
{
    if (PocketMenu_Popup(this)->isClosed()) {
        runChosenAction();
    }
}

void PocketMenu::mainAct20()
{
    if (errorMessage.update(1)) {
        setMainState(returnState);
        m_2498.enableObjWindow();
    }
}

void PocketMenu::mainAct21()
{
    if (m_2b14.stepAnim()) {
        if (m_2498.getAnim()) {
            s32 r4 = m_2b14.getAnchorX(1);
            m_2498.warpTo(r4, m_2b14.getAnchorY(1));
        }
    } else {
        hideCursor();
        setTransitionState(9);
        setPhase(1);
    }
}

void PocketMenu::mainAct22()
{
    if (PocketMenu_Popup(this)->isOpen()) {
        PocketMenu_EnterAddresseeInput(this);
    }
}

void PocketMenu::mainAct23()
{
    if (PocketMenu_TickOptionDecide(this)) {
        hideCursor();
        setMainState(0x24);
    }
}

void PocketMenu::mainAct24()
{
    if (PocketMenu_Popup(this)->isClosed()) {
        if (testFlags(0x200)) {
            if (PocketMenu_Popup(this)->getAddresseeKind(addressee) == 1) {
                writeLetterOnPaper();
                return;
            }
        }
        if (PocketMenu_Popup(this)->applyAddressee((void *)MenuCtrl_GetArg(), addressee) == 2) {
            addresseePage = addresseePage + 1;
            if (addresseePage >= PocketMenu_Popup(this)->getPageCount()) {
                addresseePage = 0;
            }
            openAddresseePage(0);
        } else {
            PocketMenu_ReturnToIdle(this);
        }
    }
}

void PocketMenu::mainAct25()
{
    u32 r4;
    handX = PocketMenu_GetTargetX(this, 0x24);
    handY = PocketMenu_GetTargetY(this, 0x24);
    r4 = useOnPlayerKind;
    if (r4 != 5 || IsZero(gFieldSceneKind)) {
        requestCameraPush();
    }
    if (requestUseOnPlayer(r4, handItem)) {
        setMainState(0x26);
        handItem = swapEquipped(r4, handItem);
        Shared_InventoryBg_StartBlink(&m_de0, 1);
    }
}

void PocketMenu::mainAct26()
{
    if (InventoryBg_UpdateBlink(&m_de0)) {
        clearHand();
        Shared_InventoryBg_LoadPictureForHeldItem(&m_de0);
        if (handItem != 0xfff1) {
            if (testFlags(0x8000)) {
                PocketMenu_FlyItemTo(this, 0x24, handSource, handItem, 0);
                return;
            }
            setHandItem(handItem, 0);
            returnHand(1);
        }
        setMainState(0x27);
        mainAct27();
    }
}

void PocketMenu::mainAct27()
{
    if (isUseOnPlayerBusy(useOnPlayerKind) == 0) {
        clearHand();
        InventoryBg_StopBlink(&m_de0);
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::mainAct2A()
{
    if (Shared_LetterGrid_UpdatePopAnim(&m_db8)) {
        clearHand();
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::mainAct2B()
{
    if (Shared_InventoryItemGrid_UpdatePresentAnim(&m_358)) {
        void *p = PlayerData_GetCurrent();
        u32 v = handItem;
        if (v == 0x136a) {
            ((PlayerData *)(p))->setFlag(0x27);
        } else if (v == 0x137b) {
            ((PlayerData *)(p))->setFlag(0x28);
        }
        handItemFlags = 0;
        putHandBack(actionTarget, 1);
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::mainAct2F()
{
    if (PlayerActor_IsInAct05()) {
        PocketMenu_ReturnToIdle(this);
    }
}

void PocketMenu::mainAct31()
{
    u16 t;
    Snd_PlaySe(0x40);
    requestCameraPush();
    t = PocketMenu_GetItem(this, actionTarget);
    if (PlayerActor_RequestHoldUpItem(&t)) {
        PocketMenu_ClearSlotItem(this, actionTarget);
        setMainState(0x2f);
    }
}

void PocketMenu::mainAct32()
{
    if (removeBlinkTimer != 0) {
        s32 r;
        removeBlinkTimer = removeBlinkTimer - 1;
        r = removeBlinkTimer % 5;
        if (r != 0) {
            if (r == 3) {
                highlightTarget(actionTarget);
            }
        } else {
            clearHighlights();
        }
        if (removeBlinkTimer == 0) {
            PocketMenu_ClearSlotItem(this, actionTarget);
        }
    } else {
        setMainState(0x2f);
        mainAct2F();
    }
}

#ifdef N3DS_DIRECT
// Stacked fruit (n3ds_options.h): a stack dropped outside leaves one fruit (requestDropItem); once it is down, what stays
// of the stack goes back into its pocket target t. FALSE if v was no stack (the game's own clearing then).
static BOOL PocketMenu_KeepFruitRest(PocketMenu *s, u32 t, u32 v)
{
    if (n3ds_fruit_count(v) < 2) return FALSE;
    PocketMenu_SetSlotItem(s, t, n3ds_fruit_rest(v), 0);
    return TRUE;
}

// The pocket menu's "Take One" on a fruit stack (addItemOptions; one action more than the game's 0-0x21, 0x22 being
// "Never Mind"): one fruit flies into an empty pocket, the rest stays (as actionTakeAttachment does with a letter's)
#define N3DS_ACTION_TAKE_ONE 0x23
static void PocketMenu_TakeOneFruit(PocketMenu *s)
{
    s32 r = PocketMenu_FindEmptyPocket(s);
    u32 v = PocketMenu_GetItem(s, s->actionTarget);
    if (r == 0x26 || n3ds_fruit_count(v) < 2) {
        PocketMenu_ReturnToIdle(s);
        PocketMenu_ShowMessage(s, 0xb, 0xff, 1);
        return;
    }
    PocketMenu_SetSlotItem(s, s->actionTarget, n3ds_fruit_rest(v), 0);
    PocketMenu_FlyItemTo(s, s->actionTarget, r, n3ds_fruit_one(v), 0);
}
#endif

void PocketMenu::mainAct28()
{
    switch (FieldAction_PollDrop(fieldRequest)) {
    case 1:
        if (testFlags(0x1000)) {
#ifdef N3DS_DIRECT
            if (!PocketMenu_KeepFruitRest(this, actionTarget, PocketMenu_GetItem(this, actionTarget)))
#endif
            Shared_InventoryItemGrid_ClearSlot(&m_358, PocketMenu_TargetToGridIndex(this, actionTarget));
            PocketMenu_ReturnToIdle(this);
        } else {
#ifdef N3DS_DIRECT
            PocketMenu_KeepFruitRest(this, handSource, handItem);
#endif
            clearHand();
            PocketMenu_ReturnToIdle(this);
        }
        break;
    case 2:
        if (testFlags(0x1000) == 0) {
            returnHand(1);
        }
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 3, 0xff, 0);
        Snd_PlaySe(0x73);
        break;
    default:
        return;
    }
    FieldAction_Release(fieldRequest);
    fieldRequest = -1;
}

void PocketMenu::mainAct29()
{
    switch (FieldAction_PollResult(fieldRequest)) {
    case 1:
        if (testFlags(0x1000)) {
            Shared_InventoryItemGrid_ClearSlot(&m_358, PocketMenu_TargetToGridIndex(this, actionTarget));
            PocketMenu_ReturnToIdle(this);
        } else {
            clearHand();
            PocketMenu_ReturnToIdle(this);
        }
        break;
    case 2:
        if (testFlags(0x1000) == 0) {
            returnHand(1);
        }
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 8, 0xff, 0);
        Snd_PlaySe(0x73);
        break;
    default:
        return;
    }
    FieldAction_Release(fieldRequest);
    fieldRequest = -1;
}

extern "C" void PocketMenu_EnterTouchIdle(PocketMenu *s)
{
    s->hideCursor();
    s->clearSelection();
    s->setMainState(0);
}

extern "C" void PocketMenu_EnterButtonIdle(PocketMenu *s)
{
    s->balloonTarget = 0x26;
    s->showCursor();
    s->restartKeyRepeat();
    s->updateLabelBalloon();
    s->setMainState(0xa);
    s->selectTarget(s->cursorTarget);
}

extern "C" void PocketMenu_ReturnToIdle(PocketMenu *s)
{
    if (MenuCtrl_IsTouch()) {
        PocketMenu_EnterTouchIdle(s);
    } else {
        PocketMenu_EnterButtonIdle(s);
    }
}

extern "C" void PocketMenu_EnterAddresseeTouch(PocketMenu *s)
{
    s->hideCursor();
    s->setMainState(8);
}

extern "C" void PocketMenu_EnterAddresseeButtons(PocketMenu *s)
{
    s->cursorToPopupBottom();
    s->setMainState(0x19);
}

extern "C" void PocketMenu_EnterAddresseeInput(PocketMenu *s)
{
    if (MenuCtrl_IsTouch()) {
        PocketMenu_EnterAddresseeTouch(s);
    } else {
        PocketMenu_EnterAddresseeButtons(s);
    }
}

extern "C" void PocketMenu_PrepareTouchOptions(PocketMenu *s)
{
    s->clearFlags(0x10000);
    if (PocketMenu_IsPocketTarget(s, s->touchedTarget)) {
        ChoiceIdList_Clear(s->optionList, 0x22);
        s->addItemOptions(s->touchedTarget);
        if (ChoiceIdList_Count(s->optionList) == 0) {
            s->setFlags(0x10000);
        }
    }
}

extern "C" void PocketMenu_BeginTouchOnTarget(PocketMenu *s, u32 a)
{
    s32 r6, r7;
    s->touchedTarget = a;
    s->setMainState(1);
    r6 = gTouchCurX;
    r7 = gTouchCurY;
    s->grabOffsetX = PocketMenu_GetTargetX(s, s->touchedTarget) - r6;
    s->grabOffsetY = PocketMenu_GetTargetY(s, s->touchedTarget) - r7;
    s->balloonTarget = a;
    s->m_23c0.queueOpen();
    s->m_23c0.commitOpen();
    s->optionsOpenDelay = 2;
    PocketMenu_PrepareTouchOptions(s);
    if (PocketMenu_IsSlotDisabled(s, a)) {
        s->clearFlags(4);
    } else {
        s->setFlags(4);
    }
    if (PocketMenu_IsPocketTarget(s, a) == 0 && a != 0x24) {
        s->requestCameraPop();
    }
    Inventory_PlayTouchSe();
}

extern "C" void PocketMenu_StartTouchDrag(PocketMenu *s, u32 a)
{
    s->handSource = a;
    s->m_23c0.hide(1);
    s->pickUp(a);
    switch (s->handKind) {
    case 1:
        s->setMainState(6);
        break;
    case 2:
        s->setMainState(5);
        break;
    }
    s->syncHandFromTouch();
    s->clearFlags(0x4000);
    Shared_Inventory_PlayPickUpSe();
    s->swapTarget = 0x26;
}

// ===== unit 02298430 =====
extern "C" void PocketMenu_StartButtonDrag(PocketMenu *s, u32 a)
{
    s->handSource = a;
    s->m_23c0.hide(1);
    if (s->chosenAction == 0) {
        s->pickUpItem(a);
    } else {
        s->pickUp(a);
    }
    switch (s->handKind) {
    case 1:
        s->returnState = 0xc;
        break;
    case 2:
        s->returnState = 0xb;
        break;
    }
    s->syncHandFromCursor();
    Shared_Inventory_PlayPickUpSe();
}

extern "C" void PocketMenu_FlyHandTo(PocketMenu *s, u32 a, u32 b)
{
    s32 t;
    s32 u;

    s->handSource = a;
    s->m_2480.setPos(s->handX, s->handY);
    t = PocketMenu_GetTargetX(s, a);
    u = PocketMenu_GetTargetY(s, a);
    s->m_2480.startLinear(t, u, b);
    s->m_2480.update();
    s->syncHandFromMover();
    s->setMainState(0x1b);
}

extern "C" void PocketMenu_FlyItemTo(PocketMenu *s, u32 a, u32 b, u32 c, u8 d)
{
    s->setHandItem(c, d);
    s->handX = PocketMenu_GetTargetX(s, a);
    s->handY = PocketMenu_GetTargetY(s, a);
    PocketMenu_FlyHandTo(s, b, 4);
}

extern "C" void PocketMenu_ShowMessage(PocketMenu *s, u32 a, u32 b, u32 c)
{
    Unk_ov096_02297fb8_Msg m;

    if (b == 0xff) {
        s->returnState = s->mainState;
    } else {
        s->returnState = b;
    }
    m.a = gU8None;
    m.a = a;
    s->errorMessage.open((u8 *)(&m), (u32)c, 0);
    s->setMainState(0x20);
    s->m_2498.disableObjWindow();
}

extern "C" void PocketMenu_StartRemoveBlink(PocketMenu *s)
{
    s->removeBlinkTimer = 0xe;
    s->setMainState(0x32);
}

extern "C" void PocketMenu_CancelVramTasks(PocketMenu *s)
{
    for (s32 i = 0; i < 2; i++) {
        s->bgTasks[i].cancel();
    }
}

extern "C" u32 PocketMenu_IsPocketTarget(PocketMenu *s, u32 id)
{
    if (id <= 0xe) {
        return 1;
    }
    return 0;
}

extern "C" u32 PocketMenu_IsLetterTarget(PocketMenu *s, u32 id)
{
    if (id >= 0xf && id <= 0x18) {
        return 1;
    }
    return 0;
}

extern "C" u32 PocketMenu_IsTabTarget(PocketMenu *s, u32 id)
{
    if (id >= 0x19 && id <= 0x20) {
        return 1;
    }
#ifdef N3DS_DIRECT
    if (id == POCKET_OPT_TARGET) {
        return 1;
    }
#endif
    return 0;
}

extern "C" u32 PocketMenu_IsSpecialTarget(PocketMenu *s, u32 id)
{
    if (id >= 0x22 && id <= 0x25) {
        return 1;
    }
    return 0;
}

extern "C" void PocketMenu_MoveCursorToTab(PocketMenu *s)
{
    s->cursorTarget = MenuTabBar_TabFromX(s->m_2498.getScreenX()) + 0x19;
}

extern "C" u32 PocketMenu_TargetToGridIndex(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return (u8)id;
    }
    if (PocketMenu_IsSpecialTarget(s, id)) {
        return (u8)(id - 4);
    }
    return 0;
}

extern "C" u32 PocketMenu_PocketIndexToTarget(PocketMenu *s, u32 id)
{
    if (id <= 0xe) {
        return (u8)id;
    }
    return 0x26;
}

extern "C" u32 PocketMenu_HitPocket(PocketMenu *s, s32 a, s32 b, s32 c)
{
    s32 r = InventoryItemGrid_FindPocketSlotAt(&s->m_358, a, b);
    if (r != 0x23) {
        if (c && InventoryItemGrid_IsSlotEmpty(&s->m_358, r)) {
            return 0x26;
        }
        return PocketMenu_PocketIndexToTarget(s, r);
    }
    return 0x26;
}

extern "C" s32 PocketMenu_DropItemAt(PocketMenu *s, u32 id)
{
    u16 v;

    if (PocketMenu_IsLetterTarget(s, id) && PocketMenu_IsSlotEmpty(s, id)) {
        return 0;
    }
#ifdef N3DS_DIRECT
    if (PocketMenu_IsLetterTarget(s, id) && n3ds_fruit_count(s->handItem) > 1) {  // no fruit stack into a letter
        s->returnHand(1);
        return 1;
    }
#endif
    if (PocketMenu_IsPocketTarget(s, id) || PocketMenu_IsLetterTarget(s, id)) {
        v = PocketMenu_GetItem(s, id);
        s->mergeItems((u16 *)(&s->handItem), s->handItemFlags, &v, PocketMenu_GetItemFlags(s, id));
        if (v != 0xfff1) {
            PocketMenu_SetSlotItem(s, s->handSource, v, PocketMenu_GetItemFlags(s, id));
        }
        s->putHandBack(id, 1);
        return 1;
    }
    if ((u8)(id + 0xde) <= 1) {
        if (s->dropHeldItem()) {
            return 1;
        }
        return 0;
    }
    if (id == 0x24) {
        if (s->useOnPlayerKind < 1) {
            s->cancelUseOnPlayer();
            return 0;
        }
        s->startUseOnPlayer();
        return 1;
    }
    if (id == 0x25) {
        if (!s->depositHeldBag()) {
            s->putHandBack(s->handSource, 1);
        }
        return 1;
    }
    if (id == 0x21) {
        s->wearHeldShirt();
        return 1;
    }
    return 0;
}

extern "C" void PocketMenu_SetSlotItem(PocketMenu *s, u32 id, u32 a, u32 b)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        u32 t = PocketMenu_TargetToGridIndex(s, id);
        InventoryItemGrid_SetSlotItem(&s->m_358, t, a, b);
        InventoryItemGrid_RefreshSlot(&s->m_358, t);
    } else if (PocketMenu_IsLetterTarget(s, id)) {
        PocketMenu_GetLetter(s, id)->setPresent(a, b);
    } else if (id == 0x25) {
        s->depositToWallet(a);
    }
}

extern "C" void PocketMenu_ClearSlotItem(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id) || PocketMenu_IsLetterTarget(s, id)) {
        PocketMenu_SetSlotItem(s, id, 0xfff1, 0);
    }
}

extern "C" void PocketMenu_ClearLetter(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsLetterTarget(s, id)) {
        s->m_db8.clearLetter(PocketMenu_TargetToLetterIndex(s, id));
    }
}

extern "C" u32 PocketMenu_FindEmptyPocket(PocketMenu *s)
{
    s32 r = Pocket_FindEmpty();
    if (r == -1) {
        return 0x26;
    }
    return (u8)r;
}

extern "C" u32 PocketMenu_TargetToLetterIndex(PocketMenu *s, u32 id)
{
    if (id >= 0xf && id <= 0x18) {
        return (u8)(id - 0xf);
    }
    return 0;
}

extern "C" u32 PocketMenu_LetterIndexToTarget(PocketMenu *s, u32 id)
{
    if (id <= 9) {
        return (u8)(id + 0xf);
    }
    return 0x26;
}

extern "C" u32 PocketMenu_HitLetter(PocketMenu *s, s32 a, s32 b, s32 c)
{
    s32 r = s->m_db8.findPocketLetterAt(a, b);
    if (r != 0x37) {
        if (c && LetterGrid_IsSlotEmpty(&s->m_db8, r)) {
            return 0x26;
        }
        return PocketMenu_LetterIndexToTarget(s, r);
    }
    return 0x26;
}

extern "C" s32 PocketMenu_DropLetterAt(PocketMenu *s, u32 id)
{
    if (!PocketMenu_IsSlotEmpty(s, id)) {
        Letter_Copy((LetterView *)&s->m_2c8c, PocketMenu_GetLetter(s, id));
        PocketMenu_SetLetter(s, s->handSource, &s->m_2c8c);
    }
    s->putHandBack(id, 1);
    return 1;
}

extern "C" void PocketMenu_SetLetter(PocketMenu *s, u32 id, void *p)
{
    if (PocketMenu_IsLetterTarget(s, id)) {
        s->m_db8.setLetter(PocketMenu_TargetToLetterIndex(s, id), p);
    }
}

extern "C" BOOL PocketMenu_HitShirtBox(PocketMenu *s, s32 x, s32 y)
{
    if (x < 4 || x >= 0x1c) {
        return FALSE;
    }
    if (y < 0xac || y >= 0xc4) {
        return FALSE;
    }
    return TRUE;
}

extern "C" s32 PocketMenu_HitSpecialTarget(PocketMenu *s, s32 x, s32 y)
{
    if (PocketMenu_HitPlayer(s, x, y)) {
        return 0x24;
    }
    if (x > 0x6c && x < 0xc4 && y > 0x50 && y < 0x68) {
        if (x < 0x98) {
            return 0x22;
        }
        return 0x23;
    }
    if (PocketMenu_HitWalletIcon(s, x, y)) {
        if (s->testFlags(0x4000)) {
            return 0x26;
        }
        return 0x25;
    }
    if (x > 0x30 && x < 0x60 && y > 0x50 && y < 0x60) {
        if (s->testFlags(0x4000)) {
            return 0x26;
        }
        return 0x25;
    }
    s->clearFlags(0x4000);
    return 0x26;
}

extern "C" BOOL PocketMenu_HitPlayer(PocketMenu *s, s32 x, s32 y)
{
    if (x > 0x88 && x < 0xa8 && y > 0x20 && y < 0x50) {
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL PocketMenu_HitWalletIcon(PocketMenu *s, s32 x, s32 y)
{
    if (Shared_InventoryItemGrid_HitTestSlot21(&s->m_358, x, y)) {
        return TRUE;
    }
    return FALSE;
}

extern "C" s32 PocketMenu_HitWalletAmount(PocketMenu *s, s32 x, s32 y)
{
    s32 v;

    if (y < 0x50 || y > 0x60) {
        return 0;
    }
    s->bellsPanelMode = 0;
    if (x < 0x30) {
        return 0;
    }
    if (x < 0x40) {
        v = 10000;
        s->bellsPanelMode = 3;
    } else if (x < 0x48) {
        v = 1000;
        s->bellsPanelMode = 2;
    } else if (x < 0x60) {
        v = 100;
        s->bellsPanelMode = 1;
    } else {
        return 0;
    }
    if (v > s->getWalletBells()) {
        return 1;
    }
    s->auxItem = Item_FindMoneyBagForAmount(v, 0, 0);
    return 2;
}

extern "C" s32 PocketMenu_GetTargetX(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return InventoryItemGrid_GetSlotX(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        return LetterGrid_GetSlotX(&s->m_db8, PocketMenu_TargetToLetterIndex(s, id));
    }
    if (PocketMenu_IsTabTarget(s, id)) {
        return MenuTabBar_GetTabX(POCKET_TAB_OF(id)) - 8;
    }
    if (PocketMenu_IsSpecialTarget(s, id)) {
        return InventoryItemGrid_GetSlotX(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    return 0;
}

extern "C" s32 PocketMenu_GetTargetY(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return InventoryItemGrid_GetSlotY(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        return Shared_LetterGrid_GetSlotY(&s->m_db8, PocketMenu_TargetToLetterIndex(s, id));
    }
    if (PocketMenu_IsTabTarget(s, id)) {
        return 8;
    }
    if (PocketMenu_IsSpecialTarget(s, id)) {
        return InventoryItemGrid_GetSlotY(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (id == 0x21) {
        return 0xb0;
    }
    return 0;
}

extern "C" s32 PocketMenu_IsSlotDisabled(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return Shared_InventoryItemGrid_IsSlotDisabled(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        return s->m_db8.isHighlighted(PocketMenu_TargetToLetterIndex(s, id));
    }
    return 0;
}

extern "C" s32 PocketMenu_IsSlotEmpty(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return InventoryItemGrid_IsSlotEmpty(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        return LetterGrid_IsSlotEmpty(&s->m_db8, PocketMenu_TargetToLetterIndex(s, id));
    }
    return 1;
}

extern "C" u32 PocketMenu_GetItem(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return Shared_InventoryItemGrid_GetSlotItem(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        if (PocketMenu_IsSlotEmpty(s, id)) {
            return 0xfff1;
        }
        return PocketMenu_GetLetter(s, id)->getPresent();
    }
    if (PocketMenu_IsSpecialTarget(s, id)) {
        return s->auxItem;
    }
    return 0xfff1;
}

extern "C" s32 PocketMenu_GetItemFlags(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsPocketTarget(s, id)) {
        return InventoryItemGrid_GetSlotFlags(&s->m_358, PocketMenu_TargetToGridIndex(s, id));
    }
    if (PocketMenu_IsLetterTarget(s, id)) {
        return PocketMenu_GetLetter(s, id)->getPresentFlags();
    }
    PocketMenu_IsSpecialTarget(s, id);
    return 0;
}

// ===== unit 02297b14 =====
extern "C" LetterView *PocketMenu_GetLetter(PocketMenu *s, u32 id)
{
    if (PocketMenu_IsLetterTarget(s, id)) {
    } else {
        return 0;
    }
    return s->m_db8.getLetter(PocketMenu_TargetToLetterIndex(s, id));
}

s32 PocketMenu::checkPlace(u32 a, u32 b, u32 c) {
    u16 tmp = 0xfff1;
    if (PocketMenu_IsPocketTarget(this, a)) {
        return 0;
    }
    if (PocketMenu_IsLetterTarget(this, a)) {
        if (PocketMenu_IsSlotEmpty(this, a)) {
            return 6;
        }
        if (c == 1) {
            return 5;
        }
        if (c == 2) {
            return 1;
        }
        LetterView *p = PocketMenu_GetLetter(this, a);
        if (p->getPresent() != 0xfff1) {
            return 3;
        }
        s32 s = p->getState();
        if (s == 7 || s == 8 || Shared_InvItem_IsNonTransferable(b)) {
            return 2;
        }
        if (Shared_InvItem_IsTurnipFishOrInsect(b)) {
            return 4;
        }
        tmp = b;
        if (Unk_ov096_022979f0_InRange(&tmp, 0x1492, 0x14fd) && handSource == 0x25) {
            return 5;
        }
        return 0;
    }
    if (a == 0x25) {
        if (c != 0) {
            return 6;
        }
        tmp = b;
        if (!Unk_ov096_022979f0_InRange(&tmp, 0x1492, 0x14fd)) {
            return 6;
        }
        s32 t = 0x1869f - getWalletBells();
        if (t < (s32)Item_GetPrice(&tmp)) {
            return 6;
        }
        return 0;
    }
    switch (a) {
    case 0x24:
        return 6;
    }
    return 6;
}

s32 PocketMenu::checkSwap(u32 a, u32 b) {
    s32 r = checkPlace(a, handItem, handItemFlags);
    if (r == 0) {
        u16 t = PocketMenu_GetItem(this, a);
        if (t == 0xfff1) {
            return 0;
        }
        u32 v = PocketMenu_GetItemFlags(this, a);
        if (b == 0x25) {
            u16 t2 = handItem;
            r = mergeItems((u16 *)(&t2), handItemFlags, &t, v);
            if ((u32)(r - 2) <= 1) {
                return 6;
            }
            if (t == 0xfff1) {
                return 0;
            }
        }
        r = checkPlace(b, t, v);
    }
    return r;
}

s32 PocketMenu::checkPlaceHand(u32 a) {
    return checkPlace(a, handItem, handItemFlags);
}

s32 PocketMenu::canNavToPlayer(u32 a) {
    if (a == 0) {
        return 1;
    }
    if (a == 2) {
        return 0;
    }
    if (a == 1) {
        if (getUseOnPlayerKind() != 1) {
            return 1;
        }
        return 0;
    }
    return 0;
}

s32 PocketMenu::canNavToDropArea(u32 a) {
    if (a == 1) {
        return canDropHeldItem();
    }
    return 0;
}

s32 PocketMenu::canNavToWallet(u32 a) {
    if (a == 2) {
        return 0;
    }
    if (a == 0) {
        return 1;
    }
    if (a == 1) {
        return isHoldingMoneyBag(0);
    }
    return 0;
}

void PocketMenu::clearSelection() {
    InventoryItemGrid_ClearCursorSlot(&m_358);
    m_db8.clearCursorSlot();
}

void PocketMenu::selectTarget(u32 a) {
    if (PocketMenu_IsPocketTarget(this, a)) {
        s32 s = PocketMenu_TargetToGridIndex(this, a);
        m_db8.clearCursorSlot();
        Shared_InventoryItemGrid_SetCursorSlot(&m_358, s);
    } else if (PocketMenu_IsLetterTarget(this, a)) {
        s32 s = PocketMenu_TargetToLetterIndex(this, a);
        InventoryItemGrid_ClearCursorSlot(&m_358);
        m_db8.setCursorSlot(s);
    } else {
        InventoryItemGrid_ClearCursorSlot(&m_358);
        m_db8.clearCursorSlot();
    }
}

void PocketMenu::clearHighlights() {
    Shared_InventoryItemGrid_ClearMarks(&m_358);
    m_db8.clearMarks();
    Shared_InventoryBg_ResetHighlight(&m_de0);
}

void PocketMenu::highlightTarget(u32 a) {
    if (PocketMenu_IsPocketTarget(this, a)) {
        InventoryItemGrid_MarkSlot(&m_358, PocketMenu_TargetToGridIndex(this, a));
    } else if (PocketMenu_IsLetterTarget(this, a)) {
        m_db8.markSlot(PocketMenu_TargetToLetterIndex(this, a));
    } else if (PocketMenu_IsSpecialTarget(this, a)) {
        switch (a) {
        case 0x25:
            InventoryItemGrid_MarkSlot(&m_358, 0x21);
            break;
        case 0x22:
        case 0x23:
            InventoryBg_SetHighlight(&m_de0, 0);
            break;
        case 0x24:
            InventoryBg_SetHighlight(&m_de0, 1);
            break;
        }
    } else if (a == 0x21) {
        InventoryItemGrid_MarkSlot(&m_358, 0x22);
    }
}

BOOL PocketMenu::hasTouchMoved() {
    s32 d = gTouchPressX - gTouchCurX;
    if (d < 0) {
        d = -d;
    }
    if (d > 8) {
        return TRUE;
    }
    d = gTouchPressY - gTouchCurY;
    if (d < 0) {
        d = -d;
    }
    if (d > 8) {
        return TRUE;
    }
    return FALSE;
}

BOOL PocketMenu::isTouchHeldFor(s32 v) {
    if (gTouchHoldFrames >= v) {
        return TRUE;
    }
    return FALSE;
}

void PocketMenu::positionLabelBalloon() {
    s32 x = PocketMenu_GetTargetX(this, balloonTarget) - 0x6d;
    s32 y = PocketMenu_GetTargetY(this, balloonTarget) - 0x78;
    if (MenuCtrl_IsButtons()) {
        y -= 8;
    }
    m_23c0.setPos(x, y);
    if (PocketMenu_IsPocketTarget(this, balloonTarget)) {
        s32 s = PocketMenu_TargetToGridIndex(this, balloonTarget);
        Shared_InventoryItemGrid_ShowSlotName(&m_358, &m_23c0, s);
    } else if (PocketMenu_IsLetterTarget(this, balloonTarget)) {
        s32 s = PocketMenu_TargetToLetterIndex(this, balloonTarget);
        m_db8.showLetterName(&m_23c0, s);
    }
}

void PocketMenu::updateLabelBalloon() {
    if (PocketMenu_IsPocketTarget(this, cursorTarget) || PocketMenu_IsLetterTarget(this, cursorTarget)) {
        if (PocketMenu_IsSlotEmpty(this, cursorTarget)) {
            m_23c0.cancelQueuedOpen();
        } else {
            balloonTarget = cursorTarget;
            m_23c0.queueOpen();
        }
    }
    if (PocketMenu_IsTabTarget(this, cursorTarget) || PocketMenu_IsSpecialTarget(this, cursorTarget)) {
        m_23c0.cancelQueuedOpen();
    }
}

void PocketMenu::drawHand() {
    if (testFlags(0x40) == 0) {
        if (handKind != 0) {
            if (handKind == 2) {
                InventoryItemGrid_DrawHeldItem(&m_358, handX, handY);
            } else if (handKind == 1) {
                m_db8.drawHeldLetter(handX, handY, &m_2b98);
            }
        }
    }
}

void PocketMenu::syncHandFromTouch() {
    handX = grabOffsetX + gTouchCurX;
    handY = grabOffsetY + gTouchCurY;
}

void PocketMenu::syncHandFromCursor() {
    handX = m_2498.getFrameScreenX() - 2;
    handY = m_2498.getFrameScreenY() - 4;
}

void PocketMenu::syncHandFromMover() {
    handX = m_2480.getX();
    handY = m_2480.getY();
}

void PocketMenu::setHandItem(u32 a, u32 b) {
    handKind = 2;
    handItem = a;
    handItemFlags = b;
    Shared_InventoryItemGrid_SetHeldItem(&m_358, handItem, handItemFlags);
    updateHandPrice();
}

void PocketMenu::pickUpItemFrom(u32 a) {
    handKind = 2;
    handItem = PocketMenu_GetItem(this, a);
    handItemFlags = PocketMenu_GetItemFlags(this, a);
    PocketMenu_ClearSlotItem(this, a);
    Shared_InventoryItemGrid_SetHeldItem(&m_358, handItem, handItemFlags);
    updateHandPrice();
}

void PocketMenu::pickUpLetter(u32 a) {
    s32 r = PocketMenu_TargetToLetterIndex(this, a);
    handKind = 1;
    LetterView *p = m_db8.getLetter(r);
    Letter_Copy((LetterView *)&m_2b98, p);
    m_db8.clearLetter(r);
}

void PocketMenu::pickUp(u32 a) {
    if (PocketMenu_IsPocketTarget(this, a)) {
        pickUpItemFrom(a);
    } else if (PocketMenu_IsLetterTarget(this, a)) {
        pickUpLetter(a);
    } else if (a == 0x25) {
        pickUpItemFrom(a);
        withdrawFromWallet(handItem);
    }
}

void PocketMenu::pickUpItem(u32 a) {
    if (PocketMenu_IsPocketTarget(this, a) || PocketMenu_IsLetterTarget(this, a)) {
        pickUpItemFrom(a);
    }
}

void PocketMenu::putHandBack(u32 k, u32 f) {
    switch (handKind) {
    case 1:
        PocketMenu_SetLetter(this, k, &m_2b98);
        break;
    case 2:
        PocketMenu_SetSlotItem(this, k, handItem, handItemFlags);
        break;
    }
    if (f != 0) {
        clearHand();
    }
}

void PocketMenu::returnHand(u32 f) {
    if (checkPlaceHand(handSource) == 0) {
        putHandBack(handSource, f);
    } else if (swapTarget != 0x26) {
        u32 a = PocketMenu_GetItem(this, swapTarget);
        u32 b = PocketMenu_GetItemFlags(this, swapTarget);
        putHandBack(swapTarget, 1);
        PocketMenu_SetSlotItem(this, handSource, a, b);
        swapTarget = 0x26;
    }
}

void PocketMenu::swapHandWith(u32 a) {
    switch (handKind) {
    case 1:
        Letter_Copy((LetterView *)&m_2c8c, (LetterView *)&m_2b98);
        pickUp(a);
        PocketMenu_SetLetter(this, a, &m_2c8c);
        break;
    case 2: {
        u32 x = handItem;
        u32 y = handItemFlags;
        pickUpItem(a);
        PocketMenu_SetSlotItem(this, a, x, y);
        break;
    }
    }
}

s32 PocketMenu::placeHandAt(u32 a) {
    s32 r;
    switch (handKind) {
    case 2:
        r = PocketMenu_DropItemAt(this, a);
        break;
    case 1:
        r = PocketMenu_DropLetterAt(this, a);
        break;
    default:
        r = 0;
        break;
    }
    return r;
}

s32 PocketMenu::dropHeldItem() {
    if (!canDropHeldItem()) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 9, 0xff, 1);
        returnHand(1);
        return 0;
    }
    s32 r = requestDropItem(handItem);
    if (r == 0) {
        returnHand(1);
        return 0;
    }
    if (r == 1) {
#ifdef N3DS_DIRECT
        PocketMenu_KeepFruitRest(this, handSource, handItem);
#endif
        clearHand();
        return 1;
    }
    if (r == 2) {
        setMainState(0x28);
        clearFlags(0x1000);
        return 0;
    }
    returnHand(1);
    return 0;
}

BOOL PocketMenu::canDropHeldItem() {
    if (handKind != 2) {
        return FALSE;
    }
    if (handItemFlags != 0) {
        return FALSE;
    }
    volatile u16 t = handItem;
    if (Unk_ov096_022968bc_InRange(&t, 0x1492, 0x14fd) && handSource == 0x25) {
        return FALSE;
    }
    return canDropItem(*(volatile u16 *)&handItem);
}

void PocketMenu::clearHand() {
    handKind = 0;
    MenuCtrl_SetHandBells(0);
}

void PocketMenu::startUseOnPlayer() {
    setMainState(0x25);
    mainAct25();
    clearFlags(0x8000);
}

s32 PocketMenu::getUseOnPlayerKind() {
    if (handKind != 2) {
        return 1;
    }
    if (handItemFlags != 0) {
        return 1;
    }
    volatile u16 t = handItem;
    BOOL r = FALSE;
    u32 v = t;
    u32 w = t;
    if (w >= 0x11a8 && v <= 0x12a7) {
        r = TRUE;
    }
    if (r) {
        return 2;
    }
    if ((v >= 0x1431 && v <= 0x1470) || (v >= 0x1471 && v <= 0x1491)) {
        t = getEquipped(4);
        u32 b = t;
        u32 a = t;
        if (a != 0xfff1 && b >= 0x13a8 && b <= 0x13c7 && !func_0204bab8((u16 *)&t)) {
            return 6;
        }
        return 3;
    }
    if (v >= 0x13a8 && v <= 0x13c7) {
        if (getEquipped(3) != 0xfff1 && !func_0204bab8((u16 *)&t)) {
            return 0;
        }
        return 4;
    }
    if ((v >= 0x13c8 && v <= 0x1407) || (v >= 0x1408 && v <= 0x1428)) {
        return 4;
    }
    if (Item_IsHoldable((u16 *)&t)) {
        return 5;
    }
    BOOL q = FALSE;
    u32 x = t;
    u32 y = t;
    if (y >= 0x1518 && x <= 0x151c) {
        q = TRUE;
    }
    if (q || (x >= 0x1531 && x <= 0x153a) || (x >= 0x153b && x <= 0x1541)) {
        return 7;
    }
    if (x >= 0x155e && x <= 0x155e) {
        return 8;
    }
    return 1;
}

u16 PocketMenu::getEquipped(s32 k) {
    void *o = PlayerData_GetCurrent();
    switch (k) {
    case 2:
        return *((PlayerData *)(o))->getShirt();
    case 3:
        return *((PlayerData *)(o))->getFaceItem();
    case 4:
        return *((PlayerData *)(o))->getHat();
    case 5:
        return *((PlayerData *)(o))->getHeldItem();
    }
    return 0xfff1;
}

u16 PocketMenu::swapEquipped(s32 k, u16 v) {
    u16 r = getEquipped(k);
    void *o = PlayerData_GetCurrent();
    volatile u16 t = v;
    switch (k) {
    case 2:
        ((PlayerData *)(o))->setShirt((u16 *)&t);
        t = r;
        if (!Unk_ov096_022968bc_InRange(&t, 0x11a8, 0x12a7)) {
            r = 0xfff1;
        }
        break;
    case 3:
        ((PlayerData *)(o))->setFaceItem((u16 *)&t);
        break;
    case 6:
        ((PlayerData *)(o))->setFaceItem((u16 *)&t);
        r = getEquipped(4);
        t = 0xfff1;
        ((PlayerData *)(o))->setHat((u16 *)&t);
        break;
    case 4:
        ((PlayerData *)(o))->setHat((u16 *)&t);
        t = r;
        if (Unk_ov096_022968bc_InRange(&t, 0x1429, 0x1430)) {
            r = 0xfff1;
        }
        break;
    case 5:
        t = r;
        if (Unk_ov096_022968bc_InRange(&t, 0x13a0, 0x13a7)) {
            r = 0xfff1;
        }
        break;
    case 7:
    case 8:
        break;
    }
    return r;
}

BOOL PocketMenu::requestUseOnPlayer(s32 k, u16 v) {
    u16 t = v;
    switch (k) {
    case 2:
        if (PlayerActor_RequestWearShirt(&t)) {
            return TRUE;
        }
        break;
    case 3:
        if (PlayerActor_RequestWearFaceItem(&t)) {
            return TRUE;
        }
        break;
    case 6:
        if (PlayerActor_RequestWearFaceItem(&t)) {
            t = 0xfff1;
            PlayerActor_RequestWearHat(&t);
            return TRUE;
        }
        break;
    case 4:
        if (PlayerActor_RequestWearHat(&t)) {
            return TRUE;
        }
        break;
    case 5:
        if (PlayerActor_RequestChangeHeldItem(&t)) {
            return TRUE;
        }
        break;
    case 7:
        if (PlayerActor_RequestHoldUpItem(&t)) {
            if (!Camera_IsViewPushed()) {
                Snd_PlaySe(0x40);
            }
            return TRUE;
        }
        break;
    case 8:
        if (PlayerActor_RequestFaceChange()) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}

BOOL PocketMenu::isUseOnPlayerBusy(s32 k) {
    switch (k) {
    case 5:
        return PlayerActor_IsChangingHeldItem();
    case 7:
    case 8:
        if (PlayerActor_IsInAct05()) {
            return FALSE;
        }
        return TRUE;
    default:
        return PlayerActor_IsChangingClothes();
    }
}

void PocketMenu::cancelUseOnPlayer() {
    PocketMenu_ReturnToIdle(this);
    returnHand(1);
    if (useOnPlayerKind == 0) {
        PocketMenu_ShowMessage(this, 7, 0xff, 1);
    }
}

void PocketMenu::updateHandPrice() {
    volatile u16 t = handItem;
    if (Unk_ov096_022968bc_InRange(&t, 0x1492, 0x14fd)) {
        MenuCtrl_SetHandBells(Item_GetPrice((u16 *)&t));
    } else {
        MenuCtrl_SetHandBells(0);
    }
}

BOOL PocketMenu::depositHeldBag() {
    u16 t = handItem;
    Item_GetPrice(&t);
    u32 r = depositToWallet(handItem);
    if (r == 0xfff1) {
        clearHand();
        PocketMenu_ReturnToIdle(this);
    } else {
        handItem = r;
        Shared_InventoryItemGrid_SetHeldItem(&m_358, handItem, handItemFlags);
        PocketMenu_FlyHandTo(this, handSource, 4);
    }
    return TRUE;
}

BOOL PocketMenu::isHoldingMoneyBag(s32 flag) {
    if (handKind != 2) {
        return FALSE;
    }
    if (handItemFlags != 0) {
        return FALSE;
    }
    volatile u16 t = handItem;
    if (Unk_ov096_022968bc_InRange(&t, 0x1492, 0x14fd)) {
        if (flag) {
            if (Item_GetPrice((u16 *)&t) + getWalletBells() > 0x1869f) {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

s32 PocketMenu::getWalletBells() {
    return ((PlayerInventory *)(PlayerData_GetCurrent()->getInventory()))->getTotalBells(0);
}

void PocketMenu::setWalletBells(s32 v) {
    PlayerInventory_SetWallet((PlayerInventory *)PlayerData_GetCurrent()->getInventory(), v, 0);
    InventoryBg_StartBellRoll(&m_de0, 0);
}

void PocketMenu::withdrawFromWallet(u16 v) {
    u16 t = v;
    u32 a = Item_GetPrice(&t);
    getWalletBells();
    u32 b = getWalletBells();
    setWalletBells(b - a);
}

u32 PocketMenu::depositToWallet(u16 v) {
    u16 t = v;
    u32 e;
    u32 a = Item_GetPrice(&t);
    s32 x;
    a += getWalletBells();
    u32 r = 0xfff1;
    if (a > 0x1869f) {
        e = a - 0x1869f;
        r = Item_FindMoneyBagForAmount(e, 1, &x);
        a -= e + x;
    }
    setWalletBells(a);
    return r;
}

s32 PocketMenu::mergeItems(u16 *a, s32 f, u16 *b, u8 g) {
    u16 loc[3];
    u32 a4, b4, lim;
    BOOL ok;
    BOOL ok2;
    s32 d0;
    if (f != 0 || g != 0) {
        return 1;
    }
#ifdef N3DS_DIRECT
    // stacked fruit (n3ds_options.h): a fruit or stack onto one of its kind makes a stack, the rest (over the most)
    // stays where the slot's was
    if (n3ds_stack_fruit() && !gCommManager->isOnline() && n3ds_fruit_kind(*a) >= 0 &&
        n3ds_fruit_kind(*a) == n3ds_fruit_kind(*b)) {
        s32 n = n3ds_fruit_count(*a) + n3ds_fruit_count(*b), k = n3ds_fruit_kind(*a);
        if (n3ds_fruit_count(*b) < N3DS_FRUIT_MAX && n3ds_fruit_count(*a) < N3DS_FRUIT_MAX) {
            *a = n3ds_fruit_make(k, n > N3DS_FRUIT_MAX ? N3DS_FRUIT_MAX : n);
            *b = n3ds_fruit_make(k, n > N3DS_FRUIT_MAX ? n - N3DS_FRUIT_MAX : 0);
            return 0;
        }
    }
#endif
    loc[0] = *a;
    loc[1] = *b;
    ok = FALSE;
    u32 x0 = *(volatile u16 *)&loc[0];
    u32 y0 = *(volatile u16 *)&loc[0];
    if (y0 >= 0x1531 && x0 <= 0x153a) {
        ok = TRUE;
    }
    if (ok) {
        ok2 = FALSE;
        u32 x1 = *(volatile u16 *)&loc[1];
        u32 y1 = *(volatile u16 *)&loc[1];
        if (y1 >= 0x1531 && x1 <= 0x153a) {
            ok2 = TRUE;
        }
        if (ok2) {
            d0 = Unk_ov096_022969bc_Idx(x0) + 1;
            s32 d1 = Unk_ov096_022969bc_Idx(x1) + 1;
            d0 += d1;
            s32 rem;
            if (d0 > 10) {
                rem = d0 - 10;
                d0 = 10;
            } else {
                rem = 0;
            }
            s32 n1 = d0 - 1;
            *a = Unk_ov096_022969bc_Ch(n1);
            if (rem > 0) {
                s32 n2 = rem - 1;
                *b = Unk_ov096_022969bc_Ch(n2);
            } else {
                *b = 0xfff1;
            }
            return 0;
        }
    }
    BOOL q;
    if (x0 >= 0x1492 && x0 <= 0x14fd) {
        q = TRUE;
    } else {
        q = FALSE;
    }
    if (q) {
        BOOL q2 = FALSE;
        u32 x2 = *(volatile u16 *)&loc[1];
        u32 y2 = *(volatile u16 *)&loc[1];
        if (y2 >= 0x1492 && x2 <= 0x14fd) {
            q2 = TRUE;
        }
        if (q2) {
            goto go;
        }
    }
    return 1;
go:
    a4 = Item_GetPrice(&loc[0]);
    b4 = Item_GetPrice(&loc[1]);
    loc[2] = 0x14fd;
    lim = Item_GetPrice(&loc[2]);
    {
        s32 out;
        BOOL n4 = (s32)a4 < 1000 ? TRUE : FALSE;
        BOOL n6 = (s32)b4 < 1000 ? TRUE : FALSE;
        if (n4 != n6) {
            return 2;
        }
        if (a4 == lim || b4 == lim) {
            return 3;
        }
        if ((s32)a4 < 1000) {
            a4 += b4;
            if ((s32)a4 > 1000) {
                b4 = a4 - 1000;
                a4 = 1000;
            } else {
                b4 = 0;
            }
        } else {
            a4 += b4;
            if ((s32)a4 <= (s32)lim) {
                b4 = 0;
            } else {
                b4 = a4 - lim;
                a4 = lim;
            }
        }
        *a = Item_FindMoneyBagForAmount(a4, 1, &out);
        if (b4 == 0) {
            *b = 0xfff1;
        } else {
            *b = Item_FindMoneyBagForAmount(b4, 1, &out);
        }
    }
    return 0;
}

void PocketMenu::showCursor() {
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    m_2498.warpTo(a, b);
    if (PocketMenu_IsTabTarget(this, cursorTarget)) {
        m_2498.setAnimIfChanged(0xd);
    } else {
        m_2498.setAnimIfChanged(1);
    }
    refreshCursor();
}

void PocketMenu::cursorToPopupBottom() {
    popupRow = PocketMenu_Popup(this)->getRowCount() - 1;
    s32 a = PocketMenu_Popup(this)->getRowX();
    s32 b = PocketMenu_Popup(this)->getRowY(popupRow);
    m_2498.warpTo(a, b);
    m_2498.setAnimIfChanged(7);
}

s32 PocketMenu::getCursorTargetX() {
    s32 t = PocketMenu_GetTargetX(this, cursorTarget);
    if (testFlags(0x20)) {
        t += 0x100;
    } else if (testFlags(0x10)) {
        t -= 0x100;
    }
    return t + 8;
}

// ===== unit 022968bc =====

s32 PocketMenu::getCursorTargetY() {
    return PocketMenu_GetTargetY(this, cursorTarget);
}

void PocketMenu::hideCursor() {
    m_2498.setAnimIfChanged(0);
    m_2498.update();
}

void PocketMenu::moveCursorToTarget() {
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    m_2498.moveToEase(a, b, 3, 1);
    returnState = mainState;
    setMainState(0xf);
}

void PocketMenu::moveCursorToPopupRow() {
    s32 a = PocketMenu_Popup(this)->getRowX();
    s32 b = PocketMenu_Popup(this)->getRowY(popupRow);
    m_2498.moveToLinear(a, b, 2);
    returnState = mainState;
    setMainState(0xf);
}

void PocketMenu::cancelPopup() {
    chosenAction = 0x22;
    popupRow = PopupChoice_DecideCancel(PocketMenu_Popup(this), 1);
    s32 a = PocketMenu_Popup(this)->getRowX();
    s32 b = PocketMenu_Popup(this)->getRowY(popupRow);
    m_2498.warpTo(a, b);
    m_2498.setAnimAtEnd(8);
    setMainState(0x1e);
}

void PocketMenu::cursorToPopupDefault() {
    if (testFlags(0x40000)) {
        popupRow = 1;
    } else {
        popupRow = 0;
    }
    s32 a = PocketMenu_Popup(this)->getRowX();
    s32 b = PocketMenu_Popup(this)->getRowY(popupRow);
    m_2498.warpTo(a, b);
    m_2498.setAnimIfChanged(7);
}

void PocketMenu::placeCursorOnTarget() {
    s32 a = getCursorTargetX();
    s32 b = getCursorTargetY();
    m_2498.warpTo(a, b);
    m_2498.setAnimIfChanged(1);
}

void PocketMenu::refreshCursor() {
    m_2498.setPoseIdle();
    m_2498.update();
}

void PocketMenu::pressTab() {
    m_2498.setPosePress();
    setMainState(0x10);
}

void PocketMenu::releaseCursor() {
    m_2498.setPoseRelease();
    setMainState(0x11);
}

void PocketMenu::actionAct00() {
    m_2498.setAnimIfChanged(4);
    setMainState(0x12);
    swapTarget = 0x26;
}

void PocketMenu::beginPutDownAt(u32 v) {
    m_23c0.hide(1);
    placeTarget = v;
    m_2498.setAnimIfChanged(5);
    setMainState(0x14);
    clearFlags(0x2000);
}

void PocketMenu::beginSwapAt(u32 v) {
    m_23c0.hide(1);
    returnState = mainState;
    placeTarget = v;
    m_2498.setAnimIfChanged(6);
    setMainState(0x15);
}

BOOL PocketMenu::canDropItem(s32 a) {
    if (Unk_ov096_0229652c_IsZero(gFieldSceneKind)) {
        return PocketMenu_CanDropOutdoor(this, a);
    }
    if (Scene_InHouseRoom()) {
        if (Shared_PocketMenu_CanEditRoom()) {
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

s32 PocketMenu::requestDropItem(s32 a) {
#ifdef N3DS_DIRECT
    a = n3ds_fruit_one(a);  // of a fruit stack one fruit (the rest stays: PocketMenu_KeepFruitRest)
#endif
    if (Unk_ov096_0229652c_IsZero(gFieldSceneKind)) {
        fieldRequest = FieldAction_RequestDrop(gCommManager->myAid, a);
        if (fieldRequest == -1) {
            PocketMenu_ReturnToIdle(this);
            PocketMenu_ShowMessage(this, 3, 0xff, 0);
            Snd_PlaySe(0x73);
            return 0;
        }
        return 2;
    }
    if (Scene_InHouseRoom()) {
        return PocketMenu_RequestDropIndoor(this, a);
    }
    return 0;
}

void PocketMenu::actionDropItem() {
    u32 t = actionTarget;
    s32 r6 = PocketMenu_GetItem(this, t);
    if (!canDropItem(r6)) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 9, 0xff, 1);
    } else {
        s32 r = requestDropItem(r6);
        if (r != 0) {
            if (r == 2) {
                setMainState(0x28);
                setFlags(0x1000);
            } else {
                s32 x = PocketMenu_TargetToGridIndex(this, t);
#ifdef N3DS_DIRECT
                if (!PocketMenu_KeepFruitRest(this, t, r6))
#endif
                Shared_InventoryItemGrid_ClearSlot(&m_358, x);
                PocketMenu_ReturnToIdle(this);
            }
        }
    }
}

void PocketMenu::actionAct04() {
    MenuCtrl_SetArg(getActionLetter());
    openAddresseeList();
    clearFlags(0x200);
}

void PocketMenu::actionEditLetter() {
    MenuCtrl_SetSavedSlot(actionTarget);
    MenuCtrl_SetArg(getActionLetter());
    requestTab(8);
    hideTabBar();
}

void PocketMenu::actionTakeAttachment() {
    s32 r6 = PocketMenu_FindEmptyPocket(this);
    if (r6 == 0x26) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 0xb, 0xff, 1);
    } else {
        s32 a = PocketMenu_GetItemFlags(this, actionTarget);
        s32 b = PocketMenu_GetItem(this, actionTarget);
        PocketMenu_ClearSlotItem(this, actionTarget);
        PocketMenu_FlyItemTo(this, actionTarget, r6, b, a);
    }
}

BOOL PocketMenu::allocLetterSlot() {
    s32 t = Inventory_FindEmptyLetter();
    if (t == -1) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 2, 0xff, 1);
        return FALSE;
    }
    MenuCtrl_SetSavedSlot(actionTarget);
    paperTarget = actionTarget;
    actionTarget = t + 0xf;
    return TRUE;
}

void PocketMenu::actionWriteLetter() {
    if (allocLetterSlot()) {
        MenuCtrl_SetArg(getActionLetter());
        openAddresseeList();
        setFlags(0x200);
    }
}

void PocketMenu::actionAct08() {
    if (allocLetterSlot()) {
        LetterView *t = getActionLetter();
        Letter_InitBottleDraft(t);
        MenuCtrl_SetArg(t);
        PocketMenu_ClearSlotItem(this, paperTarget);
        requestTab(8);
        hideTabBar();
    }
}

void PocketMenu::actionTakeOutBells() {
    s32 v;
    s32 r;
    switch (chosenAction) {
    case 0xb:
        v = Item_FindMoneyBagForAmount(getWalletBells(), 0, 0);
        break;
    case 0xc:
        v = Item_FindMoneyBagForAmount(0x64, 0, 0);
        break;
    case 0xd:
        v = Item_FindMoneyBagForAmount(0x3e8, 0, 0);
        break;
    case 0xe:
        v = Item_FindMoneyBagForAmount(0x2710, 0, 0);
        break;
    }
    r = PocketMenu_FindEmptyPocket(this);
    if (r == 0x26) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 0xc, 0xff, 1);
    } else {
        withdrawFromWallet(v);
        PocketMenu_FlyItemTo(this, 0x25, r, v, 0);
    }
}

void PocketMenu::actionTakeOff() {
    BOOL ok = TRUE;
    volatile u16 v = 0xfff1;
    u32 k = chosenAction;
    switch (k) {
    case 0xf:
        useOnPlayerKind = 3;
        break;
    case 0x10:
        useOnPlayerKind = 4;
        v = getEquipped(4);
        if (Unk_ov096_0229619c_Range(&v, 0x1429, 0x1430)) {
            ok = FALSE;
        }
        break;
    case 0x11:
        useOnPlayerKind = 5;
        v = getEquipped(5);
        if (Unk_ov096_0229619c_Range(&v, 0x13a0, 0x13a7)) {
            ok = FALSE;
        }
        break;
    default:
        PocketMenu_ReturnToIdle(this);
        return;
    }
    s32 r = PocketMenu_FindEmptyPocket(this);
    if (ok && r == 0x26) {
        PocketMenu_ReturnToIdle(this);
        PocketMenu_ShowMessage(this, 0xa, 0xff, 0);
        Snd_PlaySe(0x73);
        return;
    }
    handSource = r;
    clearHand();
    handItem = 0xfff1;
    startUseOnPlayer();
    setFlags(0x8000);
}

void PocketMenu::actionUnwrapItem() {
    u32 t = actionTarget;
    pickUpItemFrom(t);
    handX = PocketMenu_GetTargetX(this, t);
    handY = PocketMenu_GetTargetY(this, t);
    if (MenuCtrl_IsButtons()) {
        handX = handX - 2;
        handY = handY - 2;
    }
    setMainState(0x2b);
    Shared_InventoryItemGrid_StartPresentAnim(&m_358, handItem, handItemFlags);
}

void PocketMenu::actionAct09() {
    u32 t = actionTarget;
    pickUpLetter(t);
    handX = PocketMenu_GetTargetX(this, t);
    handY = PocketMenu_GetTargetY(this, t);
    if (MenuCtrl_IsButtons()) {
        handX = handX - 2;
        handY = handY - 2;
    }
    setMainState(0x2a);
    LetterGrid_StartPopAnim(&m_db8);
}

void PocketMenu::actionAct0A() { openConfirmList(); }

void PocketMenu::actionAct1A() {
    setMainState(0x31);
    mainAct31();
}

void PocketMenu::actionOpenCountdownMenu() {
    ChoiceIdList_Clear(optionList, 0x22);
    ChoiceIdList_Add(optionList, 0xdd, 0x1d);
    ChoiceIdList_Add(optionList, 0xde, 0x1e);
    ChoiceIdList_Add(optionList, 0xdf, 0x1f);
    ChoiceIdList_Add(optionList, 0xe0, 0x20);
    ChoiceIdList_Add(optionList, 0x2, 0x22);
    hideCursor();
    showOptionList(0);
}

void PocketMenu::setCountdown(s32 n) {
    CommManager *g = gCommManager;
    if (g->isOnline()) {
        if (g->myAid == 0) {
            BOOL z;
            if (n == 0) {
                z = TRUE;
            } else {
                z = FALSE;
            }
            if (z != ((HudCountdown *)(Hud_GetCountdown()))->isStopped()) {
                CommSub_Send((u8)(n + 0x12), 4);
                HudCountdown_StartWithSe(n);
            }
        } else {
            CommSub_Send((u8)(n + 0xd), 0);
        }
    } else {
        HudCountdown_StartWithSe(n);
    }
}

void PocketMenu::actionStopCountdown() {
    setCountdown(0);
    PocketMenu_ReturnToIdle(this);
}

// ===== unit 02295f94 =====

void PocketMenu::actionStartCountdown() {
    setCountdown(chosenAction - 0x1c);
    PocketMenu_ReturnToIdle(this);
}

void PocketMenu::runChosenAction() {
    u32 c = chosenAction;
    if (c != 0x22 && c != 0 && c != 0xf && c != 0x10 && c != 0x11 && c != 0x1a) {
        requestCameraPop();
    }
    if (chosenAction == 0x22) {
        PocketMenu_ReturnToIdle(this);
        return;
    }
#ifdef N3DS_DIRECT
    if (chosenAction == N3DS_ACTION_TAKE_ONE) {
        PocketMenu_TakeOneFruit(this);
        return;
    }
#endif
    static Unk_ov096_0229aea8_Fn tbl[34] = {
        &PocketMenu::actionAct00, &PocketMenu::actionAct00,
        &PocketMenu::actionDropItem, &PocketMenu::actionReadLetter,
        &PocketMenu::actionAct04, &PocketMenu::actionEditLetter,
        &PocketMenu::actionTakeAttachment, &PocketMenu::actionWriteLetter,
        &PocketMenu::actionAct08, &PocketMenu::actionAct09,
        &PocketMenu::actionAct0A, &PocketMenu::actionTakeOutBells,
        &PocketMenu::actionTakeOutBells, &PocketMenu::actionTakeOutBells,
        &PocketMenu::actionTakeOutBells, &PocketMenu::actionTakeOff,
        &PocketMenu::actionTakeOff, &PocketMenu::actionTakeOff,
        &PocketMenu::actionUseWallpaper, &PocketMenu::actionUseCarpet,
        &PocketMenu::actionUnwrapItem, &PocketMenu::actionReleaseInsect,
        &PocketMenu::actionBuryItem, &PocketMenu::actionPlantItem,
        &PocketMenu::actionReleaseFish, &PocketMenu::actionThrowBottle,
        &PocketMenu::actionAct1A, &PocketMenu::actionOpenCountdownMenu,
        &PocketMenu::actionStopCountdown, &PocketMenu::actionStartCountdown,
        &PocketMenu::actionStartCountdown, &PocketMenu::actionStartCountdown,
        &PocketMenu::actionStartCountdown, &PocketMenu::actionAct21};
    (this->*tbl[chosenAction])();
}

void PocketMenu::showOptionList(s32 a) {
    u32 r6;
    s32 r2;
    PocketMenu_Popup(this)->setRowsFromIds((PopupChoiceIdList *)(optionList), testFlags(0x40000));
    r6 = PocketMenu_GetTargetX(this, actionTarget);
    r2 = PocketMenu_GetTargetY(this, actionTarget);
    if (actionTarget == 0x25) {
        m_24fc.placeAbove(0x68, 0x68);
    } else if (a != 0) {
        m_24fc.placeAboveBalloon((LabelBalloon *)(&m_23c0));
    } else {
        m_24fc.placeNearPoint(r6, r2);
    }
    PopupChoice_Open(PocketMenu_Popup(this), 0);
    setMainState(0x1d);
}

void PocketMenu::cancelOptions() {
    Snd_PlaySe(0x2a);
    chosenAction = 0x22;
    placeCursorOnTarget();
    PopupChoice_Close(PocketMenu_Popup(this), 0);
    setMainState(0x1f);
}

void PocketMenu::closeAddresseeList() {
    Snd_PlaySe(0x28);
    addressee = 0xf;
    PopupChoice_Close(PocketMenu_Popup(this), 1);
    setMainState(0x24);
    hideCursor();
}

s32 PocketMenu::playActionSe() {
    switch (chosenAction) {
    case 0x12:
    case 0x13:
        Snd_PlaySe(0x50);
        break;
    case 0x14:
        Snd_PlaySe(0x71);
        break;
    case 9:
        Snd_PlaySe(0x24);
        return 0;
    case 2:
    case 0x16:
    case 0x17:
        Snd_PlaySe(0x25);
        return 0;
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x20:
        return 0;
    default:
        break;
    }
    return 1;
}

void PocketMenu::addItemOptions(s32 a) {
    volatile u16 v;
    if (MenuCtrl_IsButtons()) {
        ChoiceIdList_Add(optionList, 0, 0);
    }
    s32 r4 = PocketMenu_GetItemFlags(this, a);
    a = PocketMenu_GetItem(this, a);
    v = a;
    if (r4 == 0) {
        if (Unk_ov096_02295a44_Range(&v, 0x156c, 0x156c)) {
            if (((HudCountdown *)(Hud_GetCountdown()))->isStopped()) {
                ChoiceIdList_Add(optionList, 0xdc, 0x1b);
            } else {
                ChoiceIdList_Add(optionList, 0xe1, 0x1c);
            }
        }
    }
    switch (r4) {
    case 0:
        if (canDropItem(a)) {
            ChoiceIdList_Add(optionList, Unk_ov096_0229590c_IsZero(gFieldSceneKind) ? 1 : 0xa, 2);
        }
#ifdef N3DS_DIRECT
        if (n3ds_fruit_count(a) > 1 && PocketMenu_FindEmptyPocket(this) != 0x26) {  // (PocketMenu_TakeOneFruit)
            LabelString t;
            t.set((u8 *)"Take One");
            PocketMenu_Popup(this)->addCustomRow((PopupChoiceIdList *)optionList, &t, N3DS_ACTION_TAKE_ONE);
        }
#endif
        if (Unk_ov096_0229590c_IsZero(gFieldSceneKind)) {
            addFieldOptions(a);
        } else if (Scene_InHouseRoom()) {
            PocketMenu_AddRoomItemOptions(this, a);
        }
        {
            BOOL r = FALSE;
            u32 a = v;
            u32 b = v;
            if (b >= 0x1000 && a <= 0x10ff) r = TRUE;
            if (r) {
                ChoiceIdList_Add(optionList, 0x1c, 7);
            } else if (a >= 0x151f && a <= 0x151f) {
                ChoiceIdList_Add(optionList, 0x1c, 8);
            }
        }
        break;
    case 1:
        ChoiceIdList_Add(optionList, 9, 0x14);
        break;
    case 2:
        if (Shared_InvItem_IsDeliveryItem(a) == 0) {
            ChoiceIdList_Add(optionList, 0x17, 0x14);
        }
        break;
    }
}

void PocketMenu::addLetterOptions(s32 a) {
    LetterView *o = PocketMenu_GetLetter(this, a);
    if (MenuCtrl_IsButtons()) {
        ChoiceIdList_Add(optionList, 0, 1);
    }
    s32 t = o->getState();
    clearFlags(8);
    switch (t) {
    case 1:
        ChoiceIdList_Add(optionList, 0x16, 5);
        ChoiceIdList_Add(optionList, 0x20, 4);
        break;
    case 4:
        if (Unk_ov096_0229590c_IsZero(gFieldSceneKind)) {
            addBottleOption();
        }
        ChoiceIdList_Add(optionList, 0x16, 5);
        if (testFlags(8) == 0) {
            if (o->getPresent() == 0xfff1) {
                if (Unk_ov096_0229590c_IsZero(gFieldSceneKind)) {
                    ChoiceIdList_Add(optionList, 0x15, 0xa);
                }
            }
        }
        break;
    case 7:
        ChoiceIdList_Add(optionList, 0x17, 3);
        break;
    case 0:
        break;
    default:
        ChoiceIdList_Add(optionList, 0x14, 3);
        break;
    }
    if (o->getPresent() != 0xfff1) {
        ChoiceIdList_Add(optionList, 0x18, 6);
    } else if (t == 1 || t == 3 || t == 6) {
        if (Unk_ov096_0229590c_IsZero(gFieldSceneKind)) {
            ChoiceIdList_Add(optionList, 0x15, 0xa);
        }
    }
}

void PocketMenu::addTakeOutBellsOptions() {
    s32 r = getWalletBells();
    if (r >= 0x64) {
        ChoiceIdList_Add(optionList, 0x6e, 0xb);
        ChoiceIdList_Add(optionList, 0x6f, 0xc);
    }
    if (r >= 0x3e8) {
        ChoiceIdList_Add(optionList, 0x70, 0xd);
    }
    if (r >= 0x2710) {
        ChoiceIdList_Add(optionList, 0x71, 0xe);
    }
}

void PocketMenu::addTakeOffOptions() {
    if (getEquipped(5) != 0xfff1) {
        ChoiceIdList_Add(optionList, 0x75, 0x11);
    }
    if (getEquipped(4) != 0xfff1) {
        ChoiceIdList_Add(optionList, 0x76, 0x10);
    }
    if (getEquipped(3) != 0xfff1) {
        ChoiceIdList_Add(optionList, 0x77, 0xf);
    }
}

void PocketMenu::openTargetOptions(u32 a, s32 b) {
    clearFlags(0x40000);
    actionTarget = a;
    ChoiceIdList_Clear(optionList, 0x22);
    if (PocketMenu_IsPocketTarget(this, a)) {
        addItemOptions(a);
    } else if (PocketMenu_IsLetterTarget(this, a)) {
        addLetterOptions(a);
    } else if (a == 0x25) {
        addTakeOutBellsOptions();
    } else if (a == 0x24) {
        addTakeOffOptions();
    } else if (a != 0x27) {
        return;
    }
    if (ChoiceIdList_Count(optionList) == 0) {
        if (PocketMenu_IsPocketTarget(this, a) || PocketMenu_IsLetterTarget(this, a)) {
            ChoiceIdList_Add(optionList, 0x7c, 0x22);
        } else if (a == 0x25 || a == 0x27) {
            ChoiceIdList_Add(optionList, 0x7b, 0x22);
            actionTarget = 0x25;
        } else if (a == 0x24) {
            ChoiceIdList_Add(optionList, 0x7a, 0x22);
        }
    } else {
        ChoiceIdList_Add(optionList, 2, 0x22);
    }
    hideCursor();
    if (b == 0) {
        m_23c0.hide(1);
    }
    showOptionList(b);
    if (PocketMenu_IsPocketTarget(this, a) == 0 && a != 0x24) {
        requestCameraPop();
    }
}

void PocketMenu::openConfirmList() {
    setFlags(0x40000);
    ChoiceIdList_Clear(optionList, 0x22);
    ChoiceIdList_Add(optionList, 0x1a, 0x22);
    ChoiceIdList_Add(optionList, 0x15, 9);
    ChoiceIdList_Add(optionList, 0x19, 0x22);
    showOptionList(0);
}

void PocketMenu::openAddresseeList() {
    addresseePage = 0;
    openAddresseePage(1);
}

void PocketMenu::openAddresseePage(s32 x) {
    hideCursor();
    AddresseePageArg page = {addresseePage};
    PopupChoice_OpenAddresseePage(PocketMenu_Popup(this), page, x);
    setMainState(0x22);
}

// ===== unit 0229567c =====

extern "C" u32 PocketMenu_GetPlayerSlot() {
    CommManager *g = gCommManager;
    u32 v = g->myAid;
    if (g->isSlotActive(v)) {
        return (u8)v;
    }
    return 0;
}

void PocketMenu::moveCursorInPockets(u32 pad, s32 mode) {
    s32 col = cursorTarget;
    s32 row = 0;
    while (col >= 5) {
        col -= 5;
        row++;
    }
    if (MenuKeys_HasLeft(pad)) {
        if (!MenuKeys_HasUp(pad) || row == 0) {
            if (col == 0) {
                if (row == 2 && isHoldingShirt()) {
                    cursorTarget = 0x21;
                    return;
                }
                cursorTarget = (row + 2) * 2 + 0x10;
                setFlags(0x10);
            } else {
                cursorTarget = cursorTarget - 1;
                col = col - 1;
            }
        }
    } else if (MenuKeys_HasRight(pad)) {
        if (!MenuKeys_HasDown(pad)) {
            if (col == 4) {
                cursorTarget = (row + 2) * 2 + 0xf;
            } else {
                cursorTarget = cursorTarget + 1;
                col = col + 1;
            }
        }
    }
    if (PocketMenu_IsPocketTarget(this, cursorTarget)) {
        if (!testFlags(0x30)) {
            if (MenuKeys_HasUp(pad)) {
                if (row > 0) {
                    cursorTarget = cursorTarget - 5;
                } else if (col <= 2 && canNavToWallet(mode)) {
                    cursorTarget = 0x25;
                } else if (canNavToDropArea(mode)) {
                    cursorTarget = 0x22;
                } else if (canNavToPlayer(mode)) {
                    cursorTarget = 0x24;
                }
            } else if (MenuKeys_HasDown(pad)) {
                if (row < 2) {
                    cursorTarget = cursorTarget + 5;
                }
            }
        }
    }
}

void PocketMenu::moveCursorInLetters(u32 pad, s32 mode) {
    s32 t = cursorTarget - 0xf;
    s32 row = t >> 1;
    if (MenuKeys_HasLeft(pad)) {
        if ((t & 1) > 0) {
            cursorTarget = cursorTarget - 1;
        } else if (mode == 2) {
        } else if (row >= 2) {
            cursorTarget = (row - 2) * 5 + 4;
        } else {
            s32 a = canNavToDropArea(mode);
            s32 b = canNavToPlayer(mode);
            if (a & b) {
                if (row == 0) {
                    cursorTarget = 0x24;
                } else {
                    cursorTarget = 0x22;
                }
            } else if (a != 0) {
                cursorTarget = 0x22;
            } else if (b != 0) {
                cursorTarget = 0x24;
            } else if (canNavToWallet(mode)) {
                cursorTarget = 0x25;
            } else {
                cursorTarget = 4;
            }
        }
    } else if (MenuKeys_HasRight(pad)) {
        if ((t & 1) < 1) {
            cursorTarget = cursorTarget + 1;
        } else if (mode == 2) {
        } else {
            setFlags(0x20);
            if (row < 2) {
                if (canNavToWallet(mode)) {
                    cursorTarget = 0x25;
                } else {
                    s32 a = canNavToDropArea(mode);
                    s32 b = canNavToPlayer(mode);
                    if (a & b) {
                        if (row == 0) {
                            cursorTarget = 0x24;
                        } else {
                            cursorTarget = 0x22;
                        }
                    } else if (a != 0) {
                        cursorTarget = 0x22;
                    } else if (b != 0) {
                        cursorTarget = 0x24;
                    } else {
                        cursorTarget = 0;
                    }
                }
            } else {
                if (row == 4 && isHoldingShirt()) {
                    cursorTarget = 0x21;
                    return;
                }
                cursorTarget = (row - 2) * 5;
            }
        }
    }
    if (PocketMenu_IsLetterTarget(this, cursorTarget)) {
        if (!testFlags(0x30)) {
            if (MenuKeys_HasUp(pad)) {
                if (row > 0) {
                    cursorTarget = cursorTarget - 2;
                } else if (mode == 0) {
                    PocketMenu_MoveCursorToTab(this);
                    m_2498.switchToAnim0D();
                }
            } else if (MenuKeys_HasDown(pad)) {
                if (row < 4) {
                    cursorTarget = cursorTarget + 2;
                }
            }
        }
    }
}

void PocketMenu::moveCursorInTabs(u32 pad, s32 mode) {
#ifdef N3DS_DIRECT
    // the options button: Left from the pockets' tab, Right back to it, Down as from the pockets' tab
    if (cursorTarget == POCKET_OPT_TARGET) {
        if (MenuKeys_HasDown(pad)) {
            cursorTarget = 0x25;
            m_2498.switchToAnim01();
        } else if (MenuKeys_HasRight(pad)) {
            cursorTarget = 0x19;
        }
        return;
    }
    if (cursorTarget == 0x19 && MenuKeys_HasLeft(pad) && !MenuKeys_HasDown(pad)) {
        cursorTarget = POCKET_OPT_TARGET;
        return;
    }
#endif
    if (MenuKeys_HasLeft(pad)) {
        if (*(volatile u8 *)&cursorTarget > 0x19) {
            cursorTarget = cursorTarget - 1;
        }
    } else if (MenuKeys_HasRight(pad)) {
        if (*(volatile u8 *)&cursorTarget < 0x20) {
            cursorTarget = cursorTarget + 1;
        }
    }
    if (MenuKeys_HasDown(pad)) {
        s32 d = cursorTarget - 0x19;
        if (d == 7) {
            cursorTarget = 0x10;
        } else if (d == 6) {
            cursorTarget = 0xf;
        } else if (d == 0) {
            cursorTarget = 0x25;
        } else {
            cursorTarget = 0x24;
        }
        m_2498.switchToAnim01();
    }
}

void PocketMenu::moveCursorInPlayerArea(u32 pad, s32 mode) {
    u32 v = cursorTarget;
    if (v == 0x24) {
        if (MenuKeys_HasLeft(pad)) {
            if (canNavToWallet(mode)) {
                cursorTarget = 0x25;
            } else {
                setFlags(0x10);
                cursorTarget = 0x10;
            }
        } else if (MenuKeys_HasRight(pad)) {
            cursorTarget = 0xf;
        } else if (MenuKeys_HasUp(pad)) {
            if (mode == 0) {
                PocketMenu_MoveCursorToTab(this);
                m_2498.switchToAnim0D();
            }
        } else if (MenuKeys_HasDown(pad)) {
            if (canNavToDropArea(mode)) {
                cursorTarget = 0x22;
            } else {
                cursorTarget = 4;
            }
        }
    } else if ((u8)(v + 0xde) <= 1) {
        if (MenuKeys_HasLeft(pad)) {
            if (canNavToWallet(mode)) {
                cursorTarget = 0x25;
            } else {
                setFlags(0x10);
                cursorTarget = 0x12;
            }
        } else if (MenuKeys_HasRight(pad)) {
            cursorTarget = 0x11;
        } else if (MenuKeys_HasUp(pad)) {
            if (canNavToPlayer(mode)) {
                cursorTarget = 0x24;
            }
        } else if (MenuKeys_HasDown(pad)) {
            cursorTarget = 4;
        }
    } else if (v == 0x25) {
        if (MenuKeys_HasLeft(pad)) {
            setFlags(0x10);
            cursorTarget = 0x12;
        } else if (MenuKeys_HasRight(pad)) {
            if (canNavToDropArea(mode)) {
                cursorTarget = 0x22;
            } else if (canNavToPlayer(mode)) {
                cursorTarget = 0x24;
            } else {
                cursorTarget = 0x11;
            }
        } else if (MenuKeys_HasUp(pad)) {
            if (mode == 0) {
                PocketMenu_MoveCursorToTab(this);
                m_2498.switchToAnim0D();
            }
        } else if (MenuKeys_HasDown(pad)) {
            cursorTarget = 1;
        }
    }
}

BOOL PocketMenu::moveCursorByPad(u32 pad, s32 mode) {
    u8 old = cursorTarget;
    clearFlags(0x30);
    if (pad == 0) {
        return FALSE;
    }
    if (PocketMenu_IsPocketTarget(this, cursorTarget)) {
        moveCursorInPockets(pad, mode);
    } else if (PocketMenu_IsLetterTarget(this, cursorTarget)) {
        moveCursorInLetters(pad, mode);
    } else if (PocketMenu_IsTabTarget(this, cursorTarget)) {
        moveCursorInTabs(pad, mode);
    } else if (PocketMenu_IsSpecialTarget(this, cursorTarget)) {
        moveCursorInPlayerArea(pad, mode);
    } else if (cursorTarget == 0x21) {
        if (MenuKeys_HasRight(pad)) {
            cursorTarget = 0xa;
        } else if (MenuKeys_HasLeft(pad)) {
            cursorTarget = 0x18;
            setFlags(0x10);
        }
    }
    if (old != cursorTarget) {
        return TRUE;
    }
    return FALSE;
}

void PocketMenu::actionReadLetter() {
    setTransitionState(5);
    setPhase(1);
    setFlags(0x80);
    hideTabBar();
}

void PocketMenu::closeLetterView() {
    setMainState(0x21);
    m_2b14.setState(2);
    Snd_PlaySe(0x29);
}

void PocketMenu::closeLetterViewAndMenu() {
    closeLetterView();
    ((MenuTabBar *)(ProcBase_GetParent(this)))->selectTab(7);
    setFlags(0x20000);
}

LetterView *PocketMenu::getActionLetter() { return PocketMenu_GetLetter(this, actionTarget); }

void PocketMenu::writeLetterOnPaper() {
    u16 v;
    LetterView *r4 = getActionLetter();
    v = PocketMenu_GetItem(this, paperTarget);
    s32 r6 = Item_GetPaperIndex(&v);
    Letter_InitDraft(r4, (u8)r6);
    PocketMenu_Popup(this)->applyAddressee(r4, addressee);
    MenuCtrl_SetArg(r4);
    requestTab(8);
    hideTabBar();
    u32 n = Item_GetPaperCount(&v);
    s32 r2;
    if (n <= 1) {
        r2 = 0xfff1;
    } else {
        r2 = Item_MakePaper(r6, n - 1);
    }
    PocketMenu_SetSlotItem(this, paperTarget, r2, 0);
}

void PocketMenu::requestCameraPush() {
    setFlags(0x400);
    setFlags(0x800);
}

void PocketMenu::requestCameraPop() {
    clearFlags(0x400);
    setFlags(0x800);
}

void PocketMenu::updateCameraView() {
    if (testFlags(0x800)) {
        if (Camera_IsViewPushed()) {
            if (!testFlags(0x400)) {
                Camera_PopView();
            }
        } else {
            if (testFlags(0x400)) {
                Camera_PushView();
            }
        }
        clearFlags(0x800);
    }
}

void PocketMenu::wearHeldShirt() {
    struct {
        u16 a;
        u16 b;
    } l;
    l.a = *(u16 *)PlayerData_GetCurrent()->getInventoryBackground();
    l.b = handItem;
    MenuScreen_UploadClothPattern(&l.b, &bgTasks[1], clothImage, clothPalette);
    BOOL ok = FALSE;
    volatile u16 *pv = &l.a;
    u16 a = *pv;
    u16 b = *pv;
    if (b >= 0x11a8 && a <= 0x12a7) {
        ok = TRUE;
    }
    if (ok) {
        handItem = a;
        returnHand(1);
    } else {
        clearHand();
    }
    Snd_PlaySe(0x6b);
}

BOOL PocketMenu::isHoldingShirt() {
    struct Pad {
        s32 v[2];
        Pad() {}
        ~Pad() {}
    } pad;
    BOOL r;
    if (handKind == 2 && handItemFlags == 0 && handItem >= 0x11a8 && handItem <= 0x12a7) {
        r = TRUE;
    } else {
        r = FALSE;
    }
    return r;
}

BOOL PocketMenu::testFlags(u32 mask) {
    if (stateFlags & mask) {
        return TRUE;
    }
    return FALSE;
}

void PocketMenu::setFlags(u32 mask) { stateFlags = stateFlags | mask; }

void PocketMenu::clearFlags(u32 mask) { stateFlags = stateFlags & ~mask; }

// ===== unit 02294c40 =====

// destructor is implicit (member destructors run in reverse order)

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_02294c40) {
    HOST_OVERLAY_VAR(sPocketMenuProfile),
    HOST_OVERLAY_VARS_END
};

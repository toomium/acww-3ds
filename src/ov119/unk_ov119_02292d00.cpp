#include "ds_div.h"
#include "host_shared_calls.h"
#include "uptr.h"
#include "talk/EncodedString8B.h"
#include "talk/MsgString9B.h"
#include "talk/MsgString9C.h"
#include "types.h"
#include "Unk_020d8c7c.h"
#include "menu/MenuProc.h"
#include "ui/HandCursor.h"
#include "talk/MsgString193.h"
#include "ui/LabelString.h"
#include "gfx/BgVramTask.h"
#include "menu/MenuCursor.h"
#include "menu/PopupChoiceMenu.h"
#include "menu/MenuErrorMessage.h"
#include "sys/ProcProfile.h"
#include "net/CommManager.h"
#include "nitro/gxoam.h"
#include "menu/MenuTabBar.h"
#include "player/PlayerData.h"
#include "player/PlayerId.h"
#include "talk/MsgString.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e9a08.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_02051218.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0206f834.h"
#include "proto/main/unk_020742f4.h"
#include "proto/main/unk_02087e0c.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_020b2c50.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov002/unk_ov002_022013ac.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#include "proto/ov119/unk_ov119_02292d00.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_020637e4.h"
#include "proto/main/unk_02093ff0.h"
#include "net/FriendEntry.h"

extern "C" {
extern u8 gU8None;
extern u16 gPad[];

FriendEntry *FriendList_GetEntries(void *p);
BOOL DwcFriendData_Compare(void *p, void *q);
void FriendList_Compact(void *p);
void EncodedString_SetRaw(void *dst, void *src, s32 n);
u8 *Msg_SkipLines(u8 *p, s32 i);
void MI_CpuCopy8(void *src, void *dst, s32 n);
void MIi_CpuCopy16(void *src, void *dst, s32 n);

void PopupChoice_Close(void *p, u32 v);
void PopupChoice_Open(void *p, u32 v);
BOOL PopupChoice_TickDecideDelay(void *self);
u8 PopupChoice_DecideCancel(void *self, s32 x);
void PopupChoice_DecideRow(void *p, s32 a, s32 b);
BOOL PopupChoice_MoveCursor(void *p, u32 idx, void *q, u8 flag);
s32 PopupChoice_Update(void *p);
s32 PopupChoice_ForceClose(void *p);
void PopupChoice_LoadFriendBg(void *p);
void PopupChoice_Draw(void *p);


extern CommManager *gCommManager;
extern GXOamAttr data_ov119_02295588;
}

struct PopupChoiceIdList;

#define U970_A ((MenuCursorBase *)&cursor)
#define U970_B ((MenuCursor *)&cursor)
#define U970_C ((HandCursor *)&cursor)

// stack helper objects (ctor/dtor are plain calls into main)

class FriendRosterTab;
typedef void (FriendRosterTab::*Unk_ov119_02295840_Fn)();

#define A0V (*(volatile u8 *)&focus)
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
// the options button as a focus in the tab row, left of the pockets' tab (0x17 is "none"): a tab focus whose tab is
// N3DS_OPT_TAB
#define FRIEND_OPT_FOCUS 0x18
#define FRIEND_IS_TAB(v) (((v) >= 0xe && (v) <= 0x15) || (v) == FRIEND_OPT_FOCUS)
#else
#define FRIEND_IS_TAB(v) ((v) >= 0xe && (v) <= 0x15)
#endif
#define E9V (*(volatile u8 *)&labelCount)
#define B1V (*(volatile u8 *)&delayTimer)

// Vtable 0x02295840, size 0x1bcc
class FriendRosterTab : public MenuProc {
public:
    FriendRosterTab() : popup(), textLabels(), helpText(), cursor(), vramTasks(), errorMessage() {}

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
    void updateRowMarkBlink();
    void clearRowMarks();
    void drawRowMarks();
    void cancelChoice();
    void openPopup(u32 x);
    void onPopupChoice();
    u8 getPopupRowValue(u32 idx);
    void loadOwnFriendCode();
    u32 getPageOfFriend(s32 idx);
    void setPage(u8 s);
    void refreshPage();
    void drawRosterPage();
    void setRowIcon(u16 *p, u32 v);
    void drawFriendCodeHelp();
    void drawOwnCodePage();
    void drawPresentPage();
    BOOL isInRoster(void *p);

    s32 findFreeEntry();
    BOOL syncFromWifiFriendList();
    void compactRoster();
    BOOL hasGaps();
    void buildFriendIndex();
    u32 getFocusedFriendIndex();
    FriendEntry *getFocusedFriendEntry();
    FriendEntry *getFriendEntries();
    BOOL isWifiMode();
    void setPaletteFade(s32 x);
    u16 blendColor(s32 c1, s32 c2, s32 t, s32 n);
    void uploadScreen();
    u8 hitTest(s32 x, s32 y);
    BOOL activateFocus();
    void showFriend(s32 x);
    void registerPresentPlayer();
    void useCodeEntry();
    void openFriendEditor(s32 x);
    void removeFriend();
    void openRemoveConfirm();
    void openFriendChoices();
    void openRegisterChoices();

    BOOL moveFocus(u32 keys);
    void refreshCursor();
    void releaseCursor();
    void pressCursor();
    void cursorToPopupTop();
    void cancelPopup();
    void moveCursorToPopupRow();
    void moveCursorToTarget();
    void hideCursor();
    s32 getFocusY();
    s32 getFocusX();
    void showCursor();
    void addTitleLabel(u32 id);
    void addRegisterLabel();
    LabelString *allocTextLabel();
    void resetTextLabels();
    void showError(u8 v);
    void resumeInput();
    void startButtonInput();
    void startTouchInput();
    void mainAct11();
    void mainAct10();
    void mainAct0F();
    void mainAct0E();
    void mainAct0D();
    void mainAct0C();
    void mainAct0B();
    void mainAct0A();
    void mainAct09();
    void mainAct08();
    void mainAct07();

    void mainAct06();
    void updateCursorRelease();
    void updateCursorPress();
    void updateCursorMove();
    void updateButtons();
    void mainAct01();
    void updateTouch();
    void loadObjGfx();
    void loadBgGfx();
    void postInputUpdate();
    void preInputUpdate();
    void postStateUpdate();
    void preStateUpdate();
    void releaseResources();
    void initFriendRoster();
    void stateClosing();
    void stateClose();
    void stateOpening();
    void stateOpen();
    void stateLoad();
    void hideTabBar();
    BOOL requestTab(s32 idx);
    BOOL handleTabSwitch();
    void runMainState();

    /* 0x091 */ u8 unk_91[3];
    /* 0x094 */ u32 slideY;
    /* 0x098 */ u16 flags;
    /* 0x09a */ s16 selectedRow;
    /* 0x09c */ u8 page;
    /* 0x09d */ u8 targetPage;
    /* 0x09e */ u8 labelCount;
    /* 0x09f */ u8 returnState;
    /* 0x0a0 */ u8 focus;
    /* 0x0a1 */ u8 ownCodeDigits[12];
    /* 0x0ad */ u8 popupValue;
    /* 0x0ae */ u8 popupRow;
    /* 0x0af */ u8 rowCount;
    /* 0x0b0 */ u8 registerButtonPalette;
    /* 0x0b1 */ u8 delayTimer;
    /* 0x0b2 */ u8 rowMarkMask;
    /* 0x0b3 */ u8 rowMarkBlinkTimer;
    /* 0x0b4 */ u8 friendIndex;
    /* 0x0b5 */ u8 unk_b5[3];
    /* 0x0b8 */ s32 fadeLevel;
    /* 0x0bc */ u8 rosterIndices[0x20];
    /* 0x0dc */ PopupChoiceMenu popup;
    /* 0x3d0 */ PopupChoiceIdList choiceList;
    /* 0x3db */ u8 pad_3db[1];
    /* 0x3dc */ LabelString textLabels[0x13];
    /* 0x89c */ MsgString193 helpText;
    /* 0x970 */ MenuCursorBuf0 cursor;
    /* 0x9d4 */ u16 pageScreens[0x800];
    /* 0x19d4 */ void *shownScreen;
    /* 0x19d8 */ u16 objPalette[16];
    /* 0x19f8 */ u16 bgPalette[16];
    /* 0x1a18 */ u16 objPaletteWork[16];
    /* 0x1a38 */ u16 bgPaletteWork[16];
    /* 0x1a58 */ BgVramTask vramTasks[3];
    /* 0x1ac4 */ MenuErrorMessage errorMessage;
};

static inline BOOL Unk_ov119_02294a44_Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) {
        return TRUE;
    }
    return FALSE;
}

extern "C" FriendRosterTab *FriendRosterTab_Create() { return new FriendRosterTab(); }

BOOL FriendRosterTab::onCreate() {
    initFriendRoster();
    setTransitionState(0);
    setPhase(1);
    return TRUE;
}

BOOL FriendRosterTab::onDelete() {
    ((MenuTabBar *)(ProcBase_GetParent(this)))->onTabMenuClosed();
    releaseResources();
    return TRUE;
}

BOOL FriendRosterTab::onDraw() {
    s32 p = slideY + 0x60;
    s32 i;
    if (!testFlags(1)) {
        return TRUE;
    }
    PopupChoice_Draw(&popup);
    if (MenuCtrl_IsButtons()) {
        U970_A->drawWrapped();
    }
    Oam_DrawCell(1, data_ov119_02295660, 0x80, p, -1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
    for (i = 0; i < 6; i++) {
        Oam_DrawCell(1, data_ov119_02295648[i], 0x80, p, i == targetPage ? 5 : 4, 2, 0x1000, 0x1000, 0, -1, 0, 0);
    }
    Oam_DrawCell(1, data_ov119_02295778, 0x80, p, registerButtonPalette, 2, 0x1000, 0x1000, 0, -1, 0, 0);
    s32 x = 0x80;
    switch (page) {
    case 4:
        for (i = 0; i < 12; i++) {
            data_ov119_02295588.attr2 = (data_ov119_02295588.attr2 & 0xfffffc00) | (u16)(ownCodeDigits[i] * 2 + 0x1a0) & 0x3ff;
            Oam_DrawObj(1, &data_ov119_02295588, x, p, -1, 2, NULL);
            x += 14;
        }
        Oam_DrawCell(1, data_ov119_022957d8, 0x80, p, -1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
        break;
    case 5:
        break;
    default:
        if (selectedRow != -1) {
            Oam_DrawCell(1, data_ov119_022957a8, x, p + selectedRow * 16 - 16, -1, 2, 0x1000, 0x1000, 0, -1, 0, 0);
        }
        break;
    }
    return TRUE;
}// Declarations for data defined further down (definition order sets the data layout)
extern "C" GXOamAttr data_ov119_02295660[4];
extern "C" GXOamAttr data_ov119_022957d8[12];
extern "C" GXOamAttr data_ov119_02295680[5];
extern "C" ProcProfile sFriendRosterTabProfile;
extern "C" GXOamAttr data_ov119_022956d0[5];
extern "C" GXOamAttr data_ov119_022956f8[5];
extern "C" GXOamAttr data_ov119_02295720[5];
extern "C" GXOamAttr data_ov119_022956a8[5];
extern "C" GXOamAttr data_ov119_02295748[6];
extern "C" GXOamAttr data_ov119_02295588;
extern "C" GXOamAttr data_ov119_02295778[6];
extern "C" GXOamAttr data_ov119_022957a8[6];

extern "C" GXOamAttr *data_ov119_02295648[6] = {data_ov119_02295720, data_ov119_02295680, data_ov119_022956a8, data_ov119_022956d0, data_ov119_022956f8, data_ov119_02295748};

extern "C" GXOamAttr data_ov119_02295660[4] = {{0x81b440bb, 0x160, 1, 0x4, 0x0}, {0x81d440bb, 0x164, 1, 0x4, 0x0}, {0x81f440bb, 0x168, 1, 0x4, 0x0}, {0x1480bb, 0x16c, 1, 0x4, 0xffff}};

extern "C" GXOamAttr data_ov119_022957d8[12] = {{0x314000, 0x1ba, 0, 0xa, 0x0}, {0x234000, 0x1ba, 0, 0xa, 0x0}, {0x154000, 0x1ba, 0, 0xa, 0x0}, {0x74000, 0x1ba, 0, 0xa, 0x0}, {0x1f94000, 0x1b8, 0, 0xa, 0x0}, {0x1eb4000, 0x1b8, 0, 0xa, 0x0}, {0x1dd4000, 0x1b8, 0, 0xa, 0x0}, {0x1cf4000, 0x1b8, 0, 0xa, 0x0}, {0x1c14000, 0x1b6, 0, 0xa, 0x0}, {0x1b34000, 0x1b6, 0, 0xa, 0x0}, {0x1a54000, 0x1b6, 0, 0xa, 0x0}, {0x1974000, 0x1b6, 0, 0xa, 0xffff}};

BOOL FriendRosterTab::execTransition() {
    static Unk_ov119_02295840_Fn tbl[5] = {
        &FriendRosterTab::stateLoad, &FriendRosterTab::stateOpen,
        &FriendRosterTab::stateOpening, &FriendRosterTab::stateClose,
        &FriendRosterTab::stateClosing};
    preStateUpdate();
    (this->*tbl[transitionState])();
    postStateUpdate();
    return TRUE;
}

extern "C" GXOamAttr data_ov119_02295680[5] = {{0x404d00e7, 0xc2, 1, 0x4, 0x0}, {0x804240e5, 0x10b, 1, 0x4, 0x0}, {0x6280e5, 0x10f, 1, 0x4, 0x0}, {0x404240f5, 0x14b, 1, 0x4, 0x0}, {0x6200f5, 0x14f, 1, 0x4, 0xffff}};

extern "C" ProcProfile sFriendRosterTabProfile = {(void *(*)())FriendRosterTab_Create, 0xa2, 0xa6};

extern "C" GXOamAttr data_ov119_022956d0[5] = {{0x404d0007, 0xc6, 1, 0x4, 0x0}, {0x80424005, 0x10b, 1, 0x4, 0x0}, {0x628005, 0x10f, 1, 0x4, 0x0}, {0x40424015, 0x14b, 1, 0x4, 0x0}, {0x620015, 0x14f, 1, 0x4, 0xffff}};

extern "C" GXOamAttr data_ov119_022956f8[5] = {{0x404d0017, 0xc8, 0, 0x4, 0x0}, {0x80424015, 0x10b, 1, 0x4, 0x0}, {0x628015, 0x10f, 1, 0x4, 0x0}, {0x40424025, 0x14b, 1, 0x4, 0x0}, {0x620025, 0x14f, 1, 0x4, 0xffff}};

extern "C" GXOamAttr data_ov119_02295720[5] = {{0x404d00d7, 0xc0, 1, 0x5, 0x0}, {0x804240d5, 0x10b, 1, 0x5, 0x0}, {0x6280d5, 0x10f, 1, 0x5, 0x0}, {0x404240e5, 0x14b, 1, 0x4, 0x0}, {0x6200e5, 0x14f, 1, 0x4, 0xffff}};

void FriendRosterTab::runMainState() {
    static Unk_ov119_02295840_Fn tbl[18] = {
        &FriendRosterTab::updateTouch, &FriendRosterTab::mainAct01,
        &FriendRosterTab::updateButtons, &FriendRosterTab::updateCursorMove,
        &FriendRosterTab::updateCursorPress, &FriendRosterTab::updateCursorRelease,
        &FriendRosterTab::mainAct06, &FriendRosterTab::mainAct07,
        &FriendRosterTab::mainAct08, &FriendRosterTab::mainAct09,
        &FriendRosterTab::mainAct0A, &FriendRosterTab::mainAct0B,
        &FriendRosterTab::mainAct0C, &FriendRosterTab::mainAct0D,
        &FriendRosterTab::mainAct0E, &FriendRosterTab::mainAct0F,
        &FriendRosterTab::mainAct10, &FriendRosterTab::mainAct11};
    (this->*tbl[mainState])();
}

BOOL FriendRosterTab::execMain() {
    if (handleTabSwitch()) {
        return TRUE;
    }
    preInputUpdate();
    runMainState();
    postInputUpdate();
    return TRUE;
}

BOOL FriendRosterTab::execPhase3() { return TRUE; }

BOOL FriendRosterTab::execPhase4() { return TRUE; }

BOOL FriendRosterTab::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

BOOL FriendRosterTab::handleTabSwitch() {
    MenuCtrl_TickForceClose();
    if (MenuCtrl_IsForceCloseDue()) {
        switch (mainState) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 8:
        case 14:
        case 15:
            return requestTab(7);
        }
    }
    if (mainState != 0 && mainState != 2) {
        return FALSE;
    }
    s32 r5 = -1;
    if (MenuCtrl_IsTouch()) {
        r5 = MenuTabBar_HitTestTouch();
    } else {
        u32 k = gPad[1];
        if (k & 2) {
            r5 = 7;
        } else if (k & 0x800) {
            r5 = 0;
        } else if (k & 0x400) {
            r5 = 5;
        } else if (k & 4) {
            r5 = 4;
        }
    }
    return requestTab(r5);
}

BOOL FriendRosterTab::requestTab(s32 idx) {
    void *o = ProcBase_GetParent(this);
    if (idx != -1 && idx != 6) {
        ((MenuTabBar *)(o))->selectTab((u8)idx);
        transitionState = 3;
        setPhase(1);
        return TRUE;
    }
    return FALSE;
}

void FriendRosterTab::hideTabBar() {
    ((MenuTabBar *)(ProcBase_GetParent(this)))->hideTabs();
}

void FriendRosterTab::stateLoad() {
    FriendRosterTab_SetupLayers();
    loadBgGfx();
    loadObjGfx();
    addRegisterLabel();
    PopupChoice_LoadFriendBg(&popup);
    u32 v;
    if (MenuCtrl_IsFriendPageFromIndex()) {
        MenuCtrl_ClearFriendPageFromIndex();
        v = MenuCtrl_GetIndex();
    } else {
        v = 0;
    }
    ((FriendRosterTab *)(this))->setPage(getPageOfFriend(v));
    setTransitionState(1);
    stateOpen();
}

void FriendRosterTab::stateOpen() {
    beginSubSlideIn(0xa, 0, 0, 0x30);
    Gfx2d_ShowLayer(6);
    applySlideOffset(6, 0, 0);
    Gfx2d_ShowLayer(4);
    applySlideOffset(4, 0, 0);
    setFlags(1);
    slideY = getSlideOffsetY();
    setFlags(8);
    setTransitionState(2);
}

void FriendRosterTab::stateOpening() {
    if (testFlags(8)) {
        clearFlags(8);
        drawFriendCodeHelp();
    }
    if (stepSlideIn(0)) {
        setPhase(2);
        resumeInput();
    }
    applySlideOffset(6, 0, 0);
    applySlideOffset(4, 0, 0);
    slideY = getSlideOffsetY();
}

void FriendRosterTab::stateClose() {
    hideCursor();
    beginSubSlideOut(0xa, 0, 0, 0x30);
    applySlideOffset(6, 0, 0);
    applySlideOffset(4, 0, 0);
    setTransitionState(4);
    slideY = getSlideOffsetY();
}

void FriendRosterTab::stateClosing() {
    if (stepSlideOut(0)) {
        Gfx2d_ResetLayer(6);
        Gfx2d_ResetLayer(4);
        clearFlags(1);
        setPhase(5);
    } else {
        applySlideOffset(6, 0, 0);
        applySlideOffset(4, 0, 0);
        slideY = getSlideOffsetY();
    }
}

void FriendRosterTab::initFriendRoster() {
    if (!isWifiMode()) {
        if (hasGaps()) {
            compactRoster();
            ((MenuTabBar *)(ProcBase_GetParent(this)))->requestSaveOnClose();
        }
    } else {
        if (syncFromWifiFriendList()) {
            ((MenuTabBar *)(ProcBase_GetParent(this)))->requestSaveOnClose();
        }
    }
    labelCount = 0;
    flags = 0;
    focus = 8;
    shownScreen = pageScreens;
    selectedRow = -1;
    page = 0xff;
    rowCount = 0;
    registerButtonPalette = 7;
    fadeLevel = 5;
    loadOwnFriendCode();
    ((PopupChoiceMenu *)(&popup))->init(3, 1, "menu/friend/c_bg.bsc");
    buildFriendIndex();
}

void FriendRosterTab::releaseResources() {
    PopupChoice_ForceClose(&popup);
    resetTextLabels();
    vramTasks[0].cancel();
    vramTasks[1].cancel();
    vramTasks[2].cancel();
}

void FriendRosterTab::preStateUpdate() {
    resetTextLabels();
    vramTasks[0].cancel();
    vramTasks[1].cancel();
    vramTasks[2].cancel();
}

void FriendRosterTab::postStateUpdate() {
    uploadScreen();
    Shared_PopupChoice_Update(&popup);
}

void FriendRosterTab::preInputUpdate() {
    preStateUpdate();
    cursor.update();
}

void FriendRosterTab::postInputUpdate() { postStateUpdate(); }

extern "C" void FriendRosterTab_SetupLayers() {
    Gfx2d_SetLayerPriority(6, 2);
    Gfx2d_SetLayerPriority(4, 2);
    Gfx2d_SetLayerControl(6, 0, 0, 0);
    Gfx2d_SetLayerControl(4, 0, 0, 0);
}

void FriendRosterTab::loadBgGfx() {
    void *r4 = gCurrentHeap;
    Gfx2d_LoadPaletteFile("menu/friend/bg0.bpl", r4, 6, 1, 1, 5);
    File_LoadToBuffer("menu/friend/bg2.bpl", bgPalette, 0x20);
    Gfx2d_LoadCharFile("menu/friend/bg0.bch", r4, 6, 0x11, 0x11, 0x89);
    Gfx2d_LoadScreenFile("menu/friend/a_bg.bsc", r4, 6);
    File_LoadToBuffer("menu/friend/b_bg.bsc", pageScreens, 0x800);
    File_LoadToBuffer("menu/friend/d_bg.bsc", pageScreens + 0x400, 0x800);
}

void FriendRosterTab::loadObjGfx() {
    void *r4 = gCurrentHeap;
    Gfx2d_LoadCharFile("menu/friend/obj0.bch", r4, 8, 0xc0, 0xc0, 0x15d);
    Gfx2d_LoadCharFile("menu/friend/obj1.bch", r4, 8, 0x1a0, 0x1a0, 0x1df);
    Gfx2d_LoadPaletteFile("menu/friend/obj.bpl", r4, 8, 4, 4, 10);
    File_LoadToBuffer("menu/friend/obj1.bpl", objPalette, 0x20);
}

void FriendRosterTab::updateTouch() {
    if (checkSwitchToButtons(1)) {
        startButtonInput();
    } else if (Unk_ov119_02294a44_Both()) {
        u32 r = hitTest(gTouchCurX, gTouchCurY);
        if (r != 0x17) {
            focus = r;
            activateFocus();
        }
    }
}

void FriendRosterTab::mainAct01() {
    if (MenuCtrl_IsForceCloseDue()) {
        cancelChoice();
    } else if (checkSwitchToButtons(1)) {
        cursorToPopupTop();
        setMainState(6);
    } else if (Unk_ov119_02294a44_Both()) {
        s32 r5 = ((PopupChoiceMenuBody *)(&popup))->hitTestRowOrLast(gTouchCurX, gTouchCurY);
        if (r5 >= 0) {
            if (!(testFlags(0x10) && r5 == 0)) {
                PopupChoice_DecideRow(&popup, r5, 1);
                popupValue = getPopupRowValue(r5);
                setMainState(0xb);
            }
        }
    }
}

void FriendRosterTab::updateButtons() {
    if (checkSwitchToTouch()) {
        startTouchInput();
    } else {
        u32 r = takeRepeatedKeys();
        if (moveFocus(r)) {
            moveCursorToTarget();
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                pressCursor();
            } else if (k & 0x100) {
                requestTab(MenuTabBar_NextTab(6));
            } else if (k & 0x200) {
                requestTab(MenuTabBar_PrevTab(6));
            }
        }
    }
}

void FriendRosterTab::updateCursorMove() {
    if (!U970_A->isMoving()) {
        setMainState(returnState);
        runMainState();
    }
}

void FriendRosterTab::updateCursorPress() {
    if (U970_C->isAnimDone()) {
        s32 r = activateFocus();
        switch (r) {
        case 1:
            break;
        case 2:
            releaseCursor();
            break;
        default:
            releaseCursor();
            break;
        }
    }
}

void FriendRosterTab::updateCursorRelease() {
    if (U970_C->isAnimDone()) {
        refreshCursor();
        setMainState(2);
    }
}

void FriendRosterTab::mainAct06() {
    if (MenuCtrl_IsForceCloseDue()) {
        cancelChoice();
    } else if (checkSwitchToTouch()) {
        hideCursor();
        setMainState(1);
    } else {
        u32 r4 = takeRepeatedKeys();
        u8 f = testFlags(0x10);
        if (PopupChoice_MoveCursor(&popup, r4, &popupRow, f)) {
            moveCursorToPopupRow();
        } else {
            u32 k = gPad[1];
            if (k & 1) {
                U970_B->setPosePress();
                setMainState(7);
            } else if (k & 2) {
                cancelPopup();
            }
        }
    }
}

void FriendRosterTab::mainAct07() {
    if (MenuCtrl_IsForceCloseDue()) {
        cancelChoice();
    } else if (U970_C->isAnimDone()) {
        PopupChoice_DecideRow(&popup, popupRow, 1);
        popupValue = getPopupRowValue(popupRow);
        setMainState(0xb);
    }
}

void FriendRosterTab::mainAct08() {
    if (B1V != 0) {
        B1V = B1V - 1;
    } else {
        hideCursor();
        if (A0V == 0x16) {
            openRegisterChoices();
        } else {
            openFriendChoices();
        }
    }
}

void FriendRosterTab::mainAct09() {
    if (B1V != 0) {
        B1V = 0;
        compactRoster();
        ((MenuTabBar *)(ProcBase_GetParent(this)))->requestSaveOnClose();
        if (isWifiMode()) {
            if (!Net_WifiDeleteFriend(friendIndex)) {
                setMainState(0x11);
            }
        }
    } else {
        setFlags(2);
        refreshPage();
        resumeInput();
    }
}

void FriendRosterTab::mainAct0A() {
    if (((PopupChoiceMenuBody *)&popup)->isOpen()) {
        if (MenuCtrl_IsButtons()) {
            cursorToPopupTop();
            setMainState(6);
        } else {
            setMainState(1);
        }
    }
}

void FriendRosterTab::mainAct0B() {
    if (PopupChoice_TickDecideDelay(&popup)) {
        PopupChoice_Close(&popup, 0);
        if (U970_C->getAnim()) {
            hideCursor();
        }
        setMainState(0xc);
    }
}

void FriendRosterTab::mainAct0C() {
    if (((PopupChoiceMenuBody *)&popup)->isClosed()) {
        onPopupChoice();
    }
}

void FriendRosterTab::mainAct0D() {
    if (errorMessage.update(1)) {
        resumeInput();
    }
}

void FriendRosterTab::mainAct0E() {
    if (fadeLevel > 0) {
        fadeLevel = fadeLevel - 1;
        setPaletteFade(fadeLevel);
    } else {
        setPage(targetPage);
        setMainState(0xf);
    }
}

void FriendRosterTab::mainAct0F() {
    if (fadeLevel < 5) {
        fadeLevel = fadeLevel + 1;
        setPaletteFade(fadeLevel);
    } else if (MenuCtrl_IsTouch()) {
        startTouchInput();
    } else {
        releaseCursor();
    }
}

void FriendRosterTab::mainAct10() {
    FriendEntry *base = getFriendEntries();
    void *r = DwcFriendData_GetBytes(FriendEntry_GetFriendData(&base[friendIndex]));
    if (Net_WifiAddFriend(friendIndex, r)) {
        showFriend(friendIndex);
    }
}

void FriendRosterTab::mainAct11() {
    if (Net_WifiDeleteFriend(friendIndex)) {
        setFlags(2);
        refreshPage();
        resumeInput();
    }
}

void FriendRosterTab::startTouchInput() {
    hideCursor();
    setMainState(0);
}

void FriendRosterTab::startButtonInput() {
    showCursor();
    restartKeyRepeat();
    setMainState(2);
}

void FriendRosterTab::resumeInput() {
    selectedRow = -1;
    setFlags(4);
    if (MenuCtrl_IsTouch()) {
        startTouchInput();
    } else {
        startButtonInput();
    }
}

void FriendRosterTab::showError(u8 v) {
    volatile u8 buf[1];
    buf[0] = gU8None;
    buf[0] = v;
    errorMessage.open((u8 *)buf, 1, 0);
    setMainState(0xd);
    hideCursor();
}

void FriendRosterTab::resetTextLabels() {
    s32 i;
    i = 0;
    E9V = i;
    for (i = 0; i < 0x13; i++) {
        ((LabelString *)(&textLabels[i]))->destroyLabel();
    }
}

LabelString *FriendRosterTab::allocTextLabel() {
    if (E9V >= 0x13) {
        return &textLabels[0x12];
    }
    E9V = E9V + 1;
    return &textLabels[E9V - 1];
}

void FriendRosterTab::addRegisterLabel() {
    void *e = allocTextLabel();
    String_Load2dMenu(e, 0xd6);
    ((LabelString *)(e))->createLabel(8, 0xca, 6, 0xf, 0, 0);
    ((LabelString *)(e))->redrawAligned(1, 0);
}

void FriendRosterTab::addTitleLabel(u32 id) {
    void *e = allocTextLabel();
    String_Load2dMenu(e, id);
    ((LabelString *)(e))->createLabel(8, 0x160, 0xd, 0xf, 0, 0);
    ((LabelString *)(e))->redrawAligned(1, 0);
}

void FriendRosterTab::showCursor() {
    u32 c = A0V;
    if (c <= 7) {
        s32 n = rowCount;
        if (n == 0) {
            A0V = 8;
        } else if (n <= (s32)c) {
            A0V = n - 1;
        }
    }
    s32 a = getFocusX();
    s32 b = getFocusY();
    U970_A->warpTo(a, b);
    u32 v = A0V;
    if (v <= 7) {
        U970_B->setAnimIfChanged(7);
    } else if (FRIEND_IS_TAB(v)) {
        U970_B->setAnimIfChanged(0xd);
    } else {
        U970_B->setAnimIfChanged(1);
    }
    refreshCursor();
}

s32 FriendRosterTab::getFocusX() {
    u32 v = A0V;
    if (v <= 7) {
        return 0x20;
    }
    if (v >= 0xe && v <= 0x15) {
        return MenuTabBar_GetTabX(v - 0xe);
    }
#ifdef N3DS_DIRECT
    if (v == FRIEND_OPT_FOCUS) {
        return MenuTabBar_GetTabX(N3DS_OPT_TAB);
    }
#endif
    if (v >= 8 && v <= 0xd) {
        return 0xd6;
    }
    if (v == 0x16) {
        return 0xdb;
    }
    return 0x80;
}

s32 FriendRosterTab::getFocusY() {
    u32 v = A0V;
    if (v <= 7) {
        return v * 16 + 0x38;
    }
    if (FRIEND_IS_TAB(v)) {
        return 8;
    }
    if (v >= 8 && v <= 0xd) {
        return (v - 8) * 16 + 0x3f;
    }
    if (v == 0x16) {
        return 0xaa;
    }
    return 0x60;
}

void FriendRosterTab::hideCursor() {
    U970_B->setAnimIfChanged(0);
    cursor.update();
}

void FriendRosterTab::moveCursorToTarget() {
    u32 v = A0V;
    if (v <= 7) {
        U970_B->switchToAnim07();
    } else if (FRIEND_IS_TAB(v)) {
        U970_B->switchToAnim0D();
    } else {
        U970_B->switchToAnim01();
    }
    s32 a = getFocusX();
    s32 b = getFocusY();
    U970_A->moveToEase(a, b, 3, 1);
    returnState = mainState;
    setMainState(3);
}

void FriendRosterTab::moveCursorToPopupRow() {
    s32 a = ((PopupChoiceMenuBody *)&popup)->getRowX();
    s32 b = ((PopupChoiceMenuBody *)&popup)->getRowY(popupRow);
    U970_A->moveToLinear(a, b, 2);
    returnState = mainState;
    setMainState(3);
}

void FriendRosterTab::cancelPopup() {
    popupValue = 10;
    popupRow = PopupChoice_DecideCancel(&popup, 1);
    s32 a = ((PopupChoiceMenuBody *)&popup)->getRowX();
    s32 b = ((PopupChoiceMenuBody *)&popup)->getRowY(popupRow);
    U970_A->warpTo(a, b);
    U970_C->setAnimAtEnd(8);
    setMainState(0xb);
}

void FriendRosterTab::cursorToPopupTop() {
    if (testFlags(0x10)) {
        popupRow = 2;
    } else {
        popupRow = 0;
    }
    s32 a = ((PopupChoiceMenuBody *)&popup)->getRowX();
    s32 b = ((PopupChoiceMenuBody *)&popup)->getRowY(popupRow);
    U970_A->warpTo(a, b);
    U970_B->setAnimIfChanged(7);
}

void FriendRosterTab::pressCursor() {
    U970_B->setPosePress();
    setMainState(4);
}

void FriendRosterTab::releaseCursor() {
    U970_A->setPoseRelease();
    setMainState(5);
}

void FriendRosterTab::refreshCursor() {
    U970_A->setPoseIdle();
    cursor.update();
}

BOOL FriendRosterTab::moveFocus(u32 keys) {
    u32 old = A0V;
    if (keys == 0) {
        return FALSE;
    }
    if (old <= 7) {
        if (MenuKeys_HasDown(keys)) {
            if (A0V + 1 < rowCount) {
                A0V = A0V + 1;
            }
        } else if (MenuKeys_HasUp(keys)) {
            if (A0V != 0) {
                A0V = A0V - 1;
            } else {
                A0V = 0x14;
            }
        } else if (MenuKeys_HasRight(keys)) {
            s32 t = U970_A->getScreenY();
            if (t > 0x94) {
                A0V = 0x16;
            } else {
                t -= 0x35;
                if (t < 0) t = 0;
                t >>= 4;
                if (t >= 6) t = 5;
                A0V = t + 8;
            }
        }
    } else if (FRIEND_IS_TAB(old)) {
        if (MenuKeys_HasLeft(keys)) {
            if (A0V > 0xe) {
#ifdef N3DS_DIRECT
                if (A0V != FRIEND_OPT_FOCUS)  // (the options button: nothing further left)
#endif
                A0V = A0V - 1;
            }
#ifdef N3DS_DIRECT
            else {
                A0V = FRIEND_OPT_FOCUS;  // (from the pockets' tab onto the options button)
            }
#endif
        } else if (MenuKeys_HasRight(keys)) {
#ifdef N3DS_DIRECT
            if (A0V == FRIEND_OPT_FOCUS) {
                A0V = 0xe;  // (back onto the pockets' tab)
            } else
#endif
            if (A0V < 0x15) {
                A0V = A0V + 1;
            }
        } else if (MenuKeys_HasDown(keys)) {
            s32 t = U970_A->getScreenX();
            if (rowCount != 0 && t < 0x80) {
                A0V = 0;
            } else {
                A0V = 8;
            }
        }
    } else if (old >= 8 && old <= 0xd) {
        if (MenuKeys_HasUp(keys)) {
            if (A0V > 8) {
                A0V = A0V - 1;
            } else {
                A0V = 0x14;
            }
        } else if (MenuKeys_HasDown(keys)) {
            if (A0V < 0xd) {
                A0V = A0V + 1;
            } else {
                A0V = 0x16;
            }
        } else if (MenuKeys_HasLeft(keys)) {
            if (rowCount != 0) {
                s32 t = U970_A->getScreenY();
                t -= 0x30;
                if (t < 0) t = 0;
                t >>= 4;
                s32 n = rowCount;
                if (t >= n) t = n - 1;
                A0V = t;
            }
        }
    } else if (old == 0x16) {
        if (MenuKeys_HasUp(keys)) {
            A0V = 0xd;
        }
        if (MenuKeys_HasLeft(keys)) {
            u32 n = rowCount;
            if (n != 0) {
                A0V = n - 1;
            }
        }
    }
    if (old != A0V) {
        return TRUE;
    }
    return FALSE;
}

void FriendRosterTab::openRegisterChoices()
{
    registerButtonPalette = 7;
    ChoiceIdList_Clear(choiceList.msgIds, 0xa);
    CommManager *g = gCommManager;
    if (((CommManager *)(g))->isOnline()) {
        s32 skip = g->myAid;
        s32 i = 0;
        MsgString9B tmp;
        for (; i < 4; i++) {
            if (skip != i) {
                PlayerData *p = PlayerData_GetBySessionSlot(i);
                if (p) {
                    p->getPlayerId()->getNameString(&tmp);
                    ((PopupChoiceMenuBody *)(&popup))->addCustomRow(&choiceList, &tmp, (u8)(i + 6));
                }
            }
        }
    }
    ChoiceIdList_Add(choiceList.msgIds, 0xce, 5);
    ChoiceIdList_Add(choiceList.msgIds, 2, 0xa);
    openPopup(0);
}

void FriendRosterTab::openFriendChoices()
{
    ChoiceIdList_Clear(choiceList.msgIds, 0xa);
    ChoiceIdList_Add(choiceList.msgIds, 0xcf, 3);
    ChoiceIdList_Add(choiceList.msgIds, 0xd0, 2);
    FriendEntry *rec = getFocusedFriendEntry();
    if (rec) {
        if (DwcFriendData_IsNotFriendKey(FriendEntry_GetFriendData(rec)) == 0) {
            ChoiceIdList_Add(choiceList.msgIds, 0xd1, 4);
        }
    }
    ChoiceIdList_Add(choiceList.msgIds, 0xd2, 0);
    ChoiceIdList_Add(choiceList.msgIds, 2, 0xa);
    openPopup(0);
}

void FriendRosterTab::openRemoveConfirm()
{
    ChoiceIdList_Clear(choiceList.msgIds, 0xa);
    ChoiceIdList_Add(choiceList.msgIds, 0xd3, 0xa);
    ChoiceIdList_Add(choiceList.msgIds, 4, 1);
    ChoiceIdList_Add(choiceList.msgIds, 0x13, 0xa);
    openPopup(1);
    setFlags(0x10);
}

void FriendRosterTab::removeFriend()
{
    FriendEntry *rec = getFocusedFriendEntry();
    if (rec) {
        FriendEntry_Clear(rec);
        selectedRow = -1;
        setFlags(2);
        refreshPage();
        delayTimer = 1;
        setMainState(9);
        friendIndex = getFocusedFriendIndex();
    }
}

void FriendRosterTab::openFriendEditor(s32 x)
{
    MenuCtrl_SetIndex((u8)getFocusedFriendIndex());
    requestTab(x);
    hideTabBar();
}

void FriendRosterTab::useCodeEntry()
{
    s32 i = findFreeEntry();
    if (i == -1) {
        showError(0x13);
    } else {
        FriendEntry_Clear(&getFriendEntries()[i]);
        MenuCtrl_SetIndex((u8)i);
        requestTab(0xe);
        hideTabBar();
    }
}

void FriendRosterTab::registerPresentPlayer()
{
    s32 i = findFreeEntry();
    if (i == -1) {
        showError(0x13);
        return;
    }
    PlayerData *a = PlayerData_GetBySessionSlot(popupValue - 6);
    if (isInRoster(PlayerWifiData_GetOwnFriendData((u8 *)a->getWifiUserData()))) {
        showError(0x14);
        return;
    }
    FriendEntry *b = getFriendEntries();
    PlayerId *c = a->getPlayerId();
    FriendEntry *rec = &b[i];
    void *d = FriendEntry_GetFriendData(rec);
    DwcFriendData_Copy(d, PlayerWifiData_GetOwnFriendData((u8 *)a->getWifiUserData()));
    u8 *e = c->getName();
    Mem_Copy(e, FriendEntry_GetPlayerName((u8 *)rec), 8);
    u16 *f = TownId_GetName((u16 *)PlayerId_GetTownId(c));
    Mem_Copy(f, FriendEntry_GetTownName((u8 *)rec), 8);
    ((MenuTabBar *)(ProcBase_GetParent(this)))->requestSaveOnClose();
    friendIndex = i;
    if (isWifiMode()) {
        if (Net_WifiAddFriend(friendIndex, DwcFriendData_GetBytes(FriendEntry_GetFriendData(&b[friendIndex]))) == 0) {
            setMainState(0x10);
            return;
        }
    }
    showFriend(i);
}

void FriendRosterTab::showFriend(s32 x)
{
    buildFriendIndex();
    u32 t = getPageOfFriend(x);
    if (t != targetPage) {
        ((FriendRosterTab *)(this))->setPage(t);
    } else {
        refreshPage();
    }
    resumeInput();
}

BOOL FriendRosterTab::activateFocus()
{
    u32 m = focus;
#ifdef N3DS_DIRECT
    if (m == FRIEND_OPT_FOCUS) {
        return requestTab(N3DS_OPT_TAB) ? TRUE : FALSE;
    }
#endif
    if (m >= 0xe && m <= 0x15) {
        if (requestTab(m - 0xe)) {
            return TRUE;
        }
        return FALSE;
    }
    if (m >= 8 && m <= 0xd) {
        u8 k = m - 8;
        if (k != targetPage) {
            targetPage = k;
            setMainState(0xe);
            if (k == 4) {
                Snd_PlaySe(0xf);
            } else {
                Snd_PlaySe(0xb);
            }
            return TRUE;
        }
        return FALSE;
    }
    if (m == 0x16) {
        registerButtonPalette = 8;
        delayTimer = 3;
        setMainState(8);
        Snd_PlaySe(0x2b);
        return TRUE;
    }
    if (m <= 7) {
        selectedRow = m;
        setFlags(4);
        delayTimer = 3;
        setMainState(8);
        Snd_PlaySe(0x2b);
        return TRUE;
    }
    return FALSE;
}

u8 FriendRosterTab::hitTest(s32 x, s32 y)
{
    s32 i;
    s32 t = Oam_GetObjX(&data_ov119_02295648[0][1].attr01) + 0x83;
    if (t <= x && t + 0x22 >= x) {
        for (i = 0; i < 6; i++) {
            s32 u = Oam_GetObjY((s8 *)&data_ov119_02295648[i][1]) + 0x61;
            if (u <= y && u + 0x12 >= y) {
                return (u8)(i + 8);
            }
        }
    }
    if (x >= 0x18 && x <= 0xc0 && y >= 0x30 && y < 0xb0) {
        s32 r = (y - 0x30) >> 4;
        if (r < rowCount) {
            return (u8)r;
        }
    }
    if (x >= 0xc3 && x <= 0xf3 && y >= 0x9f && y <= 0xb5) {
        return 0x16;
    }
    return 0x17;
}

void FriendRosterTab::uploadScreen()
{
    updateRowMarkBlink();
    if (testFlags(4)) {
        if (page <= 3) {
            BgScreen_SetRectPalette(pageScreens, 5, 6, 0x17, 0x15, 3);
            s32 v = selectedRow;
            if (v != -1) {
                s32 y = v * 2 + 6;
                BgScreen_SetRectPalette(pageScreens, 5, y, 0x17, y + 1, 4);
            }
        }
        clearFlags(4);
        setFlags(2);
    }
    if (testFlags(2)) {
        if (vramTasks[0].requestScreen(shownScreen, 4, 0x800, 0)) {
            clearFlags(2);
        }
    }
    if (testFlags(0x20)) {
        vramTasks[1].requestPalette(bgPaletteWork, 6, 3);
        vramTasks[2].requestPalette(objPaletteWork, 8, 10);
    }
}

u16 FriendRosterTab::blendColor(s32 c1, s32 c2, s32 t, s32 n)
{
    u8 r = c2 & 0x1f;
    u8 g = (c2 & 0x3e0) >> 5;
    u8 b = (c2 & 0x7c00) >> 10;
    s32 k = n - t;
    r = DS_DIV(((u8)(c1 & 0x1f) * t + r * k), n);
    g = DS_DIV(((u8)((c1 & 0x3e0) >> 5) * t + g * k), n);
    b = DS_DIV(((u8)((c1 & 0x7c00) >> 10) * t + b * k), n);
    return r | (g << 5) | (b << 10);
}

void FriendRosterTab::setPaletteFade(s32 x)
{
    s32 v = x;
    s32 i;
    if (v < 0) {
        v = 0;
    } else if (v > 5) {
        v = 5;
    }
    setFlags(0x20);
    MIi_CpuCopy16(objPalette, objPaletteWork, 0x20);
    MIi_CpuCopy16(bgPalette, bgPaletteWork, 0x20);
    for (i = 2; i <= 5; i++) {
        bgPaletteWork[i] = blendColor(bgPalette[i], bgPalette[14], v, 5);
    }
    bgPaletteWork[15] = blendColor(bgPalette[15], bgPalette[14], v, 5);
    for (i = 1; i <= 3; i++) {
        objPaletteWork[i] = blendColor(objPalette[i], objPalette[14], v, 5);
    }
    objPaletteWork[15] = blendColor(objPaletteWork[15], objPalette[14], v, 5);
}

BOOL FriendRosterTab::isWifiMode()
{
    s32 t = Net_GetMode();
    switch (t) {
    case 3:
    case 4:
        return TRUE;
    }
    return FALSE;
}

FriendEntry *FriendRosterTab::getFriendEntries()
{
    return FriendList_GetEntries(PlayerData_GetCurrent()->getFriendList());
}

FriendEntry *FriendRosterTab::getFocusedFriendEntry()
{
    FriendEntry *b = getFriendEntries();
    s32 v = getFocusedFriendIndex();
    if (v >= 0x20) {
        return 0;
    }
    return &b[v];
}

u32 FriendRosterTab::getFocusedFriendIndex()
{
    return rosterIndices[focus + (page << 3)];
}

void FriendRosterTab::buildFriendIndex()
{
    FriendEntry *b = getFriendEntries();
    s32 i;
    s32 n = 0;
    for (i = 0; i < 0x20; i++) {
        if (DwcFriendData_IsValid(FriendEntry_GetFriendData(&b[i]))) {
            rosterIndices[n] = i;
            n++;
        }
    }
    for (; n < 0x20; n++) {
        rosterIndices[n] = 0x20;
    }
}

BOOL FriendRosterTab::hasGaps()
{
    FriendEntry *b = getFriendEntries();
    s32 flag = 0;
    s32 i;
    for (i = 0; i < 0x20; i++) {
        if (DwcFriendData_IsValid(FriendEntry_GetFriendData(&b[i])) != 0) {
            if (flag != 0) {
                return TRUE;
            }
        } else {
            flag = 1;
        }
    }
    return FALSE;
}

void FriendRosterTab::compactRoster()
{
    if (isWifiMode()) {
        buildFriendIndex();
    } else {
        FriendList_Compact(PlayerData_GetCurrent()->getFriendList());
    }
}

s32 FriendRosterTab::syncFromWifiFriendList()
{
    u8 *a = (u8 *)Net_GetWifiFriendList();
    FriendEntry *b = getFriendEntries();
    s32 i;
    s32 result = 0;
    u32 tmp[3];
    for (i = 0; i < 0x20; i++) {
        MI_CpuCopy8(a + i * 12, tmp, 12);
        if (Net_GetFriendDataType(tmp) == 0) {
            if (DwcFriendData_IsValid(FriendEntry_GetFriendData(&b[i])) == 0) {
                continue;
            }
        }
        FriendEntry *rec = &b[i];
        if (Net_IsSameFriendData(tmp, DwcFriendData_GetBytes(FriendEntry_GetFriendData(rec))) != 0) {
            s32 t = Net_IsBuddyFriendData(tmp);
            if (t == Net_IsBuddyFriendData(DwcFriendData_GetBytes(FriendEntry_GetFriendData(rec)))) {
                continue;
            }
        }
        MI_CpuCopy8(tmp, DwcFriendData_GetBytes(FriendEntry_GetFriendData(rec)), 12);
        result = 1;
    }
    return result;
}

s32 FriendRosterTab::findFreeEntry()
{
    s32 i;
    FriendEntry *p = getFriendEntries();
    for (i = 0; i < 0x20; p++, i++) {
        if (DwcFriendData_IsValid(FriendEntry_GetFriendData(p)) == 0) {
            return i;
        }
    }
    return -1;
}

BOOL FriendRosterTab::isInRoster(void *p) {
    FriendEntry *r = getFriendEntries();
    s32 i;
    for (i = 0; i < 0x20; i++) {
        if (DwcFriendData_IsValid(FriendEntry_GetFriendData(r))) {
            if (DwcFriendData_Compare(p, FriendEntry_GetFriendData(r))) {
                return TRUE;
            }
        }
        r++;
    }
    return FALSE;
}

void FriendRosterTab::drawPresentPage() {
    s32 x = 0x1ce;
    s32 i;
    s32 y = 0x14e;
    PlayerData *rec;
    s32 cur;
    LabelString *a;
    LabelString *b;
    CommManager *g;
    s32 pos = 0xc3;
    g = gCommManager;
    cur = g->myAid;
    s32 n = 0;
    MsgString9C A;
    MsgString9B B;
    for (i = 0; i < 8; i++) {
        a = allocTextLabel();
        b = allocTextLabel();
        rec = 0;
        if (((CommManager *)(g))->isOnline()) {
            if (cur == n) {
                n++;
            }
            if (n < 4) {
                rec = PlayerData_GetBySessionSlot(n);
                n++;
            }
        }
        if (rec != 0) {
            PlayerId *g2 = rec->getPlayerId();
            TownId_GetNameString(PlayerId_GetTownId(g2), &A);
            String_SetSlot(0, &A);
            String_Load2dMenu(a, 0x66);
            g2->getNameString(&B);
            b->copy(&B);
            setRowIcon(&pageScreens[pos], 0x58);
            rowMarkMask = rowMarkMask | (1 << i);
        } else {
            ((MsgString *)(a))->clear();
            ((MsgString *)(b))->clear();
            setRowIcon(&pageScreens[pos], 0x10);
        }
        ((LabelString *)(a))->createLabel(4, x, 10, 0xf, 0, 0);
        ((LabelString *)(a))->redrawAligned(0, 0);
        ((LabelString *)(b))->createLabel(4, y, 8, 0xf, 0, 0);
        ((LabelString *)(b))->redrawAligned(0, 0);
        x += 0x14;
        y += 0x10;
        pos += 0x40;
    }
    BgScreen_SetRectPalette(pageScreens, 5, 6, 0x17, 0x15, 5);
}

void FriendRosterTab::drawOwnCodePage() {
    PlayerId *g = PlayerData_GetCurrent()->getPlayerId();
    LabelString *a = allocTextLabel();
    MsgString9C A;
    TownId_GetNameString(PlayerId_GetTownId(g), &A);
    String_SetSlot(0, &A);
    String_Load2dMenu(a, 0x66);
    ((LabelString *)(a))->createLabel(4, 0x102, 10, 0xf, 0, 0);
    ((LabelString *)(a))->redrawAligned(0, 0);
    LabelString *b = allocTextLabel();
    MsgString9B B;
    g->getNameString(&B);
    b->copy(&B);
    ((LabelString *)(b))->createLabel(4, 0x116, 8, 0xf, 0, 0);
    ((LabelString *)(b))->redrawAligned(0, 0);
}

void FriendRosterTab::drawFriendCodeHelp() {
    u8 ch = gU8None;
    void *p = PlayerData_GetCurrent();
    s32 i;
    if (PlayerWifiData_HasUserId(((PlayerData *)(p))->getWifiUserData()) == 0) {
        ch = 0xd4;
    } else if (PlayerWifiData_IsConfigValid(((PlayerData *)(p))->getWifiUserData()) == 0) {
        ch = 0xec;
    } else {
        ch = 0xd5;
    }
    String_Load2d(&helpText, &ch, 0);
    for (i = 0; i < 3; i++) {
        void *o = allocTextLabel();
        ((MsgString *)(o))->setLine(Msg_SkipLines(helpText.text, i));
        ((LabelString *)(o))->createLabel(4, i * 0x28 + 0x8a, 0x14, 0xf, 0, 0);
        ((LabelString *)(o))->redrawAligned(0, 0);
    }
}

void FriendRosterTab::setRowIcon(u16 *p, u32 v) {
    p[0] = p[0] & ~0x3ff;
    p[0] = p[0] | v;
    if (v != 0x10) {
        v = (u16)(v + 1);
    }
    p[1] = p[1] & ~0x3ff;
    p[1] = p[1] | v;
    if (v != 0x10) {
        v = (u16)(v + 1);
    }
    p[0x20] = p[0x20] & ~0x3ff;
    p[0x20] = p[0x20] | v;
    if (v != 0x10) {
        v = (u16)(v + 1);
    }
    p[0x21] = p[0x21] & ~0x3ff;
    p[0x21] = p[0x21] | v;
}

void FriendRosterTab::drawRosterPage() {
    s32 i;
    FriendEntry *recs;
    LabelString *a;
    LabelString *b;
    recs = getFriendEntries();
    s32 base = page << 3;
    s32 x = 0x1ce;
    s32 y = 0x14e;
    s32 pos = 0xc3;
    MsgString9C A;
    EncodedString8B B;
    for (i = 0; i < 8; i++) {
        a = allocTextLabel();
        b = allocTextLabel();
        s32 idx = rosterIndices[base];
        FriendEntry *rec;
        if (idx < 0x20) {
            rec = &recs[idx];
        } else {
            rec = 0;
        }
        if (rec != 0 && DwcFriendData_IsValid(FriendEntry_GetFriendData(rec))) {
            EncodedString_SetRaw(&B, FriendEntry_GetTownName((u8 *)rec), 8);
            A.fromEncoded(&B, 0, 0);
            String_SetSlot(0, &A);
            String_Load2dMenu(a, 0x66);
            String_FromEncodedBytes(b, FriendEntry_GetPlayerName((u8 *)rec), 8);
            rowCount = rowCount + 1;
            if (DwcFriendData_IsNotFriendKey(FriendEntry_GetFriendData(rec))) {
                setRowIcon(&pageScreens[pos], 0x50);
            } else {
                setRowIcon(&pageScreens[pos], 0x54);
            }
        } else {
            ((MsgString *)(a))->clear();
            ((MsgString *)(b))->clear();
            setRowIcon(&pageScreens[pos], 0x10);
        }
        ((LabelString *)(a))->createLabel(4, x, 10, 0xf, 0, 0);
        ((LabelString *)(a))->redrawAligned(0, 0);
        ((LabelString *)(b))->createLabel(4, y, 8, 0xf, 0, 0);
        ((LabelString *)(b))->redrawAligned(0, 0);
        x += 0x14;
        y += 0x10;
        base++;
        pos += 0x40;
    }
}

void FriendRosterTab::refreshPage() {
    rowMarkMask = 0;
    rowMarkBlinkTimer = 0;
    clearRowMarks();
    rowCount = 0;
    u8 t = page;
    switch (t) {
    case 5:
        drawPresentPage();
        break;
    case 4:
        drawOwnCodePage();
        break;
    default:
        setFlags(4);
        drawRosterPage();
        break;
    }
}

void FriendRosterTab::setPage(u8 s) {
    if (s != page) {
        targetPage = s;
        page = s;
        u8 t = page;
        if (t == 5) {
            addTitleLabel(0xeb);
            shownScreen = pageScreens;
        } else if (t == 4) {
            addTitleLabel(0xea);
            shownScreen = &pageScreens[0x400];
        } else {
            addTitleLabel(0xcd);
            shownScreen = pageScreens;
        }
        setFlags(2);
        refreshPage();
    }
}

u32 FriendRosterTab::getPageOfFriend(s32 idx) {
    FriendEntry *base = getFriendEntries();
    s32 cnt = 0;
    s32 i;
    for (i = 0; i < 0x20; i++) {
        if (DwcFriendData_IsValid(FriendEntry_GetFriendData(&base[i]))) {
            cnt++;
            if (i >= idx) {
                i = 0x20;
            }
        }
    }
    if (cnt > 0) {
        return ((u32)(cnt - 1) << 21) >> 24;
    }
    return 0;
}

void FriendRosterTab::loadOwnFriendCode() {
    void *p = PlayerData_GetCurrent();
    s32 i;
    if ((PlayerWifiData_HasUserId(((PlayerData *)(p))->getWifiUserData()) & PlayerWifiData_IsConfigValid(((PlayerData *)(p))->getWifiUserData())) == 0) {
        for (i = 0; i < 12; i++) {
            ownCodeDigits[i] = 10;
        }
    } else {
        u64 v = PlayerWifiData_GetFriendCode(((PlayerData *)(p))->getWifiUserData());
        for (i = 11; i >= 0; i--) {
            ownCodeDigits[i] = (u8)(v % 10);
            v = v / 10;
        }
    }
}

u8 FriendRosterTab::getPopupRowValue(u32 idx) { return choiceList.values[idx]; }

void FriendRosterTab::onPopupChoice() {
    switch (popupValue) {
    case 1:
        removeFriend();
        break;
    case 0:
        openRemoveConfirm();
        break;
    case 2:
        openFriendEditor(9);
        break;
    case 3:
        openFriendEditor(0xb);
        break;
    case 4:
        openFriendEditor(0xd);
        break;
    case 5:
        useCodeEntry();
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        registerPresentPlayer();
        break;
    case 10:
    default:
        resumeInput();
        break;
    }
}

void FriendRosterTab::openPopup(u32 x) {
    ((PopupChoiceMenuBody *)(&popup))->setRowsFromIds(&choiceList, x);
    u32 r;
    if (focus == 0x16) {
        r = 0x9c;
    } else {
        r = 0xb8;
    }
    ((PopupChoiceMenu *)(&popup))->placeAbove(0x100, r);
    PopupChoice_Open(&popup, 0);
    setMainState(10);
    clearFlags(0x10);
}

void FriendRosterTab::cancelChoice() {
    popupValue = 10;
    hideCursor();
    PopupChoice_Close(&popup, 0);
    setMainState(0xc);
}

void FriendRosterTab::drawRowMarks() {
    s32 i;
    clearRowMarks();
    for (i = 0; i < 8; i++) {
        if (rowMarkMask & (1 << i)) {
            BgScreen_SetRectPalette(pageScreens, 3, i * 2 + 6, 4, i * 2 + 7, 5);
        }
    }
}

void FriendRosterTab::clearRowMarks() {
    BgScreen_SetRectPalette(pageScreens, 3, 6, 4, 0x15, 3);
    setFlags(2);
}

void FriendRosterTab::updateRowMarkBlink() {
    rowMarkBlinkTimer = rowMarkBlinkTimer + 1;
    u8 c = rowMarkBlinkTimer;
    if (c == 10) {
        drawRowMarks();
    } else if (c >= 0x19) {
        clearRowMarks();
        rowMarkBlinkTimer = 0;
    }
}

BOOL FriendRosterTab::testFlags(u32 mask) {
    if (flags & mask) {
        return TRUE;
    }
    return FALSE;
}

void FriendRosterTab::setFlags(u32 mask) { flags = flags | mask; }

void FriendRosterTab::clearFlags(u32 mask) { flags = flags & ~mask; }

extern "C" GXOamAttr data_ov119_022956a8[5] = {{0x404d00f7, 0xc4, 1, 0x4, 0x0}, {0x804240f5, 0x10b, 1, 0x4, 0x0}, {0x6280f5, 0x10f, 1, 0x4, 0x0}, {0x40424005, 0x14b, 1, 0x4, 0x0}, {0x620005, 0x14f, 1, 0x4, 0xffff}};

extern "C" GXOamAttr data_ov119_02295748[6] = {{0x40490027, 0xd0, 0, 0xb, 0x0}, {0x598027, 0xd2, 0, 0xb, 0x0}, {0x80424025, 0x10b, 1, 0xb, 0x0}, {0x628025, 0x10f, 1, 0xb, 0x0}, {0x40424035, 0x14b, 1, 0xb, 0x0}, {0x620035, 0x14f, 1, 0xb, 0xffff}};

extern "C" GXOamAttr data_ov119_02295588 = {0x419700f0, 0x1a0, 0, 0xa, 0xffff};

extern "C" GXOamAttr data_ov119_02295778[6] = {{0x80454041, 0xca, 1, 0x7, 0x0}, {0x40650041, 0xce, 1, 0x7, 0x0}, {0x8059403f, 0x102, 1, 0x7, 0x0}, {0x4059404f, 0x142, 1, 0x7, 0x0}, {0x8041403f, 0x100, 1, 0x7, 0x0}, {0x4041404f, 0x140, 1, 0x7, 0xffff}};

extern "C" GXOamAttr data_ov119_022957a8[6] = {{0x419840ed, 0x130, 1, 0x5, 0x0}, {0x402040ed, 0x130, 1, 0x5, 0x0}, {0x400040ed, 0x130, 1, 0x5, 0x0}, {0x41e840ed, 0x130, 1, 0x5, 0x0}, {0x41c840ed, 0x130, 1, 0x5, 0x0}, {0x41a840ed, 0x130, 1, 0x5, 0xffff}};

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov119_02292d00) {
    HOST_OVERLAY_VAR(data_ov119_02295648),
    HOST_OVERLAY_VAR(data_ov119_02295660),
    HOST_OVERLAY_VAR(data_ov119_022957d8),
    HOST_OVERLAY_VAR(data_ov119_02295680),
    HOST_OVERLAY_VAR(sFriendRosterTabProfile),
    HOST_OVERLAY_VAR(data_ov119_022956d0),
    HOST_OVERLAY_VAR(data_ov119_022956f8),
    HOST_OVERLAY_VAR(data_ov119_02295720),
    HOST_OVERLAY_VAR(data_ov119_022956a8),
    HOST_OVERLAY_VAR(data_ov119_02295748),
    HOST_OVERLAY_VAR(data_ov119_02295588),
    HOST_OVERLAY_VAR(data_ov119_02295778),
    HOST_OVERLAY_VAR(data_ov119_022957a8),
    HOST_OVERLAY_VARS_END
};

#include "types.h"
#include "Unk_020d8c7c.h"
#include "text/Unk_02050288.h"
#include "sys/PrioListNode.h"
#include "talk/MsgStringBase.h"
#include "gfx/BgTransfer.h"
#include "gfx/SpriteAnim.h"
#include "ui/UiWidget.h"
#include "snd/BgmVolumeMixer.h"
#include "gfx/VramTask.h"
#include "talk/LabelBalloonText.h"
#include "menu/MenuSlide.h"
#include "menu/MenuProc.h"
#include "ui/LabelBalloon.h"
#include "gfx/BgVramTask.h"
#include "snd/BgmManager.h"
#include "ui/TouchPromptBalloon.h"
#include "menu/MenuErrorMessage.h"
#include "menu/MenuTabBar.h"
#include "sys/ProcProfile.h"
#include "proto/autoload_2/unk_020e7fd4.h"
#include "proto/autoload_2/unk_020e8558.h"
#include "proto/autoload_2/unk_020ec848.h"
#include "proto/autoload_2/unk_02128030.h"
#include "proto/main/unk_0200137c.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0205b7b0.h"
#include "proto/main/unk_02063904.h"
#include "proto/main/unk_02063f3c.h"
#include "proto/main/unk_0206db88.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_02087e70.h"
#include "proto/main/unk_0209eb94.h"
#include "proto/main/unk_020b7d84.h"
#include "proto/ov090/unk_ov090_022918e0.h"
#ifdef N3DS_DIRECT
#include "n3ds_ui.h"
#include "proto/main/unk_020501d4.h"
#include "text/Unk_02050288.h"
#define N3DS_MENU_STATE() n3ds_menu_state(1, visible, slideY, curTab)
#define N3DS_IS_TAB(i) ((i) <= 6 || (i) == N3DS_OPT_TAB)  // (the options page: one more tab)
#else
#define N3DS_IS_TAB(i) ((i) <= 6)
#endif

extern "C" {

extern u16 gPad[];
}









class MenuProc;
typedef void (MenuProc::*Unk_ov002_02200a68_Fn)();


// ---------------------------------------------------------------- ov090 declarations
extern "C" {

extern u8 gFieldSceneKind;
}

class Unk_02083b0_dummy;








extern "C" BgmManager *data_021c1b3c;

class MenuTabBar;
typedef void (MenuTabBar::*Unk_ov090_022921e0_Fn)();


extern "C" {
}

extern "C" u32 sTabBarOamCells[32];

static inline BOOL Unk_ov090_02291aa0_Both() {
    if (gTouchHeld != 0 && gTouchChanged != 0) {
        return TRUE;
    }
    return FALSE;
}

extern "C" MenuTabBar *MenuTabBar_Create() { return new MenuTabBar(); }

BOOL MenuTabBar::onCreate() {
    justOpened = 1;
    MenuCtrl_SetSyncMsgMenu();
    initTabBar();
    MenuScreen_BeginOpen();
    loadedTab = 0xff;
    sTabSwitchCooldown = 0;
    saveState = 0;
    mainState = 1;
    transitionState = 0;
    setPhase(0);
    MenuCtrl_ClearSavedSlot();
    Snd_BeginMenuDuck();
    MenuCtrl_SyncFromInputMode();
#ifdef N3DS_DIRECT
    {
        GameFontDesc *fd = (GameFontDesc *)&gFontA;
        n3ds_menu_font(fd->header, fd->glyphs, fd->bitmaps);
    }
    N3DS_MENU_STATE();
#endif
    return TRUE;
}

BOOL MenuTabBar::onDelete() {
    MenuScreen_Reset();
    releaseResources();
    Snd_EndMenuDuck();
    MenuCtrl_ClearSyncMsgMenu();
#ifdef N3DS_DIRECT
    n3ds_menu_state(0, 0, 0, 0);
#endif
    return TRUE;
}

BOOL MenuTabBar::onDraw() {
    if (visible == 0) {
        return FALSE;
    }
    s32 i;
    s32 j = 0;
    i = j;
    for (; i <= 7; i++, j += 2) {
        Oam_DrawObj(1, &sTabBarOamCells[j * 2], 0x80, slideY + 0x50, -1, 2, NULL);
        Oam_DrawObj(1, &sTabBarOamCells[(j + 1) * 2], 0x80, slideY + 0x50, -1, 2, NULL);
    }
#ifdef N3DS_DIRECT
    gpu2d_opt_button(slideY - 15);  // (the tabs' tops)
#endif
    return TRUE;
}

// Declarations for data defined further down (definition order sets the data layout)
extern "C" ProcProfile sMenuTabBarProfile;
extern "C" u32 sTabBarOamCells[32];

extern "C" ProcProfile sMenuTabBarProfile = {(void *(*)())MenuTabBar_Create, 0x8f, 0x93};

extern "C" u32 sTabBarOamCells[32] = {
    0x404900a1, 0x0000f092, 0x005980a1, 0x0000f094, 0x41b300a1, 0x0000f080, 0x01c380a1, 0x0000f082, 0x41cc00a1, 0x0000f083, 0x01dc80a1, 0x0000f085, 0x41e500a1, 0x0000f086, 0x01f580a1, 0x0000f088, 0x41fe00a1, 0x0000f089, 0x000e80a1, 0x0000f08b, 0x401700a1, 0x0000f08c, 0x002780a1, 0x0000f08e, 0x403000a1, 0x0000f08f, 0x004080a1, 0x0000f091, 0x406700a1, 0x0000f095, 0x007780a1, 0xfffff097
};

BOOL MenuTabBar::execTransition() {
    static Unk_ov090_022921e0_Fn tbl[3] = {&MenuTabBar::stateLoad, &MenuTabBar::stateSlideIn,
                                           &MenuTabBar::stateSlideOut};
    cancelVramTasks();
    (this->*tbl[transitionState])();
#ifdef N3DS_DIRECT
    N3DS_MENU_STATE();
#endif
    return TRUE;
}

BOOL MenuTabBar::execMain() {
    static Unk_ov090_022921e0_Fn tbl[3] = {&MenuTabBar::updateIdle, &MenuTabBar::updateOpenTabMenu,
                                           &MenuTabBar::updateSaving};
    cancelVramTasks();
    if (saveState == 2) {
        if (Save_WritePlayerFriendList()) {
            u8 c;
            mainState = 2;
            c = 0x1f;
            errorMessage.open(&c, 1, 1);
            saveState = 3;
        } else {
            saveState = 0;
            MenuScreen_BeginClose();
        }
    }
    if (sTabSwitchCooldown != 0) {
        sTabSwitchCooldown = sTabSwitchCooldown - 1;
    }
    (this->*tbl[mainState])();
    if (tabsShown != 0) {
        if (slideY != 0x10) {
            if (slideY >= 0xe) {
                slideY = 0x10;
            } else {
                slideY = slideY + 2;
            }
        }
    } else if (slideY != 0) {
        if (slideY <= 4) {
            visible = 0;
            slideY = 0;
        } else {
            slideY = slideY - 4;
        }
    }
    if (lrSwitchEnabled != 0) {
        if ((gPad[1] & 0x100) != 0) {
            showTab(MenuTabBar_NextTab(curTab));
            Snd_PlaySe(3);
        }
        if ((gPad[1] & 0x200) != 0) {
            showTab(MenuTabBar_PrevTab(curTab));
            Snd_PlaySe(3);
        }
    }
#ifdef N3DS_DIRECT
    N3DS_MENU_STATE();
    {
        // leaving the options page as a tab's menu does: selectTab at once, onTabMenuClosed once it has slid out; the
        // forced close counted down as the tabs' menus do (their handleTabSwitch)
        int se, closed, tab, forced = 0;
        if (curTab == N3DS_OPT_TAB) {
            MenuCtrl_TickForceClose();
            forced = MenuCtrl_IsForceCloseDue();
        }
        tab = n3ds_menu_step(forced, &se, &closed);
        if (se >= 0) Snd_PlaySe(se);
        if (tab == N3DS_MENU_PREV) tab = MenuTabBar_PrevTab(N3DS_OPT_TAB);
        if (tab == N3DS_MENU_NEXT) tab = MenuTabBar_NextTab(N3DS_OPT_TAB);
        if (tab >= 0) selectTab((u32)tab);
        if (closed) onTabMenuClosed();
    }
#endif
    return TRUE;
}

BOOL MenuTabBar::execPhase3() { return TRUE; }

BOOL MenuTabBar::execPhase4() { return TRUE; }

BOOL MenuTabBar::execClosed() {
    ProcBase_RequestDelete(this);
    return TRUE;
}

void MenuTabBar::selectTab(u32 idx) {
    justOpened = 0;
    u32 old = curTab;
    showTab(idx);
    if (idx == 7) {
        tabsShown = 0;
        if (saveState == 1) {
            saveState = 2;
        }
        if (saveState == 0) {
            MenuScreen_BeginClose();
        }
        Snd_PlaySe(2);
        data_021c1b3c->mixer.endMenuDuck();
    } else {
        if (N3DS_IS_TAB(idx) && N3DS_IS_TAB(old)) {
            Snd_PlaySe(3);
        }
    }
    if (idx == 0) {
        MenuCtrl_ClearSavedSlot();
    }
    if (N3DS_IS_TAB(old) && N3DS_IS_TAB(idx)) {
        lrSwitchEnabled = 1;
    }
}

u8 MenuTabBar::onTabMenuClosed() {
    switch (curTab) {
    case 7:
        if (saveState == 0) {
            MenuScreen_ReleaseCloseHold();
            beginClose();
        }
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
#ifdef N3DS_DIRECT
    case N3DS_OPT_TAB:
#endif
        mainState = 1;
        break;
    }
    return curTab;
}

void MenuTabBar::beginClose() {
    transitionState = 2;
    setPhase(1);
    Gfx2d_SetSubBgModeState(1);
}

void MenuTabBar::updateIdle() {}

void MenuTabBar::updateOpenTabMenu() {
    if (sTabSwitchCooldown == 0) {
        lrSwitchEnabled = 0;
        switch (curTab) {
        case 0: {
            BOOL r = FALSE;
            if (gFieldSceneKind == 0) {
                r = TRUE;
            }
            if (r) {
                MenuCtrl_RequestOpenNested(2);
            } else {
                MenuCtrl_RequestOpenNested(1);
            }
            break;
        }
        case 1:
            MenuCtrl_RequestOpenNested(3);
            break;
        case 2:
            MenuCtrl_RequestOpenNested(4);
            break;
        case 3:
            MenuCtrl_RequestOpenNested(5);
            break;
        case 4:
            MenuCtrl_RequestOpenNested(6);
            break;
        case 5:
            MenuCtrl_RequestOpenNested(7);
            break;
        case 8:
            MenuCtrl_RequestOpenNested(8);
            break;
        case 6:
            MenuCtrl_RequestOpenNested(0x24);
            break;
        case 9:
        case 10:
        case 11:
        case 12:
            MenuCtrl_RequestOpenNested(0xf);
            MenuCtrl_SetMode(curTab + 0xf);
            break;
        case 13:
        case 14:
            MenuCtrl_RequestOpenNested(0x2b);
            MenuCtrl_SetMode(curTab);
            break;
#ifdef N3DS_DIRECT
        case N3DS_OPT_TAB:
            n3ds_menu_page();
            break;
#endif
        case 7:
            break;
        }
        mainState = 0;
    }
}

void MenuTabBar::updateSaving() { errorMessage.update(0); }

void MenuTabBar::stateLoad() {
    Gfx2d_LoadPaletteFile("menu/tag/obj.bpl", gCurrentHeap, 8, 0xf, 0xf, 0xf);
    File_LoadToBuffer("menu/tag/obj1.bch", commonObjChars, 0x800);
    File_LoadToBuffer("menu/tag/obj2.bch", tabObjChars, 0x800);
    showTab(MenuCtrl_GetMode());
    visible = 1;
    slideY = 0;
    transitionState = 1;
    Gfx2d_SetSubBgModeState(0);
}

void MenuTabBar::stateSlideIn() {
    tabsShown = 1;
    setPhase(2);
}

void MenuTabBar::stateSlideOut() {
    if (slideY <= 4) {
        visible = 0;
        setPhase(0);
    } else {
        slideY = slideY - 4;
    }
}

void MenuTabBar::initTabBar() {
    visible = 0;
    slideY = 0;
    tabsShown = 0;
    lrSwitchEnabled = 0;
    Snd_PlaySe(1);
    data_021c1b3c->mixer.setMenuDuck(0);
}

void MenuTabBar::releaseResources() { cancelVramTasks(); }

extern "C" s32 MenuTabBar_HitTestTouch() {
    if (!Unk_ov090_02291aa0_Both()) {
        return -1;
    }
    s32 v = gTouchCurX;
    s32 lim = gTouchCurY;
    if (lim > 0x10) {
        return -1;
    }
#ifdef N3DS_DIRECT
    if (v >= 26 && v < 50) {  // the options page's button
        return N3DS_OPT_TAB;
    }
#endif
    if (v < 0x35) {
        return -1;
    }
    if (v >= 0x100) {
        return -1;
    }
    s32 r = (v - 0x35) / 0x19;
    if (r >= 7) {
        r = 7;
    }
    return r;
}

void MenuTabBar::showTabs() {
    tabsShown = 1;
    visible = 1;
}

void MenuTabBar::hideTabs() { tabsShown = 0; }

extern "C" s32 MenuTabBar_GetTabX(s32 a) {
#ifdef N3DS_DIRECT
    if (a == N3DS_OPT_TAB) {
        return 0x26;  // (the options button's middle, x 26-49: the tabs' are their left + 12, as 0x3f)
    }
#endif
    if (a == 7) {
        return 0xf3;
    }
    return a * 0x19 + 0x3f;
}

// (the direct build: the options page between the friends and the pockets)
extern "C" u8 MenuTabBar_PrevTab(u8 a) {
#ifdef N3DS_DIRECT
    if (a == N3DS_OPT_TAB) {
        sTabSwitchCooldown = 10;
        return 6;
    }
    if (a == 0) {
        sTabSwitchCooldown = 10;
        return N3DS_OPT_TAB;
    }
#endif
    if (a <= 6) {
        sTabSwitchCooldown = 10;
        if (a == 0) {
            return 6;
        }
        return a - 1;
    }
    return a;
}

extern "C" u8 MenuTabBar_NextTab(u8 a) {
    sTabSwitchCooldown = 10;
#ifdef N3DS_DIRECT
    if (a == 6) {
        return N3DS_OPT_TAB;
    }
    if (a == N3DS_OPT_TAB) {
        return 0;
    }
#endif
    if (a <= 6) {
        if (a == 6) {
            return 0;
        }
        return a + 1;
    }
    return a;
}

void MenuTabBar::cancelVramTasks() {
    s32 i;
    for (i = 0; i < 3; i++) {
        vramTasks[i].cancel();
    }
}

void MenuTabBar::showTab(u32 idx) {
    curTab = idx;
    if (idx <= 7) {
        if (idx != loadedTab) {
            char buf[0x24];
            vramTasks[0].requestChars(commonObjChars, 8, 0x80, 0x80, 0xbf);
            u8 *a = (u8 *)tabObjChars + idx * 0x60;  // (bytes: the tab's 3 tiles of a row; the next row 0x400 on)
            u32 t = idx * 3 + 0x80;
            vramTasks[1].requestCharPair(a, a + 0x400, 8, t, t + 2, t + 0x20, t + 0x22);
            Str_SPrintf(buf, "menu/tag/obj%d.bpl", idx);
            File_LoadToBuffer(buf, tabPalette, 0x20);
            vramTasks[2].requestPalette(tabPalette, 8, 0xf);
            loadedTab = idx;
        }
    }
#ifdef N3DS_DIRECT
    // the options page: every tab of the game's unlit (the host lights its button): their tiles and the base palette
    // (a lit tab's obj%d.bpl changes palette 15, which all the tabs use)
    if (idx == N3DS_OPT_TAB && idx != loadedTab) {
        vramTasks[0].requestChars(commonObjChars, 8, 0x80, 0x80, 0xbf);
        File_LoadToBuffer("menu/tag/obj.bpl", tabPalette, 0x20);
        vramTasks[2].requestPalette(tabPalette, 8, 0xf);
        loadedTab = idx;
    }
#endif
}

void MenuTabBar::requestSaveOnClose() { saveState = 1; }

extern "C" s32 MenuTabBar_TabFromX(s32 a) {
    s32 r = (a - 0x33) / 0x19;
    if (r < 0) {
        r = 0;
    }
    if (r > 7) {
        r = 7;
    }
    return r;
}

// ---------------------------------------------------------------- MenuTabBar

BOOL MenuTabBar::isJustOpened() {
    if (justOpened != 0) {
        return TRUE;
    }
    return FALSE;
}
extern "C" {
u8 sTabSwitchCooldown;
}

// ---- overlay variables (generated: tools/host_overlay_vars.py) ----
#include "host_overlay_vars.h"

HOST_OVERLAY_VARS(unk_ov090_022918e0) {
    HOST_OVERLAY_VAR(sMenuTabBarProfile),
    HOST_OVERLAY_VAR(sTabBarOamCells),
    HOST_OVERLAY_VAR(sTabSwitchCooldown),
    HOST_OVERLAY_VARS_END
};

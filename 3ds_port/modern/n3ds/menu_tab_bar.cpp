// The enhanced port's options page in the pause menu, moved out of src/ov090/unk_ov090_022918e0.cpp (game code: the
// game's flags; modern/n3ds/ is built by tools/configure_host.py for the 3DS port only).
#ifdef N3DS_PORT
#include "types.h"
#include "n3ds_ui.h"
#include "menu/MenuTabBar.h"
#include "menu/PocketMenu.h"
#include "menu/Keyboard.h"
#include "proto/ov002/unk_ov002_02200840.h"
#include "proto/ov095/unk_02292360.h"
#include "proto/main/unk_020039ec.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/ov090/unk_ov090_022918e0.h"

// (MenuTabBar::execMain)
void N3ds_MenuTabBarStep(MenuTabBar *s) {
    // leaving the options page as a tab's menu does: selectTab at once, onTabMenuClosed once it has slid out; the
    // forced close counted down as the tabs' menus do (their handleTabSwitch)
    int se, closed, tab, forced = 0;
    if (s->curTab == N3DS_OPT_TAB) {
        MenuCtrl_TickForceClose();
        forced = MenuCtrl_IsForceCloseDue();
    }
    tab = n3ds_menu_step(forced, &se, &closed);
    if (se >= 0) Snd_PlaySe(se);
    if (tab == N3DS_MENU_PREV) tab = MenuTabBar_PrevTab(N3DS_OPT_TAB);
    if (tab == N3DS_MENU_NEXT) tab = MenuTabBar_NextTab(N3DS_OPT_TAB);
    if (tab >= 0) s->selectTab((u32)tab);
    if (closed) s->onTabMenuClosed();
}

// (src/ov096/unk_02294c40.cpp PocketMenu::moveCursorInTabs) TRUE if the cursor was on the options button or went onto it
// (the game's moving then is not to run)
extern "C" BOOL N3ds_PocketMoveOptButton(PocketMenu *s, u32 pad) {
    // the options button: Left from the pockets' tab, Right back to it, Down as from the pockets' tab
    if (s->cursorTarget == POCKET_OPT_TARGET) {
        if (MenuKeys_HasDown(pad)) {
            s->cursorTarget = 0x25;
            s->m_2498.switchToAnim01();
        } else if (MenuKeys_HasRight(pad)) {
            s->cursorTarget = 0x19;
        }
        return TRUE;
    }
    if (s->cursorTarget == 0x19 && MenuKeys_HasLeft(pad) && !MenuKeys_HasDown(pad)) {
        s->cursorTarget = POCKET_OPT_TARGET;
        return TRUE;
    }
    return FALSE;
}

// (src/ov095/unk_02292360.cpp Keyboard_MoveOnChatKeys) TRUE if the cursor was on the options button or went onto it
extern "C" BOOL N3ds_KeyboardMoveOptKey(Keyboard *s, u32 p) {
    // the options button: Left from the pockets' tab key, Right back to it, Down as from it
    if (s->cursorKey == N3DS_KB_OPT_KEY) {
        if (MenuKeys_HasDown(p) != 0) {
            Keyboard_SetFlags(s, 0x10);
        } else if (MenuKeys_HasRight(p) != 0) {
            s->cursorKey = 0xcd;
        }
        return TRUE;
    }
    if (s->cursorKey == 0xcd && MenuKeys_HasLeft(p) != 0 && MenuKeys_HasDown(p) == 0) {
        s->cursorKey = N3DS_KB_OPT_KEY;
        return TRUE;
    }
    return FALSE;
}
#endif

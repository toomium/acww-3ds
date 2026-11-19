// "New Leaf controls" (include/port_options.h): the tools swapped with the D-pad's left and right, as AC:WW+'s L / R
// (github.com/migueliranzo/acww-plus, MIT). Game code (the game's flags, modern/port/ is built for the console ports
// only). While the local player stands and the pockets could open:
//   right: the next tool from the pockets in hand (the one in hand takes its pocket)
//   left:  the tool in hand put away into a free pocket; with empty hands the last one taken out again
// The equip is the pocket menu's own request (PlayerActor_RequestChangeHeldItem: its animation, its sounds).
#ifdef ENHANCED_PORT
#include "types.h"
#include "port_options.h"
#include "host_input.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "proto/main/unk_02097444.h"
#include "proto/main/unk_0203d4d8.h"
#include "proto/main/unk_0206e54c.h"
#include "proto/main/unk_0209c390.h"

extern "C" {
s32 Scene_GetCurrent();
s32 PlayerActor_RequestChangeHeldItem(u16 *p);
void Pocket_SetItem(u16 *a, s32 b, s32 c);
}

enum { EMPTY = 0xfff1 };
static int is_tool(u16 v) { return v >= 0x1369 && v <= 0x137b; }  // the shovel .. the golden slingshot
static int is_toy(u16 v) { return v >= 0x13a0 && v <= 0x13a7; }   // gone once put away (PocketMenu::swapEquipped)

static u8 sNext;       // where right starts looking
static s8 sPut = -1;   // the pocket left put the tool into last
static u8 sField;      // an equip from here under way: its end (action 5, the pockets' "item chosen" pose) goes back
                       // to standing, as there is no pocket menu to close (Unk_02007694::mainAct05)

extern "C" int Port_ToolSwapEquipping(void) { return sField; }
extern "C" void Port_ToolSwapEquipped(void) { sField = 0; }

extern "C" unsigned port_dpad_lr_held(void) { return port_new_leaf_controls() ? gHostDpad & 0x30 : 0; }

// pocket k's item in hand, the hand's into pocket k; 1 if the game took the request
static int equip(PlayerInventory *inv, s32 k) {
    u16 item = *inv->getPocket(k), old = *PlayerData_GetCurrent()->getHeldItem();
    if (!PlayerActor_RequestChangeHeldItem(&item)) return 0;
    Pocket_SetItem(&old, 0, k);
    return sField = 1;
}
static int free_tool(PlayerInventory *inv, s32 k) { return is_tool(*inv->getPocket(k)) && inv->getPocketFlags(k) == 0; }

extern "C" int Port_ToolSwapTick(void) {
    sField = 0;  // (standing again)
    if (!port_new_leaf_controls()) return 0;
    u32 key = host_dpad_tap();
    // (as the pockets open from the field: MenuManager::onExecute)
    if (!key || Scene_GetCurrent() == 6 || gSoftResetRequested || TalkRequest_IsActive() || !MenuCtrl_IsIdle()) return 0;
    PlayerInventory *inv = (PlayerInventory *)PlayerData_GetCurrent()->getInventory();
    u16 hand = *PlayerData_GetCurrent()->getHeldItem();
    if (is_toy(hand)) return 0;
    if (key & 0x10) {  // right: the next tool, from the pocket after the one taken last
        for (s32 i = 0; i < 15; i++) {
            s32 k = (sNext + i) % 15;
            if (!free_tool(inv, k)) continue;
            if (!equip(inv, k)) return 0;
            sNext = (u8)((k + 1) % 15), sPut = -1;
            return 1;
        }
        return 0;
    }
    if (hand != EMPTY) {  // left: put away (pockets full: kept)
        s32 k = inv->findEmptyPocket();
        if (k < 0) return 0;
        u16 none = EMPTY;
        if (!PlayerActor_RequestChangeHeldItem(&none)) return 0;
        Pocket_SetItem(&hand, 0, k);
        sPut = (s8)k;
        return sField = 1;
    }
    // left with empty hands: the tool put away last taken out again, else the first tool
    if (sPut >= 0 && free_tool(inv, sPut) && equip(inv, sPut)) return sPut = -1, 1;
    for (s32 k = 0; k < 15; k++)
        if (free_tool(inv, k)) return equip(inv, k);
    return 0;
}
#endif

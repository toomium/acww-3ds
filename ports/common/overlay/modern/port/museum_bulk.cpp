// "Several items for Blathers" (include/port_options.h), from AC:WW+ (github.com/migueliranzo/acww-plus, MIT). Game
// code (the game's flags, modern/port/ is built for the console ports only).
//
// His "Donation!" and "Check a fossil!" open the pocket picker (ov101 PocketItemSelectMenu) for one item; after his
// answer he asks for another and opens it again. While it is open X marks pockets (the grid's own ring, its marked
// bits); each time he would open it again, the next marked pocket is handed to him instead, as if picked. A marked
// pocket whose item changed since is passed over. Cancelling the picker, or a new conversation, drops the marks.
#ifdef ENHANCED_PORT
#include "types.h"
#include "port_options.h"
#include "proto/ov094/unk_ov094_02292da8.h"

extern "C" {
u16 Pocket_GetItem(s32 i);
void MenuCtrl_SetIndex(u32 v);
void MenuCtrl_SetResult(u32 v);
}

static u16 sMarks;      // the marked pockets (bit = slot)
static u16 sItems[15];  // what each held when marked
static u8 sOpen;        // his picker is open

extern "C" int Port_MuseumPickerOpen(void) { return sOpen && port_museum_bulk(); }

extern "C" void Port_MuseumMark(int slot, unsigned *bits) {
    if (sMarks & 1 << slot) {
        sMarks &= ~(1 << slot);
        InventoryItemGrid_ClearBit(bits, slot);
    } else {
        sMarks |= 1 << slot;
        sItems[slot] = Pocket_GetItem(slot);
        InventoryItemGrid_SetBit(bits, slot);
    }
}

extern "C" void Port_MuseumPicked(int slot, unsigned *bits) {
    if (!(sMarks & 1 << slot)) return;
    sMarks &= ~(1 << slot);
    InventoryItemGrid_ClearBit(bits, slot);
}

extern "C" int Port_MuseumNext(void) {
    for (s32 slot = 0; slot < 15 && sMarks; slot++) {
        if (!(sMarks & 1 << slot)) continue;
        sMarks &= ~(1 << slot);
        if (Pocket_GetItem(slot) == sItems[slot]) {
            MenuCtrl_SetIndex(slot);
            MenuCtrl_SetResult(1);  // (MenuCtrl_IsResultOk)
            return 1;
        }
    }
    sOpen = 1;
    return 0;
}

extern "C" void Port_MuseumOpened(int ok) {
    sOpen = 0;
    if (!ok) sMarks = 0;
}

extern "C" void Port_MuseumReset(void) { sMarks = 0, sOpen = 0; }
#endif

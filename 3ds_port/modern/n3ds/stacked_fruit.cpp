// The enhanced port's stacked fruit (include/n3ds_options.h): what the game's files call for it, moved out of them. Game
// code (the game's flags, modern/n3ds/ is built by tools/configure_host.py for the 3DS port only).
#ifdef N3DS_PORT
#include "types.h"
#include "n3ds_options.h"
#include "menu/InventoryItemGrid.h"
#include "menu/PocketMenu.h"
#include "net/CommManager.h"
#include "player/PlayerData.h"
#include "player/PlayerInventory.h"
#include "proto/main/unk_02097444.h"
#include "proto/ov096/unk_02294c40.h"

extern "C" {
void MI_CpuCopy8(const void *src, void *dst, u32 n);  // SDK assembly
extern void *gCommManager;
}

// (src/ov094/unk_ov094_02292da8.cpp InventoryItemGrid_LoadSlotIcon, InventoryItemGrid_SetSpriteItem)
// Stacked fruit (n3ds_options.h): a stack's icon with its count in the bottom right corner, a light 3x5 digit in a dark
// frame. The icons' palettes (menu/icon/b_obj_itm.bpl, OBJ palettes 7-14) all have their dark outline at 0xf and a
// light grey at 0xd (palette 7: white at 9). src as the icon pages hold it: the top tiles at src, the bottom at +0x400.
static u8 sStackIcon[0x440];
extern "C" u8 *InventoryItemGrid_StackIcon(u8 *src, u32 item, u32 pal)
{
    // the digits' rows (3 bits each, the left pixel the top bit), from the top
    static const u8 digits[10][5] = {{7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
                                     {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 2, 2}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}};
    s32 n = n3ds_fruit_count(item);
    if (n < 2) return src;
    MI_CpuCopy8(src, sStackIcon, 0x40);
    MI_CpuCopy8(src + 0x400, sStackIcon + 0x400, 0x40);
    const u8 *d = digits[n % 10];
    u32 light = pal == 7 ? 9 : 0xd;
    for (s32 y = 9; y < 16; y++) {      // the digit at x 12-14, y 10-14; its frame one pixel around it
        for (s32 x = 11; x < 16; x++) {
            BOOL on = x >= 12 && x <= 14 && y >= 10 && y <= 14 && (d[y - 10] >> (14 - x) & 1);
            u32 c = on ? light : 0xf;
            u8 *b = sStackIcon + (y >> 3) * 0x400 + (x >> 3) * 0x20 + (y & 7) * 4 + (x & 7) / 2;
            *b = (x & 1) ? (u8)((*b & 0x0f) | c << 4) : (u8)((*b & 0xf0) | c);
        }
    }
    return sStackIcon;
}

// (n3ds_options.h; src/main/unk_02098e90.cpp Pocket_AddFoundItem)
extern "C" int Pocket_FindFruitStack(u16 *a) {
    PlayerInventory *inv = (PlayerInventory *)PlayerData_GetCurrent()->getInventory();
    s32 f = n3ds_fruit_kind(*a);
    if (!n3ds_stack_fruit() || n3ds_fruit_count(*a) != 1 || ((CommManager *)(gCommManager))->isOnline()) return -1;
    for (s32 i = 0; i < 15; i++) {
        u16 v = *inv->getPocket(i);
        if (n3ds_fruit_kind(v) == f && n3ds_fruit_count(v) < N3DS_FRUIT_MAX && inv->getPocketFlags(i) == 0) return i;
    }
    return -1;
}

// (src/ov096/unk_02294c40.cpp)
// Stacked fruit (n3ds_options.h): a stack dropped outside leaves one fruit (requestDropItem); once it is down, what stays
// of the stack goes back into its pocket target t. FALSE if v was no stack (the game's own clearing then).
extern "C" BOOL PocketMenu_KeepFruitRest(PocketMenu *s, u32 t, u32 v)
{
    if (n3ds_fruit_count(v) < 2) return FALSE;
    PocketMenu_SetSlotItem(s, t, n3ds_fruit_rest(v), 0);
    return TRUE;
}

// The pocket menu's "Take One" on a fruit stack (addItemOptions; one action more than the game's 0-0x21, 0x22 being
// "Never Mind"): one fruit flies into an empty pocket, the rest stays (as actionTakeAttachment does with a letter's)
extern "C" void PocketMenu_TakeOneFruit(PocketMenu *s)
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

// (src/ov096/unk_02294c40.cpp PocketMenu::mergeItems) TRUE if the two made a stack (the game's merging is then not to run)
extern "C" BOOL N3ds_MergeFruit(u16 *a, u16 *b)
{
    // stacked fruit (n3ds_options.h): a fruit or stack onto one of its kind makes a stack, the rest (over the most)
    // stays where the slot's was
    if (n3ds_stack_fruit() && !((CommManager *)(gCommManager))->isOnline() && n3ds_fruit_kind(*a) >= 0 &&
        n3ds_fruit_kind(*a) == n3ds_fruit_kind(*b)) {
        s32 n = n3ds_fruit_count(*a) + n3ds_fruit_count(*b), k = n3ds_fruit_kind(*a);
        if (n3ds_fruit_count(*b) < N3DS_FRUIT_MAX && n3ds_fruit_count(*a) < N3DS_FRUIT_MAX) {
            *a = n3ds_fruit_make(k, n > N3DS_FRUIT_MAX ? N3DS_FRUIT_MAX : n);
            *b = n3ds_fruit_make(k, n > N3DS_FRUIT_MAX ? n - N3DS_FRUIT_MAX : 0);
            return TRUE;
        }
    }
    return FALSE;
}
#endif

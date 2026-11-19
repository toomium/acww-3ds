#ifndef PORT_OPTIONS_H
#define PORT_OPTIONS_H

// The enhanced 3DS port's enhancements as the game code sees them (ENHANCED_PORT): each a
// row of the options page (modern/host/options_cfg.c) and what the game needs for it. Self-contained (no includes).
#ifdef ENHANCED_PORT

#ifdef __cplusplus
extern "C" {
#endif

// stacked fruit switched on: a fruit picked up or dragged onto one of its kind joins it (the stacks there keep working
// when it is off)
int port_stack_fruit(void);
// "Main screen on top": the town and the rooms kept on the top screen; the pause menu opens and closes on the bottom
// screen alone (src/main/unk_0206db88.cpp)
int port_main_top(void);
// the pause menu opening or closing that way (src/main/unk_0206db88.cpp): taken when it starts opening
int port_menu_on_sub(void);
// the pocket a fruit (*item, as picked up) joins: a stack or single fruit of its kind with room, not flagged (or a Bell
// bag: the bag it adds up with, "Bell bags stack"); -1 none,
// or stacking off, or online (src/main/unk_02098e90.cpp)
int Pocket_FindFruitStack(unsigned short *item);
// a stack's icon (the icon pages' src: the top tiles at src, the bottom at +0x400) with its count drawn in; src itself
// for anything else (modern/port/stacked_fruit.cpp; src/ov094/unk_ov094_02292da8.cpp)
unsigned char *InventoryItemGrid_StackIcon(unsigned char *src, unsigned item, unsigned pal);
// the name tables are the EUR ROM's (ACWW_LANG, modern/ds/port_lang.c): a name's record has a third byte before the
// name, its grammatical form (0xff none), and the furniture's records are 0x20 bytes instead of 0x1c
int Port_LangEur(void);
// "New Leaf controls" switched on: Y picks up and pulls weeds (B on the DS), B only runs, X opens the pockets (Y on the
// DS) and the map has no key (its touch button stays)
int port_new_leaf_controls(void);
// the wide view's ranges (src/ov003, ov009, ov068: the trees, buildings and villagers out to the picture's sides) widened:
// the wide view on and ACWW_WIDE_RANGES not 0 (plat_3ds.c sets 0 on an Old 3DS: they cost it 2-4 fps)
int port_wide_ranges(void);
static inline unsigned port_key_pick_up(void) { return port_new_leaf_controls() ? 0x800 : 0x2; }   // Y : B
static inline unsigned port_key_pockets(void) { return port_new_leaf_controls() ? 0x400 : 0x800; } // X : Y

// The enhancements taken from AC:WW+ (github.com/migueliranzo/acww-plus, MIT), each an option, off by default.
// "Monkeys move in": Champ, Nana, Simon, Tammi, Monty and Elise (species 0x90-0x95, the only ones of the never chosen
// move class 2) may be a new town's residents or move in, as class 0 (src/main/unk_02077ac4.cpp)
int port_monkeys(void);
static inline unsigned port_move_class(unsigned species, unsigned c) {
    return c == 2 && species >= 0x90 && species <= 0x95 && port_monkeys() ? 0 : c;
}
// "Nook's without multiplayer": Nookington's without a visitor's purchase, and the sale days, evening sales and
// spotlight items with one resident (src/main/unk_020ac750.cpp)
int port_nook_solo(void);
// "Shake trees with tools": A with the fishing rod, or the net when no bug is near, shakes the tree ahead
// (src/ov003/unk_ov003_02204ce8.cpp)
int port_tool_shake(void);
// "Bell bags stack": a Bell bag picked up joins a pocket's bag when the two make a bag (Pocket_FindFruitStack)
int port_bell_stack(void);
static inline int port_bag_bells(unsigned v) {  // a Bell bag's Bells (0: no bag): 100-900, then 1,000-99,000
    return v < 0x1492 || v > 0x14fd ? 0 : v - 0x1492 < 9 ? (int)(v - 0x1492 + 1) * 100 : (int)(v - 0x1492 - 8) * 1000;
}
static inline unsigned short port_bag_of(int bells) {  // the bag of these Bells (0: there is none)
    return (unsigned short)(bells > 0 && bells <= 900 && bells % 100 == 0 ? 0x1492 + bells / 100 - 1 :
                            bells >= 1000 && bells <= 99000 && bells % 1000 == 0 ? 0x1492 + 8 + bells / 1000 : 0);
}
// "Several items for Blathers": in his item picker X marks pockets; each time he would open it again the next marked
// one is handed to him (modern/port/museum_bulk.cpp; src/ov047, src/ov101)
int port_museum_bulk(void);
int Port_MuseumPickerOpen(void);                // his picker is open: X marks (ov101 PocketItemSelectMenu::mainAct06)
void Port_MuseumMark(int slot, unsigned *bits);  // X on pocket slot: marked or unmarked (bits: the grid's ring marks)
void Port_MuseumPicked(int slot, unsigned *bits);  // A on slot: no mark any more (the game takes it)
int Port_MuseumNext(void);    // he would open the picker: 1 with the next marked slot as the menu's result instead, else
                              // 0 (the picker opens, X marks in it)
void Port_MuseumOpened(int ok);  // what he got from the picker (ok: an item picked; else the marks dropped)
void Port_MuseumReset(void);                    // a new conversation: the marks dropped
// "New Leaf controls" also swap tools with the D-pad's left and right (modern/port/tool_swap.cpp)
int Port_ToolSwapTick(void);          // the local player standing (PlayerActor::waitCheckInput): 1 if a swap started
int Port_ToolSwapEquipping(void);     // a swap's equip ends (action 5): back to standing, then Port_ToolSwapEquipped
void Port_ToolSwapEquipped(void);
unsigned port_dpad_lr_held(void);     // the D-pad's left / right held while they are the tools': not for walking

#ifdef __cplusplus
}
#endif

// Stacked fruit: 2 to PORT_FRUIT_MAX of a fruit (0x1518 + f, f 0-4) as one item, 0x1570 + f * 8 + n - 2. No DS item
// has these codes (the item tables end at 0x156d); their info records are the fruit's (port_item_record), the price
// the fruit's times n (Item_GetPrice). A stack leaves the pockets one fruit at a time where it is dropped, buried or
// given to an NPC (Pocket_GetItem / Pocket_RemoveItem, a villager's errand); Nook's sell menu takes it whole at that
// price; the pocket menu shows the count in the icon and has "Take One"; the other ways a stack
// leaves the pockets are not handled yet.
#define PORT_FRUIT_MAX 9
static inline int port_fruit_kind(unsigned v) {  // 0-4 for a fruit or a stack of it, else -1
    if (v >= 0x1518 && v <= 0x151c) return (int)(v - 0x1518);
    if (v >= 0x1570 && v < 0x1570 + 5 * 8) return (int)(v - 0x1570) >> 3;
    return -1;
}
static inline int port_fruit_count(unsigned v) {  // how many fruit the item is (0: no fruit)
    if (v >= 0x1518 && v <= 0x151c) return 1;
    if (v >= 0x1570 && v < 0x1570 + 5 * 8) return (int)((v - 0x1570) & 7) + 2;
    return 0;
}
static inline unsigned short port_fruit_make(int f, int n) {  // n of fruit f (0: no item)
    return (unsigned short)(n <= 0 ? 0xfff1 : n == 1 ? 0x1518 + f : 0x1570 + f * 8 + n - 2);
}
static inline unsigned short port_fruit_one(unsigned v) {  // what leaves a stack: one fruit (other items unchanged)
    return port_fruit_count(v) > 1 ? port_fruit_make(port_fruit_kind(v), 1) : (unsigned short)v;
}
static inline unsigned short port_fruit_rest(unsigned v) {  // what stays of a stack once one left (0xfff1 otherwise)
    return port_fruit_count(v) > 1 ? port_fruit_make(port_fruit_kind(v), port_fruit_count(v) - 1) : 0xfff1;
}
static inline unsigned port_item_record(unsigned i) {  // an item info index (code & 0xfff): a stack's is its fruit's
    return i >= 0x570 && i < 0x570 + 5 * 8 ? 0x518 + ((i - 0x570) >> 3) : i;
}

#endif
#endif

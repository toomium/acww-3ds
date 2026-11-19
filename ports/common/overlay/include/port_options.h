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
// the pocket a fruit (*item, as picked up) joins: a stack or single fruit of its kind with room, not flagged; -1 none,
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

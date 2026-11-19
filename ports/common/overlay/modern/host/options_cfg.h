#ifndef OPTIONS_CFG_H
#define OPTIONS_CFG_H

// The enhanced 3DS port's options page: the table of settings, acww.cfg's rewriting and
// what the game's pause menu says (include/port_ui.h). Everything here is ENHANCED_PORT only; the body is empty otherwise.
#ifdef ENHANCED_PORT
#include <stdint.h>

enum { OPT_APPLY_LIVE = 1 };  // opt_set applies at once (setenv + apply function); else the value waits for a restart

typedef struct {
    const char *group, *label, *name;  // group: the page's tab; name: the ACWW_* variable
    const char *desc;                  // what it does, under the rows (the page wraps it)
    int def, live;                     // default (0 / 1); live: changes at once
    void (*apply)(int);
    int count, first;                  // a choice (count > 0): the values first .. first + count - 1, shown as
    const char *const *values;         // values[v - first]; else (0) Off / On
} Option;

int opt_count(void);
const Option *opt_info(int i);
int opt_get(int i);
void opt_set(int i, int v);
int opt_save(void);       // writes the changed options into acww.cfg; 0 if it worked
void opt_store(const char *name, int v);  // option `name` set (if the page has it) and acww.cfg written: the Switch's L3
void opt_frame(void);     // once a frame, on the game's thread: the test hooks, the page's picture for the host
uint32_t opt_keys(uint32_t pressed);       // the keys the game gets (bit set = pressed), once a frame
void opt_touch(int *down, int *x, int *y);  // the touch the game gets (down cleared if the page used it)

// the pause menu's state, as the game last said it (port_menu_state, port_menu_font)
typedef struct {
    int open, visible, slideY, curTab;
    const void *fontHeader, *fontGlyphs, *fontBitmaps;
} OptMenu;

#endif
#endif

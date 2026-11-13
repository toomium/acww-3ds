#ifndef OPTIONS_3DS_H
#define OPTIONS_3DS_H

// The enhanced 3DS port's options page: the table of settings, acww.cfg's rewriting and
// what the game's pause menu says (include/n3ds_ui.h). Everything here is N3DS_PORT only; the body is empty otherwise.
#ifdef N3DS_PORT
#include <stdint.h>

enum { OPT_APPLY_LIVE = 1 };  // opt_set applies at once (setenv + apply function); else the value waits for a restart

typedef struct {
    const char *group, *label, *name;  // name: the ACWW_* variable
    int def, live;                     // default (0 / 1); live: changes at once
    void (*apply)(int);
} Option;

int opt_count(void);
const Option *opt_info(int i);
int opt_get(int i);
void opt_set(int i, int v);
int opt_save(void);       // writes the changed options into acww.cfg; 0 if it worked
void opt_frame(void);     // once a frame, on the game's thread: the test hooks, the page's picture for the host
uint32_t opt_keys(uint32_t pressed);       // the keys the game gets (bit set = pressed), once a frame
void opt_touch(int *down, int *x, int *y);  // the touch the game gets (down cleared if the page used it)

// the pause menu's state, as the game last said it (n3ds_menu_state, n3ds_menu_font)
typedef struct {
    int open, visible, slideY, curTab;
    const void *fontHeader, *fontGlyphs, *fontBitmaps;
} OptMenu;

#endif
#endif

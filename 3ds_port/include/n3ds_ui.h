#ifndef N3DS_UI_H
#define N3DS_UI_H

// The enhanced 3DS port (N3DS_PORT): the UI at the wide screens' edges. The game says
// which OBJs, BGs and windows make a UI element and where it belongs; the host draws the same pixels moved by the
// margins' width (modern/host/gpu2d/gpu2d.h, pica_display.c). Self-contained (no includes).
#ifdef N3DS_PORT

#ifdef __cplusplus
extern "C" {
#endif

// an anchor: the edges (as gpu2d.h's GPU2D_*), or N3DS_UI_NEAR(id) (id 0-7): towards the edges the element (its OBJs
// with this anchor) is in the outer third of, as the host finds each frame
enum { N3DS_UI_LEFT = 1, N3DS_UI_RIGHT = 2, N3DS_UI_TOP = 4, N3DS_UI_BOTTOM = 8 };
#define N3DS_UI_NEAR(id) (0x10 | (id) << 5)

// the OAM entries drawn into mode's buffer (Oam_UseBufferA) since Oam_Count gave `from`: anchored
// (src/main/unk_02087e70.cpp; given to the host with the buffers at the V-blank)
int Oam_Count(int mode);
void Oam_Anchor(int mode, int from, unsigned anchor);
// BGs (bit n: BGn) and windows (bit w: window w) of an engine (0 main, 1 sub) anchored from the next frame on, until
// changed (bgs 0: none)
void gpu2d_bg_anchor(int engine, int bgs, int windows, int anchor);
// the menus' wipe (src/main/unk_0206db88.cpp): its position for the next frame, the host drawing window 1's zigzag
// over the whole wide screen instead of the game's H-blank (gpu2d.h)
void gpu2d_menu_wipe(int pos);
// the text box's BG shown (TalkFrame_ShowBg / HideBg, src/main/unk_02065e88.cpp): the choices go with it
extern int gN3dsTalkBgShown;
// the pause menu (MenuTabBar, src/ov090/unk_ov090_022918e0.cpp) for the options page (modern/host/options_3ds.c): one
// more tab there, N3DS_OPT_TAB, left of the pockets (L from the pockets, R from the friends, its button touched), a tab
// with no menu of the game's: the host draws the page
#define N3DS_OPT_TAB 15
// the options button as the chat keyboard's key in the tab row (src/ov095, left of its tab keys 0xcd-0xd4; 0xc9-0xe1
// are its special keys) and that key's code (src/ov111 ChatMenu::pressKeyCode; 0x100-0x123 are taken)
#define N3DS_KB_OPT_KEY 0xe2
#define N3DS_KB_OPT_CODE 0x1f0
// the options button as a hand target in the tab row of the pocket menu, left of the pockets' tab (0x26 and 0x27 are
// taken): a tab target whose tab is N3DS_OPT_TAB (src/ov096)
#define POCKET_OPT_TARGET 0x28
void n3ds_menu_state(int open, int visible, int slideY, int curTab);
void n3ds_menu_font(const void *header, const void *glyphs, const void *bitmaps);  // gFontA's three pointers
void n3ds_menu_page(void);  // the options tab's turn to open (the tab before it gone): the page slides in
// once a game frame, from execMain: the page's animation; forced: the game's forced close is due (MenuCtrl_IsForceCloseDue,
// ticked while the page is the tab); returns the tab the page leaves for (-1 none: 7 closes the menu; N3DS_MENU_PREV /
// N3DS_MENU_NEXT: L / R, the game's MenuTabBar_PrevTab / NextTab give the tab and their pause), *closed 1 once it has
// slid out (as a tab's menu deleted), *se a sound to play (-1 none: 0x3b cursor, 0x27 button)
#define N3DS_MENU_PREV (-2)
#define N3DS_MENU_NEXT (-3)
int n3ds_menu_step(int forced, int *se, int *closed);
// the options page's button drawn with the tab bar (MenuTabBar::onDraw) at line y of the bottom screen: as an OBJ, shown
// with the OAM the game loads next (gpu2d_obj_anchors), cut at the DS's picture as the tabs
void gpu2d_opt_button(int y);

#ifdef __cplusplus
}
#endif

#endif
#endif

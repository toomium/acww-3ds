#ifdef ENHANCED_PORT
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "host.h"
#include "host_mem.h"
#include "port_ui.h"
#include "options_cfg.h"

#ifdef __3DS__
#define CFG "sdmc:/3ds/acww/acww.cfg"
#elif defined(__SWITCH__)
#define CFG "sdmc:/switch/acww/acww.cfg"  // (plat_switch.c reads it there)
#else
#define CFG "acww.cfg"
#endif

void g3p_set_interp(int on);  // gpu3d_pica.c
static void apply_interp(int v) { g3p_set_interp(v); }

#ifdef __SWITCH__
// The Switch: no "Main screen on top" (its one-screen layout shows the world large wherever the game puts it; the
// option kept the touch screen's panel up and took its touches), and the display's settings: the layout of the two
// screens (gl_display.c, L3 the next: kept here too) and how many times as large they are drawn (after a restart)
void gld_set_layout(int layout);  // gl_display.c
static void apply_layout(int v) { gld_set_layout(v); }
static const char *const kLayouts[] = {"One screen", "Side by side", "Stacked", "Bottom big", "Top big", "Bottom only", "Top only"};
static const char *const kScales[] = {"1x", "2x", "3x", "4x"};
static Option sOpts[] = {
    {"Game", "60 fps", "ACWW_PICA_INTERP", 1, 1, apply_interp, 0, 0, 0},
    {"Game", "Wide screen", "ACWW_WIDE", 1, 0, 0, 0, 0, 0},  // read every frame by gpu3d_wide_wanted: only after a restart
    {"Game", "Screen layout", "ACWW_LAYOUT", 0, 1, apply_layout, 7, 0, kLayouts},
    {"Game", "Resolution", "ACWW_SWITCH_SCALE", 3, 0, 0, 4, 1, kScales},  // (gl_display.c gld_scale: at the start)
    {"Enhancements", "Stacked fruit", "ACWW_STACK_FRUIT", 0, 1, 0, 0, 0, 0},  // read by the game (port_stack_fruit)
    {"Enhancements", "New Leaf controls", "ACWW_NL_CONTROLS", 0, 1, 0, 0, 0, 0},  // read by the game (port_new_leaf_controls)
};
#else
static Option sOpts[] = {
    {"Game", "60 fps", "ACWW_PICA_INTERP", 1, 1, apply_interp, 0, 0, 0},
    {"Game", "Wide screen", "ACWW_WIDE", 1, 0, 0, 0, 0, 0},  // read every frame by gpu3d_wide_wanted: only after a restart
    {"Game", "Main screen on top", "ACWW_MAIN_TOP", 0, 1, 0, 0, 0, 0},  // read every frame by gpu2d's top_engine
    {"Enhancements", "Stacked fruit", "ACWW_STACK_FRUIT", 0, 1, 0, 0, 0, 0},  // read by the game (port_stack_fruit)
    {"Enhancements", "New Leaf controls", "ACWW_NL_CONTROLS", 0, 1, 0, 0, 0, 0},  // read by the game (port_new_leaf_controls)
};
#endif
#define NOPT ((int)(sizeof sOpts / sizeof *sOpts))

static int sVal[NOPT], sChanged[NOPT], sInit;

static void init(void) {
    if (sInit) return;
    sInit = 1;
    for (int i = 0; i < NOPT; i++) {
        const char *e = getenv(sOpts[i].name);
        const Option *o = &sOpts[i];
        sVal[i] = !e ? o->def : !o->count ? atoi(e) != 0 : atoi(e) < o->first || atoi(e) >= o->first + o->count ? o->def : atoi(e);
    }
}
// the row of option `name`, -1 if the page hasn't it (the Switch: no "Main screen on top")
static int row_of(const char *name) {
    for (int i = 0; i < NOPT; i++)
        if (!strcmp(sOpts[i].name, name)) return i;
    return -1;
}

int opt_count(void) { return NOPT; }
const Option *opt_info(int i) { return &sOpts[i]; }
int opt_get(int i) { init(); return sVal[i]; }

// (include/port_options.h) the game's view of the enhancements' rows, found by their names
int port_stack_fruit(void) {
    static int row = -2;
    if (row == -2) row = row_of("ACWW_STACK_FRUIT");
    return row >= 0 && opt_get(row);
}
// (gpu2d.c, src/main/unk_0206db88.cpp) the town and the rooms kept on the top screen, the pause menu opening without
// the screens changing places
int port_main_top(void) {
    static int row = -2;
    if (row == -2) row = row_of("ACWW_MAIN_TOP");
    return row >= 0 && opt_get(row);
}
int port_new_leaf_controls(void) {
    static int row = -2;
    if (row == -2) row = row_of("ACWW_NL_CONTROLS");
    return row >= 0 && opt_get(row);
}
int gpu3d_wide_wanted(void);  // (gpu3d/gpu3d_geom.c: ACWW_WIDE)
int port_wide_ranges(void) {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("ACWW_WIDE_RANGES");
        on = gpu3d_wide_wanted() && (!e || atoi(e));
    }
    return on;
}

void opt_set(int i, int v) {
    init();
    const Option *o = &sOpts[i];
    v = !o->count ? v != 0 : v < o->first ? o->first + o->count - 1 : v >= o->first + o->count ? o->first : v;  // (a choice: round)
    if (v == sVal[i]) return;
    sVal[i] = v;
    sChanged[i] = 1;
    if (sOpts[i].live) {
        char s[12];
        snprintf(s, sizeof s, "%d", v);
        setenv(sOpts[i].name, s, 1);
        if (sOpts[i].apply) sOpts[i].apply(v);
    }
}

void opt_store(const char *name, int v) {
    int i = row_of(name);
    if (i < 0) return;
    opt_set(i, v);
    opt_save();
}

// The cfg file with the changed options' lines replaced (the first `NAME=` line) or appended; every other byte kept.
int opt_save(void) {
    init();
    char *in = 0;
    long n = 0;
    FILE *f = fopen(CFG, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        n = ftell(f);
        rewind(f);
        in = malloc((size_t)n + 1);
        if (!in || fread(in, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(in); return 1; }
        fclose(f);
    }
    FILE *o = fopen(CFG ".tmp", "wb");
    if (!o) { free(in); return 1; }
    int done[NOPT] = {0};
    for (long p = 0; p < n;) {
        long e = p;
        while (e < n && in[e] != '\n') e++;
        int hit = -1;
        for (int i = 0; i < NOPT; i++) {
            size_t l = strlen(sOpts[i].name);
            if (sChanged[i] && !done[i] && (size_t)(e - p) > l && !memcmp(in + p, sOpts[i].name, l) && in[p + (long)l] == '=')
                hit = i;
        }
        if (hit >= 0) {
            done[hit] = 1;
            fprintf(o, "%s=%d", sOpts[hit].name, sVal[hit]);
            if (e > p && in[e - 1] == '\r') fputc('\r', o);
            if (e < n) fputc('\n', o);
        } else {
            fwrite(in + p, 1, (size_t)(e - p) + (e < n), o);
        }
        p = e + 1;
    }
    if (n && in[n - 1] != '\n') fputc('\n', o);  // (so an appended line starts on its own)
    for (int i = 0; i < NOPT; i++)
        if (sChanged[i] && !done[i]) fprintf(o, "%s=%d\n", sOpts[i].name, sVal[i]);
    free(in);
    if (fclose(o)) return 1;
    // shortcut: rename over an existing file is unverified on sdmc, so move the old one aside (host_plat_init undoes a crash here)
    remove(CFG ".bak");
    rename(CFG, CFG ".bak");
    if (rename(CFG ".tmp", CFG)) { rename(CFG ".bak", CFG); return 1; }
    remove(CFG ".bak");
    memset(sChanged, 0, sizeof sChanged);
    return 0;
}

// --- text in the game's font (gFontA, the pointers the pause menu gave: port_menu_font)
typedef struct { uint32_t count; uint16_t cellW, cellH; } FontHeader;
typedef struct { uint16_t code; uint8_t width, pad; } FontGlyph;
static OptMenu sMenu;
#define RGBA(r, g, b) ((uint32_t)(r) << 24 | (uint32_t)(g) << 16 | (uint32_t)(b) << 8 | 255u)
enum { PW = 256, PH = 192 };

// s at (x, y) (px NULL: only measured), the top left of its 16x16 cells, in a picture PW wide and rows high; returns the pen's x after it (a
// glyph's width and one pixel between letters, as the game's letterSpacing)
static int text(uint32_t *px, int rows, int x, int y, const char *s, uint32_t colour) {
    const FontHeader *h = sMenu.fontHeader;
    const FontGlyph *g = sMenu.fontGlyphs;
    const uint8_t *bm = sMenu.fontBitmaps;
    if (!h) return x;
    int rowBytes = h->cellW >> 3, cell = h->cellW * h->cellH >> 3;
    for (; *s; s++) {
        uint32_t i = 0;
        while (i < h->count && g[i].code != (uint8_t)*s) i++;
        if (i == h->count) continue;
        const uint8_t *b = bm + (size_t)cell * i;
        for (int r = 0; r < h->cellH; r++)
            for (int c = 0; c < g[i].width; c++)
                if (px && b[r * rowBytes + c / 8] >> (7 - c % 8) & 1 && x + c >= 0 && x + c < PW && y + r >= 0 && y + r < rows)
                    px[(y + r) * PW + x + c] = colour;
        x += g[i].width + 1;
    }
    return x;
}
static void fill(uint32_t *px, int x0, int y0, int x1, int y1, uint32_t c) {
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++) px[y * PW + x] = c;
}

// ACWW_OPT_TEXT=file.ppm: the words the page uses, written the first time the menu opens (to check the letters)
static void text_test(void) {
    static int done;
    const char *name = getenv("ACWW_OPT_TEXT");
    if (done || !name || !sMenu.fontHeader) return;
    done = 1;
    static uint32_t px[PW * 96];
    fill(px, 0, 0, PW, 96, RGBA(255, 255, 255));
    static const char *const words[] = {"Options", "Game", "60 fps", "Wide screen", "On", "Off"};
    for (int i = 0; i < 6; i++) text(px, 96, 4, 16 * i, words[i], RGBA(0, 0, 0));
    FILE *f = fopen(name, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d 96\n255\n", PW);
    for (int i = 0; i < PW * 96; i++) fputc(px[i] >> 24, f), fputc(px[i] >> 16 & 255, f), fputc(px[i] >> 8 & 255, f);
    fclose(f);
}

// --- the page and the pause menu's button, one picture (gpu2d_options): the button at (0, 0), 24x16, the page in
// lines 17-191
void gpu2d_options(const uint32_t *pic, uint32_t serial, int pageY);  // gpu2d.c
// The page is the pause menu's tab PORT_OPT_TAB (include/port_ui.h). It slides in from the bottom and out again as the
// tabs' menus do (MenuSlide, measured in the PC build: in 192 * p * p over p = 3/4, 2/4, 1/4, 0, a step a game frame;
// out 64 more a step, 3 steps), and takes input only while it is in place.
enum { PAGE_NONE, PAGE_IN, PAGE_OPEN, PAGE_OUT };
static int sPhase, sStep, sCursor, sSound = -1;
static uint32_t sPic[2][PW * PH], sSerial;
static int page_y(void) {  // the page's offset downwards (PH: not shown)
    return sPhase == PAGE_IN ? 12 * sStep * sStep : sPhase == PAGE_OUT ? 64 * sStep : sPhase == PAGE_OPEN ? 0 : PH;
}

// a rectangle with rounded corners (radius r)
static void rfill(uint32_t *px, int x0, int y0, int x1, int y1, int r, uint32_t c) {
    for (int y = y0; y < y1; y++) {
        int k = y - y0 < r ? y - y0 : y1 - 1 - y < r ? y1 - 1 - y : r, in = 0;
        if (k < r) {  // the corner's circle: the line's inset
            float dy = (float)(r - k) - 0.5f;
            in = r - (int)sqrtf((float)(r * r) - dy * dy);
        }
        fill(px, x0 + in, y, x1 - in, y + 1, c);
    }
}

// The button: a tab as the game's (their OBJs in the PC build): 24x16, a 1-pixel outline with rounded corners, the
// symbol in the outline's colour on a fill; unlit a dark outline on the page's slate blue-grey, lit (the page is the
// tab shown) a lighter outline on the cream every lit tab has
static void draw_button(uint32_t *px, int lit) {
    static const char *const shape[4] = {  // the top lines (the bottom ones mirrored): # outline, o fill (the pockets' tab)
        "..####################..", ".##oooooooooooooooooo##.", "##oooooooooooooooooooo##", "#oooooooooooooooooooooo#",
    };
    static const uint16_t gear[10] = {0x0f0, 0x6f6, 0x7fe, 0x39c, 0xf0f, 0xf0f, 0x39c, 0x7fe, 0x6f6, 0x0f0};  // 12x10
    uint32_t line = lit ? RGBA(136, 158, 196) : RGBA(48, 60, 92), body = lit ? RGBA(251, 251, 219) : RGBA(112, 134, 170);
    for (int y = 0; y < 16; y++) {
        const char *s = shape[y < 3 ? y : y > 12 ? 15 - y : 3];
        for (int x = 0; x < 24; x++)
            if (s[x] != '.') px[y * PW + x] = s[x] == '#' ? line : body;
    }
    for (int r = 0; r < 10; r++)  // (rows 3-12, as the game's symbols)
        for (int c = 0; c < 12; c++)
            if (gear[r] >> (11 - c) & 1) px[(3 + r) * PW + 6 + c] = line;
}

// The menus' hand (HandCursor) with its tip at the top left of (x, y)'s cell: its resting cell, 16x16 from tile 8 of
// the sub engine's OBJ VRAM (2D mapping), palette 0, over its shadow, the same tiles in palette 1 six lines lower. The
// tiles are ab_all's (AbAllObjGfx::uploadChars, OBJ 0-0x1000), the same in every pause menu tab (PC build, 2026-11-07).
// shortcut: the resting frame only (the game's hand bobs), and shown in touch mode too; the game's anim if it looks dead
static void draw_hand(uint32_t *px, int x, int y) {
    const uint8_t *chr = HostVram_CpuPtr(0x06600000);
    const uint16_t *pal = (const uint16_t *)(HW_PLTT + 0x600);  // (sub OBJ palettes)
    for (int layer = 1; layer >= 0; layer--)
        for (int r = 0; r < 16; r++)
            for (int c = 0; c < 16; c++) {
                int t = 8 + (r >> 3) * 32 + (c >> 3), v = chr[t * 32 + (r & 7) * 4 + (c & 7) / 2] >> (c & 1) * 4 & 15;
                int px_ = x + c, py = y + r + 6 * layer;
                if (!v || px_ < 0 || px_ >= PW || py < 0 || py >= PH) continue;
                unsigned k = pal[layer * 16 + v], R = k & 31, G = k >> 5 & 31, B = k >> 10 & 31;
                px[py * PW + px_] = RGBA(R << 3 | R >> 2, G << 3 | G >> 2, B << 3 | B >> 2);  // (as gpu2d.c's sprites)
            }
}

// the rows' step: 18 pixels, 16 when the rows and their groups' headings wouldn't fit above "* after a restart" (164)
static int row_step(void) {
    int n = 0;
    const char *group = "";
    for (int i = 0; i < NOPT; i++, n++)
        if (strcmp(group, sOpts[i].group)) group = sOpts[i].group, n++;
    return 46 + n * 18 <= 164 ? 18 : 46 + n * 16 <= 164 ? 16 : 15;
}
// the "* after a restart" line: under the rows (at 164, or right below them when they reach further: the Switch's)
static int note_y(void) {
    int n = 0;
    const char *group = "";
    for (int i = 0; i < NOPT; i++, n++)
        if (strcmp(group, sOpts[i].group)) group = sOpts[i].group, n++;
    return 46 + n * row_step() <= 164 ? 164 : 46 + n * row_step();
}

static void draw_picture(uint32_t *px) {
    memset(px, 0, PW * PH * 4);
    draw_button(px, sMenu.curTab == PORT_OPT_TAB);
    // the page as the other tabs' panels: a light edge, a coloured band (slate blue-grey, its own), the title in a label,
    // a pale inner panel
    const uint32_t ink = RGBA(52, 62, 86), head = RGBA(88, 112, 160), pale = RGBA(236, 241, 247);
    rfill(px, 3, 18, 253, 190, 12, RGBA(222, 229, 240));
    rfill(px, 6, 21, 250, 187, 10, RGBA(112, 134, 170));
    rfill(px, 36, 24, 190, 40, 8, RGBA(255, 255, 255));
    rfill(px, 10, 43, 246, 183, 6, pale);
    text(px, PH, 113 - text(0, 0, 0, 0, "Options", 0) / 2, 24, "Options", ink);
    int y = 46, handY = -1, step = row_step();
    const char *group = "";
    int restart = 0;
    for (int i = 0; i < NOPT; i++) {
        if (strcmp(group, sOpts[i].group)) {
            group = sOpts[i].group;
            text(px, PH, 16, y, group, head);
            y += step;
        }
        if (i == sCursor) handY = y;
        int x = text(px, PH, 22, y, sOpts[i].label, ink);
        if (!sOpts[i].live) text(px, PH, x, y, " *", head), restart = 1;
        const char *v = sOpts[i].count ? sOpts[i].values[sVal[i] - sOpts[i].first] : sVal[i] ? "On" : "Off";
        text(px, PH, 226 - text(0, 0, 0, 0, v, 0), y, v, ink);  // (room right of it for the hand)
        y += step;
    }
    if (restart) text(px, PH, 16, note_y(), "* after a restart", head);
    // the cursor: the hand on the row's value, as the friends' tab points at a name (only while the page is the tab: the
    // picture is drawn at the start too, before the menus' tiles are loaded)
    if (handY >= 0 && sMenu.curTab == PORT_OPT_TAB) draw_hand(px, 224, handY - 4);
}

static int any_changed(void) {
    for (int i = 0; i < NOPT; i++)
        if (sChanged[i]) return 1;
    return 0;
}
static int sRequest = -1;
// the page leaves for tab t (7 closes the pause menu, PORT_MENU_PREV / NEXT: L / R): the game selects it at once and
// takes the page as closed once it has slid out (port_menu_step); the changed options go into acww.cfg
static void page_leave(int t) {
    if (any_changed()) opt_save();
    sPhase = PAGE_OUT, sStep = 0, sRequest = t;
}

// --- input: while the page is there (sliding or in place) the game gets no keys and no touch; what the page took stays
// hidden from the game until it is released. The way into the page is the game's (L / R, its button: MenuTabBar).
enum { K_A = 1, K_B = 2, K_SELECT = 4, K_RIGHT = 16, K_LEFT = 32, K_UP = 64, K_DOWN = 128, K_R = 256, K_L = 512, K_X = 1024, K_Y = 2048 };
enum { BTN_X = 26, BTN_W = 24 };

static int row_y(int i) {  // the top of option i's row in the page's picture (as draw_picture lays it out)
    int y = 46, step = row_step();
    const char *group = "";
    for (int k = 0;; k++) {
        if (strcmp(group, sOpts[k].group)) group = sOpts[k].group, y += step;
        if (k == i) return y;
        y += step;
    }
}
// (a choice: the next value, or with Left the one before)
static void page_toggle(int i, int back) {
    opt_set(i, sOpts[i].count ? sVal[i] + (back ? -1 : 1) : !sVal[i]);
    sSound = 0x27;
}

// The pressed keys (bit set = pressed; KEYINPUT's 0-9, X 10, Y 11) as the game gets them. Once a frame.
uint32_t opt_keys(uint32_t pressed) {
    static uint32_t prev, hide;
    uint32_t now = pressed & ~prev;
    prev = pressed;
    hide &= pressed;
    if (sPhase == PAGE_OPEN) {
        if ((now & K_UP) && sCursor > 0) sCursor--, sSound = 0x3b;
        if ((now & K_DOWN) && sCursor < NOPT - 1) sCursor++, sSound = 0x3b;
        if (now & (K_A | K_LEFT | K_RIGHT)) page_toggle(sCursor, (now & K_LEFT) && !(now & (K_A | K_RIGHT)));
        // as the tabs' menus: B closes the pause menu, L / R go on to the friends / the pockets, Y to the pockets, X the
        // map, Select the chat (their handleTabSwitch)
        if (now & K_B) page_leave(7);
        else if (now & K_L) page_leave(PORT_MENU_PREV);
        else if (now & K_R) page_leave(PORT_MENU_NEXT);
        else if (now & K_Y) page_leave(0);
        else if (now & K_X) page_leave(5);
        else if (now & K_SELECT) page_leave(4);
    }
    if (sPhase != PAGE_NONE) hide |= pressed;
    return pressed & ~hide;
}

int gpu2d_main_top_forced(void);  // gpu2d.c: engine A on the top screen by the option, not by the game
// The touch (down, x, y in the DS's pixels) as the game gets it. Once a frame (the first call of a frame does the work).
void opt_touch(int *down, int *x, int *y) {
    static unsigned long long frame = ~0ull;
    static int prevDown, hide;
    if (frame != host_frame()) {
        frame = host_frame();
        int d = *down, tx = *x, ty = *y, newly = d && !prevDown, tabRow = ty <= 0x10;
        prevDown = d;
        if (!d) hide = 0;
        if (sPhase == PAGE_OPEN && newly) {
            if (tabRow && tx >= 0x35) {  // another tab (MenuTabBar_HitTestTouch's)
                int t = (tx - 0x35) / 0x19;
                page_leave(t > 7 ? 7 : t);
            } else if (!tabRow) {
                for (int i = 0; i < NOPT; i++)
                    if (tx >= 10 && tx < 246 && ty >= row_y(i) - 1 && ty < row_y(i) + row_step() - 1) sCursor = i, page_toggle(i, 0);
            }
        }
        // (the tab row left of the pockets' tab but for the button: nothing, but the game would switch to its touch mode)
        if (d && (sPhase != PAGE_NONE || (newly && sMenu.open && tabRow && tx < 0x35 && !(tx >= BTN_X && tx < BTN_X + BTN_W))))
            hide = 1;
    }
    // the world moved to the top screen by "Main screen on top": the touch screen shows the other engine's picture (the
    // sky), where the game would take a touch as one on the world, so the game gets none
    if (hide || gpu2d_main_top_forced()) *down = 0;
}

// once a frame: the picture redrawn when what it shows changed, and what the host draws given to gpu2d
static void page_frame(void) {
    init();
    static unsigned long long sig = ~0ull;
    unsigned long long now = (unsigned long long)(sMenu.curTab == PORT_OPT_TAB) | (unsigned long long)(sMenu.fontHeader != 0) << 1 |
                             (unsigned long long)sCursor << 2;
    for (int i = 0; i < NOPT; i++) now |= (unsigned long long)sVal[i] << (8 + 4 * i);  // (4 bits a row: the choices' values)
    if (now != sig) {
        sig = now;
        draw_picture(sPic[++sSerial & 1]);
    }
    gpu2d_options(sPic[sSerial & 1], sSerial, sMenu.open && sMenu.visible ? page_y() : PH);
}

// ACWW_OPT_AT="frame:NAME=value;...": at those frames opt_set + opt_save as the page does
static void at_hook(void) {
    static const char *sAt;
    static int sRead;
    if (!sRead) { sRead = 1; sAt = getenv("ACWW_OPT_AT"); }
    if (!sAt) return;
    unsigned long long f = host_frame();
    for (const char *p = sAt; *p;) {
        char *end;
        unsigned long long at = strtoull(p, &end, 10);
        const char *semi = strchr(end, ';');
        if (*end == ':' && at == f)
            for (int i = 0; i < NOPT; i++) {
                size_t l = strlen(sOpts[i].name);
                if (!strncmp(end + 1, sOpts[i].name, l) && end[1 + l] == '=') {
                    opt_set(i, atoi(end + 2 + l));
                    opt_save();
                }
            }
        p = semi ? semi + 1 : p + strlen(p);
    }
}

void opt_frame(void) {
    if (!sMenu.open && sPhase != PAGE_NONE) {  // (the pause menu gone with the page there)
        if (any_changed()) opt_save();
        sPhase = PAGE_NONE, sRequest = -1;
    }
    at_hook();
    text_test();
    page_frame();
}

// --- what the game says (include/port_ui.h) ---

void port_menu_state(int open, int visible, int slideY, int curTab) {
    sMenu.open = open, sMenu.visible = visible, sMenu.slideY = slideY, sMenu.curTab = curTab;
}
void port_menu_font(const void *header, const void *glyphs, const void *bitmaps) {
    sMenu.fontHeader = header, sMenu.fontGlyphs = glyphs, sMenu.fontBitmaps = bitmaps;
}
void port_menu_page(void) { sPhase = PAGE_IN, sStep = 4; }
int port_menu_step(int forced, int *se, int *closed) {
    if (forced && sPhase == PAGE_OPEN) page_leave(7);  // (the tabs' menus too only where they take input)
    int t = sRequest;
    sRequest = -1;
    *closed = 0;
    if (sPhase == PAGE_IN && --sStep == 0) sPhase = PAGE_OPEN;
    else if (sPhase == PAGE_OUT && ++sStep == 3) sPhase = PAGE_NONE, *closed = 1;
    *se = sSound;
    sSound = -1;
    return t;
}
#endif

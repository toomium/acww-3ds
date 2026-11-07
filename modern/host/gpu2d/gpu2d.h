// The project's own renderer for the DS's two 2D display engines (docs/portability.md 5.1). Plain C, no host or
// emulator dependencies: it is given the display registers as they are written, and at each frame the VRAM banks,
// the palettes, OAM and the 3D engine's output, and renders both screens.
//
// Written from the hardware's documented behaviour (GBATEK, "DS Video"): BG modes 0-6 (text BGs in 16 and 256
// colours with extended palettes, affine BGs, extended affine BGs with 16-bit map entries and 256-colour and
// direct-colour bitmaps, the large bitmap of mode 6), the 3D layer as BG0 of engine A, OBJs (normal, affine,
// double size, 1D and 2D mapping of tiles and bitmaps, extended OBJ palettes, semi-transparent OBJs, the OBJ window),
// the windows, colour special effects (alpha blending, brightness up and down), mosaic, the display modes (off,
// normal, VRAM display; main memory display is reported, not shown) and master brightness. Display capture is not
// implemented (the game doesn't use it: a capture request is reported once).
//
// The registers are kept per engine as the 0x70 bytes from 0x04000000 (engine A) and 0x04001000 (engine B), plus
// POWCNT1 (0x04000304) and VRAMCNT (0x04000240-0x04000249); other addresses are ignored. The frame is rendered line
// by line, with an H-blank callback between lines (Gpu2dFrameInput.hblank) for the game's per-line effects.

#ifndef GPU2D_H
#define GPU2D_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Gpu2dFrameInput.layers (the 3DS: its GPU composites, pica_display.c): instead of compositing, each line's layers
// and its state. The BGs the GPU can draw from VRAM itself (Gpu2dLineState.gpuBg: text BGs in the standard palette,
// affine BGs, no mosaic) are left to it; the others' and the OBJs' pixels come in the 3DS GPU's formats (256 x 192
// each, row by row) with the windows' layer masks applied. Pixels of a layer the line doesn't show are left as they
// were.
typedef struct Gpu2dLineState {
    uint32_t dispcnt;        // DISPCNT (bits 3 3D, 8-12 BG0-3 and OBJ shown, 13-15 windows)
    uint8_t gpuBg;           // bit n: BG n is drawn by the GPU from VRAM (not in Gpu2dLayers.bg)
    uint8_t bgKind[4];       // 1 text, 2 affine (for the BGs in gpuBg)
    uint16_t bgcnt[4], bghofs[4], bgvofs[4];  // BGxCNT, BGxHOFS, BGxVOFS
    int16_t affine[2][4];    // BG2, BG3: PA, PB, PC, PD
    uint16_t winin, winout;  // WININ, WINOUT (with DISPCNT bits 13-15: Gpu2dLayers.region)
    uint16_t bldcnt, bldalpha, bldy, backdrop;  // BLDCNT, BLDALPHA, BLDY; palette colour 0 (BGR555)
    uint16_t masterBright;   // MASTER_BRIGHT
    int16_t hofs3d;          // BG0HOFS (the 3D layer's scroll), -256 to 255
    uint8_t prio[4];         // BG0-3's priority
    uint8_t white;           // 1: the line is white (display off, forced blank, engine off; VRAM and main memory
                             // display, which the game doesn't use, too)
    // with sprites (Gpu2dLayers.sprite; else 0): the windows' shapes instead of Gpu2dLayers.region: WIN0H, WIN1H,
    // and which of windows 0 and 1 the line is inside vertically (bits 0, 1)
    uint16_t winh[2];
    uint8_t winV;
} Gpu2dLineState;
// The OBJs as sprites (the 3DS's direct build): with Gpu2dLayers.sprite set, the OBJs
// aren't drawn line by line into `obj`: each shown OBJ's box comes once a frame as its pixels in the atlas `sprite`
// (GPU2D_ATLAS_W x GPU2D_ATLAS_H, RGBA8 as `obj`'s with alpha 255, 0 transparent; affine ones transformed) and its
// place, for the GPU to draw; the windows too (Gpu2dLineState.winh, winV; the OBJ window from its OBJs' sprites),
// not as Gpu2dLayers.region. (A BG the CPU draws then has the windows applied without the OBJ window.)
enum { GPU2D_ATLAS_W = 512, GPU2D_ATLAS_H = 256 };
typedef struct Gpu2dSprite {
    int16_t x, y;   // the box's top left on the screen (y below 0: wrapped round from the bottom)
    uint16_t w, h;  // the box (twice the OBJ's size for a double-size affine one)
    uint16_t u, v;  // its pixels in the atlas
    uint8_t prio;
    uint8_t window;  // 1: an OBJ-window OBJ (it shapes the OBJ window, isn't shown)
#ifdef N3DS_DIRECT
    uint8_t anchor;  // the UI element it belongs to (gpu2d_obj_anchors; 0 none)
#endif
} Gpu2dSprite;
#ifdef N3DS_DIRECT
// The enhanced port's UI at the wide screens' edges: an element's anchor, the edges the
// host moves it to (by the margins' width), or GPU2D_NEAR | id << 5 (id 0-7): towards the edges its OBJs (those with
// that anchor) are in the outer third of
enum { GPU2D_LEFT = 1, GPU2D_RIGHT = 2, GPU2D_TOP = 4, GPU2D_BOTTOM = 8, GPU2D_NEAR = 0x10 };
typedef struct Gpu2dBgAnchor {
    uint8_t bgs, windows, anchor;  // the BGs (bit n: BGn) and windows (bit w: window w) moved, and where
    uint16_t bgcnt[4];             // the BGs' BGxCNT when given: moved only on lines where they are the same
} Gpu2dBgAnchor;
// The enhanced port's wide view: an affine BG whose parameters the game changes line by
// line (the sky's clouds) on the lines beyond the DS's screen, as the game would set them there (gpu2d_margin_affine):
// lines -GPU2D_MARGIN to -1, then 192 to 191 + GPU2D_MARGIN; PA and the reference point (PB, PC, PD as at the edge line)
enum { GPU2D_MARGIN = 24 };
typedef struct Gpu2dMarginAffine {
    int bg;  // 2 or 3; 0: none
    int16_t pa[2 * GPU2D_MARGIN];
    int32_t x[2 * GPU2D_MARGIN], y[2 * GPU2D_MARGIN];
} Gpu2dMarginAffine;
#endif
typedef struct Gpu2dLayers {
    uint16_t *bg[2][4];      // per engine BG0-3: RGBA5551 (r << 11 | g << 6 | b << 1 | 1), 0 transparent
    uint32_t *obj[2];        // OBJs: RGBA8 (r << 24 | g << 16 | b << 8 | (priority + 1) * 32), 0 transparent
    // with windows (DISPCNT bits 13-15): the window each pixel is in, as alpha 63 + 64 * region (0 outside, 1
    // window 0, 2 window 1, 3 the OBJ window); WININ / WINOUT say what each shows
    uint32_t *region[2];
    uint8_t objPrio[2][192];  // the OBJ priorities with pixels on each line (bit 0-3: priority 0-3)
    Gpu2dLineState line[2][192];
    int32_t ref[2][192][4];  // BG2 X, Y, BG3 X, Y: the affine BGs' internal reference points at each line (20.8)
    // each engine's BG VRAM (16 KB granules, the address mask) and BG palette (0x200 bytes), as at the frame's end
    const uint8_t *const *bgGran[2];
    uint32_t bgMask[2];
    const uint8_t *palette[2];
    int topEngine;           // the engine on the top screen (POWCNT1 bit 15)
    uint32_t *sprite;        // the OBJs as sprites (above; NULL: in `obj`)
    Gpu2dSprite sprites[2][128];  // per engine, in OAM order
    int nsprites[2];
#ifdef N3DS_DIRECT
    Gpu2dMarginAffine margin[2];  // per engine, for this frame
    int iris;                     // the screen transitions' iris (gpu2d_iris) for this frame
    int menuWipe;                 // the menus' wipe (gpu2d_menu_wipe) for this frame
    const uint32_t *optPic;       // the options page's picture (gpu2d_options) and its serial, for this frame
    uint32_t optSerial;
    int optPageY, optBtnY;
    Gpu2dBgAnchor bgAnchor[2];    // per engine (gpu2d_bg_anchor), for this frame
#endif
} Gpu2dLayers;

typedef struct Gpu2dFrameInput {
    const uint8_t *bank[9];   // VRAM banks A-I (128, 128, 128, 128, 64, 16, 16, 32, 16 KB)
    const uint8_t *palette;   // 0x800 bytes: engine A BG, A OBJ, B BG, B OBJ (0x200 each)
    const uint8_t *oam;       // 0x800 bytes: engine A, engine B
    // the 3D engine's picture for this frame (engine A's BG0 when DISPCNT bit 3 is set): 192 lines of 256 pixels,
    // red bits 0-5, green 8-13, blue 16-21, alpha 24-28 (0-31; 0: no pixel), not scrolled; NULL: no 3D picture
    const uint32_t *lines3d;
    // called after each visible line (0-191) of both engines, the H-blank: register writes (gpu2d_write*) and
    // palette changes made there show from the next line on (NULL: none); it returns 0 if nothing ran there that
    // could change the palettes or OAM
    int (*hblank)(int line, void *context);
    void *hblankContext;
    // 1: the 3D layer is composited by the caller (lines3d NULL; the GPU: gpu2d_deferred), not here
    int defer3d;
    // the pictures drawn by a GPU (gpu2d_gpu_frame; implies defer3d): 1 the frame's inputs recorded for it and no
    // pixels drawn here, 2 both (to check the GPU's against these)
    int gpu;
    // the layers to the caller instead of the pictures (gpu2d_screen then stale); NULL: none
    Gpu2dLayers *layers;
    // with layers: only each line's state recorded here (with the H-blanks), and VRAM's banks, the palettes and OAM
    // copied; the layers are drawn from that by gpu2d_draw_layers, later and on another thread (the 3DS's GPU
    // thread). Gpu2dLayers.bgGran and .palette then point to the copies.
    int deferLayers;
} Gpu2dFrameInput;

void gpu2d_reset(void);
// register writes (addresses as on the DS; little-endian, unaligned halves are split)
void gpu2d_write8(uint32_t addr, uint8_t value);
void gpu2d_write16(uint32_t addr, uint16_t value);
void gpu2d_write32(uint32_t addr, uint32_t value);
uint32_t gpu2d_read32(uint32_t addr);  // what was written (diagnostics)

void gpu2d_render_frame(const Gpu2dFrameInput *in);
// deferLayers: the record the last frame went to (two in turn), and drawing a record's layers (and Gpu2dLayers'
// frame fields; not while the record after the next is made)
int gpu2d_record(void);
void gpu2d_draw_layers(int record);
#ifdef N3DS_DIRECT
// an affine BG's parameters on a line beyond the DS's screen (Gpu2dMarginAffine), given by the game before a frame (its
// V-blank) for that frame
void gpu2d_margin_affine(int engine, int bg, int line, int pa, int32_t x, int32_t y);
// the screen transitions' iris, drawn by the host over both whole screens: 0 open to 0x1000 closed, -1 none; given by
// the game at its V-blank, from the next frame on
void gpu2d_iris(int progress);
// the menus' wipe (MenuScreen_WipeInVBlank / WipeOutVBlank): engine A's window 1, its left edge on line y at
// pos - (a zigzag of 24 lines up and 24 down), drawn by the host over the whole wide screen; given by the game at its
// V-blank, from the next frame on, kept while that window is on (-1 none)
void gpu2d_menu_wipe(int pos);
// the options page (modern/host/options_3ds.c): a 256x192 RGBA picture (0xRRGGBBAA; the page in lines 17-191, the pause
// menu's button at (0, 0), 24x16), drawn over the bottom screen; serial changes whenever its pixels did (the host
// uploads it then), pageY the page's offset downwards (0 in place, 192 or more not shown: it slides); from the next
// frame on, kept until changed. The picture must stay as it is until two more serials have been given.
#define GPU2D_NO_BTN (-999)
void gpu2d_options(const uint32_t *pic, uint32_t serial, int pageY);
// (gpu2d_opt_button, n3ds_ui.h: the button's top line, as an OBJ: shown with the next OAM given, for that OAM only)
// the UI elements' anchors (Gpu2dSprite.anchor): one for each of an engine's 128 OAM entries, given with the OAM the game
// loads (0x400 bytes) at its V-blank; an entry keeps its anchor while OAM holds what it held then
void gpu2d_obj_anchors(int engine, const void *oam, const uint8_t *anchors);
// BGs and windows that make a UI element with the OBJs of the same anchor (Gpu2dBgAnchor); given by the game when it
// shows them (their BGxCNT set up), from the next frame on, kept until changed (bgs 0: none)
void gpu2d_bg_anchor(int engine, int bgs, int windows, int anchor);
#endif
// screen 0 top, 1 bottom (POWCNT1 bit 15 decides which engine shows where): 256x192 pixels, 0xffRRGGBB
const uint32_t *gpu2d_screen(int screen);

// with defer3d: per pixel of a screen (256x192) two words that say how its picture comes from the 3D layer's pixel P
// there (which the caller has): word 0 bits 0-17 the colour A (6 bits a channel: r | g << 6 | b << 12) where P is
// transparent; word 1 bits 30-31 the kind, 0-29 by the kind (colour C as A, bits 18-22 p1, 23-27 p2, 28-29 e):
//   0: A whatever P is (A is final)
//   1: C where P is opaque (something in front of the 3D layer, effects done)
//   2: where P is opaque, blend(C, P, p1, p2): min(63, (C * p1 + P * p2 + 8) >> 4) (C in front of the 3D layer)
//   3: where P is opaque, the 3D layer on top, C below it: e = 0 P; 1 (P * (Pa + 1) + C * (31 - Pa) + 16) >> 5;
//      2 brightness up by p1 (P + ((63 - P) * p1 + 8) >> 4); 3 down (P - (P * p1 + 7) >> 4)
// For kinds 1-3 the master brightness (gpu2d_deferred_lines: MASTER_BRIGHT, bits 0-15) is the caller's to apply
// (brightness up by f: c + ((63 - c) * f) >> 4, down: c - (c * f + 15) >> 4, f = min(16, bits 0-4)); A already has
// it for kind 0 only. P is the 3D picture's pixel at x + the line's BG0HOFS (bits 16-24, 9-bit signed).
const uint32_t *gpu2d_deferred(int screen);
const uint32_t *gpu2d_deferred_lines(int screen);

// With Gpu2dFrameInput.gpu: what a GPU needs to draw the frame into gpu2d_deferred's words (wgpu_display.c, whose
// shader follows this renderer's rules). VRAM is laid out as the banks A-I at GPU2D_BANK_OFFSET, a zero granule at
// GPU2D_VRAM_ZERO, and the composites of overlapping banks (`overlap`, overlapSize bytes) at GPU2D_VRAM_OVERLAP.
#define GPU2D_BANK_OFFSET {0x00000u, 0x20000u, 0x40000u, 0x60000u, 0x80000u, 0x90000u, 0x94000u, 0x98000u, 0xa0000u}
enum { GPU2D_VRAM_ZERO = 0xa4000, GPU2D_VRAM_OVERLAP = 0xa8000, GPU2D_LINE_WORDS = 40, GPU2D_MAX_VERSIONS = 193 };
// a line of an engine (GPU2D_LINE_WORDS words): 0-27 the registers (0x04000000 / 0x04001000 + 0-0x6f), 28-31 the
// affine BGs' reference points (BG2 X, Y, BG3 X, Y) at this line, 32 the windows' flags at its start (window 0
// bits 0-1, window 1 bits 2-3: inside vertically, horizontally), 33 the line OBJ mosaic shows, 34 / 35 the palette /
// OAM version, 36 POWCNT1
enum { G2L_REGS = 0, G2L_REF = 28, G2L_WIN = 32, G2L_OBJMOSAIC = 33, G2L_PALETTE = 34, G2L_OAM = 35, G2L_POWCNT = 36 };
// an engine's VRAM tables (64 words, byte offsets into the layout): 0-31 the BG area's 16 KB granules, 32-47 the
// OBJ area's, 48-51 the extended BG palette slots, 52 the extended OBJ palette (unmapped ones: the zero granule),
// 53-56 banks A-D when mapped to the LCDC (else ~0); word 63 of engine A's: the screen it shows on (0 top)
enum { G2T_BG = 0, G2T_OBJ = 32, G2T_BGEXT = 48, G2T_OBJEXT = 52, G2T_LCDC = 53, G2T_SCREEN_A = 63 };
typedef struct Gpu2dGpuFrame {
    const uint8_t *overlap;
    uint32_t overlapSize;
    uint32_t tables[2][64];
    uint32_t lines[2][192][GPU2D_LINE_WORDS];
    const uint8_t *palettes;  // paletteCount versions of the palettes (0x800 bytes each: as Gpu2dFrameInput.palette)
    uint32_t paletteCount;
    const uint8_t *oams;      // oamCount versions of OAM (0x800 bytes each)
    uint32_t oamCount;
    // line 0's OBJ pixels (prepared during the previous frame): value 0-15, source 16-17, priority 18-19, kind 20-21,
    // alpha 22-25, mosaic 26, OBJ window 27
    uint32_t obj0[2][256];
} Gpu2dGpuFrame;
const Gpu2dGpuFrame *gpu2d_gpu_frame(void);

// diagnostics: what made pixel (x, y) of a screen in the last frame, as text ("engine A: OBJ#12 over BG2, alpha");
// only with gpu2d_set_diagnostics(1) (it costs time)
void gpu2d_set_diagnostics(int on);
void gpu2d_describe_pixel(int screen, int x, int y, char *buf, size_t size);
// ACWW_GPU2D_STATS-style census of the features the frames used (one line per feature with the first frame)
void gpu2d_print_features(void);

#ifdef __cplusplus
}
#endif

#endif

// The 2D renderer's internals (gpu2d.h): the per-engine state and the helpers that gpu2d.c (frames, VRAM mapping,
// windows, compositing), gpu2d_bg.c (BG layers) and gpu2d_obj.c (OBJs) share.

#ifndef GPU2D_INTERNAL_H
#define GPU2D_INTERNAL_H

#include <stdint.h>
#include <string.h>

#include "gpu2d.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;

enum { SCREEN_W = 256, SCREEN_H = 192, FRAME_LINES = 263 };
enum { GRANULE_SHIFT = 14, GRANULE = 1 << GRANULE_SHIFT };  // VRAM is mapped in 16 KB steps

// the display registers (offsets from 0x04000000 / 0x04001000)
enum {
    REG_DISPCNT = 0x00, REG_BGCNT = 0x08, REG_BGHOFS = 0x10, REG_BGVOFS = 0x12, REG_BG2PA = 0x20,
    REG_WIN0H = 0x40, REG_WIN1H = 0x42, REG_WIN0V = 0x44, REG_WIN1V = 0x46, REG_WININ = 0x48, REG_WINOUT = 0x4a,
    REG_MOSAIC = 0x4c, REG_BLDCNT = 0x50, REG_BLDALPHA = 0x52, REG_BLDY = 0x54, REG_DISPCAPCNT = 0x64,
    REG_MASTER_BRIGHT = 0x6c, REG_SIZE = 0x70,
};

// the layers, numbered as BLDCNT's bits: BG0-3, OBJ, backdrop
enum { LAYER_BG0 = 0, LAYER_OBJ = 4, LAYER_BACKDROP = 5, LAYER_NONE = 7 };

// a pixel of a layer's line: BGR555 colour in bits 0-14, PIX_OPAQUE when something is there. The 3D layer's pixels
// are kept apart in the 3D format (gpu2d.h).
enum { PIX_OPAQUE = 0x8000 };

// what an OBJ pixel is (Engine.objKind)
enum { OBJ_NORMAL = 0, OBJ_SEMI_TRANSPARENT = 1, OBJ_BITMAP = 2 };

// an OBJ pixel's colour (ObjLine.source / value): a palette colour is looked up when the line is composited, so
// a pixel prepared a line ahead shows the palette of the line it is on
enum { OBJ_SRC_NONE = 0, OBJ_SRC_DIRECT = 1, OBJ_SRC_PALETTE = 2, OBJ_SRC_EXTPAL = 3 };

typedef struct ObjLine {
    u16 value[SCREEN_W];   // BGR555 (direct), the OBJ palette's entry, or the extended OBJ palette's
    u8 source[SCREEN_W];   // OBJ_SRC_* (OBJ_SRC_NONE: no OBJ pixel)
    u8 prio[SCREEN_W];     // 0-3
    u8 kind[SCREEN_W];     // OBJ_*
    u8 alpha[SCREEN_W];    // a bitmap OBJ's alpha (0-15)
    u8 index[SCREEN_W];    // the OAM entry (diagnostics)
    // 1: the pixel is a mosaic OBJ's (attr0 bit 12): of the front OBJ where one is opaque, else of the back one
    // whose box covers it (a transparent pixel of a mosaic OBJ takes part in the horizontal mosaic too)
    u8 mosaic[SCREEN_W];
    u16 coverKey[SCREEN_W];  // (while drawing: the back covering OBJ, priority * 128 + entry + 1; 0: none)
    u8 window[SCREEN_W];   // 1: inside the OBJ window (an opaque pixel of an OBJ in window mode)
} ObjLine;

typedef struct Engine {
    int num;                   // 0: engine A, 1: engine B
    u8 regs[REG_SIZE];         // the registers as written
    // VRAM as this engine sees it this frame (gpu2d.c map_vram): 16 KB granules, a zero granule where nothing is
    // mapped; the address masks of the BG and OBJ areas (A: 512 and 256 KB, B: 128 KB each)
    const u8 *bgGran[32], *objGran[16];
    u32 bgMask, objMask;
    const u8 *bgExtPal[4];     // the four 8 KB extended BG palette slots (NULL: not mapped)
    const u8 *objExtPal;       // the 8 KB extended OBJ palette (NULL: not mapped)
    const u8 *palette;         // 0x400 bytes: BG palette, then OBJ palette
    const u8 *oam;             // 0x400 bytes
#ifdef N3DS_DIRECT
    const u8 *objAnchor;       // each OAM entry's anchor (gpu2d_obj_anchors) for this frame
#endif
    // the affine BGs' internal reference points (BG2, BG3) for line refLine: from BGxX/BGxY at the frame's start,
    // then each line adds PB/PD, and a write to BGxX / BGxY (bit 0 / 1 of refWritten[i], an H-blank effect) reloads it
    s32 refX[2], refY[2];
    int refLine;
    u8 refWritten[2];
    u8 winActive[2];           // windows 0 and 1: bit 0 inside vertically, bit 1 horizontally (gpu2d.c)
    u16 powcnt;                // POWCNT1 for the line being drawn
    // the line buffers
    u16 bg[4][SCREEN_W];
    ObjLine obj;
    ObjLine objNextLine0;      // OBJs are prepared one line ahead: line 0's at the end of the previous frame
    int haveNextLine0;
    // OBJ vertical mosaic: the line mosaic OBJs show (set every MOSAIC bits 12-15 + 1 lines), and the line counter
    int objMosaicLine, objMosaicCount;
    u32 out[SCREEN_W * SCREEN_H];  // this engine's picture (6-bit channels: r | g << 8 | b << 16)
    // with the 3D layer deferred (gpu2d.h gpu2d_deferred): word 1 per pixel (0: kind 0), and per line
    // MASTER_BRIGHT | BG0HOFS << 16
    u32 defer[SCREEN_W * SCREEN_H];
    u32 lineInfo[SCREEN_H];
    // diagnostics: per pixel the top layer, the one below it (LAYER_*, 6: 3D) and the effect applied
    u8 dbgTop[SCREEN_W * SCREEN_H], dbgBelow[SCREEN_W * SCREEN_H], dbgEffect[SCREEN_W * SCREEN_H];
    u8 dbgObj[SCREEN_W * SCREEN_H], dbgAlpha[SCREEN_W * SCREEN_H];
    u32 dbgTopColor[SCREEN_W * SCREEN_H], dbgBelowColor[SCREEN_W * SCREEN_H];  // (6-bit channels)
} Engine;

static inline u16 reg16(const Engine *e, u32 off) { return (u16)(e->regs[off] | e->regs[off + 1] << 8); }
static inline u32 reg32(const Engine *e, u32 off) { return reg16(e, off) | (u32)reg16(e, off + 2) << 16; }

// VRAM reads through an engine's granule tables
static inline u8 vram8(const u8 *const *gran, u32 mask, u32 addr) {
    addr &= mask;
    return gran[addr >> GRANULE_SHIFT][addr & (GRANULE - 1)];
}
static inline u16 vram16(const u8 *const *gran, u32 mask, u32 addr) {
    addr &= mask & ~1u;
    const u8 *p = gran[addr >> GRANULE_SHIFT] + (addr & (GRANULE - 1));
    return (u16)(p[0] | p[1] << 8);
}
static inline u8 bg8(const Engine *e, u32 a) { return vram8(e->bgGran, e->bgMask, a); }
static inline u16 bg16(const Engine *e, u32 a) { return vram16(e->bgGran, e->bgMask, a); }
static inline u8 obj8(const Engine *e, u32 a) { return vram8(e->objGran, e->objMask, a); }
static inline u16 obj16(const Engine *e, u32 a) { return vram16(e->objGran, e->objMask, a); }
static inline u16 pal16(const u8 *p, u32 index) { return (u16)(p[index * 2] | p[index * 2 + 1] << 8); }

// features seen (gpu2d_print_features)
enum Feature {
    F_MODE0, F_MODE1, F_MODE2, F_MODE3, F_MODE4, F_MODE5, F_MODE6, F_MODE7,
    F_3D, F_TEXT4, F_TEXT8, F_TEXT8_EXTPAL, F_AFFINE, F_EXT_TILES, F_EXT_TILES_EXTPAL, F_EXT_BITMAP8,
    F_EXT_DIRECT, F_LARGE_BITMAP, F_BG_MOSAIC, F_OBJ, F_OBJ_AFFINE, F_OBJ_DOUBLE, F_OBJ_8BPP, F_OBJ_EXTPAL,
    F_OBJ_BITMAP, F_OBJ_SEMI, F_OBJ_WINDOW, F_OBJ_MOSAIC, F_OBJ_2D, F_WIN0, F_WIN1, F_WINOBJ, F_ALPHA,
    F_BRIGHT_UP, F_BRIGHT_DOWN, F_MASTER_UP, F_MASTER_DOWN, F_FORCED_BLANK, F_DISPLAY_OFF, F_DISPLAY_VRAM,
    F_DISPLAY_FIFO, F_CAPTURE, F_ENGINE_B_OFF, F_ENGINE_A_OFF, F_COUNT
};
void gpu2d_feature(const Engine *e, enum Feature f);

// gpu2d_bg.c: BG n's pixels of a line into e->bg[n] (all transparent when the BG shows nothing in this mode)
void gpu2d_render_bg_line(Engine *e, int n, int line);
// what BG n is in the engine's BG mode: 0 nothing, 1 text, 2 affine, 3 extended, 4 the large bitmap
int gpu2d_bg_kind(const Engine *e, int n);
// gpu2d_obj.c: the OBJs' pixels of a line into *dst
void gpu2d_render_obj_line(Engine *e, int line, ObjLine *dst);
// gpu2d_obj.c: the shown OBJs (the OBJ-window ones flagged) as sprites, their boxes' pixels into the atlas from (*ax, *ay)
// on in rows *rowH tall (Gpu2dLayers.sprite); returns how many were put into out (a box that doesn't fit is left out)
struct Gpu2dSprite;
int gpu2d_obj_sprites(const Engine *e, struct Gpu2dSprite *out, u32 *atlas, int *ax, int *ay, int *rowH);

#endif

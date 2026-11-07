// The 2D renderer's BG layers (gpu2d.h): one line of a BG into the engine's line buffer, as BGR555 | PIX_OPAQUE.
//
// What BG n is depends on the BG mode (DISPCNT bits 0-2):
//   mode   BG0   BG1   BG2       BG3
//   0      text  text  text      text
//   1      text  text  text      affine
//   2      text  text  affine    affine
//   3      text  text  text      extended
//   4      text  text  affine    extended
//   5      text  text  extended  extended
//   6      (3D)  -     large     -          (engine A only)
// Engine A's BG0 is the 3D layer when DISPCNT bit 3 is set (gpu2d.c composites it); here it is then empty.

#include "gpu2d_internal.h"

enum BgKind { BG_NONE, BG_TEXT, BG_AFFINE, BG_EXTENDED, BG_LARGE };

static const u8 kBgKinds[8][4] = {
    {BG_TEXT, BG_TEXT, BG_TEXT, BG_TEXT},       {BG_TEXT, BG_TEXT, BG_TEXT, BG_AFFINE},
    {BG_TEXT, BG_TEXT, BG_AFFINE, BG_AFFINE},   {BG_TEXT, BG_TEXT, BG_TEXT, BG_EXTENDED},
    {BG_TEXT, BG_TEXT, BG_AFFINE, BG_EXTENDED}, {BG_TEXT, BG_TEXT, BG_EXTENDED, BG_EXTENDED},
    {BG_NONE, BG_NONE, BG_LARGE, BG_NONE},      {BG_NONE, BG_NONE, BG_NONE, BG_NONE},
};

static const u8 kZeroPalette[0x2000];  // an extended palette slot that isn't mapped reads as 0

// the character and screen (map) base of BG n: BGxCNT's (16 KB and 2 KB steps), plus DISPCNT's (64 KB steps,
// engine A only)
static u32 char_base(const Engine *e, u16 cnt) {
    u32 b = (cnt >> 2 & 15) * 0x4000u;
    if (e->num == 0) b += (reg32(e, REG_DISPCNT) >> 24 & 7) * 0x10000u;
    return b;
}
static u32 map_base(const Engine *e, u16 cnt) {
    u32 b = (cnt >> 8 & 31) * 0x800u;
    if (e->num == 0) b += (reg32(e, REG_DISPCNT) >> 27 & 7) * 0x10000u;
    return b;
}

// the extended palette slot BG n uses for 256-colour tiles, or NULL for the standard palette (DISPCNT bit 30 off).
// BG0 and BG1 can use slot 2 and 3 instead of 0 and 1 (BGxCNT bit 13).
static const u8 *ext_palette(const Engine *e, int n, u16 cnt) {
    if (!(reg32(e, REG_DISPCNT) & 1u << 30)) return NULL;
    int slot = n;
    if (n < 2 && (cnt & 0x2000)) slot += 2;
    return e->bgExtPal[slot] ? e->bgExtPal[slot] : kZeroPalette;
}

// a pixel of an 8x8 tile in 256 colours: tile entry (text and extended BGs' 16-bit format: tile 0-9, flips 10-11,
// palette 12-15) and the pixel's place in the tile
static u16 tile8_pixel(const Engine *e, u32 charBase, u16 entry, u32 px, u32 py, const u8 *ext) {
    if (entry & 0x400) px = 7 - px;
    if (entry & 0x800) py = 7 - py;
    u8 idx = bg8(e, charBase + (entry & 0x3ffu) * 64 + py * 8 + px);
    if (!idx) return 0;
    u16 c = ext ? pal16(ext, (entry >> 12) * 256u + idx) : pal16(e->palette, idx);
    return (u16)((c & 0x7fff) | PIX_OPAQUE);
}

// text BG: scrolled tiles, 256x256 to 512x512 pixels in 32x32-tile blocks of 2 KB (blocks left to right, then top to
// bottom)
static void text_line(Engine *e, int n, int line, u16 *dst) {
    u16 cnt = reg16(e, REG_BGCNT + 2 * n);
    u32 hofs = reg16(e, REG_BGHOFS + 4 * n) & 0x1ff, vofs = reg16(e, REG_BGVOFS + 4 * n) & 0x1ff;
    u32 cb = char_base(e, cnt), mb = map_base(e, cnt);
    u32 wide = cnt & 0x4000 ? 512 : 256, tall = cnt & 0x8000 ? 512 : 256;
    u32 y = (line + vofs) & (tall - 1);
    u32 rowBase = mb + (y >= 256 ? (wide == 512 ? 2 : 1) * 0x800u : 0) + (y & 255) / 8 * 64;
    int color256 = cnt & 0x80;
    const u8 *ext = color256 ? ext_palette(e, n, cnt) : NULL;
    gpu2d_feature(e, color256 ? (ext ? F_TEXT8_EXTPAL : F_TEXT8) : F_TEXT4);
    for (int x = 0; x < SCREEN_W;) {  // a tile at a time
        u32 sx = (x + hofs) & (wide - 1);
        u16 entry = bg16(e, rowBase + (sx >= 256 ? 0x800u : 0) + (sx & 255) / 8 * 2);
        u32 tile = entry & 0x3ffu, py = entry & 0x800 ? 7 - (y & 7) : y & 7;
        u8 row[8];  // the tile's row as shown (colour indexes, 0 transparent)
        const u8 *pal;
        if (color256) {
            for (u32 i = 0; i < 8; i++) row[i] = bg8(e, cb + tile * 64 + py * 8 + i);
            pal = ext ? ext + (entry >> 12) * 512u : e->palette;
        } else {
            for (u32 i = 0; i < 4; i++) {
                u8 b = bg8(e, cb + tile * 32 + py * 4 + i);
                row[2 * i] = b & 15;
                row[2 * i + 1] = b >> 4;
            }
            pal = e->palette + (entry >> 12) * 32u;
        }
        int flip = entry & 0x400 ? 7 : 0;
        for (u32 px = sx & 7; px < 8 && x < SCREEN_W; px++, x++) {
            u8 idx = row[px ^ flip];
            dst[x] = idx ? (u16)((pal16(pal, idx) & 0x7fff) | PIX_OPAQUE) : 0;
        }
    }
}

// the affine parameters of BG n (2 or 3): PA, PC (per pixel), and the reference point of this line (20.8 fixed
// point: the internal reference point, gpu2d.c; a mosaic line above the current one steps back by PB, PD)
static void affine_start(const Engine *e, int n, int line, s32 *x, s32 *y, s32 *pa, s32 *pc) {
    u32 r = REG_BG2PA + (u32)(n - 2) * 0x10;
    *pa = (s16)reg16(e, r);
    s32 pb = (s16)reg16(e, r + 2);
    *pc = (s16)reg16(e, r + 4);
    s32 pd = (s16)reg16(e, r + 6);
    *x = e->refX[n - 2] + pb * (line - e->refLine);
    *y = e->refY[n - 2] + pd * (line - e->refLine);
}

// coordinate c of a w-wide area: wrapped (BGxCNT bit 13) or -1 outside
static inline s32 affine_coord(s32 c, s32 w, int wrap) {
    if (wrap) return c & (w - 1);
    return c >= 0 && c < w ? c : -1;
}

// affine BG: 8-bit map entries, 256-colour tiles in the standard palette, 128x128 to 1024x1024 pixels
static void affine_line(Engine *e, int n, int line, u16 *dst) {
    u16 cnt = reg16(e, REG_BGCNT + 2 * n);
    s32 size = 128 << (cnt >> 14), x0, y0, pa, pc;
    int wrap = cnt & 0x2000;
    u32 cb = char_base(e, cnt), mb = map_base(e, cnt);
    affine_start(e, n, line, &x0, &y0, &pa, &pc);
    gpu2d_feature(e, F_AFFINE);
    for (int x = 0; x < SCREEN_W; x++) {
        s32 tx = affine_coord((x0 + pa * x) >> 8, size, wrap), ty = affine_coord((y0 + pc * x) >> 8, size, wrap);
        if (tx < 0 || ty < 0) { dst[x] = 0; continue; }
        u8 tile = bg8(e, mb + (u32)(ty / 8 * (size / 8) + tx / 8));
        u8 idx = bg8(e, cb + tile * 64u + (u32)(ty & 7) * 8 + (u32)(tx & 7));
        dst[x] = idx ? (u16)((pal16(e->palette, idx) & 0x7fff) | PIX_OPAQUE) : 0;
    }
}

// a 256-colour or direct-colour bitmap of w x h pixels at base, through the affine transformation
static void bitmap_line(Engine *e, int n, int line, u16 *dst, u32 base, s32 w, s32 h, int direct) {
    u16 cnt = reg16(e, REG_BGCNT + 2 * n);
    s32 x0, y0, pa, pc;
    int wrap = cnt & 0x2000;
    affine_start(e, n, line, &x0, &y0, &pa, &pc);
    for (int x = 0; x < SCREEN_W; x++) {
        s32 tx = affine_coord((x0 + pa * x) >> 8, w, wrap), ty = affine_coord((y0 + pc * x) >> 8, h, wrap);
        if (tx < 0 || ty < 0) { dst[x] = 0; continue; }
        u32 at = (u32)(ty * w + tx);
        if (direct) {
            u16 c = bg16(e, base + at * 2);
            dst[x] = c & 0x8000 ? (u16)((c & 0x7fff) | PIX_OPAQUE) : 0;
        } else {
            u8 idx = bg8(e, base + at);
            dst[x] = idx ? (u16)((pal16(e->palette, idx) & 0x7fff) | PIX_OPAQUE) : 0;
        }
    }
}

// extended affine BG: BGxCNT bit 7 clear: 16-bit map entries (as text BGs': flips, extended palettes), 128x128 to
// 1024x1024; set: a bitmap at the screen base in 16 KB steps, 128x128, 256x256, 512x256 or 512x512, of 256 colours
// (bit 2 clear) or direct colours (bit 2 set, bit 15 of a pixel: opaque)
static void extended_line(Engine *e, int n, int line, u16 *dst) {
    u16 cnt = reg16(e, REG_BGCNT + 2 * n);
    int sz = cnt >> 14;
    if (cnt & 0x80) {
        static const s32 kW[4] = {128, 256, 512, 512}, kH[4] = {128, 256, 256, 512};
        int direct = cnt & 4;
        gpu2d_feature(e, direct ? F_EXT_DIRECT : F_EXT_BITMAP8);
        bitmap_line(e, n, line, dst, (cnt >> 8 & 31) * 0x4000u, kW[sz], kH[sz], direct);
        return;
    }
    s32 size = 128 << sz, x0, y0, pa, pc;
    int wrap = cnt & 0x2000;
    u32 cb = char_base(e, cnt), mb = map_base(e, cnt);
    const u8 *ext = ext_palette(e, n, cnt);
    gpu2d_feature(e, ext ? F_EXT_TILES_EXTPAL : F_EXT_TILES);
    affine_start(e, n, line, &x0, &y0, &pa, &pc);
    for (int x = 0; x < SCREEN_W; x++) {
        s32 tx = affine_coord((x0 + pa * x) >> 8, size, wrap), ty = affine_coord((y0 + pc * x) >> 8, size, wrap);
        if (tx < 0 || ty < 0) { dst[x] = 0; continue; }
        u16 entry = bg16(e, mb + (u32)(ty / 8 * (size / 8) + tx / 8) * 2);
        dst[x] = tile8_pixel(e, cb, entry, (u32)(tx & 7), (u32)(ty & 7), ext);
    }
}

int gpu2d_bg_kind(const Engine *e, int n) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    if (e->num == 1 && (dispcnt & 7) > 5) return BG_NONE;
    return kBgKinds[dispcnt & 7][n];
}

// BG n's pixels of line `line`
void gpu2d_render_bg_line(Engine *e, int n, int line) {
    u16 *dst = e->bg[n];
    u32 dispcnt = reg32(e, REG_DISPCNT);
    int mode = dispcnt & 7;
    u16 cnt = reg16(e, REG_BGCNT + 2 * n);
    memset(dst, 0, sizeof e->bg[n]);
    if (!(dispcnt & 0x100u << n)) return;
    if (e->num == 1 && mode > 5) return;      // (engine B has no mode 6)
    if (n == 0 && e->num == 0 && (dispcnt & 8)) return;  // the 3D layer
    // vertical mosaic: the line at the start of the mosaic block (BGxCNT bit 6; MOSAIC bits 4-7: height - 1)
    int mosaic = cnt & 0x40;
    u16 mos = reg16(e, REG_MOSAIC);
    if (mosaic) {
        gpu2d_feature(e, F_BG_MOSAIC);
        line -= line % ((mos >> 4 & 15) + 1);
    }
    switch (kBgKinds[mode][n]) {
    case BG_TEXT: text_line(e, n, line, dst); break;
    case BG_AFFINE: affine_line(e, n, line, dst); break;
    case BG_EXTENDED: extended_line(e, n, line, dst); break;
    case BG_LARGE: {  // (BGxCNT bits 14-15: 512x1024, 1024x512, 512x256, 512x512)
        static const s32 kW[4] = {512, 1024, 512, 512}, kH[4] = {1024, 512, 256, 512};
        gpu2d_feature(e, F_LARGE_BITMAP);
        bitmap_line(e, n, line, dst, 0, kW[cnt >> 14], kH[cnt >> 14], 0);
        break;
    }
    default: break;
    }
    // horizontal mosaic: each block of width (MOSAIC bits 0-3) + 1 shows its first pixel
    int mw = (mos & 15) + 1;
    if (mosaic && mw > 1)
        for (int x = 0; x < SCREEN_W; x++) dst[x] = dst[x - x % mw];
}

// The 2D renderer's OBJs (gpu2d.h): one line of the 128 OAM entries into an ObjLine.
//
// An OAM entry is three halves (attribute 2's fourth half holds a quarter of an affine parameter group):
//   attr0: Y 0-7, affine 8, double size (affine) / hidden (not affine) 9, mode 10-11 (normal, semi-transparent,
//          OBJ window, bitmap), mosaic 12, 256 colours 13, shape 14-15
//   attr1: X 0-8 (signed), affine group 9-13 (affine) / flips 12-13 (not affine), size 14-15
//   attr2: tile 0-9, priority 10-11, palette 12-15 (a bitmap OBJ's alpha)
// Entries are drawn in order; a later entry's pixel replaces an earlier one only if its priority is higher
// (numerically lower), so at equal priority the lower entry is in front.

#include <stdio.h>

#include "gpu2d_internal.h"

static const u8 kObjW[3][4] = {{8, 16, 32, 64}, {16, 32, 32, 64}, {8, 8, 16, 32}};
static const u8 kObjH[3][4] = {{8, 16, 32, 64}, {8, 8, 16, 32}, {16, 32, 32, 64}};

// texel (u, v) of a w-wide OBJ: OBJ_SRC_* << 16 | the colour or palette entry, or 0 (transparent)
static u32 obj_texel(const Engine *e, u32 dispcnt, u16 a0, u16 a2, u32 u, u32 v, u32 w) {
    u32 tile = a2 & 0x3ffu;
    if ((a0 >> 10 & 3) == 3) {  // bitmap: direct colours, bit 15 opaque
        u32 addr;
        if (dispcnt & 0x40) {  // 1D: 128 (or 256, engine A with DISPCNT bit 22) bytes per tile number
            u32 step = e->num == 0 && (dispcnt & 1u << 22) ? 256 : 128;
            addr = tile * step + (v * w + u) * 2;
        } else if (dispcnt & 0x20) {  // 2D, 256 pixels wide: tile bits 0-4 x / 8, bits 5-9 y / 8
            addr = (tile & 0x1f) * 0x10 + (tile & 0x3e0) * 0x80 + v * 512 + u * 2;
        } else {  // 2D, 128 pixels wide: tile bits 0-3 x / 8, bits 4-9 y / 8
            addr = (tile & 0xf) * 0x10 + (tile & 0x3f0) * 0x80 + v * 256 + u * 2;
        }
        u16 c = obj16(e, addr);
        return c & 0x8000 ? (u32)OBJ_SRC_DIRECT << 16 | (c & 0x7fffu) : 0;
    }
    int color256 = a0 & 0x2000;
    u32 addr;
    if (dispcnt & 0x10) {  // 1D: tile numbers in steps of 32 << DISPCNT bits 20-21 bytes, the tiles in a row
        addr = tile * (32u << (dispcnt >> 20 & 3)) + (v / 8 * (w / 8) + u / 8) * (color256 ? 64 : 32);
    } else {  // 2D: a 32-tile-wide character map of 32-byte tiles
        addr = tile * 32 + v / 8 * 1024 + u / 8 * (color256 ? 64 : 32);
    }
    if (color256) {
        u8 idx = obj8(e, addr + (v & 7) * 8 + (u & 7));
        if (!idx) return 0;
        if (dispcnt & 1u << 31) return (u32)OBJ_SRC_EXTPAL << 16 | ((a2 >> 12) * 256u + idx);
        return (u32)OBJ_SRC_PALETTE << 16 | idx;
    }
    u8 b = obj8(e, addr + (v & 7) * 4 + (u & 7) / 2);
    u8 idx = u & 1 ? b >> 4 : b & 15;
    return idx ? (u32)OBJ_SRC_PALETTE << 16 | ((a2 >> 12) * 16u + idx) : 0;
}

void gpu2d_render_obj_line(Engine *e, int line, ObjLine *dst) {
    memset(dst, 0, sizeof *dst);
    u32 dispcnt = reg32(e, REG_DISPCNT);
    if (line == 0) e->objMosaicLine = e->objMosaicCount = 0;
    if (!(dispcnt & 0x1000)) return;
    for (int i = 0; i < 128; i++) {
        const u8 *o = e->oam + i * 8;
        u16 a0 = (u16)(o[0] | o[1] << 8), a1 = (u16)(o[2] | o[3] << 8), a2 = (u16)(o[4] | o[5] << 8);
        int affine = a0 & 0x100, mode = a0 >> 10 & 3, shape = a0 >> 14;
        if ((!affine && (a0 & 0x200)) || shape == 3) continue;
        s32 w = kObjW[shape][a1 >> 14], h = kObjH[shape][a1 >> 14];
        s32 bw = w, bh = h;  // the box on the screen (twice the size for a double-size affine OBJ)
        if (affine && (a0 & 0x200)) { bw *= 2; bh *= 2; }
        s32 rel = (line - (a0 & 0xff)) & 0xff;  // (Y wraps around at 256)
        if (rel >= bh) continue;
        int mosaic = (a0 & 0x1000) && mode != 2;
        if (mosaic) {  // vertical mosaic: the OBJ's line at the mosaic line (gpu2d.c)
            gpu2d_feature(e, F_OBJ_MOSAIC);
            rel = (e->objMosaicLine - (a0 & 0xff)) & 0xff;
        }
        s32 x0 = a1 & 0x1ff;
        if (x0 >= 256) x0 -= 512;
        u32 prio = a2 >> 10 & 3;
        gpu2d_feature(e, F_OBJ);
        if (mode == 1) gpu2d_feature(e, F_OBJ_SEMI);
        if (mode == 2) gpu2d_feature(e, F_OBJ_WINDOW);
        if (mode == 3) {
            gpu2d_feature(e, F_OBJ_BITMAP);
            if (!(a2 >> 12)) continue;  // (alpha 0: not shown)
            if ((dispcnt & 0x60) == 0x60) continue;  // (1D mapping with the 2D width bit: prohibited; not shown)
        } else if (a0 & 0x2000) {
            gpu2d_feature(e, (dispcnt & 1u << 31) ? F_OBJ_EXTPAL : F_OBJ_8BPP);
        }
        if (!(dispcnt & 0x10) && mode != 3) gpu2d_feature(e, F_OBJ_2D);
        s32 pa = 0x100, pb = 0, pc = 0, pd = 0x100;
        if (affine) {
            const u8 *g = e->oam + (a1 >> 9 & 31) * 32;
            pa = (s16)(g[6] | g[7] << 8);
            pb = (s16)(g[14] | g[15] << 8);
            pc = (s16)(g[22] | g[23] << 8);
            pd = (s16)(g[30] | g[31] << 8);
            gpu2d_feature(e, (a0 & 0x200) ? F_OBJ_DOUBLE : F_OBJ_AFFINE);
        }
        for (s32 px = 0; px < bw; px++) {
            s32 sx = x0 + px;
            if (sx < 0 || sx >= SCREEN_W) continue;
            s32 tx = px;  // (horizontal mosaic: when the line is composited, gpu2d.c)
            s32 u, v;
            if (affine) {  // from the box's centre, through the matrix, to the texture's centre
                s32 dx = tx - bw / 2, dy = rel - bh / 2;
                u = ((pa * dx + pb * dy) >> 8) + w / 2;
                v = ((pc * dx + pd * dy) >> 8) + h / 2;
                if (u < 0 || u >= w || v < 0 || v >= h) continue;
            } else {
                u = (a1 & 0x1000) ? w - 1 - tx : tx;
                v = (a1 & 0x2000) ? h - 1 - rel : rel;
                if (u < 0) continue;
            }
            u32 c = obj_texel(e, dispcnt, a0, a2, (u32)u, (u32)v, (u32)w);
            if (mode != 2) {  // (the back OBJ covering a pixel decides a transparent pixel's mosaic flag)
                u16 key = (u16)(prio * 128 + (u32)i + 1);
                if (key > dst->coverKey[sx]) {
                    dst->coverKey[sx] = key;
                    if (!dst->source[sx]) dst->mosaic[sx] = (u8)mosaic;
                }
            }
            if (!c) continue;
            if (mode == 2) { dst->window[sx] = 1; continue; }
            if (dst->source[sx] && prio >= dst->prio[sx]) continue;
            dst->value[sx] = (u16)c;
            dst->source[sx] = (u8)(c >> 16);
            dst->prio[sx] = (u8)prio;
            dst->kind[sx] = mode == 1 ? OBJ_SEMI_TRANSPARENT : mode == 3 ? OBJ_BITMAP : OBJ_NORMAL;
            dst->alpha[sx] = (u8)(a2 >> 12);
            dst->index[sx] = (u8)i;
            dst->mosaic[sx] = (u8)mosaic;
        }
    }
}

// a not affine OBJ in the standard palette, a tile row at a time: its pixels from the tile data where it lies
static void sprite_tiles(const Engine *e, u32 dispcnt, u16 a0, u16 a1, u16 a2, s32 w, s32 h, const u32 *pal, u32 *dst) {
    int color256 = a0 & 0x2000, bytes = color256 ? 64 : 32;
    u32 tile = a2 & 0x3ffu, palBase = color256 ? 0 : (a2 >> 12) * 16u;
    for (s32 py = 0; py < h; py++) {
        u32 *d = dst + py * GPU2D_ATLAS_W;
        u32 v = (u32)((a1 & 0x2000) ? h - 1 - py : py);
        for (s32 tx = 0; tx < w / 8; tx++) {
            u32 addr = dispcnt & 0x10 ? tile * (32u << (dispcnt >> 20 & 3)) + (v / 8 * (u32)(w / 8) + (u32)tx) * bytes
                                      : tile * 32 + v / 8 * 1024 + (u32)tx * bytes;
            addr = (addr + (v & 7) * (color256 ? 8 : 4)) & e->objMask;
            const u8 *p = e->objGran[addr >> GRANULE_SHIFT] + (addr & (GRANULE - 1));
            for (int k = 0; k < 8; k++) {
                u32 idx = color256 ? p[k] : (p[k / 2] >> (4 * (k & 1)) & 15);
                s32 u = tx * 8 + k;
                d[(a1 & 0x1000) ? w - 1 - u : u] = idx ? pal[palBase + idx] : 0;
            }
        }
    }
}

int gpu2d_obj_sprites(const Engine *e, struct Gpu2dSprite *out, u32 *atlas, int *ax, int *ay, int *rowH) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    int n = 0;
    u32 pal[256];  // (the OBJ palette as the atlas's pixels)
    for (u32 i = 0; i < 256; i++) {
        u16 c = pal16(e->palette + 0x200, i);
        u32 r = c & 31, g = c >> 5 & 31, b = c >> 10 & 31;
        pal[i] = (r << 3 | r >> 2) << 24 | (g << 3 | g >> 2) << 16 | (b << 3 | b >> 2) << 8 | 0xffu;
    }
    for (int i = 0; i < 128; i++) {
        const u8 *o = e->oam + i * 8;
        u16 a0 = (u16)(o[0] | o[1] << 8), a1 = (u16)(o[2] | o[3] << 8), a2 = (u16)(o[4] | o[5] << 8);
        int affine = a0 & 0x100, mode = a0 >> 10 & 3, shape = a0 >> 14;
        if ((!affine && (a0 & 0x200)) || shape == 3) continue;
        if (mode == 3 && (!(a2 >> 12) || (dispcnt & 0x60) == 0x60)) continue;  // (as gpu2d_render_obj_line)
        s32 w = kObjW[shape][a1 >> 14], h = kObjH[shape][a1 >> 14], bw = w, bh = h;
        if (affine && (a0 & 0x200)) bw *= 2, bh *= 2;
        s32 x0 = a1 & 0x1ff, y0 = a0 & 0xff;
        if (x0 >= 256) x0 -= 512;
        // (Y wraps around at 256; shortcut: a box over both the bottom and line 0, only a 128-tall double-size one at
        // Y 129-191, shows its top part only: give it a second sprite if one is seen)
        if (y0 >= SCREEN_H && y0 + bh > 256) y0 -= 256;
        if (x0 >= SCREEN_W || x0 + bw <= 0 || y0 >= SCREEN_H) continue;
        if (*ax + bw > GPU2D_ATLAS_W) *ax = 0, *ay += *rowH, *rowH = 0;
        if (*ay + bh > GPU2D_ATLAS_H) {  // shortcut: an atlas full of OBJs leaves the rest out; grow it if this is seen
            static unsigned dropped;
            if (dropped++ % 600 == 0) fprintf(stderr, "gpu2d: the sprite atlas is full: OBJs left out (%u so far)\n", dropped);
            continue;
        }
        Gpu2dSprite *s = &out[n++];
        s->x = (int16_t)x0, s->y = (int16_t)y0, s->w = (uint16_t)bw, s->h = (uint16_t)bh;
        s->u = (uint16_t)*ax, s->v = (uint16_t)*ay, s->prio = (uint8_t)(a2 >> 10 & 3), s->window = mode == 2;
#ifdef N3DS_DIRECT
        s->anchor = e->objAnchor ? e->objAnchor[i] : 0;
#endif
        if (!affine && mode != 3 && !((a0 & 0x2000) && (dispcnt & 1u << 31))) {
            sprite_tiles(e, dispcnt, a0, a1, a2, w, h, pal, atlas + *ay * GPU2D_ATLAS_W + *ax);
            *ax += bw;
            if (bh > *rowH) *rowH = bh;
            continue;
        }
        s32 pa = 0x100, pb = 0, pc = 0, pd = 0x100;
        if (affine) {
            const u8 *g = e->oam + (a1 >> 9 & 31) * 32;
            pa = (s16)(g[6] | g[7] << 8), pb = (s16)(g[14] | g[15] << 8);
            pc = (s16)(g[22] | g[23] << 8), pd = (s16)(g[30] | g[31] << 8);
        }
        for (s32 py = 0; py < bh; py++) {
            u32 *d = atlas + (*ay + py) * GPU2D_ATLAS_W + *ax;
            for (s32 px = 0; px < bw; px++) {
                s32 u, v;
                if (affine) {  // (as gpu2d_render_obj_line)
                    s32 dx = px - bw / 2, dy = py - bh / 2;
                    u = ((pa * dx + pb * dy) >> 8) + w / 2;
                    v = ((pc * dx + pd * dy) >> 8) + h / 2;
                    if (u < 0 || u >= w || v < 0 || v >= h) { d[px] = 0; continue; }
                } else {
                    u = (a1 & 0x1000) ? w - 1 - px : px;
                    v = (a1 & 0x2000) ? h - 1 - py : py;
                }
                u32 t = obj_texel(e, dispcnt, a0, a2, (u32)u, (u32)v, (u32)w);
                if (!t) { d[px] = 0; continue; }
                u32 src = t >> 16, idx = t & 0xffff;
                u16 c = src == OBJ_SRC_DIRECT ? (u16)idx
                      : src == OBJ_SRC_EXTPAL ? (e->objExtPal ? pal16(e->objExtPal, idx & 0xfffu) : 0)
                                              : pal16(e->palette + 0x200, idx & 0xffu);
                u32 r = c & 31, g = c >> 5 & 31, b = c >> 10 & 31;
                d[px] = (r << 3 | r >> 2) << 24 | (g << 3 | g >> 2) << 16 | (b << 3 | b >> 2) << 8 | 0xffu;
            }
        }
        *ax += bw;
        if (bh > *rowH) *rowH = bh;
    }
    return n;
}

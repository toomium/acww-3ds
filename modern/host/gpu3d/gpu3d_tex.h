// The own 3D engine's texture fetch (gpu3d.h): the texture image and palette slots as VRAMCNT maps banks A-G to
// them, and one texel of a polygon's texture in each of the 7 formats (GBATEK "DS 3D Texture Formats", "DS 3D
// Texture Attributes").

// (included by gpu3d_raster.c only, so that the texel fetch is inlined into the pixel loop)
#ifndef GPU3D_TEX_H
#define GPU3D_TEX_H

#include <string.h>

#include "gpu3d_internal.h"

// the banks behind each slot, for the frame being drawn: image slots of 128 KB (banks A-D, MST 3, OFS the slot),
// palette slots of 16 KB (bank E: slots 0-3; F and G: slot OFS bit 0 + OFS bit 1 * 4); banks mapped to one slot
// together are OR-ed
enum { IMG_SLOT = 0x20000, PAL_SLOT = 0x4000 };
static const u8 *sImg[4][4];
static int sNImg[4];
static const u8 *sPal[8][3];
static int sNPal[8];
static const u32 kBankSize[9] = {0x20000, 0x20000, 0x20000, 0x20000, 0x10000, 0x4000, 0x4000, 0x8000, 0x4000};

// the slots as flat images for the frame being drawn (the banks OR-ed; unmapped slots zero)
static u8 sImgFlat[4 * IMG_SLOT];
static u8 sPalFlat[8 * PAL_SLOT];

// returns 0, doing nothing, while the VRAM's mapping generation is the one of the last call (the slots unchanged)
static int g3_tex_setup(void) {
    static u32 done = ~0u;
    if (gG3VramGeneration != ~0u && gG3VramGeneration == done) return 0;
    done = gG3VramGeneration;
    memset(sNImg, 0, sizeof sNImg);
    memset(sNPal, 0, sizeof sNPal);
    if (gG3Banks)
        for (int b = 0; b < 7; b++) {
            u8 c = gG3VramCnt[b];
            if (!(c & 0x80) || (c & 7) != 3 || !gG3Banks[b]) continue;
            int ofs = c >> 3 & 3;
            if (b < 4) sImg[ofs][sNImg[ofs]++] = gG3Banks[b];
            else if (b == 4)
                for (int s = 0; s < 4; s++) sPal[s][sNPal[s]++] = gG3Banks[b] + s * PAL_SLOT;
            else {
                int s = (ofs & 1) + (ofs >> 1) * 4;
                sPal[s][sNPal[s]++] = gG3Banks[b];
            }
        }
    for (int s = 0; s < 4; s++) {
        u8 *d = sImgFlat + s * IMG_SLOT;
        if (!sNImg[s]) { memset(d, 0, IMG_SLOT); continue; }
        memcpy(d, sImg[s][0], IMG_SLOT);
        for (int i = 1; i < sNImg[s]; i++)
            for (u32 k = 0; k < IMG_SLOT; k++) d[k] |= sImg[s][i][k];
    }
    for (int s = 0; s < 8; s++) {
        u8 *d = sPalFlat + s * PAL_SLOT;
        if (!sNPal[s]) { memset(d, 0, PAL_SLOT); continue; }
        memcpy(d, sPal[s][0], PAL_SLOT);
        for (int i = 1; i < sNPal[s]; i++)
            for (u32 k = 0; k < PAL_SLOT; k++) d[k] |= sPal[s][i][k];
    }
    (void)kBankSize;
    return 1;
}

static inline u8 img8(u32 a) { return sImgFlat[a & 0x7ffff]; }
static inline u16 img16(u32 a) { return (u16)(img8(a) | img8(a + 1) << 8); }
static inline u32 img32(u32 a) { return (u32)img16(a) | (u32)img16(a + 2) << 16; }
static inline u16 pal16(u32 a) {
    a &= 0x1ffff;
    return (u16)(sPalFlat[a] | sPalFlat[(a + 1) & 0x1ffff] << 8);
}

// a coordinate (whole texels) into the texture: repeated (flipped every other time) or clamped
static inline int wrap(int c, int size, int repeat, int flip) {
    if (!repeat) return c < 0 ? 0 : c >= size ? size - 1 : c;
    if (flip && (c & size)) return size - 1 - (c & (size - 1));
    return c & (size - 1);
}

// the texel at texture coordinates (s, t) (whole texels) of a polygon's texture: BGR555 in bits 0-14, alpha 0-31
// in bits 16-20 (0: transparent)
static inline u32 g3_texel(u32 texparam, u32 texpal, int s, int t) {
    int fmt = texparam >> 26 & 7;
    int w = 8 << (texparam >> 20 & 7), h = 8 << (texparam >> 23 & 7);
    s = wrap(s, w, texparam >> 16 & 1, texparam >> 18 & 1);
    t = wrap(t, h, texparam >> 17 & 1, texparam >> 19 & 1);
    u32 base = (texparam & 0xffff) << 3;
    u32 palbase = (texpal & 0x1fff) << 4;
    int zero = texparam >> 29 & 1;  // (palette colour 0 transparent)
    u32 i = (u32)(t * w + s);
    switch (fmt) {
    case 1: {  // A3I5
        u8 v = img8(base + i);
        u32 a = v >> 5;
        return (pal16(palbase + (v & 31) * 2) & 0x7fffu) | ((a << 2) + (a >> 1)) << 16;
    }
    case 2: {  // 4 colours (the palette base in steps of 8 bytes)
        u32 c = img8(base + (i >> 2)) >> ((i & 3) * 2) & 3;
        if (zero && !c) return 0;
        return (pal16(((texpal & 0x1fff) << 3) + c * 2) & 0x7fffu) | 31u << 16;
    }
    case 3: {  // 16 colours
        u32 c = img8(base + (i >> 1)) >> ((i & 1) * 4) & 15;
        if (zero && !c) return 0;
        return (pal16(palbase + c * 2) & 0x7fffu) | 31u << 16;
    }
    case 4: {  // 256 colours
        u32 c = img8(base + i);
        if (zero && !c) return 0;
        return (pal16(palbase + c * 2) & 0x7fffu) | 31u << 16;
    }
    case 5: {  // 4x4 texel blocks: 2 bits per texel in slot 0 or 2, the block's palette word in slot 1
        u32 block = (u32)((t >> 2) * (w >> 2) + (s >> 2));
        u32 addr = base + block * 4;
        u32 bits = img32(addr);
        u32 c = bits >> (((t & 3) * 4 + (s & 3)) * 2) & 3;
        u32 slot1 = 0x20000 + ((addr & 0x1ffff) >> 1) + ((addr >> 17) == 2 ? 0x10000 : 0);
        u16 info = img16(slot1);
        u32 pb = palbase + (u32)(info & 0x3fff) * 4;
        int mode = info >> 14;
        u16 c0 = pal16(pb), c1 = pal16(pb + 2);
        if (c == 3 && mode < 2) return 0;
        if (c < 2 || mode == 2) return (pal16(pb + c * 2) & 0x7fffu) | 31u << 16;
        u32 r0 = c0 & 31, g0 = c0 >> 5 & 31, b0 = c0 >> 10 & 31, r1 = c1 & 31, g1 = c1 >> 5 & 31, b1 = c1 >> 10 & 31;
        u32 r, g, b;
        if (mode == 1) {  // (c == 2) the average
            r = (r0 + r1) / 2, g = (g0 + g1) / 2, b = (b0 + b1) / 2;
        } else if (c == 2) {
            r = (r0 * 5 + r1 * 3) / 8, g = (g0 * 5 + g1 * 3) / 8, b = (b0 * 5 + b1 * 3) / 8;
        } else {
            r = (r0 * 3 + r1 * 5) / 8, g = (g0 * 3 + g1 * 5) / 8, b = (b0 * 3 + b1 * 5) / 8;
        }
        return r | g << 5 | b << 10 | 31u << 16;
    }
    case 6: {  // A5I3
        u8 v = img8(base + i);
        return (pal16(palbase + (v & 7) * 2) & 0x7fffu) | (u32)(v >> 3) << 16;
    }
    case 7: {  // direct colour, bit 15 opaque
        u16 v = img16(base + i * 2);
        return (v & 0x8000) ? (v & 0x7fffu) | 31u << 16 : 0;
    }
    default: return 31u << 16;  // (no texture)
    }
}

#endif

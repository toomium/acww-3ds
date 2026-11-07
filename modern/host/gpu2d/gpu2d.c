// The 2D renderer (gpu2d.h): registers, VRAM mapping, the frame and line loop, windows, the compositing of the
// layers with the colour special effects, display modes and master brightness.
//
// Colours: the layers' pixels are BGR555 (the 3D layer's are 6 bits a channel); the compositing works in 6 bits a
// channel (a 5-bit channel c is 2c), as the DS's display path does, and the screens get 8 bits a channel (6-bit c is
// c << 2 | c >> 4).

#include <stdio.h>

#include "gpu2d_internal.h"

static Engine sEngine[2];
static u16 sPowCnt;       // POWCNT1
static u8 sVramCnt[9];    // VRAMCNT_A-I
static u32 sScreen[2][SCREEN_W * SCREEN_H];
static u32 sFrame;        // frames rendered
static int sDefer3d;      // the 3D layer left to the caller (Gpu2dFrameInput.defer3d)
static int sGpu;          // Gpu2dFrameInput.gpu: 1 record the frame for a GPU, 2 and draw it here too
static Gpu2dLayers *sLayers;  // Gpu2dFrameInput.layers
static int sDeferLayers;  // Gpu2dFrameInput.deferLayers (with sLayers)
static Gpu2dGpuFrame sGpuFrame;
static u8 sPalettes[GPU2D_MAX_VERSIONS][0x800], sOams[GPU2D_MAX_VERSIONS][0x800];
static u32 sDeferred[2][SCREEN_W * SCREEN_H * 2], sDeferredLines[2][SCREEN_H];  // (gpu2d_deferred)
static int sDiagnostics;  // keep what gpu2d_describe_pixel reports

void gpu2d_set_diagnostics(int on) { sDiagnostics = on; }

// ---- features seen: the frame each was first seen in, + 1 (0: not yet)
static u32 sFeatureFrame[2][F_COUNT];
static const char *const kFeatureNames[F_COUNT] = {
    "BG mode 0", "BG mode 1", "BG mode 2", "BG mode 3", "BG mode 4", "BG mode 5", "BG mode 6", "BG mode 7",
    "3D layer", "text BG 16 colours", "text BG 256 colours", "text BG extended palette", "affine BG",
    "extended BG 16-bit tiles", "extended BG tiles, extended palette", "extended BG 256-colour bitmap",
    "extended BG direct-colour bitmap", "large bitmap BG", "BG mosaic", "OBJ", "affine OBJ", "double-size OBJ",
    "256-colour OBJ", "OBJ extended palette", "bitmap OBJ", "semi-transparent OBJ", "OBJ window", "OBJ mosaic",
    "OBJ 2D tile mapping", "window 0", "window 1", "OBJ window enabled", "alpha blending", "brightness up",
    "brightness down", "master brightness up", "master brightness down", "forced blank", "display off",
    "VRAM display", "main memory display (not shown)", "display capture (not done)", "engine B off (POWCNT1)",
    "engine A off (POWCNT1)",
};
void gpu2d_feature(const Engine *e, enum Feature f) {
    if (!sFeatureFrame[e->num][f]) sFeatureFrame[e->num][f] = sFrame + 1;
}
void gpu2d_print_features(void) {
    for (int n = 0; n < 2; n++)
        for (int f = 0; f < F_COUNT; f++)
            if (sFeatureFrame[n][f])
                fprintf(stderr, "gpu2d: engine %c: %s (first in frame %u)\n", 'A' + n, kFeatureNames[f],
                        sFeatureFrame[n][f] - 1);
}

// ---- registers
void gpu2d_reset(void) {
    memset(sEngine, 0, sizeof sEngine);
    sEngine[1].num = 1;
    sPowCnt = 0;
    memset(sVramCnt, 0, sizeof sVramCnt);
    memset(sScreen, 0, sizeof sScreen);
    sFrame = 0;
}

// a write to BGxX / BGxY (BG2: 0x28-0x2f, BG3: 0x38-0x3f) reloads the internal reference point for the next line
static void engine_write8(Engine *e, u32 off, u8 value) {
    e->regs[off] = value;
    if ((off & ~0x17u) == 0x28) e->refWritten[off >> 4 & 1] |= (u8)(1 << (off >> 2 & 1));
}
// (a pointer the compiler can't see through: it would otherwise fold `regs + (addr - 0x04000000)` into the
// constant `regs - 0x04000000` plus addr, an address outside the program, which a 3DS program's relocations
// (.3dsx) can't express)
static void *opaque(void *p) {
    __asm__("" : "+r"(p));
    return p;
}
void gpu2d_write8(uint32_t addr, uint8_t value) {
    if (addr >= 0x04000000 && addr < 0x04000000 + REG_SIZE) engine_write8(opaque(&sEngine[0]), addr - 0x04000000, value);
    else if (addr >= 0x04001000 && addr < 0x04001000 + REG_SIZE)
        engine_write8(opaque(&sEngine[1]), addr - 0x04001000, value);
    else if (addr == 0x04000304) sPowCnt = (u16)((sPowCnt & 0xff00) | value);
    else if (addr == 0x04000305) sPowCnt = (u16)((sPowCnt & 0x00ff) | value << 8);
    else if (addr >= 0x04000240 && addr <= 0x04000246) ((u8 *)opaque(sVramCnt))[addr - 0x04000240] = value;
    else if (addr == 0x04000248 || addr == 0x04000249) ((u8 *)opaque(sVramCnt))[addr - 0x04000248 + 7] = value;
}
void gpu2d_write16(uint32_t addr, uint16_t value) {
    gpu2d_write8(addr, (u8)value);
    gpu2d_write8(addr + 1, (u8)(value >> 8));
}
void gpu2d_write32(uint32_t addr, uint32_t value) {
    gpu2d_write16(addr, (u16)value);
    gpu2d_write16(addr + 2, (u16)(value >> 16));
}
uint32_t gpu2d_read32(uint32_t addr) {
    if (addr >= 0x04000000 && addr + 4 <= 0x04000000 + REG_SIZE) return reg32(opaque(&sEngine[0]), addr - 0x04000000);
    if (addr >= 0x04001000 && addr + 4 <= 0x04001000 + REG_SIZE) return reg32(opaque(&sEngine[1]), addr - 0x04001000);
    if (addr == 0x04000304) return sPowCnt;
    if (addr == 0x04000240) return sVramCnt[0] | sVramCnt[1] << 8 | sVramCnt[2] << 16 | (u32)sVramCnt[3] << 24;
    if (addr == 0x04000244) return sVramCnt[4] | sVramCnt[5] << 8 | sVramCnt[6] << 16;
    if (addr == 0x04000248) return sVramCnt[7] | sVramCnt[8] << 8;
    return 0;
}

// ---- VRAM: which bank shows where (VRAMCNT: bit 7 enable, bits 0-2 MST, bits 3-4 offset)
//   bank  MST 0  1                      2                 3          4               5
//   A, B  LCDC   A BG 128K*ofs          A OBJ 128K*ofs.0  (texture)
//   C     LCDC   A BG 128K*ofs          (ARM7)            (texture)  B BG
//   D     LCDC   A BG 128K*ofs          (ARM7)            (texture)  B OBJ
//   E     LCDC   A BG 0                 A OBJ 0           (tex pal)  A BG ext pal
//   F, G  LCDC   A BG 16K*ofs.0+64K*ofs.1, A OBJ the same (tex pal)  A BG ext pal slots 2*ofs.0+  A OBJ ext pal
//   H     LCDC   B BG 0                 B BG ext pal
//   I     LCDC   B BG 0x8000            B OBJ 0           B OBJ ext pal
// (A and B, H and I have 2-bit MSTs.) The small banks repeat in the areas: F and G 32 KB further on, H (B BG) 64 KB
// further on, I (B BG) at 0x8000, 0xc000, 0x18000 and 0x1c000, I (B OBJ) all over the 128 KB. Where banks overlap, the engine reads what they hold OR-ed
// together.
static const u32 kBankSize[9] = {0x20000, 0x20000, 0x20000, 0x20000, 0x10000, 0x4000, 0x4000, 0x8000, 0x4000};
static const u8 kZeroGranule[GRANULE];
enum { AREA_A_BG, AREA_A_OBJ, AREA_B_BG, AREA_B_OBJ, AREA_A_BGEXT, AREA_A_OBJEXT, AREA_B_BGEXT, AREA_B_OBJEXT,
       AREA_NONE };
static u8 sOverlap[32 + 16 + 8 + 8 + 10][GRANULE];  // composites of overlapping banks (one per granule or slot)
static int sOverlapUsed;

// gran[i] |= src (one 16 KB granule, or 8 KB for an extended palette slot)
static void map_piece(const u8 **slot, const u8 *src, u32 size) {
    if (!*slot) { *slot = src; return; }
    if (sOverlapUsed == (int)(sizeof sOverlap / sizeof sOverlap[0])) return;
    u8 *c = sOverlap[sOverlapUsed++];
    for (u32 i = 0; i < size; i++) c[i] = (*slot)[i] | src[i];
    *slot = c;
}

// the places a bank shows again at, after its first (offsets from it; 0 ends the list)
static const u32 kMirrorFG[] = {0x8000, 0}, kMirrorH[] = {0x10000, 0}, kMirrorIBg[] = {0x4000, 0x10000, 0x14000, 0},
                 kMirrorIObj[] = {0x4000, 0x8000, 0xc000, 0x10000, 0x14000, 0x18000, 0x1c000, 0}, kMirrorNone[] = {0};

// where bank i shows under control value c: the area, the offset in it, and its repeats
static int bank_target(int i, u8 c, u32 *ofs, const u32 **mirrors) {
    if (!(c & 0x80)) return AREA_NONE;
    u32 o = c >> 3 & 3, mst = c & ((i < 2 || i > 6) ? 3 : 7);
    *ofs = 0;
    *mirrors = kMirrorNone;
    if ((i == 5 || i == 6) && (mst == 1 || mst == 2)) *mirrors = kMirrorFG;
    else if (i == 7 && mst == 1) *mirrors = kMirrorH;
    else if (i == 8 && mst == 1) *mirrors = kMirrorIBg;
    else if (i == 8 && mst == 2) *mirrors = kMirrorIObj;
    switch (i) {
    case 0: case 1:
        if (mst == 1) { *ofs = 0x20000 * o; return AREA_A_BG; }
        if (mst == 2) { *ofs = 0x20000 * (o & 1); return AREA_A_OBJ; }
        break;
    case 2: case 3:
        if (mst == 1) { *ofs = 0x20000 * o; return AREA_A_BG; }
        if (mst == 4) return i == 2 ? AREA_B_BG : AREA_B_OBJ;
        break;
    case 4:
        if (mst == 1) return AREA_A_BG;
        if (mst == 2) return AREA_A_OBJ;
        if (mst == 4) return AREA_A_BGEXT;
        break;
    case 5: case 6:
        if (mst == 1 || mst == 2) { *ofs = 0x4000 * (o & 1) + 0x10000 * (o >> 1); return mst == 1 ? AREA_A_BG : AREA_A_OBJ; }
        if (mst == 4) { *ofs = 0x4000 * (o & 1); return AREA_A_BGEXT; }
        if (mst == 5) return AREA_A_OBJEXT;
        break;
    case 7:
        if (mst == 1) return AREA_B_BG;
        if (mst == 2) return AREA_B_BGEXT;
        break;
    case 8:
        if (mst == 1) { *ofs = 0x8000; return AREA_B_BG; }
        if (mst == 2) return AREA_B_OBJ;
        if (mst == 3) return AREA_B_OBJEXT;
        break;
    }
    return AREA_NONE;
}

static void map_vram(const Gpu2dFrameInput *in) {
    Engine *a = &sEngine[0], *b = &sEngine[1];
    const u8 *gran[4][32] = {{0}};  // A BG, A OBJ, B BG, B OBJ
    const u8 *ext[4][4] = {{0}};    // A BG ext slots, A OBJ ext, B BG ext slots, B OBJ ext
    static const u32 kAreaSize[4] = {0x80000, 0x40000, 0x20000, 0x20000};
    sOverlapUsed = 0;
    for (int i = 0; i < 9; i++) {
        u32 ofs;
        const u32 *mirrors;
        int area = bank_target(i, sVramCnt[i], &ofs, &mirrors);
        if (area == AREA_NONE) continue;
        const u8 *bank = in->bank[i];
        if (area <= AREA_B_OBJ) {
            for (int k = -1; k < 0 || mirrors[k]; k++)
                for (u32 o = 0; o < kBankSize[i]; o += GRANULE) {
                    u32 at = ofs + (k < 0 ? 0 : mirrors[k]) + o;
                    if (at < kAreaSize[area]) map_piece(&gran[area][at >> GRANULE_SHIFT], bank + o, GRANULE);
                }
        } else if (area == AREA_A_BGEXT || area == AREA_B_BGEXT) {
            // 8 KB slots: E and H hold all four (their first 32 KB), F and G two (from slot 2 * offset bit 0)
            u32 first = ofs / 0x2000, n = kBankSize[i] >= 0x8000 ? 4 : 2;
            for (u32 s = 0; s < n && first + s < 4; s++)
                map_piece(&ext[area - AREA_A_BGEXT][first + s], bank + s * 0x2000, 0x2000);
        } else {
            map_piece(&ext[area - AREA_A_BGEXT][0], bank, 0x2000);
        }
    }
    for (int n = 0; n < 32; n++) {
        a->bgGran[n] = gran[0][n] ? gran[0][n] : kZeroGranule;
        if (n < 16) a->objGran[n] = gran[1][n] ? gran[1][n] : kZeroGranule;
        if (n < 16) b->bgGran[n] = gran[2][n & 7] ? gran[2][n & 7] : kZeroGranule;
        if (n < 16) b->objGran[n] = gran[3][n & 7] ? gran[3][n & 7] : kZeroGranule;
    }
    a->bgMask = 0x7ffff; a->objMask = 0x3ffff;
    b->bgMask = 0x1ffff; b->objMask = 0x1ffff;
    for (int s = 0; s < 4; s++) { a->bgExtPal[s] = ext[0][s]; b->bgExtPal[s] = ext[2][s]; }
    a->objExtPal = ext[1][0];
    b->objExtPal = ext[3][0];
}

// bank i (A-D) if it is mapped to the LCDC (for the VRAM display), else NULL
static const u8 *lcdc_bank(const Gpu2dFrameInput *in, int i) {
    return (sVramCnt[i] & 0x87) == 0x80 ? in->bank[i] : NULL;
}

// ---- windows: per pixel the layers shown and whether effects apply (WININ / WINOUT bits 0-5)
// Each of the two rectangular windows has an "inside vertically" and an "inside horizontally" flag. At the start of
// every line (all 263, the line number taken modulo 256) the vertical flag is cleared when the line is Y2, else set
// when it is Y1; along a drawn line the horizontal flag is cleared at X2, else set at X1. The flags keep their state
// across lines and frames, so X1 > X2 or Y1 > Y2 wrap around, and a window whose coordinates change starts from the
// state the old ones left.
static void window_vertical(Engine *e, int line) {
    for (int w = 0; w < 2; w++) {
        u16 v = reg16(e, w ? REG_WIN1V : REG_WIN0V);
        if ((line & 0xff) == (v & 0xff)) e->winActive[w] &= ~1;
        else if ((line & 0xff) == v >> 8) e->winActive[w] |= 1;
    }
}
// (region: when not NULL, also the window each pixel is in: 0 outside, 1 window 0, 2 window 1, 3 the OBJ window)
static void window_line(Engine *e, const ObjLine *obj, u8 *mask, u8 *region) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    if (!(dispcnt & 0xe000)) { memset(mask, 0x3f, SCREEN_W); return; }
    u16 winin = reg16(e, REG_WININ), winout = reg16(e, REG_WINOUT);
    memset(mask, winout & 0x3f, SCREEN_W);
    if (region) memset(region, 0, SCREEN_W);
    if (dispcnt & 0x8000) {
        gpu2d_feature(e, F_WINOBJ);
        for (int x = 0; x < SCREEN_W; x++)
            if (obj->window[x]) {
                mask[x] = winout >> 8 & 0x3f;
                if (region) region[x] = 3;
            }
    }
    for (int w = 1; w >= 0; w--) {  // (window 0 over window 1)
        if (!(dispcnt & 0x2000u << w)) continue;
        gpu2d_feature(e, w ? F_WIN1 : F_WIN0);
        u16 h = reg16(e, w ? REG_WIN1H : REG_WIN0H);
        int x1 = h >> 8, x2 = h & 0xff;
        u8 in = (u8)(winin >> (8 * w) & 0x3f);
        for (int x = 0; x < SCREEN_W; x++) {
            if (x == x2) e->winActive[w] &= ~2;
            else if (x == x1) e->winActive[w] |= 2;
            if (e->winActive[w] == 3) {
                mask[x] = in;
                if (region) region[x] = (u8)(w + 1);
            }
        }
    }
}

// window_line's effect on the windows' horizontal flags, without drawing (Gpu2dFrameInput.gpu): along the line the
// flag is cleared at X2, else set at X1, so at its end the later of the two decides (X2 at the same place)
static void window_advance(Engine *e) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    if (!(dispcnt & 0xe000)) return;
    for (int w = 0; w < 2; w++) {
        if (!(dispcnt & 0x2000u << w)) continue;
        u16 h = reg16(e, w ? REG_WIN1H : REG_WIN0H);
        if ((h & 0xff) >= h >> 8) e->winActive[w] &= ~2;
        else e->winActive[w] |= 2;
    }
}

// ---- colours (6 bits a channel: r | g << 8 | b << 16)
static inline u32 rgb6(u16 c) {
    return (u32)(c & 0x1f) << 1 | (u32)(c >> 5 & 0x1f) << 9 | (u32)(c >> 10 & 0x1f) << 17;
}
static inline u32 chan(u32 c, int i) { return c >> (8 * i) & 0x3f; }
static inline u32 pack(u32 r, u32 g, u32 b) { return r | g << 8 | b << 16; }
// alpha blending: (a * eva + b * evb) / 16, rounded, at most 63
static u32 blend(u32 a, u32 b, u32 eva, u32 evb) {
    u32 o[3];
    for (int i = 0; i < 3; i++) {
        u32 v = (chan(a, i) * eva + chan(b, i) * evb + 8) >> 4;
        o[i] = v > 63 ? 63 : v;
    }
    return pack(o[0], o[1], o[2]);
}
// the 3D layer over a 2nd target: by the 3D pixel's alpha (0-31), (a * (alpha + 1) + b * (31 - alpha)) / 32, rounded
static u32 blend3d(u32 a, u32 b, u32 alpha) {
    u32 eva = alpha + 1, evb = 31 - alpha, o[3];
    for (int i = 0; i < 3; i++) o[i] = (chan(a, i) * eva + chan(b, i) * evb + 16) >> 5;
    return pack(o[0], o[1], o[2]);
}
// brightness up and down by ev / 16, with the rounding bias of BLDY (8, 7) or MASTER_BRIGHT (0, 15)
static u32 brighten(u32 c, u32 ev, u32 bias) {
    u32 o[3];
    for (int i = 0; i < 3; i++) o[i] = chan(c, i) + (((63 - chan(c, i)) * ev + bias) >> 4);
    return pack(o[0], o[1], o[2]);
}
static u32 darken(u32 c, u32 ev, u32 bias) {
    u32 o[3];
    for (int i = 0; i < 3; i++) o[i] = chan(c, i) - ((chan(c, i) * ev + bias) >> 4);
    return pack(o[0], o[1], o[2]);
}

// ---- OBJ mosaic
// horizontally, when a line is composited: an OBJ mosaic pixel next to one shows that one's colour, but for the
// first pixel of each block (MOSAIC bits 8-11 + 1 pixels wide from the screen's left)
static void obj_mosaic_x(const Engine *e, ObjLine *obj) {
    int w = (reg16(e, REG_MOSAIC) >> 8 & 15) + 1;
    if (w == 1) return;
    int last = 0;
    for (int x = 1; x < SCREEN_W; x++) {
        if (!(obj->mosaic[last] && obj->mosaic[x]) || x % w == 0) { last = x; continue; }
        obj->value[x] = obj->value[last];
        obj->source[x] = obj->source[last];
        obj->prio[x] = obj->prio[last];
        obj->kind[x] = obj->kind[last];
        obj->alpha[x] = obj->alpha[last];
        obj->index[x] = obj->index[last];
    }
}
// vertically, after each line: every MOSAIC bits 12-15 + 1 lines, the next line becomes the one mosaic OBJs show
static void obj_mosaic_y(Engine *e, int line) {
    int h = reg16(e, REG_MOSAIC) >> 12 & 15;
    if (e->objMosaicCount == h) { e->objMosaicCount = 0; e->objMosaicLine = line + 1; }
    else e->objMosaicCount = (e->objMosaicCount + 1) & 15;
}

// ---- compositing one line of an engine's BG and OBJ layers
// an OBJ pixel's BGR555 colour: the palettes as DISPCNT bit 31 selects now (a pixel prepared for the extended
// palette shows its low 8 bits in the standard palette when it is off)
static u16 obj_color(const Engine *e, u32 dispcnt, const ObjLine *obj, int x) {
    switch (obj->source[x]) {
    case OBJ_SRC_DIRECT: return obj->value[x];
    case OBJ_SRC_EXTPAL:
        if (dispcnt & 1u << 31) return e->objExtPal ? pal16(e->objExtPal, obj->value[x] & 0xfffu) : 0;
        /* fall through */
    case OBJ_SRC_PALETTE: return pal16(e->palette + 0x200, obj->value[x] & 0xffu);
    }
    return 0;
}

enum { DBG_3D = 6 };  // (dbgTop / dbgBelow: LAYER_* or the 3D layer)
enum { EFF_NONE, EFF_ALPHA, EFF_UP, EFF_DOWN, EFF_SEMI_OBJ, EFF_BITMAP_OBJ, EFF_3D };

// gpu2d.h gpu2d_deferred's word 1 for a pixel where the 3D layer could show: `at` 0 if it would be the top layer, 1
// if one is in front of it (col0 / lay0: the front layer, else the layer below it - the top layer when the 3D layer's
// pixel is transparent)
static inline u32 pack18(u32 c) { return (c & 0x3f) | (c >> 8 & 0x3f) << 6 | (c >> 16 & 0x3f) << 12; }
static u32 deferred_3d(const ObjLine *obj, int x, int at, u32 col0, u8 lay0, int effOk, u32 first, u32 second,
                       u32 effect, u32 eva, u32 evb, u32 evy) {
    enum { KIND_C = 1u << 30, KIND_BLEND = 2u << 30, KIND_TOP = 3u << 30 };
    int below3d = second & 1;  // (the 3D layer is BG0)
    if (at == 1) {
        if (lay0 == LAYER_OBJ && obj->kind[x] != OBJ_NORMAL) {
            if (!below3d) return KIND_C | pack18(col0);
            u32 a = obj->kind[x] == OBJ_BITMAP ? obj->alpha[x] + 1u : eva, b = obj->kind[x] == OBJ_BITMAP ? 15u - obj->alpha[x] : evb;
            return KIND_BLEND | pack18(col0) | a << 18 | b << 23;
        }
        if (effOk && ((first >> lay0) & 1)) {
            if (effect == 1 && below3d) return KIND_BLEND | pack18(col0) | eva << 18 | evb << 23;
            if (effect == 2) return KIND_C | pack18(brighten(col0, evy, 8));
            if (effect == 3) return KIND_C | pack18(darken(col0, evy, 7));
        }
        return KIND_C | pack18(col0);
    }
    if ((second >> lay0) & 1) return KIND_TOP | pack18(col0) | 1u << 28;
    if (effOk && (first & 1) && (effect == 2 || effect == 3)) return KIND_TOP | evy << 18 | effect << 28;
    return KIND_TOP;
}

static void compose_line(Engine *e, int line, const u32 *line3d, u32 *dst) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    const ObjLine *obj = &e->obj;
    u8 mask[SCREEN_W];
    window_line(e, obj, mask, NULL);
    obj_mosaic_x(e, &e->obj);
    int has3d = e->num == 0 && (dispcnt & 8) && (dispcnt & 0x100) && line3d;
    // the 3D layer deferred: composited as if its pixels were transparent, and where one could show, how it would
    int defer3d = e->num == 0 && (dispcnt & 8) && (dispcnt & 0x100) && sDefer3d;
    if (has3d || defer3d) gpu2d_feature(e, F_3D);
    u32 *defer = e->defer + line * SCREEN_W;
    s32 hofs3d = reg16(e, REG_BGHOFS) & 0x1ff;
    if (hofs3d & 0x100) hofs3d -= 0x200;
    // the layers front to back: at each priority the OBJs, then BG0-3 (OBJ_ORDER stands for the OBJs, whose
    // priority is per pixel)
    enum { OBJ_ORDER = 4 + 4 };
    u8 order[4 * 5];
    int layers = 0;
    for (u32 p = 0; p < 4; p++) {
        if (dispcnt & 0x1000) order[layers++] = (u8)(OBJ_ORDER + p);
        for (int n = 0; n < 4; n++)
            if ((reg16(e, REG_BGCNT + 2 * n) & 3) == p && (dispcnt & 0x100u << n)) order[layers++] = (u8)n;
    }
    u16 bldcnt = reg16(e, REG_BLDCNT), bldalpha = reg16(e, REG_BLDALPHA);
    u32 effect = bldcnt >> 6 & 3, first = bldcnt & 0x3f, second = bldcnt >> 8 & 0x3f;
    u32 eva = bldalpha & 0x1f, evb = bldalpha >> 8 & 0x1f, evy = reg16(e, REG_BLDY) & 0x1f;
    if (eva > 16) eva = 16;
    if (evb > 16) evb = 16;
    if (evy > 16) evy = 16;
    u32 backdrop = rgb6(pal16(e->palette, 0));
    u32 effectsUsed = 0;  // (1 << EFF_*)
    for (int x = 0; x < SCREEN_W; x++) {
        // the two front layers at x (the backdrop behind all)
        u32 col[2] = {backdrop, backdrop};
        u8 lay[2] = {LAYER_BACKDROP, LAYER_BACKDROP};  // (the 3D layer: LAYER_BG0 with is3d)
        int is3d[2] = {0, 0}, found = 0, at3d = -1;
        u32 alpha3d = 0;
        u8 m = mask[x];
        for (int k = 0; k < layers && found < 2; k++) {
            int n = order[k];
            if (n >= OBJ_ORDER) {
                if ((m & 0x10) && obj->source[x] && obj->prio[x] == n - OBJ_ORDER) {
                    col[found] = rgb6(obj_color(e, dispcnt, obj, x));
                    lay[found++] = LAYER_OBJ;
                }
            } else if (!(m & 1 << n)) {
                continue;
            } else if (n == 0 && defer3d) {
                s32 sx = x + hofs3d;
                if (sx >= 0 && sx < SCREEN_W && at3d < 0) at3d = found;
            } else if (n == 0 && has3d) {
                s32 sx = x + hofs3d;
                if (sx < 0 || sx >= SCREEN_W) continue;
                u32 c = line3d[sx];
                if (!(c >> 24 & 0x1f)) continue;
                if (!found) alpha3d = c >> 24 & 0x1f;
                is3d[found] = 1;
                col[found] = c & 0x3f3f3f;
                lay[found++] = LAYER_BG0;
            } else if (e->bg[n][x] & PIX_OPAQUE) {
                col[found] = rgb6(e->bg[n][x]);
                lay[found++] = (u8)n;
            }
        }
        if (!found) lay[1] = LAYER_NONE;  // (the backdrop is the top: nothing below it to blend with)
        // the colour special effect: semi-transparent and bitmap OBJs and the 3D layer blend with a 2nd target below
        // them (BLDCNT bits 8-13) whatever the effect, others as BLDCNT says where the window's bit 5 enables it
        u32 out = col[0], eff = EFF_NONE;
        int effOk = m & 0x20, below2nd = (second >> lay[1]) & 1;
        if (lay[0] == LAYER_OBJ && obj->kind[x] != OBJ_NORMAL && below2nd) {
            if (obj->kind[x] == OBJ_BITMAP) {
                out = blend(col[0], col[1], obj->alpha[x] + 1u, 15u - obj->alpha[x]);
                eff = EFF_BITMAP_OBJ;
            } else {
                out = blend(col[0], col[1], eva, evb);
                eff = EFF_SEMI_OBJ;
            }
        } else if (is3d[0] && below2nd) {
            out = blend3d(col[0], col[1], alpha3d);
            eff = EFF_3D;
        } else if (effOk && ((first >> lay[0]) & 1)) {
            if (effect == 1 && below2nd) { out = blend(col[0], col[1], eva, evb); eff = EFF_ALPHA; }
            else if (effect == 2) { out = brighten(col[0], evy, 8); eff = EFF_UP; }
            else if (effect == 3) { out = darken(col[0], evy, 7); eff = EFF_DOWN; }
        }
        effectsUsed |= 1u << eff;
        dst[x] = out;
        if (defer3d) defer[x] = at3d < 0 ? 0 : deferred_3d(obj, x, at3d, col[0], lay[0], effOk, first, second, effect,
                                                         eva, evb, evy);
        if (sDiagnostics) {
            int at = line * SCREEN_W + x;
            e->dbgTop[at] = is3d[0] ? DBG_3D : lay[0];
            e->dbgBelow[at] = is3d[1] ? DBG_3D : lay[1];
            e->dbgEffect[at] = (u8)eff;
            e->dbgObj[at] = obj->index[x];
            e->dbgAlpha[at] = (u8)(is3d[0] ? alpha3d : obj->alpha[x]);
            e->dbgTopColor[at] = col[0];
            e->dbgBelowColor[at] = col[1];
        }
    }
    if (effectsUsed & (1u << EFF_ALPHA | 1u << EFF_SEMI_OBJ | 1u << EFF_BITMAP_OBJ | 1u << EFF_3D)) gpu2d_feature(e, F_ALPHA);
    if (effectsUsed & 1u << EFF_UP) gpu2d_feature(e, F_BRIGHT_UP);
    if (effectsUsed & 1u << EFF_DOWN) gpu2d_feature(e, F_BRIGHT_DOWN);
}

// one line of an engine's picture: the display mode (DISPCNT bits 16-17), then master brightness
// ---- the layers to the caller (Gpu2dFrameInput.layers): a line's BG and OBJ pixels in the 3DS GPU's formats, the
// windows' layer masks applied, and the line's compositing state
static u16 sTo5551[0x8000];
static void layers_line(Engine *e, int line, Gpu2dLineState *ls) {
    u32 dispcnt = reg32(e, REG_DISPCNT);
    if (!sTo5551[1])
        for (u32 c = 0; c < 0x8000; c++) sTo5551[c] = (u16)((c & 31) << 11 | (c >> 5 & 31) << 6 | (c >> 10 & 31) << 1 | 1);
    u8 mask[SCREEN_W], region[SCREEN_W];
    if (sLayers->sprite) {  // (the windows' shapes go to the GPU: only the CPU's BGs need the mask)
        static const ObjLine kNoObj;
        int cpuBg = 0;
        for (int n = 0; n < 4; n++) cpuBg |= (dispcnt & 0x100u << n) && !(ls->gpuBg >> n & 1) && !(n == 0 && (dispcnt & 8) && e->num == 0);
        if (cpuBg) window_line(e, &kNoObj, mask, NULL);
        else window_advance(e);
    } else {
        window_line(e, &e->obj, mask, region);
        obj_mosaic_x(e, &e->obj);
    }
    int is3d = e->num == 0 && (dispcnt & 8);
    if (is3d && (dispcnt & 0x100)) gpu2d_feature(e, F_3D);
    for (int n = 0; n < 4; n++) {
        ls->prio[n] = reg16(e, REG_BGCNT + 2 * (u32)n) & 3;
        if (!(dispcnt & 0x100u << n) || (n == 0 && is3d) || (ls->gpuBg >> n & 1)) continue;
        u16 *d = sLayers->bg[e->num][n] + line * SCREEN_W;
        const u16 *s = e->bg[n];
        for (int x = 0; x < SCREEN_W; x++)
            d[x] = (s[x] & PIX_OPAQUE) && (mask[x] >> n & 1) ? sTo5551[s[x] & 0x7fff] : 0;
    }
    u32 prios = 0;
    if ((dispcnt & 0x1000) && !sLayers->sprite) {  // (sprites: gpu2d_draw_layers)
        u32 *d = sLayers->obj[e->num] + line * SCREEN_W;
        const ObjLine *obj = &e->obj;
        for (int x = 0; x < SCREEN_W; x++) {
            if (!obj->source[x] || !(mask[x] & 0x10)) { d[x] = 0; continue; }
            prios |= 1u << obj->prio[x];
            u16 c = obj_color(e, dispcnt, obj, x);
            u32 r = c & 31, g = c >> 5 & 31, b = c >> 10 & 31;
            d[x] = (r << 3 | r >> 2) << 24 | (g << 3 | g >> 2) << 16 | (b << 3 | b >> 2) << 8 | (obj->prio[x] + 1u) * 32;
        }
    }
    sLayers->objPrio[e->num][line] = (u8)prios;
    if ((dispcnt & 0xe000) && !sLayers->sprite) {
        u32 *d = sLayers->region[e->num] + line * SCREEN_W;
        for (int x = 0; x < SCREEN_W; x++) d[x] = 63u + 64u * region[x];
    }
}

static void render_line(Engine *e, const Gpu2dFrameInput *in, int line) {
    u32 dispcnt = reg32(e, REG_DISPCNT), *dst = e->out + line * SCREEN_W;
    u16 powcnt = e->powcnt;
    if (sDeferLayers && in) {  // (recorded for gpu2d_draw_layers, which draws with in NULL: the state the drawing
                               // below changes, as the GPU's)
        if (!(e->num == 1 && !(powcnt & 0x200)) && !(dispcnt & 0x80)) window_advance(e);
        obj_mosaic_y(e, line);
        return;
    }
    if (sLayers) {  // (the caller composites: the layers and the state)
        Gpu2dLineState *ls = &sLayers->line[e->num][line];
        s32 hofs3d = reg16(e, REG_BGHOFS) & 0x1ff;
        ls->dispcnt = dispcnt;
        ls->bldcnt = reg16(e, REG_BLDCNT), ls->bldalpha = reg16(e, REG_BLDALPHA), ls->bldy = reg16(e, REG_BLDY);
        ls->backdrop = pal16(e->palette, 0) & 0x7fff;
        ls->masterBright = reg16(e, REG_MASTER_BRIGHT);
        ls->hofs3d = (int16_t)(hofs3d & 0x100 ? hofs3d - 0x200 : hofs3d);
        u32 mode = dispcnt >> 16 & (e->num ? 1 : 3);
        ls->white = (e->num == 1 && !(powcnt & 0x200)) || (dispcnt & 0x80) || mode != 1;
        if (mode == 0) gpu2d_feature(e, F_DISPLAY_OFF);
        if (mode >= 2) gpu2d_feature(e, mode == 2 ? F_DISPLAY_VRAM : F_DISPLAY_FIFO);
        if (dispcnt & 0x80) gpu2d_feature(e, F_FORCED_BLANK);
        ls->winin = reg16(e, REG_WININ), ls->winout = reg16(e, REG_WINOUT);
        if (sLayers->sprite) {
            ls->winh[0] = reg16(e, REG_WIN0H), ls->winh[1] = reg16(e, REG_WIN1H);
            ls->winV = (u8)((e->winActive[0] & 1) | (e->winActive[1] & 1) << 1);
        }
        ls->gpuBg = 0;
        for (int n = 0; n < 4; n++) {
            u16 cnt = reg16(e, REG_BGCNT + 2 * (u32)n);
            ls->bgcnt[n] = cnt;
            ls->bghofs[n] = reg16(e, REG_BGHOFS + 4 * (u32)n) & 0x1ff;
            ls->bgvofs[n] = reg16(e, REG_BGVOFS + 4 * (u32)n) & 0x1ff;
            int kind = gpu2d_bg_kind(e, n);
            ls->bgKind[n] = (u8)kind;
            // (the GPU's: text BGs in the standard palette, affine BGs; no mosaic)
            if ((kind == 1 && !((cnt & 0x80) && (dispcnt & 1u << 30))) || kind == 2)
                if (!(cnt & 0x40)) ls->gpuBg |= (u8)(1 << n);
        }
        for (int i = 0; i < 2; i++) {
            u32 r = REG_BG2PA + 0x10 * (u32)i;
            for (int k = 0; k < 4; k++) ls->affine[i][k] = (int16_t)reg16(e, r + 2 * (u32)k);
            sLayers->ref[e->num][line][2 * i] = e->refX[i] + ls->affine[i][1] * (line - e->refLine);
            sLayers->ref[e->num][line][2 * i + 1] = e->refY[i] + ls->affine[i][3] * (line - e->refLine);
        }
        if (!(e->num == 1 && !(powcnt & 0x200)) && !(dispcnt & 0x80)) {
            for (int n = 0; n < 4; n++)
                if (!(ls->gpuBg >> n & 1)) gpu2d_render_bg_line(e, n, line);
            layers_line(e, line, ls);
        }
        obj_mosaic_y(e, line);
        return;
    }
    if (sGpu == 1) {  // (drawn by the GPU: only the state that goes from line to line, as below)
        e->lineInfo[line] = reg16(e, REG_MASTER_BRIGHT) | (u32)(reg16(e, REG_BGHOFS) & 0x1ff) << 16;
        if (e->num == 1 && !(powcnt & 0x200)) return;
        if (!(dispcnt & 0x80)) window_advance(e);
        obj_mosaic_y(e, line);
        return;
    }
    if (sDiagnostics) {  // (compose_line sets them)
        memset(e->dbgTop + line * SCREEN_W, LAYER_NONE, SCREEN_W);
        memset(e->dbgEffect + line * SCREEN_W, EFF_NONE, SCREEN_W);
    }
    u32 *defer = e->defer + line * SCREEN_W;
    if (sDefer3d) {  // (compose_line sets them where the 3D layer can show)
        memset(defer, 0, SCREEN_W * sizeof *defer);
        e->lineInfo[line] = 0;
    }
    if (e->num == 1 && !(powcnt & 0x200)) {  // engine B switched off (POWCNT1 bit 9): white, no brightness
        gpu2d_feature(e, F_ENGINE_B_OFF);
        for (int x = 0; x < SCREEN_W; x++) dst[x] = 0x3f3f3f;
        return;
    }
    if (e->num == 0 && !(powcnt & 2)) gpu2d_feature(e, F_ENGINE_A_OFF);
    if (e->num == 0 && (reg32(e, REG_DISPCAPCNT) & 1u << 31)) gpu2d_feature(e, F_CAPTURE);
    gpu2d_feature(e, (enum Feature)(F_MODE0 + (dispcnt & 7)));
    // the BG and OBJ picture is made whatever the display mode (the windows' state advances with it)
    u32 composed[SCREEN_W];
    if (dispcnt & 0x80) {  // forced blank: white
        gpu2d_feature(e, F_FORCED_BLANK);
        for (int x = 0; x < SCREEN_W; x++) composed[x] = 0x3f3f3f;
    } else {
        for (int n = 0; n < 4; n++) gpu2d_render_bg_line(e, n, line);
        compose_line(e, line, in->lines3d ? in->lines3d + line * SCREEN_W : NULL, composed);
    }
    obj_mosaic_y(e, line);
    u32 mode = dispcnt >> 16 & (e->num ? 1 : 3);
    if (sDefer3d && mode != 1) memset(defer, 0, SCREEN_W * sizeof *defer);
    if (mode == 1) {
        memcpy(dst, composed, sizeof composed);
    } else if (mode == 0) {  // display off: white, no brightness
        gpu2d_feature(e, F_DISPLAY_OFF);
        for (int x = 0; x < SCREEN_W; x++) dst[x] = 0x3f3f3f;
        return;
    } else if (mode == 2) {  // VRAM display: bank A-D (DISPCNT bits 18-19) as direct colours, if mapped to the LCDC
        gpu2d_feature(e, F_DISPLAY_VRAM);
        const u8 *bank = lcdc_bank(in, dispcnt >> 18 & 3);
        for (int x = 0; x < SCREEN_W; x++) dst[x] = bank ? rgb6(pal16(bank, (u32)(line * SCREEN_W + x))) : 0;
    } else {  // main memory display (fed by DMA): not done
        gpu2d_feature(e, F_DISPLAY_FIFO);
        memset(dst, 0, SCREEN_W * sizeof *dst);
    }
    u16 mb = reg16(e, REG_MASTER_BRIGHT);
    u32 f = mb & 0x1f;
    if (f > 16) f = 16;
    if (sDefer3d) {  // the pixels the 3D layer can show at: their colour before master brightness, which the caller applies
        s32 hofs3d = reg16(e, REG_BGHOFS) & 0x1ff;
        e->lineInfo[line] = mb | (u32)hofs3d << 16;
        for (int x = 0; x < SCREEN_W; x++)
            if (defer[x]) composed[x] = dst[x];
    }
    if ((mb >> 14) == 1 && f) {
        gpu2d_feature(e, F_MASTER_UP);
        for (int x = 0; x < SCREEN_W; x++) dst[x] = brighten(dst[x], f, 0);
    } else if ((mb >> 14) == 2 && f) {
        gpu2d_feature(e, F_MASTER_DOWN);
        for (int x = 0; x < SCREEN_W; x++) dst[x] = darken(dst[x], f, 15);
    }
    if (sDefer3d)
        for (int x = 0; x < SCREEN_W; x++)
            if (defer[x]) dst[x] = composed[x];
}

// a BGxX / BGxY reference point: 28-bit signed
static s32 ref28(const Engine *e, u32 off) { return (s32)(reg32(e, off) << 4) >> 4; }

// ---- recording the frame for a GPU (gpu2d.h Gpu2dGpuFrame)
// where a VRAM piece the engines read is in the GPU's VRAM layout
static u32 gpu_offset(const Gpu2dFrameInput *in, const u8 *p) {
    static const u32 kOffset[9] = GPU2D_BANK_OFFSET;
    if (!p) return GPU2D_VRAM_ZERO;
    for (int i = 0; i < 9; i++)
        if (p >= in->bank[i] && p < in->bank[i] + kBankSize[i]) return kOffset[i] + (u32)(p - in->bank[i]);
    if (p >= sOverlap[0] && p < sOverlap[0] + sizeof sOverlap) return GPU2D_VRAM_OVERLAP + (u32)(p - sOverlap[0]);
    return GPU2D_VRAM_ZERO;  // (kZeroGranule)
}
static void gpu_tables(const Gpu2dFrameInput *in) {
    Gpu2dGpuFrame *f = &sGpuFrame;
    for (int n = 0; n < 2; n++) {
        const Engine *e = &sEngine[n];
        u32 *t = f->tables[n];
        memset(t, 0, 64 * sizeof *t);
        for (int k = 0; k < 32; k++) t[G2T_BG + k] = gpu_offset(in, n == 0 || k < 16 ? e->bgGran[k] : NULL);
        for (int k = 0; k < 16; k++) t[G2T_OBJ + k] = gpu_offset(in, e->objGran[k]);
        for (int k = 0; k < 4; k++) t[G2T_BGEXT + k] = gpu_offset(in, e->bgExtPal[k]);
        t[G2T_OBJEXT] = gpu_offset(in, e->objExtPal);
        for (int k = 0; k < 4; k++) t[G2T_LCDC + k] = lcdc_bank(in, k) ? gpu_offset(in, in->bank[k]) : ~0u;
    }
    f->overlap = sOverlap[0];
    f->overlapSize = (u32)sOverlapUsed * GRANULE;
}
static u32 pack_obj(const ObjLine *o, int x) {
    return o->value[x] | (u32)o->source[x] << 16 | (u32)o->prio[x] << 18 | (u32)o->kind[x] << 20 |
           (u32)o->alpha[x] << 22 | (u32)(o->mosaic[x] & 1) << 26 | (u32)(o->window[x] & 1) << 27;
}
static u32 *record_line(int n, int line);  // (deferred layers: below)
static u32 record_changes_so_far(void);
static void gpu_line_to(int n, u32 *l) {
    const Engine *e = &sEngine[n];
    memset(l, 0, GPU2D_LINE_WORDS * sizeof *l);
    for (int k = 0; k < REG_SIZE / 4; k++) l[G2L_REGS + k] = reg32(e, (u32)k * 4);
    for (int i = 0; i < 2; i++) l[G2L_REF + 2 * i] = (u32)e->refX[i], l[G2L_REF + 2 * i + 1] = (u32)e->refY[i];
    l[G2L_WIN] = (u32)(e->winActive[0] & 3) | (u32)(e->winActive[1] & 3) << 2;
    l[G2L_OBJMOSAIC] = (u32)e->objMosaicLine;
    l[G2L_PALETTE] = sGpuFrame.paletteCount - 1;
    l[G2L_OAM] = sGpuFrame.oamCount - 1;
    if (sDeferLayers) l[G2L_PALETTE] = record_changes_so_far();  // (deferred: the changes before the line)
    l[G2L_POWCNT] = sPowCnt;
}
static void gpu_line(int n, int line) { gpu_line_to(n, sDeferLayers ? record_line(n, line) : sGpuFrame.lines[n][line]); }
// after an H-blank: a new version of the palettes or OAM if it changed them
static void gpu_versions(const Gpu2dFrameInput *in) {
    Gpu2dGpuFrame *f = &sGpuFrame;
    if (memcmp(sPalettes[f->paletteCount - 1], in->palette, 0x800) && f->paletteCount < GPU2D_MAX_VERSIONS)
        memcpy(sPalettes[f->paletteCount++], in->palette, 0x800);
    if (memcmp(sOams[f->oamCount - 1], in->oam, 0x800) && f->oamCount < GPU2D_MAX_VERSIONS)
        memcpy(sOams[f->oamCount++], in->oam, 0x800);
}
const Gpu2dGpuFrame *gpu2d_gpu_frame(void) { return &sGpuFrame; }

// ---- the layers recorded and drawn later (Gpu2dFrameInput.deferLayers): each line's state as for a GPU
// (gpu_line), the palettes and OAM as at the frame's start and their words that changed in an H-blank since, in
// order (the game's sky changes a colour every line), line 0's OBJs as prepared, and the 2D banks of VRAM copied
// (with the composites of overlapping ones); gpu2d_draw_layers then draws each line with engines that take that
// state. Two records in turn: one is drawn while the next frame is recorded.
typedef struct {
    u32 lines[2][SCREEN_H][GPU2D_LINE_WORDS];
    u32 start[0x1000 / 4];        // the palettes, then OAM, at the frame's start
    u32 final[0x800 / 4];         // the palettes at its end (Gpu2dLayers.palette)
    u32 changes;
    struct { u32 index, value; } change[SCREEN_H * 0x1000 / 4];
    u8 vram[0xa4000], overlap[sizeof sOverlap];
    const u8 *gran[2][2][32], *ext[2][5];  // per engine: BG, OBJ granules; BG ext slots, OBJ ext
    u32 bgMask[2], objMask[2];
    ObjLine obj0[2];
    int haveObj0[2], topEngine;
#ifdef N3DS_DIRECT
    Gpu2dMarginAffine margin[2];
    int iris, menuWipe;
    const uint32_t *optPic;
    uint32_t optSerial;
    int optPageY, optBtnY;
    u8 objAnchor[2][128];
    Gpu2dBgAnchor bgAnchor[2];
#endif
} Record;
static Record sRecord[2];
static int sRecording;            // the record being made, or made last
static u32 sShadow[0x1000 / 4];  // the palettes and OAM as recorded up to now
int gpu2d_record(void) { return sRecording; }
static u32 *record_line(int n, int line) { return sRecord[sRecording].lines[n][line]; }
static u32 record_changes_so_far(void) { return sRecord[sRecording].changes; }

#ifdef N3DS_DIRECT
static Gpu2dMarginAffine sMargin[2];  // (given for the next frame: recorded at its start)
void gpu2d_margin_affine(int engine, int bg, int line, int pa, int32_t x, int32_t y) {
    int k = line < 0 ? line + GPU2D_MARGIN : line - SCREEN_H + GPU2D_MARGIN;
    if (engine < 0 || engine > 1 || k < 0 || k >= 2 * GPU2D_MARGIN || (line >= 0 && line < SCREEN_H)) return;
    Gpu2dMarginAffine *m = &sMargin[engine];
    m->bg = bg, m->pa[k] = (int16_t)pa, m->x[k] = x, m->y[k] = y;
}
static int sIris = -1;  // (kept until changed)
void gpu2d_iris(int progress) { sIris = progress; }
static int sMenuWipe = -1;  // (kept while engine A's window 1 is on)
void gpu2d_menu_wipe(int pos) { sMenuWipe = pos; }
static const uint32_t *sOptPic;  // (the options page: kept until changed)
static uint32_t sOptSerial;
static int sOptPageY = 192, sOptBtnY = GPU2D_NO_BTN;
static int sOptBtnNext = GPU2D_NO_BTN;
void gpu2d_options(const uint32_t *pic, uint32_t serial, int pageY) { sOptPic = pic, sOptSerial = serial, sOptPageY = pageY; }
void gpu2d_opt_button(int y) { sOptBtnNext = y; }
static u8 sObjAnchor[2][128], sObjAnchorOam[2][128][6];  // (each entry's anchor, and what it was given for)
static Gpu2dBgAnchor sBgAnchor[2];
void gpu2d_obj_anchors(int engine, const void *oam, const uint8_t *anchors) {
    if (engine < 0 || engine > 1) return;
    memcpy(sObjAnchor[engine], anchors, 128);
    for (int i = 0; i < 128; i++) memcpy(sObjAnchorOam[engine][i], (const u8 *)oam + 8 * i, 6);
    if (engine == 1) sOptBtnY = sOptBtnNext, sOptBtnNext = GPU2D_NO_BTN;  // (the options button goes with the OAM, as an OBJ)
}
void gpu2d_bg_anchor(int engine, int bgs, int windows, int anchor) {
    if (engine < 0 || engine > 1) return;
    Gpu2dBgAnchor *a = &sBgAnchor[engine];
    *a = (Gpu2dBgAnchor){(u8)bgs, (u8)windows, (u8)(bgs ? anchor : 0), {0}};
    for (int n = 0; n < 4; n++) {  // (another screen's BG with these numbers isn't moved: a menu opened meanwhile)
        u32 v = gpu2d_read32(0x04000008u + 0x1000u * (u32)engine + 4u * (u32)(n / 2));
        a->bgcnt[n] = (u16)(n & 1 ? v >> 16 : v);
    }
}
#endif
static void record_start(const Gpu2dFrameInput *in) {
    Record *r = &sRecord[sRecording ^= 1];
#ifdef N3DS_DIRECT
    memcpy(r->margin, sMargin, sizeof r->margin);
    sMargin[0].bg = sMargin[1].bg = 0;
    r->iris = sIris;
    if (!(gpu2d_read32(0x04000000) & 0x4000)) sMenuWipe = -1;
    r->menuWipe = sMenuWipe;
    r->optPic = sOptPic, r->optSerial = sOptSerial, r->optPageY = sOptPageY, r->optBtnY = sOptBtnY;
    // (an anchor only for an entry still as the game gave it: OAM loaded another way gets none)
    for (int n = 0; n < 2; n++)
        for (int i = 0; i < 128; i++)
            r->objAnchor[n][i] = sObjAnchor[n][i] && !memcmp(sObjAnchorOam[n][i], in->oam + 0x400 * n + 8 * i, 6) ? sObjAnchor[n][i] : 0;
    memcpy(r->bgAnchor, sBgAnchor, sizeof r->bgAnchor);
#endif
    memcpy(sShadow, in->palette, 0x800);
    memcpy(sShadow + 0x200, in->oam, 0x800);
    memcpy(r->start, sShadow, sizeof r->start);
    r->changes = 0;
    for (int n = 0; n < 2; n++) r->obj0[n] = sEngine[n].objNextLine0, r->haveObj0[n] = sEngine[n].haveNextLine0;
}
__attribute__((noinline)) static void record_changes(const Gpu2dFrameInput *in) {  // (its loop gets the registers)
    Record *r = &sRecord[sRecording];
#ifdef N3DS_DIRECT
    // The enhanced port: the game's only H-blank work is the sky's (Sky_HBlankMain,
    // Sky_HBlankSub), which changes the BG palettes' colour 0 (the backdrop) of both engines and registers; so only
    // those two palette words here, not the whole 4 KB of palettes and OAM after every line. shortcut: another change
    // of the palettes or OAM in an H-blank shows from the next frame on; compare all again if the game gets one.
    static const u32 kWatched[2] = {0, 0x100};
    for (int k = 0; k < 2; k++) {
        u32 i = kWatched[k], now = ((const u32 *)in->palette)[i];
        if (now != sShadow[i]) {
            sShadow[i] = now;
            r->change[r->changes].index = i, r->change[r->changes].value = now;
            r->changes++;
        }
    }
    return;
#endif
    for (u32 part = 0; part < 2; part++) {
        const u32 *now = (const u32 *)(part ? in->oam : in->palette);  // (both 0x800-aligned: host_mem.h)
        u32 *was = sShadow + part * 0x200;
        for (u32 i = 0; i < 0x200; i += 8) {
            if (!((now[i] ^ was[i]) | (now[i + 1] ^ was[i + 1]) | (now[i + 2] ^ was[i + 2]) | (now[i + 3] ^ was[i + 3]) |
                  (now[i + 4] ^ was[i + 4]) | (now[i + 5] ^ was[i + 5]) | (now[i + 6] ^ was[i + 6]) |
                  (now[i + 7] ^ was[i + 7])))
                continue;
            for (u32 k = i; k < i + 8; k++)
                if (now[k] != was[k]) {
                    was[k] = now[k];
                    r->change[r->changes].index = part * 0x200 + k, r->change[r->changes].value = now[k];
                    r->changes++;
                }
        }
    }
}
static const u8 *copied(const Gpu2dFrameInput *in, Record *r, const u8 *p) {
    static const u32 kOffset[9] = GPU2D_BANK_OFFSET;
    if (!p) return p;
    for (int i = 0; i < 9; i++)
        if (p >= in->bank[i] && p < in->bank[i] + kBankSize[i]) return r->vram + kOffset[i] + (p - in->bank[i]);
    if (p >= sOverlap[0] && p < sOverlap[0] + sizeof sOverlap) return r->overlap + (p - sOverlap[0]);
    return p;  // (kZeroGranule)
}
static void record_end(const Gpu2dFrameInput *in) {
    static const u32 kOffset[9] = GPU2D_BANK_OFFSET;
    Record *r = &sRecord[sRecording];
    for (int i = 0; i < 9; i++) {
        u32 ofs;
        const u32 *mirrors;
        if (bank_target(i, sVramCnt[i], &ofs, &mirrors) != AREA_NONE) memcpy(r->vram + kOffset[i], in->bank[i], kBankSize[i]);
    }
    memcpy(r->overlap, sOverlap, (size_t)sOverlapUsed * GRANULE);
    for (int n = 0; n < 2; n++) {
        const Engine *e = &sEngine[n];
        for (int k = 0; k < 32; k++) r->gran[n][0][k] = copied(in, r, e->bgGran[k]);
        for (int k = 0; k < 16; k++) r->gran[n][1][k] = copied(in, r, e->objGran[k]);
        for (int k = 0; k < 4; k++) r->ext[n][k] = copied(in, r, e->bgExtPal[k]);
        r->ext[n][4] = copied(in, r, e->objExtPal);
        r->bgMask[n] = e->bgMask, r->objMask[n] = e->objMask;
    }
    memcpy(r->final, sShadow, sizeof r->final);
    r->topEngine = (sPowCnt & 0x8000) ? 0 : 1;
}

void gpu2d_draw_layers(int record) {
    static Engine draw[2];  // (the drawing thread's: the state from the record line by line)
    static u32 work[0x1000 / 4];  // the palettes, then OAM, as at the line drawn
    const Record *r = &sRecord[record];
    memcpy(work, r->start, sizeof work);
    u32 applied = 0;
    for (int n = 0; n < 2; n++) {
        Engine *d = &draw[n];
        d->num = n;
        memcpy(d->bgGran, r->gran[n][0], sizeof d->bgGran);
        memcpy(d->objGran, r->gran[n][1], sizeof d->objGran);
        d->bgMask = r->bgMask[n], d->objMask = r->objMask[n];
        memcpy(d->bgExtPal, r->ext[n], sizeof d->bgExtPal);
        d->objExtPal = r->ext[n][4];
        sLayers->bgGran[n] = r->gran[n][0];
        sLayers->bgMask[n] = r->bgMask[n];
        sLayers->palette[n] = (const u8 *)r->final + 0x400 * n;
    }
    sLayers->topEngine = r->topEngine;
#ifdef N3DS_DIRECT
    memcpy(sLayers->margin, r->margin, sizeof sLayers->margin);
    sLayers->iris = r->iris;
    sLayers->menuWipe = r->menuWipe;
    sLayers->optPic = r->optPic, sLayers->optSerial = r->optSerial, sLayers->optPageY = r->optPageY, sLayers->optBtnY = r->optBtnY;
    memcpy(sLayers->bgAnchor, r->bgAnchor, sizeof sLayers->bgAnchor);
    for (int n = 0; n < 2; n++) draw[n].objAnchor = r->objAnchor[n];
#endif
    int ax = 0, ay = 0, rowH = 0;  // (the sprite atlas, filled at line 0)
    for (int line = 0; line < SCREEN_H; line++) {
        for (; applied < r->lines[0][line][G2L_PALETTE]; applied++) work[r->change[applied].index] = r->change[applied].value;
        for (int n = 0; n < 2; n++) {
            Engine *d = &draw[n];
            const u32 *l = r->lines[n][line];
            for (int k = 0; k < REG_SIZE / 4; k++) memcpy(d->regs + 4 * k, &l[G2L_REGS + k], 4);
            for (int i = 0; i < 2; i++) d->refX[i] = (s32)l[G2L_REF + 2 * i], d->refY[i] = (s32)l[G2L_REF + 2 * i + 1];
            d->refLine = line;
            d->winActive[0] = l[G2L_WIN] & 3, d->winActive[1] = l[G2L_WIN] >> 2 & 3;
            d->objMosaicLine = (int)l[G2L_OBJMOSAIC];
            d->powcnt = (u16)l[G2L_POWCNT];
            d->palette = (const u8 *)work + 0x400 * n;
            d->oam = (const u8 *)(work + 0x200) + 0x400 * n;
            if (line == 0 && sLayers->sprite)
                sLayers->nsprites[n] = gpu2d_obj_sprites(d, sLayers->sprites[n], sLayers->sprite, &ax, &ay, &rowH);
            if (sLayers->sprite) {}  // (the sprites instead)
            else if (line == 0 && r->haveObj0[n]) d->obj = r->obj0[n];
            else if (line == 0) memset(&d->obj, 0, sizeof d->obj);
            else gpu2d_render_obj_line(d, line, &d->obj);
            render_line(d, 0, line);
        }
    }
}

void gpu2d_render_frame(const Gpu2dFrameInput *in) {
    sGpu = in->gpu;
    sDefer3d = in->defer3d || sGpu;
    sLayers = in->layers;
    sDeferLayers = sLayers && in->deferLayers;
    map_vram(in);
    if (sDeferLayers) record_start(in);
    if (sGpu) {
        gpu_tables(in);
        sGpuFrame.palettes = sPalettes[0];
        sGpuFrame.oams = sOams[0];
        memcpy(sPalettes[0], in->palette, 0x800);
        memcpy(sOams[0], in->oam, 0x800);
        sGpuFrame.paletteCount = sGpuFrame.oamCount = 1;
        for (int n = 0; n < 2; n++)  // line 0's OBJs: prepared at the end of the last frame
            for (int x = 0; x < SCREEN_W; x++)
                sGpuFrame.obj0[n][x] = sEngine[n].haveNextLine0 ? pack_obj(&sEngine[n].objNextLine0, x) : 0;
        sGpuFrame.tables[0][G2T_SCREEN_A] = 0;
    }
    for (int n = 0; n < 2; n++) {
        Engine *e = &sEngine[n];
        e->palette = in->palette + 0x400 * n;
        e->oam = in->oam + 0x400 * n;
        for (int i = 0; i < 2; i++) {
            e->refX[i] = ref28(e, 0x28 + 0x10 * (u32)i);
            e->refY[i] = ref28(e, 0x2c + 0x10 * (u32)i);
            e->refWritten[i] = 0;
        }
        e->refLine = 0;
    }
    // line by line, both engines: after each line its H-blank (in->hblank), whose register and palette writes the
    // lines below show
    for (int line = 0; line < SCREEN_H; line++) {
        for (int n = 0; n < 2; n++) {
            Engine *e = &sEngine[n];
            window_vertical(e, line);
            e->powcnt = sPowCnt;
            // OBJs are prepared a line ahead (during the line before): line 0's during the previous frame's
            // last line, before this frame's register, OAM and VRAM changes
            if (sGpu || sDeferLayers) gpu_line(n, line);
            if (sGpu == 1 || sDeferLayers) {}  // (the GPU draws the OBJs; gpu2d_draw_layers)
            else if (line == 0 && e->haveNextLine0) e->obj = e->objNextLine0;
            else if (line == 0) memset(&e->obj, 0, sizeof e->obj);
            else gpu2d_render_obj_line(e, line, &e->obj);
            render_line(e, in, line);
        }
        int ran = in->hblank ? in->hblank(line, in->hblankContext) : 0;
        if (sDeferLayers && ran) record_changes(in);
        else if (sGpu) gpu_versions(in);
        for (int n = 0; n < 2; n++) {  // the internal reference points: the next line's
            Engine *e = &sEngine[n];
            for (int i = 0; i < 2; i++) {
                u32 r = REG_BG2PA + 0x10 * (u32)i;
                e->refX[i] = e->refWritten[i] & 1 ? ref28(e, 0x28 + 0x10 * (u32)i)
                                                  : e->refX[i] + (s16)reg16(e, r + 2) * (line + 1 - e->refLine);
                e->refY[i] = e->refWritten[i] & 2 ? ref28(e, 0x2c + 0x10 * (u32)i)
                                                  : e->refY[i] + (s16)reg16(e, r + 6) * (line + 1 - e->refLine);
                e->refWritten[i] = 0;
            }
            e->refLine = line + 1;
        }
    }
    for (int n = 0; n < 2; n++) {
        Engine *e = &sEngine[n];
        for (int line = SCREEN_H; line < FRAME_LINES; line++) window_vertical(e, line);
        gpu2d_render_obj_line(e, 0, &e->objNextLine0);
        e->haveNextLine0 = 1;
    }
    // to the screens: engine A on the top screen if POWCNT1 bit 15 is set
    if (sDeferLayers) {  // (the layers' own fields too: gpu2d_draw_layers)
        record_end(in);
        sFrame++;
        return;
    }
    if (sLayers) {
        sLayers->topEngine = (sPowCnt & 0x8000) ? 0 : 1;
        for (int n = 0; n < 2; n++) {
            sLayers->bgGran[n] = sEngine[n].bgGran;
            sLayers->bgMask[n] = sEngine[n].bgMask;
            sLayers->palette[n] = sEngine[n].palette;
        }
        sFrame++;
        return;
    }
    if (sGpu) sGpuFrame.tables[0][G2T_SCREEN_A] = (sPowCnt & 0x8000) ? 0 : 1;
    for (int n = 0; n < 2; n++) {
        int screen = (sPowCnt & 0x8000) ? n : 1 - n;
        if (sGpu == 1) {  // (the pictures are the GPU's: the lines' master brightness and 3D scroll only)
            memcpy(sDeferredLines[screen], sEngine[n].lineInfo, sizeof sEngine[n].lineInfo);
            continue;
        }
        if (sDefer3d) {
            const Engine *e = &sEngine[n];
            u32 *d = sDeferred[screen];
            for (int i = 0; i < SCREEN_W * SCREEN_H; i++) d[2 * i] = pack18(e->out[i]), d[2 * i + 1] = e->defer[i];
            memcpy(sDeferredLines[screen], e->lineInfo, sizeof e->lineInfo);
            continue;
        }
        u32 *s = sScreen[screen];
        const u32 *src = sEngine[n].out;
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            u32 c = src[i], r = c & 0x3f, g = c >> 8 & 0x3f, b = c >> 16 & 0x3f;
            r = r << 2 | r >> 4; g = g << 2 | g >> 4; b = b << 2 | b >> 4;
            s[i] = 0xff000000u | r << 16 | g << 8 | b;
        }
    }
    sFrame++;
}

const uint32_t *gpu2d_screen(int screen) { return sScreen[screen & 1]; }
const uint32_t *gpu2d_deferred(int screen) { return sDeferred[screen & 1]; }
const uint32_t *gpu2d_deferred_lines(int screen) { return sDeferredLines[screen & 1]; }

void gpu2d_describe_pixel(int screen, int x, int y, char *buf, size_t size) {
    int n = (sPowCnt & 0x8000) ? screen : 1 - screen;
    const Engine *e = &sEngine[n];
    static const char *const kLayer[8] = {"BG0", "BG1", "BG2", "BG3", "OBJ", "backdrop", "3D", "none"};
    static const char *const kEffect[7] = {"none", "alpha", "brightness up", "brightness down",
                                           "semi-transparent OBJ", "bitmap OBJ alpha", "3D alpha"};
    int at = y * SCREEN_W + x;
    char top[16];
    if (e->dbgTop[at] == LAYER_OBJ) snprintf(top, sizeof top, "OBJ#%u", e->dbgObj[at]);
    else snprintf(top, sizeof top, "%s", kLayer[e->dbgTop[at] & 7]);
    u32 dispcnt = reg32(e, REG_DISPCNT);
    snprintf(buf, size, "engine %c (mode %u, dispcnt %08x, bldcnt %04x %04x %04x, master %04x): %s %06x (alpha %u) "
             "over %s %06x, effect %s", 'A' + n, dispcnt & 7, dispcnt, reg16(e, REG_BLDCNT), reg16(e, REG_BLDALPHA),
             reg16(e, REG_BLDY), reg16(e, REG_MASTER_BRIGHT), top, e->dbgTopColor[at], e->dbgAlpha[at],
             kLayer[e->dbgBelow[at] & 7], e->dbgBelowColor[at], kEffect[e->dbgEffect[at] % 7]);
    size_t used = strlen(buf);
    if (e->dbgTop[at] == LAYER_OBJ && e->oam) {
        const u8 *o = e->oam + e->dbgObj[at] * 8;
        used += (size_t)snprintf(buf + used, size - used, "; OBJ attributes %04x %04x %04x", o[0] | o[1] << 8,
                                 o[2] | o[3] << 8, o[4] | o[5] << 8);
        if (o[1] & 1) {
            const u8 *g = e->oam + ((o[2] | o[3] << 8) >> 9 & 31) * 32;
            used += (size_t)snprintf(buf + used, size - used, " (matrix %04x %04x %04x %04x)", g[6] | g[7] << 8,
                                     g[14] | g[15] << 8, g[22] | g[23] << 8, g[30] | g[31] << 8);
        }
    }
    used += (size_t)snprintf(buf + used, size - used, "; bgcnt %04x %04x %04x %04x", reg16(e, REG_BGCNT),
                             reg16(e, REG_BGCNT + 2), reg16(e, REG_BGCNT + 4), reg16(e, REG_BGCNT + 6));
    if (dispcnt & 0xe000)
        snprintf(buf + used, size - used, "; windows %04x %04x / %04x %04x in %04x out %04x", reg16(e, REG_WIN0H),
                 reg16(e, REG_WIN0V), reg16(e, REG_WIN1H), reg16(e, REG_WIN1V), reg16(e, REG_WININ),
                 reg16(e, REG_WINOUT));
}

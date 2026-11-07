// The own 3D engine's rasteriser on the 3DS's GPU (PICA200, citro3d): the polygon list the
// geometry engine made (gpu3d_geom.c) drawn at the DS's resolution, then the colour, depth and fog flags read back
// into the software rasteriser's buffers (gpu3d_raster.c), whose fog pass and the 2D renderer's composite follow as
// for its own pictures. Not pixel-exact: the GPU's rasterisation, interpolation and 8-bit colour arithmetic.
//   - textures: decoded on the CPU with the rasteriser's texel fetch (g3_texture_rgba) into RGBA8 in the GPU's tiled
//     layout, cached by address, size, format and palette until the VRAM under them changes (4 KB pieces)
//   - modulation and decal in the texture combiners; the colour premultiplied by alpha (a second stage), so that
//     blending over a transparent pixel keeps the polygon's own colour, as on the DS; alpha blended with max
//   - the translucent polygon ID rule in the stencil buffer's bits 0-6 (as gpu3d_wgpu.c), the fog flag in its bit 7:
//     opaque pixels set theirs, translucent ones clear it when they have none
//   - alpha test, depth test less or less-equal (attr bit 14), translucent depth update (attr bit 11)
//   - the Z-buffer (linear in screen space: z * w through the perspective divide) and the W-buffer (the GPU's W-buffer
//     mode: the depth interpolated perspective-correct, not divided)
// Not done: anti-aliasing (the GPU has no coverage), and what the game doesn't use (toon / highlight, shadows, edge marking, wireframe, the rear plane).

#include "gpu3d_internal.h"
#include "../prof.h"

#ifndef __3DS__

int g3p_init(void) { return 0; }
void *g3p_picture(void) { return 0; }
void g3p_wait(void) {}
void g3p_render(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, u32 *color, u32 *depth, u8 *flags) {
    (void)r, (void)img, (void)pal, (void)texChanged, (void)color, (void)depth, (void)flags;
}

#else

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>
#include <citro3d.h>

#include "../pica_display.h"

extern const u8 gpu3d_pica_shbin[];  // (the vertex shader, gpu3d_pica.v.pica: picasso and bin2s)
extern const u32 gpu3d_pica_shbin_size;

enum { W = 256, H = 192, IMG_SIZE = 0x80000, PAL_SIZE = 0x20000, PIECE = 0x1000 };
enum { MAX_VERTS = MAX_POLYGONS * 24 };  // (a polygon of up to 10 vertices as up to 8 triangles)

typedef struct {
    float pos[4];  // clip position: (x, y, -Z, 1) * W, or (x * W, y * W, -Z, W) for the W-buffer (depth 0 to -W)
    float col[4];  // colour (6 bits) and the polygon's alpha (5 bits), 0-1
    float tc[2];   // texture coordinates, 0-1 over the texture
} PVertex;

static DVLB_s *sDvlb;
static shaderProgram_s sProgram;
static C3D_RenderTarget *sTarget;
// the pictures drawn for the GPU's compositing (color NULL: pica_display.c), into two textures in turn: the one
// drawn last (sPicture) is what the screens show while the next is drawn
static C3D_Tex sPic[2];
static C3D_RenderTarget *sPicTarget[2];
static int sPicture = -1;
static PVertex *sVerts;
static s8 sTargetLoc;  // (the vertex shader's `target`)
static u32 *sColorOut, *sDepthOut;  // the read-back pictures (linear memory)

// ---- the texture cache
enum { NTEX = 256 };
typedef struct {
    u32 key, pal;  // texparam without the wrap bits, texpal (palette formats)
    u32 ver;       // the newest VRAM piece version under it when it was decoded
    u32 used;      // the frame it was last used in
    u32 checked;   // sVer when its version was last compared (unchanged since: still valid)
    u32 palLo, palLen;  // 4x4 (format 5): the palette bytes its blocks use (when decoded)
    int live;
    C3D_Tex tex;
} TexEntry;
static TexEntry sTex[NTEX];
static u32 sFrame;
// a version per FINE bytes of the texture image and palette slots, bumped when they change: small, so that a
// palette or texture the game animates doesn't make its neighbours in VRAM look changed (their decoding again, 4 KB
// pieces before: a palette's five colours had six 4x4 textures decoded again, 30 ms on core 2)
enum { FINE = 64 };
static u8 sImgCopy[IMG_SIZE], sPalCopy[PAL_SIZE];
static u32 sImgVer[IMG_SIZE / FINE], sPalVer[PAL_SIZE / FINE], sVer;

static void track(const u8 *src, u8 *copy, u32 *ver, u32 size) {
    for (u32 o = 0; o < size; o += PIECE)  // (PIECE at a time, then the FINE bytes that differ)
        if (memcmp(copy + o, src + o, PIECE))
            for (u32 f = o; f < o + PIECE; f += FINE)
                if (memcmp(copy + f, src + f, FINE)) {
                    memcpy(copy + f, src + f, FINE);
                    ver[f / FINE] = ++sVer;
                }
}
static u32 newest(const u32 *ver, u32 size, u32 start, u32 len) {
    u32 v = 0;
    if (!len) return 0;
    for (u32 p = start / FINE, n = 0; n <= len / FINE + 1 && n < size / FINE; p = (p + 1) % (size / FINE), n++)
        if (ver[p] > v) v = ver[p];
    return v;
}
static u32 slot1_4x4(u32 base) { return 0x20000 + ((base & 0x1ffff) >> 1) + ((base >> 17) == 2 ? 0x10000 : 0); }
// a 4x4 texture's palette bytes: from its palette base, the lowest to the highest block's offset (4 bytes a unit;
// a block uses up to 4 colours: 8 bytes)
static void palette_4x4(u32 tp, u32 tpal, u32 *lo, u32 *len) {
    int w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    u32 slot1 = slot1_4x4((tp & 0xffff) << 3 & (IMG_SIZE - 1)), mn = 0x3fff, mx = 0;
    for (int b = 0; b < w * h / 16; b++) {
        u32 a = (slot1 + 2 * (u32)b) & (IMG_SIZE - 1), off = (sImgCopy[a] | sImgCopy[a + 1] << 8) & 0x3fff;
        if (off < mn) mn = off;
        if (off > mx) mx = off;
    }
    *lo = (((tpal & 0x1fff) << 4) + mn * 4) & (PAL_SIZE - 1);
    *len = (mx - mn) * 4 + 8;
}
// the newest version of the VRAM a texture reads (GBATEK "DS 3D Texture Formats": the image, 4x4's second slot,
// the palette)
static u32 texture_version(u32 tp, u32 tpal) {
    int fmt = tp >> 26 & 7, w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    static const u8 bits[8] = {0, 8, 2, 4, 8, 2, 8, 16};
    static const u16 colours[8] = {0, 32, 4, 16, 256, 0, 8, 0};
    u32 base = (tp & 0xffff) << 3 & (IMG_SIZE - 1);
    u32 v = newest(sImgVer, IMG_SIZE, base, (u32)(w * h * bits[fmt] / 8));
    if (fmt == 5) {  // (the 4x4 blocks' palette info in slot 1; the palette bytes they use: palette_4x4)
        u32 i = newest(sImgVer, IMG_SIZE, slot1_4x4(base), (u32)(w * h / 8));
        return v > i ? v : i;
    }
    if (colours[fmt]) {
        u32 pb = fmt == 2 ? (tpal & 0x1fff) << 3 : (tpal & 0x1fff) << 4;
        u32 p = newest(sPalVer, PAL_SIZE, pb & (PAL_SIZE - 1), colours[fmt] * 2u);
        if (p > v) v = p;
    }
    return v;
}

// row-major RGBA8 into the GPU's layout: 8x8 tiles, row by row, the texels in a tile in Morton (Z) order
// (half16: as RGBA5551, the DS's 5-bit colours and a 1-bit alpha exactly: half the GPU's reads of a texel)
static void tile(const u32 *src, void *dst, int w, int h, int half16) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int m = (x & 1) | (y & 1) << 1 | (x & 2) << 1 | (y & 2) << 2 | (x & 4) << 2 | (y & 4) << 3;
            int i = ((y >> 3) * (w >> 3) + (x >> 3)) * 64 + m;
            u32 c = src[y * w + x];
            if (half16)
                ((u16 *)dst)[i] = (u16)((c >> 27) << 11 | (c >> 19 & 31) << 6 | (c >> 11 & 31) << 1 | (c >> 7 & 1));
            else ((u32 *)dst)[i] = c;
        }
}

// the formats with a 1-bit alpha at most (all but A3I5 and A5I3): as RGBA5551, exact (g3_texture_rgba's 8 bits are a
// 5-bit value repeated); the direct build, ACWW_PICA_TEX16=0: all RGBA8, as before
static int half16(u32 tp) {
    static int on = -1;
#ifdef N3DS_DIRECT
    if (on < 0) on = !getenv("ACWW_PICA_TEX16") || atoi(getenv("ACWW_PICA_TEX16"));
#else
    on = 0;  // (the 3DS build as it was)
#endif
    int fmt = tp >> 26 & 7;
    return on && fmt != 1 && fmt != 6;
}

static C3D_Tex *texture(u32 tp, u32 tpal) {
    u32 key = tp & 0x3ff0ffff, pal = (tp >> 26 & 7) == 7 ? 0 : tpal & 0x1fff;
    // (the polygons in a row mostly share a texture, and VRAM rarely changes: the last one found, no version
    // compared while no VRAM piece has changed since)
    static TexEntry *last;
    if (last && last->live && last->key == key && last->pal == pal && last->checked == sVer) {
        last->used = sFrame;
        return &last->tex;
    }
    u32 ver = texture_version(tp, tpal);
    int is4x4 = (tp >> 26 & 7) == 5;
    TexEntry *e = 0, *lru = 0;
    for (int i = 0; i < NTEX; i++) {
        TexEntry *t = &sTex[i];
        if (t->live && t->key == key && t->pal == pal) { e = t; break; }
        if (!lru || !t->live || (lru->live && t->used < lru->used)) lru = t;
    }
    if (e && is4x4) {  // (its blocks unchanged since it was decoded: the palette bytes it used then)
        u32 p = newest(sPalVer, PAL_SIZE, e->palLo, e->palLen);
        if (p > ver) ver = p;
    }
    if (e && e->ver >= ver) {
        e->used = sFrame, e->checked = sVer;
        last = e;
        return &e->tex;
    }
    int w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    if (!e) {  // (a new one, in place of the one unused for longest)
        e = lru;
        if (e->live) C3D_TexDelete(&e->tex);
        e->live = 0;
        if (!C3D_TexInit(&e->tex, (u16)w, (u16)h, half16(tp) ? GPU_RGBA5551 : GPU_RGBA8)) return 0;
        C3D_TexSetFilter(&e->tex, GPU_NEAREST, GPU_NEAREST);
        e->live = 1, e->key = key, e->pal = pal;
    }
    u32 *rgba = malloc((size_t)w * h * 4);
    if (!rgba) return 0;
    g3_texture_rgba(tp, tpal, rgba);
    tile(rgba, e->tex.data, w, h, half16(tp));
    free(rgba);
    C3D_TexFlush(&e->tex);
    if (is4x4) {
        palette_4x4(tp, tpal, &e->palLo, &e->palLen);
        u32 p = newest(sPalVer, PAL_SIZE, e->palLo, e->palLen);
        if (p > ver) ver = p;
    }
    e->ver = ver, e->used = sFrame, e->checked = sVer;
    last = e;
    return &e->tex;
}

static GPU_TEXTURE_WRAP_PARAM wrap(u32 tp, int axis) {
    if (!(tp >> (16 + axis) & 1)) return GPU_CLAMP_TO_EDGE;
    return tp >> (18 + axis) & 1 ? GPU_MIRRORED_REPEAT : GPU_REPEAT;
}

#ifdef N3DS_DIRECT
// ---- the direct build's vertex lists: the polygons as the geometry engine makes them
// (g3p_polygon, on the game's thread), in batches of the same state, drawn opaque first, then translucent, each in
// the order made (the game sorts its translucent polygons itself: manual sort). Three lists in turn: one filled, one
// drawn, and the one drawn before it.
// shortcut: the list filled next is the one drawn two swaps ago, taken as done by the GPU (a swap is a frame or more,
// a 3D picture 2-4 ms of the GPU's); wait for the GPU there if it ever isn't.
// Each vertex also keeps its object coordinates and its clip matrix (Rec, Seg: normal memory), so that the picture
// can be drawn again from other matrices (replay: the pictures between the game's).
enum { LISTS = 3, LIST_VERTS = MAX_POLYGONS * 6, MAX_SEGS = 1024, NO_SEG = 0xffff };  // (a quad is two triangles)
typedef struct {
    u32 attr, texparam, texpal;
    u8 trans, wbuffer;
    int first, count;  // its vertices in the list
    float vx[2], vy[2];  // its viewport (sVx, sVy)
} Batch;
typedef struct {
    s32 obj[3];
    u16 seg;  // its clip matrix (NO_SEG: the list had too many; replayed as drawn)
} Rec;
typedef struct {
    s32 m[16];       // a clip matrix (20.12)
    s32 m0[16];      // the matrix it is paired with in the list before (pair; m: none), the pictures between from it
    s32 anchor[3];   // the object coordinates of the first vertex under it
    u32 sig;         // what is drawn with it: a hash of its first vertices' object coordinates, n of them
    int n;
} Seg;
typedef struct {
    PVertex *v;
    Rec *rec;  // (a vertex's, as v)
    int nv, npoly, nbatch, nseg;
    Batch batch[MAX_POLYGONS];
    Seg *seg;  // MAX_SEGS
} List;
static List sList[LISTS];
static int sFill;
// the replayed vertices drawn (linear memory), two in turn: one written while the GPU may still read the other
static PVertex *sReplay[2];
static int sReplayBuf;
// the viewport: the DS screen's clip coordinates from the geometry engine's (x' = x * sVx[0] + w * sVx[1], and y')
static float sVx[2] = {1, 0}, sVy[2] = {1, 0};

void g3p_viewport(const u32 vp[6]) {
    // (pixels: x = (x / w + 1) * width / 2 + x1, y = (1 - y / w) * height / 2 + top; the screen's clip space has
    // x / w = x / 128 - 1, y / w = 1 - y / 96)
    sVx[0] = (float)vp[4] / 256, sVx[1] = (float)(vp[4] + 2 * vp[0]) / 256 - 1;
    sVy[0] = (float)vp[5] / 192, sVy[1] = 1 - (float)vp[5] / 192 - (float)vp[3] / 96;
}

int g3p_matrix(const s32 clip[16]) {
    List *l = &sList[sFill];
    if (!l->seg || l->nseg >= MAX_SEGS) return NO_SEG;
    Seg *s = &l->seg[l->nseg];
    memcpy(s->m, clip, sizeof s->m);
    s->sig = 2166136261u, s->n = 0;
    return l->nseg++;
}

void g3p_vertex(int seg, const s32 obj[3]) {
    if (seg == NO_SEG) return;
    Seg *s = &sList[sFill].seg[seg];
    if (s->n >= 4) return;
    if (!s->n) memcpy(s->anchor, obj, sizeof s->anchor);
    for (int i = 0; i < 3; i++) s->sig = (s->sig ^ (u32)obj[i]) * 16777619u;  // (FNV-1a)
    s->n++;
}

// a vertex's clip coordinates (the geometry engine's, x 4096) as the GPU's: the viewport applied, the depth as the
// DS's (Z or W); eq: the depth test "equal"'s margin
static void clip_pos(float o[4], const s32 p[4], const float vx[2], const float vy[2], int wbuffer, float eq) {
    const float k = 1 / 4096.0f, w = (float)p[3] * k;
    o[0] = (float)p[0] * k * vx[0] + w * vx[1];
    o[1] = (float)p[1] * k * vy[0] + w * vy[1];
    if (wbuffer) {  // (the depth W itself, 24 bits: not divided by w, C3D_DepthMap; clipped at w > 0 only)
        float d = (float)p[3] * (1 / 16777215.0f) - eq;
        o[2] = d < 0 ? 0 : d > 1 ? -1 : -d;
    } else {  // (z / w from -1 to 1 into the GPU's 0 to -1: the near and far planes the DS's)
        o[2] = ((float)p[2] * k + w) * -0.5f + eq * w;
    }
    o[3] = w;
}
// (depth test "equal" passes within 0x200 on the DS, gpu3d_raster.c depth_pass: moved nearer by that, then less or
// equal)
static float depth_eq(u32 attr) { return attr & 0x4000 ? 0x200 / 16777215.0f : 0; }

// a vertex through m, as the geometry engine (gpu3d_geom.c submit_vertex)
static void transform(s32 p[4], const s32 m[16], const s32 obj[3]) {
    s64 x = obj[0], y = obj[1], z = obj[2];
    for (int c = 0; c < 4; c++)
        p[c] = (s32)((x * m[c] + y * m[4 + c] + z * m[8 + c] + 0x1000 * (s64)m[12 + c]) >> 12);
}

// the list's vertices again into out at t (16 bits) between their seg's pair: each from its object coordinates
// through the paired matrix (m0) and its own (m), the two positions interpolated, the colours and texture coordinates
// as drawn; returns the vertices whose position differs from the list's (t = 1: none). The positions, not the
// matrices, are interpolated: vertices the game made the same in both pictures (a seam between two pieces, a decal on
// a surface) stay the same between them, where a matrix rounded element by element moved them apart.
static int replay(const List *l, s64 t, PVertex *out) {
    int differ = 0;
    for (int k = 0; k < l->nbatch; k++) {
        const Batch *b = &l->batch[k];
        float eq = depth_eq(b->attr);
        // (the last three vertices made: a polygon's triangles share its first vertex and an edge, and polygons in a
        // row share vertices; the same object coordinates and matrix give the same position)
        int recent[3] = {-1, -1, -1}, next = 0;
        for (int i = b->first; i < b->first + b->count; i++) {
            const Rec *r = &l->rec[i];
            out[i] = l->v[i];
            if (r->seg == NO_SEG) continue;
            int j = -1;
            for (int c = 0; c < 3 && j < 0; c++) {
                const Rec *q = recent[c] < 0 ? 0 : &l->rec[recent[c]];
                if (q && q->seg == r->seg && q->obj[0] == r->obj[0] && q->obj[1] == r->obj[1] && q->obj[2] == r->obj[2])
                    j = recent[c];
            }
            if (j >= 0) {
                memcpy(out[i].pos, out[j].pos, sizeof out[i].pos);
                differ += memcmp(out[i].pos, l->v[i].pos, sizeof out[i].pos) != 0;
                continue;
            }
            recent[next] = i, next = (next + 1) % 3;
            const Seg *sg = &l->seg[r->seg];
            s32 p[4], p0[4];
            transform(p, sg->m, r->obj);
            if (t < 0x10000) {
                transform(p0, sg->m0, r->obj);
                for (int c = 0; c < 4; c++) p[c] = p0[c] + (s32)(((s64)(p[c] - p0[c]) * t) >> 16);
            }
            clip_pos(out[i].pos, p, b->vx, b->vy, b->wbuffer, eq);
            differ += memcmp(out[i].pos, l->v[i].pos, sizeof out[i].pos) != 0;
        }
    }
    return differ;
}

// ---- the pictures between the game's: the game draws a 3D picture every third frame
// (gVBlanksPerFrame = 3); the frames between show the list again with each clip matrix moved from its pair in the
// list before towards its own, so the newest picture is shown in full one game frame late (t = 1 on its last frame).
// The pairs: the matrices under which the same vertices are drawn (Seg.sig), the nearest on the screen of those;
// none (not interpolated) when none is within a quarter of the screen (for a vertex off the screen, a quarter of
// its distance from the middle). Each matrix of the list before goes to the nearest only (and to those as near: the
// same drawn again): one new in the picture, as a tree coming in at the wide screen's edge or the horizon, else took
// its neighbour's and was drawn between the two, gone from its place for a frame. Fewer than half paired: a cut, none.
static u16 sHead[1024], sNext[MAX_SEGS], sClaim[MAX_SEGS];  // sClaim: a matrix of the list before's nearest, + 1 (0: none)
static s16 sFound[MAX_SEGS];
static float sBest[MAX_SEGS];
static u32 sInterpLists, sInterpSegs, sInterpPaired, sInterpCuts, sInterpBatches, sInterpVerts;

// obj's position on the screen (x / w, y / w) and its depth (w) through m; 0 behind the eye
static int screen_of(const s32 m[16], const s32 obj[3], float xyw[3]) {
    s64 x = obj[0], y = obj[1], z = obj[2], p[4];
    for (int c = 0; c < 4; c++) p[c] = x * m[c] + y * m[4 + c] + z * m[8 + c] + 0x1000 * (s64)m[12 + c];
    if (p[3] <= 0) return 0;
    float iw = 1.0f / (float)p[3];
    xyw[0] = (float)p[0] * iw, xyw[1] = (float)p[1] * iw, xyw[2] = (float)p[3];
    return 1;
}

static void pair(List *l, const List *pl) {
    int paired = 0;
    memset(sHead, 0xff, sizeof sHead);
    for (int i = 0; i < pl->nseg; i++) {
        u32 h = pl->seg[i].sig & 1023;
        sNext[i] = sHead[h], sHead[h] = (u16)i;
    }
    for (int i = 0; i < l->nseg; i++) {
        Seg *sg = &l->seg[i];
        memcpy(sg->m0, sg->m, sizeof sg->m);
        sFound[i] = -1;
        float a[3], b[3];
        int found = -1, tries = 0;
        if (!sg->n || !screen_of(sg->m, sg->anchor, a)) continue;
        // (a vertex off the screen gets a quarter of its distance from the middle: one near the eye, as a large piece of
        // ground's first, moves a screen or more from one picture to the next)
        float r = fmaxf(1, fmaxf(fabsf(a[0]), fabsf(a[1]))), radius = 0.25f * 0.25f * r * r, best = INFINITY;
        for (u16 j = sHead[sg->sig & 1023]; j != 0xffff && tries < 32; j = sNext[j]) {
            const Seg *q = &pl->seg[j];
            if (q->sig != sg->sig || q->n != sg->n) continue;
            tries++;  // (32 at most: many of the same, as particles, cost nothing much)
            if (!screen_of(q->m, sg->anchor, b)) continue;
            // (the depth too, relative: of two alike one behind the other on the screen, as two trees, the one in front
            // took the one behind's and was drawn between, smaller)
            float d = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]), e = (a[2] - b[2]) / a[2];
            if (d < radius && d + e * e < best) best = d + e * e, found = j;
        }
        sFound[i] = (s16)found, sBest[i] = best;
        if (found >= 0 && (!sClaim[found] || best < sBest[sClaim[found] - 1])) sClaim[found] = (u16)(i + 1);
    }
    for (int i = 0; i < l->nseg; i++) {
        int j = sFound[i];
        if (j >= 0 && sBest[i] == sBest[sClaim[j] - 1]) memcpy(l->seg[i].m0, pl->seg[j].m, sizeof l->seg[i].m0), paired++;
    }
    for (int i = 0; i < l->nseg; i++)
        if (sFound[i] >= 0) sClaim[sFound[i]] = 0;
    sInterpLists++, sInterpSegs += (u32)l->nseg, sInterpPaired += (u32)paired;
    sInterpBatches += (u32)l->nbatch, sInterpVerts += (u32)l->nv;
    if (paired * 2 < l->nseg) {
        for (int i = 0; i < l->nseg; i++) memcpy(l->seg[i].m0, l->seg[i].m, sizeof l->seg[i].m0);
        sInterpCuts++;
    }
    static int stats = -1;  // (ACWW_PICA_STATS=1: every 300 lists)
    if (stats < 0) stats = getenv("ACWW_PICA_STATS") && atoi(getenv("ACWW_PICA_STATS"));
    if (stats && sInterpLists % 300 == 0) {
        fprintf(stderr, "pica interp: %u lists, %u matrices, %u paired, %u cuts; a list %u draws (batches), %u vertices\n",
                (unsigned)sInterpLists, (unsigned)sInterpSegs, (unsigned)sInterpPaired, (unsigned)sInterpCuts,
                (unsigned)(sInterpBatches / 300), (unsigned)(sInterpVerts / 300));
        sInterpSegs = sInterpPaired = sInterpCuts = sInterpBatches = sInterpVerts = 0;
    }
}

// the phase, counted on the game's thread: frames (g3p_between, every frame) since the list shown was first drawn,
// and the frames between it and the one before
static int sInterp = -1, sShownList = -1, sPeriod;
static u32 sFrames, sListFrame;

static int interp_on(void) {  // ACWW_PICA_INTERP=0: off
    if (sInterp < 0) sInterp = !getenv("ACWW_PICA_INTERP") || atoi(getenv("ACWW_PICA_INTERP"));
    return sInterp;
}
void g3p_set_interp(int on) { sInterp = on != 0; }  // (the options page: the game's thread, like interp_on)
int g3p_between(const G3RenderState *r) {
    // shortcut: counts g3_render's frames, every frame only with ACWW_FRAMESKIP=0 (the direct build's default)
    sFrames++;
    if (!interp_on()) return 0;
    return r->list == sShownList && sPeriod >= 2 && sPeriod <= 4 && (int)(sFrames - sListFrame) < sPeriod;
}

void g3p_polygon(G3Vertex *const *v, int n, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer) {
    List *l = &sList[sFill];
    if (!l->v || !l->rec || l->npoly >= MAX_POLYGONS || l->nv + (n - 2) * 3 > LIST_VERTS) return;
    l->npoly++;
    Batch *b = l->nbatch ? &l->batch[l->nbatch - 1] : 0;
    if (!b || b->attr != attr || b->texparam != texparam || b->texpal != texpal || b->wbuffer != wbuffer ||
        memcmp(b->vx, sVx, sizeof sVx) || memcmp(b->vy, sVy, sizeof sVy)) {
        b = &l->batch[l->nbatch++];
        b->attr = attr, b->texparam = texparam, b->texpal = texpal, b->trans = (u8)translucent;
        b->wbuffer = (u8)wbuffer, b->first = l->nv, b->count = 0;
        memcpy(b->vx, sVx, sizeof sVx), memcpy(b->vy, sVy, sizeof sVy);
    }
    static const float kSize[8] = {1 / 128.0f, 1 / 256.0f, 1 / 512.0f, 1 / 1024.0f,
                                   1 / 2048.0f, 1 / 4096.0f, 1 / 8192.0f, 1 / 16384.0f};  // (1 / (16 * size))
    const float k6 = 1 / 63.0f, tw = kSize[texparam >> 20 & 7], th = kSize[texparam >> 23 & 7], eq = depth_eq(attr);
    u32 pa = attr >> 16 & 31;
    float alpha = (float)(pa ? pa : 31) * (1.0f / 31.0f);
    PVertex pv[10];
    Rec rv[10];
    for (int i = 0; i < n; i++) {
        const G3Vertex *x = v[i];
        PVertex *o = &pv[i];
        clip_pos(o->pos, x->pos, b->vx, b->vy, wbuffer, eq);
        memcpy(rv[i].obj, x->obj, sizeof rv[i].obj);
        rv[i].seg = x->seg;
        for (int c = 0; c < 3; c++) o->col[c] = (float)(x->color[c] >> 3) * k6;
        o->col[3] = alpha;
        // (v from the top: the GPU's v = 1 is the texture's first row in memory, as tile() lays it out)
        o->tc[0] = (float)x->tex[0] * tw, o->tc[1] = 1.0f - (float)x->tex[1] * th;
    }
    PVertex *out = l->v + l->nv;
    for (int i = 1; i + 1 < n; i++) *out++ = pv[0], *out++ = pv[i], *out++ = pv[i + 1];
    Rec *ro = l->rec + l->nv;
    for (int i = 1; i + 1 < n; i++) *ro++ = rv[0], *ro++ = rv[i], *ro++ = rv[i + 1];
    l->nv += (n - 2) * 3;
    b->count += (n - 2) * 3;
}

int g3p_swap(void) {
    int handed = sFill;
    sFill = (sFill + 1) % LISTS;
    sList[sFill].nv = sList[sFill].npoly = sList[sFill].nbatch = sList[sFill].nseg = 0;
    return handed;
}
#endif

// ---- set-up
int g3p_init(void) {
    if (sTarget) return 1;
    pdisp_gsp_init();  // (the frontend's gfxInit may come before or after)
    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 4)) return 0;
    sTarget = C3D_RenderTargetCreate(W, H, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
#ifdef N3DS_DIRECT
    sVerts = sList[0].v = linearAlloc(LISTS * LIST_VERTS * sizeof(PVertex));
    for (int i = 1; i < LISTS; i++) sList[i].v = sList[0].v ? sList[0].v + i * LIST_VERTS : 0;
    for (int i = 0; i < LISTS; i++) {
        sList[i].rec = malloc(LIST_VERTS * sizeof(Rec));
        sList[i].seg = malloc(MAX_SEGS * sizeof(Seg));
    }
    for (int i = 0; i < 2; i++) sReplay[i] = linearAlloc(LIST_VERTS * sizeof(PVertex));  // (NULL: drawn as made)
#else
    sVerts = linearAlloc(MAX_VERTS * sizeof(PVertex));
#endif
    sColorOut = linearAlloc(W * H * 4);
    sDepthOut = linearAlloc(W * H * 4);
    sDvlb = DVLB_ParseFile((u32 *)gpu3d_pica_shbin, gpu3d_pica_shbin_size);
    if (!sTarget || !sVerts || !sColorOut || !sDepthOut || !sDvlb) return 0;
    for (int i = 0; i < 2; i++) {  // (the wide view: 512 wide for its 400 x 240)
        if (!C3D_TexInitVRAM(&sPic[i], gpu3d_wide_wanted() ? 512 : 256, 256, GPU_RGBA8)) return 0;
        C3D_TexSetFilter(&sPic[i], GPU_NEAREST, GPU_NEAREST);
        if (!(sPicTarget[i] = C3D_RenderTargetCreateFromTex(&sPic[i], GPU_TEXFACE_2D, 0, GPU_RB_DEPTH24_STENCIL8)))
            return 0;
    }
    shaderProgramInit(&sProgram);
    shaderProgramSetVsh(&sProgram, &sDvlb->DVLE[0]);
    sTargetLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "target");
    return 1;
}

// ---- a frame
typedef struct {
    C3D_Tex *tex;
    u32 wrap;
    int mode;  // 0 colour only, 1 modulation, 2 decal
    int trans, blend, depthEq, depthWrite, fog, id, wbuffer;
} State;

static void apply(const State *s, const G3RenderState *r) {
    C3D_TexEnv *e = C3D_GetTexEnv(0);
    C3D_TexEnvInit(e);
    if (s->mode == 0) {
        C3D_TexEnvSrc(e, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(e, C3D_Both, GPU_REPLACE);
    } else if (s->mode == 1) {
        C3D_TexEnvSrc(e, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(e, C3D_Both, GPU_MODULATE);
    } else {  // decal: the texture over the vertex colour by the texel's alpha, the polygon's alpha
        C3D_TexEnvSrc(e, C3D_RGB, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_TEXTURE0);
        C3D_TexEnvOpRgb(e, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_ALPHA);
        C3D_TexEnvFunc(e, C3D_RGB, GPU_INTERPOLATE);
        C3D_TexEnvSrc(e, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvFunc(e, C3D_Alpha, GPU_REPLACE);
    }
    e = C3D_GetTexEnv(1);  // the colour premultiplied by alpha
    C3D_TexEnvInit(e);
    C3D_TexEnvSrc(e, C3D_RGB, GPU_PREVIOUS, GPU_PREVIOUS, GPU_PREVIOUS);
    C3D_TexEnvOpRgb(e, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_ALPHA, GPU_TEVOP_RGB_SRC_COLOR);
    C3D_TexEnvFunc(e, C3D_RGB, GPU_MODULATE);
    C3D_TexEnvSrc(e, C3D_Alpha, GPU_PREVIOUS, GPU_PREVIOUS, GPU_PREVIOUS);
    C3D_TexEnvFunc(e, C3D_Alpha, GPU_REPLACE);
    if (s->tex) {
        C3D_TexSetWrap(s->tex, (GPU_TEXTURE_WRAP_PARAM)(s->wrap & 0xff), (GPU_TEXTURE_WRAP_PARAM)(s->wrap >> 8));
        C3D_TexBind(0, s->tex);
    }
    // alpha 0 never drawn; DISP3DCNT bit 2: only above ALPHA_TEST_REF (5 bits; the GPU's 8 keep the order)
    C3D_AlphaTest(true, GPU_GREATER, r->dispcnt & 4 ? (r->alphaRef * 255 + 15) / 31 : 0);
    // depth = -z / w, or -z for the W-buffer: the DS's 0-1, nearer smaller (not reversed as citro3d's own set-up: the
    // GPU computes depth in 24-bit floats, which are much finer near 0 than near 1, and the DS's depths are small)
    C3D_DepthMap(!s->wbuffer, -1.0f, 0.0f);
    C3D_DepthTest(true, s->depthEq ? GPU_LEQUAL : GPU_LESS, s->depthWrite ? GPU_WRITE_ALL : GPU_WRITE_COLOR);
    if (s->trans) {
        // not over its own ID (bits 0-6), which it then writes; the fog flag cleared unless it has fog
        C3D_StencilTest(true, GPU_NOTEQUAL, 0x40 | s->id, 0x7f, s->fog ? 0x7f : 0xff);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_MAX, GPU_ONE, s->blend ? GPU_ONE_MINUS_SRC_ALPHA : GPU_ZERO, GPU_ONE,
                       GPU_ONE);
    } else {
        C3D_StencilTest(true, GPU_ALWAYS, s->fog ? 0x80 : 0, 0xff, 0x80);
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO);
    }
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_REPLACE);
}

static void state_of(State *s, const G3RenderState *r, u32 attr, u32 texparam, u32 texpal, int trans, int wbuffer) {
    memset(s, 0, sizeof *s);
    int fmt = texparam >> 26 & 7;
    if (fmt && (r->dispcnt & 1)) s->tex = texture(texparam, texpal);
    s->mode = !s->tex ? 0 : (attr >> 4 & 3) == 1 ? 2 : 1;
    if (s->tex) s->wrap = (u32)wrap(texparam, 0) | (u32)wrap(texparam, 1) << 8;
    s->trans = trans;
    s->blend = (r->dispcnt & 8) != 0;
    s->depthEq = (attr & 0x4000) != 0;
    s->depthWrite = !trans || (attr & 0x800);
    s->fog = attr >> 15 & 1;
    s->id = (int)(attr >> 24 & 63);
    s->wbuffer = wbuffer;
}

static u32 rgb5_to_6(u32 c) { return c ? c * 2 + 1 : 0; }

void *g3p_picture(void) { return sPicture >= 0 ? &sPic[sPicture] : 0; }
void g3p_wait(void) { pdisp_wait(PDISP_3D, 0); }

// the fog's density (0-128) at depth z, as gpu3d_raster.c fog_pass
static s32 fog_density(const G3RenderState *r, s32 z) {
    int shift = r->dispcnt >> 8 & 15;
    s32 step = shift <= 10 ? 0x400 >> shift : 0, b0 = r->fogOffset + step, d;
    z >>= 9;
    if (z < b0 || !step) d = z < b0 ? r->fogTable[0] : r->fogTable[31];
    else {
        s32 n = (z - b0) >> (10 - shift);
        if (n >= 31) d = r->fogTable[31];
        else {
            s32 frac = (z - b0) - n * step, v = (r->fogTable[n + 1] - r->fogTable[n]) * frac;
            d = r->fogTable[n] + (v >= 0 ? v >> (10 - shift) : -(-v >> (10 - shift)));
        }
    }
    return d >= 127 ? 128 : d;
}

static void render(const G3RenderState *r, void *vb, u32 *color, u32 *depth, u8 *flags);
static void *prepare(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, int direct);
// for the compositing (color NULL): a job for the GPU thread (pica_display.c), which reads the list and the textures
// until the game's thread waits for it, into the picture chosen here (sPicture: the compositing jobs submitted
// before it show the other one); read back: at once
static struct {
    const G3RenderState *r;
    const u8 *img, *pal;
    int texChanged;
    int num, den;  // the direct build: the picture at t = num / den between the list before and r's (den 0: r's)
    int prev;      // the list before, to be paired with first (-1 none)
} sJob;
static int sJobPicture;
static void render_job(void *arg) {
    (void)arg;
    render(sJob.r, prepare(sJob.r, sJob.img, sJob.pal, sJob.texChanged, 1), 0, 0, 0);
}
// what a picture's drawing reads, made before its citro3d frame begins (so that it overlaps the GPU's work before):
// the texture VRAM's versions; the direct build: the pairs, the picture between's vertices (replay), the fog in their
// alpha. The replay buffer written was last drawn two jobs ago, done by the GPU before the last job's
// C3D_FrameBegin. (On the game's thread instead, core 0 waiting for core 2 anyway, it didn't pay on hardware: the GPU
// is the limit.) Returns the vertices to draw (NULL: the build's own list)
static void *prepare(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, int direct) {
    int fogAlpha = direct && (r->dispcnt & 0xc0) == 0xc0;
    u32 fa = r->fogColor >> 16 & 31;
    (void)fogAlpha, (void)fa;
    if (texChanged) {  // (ACWW_FPS's profile of the GPU thread: "VRAM")
        HOST_PROF_ON(gPicaProf, PROF_VRAM);
        track(img, sImgCopy, sImgVer, IMG_SIZE);
        track(pal, sPalCopy, sPalVer, PAL_SIZE);
    }
#ifdef N3DS_DIRECT
    List *l = &sList[r->list];
    PVertex *vb = l->v;  // (the vertices drawn)
    // ACWW_PICA_REPLAY=1: the check of the replay (D1): the list drawn from its object coordinates and matrices,
    // which must give the geometry engine's positions exactly; the vertices that don't counted, every 300 pictures
    static int check = -1;
    static unsigned checked, differ;
    if (check < 0) check = getenv("ACWW_PICA_REPLAY") && atoi(getenv("ACWW_PICA_REPLAY"));
    HostProfSaved replayProf = {gPicaProf, *gPicaProf};  // (the profile: "vertices", the pairs and the replay)
    *gPicaProf = PROF_VERTEX;
    if (direct && sJob.prev >= 0) pair(l, &sList[sJob.prev]);
    if (direct && sJob.den && sReplay[0]) {  // (a picture between the game's)
        vb = sReplay[sReplayBuf ^= 1];
        replay(l, ((s64)sJob.num << 16) / sJob.den, vb);
    } else if (check && sReplay[0]) {
        vb = sReplay[sReplayBuf ^= 1];
        differ += (unsigned)replay(l, 0x10000, vb);
        if (++checked % 300 == 0) fprintf(stderr, "pica replay: %u pictures, %u vertices differ\n", checked, differ);
    }
    host_prof_restore_on(&replayProf);
    // the fog worked into the alpha (as below), from the depth at each vertex (the list's own vertices too: the same
    // values again if the GPU still draws them)
    if (fogAlpha) {  // (the profile: "lighting")
        HOST_PROF_ON(gPicaProf, PROF_LIGHT);
        for (int k = 0; k < l->nbatch; k++) {
            const Batch *b = &l->batch[k];
            if (!(b->attr & 0x8000)) continue;
            u32 pa = b->attr >> 16 & 31, a = pa ? pa : 31;
            for (PVertex *o = vb + b->first, *e = o + b->count; o < e; o++) {
                float z = b->wbuffer ? -o->pos[2] : -o->pos[2] / o->pos[3];
                u32 d = (u32)fog_density(r, (s32)(z < 0 ? 0 : z > 1 ? 16777215 : z * 16777215));
                o->col[3] = (float)((fa * d + a * (128 - d)) >> 7) * (1.0f / 31.0f);
            }
        }
    }
    return vb;
#else
    return 0;
#endif
}
void g3p_render(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, u32 *color, u32 *depth, u8 *flags) {
    if (color) {
        pdisp_idle();
        render(r, prepare(r, img, pal, texChanged, 0), color, depth, flags);
        return;
    }
    pdisp_wait(PDISP_3D, 0);
    sJob.r = r, sJob.img = img, sJob.pal = pal, sJob.texChanged = texChanged;
#ifdef N3DS_DIRECT
    sJob.prev = -1;
    if (r->list != sShownList) {  // (a new list: the frames since the one before)
        sPeriod = sShownList >= 0 ? (int)(sFrames - sListFrame) : 0;
        sJob.prev = sShownList;
        sShownList = r->list, sListFrame = sFrames;
    }
    sJob.num = (int)(sFrames - sListFrame) + 1;
    sJob.den = interp_on() && sPeriod >= 2 && sPeriod <= 4 && sJob.num < sPeriod ? sPeriod : 0;
    if (!sJob.den) sJob.prev = -1;  // shortcut: a list first drawn without its pictures between isn't paired later
#endif
    sPicture = sJobPicture = sPicture < 0 ? 0 : sPicture ^ 1;
    pdisp_submit(PDISP_3D, render_job, 0);
}

static void render(const G3RenderState *r, void *vertices, u32 *color, u32 *depth, u8 *flags) {
    HOST_PROF_ON(gPicaProf, PROF_3D);
    sFrame++;
    static int stats = -1;  // ACWW_PICA_STATS=1: the textures the 3D used in the last 300 pictures, every 300
    if (stats < 0) stats = getenv("ACWW_PICA_STATS") && atoi(getenv("ACWW_PICA_STATS"));
    if (stats && sFrame % 300 == 0) {
        u32 n = 0, bytes = 0;
        for (int i = 0; i < NTEX; i++)
            if (sTex[i].live && sFrame - sTex[i].used < 300) n++, bytes += (u32)sTex[i].tex.size;
        fprintf(stderr, "pica: the 3D's textures: %lu in use, %lu KB\n", (unsigned long)n, (unsigned long)(bytes / 1024));
    }
    // for the GPU's compositing (color NULL): into the next texture, nothing read back; the fog only "alpha only"
    // (the caller sends the colour fog here: gpu3d_raster.c), worked into the polygons' alpha at their vertices (with
    // a fog alpha of 0 exactly the DS's: the density then scales the alpha)
    int direct = color == 0, fogAlpha = direct && (r->dispcnt & 0xc0) == 0xc0;
    u32 fa = r->fogColor >> 16 & 31;
    C3D_RenderTarget *target = direct ? sPicTarget[sJobPicture] : sTarget;
    u32 cc = r->clearColor, ca = r->clearAttrHi & 31;
    u32 cr = rgb5_to_6(cc & 31), cg = rgb5_to_6(cc >> 5 & 31), cb = rgb5_to_6(cc >> 10 & 31);
    u32 cd = (u32)(r->clearDepth & 0x7fff) * 0x200 + ((r->clearDepth & 0x7fff) == 0x7fff ? 0x1ff : 0);
    if (fogAlpha && (cc & 0x8000)) ca = (fa * (u32)fog_density(r, (s32)cd) + ca * (128 - (u32)fog_density(r, (s32)cd))) >> 7;
    u32 a8 = ca * 255 / 31;

#ifdef N3DS_DIRECT
    List *l = &sList[r->list];
#endif
    if (direct) pdisp_frame_open(PDISP_3D);  // (the compositing goes on in its frame: pica_display.c)
    else {
        HOST_PROF_ON(gPicaProf, PROF_GPU);
        C3D_FrameBegin(0);  // (not paced to the display: hw.c paces the frames; waits for the GPU's work before)
    }
    // (the colour premultiplied as the polygons')
    C3D_RenderTargetClear(target, C3D_CLEAR_ALL,
                          (cr * 255 / 63 * a8 / 255) << 24 | (cg * 255 / 63 * a8 / 255) << 16 |
                              (cb * 255 / 63 * a8 / 255) << 8 | a8,
                          cd | (cc & 0x8000 ? 0x80u : 0) << 24);
    C3D_FrameDrawOn(target);
    C3D_BindProgram(&sProgram);
    // (the wide view: the DS's picture GPU3D_WIDE_X / Y in from the corner of the 512 x 256 picture)
    const int wide = direct && gpu3d_wide();
#ifdef N3DS_DIRECT  // (the vertices in the DS screen's clip space: into the picture's)
    {
        float tw = direct ? (wide ? 512.0f : 256.0f) : W, th = direct ? 256.0f : H;
        float ox = wide ? GPU3D_WIDE_X : 0, oy = wide ? GPU3D_WIDE_Y : 0;
        C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc, 256 / tw, 0, 0, (256 + 2 * ox) / tw - 1);
        C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 1, 0, 192 / th, 0, 1 - (192 + 2 * oy) / th);
    }
#else  // (the vertices in the picture's clip space already)
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc, 1, 0, 0, 0);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 1, 0, 1, 0, 0);
#endif
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 2, 0, 0, 1, 0);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 3, 0, 0, 0, 1);
    C3D_AttrInfo *ai = C3D_GetAttrInfo();
    AttrInfo_Init(ai);
    AttrInfo_AddLoader(ai, 0, GPU_FLOAT, 4);
    AttrInfo_AddLoader(ai, 1, GPU_FLOAT, 4);
    AttrInfo_AddLoader(ai, 2, GPU_FLOAT, 2);
    C3D_BufInfo *bi = C3D_GetBufInfo();
    BufInfo_Init(bi);
#ifdef N3DS_DIRECT
    PVertex *vb = vertices;
    BufInfo_Add(bi, vb, sizeof(PVertex), 3, 0x210);
#else
    BufInfo_Add(bi, sVerts, sizeof(PVertex), 3, 0x210);
#endif
    C3D_CullFace(GPU_CULL_NONE);      // (the geometry engine culled already)
    for (int i = 2; i < 6; i++) C3D_TexEnvInit(C3D_GetTexEnv(i));

#ifdef N3DS_DIRECT
    {
        State cur;
        int have = 0;
        for (int pass = 0; pass < 2; pass++)  // (opaque, then translucent)
            for (int k = 0; k < l->nbatch; k++) {
                const Batch *b = &l->batch[k];
                if (b->trans != pass) continue;
                State s;
                state_of(&s, r, b->attr, b->texparam, b->texpal, b->trans, b->wbuffer);
                if (!have || memcmp(&s, &cur, sizeof s)) {
                    apply(&s, r);
                    cur = s;
                    have = 1;
                }
                C3D_DrawArrays(GPU_TRIANGLES, b->first, b->count);
            }
        GSPGPU_FlushDataCache(vb, (u32)l->nv * sizeof(PVertex));
    }
#else
    const float hy = direct ? 256.0f : (float)H;  // (the texture's rows: lines below 192 aren't drawn)
    int nv = 0, first = 0;
    State cur;
    int have = 0;
    for (int k = 0; k < r->npoly; k++) {
        const G3Polygon *p = r->poly[k];
        if (p->nv < 3 || nv + (p->nv - 2) * 3 > MAX_VERTS) continue;
        State s;
        state_of(&s, r, p->attr, p->texparam, p->texpal, p->translucent, p->wbuffer);
        if (!have || memcmp(&s, &cur, sizeof s)) {
            if (nv > first) C3D_DrawArrays(GPU_TRIANGLES, first, nv - first);
            first = nv;
            apply(&s, r);
            cur = s;
            have = 1;
        }
        u32 pa = p->attr >> 16 & 31;
        // (multiplications by reciprocals: the compiler keeps float divisions as divisions, which are slow here)
        static const float kSize[8] = {1 / 128.0f, 1 / 256.0f, 1 / 512.0f, 1 / 1024.0f,
                                       1 / 2048.0f, 1 / 4096.0f, 1 / 8192.0f, 1 / 16384.0f};  // (1 / (16 * size))
        const float kx = 2.0f / (16 * (wide ? 512 : W)), ky = 2.0f / (16 * hy), kz = 1.0f / 16777215.0f, k6 = 1.0f / 63.0f;
        const float ox = wide ? GPU3D_WIDE_X * 16.0f : 0, oy = wide ? GPU3D_WIDE_Y * 16.0f : 0;
        float alpha = (float)(pa ? pa : 31) * (1.0f / 31.0f);
        float tw = s.tex ? kSize[p->texparam >> 20 & 7] : 1.0f, th = s.tex ? kSize[p->texparam >> 23 & 7] : 1.0f;
        PVertex pv[10];
        for (int i = 0; i < p->nv; i++) {
            const G3Vertex *v = p->v[i];
            float w = (float)(p->w[i] > 0 ? p->w[i] : 1);
            float x = ((float)v->hx + ox) * kx - 1.0f, y = 1.0f - ((float)v->hy + oy) * ky;
            // (depth test "equal" passes within 0x200 on the DS, gpu3d_raster.c depth_pass: moved nearer by that, then
            // less or equal)
            float z = ((float)p->z[i] - (s.depthEq ? 0x200 : 0)) * kz;
            z = z < 0.0f ? 0.0f : z > 1.0f ? 1.0f : z;
            PVertex *o = &pv[i];
            o->pos[0] = x * w, o->pos[1] = y * w, o->pos[2] = s.wbuffer ? -z : -z * w, o->pos[3] = w;
            for (int c = 0; c < 3; c++) o->col[c] = (float)(v->color[c] >> 3) * k6;
            o->col[3] = alpha;
            if (fogAlpha && (p->attr & 0x8000)) {
                u32 d = (u32)fog_density(r, p->z[i]), a = pa ? pa : 31;
                o->col[3] = (float)((fa * d + a * (128 - d)) >> 7) * (1.0f / 31.0f);
            }
            // (v from the top: the GPU's v = 1 is the texture's first row in memory, as tile() lays it out)
            o->tc[0] = (float)v->tex[0] * tw, o->tc[1] = 1.0f - (float)v->tex[1] * th;
        }
        for (int i = 1; i + 1 < p->nv; i++) sVerts[nv++] = pv[0], sVerts[nv++] = pv[i], sVerts[nv++] = pv[i + 1];
    }
    if (nv > first) C3D_DrawArrays(GPU_TRIANGLES, first, nv - first);
    GSPGPU_FlushDataCache(sVerts, (u32)nv * sizeof(PVertex));
#endif
    if (direct) {  // (the GPU draws while the CPU goes on, the compositing in the same frame)
        pdisp_frame_run();
        return;
    }
    // the pictures back to linear memory (the transfers wait for the drawing before them, and for themselves)
    u32 flags3 = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
                 GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                 GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);
    int fog = (r->dispcnt & 0x80) != 0;  // (the depth and the fog flags are only the fog pass's)
    {
        HOST_PROF_ON(gPicaProf, PROF_GPU);
        C3D_SyncDisplayTransfer(sTarget->frameBuf.colorBuf, GX_BUFFER_DIM(W, H), sColorOut, GX_BUFFER_DIM(W, H), flags3);
        if (fog) C3D_SyncDisplayTransfer(sTarget->frameBuf.depthBuf, GX_BUFFER_DIM(W, H), sDepthOut, GX_BUFFER_DIM(W, H), flags3);
    }
    C3D_FrameEnd(0);
    GSPGPU_InvalidateDataCache(sColorOut, W * H * 4);
    if (fog) GSPGPU_InvalidateDataCache(sDepthOut, W * H * 4);

    // into the rasteriser's formats: the colour back from premultiplied, 6 bits; alpha 5 bits; depth 24 bits. (A
    // reciprocal per alpha: the ARM11 has no divide instruction.)
    static u32 recip[256], alpha5[256];
    if (!recip[1])
        for (u32 a = 1; a < 256; a++) recip[a] = ((63u << 16) + a / 2) / a, alpha5[a] = (a * 31 + 127) / 255;
    u32 clear = cr | cg << 8 | cb << 16;
    for (int i = 0; i < W * H; i++) {
        u32 px = sColorOut[i], a = px & 0xff, c = clear;
        if (a) {
            u32 k = recip[a], r = ((px >> 24) * k + 0x8000) >> 16, g = ((px >> 16 & 0xff) * k + 0x8000) >> 16,
                b = ((px >> 8 & 0xff) * k + 0x8000) >> 16;
            c = (r > 63 ? 63 : r) | (g > 63 ? 63 : g) << 8 | (b > 63 ? 63 : b) << 16;
        }
        color[i] = c | alpha5[a] << 24;
    }
    if (fog)
        for (int i = 0; i < W * H; i++) depth[i] = sDepthOut[i] & 0xffffff, flags[i] = (u8)(sDepthOut[i] >> 31);
}

#endif

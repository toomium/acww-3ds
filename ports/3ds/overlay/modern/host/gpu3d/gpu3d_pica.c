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

#if defined(__SWITCH__)
// (the Switch: gpu3d_gl.c)
#elif !defined(__3DS__)

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

#ifdef ENHANCED_PORT
// (24 bytes: the colour and texture coordinates raw, scaled by the vertex shader's cscale / tscale / tbias)
typedef struct {
    float pos[4];  // clip position: (x, y, -Z, 1) * W, or (x * W, y * W, -Z, W) for the W-buffer (depth 0 to -W)
    u8 col[4];     // colour (6 bits, 0-63) and the polygon's alpha (5 bits, 0-31)
    s16 tc[2];     // the DS's texture coordinates (1.11.4)
} PVertex;
#else
typedef struct {
    float pos[4];  // clip position: (x, y, -Z, 1) * W, or (x * W, y * W, -Z, W) for the W-buffer (depth 0 to -W)
    float col[4];  // colour (6 bits) and the polygon's alpha (5 bits), 0-1
    float tc[2];   // texture coordinates, 0-1 over the texture
} PVertex;
#endif

static DVLB_s *sDvlb;
static shaderProgram_s sProgram;
static C3D_RenderTarget *sTarget;
// the pictures drawn for the GPU's compositing (color NULL: pica_display.c), into two textures in turn: the one
// drawn last (sPicture) is what the screens show while the next is drawn
static C3D_Tex sPic[2];
static C3D_RenderTarget *sPicTarget[2];
static int sPicture = -1;
static PVertex *sVerts;
static s8 sTargetLoc, sCscaleLoc, sTscaleLoc;  // (the vertex shader's `target`, `cscale`, and `tscale` with `tbias` after it)
static s8 sLbaseLoc, sLdirLoc, sLcolLoc;       // (its lighting for the display lists drawn by the GPU: `lbase`, `ldir`, `lcol`)
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
static u32 sTexFreed, sTexFailed;  // (entries freed for the linear heap, textures that failed: ACWW_PICA_STATS)
#include "gpu3d_texver.h"  // (the texture VRAM's versions: track, newest, palette_4x4, texture_version)

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
// 5-bit value repeated); the port, ACWW_PICA_TEX16=0: all RGBA8, as before
static int half16(u32 tp) {
    static int on = -1;
#ifdef ENHANCED_PORT
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
    // (an entry per hash of the key and palette: the search through all of them only when it isn't the one; and an
    // entry whose version was compared while no VRAM piece has changed since is still valid, as `last`)
    static u8 hint[256];
    u32 hh = (key * 2654435761u ^ pal * 40503u) >> 24;
    TexEntry *e = 0, *lru = 0;
    if (sTex[hint[hh]].live && sTex[hint[hh]].key == key && sTex[hint[hh]].pal == pal) e = &sTex[hint[hh]];
    for (int i = 0; i < NTEX && !e; i++) {
        TexEntry *t = &sTex[i];
        if (t->live && t->key == key && t->pal == pal) { e = t; break; }
        if (!lru || !t->live || (lru->live && t->used < lru->used)) lru = t;
    }
    if (e && e->checked == sVer) {
        e->used = sFrame;
        hint[hh] = (u8)(e - sTex), last = e;
        return &e->tex;
    }
    u32 ver = texture_version(tp, tpal);
    int is4x4 = (tp >> 26 & 7) == 5;
    if (e && is4x4) {  // (its blocks unchanged since it was decoded: the palette bytes it used then)
        u32 p = newest(sPalVer, PAL_SIZE, e->palLo, e->palLen);
        if (p > ver) ver = p;
    }
    if (e && e->ver >= ver) {
        e->used = sFrame, e->checked = sVer;
        hint[hh] = (u8)(e - sTex), last = e;
        return &e->tex;
    }
    int w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    u32 *rgba = malloc((size_t)w * h * 4);  // (before an entry is taken: one taken and not decoded would look valid)
    if (!rgba) return 0;
    if (!e) {  // (a new one, in place of the one unused for longest)
        e = lru;
        if (e->live) C3D_TexDelete(&e->tex);
        e->live = 0;
        // (the linear heap full, as on an Old 3DS with its 13.5 MB: the textures unused for longest freed until this
        // one fits; not those of this picture or the one before, which the GPU may still read. Without, every new
        // texture failed and was drawn untextured, white)
        while (!C3D_TexInit(&e->tex, (u16)w, (u16)h, half16(tp) ? GPU_RGBA5551 : GPU_RGBA8)) {
            TexEntry *old = 0;
            for (int i = 0; i < NTEX; i++)
                if (sTex[i].live && sFrame - sTex[i].used >= 2 && (!old || sTex[i].used < old->used)) old = &sTex[i];
            if (!old) {
                sTexFailed++;
                free(rgba);
                return 0;
            }
            C3D_TexDelete(&old->tex);
            old->live = 0;
            sTexFreed++;
        }
        C3D_TexSetFilter(&e->tex, GPU_NEAREST, GPU_NEAREST);
        e->live = 1, e->key = key, e->pal = pal;
    }
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
    hint[hh] = (u8)(e - sTex), last = e;
    return &e->tex;
}

static GPU_TEXTURE_WRAP_PARAM wrap(u32 tp, int axis) {
    if (!(tp >> (16 + axis) & 1)) return GPU_CLAMP_TO_EDGE;
    return tp >> (18 + axis) & 1 ? GPU_MIRRORED_REPEAT : GPU_REPEAT;
}

#ifdef ENHANCED_PORT
#define G3_LINEAR_ALLOC linearAlloc
#include "gpu3d_lists.h"  // (the lists, the pictures between, the display lists drawn by the GPU)
#endif

// ---- set-up
int g3p_init(void) {
    if (sTarget) return 1;
    pdisp_gsp_init();  // (the frontend's gfxInit may come before or after)
    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 4)) return 0;
    sTarget = C3D_RenderTargetCreate(W, H, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
#ifdef ENHANCED_PORT
    sVerts = sList[0].v = linearAlloc(LISTS * LIST_VERTS * sizeof(PVertex));
    for (int i = 1; i < LISTS; i++) sList[i].v = sList[0].v ? sList[0].v + i * LIST_VERTS : 0;
    sList[0].recorded = recording();  // (the first list's; the others' at g3p_swap)
    for (int i = 0; i < LISTS; i++) {
        sList[i].rec = malloc(LIST_VERTS * sizeof(Rec));
        sList[i].seg = malloc(MAX_SEGS * sizeof(Seg));
    }
    if (recording()) replay_alloc();
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
    sCscaleLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "cscale");
    sTscaleLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "tscale");  // (tbias after it)
    sLbaseLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "lbase");
    sLdirLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "ldir");
    sLcolLoc = shaderInstanceGetUniformLocation(sProgram.vertexShader, "lcol");
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
static s64 sGpuT = 0x10000;  // (the port: where this picture is between the pair, as replay's t: the GPU's draws)
static struct {
    const G3RenderState *r;
    const u8 *img, *pal;
    int texChanged;
    int num, den;  // the port: the picture at t = num / den between the list before and r's (den 0: r's)
    int prev;      // the list before, to be paired with first (-1 none)
} sJob;
static int sJobPicture;
static void render_job(void *arg) {
    (void)arg;
    render(sJob.r, prepare(sJob.r, sJob.img, sJob.pal, sJob.texChanged, 1), 0, 0, 0);
}
// what a picture's drawing reads, made before its citro3d frame begins (so that it overlaps the GPU's work before):
// the texture VRAM's versions; the port: the pairs, the picture between's vertices (replay), the fog in their
// alpha. The replay buffer written was last drawn two jobs ago, done by the GPU before the last job's
// C3D_FrameBegin. (On the game's thread instead, core 0 waiting for core 2 anyway, it didn't pay on hardware: the GPU
// is the limit.) Returns the vertices to draw (NULL: the build's own list)
static void *prepare(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, int direct) {
    int fogAlpha = direct && (r->dispcnt & 0xc0) == 0xc0;
    u32 fa = r->fogColor >> 16 & 31;
    (void)fogAlpha, (void)fa;
    if (texChanged) {  // (ACWW_FPS's profile of the GPU thread: "VRAM")
        HOST_PROF_ON(gPicaProf, PROF_VRAM);
#ifdef ENHANCED_PORT
        for (int k = 0; k < gG3TexParts; k++) {  // (only the parts the uploads changed: gpu3d_tex.h)
            u32 at = gG3TexPart[k][0] & 0x7fffffff, end = at + gG3TexPart[k][1];
            int p = gG3TexPart[k][0] >> 31;
            for (u32 f = at / FINE * FINE; f < end; f += FINE)
                if (memcmp((p ? sPalCopy : sImgCopy) + f, (p ? pal : img) + f, FINE)) {
                    memcpy((p ? sPalCopy : sImgCopy) + f, (p ? pal : img) + f, FINE);
                    (p ? sPalVer : sImgVer)[f / FINE] = ++sVer;
                }
        }
        if (gG3TexParts < 0)
#endif
        {
            track(img, sImgCopy, sImgVer, IMG_SIZE);
            track(pal, sPalCopy, sPalVer, PAL_SIZE);
        }
    }
#ifdef ENHANCED_PORT
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
    sGpuT = 0x10000;
    if (direct && sJob.den && sReplay[0]) {  // (a picture between the game's)
        vb = sReplay[sReplayBuf ^= 1];
        replay(l, sGpuT = ((s64)sJob.num << 16) / sJob.den, vb);
    } else if (check && sReplay[0] && l->recorded) {
        vb = sReplay[sReplayBuf ^= 1];
        differ += (unsigned)replay(l, 0x10000, vb);
        if (++checked % 300 == 0) fprintf(stderr, "pica replay: %u pictures, %u vertices differ\n", checked, differ);
    }
    host_prof_restore_on(&replayProf);
    // the fog worked into the alpha (as below), from the depth at each vertex (the list's own vertices too: the same
    // values again if the GPU still draws them)
    if (fogAlpha) {  // (the profile: "fog")
        HOST_PROF_ON(gPicaProf, PROF_FOG);
        for (int k = 0; k < l->nbatch; k++) {
            const Batch *b = &l->batch[k];
            // (a display list's draw: first and count are indices into sGi, not vertices here. shortcut: its polygons
            // get no fog in their alpha; the vertex shader would have to work it out from the depth)
            if (!(b->attr & 0x8000) || b->gpu) continue;
            u32 pa = b->attr >> 16 & 31, a = pa ? pa : 31;
            for (PVertex *o = vb + b->first, *e = o + b->count; o < e; o++) {
                float z = b->wbuffer ? -o->pos[2] : -o->pos[2] / o->pos[3];
                u32 d = (u32)fog_density(r, (s32)(z < 0 ? 0 : z > 1 ? 16777215 : z * 16777215));
                o->col[3] = (u8)((fa * d + a * (128 - d)) >> 7);
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
#ifdef ENHANCED_PORT
    sJob.prev = -1;
    if (r->list != sShownList) {  // (a new list: the frames since the one before)
        sPeriod = sShownList >= 0 ? (int)(sFrames - sListFrame) : 0;
        sJob.prev = sShownList;
        sShownList = r->list, sListFrame = sFrames;
    }
    sJob.num = (int)(sFrames - sListFrame) + 1;
    int interp = interp_on() && sList[r->list].recorded;  // (a list begun with them off has nothing to pair)
    sJob.den = interp && sPeriod >= 2 && sPeriod <= 4 && sJob.num < sPeriod ? sPeriod : 0;
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
        fprintf(stderr, "pica: the 3D's textures: %lu in use, %lu KB; so far %lu freed for memory, %lu failed\n",
                (unsigned long)n, (unsigned long)(bytes / 1024), (unsigned long)sTexFreed, (unsigned long)sTexFailed);
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

#ifdef ENHANCED_PORT
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
#ifdef ENHANCED_PORT  // (the vertices in the DS screen's clip space: into the picture's)
    const float tw = direct ? (wide ? 512.0f : 256.0f) : W, th = direct ? 256.0f : H;
    const float tx = 256 / tw, txw = (256 + 2 * (wide ? GPU3D_WIDE_X : 0)) / tw - 1;
    const float ty = 192 / th, tyw = 1 - (192 + 2 * (wide ? GPU3D_WIDE_Y : 0)) / th;
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc, tx, 0, 0, txw);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 1, 0, ty, 0, tyw);
#else  // (the vertices in the picture's clip space already)
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc, 1, 0, 0, 0);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 1, 0, 1, 0, 0);
#endif
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 2, 0, 0, 1, 0);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 3, 0, 0, 0, 1);
#ifdef ENHANCED_PORT  // (the colours 0-63 and alpha 0-31, the texture coordinates scaled per batch: below)
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sCscaleLoc, 1 / 63.0f, 1 / 63.0f, 1 / 63.0f, 1 / 31.0f);
#else  // (the vertices' colours and texture coordinates as they are)
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sCscaleLoc, 1, 1, 1, 1);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc, 1, 1, 1, 1);
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc + 1, 0, 0, 0, 0);
#endif
    C3D_FVUnifSet(GPU_VERTEX_SHADER, sLbaseLoc, 0, 0, 0, 0);  // (no lighting: the colours as they come)
    for (int i = 0; i < 4; i++) {
        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLdirLoc + i, 0, 0, 0, 0);
        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLcolLoc + i, 0, 0, 0, 0);
    }
    C3D_AttrInfo *ai = C3D_GetAttrInfo();
    AttrInfo_Init(ai);
    AttrInfo_AddLoader(ai, 0, GPU_FLOAT, 4);
#ifdef ENHANCED_PORT
    AttrInfo_AddLoader(ai, 1, GPU_UNSIGNED_BYTE, 4);
    AttrInfo_AddLoader(ai, 2, GPU_SHORT, 2);
#else
    AttrInfo_AddLoader(ai, 1, GPU_FLOAT, 4);
    AttrInfo_AddLoader(ai, 2, GPU_FLOAT, 2);
#endif
    C3D_BufInfo *bi = C3D_GetBufInfo();
    BufInfo_Init(bi);
#ifdef ENHANCED_PORT
    PVertex *vb = vertices;
    BufInfo_Add(bi, vb, sizeof(PVertex), 3, 0x210);
#else
    BufInfo_Add(bi, sVerts, sizeof(PVertex), 3, 0x210);
#endif
    C3D_CullFace(GPU_CULL_NONE);      // (the geometry engine culled already)
    for (int i = 2; i < 6; i++) C3D_TexEnvInit(C3D_GetTexEnv(i));

#ifdef ENHANCED_PORT
    {
        State cur;
        int have = 0, size = -1;  // size: the texture size fields the texture coordinates' scale is set for
        // ACWW_PICA_DRAWTEST=<n>: a trial of what draws cost (before the lists' vertices go to the GPU, each list a
        // draw with its matrix): the picture's batches cut into about n more draws, each with five uniform rows sent
        // again; the same picture. The loop's ticks and draws in the log; 0: only counted.
        static int drawtest = -2;
        if (drawtest == -2) drawtest = getenv("ACWW_PICA_DRAWTEST") ? atoi(getenv("ACWW_PICA_DRAWTEST")) : -1;
        const int chunk = drawtest > 0 ? (l->nv / 3 / drawtest + 1) * 3 : 0;  // (whole triangles)
        u32 draws = 0;
        const u64 t0 = svcGetSystemTick();
        // the display lists' draws (g3p_gpu_draw) take their vertices from sGv, in another format, and set the
        // shader's matrix, lighting and the culling: put back for the next batch of the other kind
        C3D_AttrInfo aiCpu = *ai, aiList;
        C3D_BufInfo biCpu = *bi, biList;
        AttrInfo_Init(&aiList);
        AttrInfo_AddLoader(&aiList, 0, GPU_SHORT, 4);
        AttrInfo_AddLoader(&aiList, 1, GPU_SHORT, 4);
        AttrInfo_AddLoader(&aiList, 2, GPU_SHORT, 2);
        BufInfo_Init(&biList);
        if (sGv) BufInfo_Add(&biList, sGv, sizeof(G3GVertex), 3, 0x210);
        int listState = 0;
        static u32 flushedV, flushedI;
        u32 gv = sGvN, gi = sGiN;
        if (gv > flushedV) GSPGPU_FlushDataCache(sGv, gv * sizeof *sGv), flushedV = gv;
        if (gi > flushedI) GSPGPU_FlushDataCache(sGi, gi * sizeof *sGi), flushedI = gi;
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
                if (size != (int)(b->texparam >> 20 & 0x3f)) {  // (v from the top: the GPU's v = 1 is the texture's
                    size = (int)(b->texparam >> 20 & 0x3f);     // first row in memory, as tile() lays it out)
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc, kSize[size & 7], -kSize[size >> 3], 1, 1);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc + 1, 0, 1, 0, 0);
                }
                if (b->gpu) {
                    const G3GDraw *g = &l->draw[b->gpu - 1];
                    const float *c = g->clip, eq = depth_eq(b->attr);
                    float moved[16];
                    if (sGpuT < 0x10000 && b->seg != NO_SEG) {  // (a picture between: the matrix moved as replay's
                        const Seg *sg = &l->seg[b->seg];        // positions, the same transform as dl_piece_gpu's)
                        if (memcmp(sg->m, sg->m0, sizeof sg->m)) {
                            s32 m[16];
                            for (int i = 0; i < 16; i++) m[i] = sg->m0[i] + (s32)(((s64)(sg->m[i] - sg->m0[i]) * sGpuT) >> 16);
                            for (int i = 0; i < 4; i++) {
                                for (int j = 0; j < 3; j++) moved[4 * i + j] = (float)m[4 * j + i] * (1 / 16777216.0f);
                                moved[4 * i + 3] = (float)m[12 + i] * (1 / 4096.0f);
                            }
                            c = moved;
                        }
                    }
                    if (!listState) C3D_SetAttrInfo(&aiList), C3D_SetBufInfo(&biList), listState = 1;
                    // (clip_pos and `target` in one matrix: the viewport, the picture, the depth. The W-buffer's
                    // is w itself; not limited to 1 as clip_pos does: beyond the far plane only)
                    float m[4][4];
                    for (int j = 0; j < 4; j++) {
                        m[0][j] = tx * (b->vx[0] * c[j] + b->vx[1] * c[12 + j]) + txw * c[12 + j];
                        m[1][j] = ty * (b->vy[0] * c[4 + j] + b->vy[1] * c[12 + j]) + tyw * c[12 + j];
                        m[2][j] = b->wbuffer ? c[12 + j] * (-4096 / 16777215.0f) + (j == 3 ? eq : 0)
                                             : (c[8 + j] + c[12 + j]) * -0.5f + eq * c[12 + j];
                        m[3][j] = c[12 + j];
                    }
                    for (int i = 0; i < 4; i++) C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + i, m[i][0], m[i][1], m[i][2], m[i][3]);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sCscaleLoc, g->cscale[0], g->cscale[1], g->cscale[2], g->cscale[3]);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sLbaseLoc, g->base[0], g->base[1], g->base[2], g->base[3]);
                    for (int i = 0; i < 4; i++) {
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLdirLoc + i, g->ldir[i][0], g->ldir[i][1], g->ldir[i][2], 0);
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLcolLoc + i, g->lcol[i][0], g->lcol[i][1], g->lcol[i][2], 0);
                    }
                    if (b->texparam >> 30) {  // (the DS's texture matrix, then the size's scale; the next batch sets its own)
                        const float su = kSize[size & 7], sv = -kSize[size >> 3];
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc, su * g->tex[0][0], sv * g->tex[1][0], 1, 1);
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sTscaleLoc + 1, su * g->tex[0][1], sv * g->tex[1][1] + 1, 0, 0);
                        size = -1;
                    }
                    // (the DS's sides: bit 7 the front drawn, bit 6 the back; the picture is drawn turned)
                    C3D_CullFace((b->attr & 0xc0) == 0xc0 ? GPU_CULL_NONE : b->attr & 0x80 ? GPU_CULL_BACK_CCW : GPU_CULL_FRONT_CCW);
                    C3D_DrawElements(GPU_TRIANGLES, b->count, C3D_UNSIGNED_SHORT, sGi + b->first);
                    draws++;
                    continue;
                }
                if (listState) {
                    listState = 0;
                    C3D_SetAttrInfo(&aiCpu), C3D_SetBufInfo(&biCpu);
                    C3D_CullFace(GPU_CULL_NONE);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc, tx, 0, 0, txw);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 1, 0, ty, 0, tyw);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 2, 0, 0, 1, 0);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sTargetLoc + 3, 0, 0, 0, 1);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sCscaleLoc, 1 / 63.0f, 1 / 63.0f, 1 / 63.0f, 1 / 31.0f);
                    C3D_FVUnifSet(GPU_VERTEX_SHADER, sLbaseLoc, 0, 0, 0, 0);
                    for (int i = 0; i < 4; i++) {
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLdirLoc + i, 0, 0, 0, 0);
                        C3D_FVUnifSet(GPU_VERTEX_SHADER, sLcolLoc + i, 0, 0, 0, 0);
                    }
                }
                if (!chunk) C3D_DrawArrays(GPU_TRIANGLES, b->first, b->count), draws++;
                else
                    for (int f = 0; f < b->count; f += chunk, draws++) {
                        // (`target` and `cscale` marked changed: citro3d sends the five rows again with the draw)
                        C3D_FVUnifWritePtr(GPU_VERTEX_SHADER, sTargetLoc, 5);
                        C3D_DrawArrays(GPU_TRIANGLES, b->first + f, b->count - f < chunk ? b->count - f : chunk);
                    }
            }
        if (drawtest >= 0) {
            static u64 ticks;
            static u32 total;
            ticks += svcGetSystemTick() - t0, total += draws;
            if (sFrame % 300 == 0) {
                fprintf(stderr, "pica drawtest: %lu draws and %llu ticks a picture (the batches' loop, 300 pictures)\n",
                        (unsigned long)(total / 300), (unsigned long long)(ticks / 300));
                ticks = 0, total = 0;
            }
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

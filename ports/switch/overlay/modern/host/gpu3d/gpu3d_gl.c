// The own 3D engine's rasteriser on the Switch's GPU (OpenGL ES 3 through Mesa, gl_display.h's context): the
// console ports' GPU interface g3p_* (gpu3d_internal.h), as gpu3d_pica.c is on the 3DS. The geometry engine
// (gpu3d_geom.c, ENHANCED_PORT) hands its polygons here; g3p_render draws a list, as gpu3d_pica.c draws: the vertices
// in the DS screen's clip space, its depth (Z or W), alpha test, the translucent polygons' ID test in the stencil, the
// colours premultiplied by alpha. Into a picture for the GPU's compositing (comp2d.c, gl_display.c), two in turn,
// with the wide view (gpu3d.h) 512 x 256 with the DS's picture at GPU3D_WIDE_X / Y; or (the colour fog, the CPU's
// compositing) into a 256 x 192 one read back into the rasteriser's buffers.
// The lists and the pictures between the game's (60 a second from the game's 20) are gpu3d_pica.c's, gpu3d_lists.h.
// shortcut: no display lists drawn by the GPU (G3_NO_GPU_LISTS: g3p_gpu_store refuses),
// no fog (read back: its flags as none; for the compositing: not worked into the alpha) (gpu3d_pica.c has these).
#ifdef __SWITCH__

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLES3/gl3.h>
#include <switch.h>  // (the worker thread: threadCreate, Mutex, CondVar)

#include "gpu3d_internal.h"
#include "../gl_display.h"
#include "../plat.h"
#include "../prof.h"

enum { W = 256, H = 192, IMG_SIZE = 0x80000, PAL_SIZE = 0x20000, PIECE = 0x1000 };

// (24 bytes: the colour and texture coordinates raw, scaled by the vertex shader's cscale / tscale; as gpu3d_pica.c's)
typedef struct {
    float pos[4];  // clip position: (x, y, -Z, 1) * W, or (x * W, y * W, -Z, W) for the W-buffer (depth 0 to -W)
    u8 col[4];     // colour (6 bits, 0-63) and the polygon's alpha (5 bits, 0-31)
    s16 tc[2];     // the DS's texture coordinates (1.11.4)
} PVertex;

// ---- the lists, the pictures between the game's: as gpu3d_pica.c's (gpu3d_lists.h); no display lists drawn by the GPU
#define G3_LINEAR_ALLOC malloc
#define G3_NO_GPU_LISTS
#include "gpu3d_lists.h"

// ---- the program, the read-back picture's framebuffer, the pictures for the compositing (gl_display.h)
static int sReady;
static GLuint sProg, sFbo, sColorRb, sDepthRb, sVao, sVbo;
static struct CgTex sPic[2];
static int sPicture = -1;
static GLint sTargetLoc, sCscaleLoc, sTscaleLoc, sWbufLoc, sModeLoc, sArefLoc;
static u8 sPixels[W * H * 4];

static const char kVs[] =
    "#version 300 es\n"
    "layout(location = 0) in vec4 inpos;\n"
    "layout(location = 1) in vec4 inclr;\n"
    "layout(location = 2) in vec2 intc;\n"
    "uniform vec4 target[4];\n"
    "uniform vec4 cscale;\n"
    "uniform vec4 tscale;\n"  // (xy the scale, zw the bias)
    "uniform int wbuffer;\n"
    "out vec4 vclr;\n"
    "out vec2 vtc;\n"
    "out float vd;\n"
    "void main() {\n"
    "    vec4 p = vec4(dot(target[0], inpos), dot(target[1], inpos), dot(target[2], inpos), dot(target[3], inpos));\n"
    // the DS's depth (gpu3d_pica.c's 0 to -1: -z / w, or -z itself for the W-buffer) into GL's window depth 0 to 1;
    // the W-buffer's interpolated perspective-correct as a varying (gl_FragDepth), as the DS's W
    "    vd = -p.z;\n"
    "    p.z = wbuffer != 0 ? (2.0 * vd - 1.0) * p.w : -2.0 * p.z - p.w;\n"
    "    gl_Position = p;\n"
    "    vclr = min(inclr * cscale, vec4(1.0));\n"
    "    vtc = intc * tscale.xy + tscale.zw;\n"
    "}\n";
static const char kFs[] =
    "#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"  // (wbuffer: a uniform of both shaders has the same precision in both)
    "in vec4 vclr;\n"
    "in vec2 vtc;\n"
    "in float vd;\n"
    "uniform sampler2D tex;\n"
    "uniform int mode;\n"  // 0 colour only, 1 modulation, 2 decal
    "uniform int wbuffer;\n"
    "uniform float aref;\n"
    "out vec4 o;\n"
    "void main() {\n"
    "    vec4 c = vclr;\n"
    "    if (mode == 1) c *= texture(tex, vtc);\n"
    "    else if (mode == 2) { vec4 t = texture(tex, vtc); c.rgb = mix(c.rgb, t.rgb, t.a); }\n"
    "    if (c.a <= aref) discard;\n"  // (alpha 0 never drawn; DISP3DCNT bit 2: only above ALPHA_TEST_REF)
    "    o = vec4(c.rgb * c.a, c.a);\n"
    "    gl_FragDepth = wbuffer != 0 ? vd : gl_FragCoord.z;\n"
    "}\n";

static GLuint shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, 0);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, sizeof log, 0, log);
        fprintf(stderr, "gl: a shader didn't compile: %s\n", log);
    }
    return s;
}

int g3p_init(void) {
    if (sReady) return 1;
    if (!gld_context()) return 0;
    sList[0].v = malloc(LISTS * LIST_VERTS * sizeof(PVertex));
    for (int i = 1; i < LISTS; i++) sList[i].v = sList[0].v ? sList[0].v + i * LIST_VERTS : 0;
    sList[0].recorded = recording();  // (the first list's; the others' at g3p_swap)
    for (int i = 0; i < LISTS; i++) {
        sList[i].rec = malloc(LIST_VERTS * sizeof(Rec));
        sList[i].seg = malloc(MAX_SEGS * sizeof(Seg));
    }
    if (recording()) replay_alloc();
    gld_enter();
    sProg = glCreateProgram();
    glAttachShader(sProg, shader(GL_VERTEX_SHADER, kVs));
    glAttachShader(sProg, shader(GL_FRAGMENT_SHADER, kFs));
    glLinkProgram(sProg);
    GLint ok = 0;
    glGetProgramiv(sProg, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(sProg, sizeof log, 0, log);
        fprintf(stderr, "gl: the shaders didn't link: %s\n", log);
    }
    sTargetLoc = glGetUniformLocation(sProg, "target");
    sCscaleLoc = glGetUniformLocation(sProg, "cscale");
    sTscaleLoc = glGetUniformLocation(sProg, "tscale");
    sWbufLoc = glGetUniformLocation(sProg, "wbuffer");
    sModeLoc = glGetUniformLocation(sProg, "mode");
    sArefLoc = glGetUniformLocation(sProg, "aref");
    glGenRenderbuffers(1, &sColorRb);
    glBindRenderbuffer(GL_RENDERBUFFER, sColorRb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, W, H);
    glGenRenderbuffers(1, &sDepthRb);
    glBindRenderbuffer(GL_RENDERBUFFER, sDepthRb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, W, H);
    glGenFramebuffers(1, &sFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, sFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, sColorRb);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sDepthRb);
    int complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glGenVertexArrays(1, &sVao);
    glBindVertexArray(sVao);
    glGenBuffers(1, &sVbo);
    glBindBuffer(GL_ARRAY_BUFFER, sVbo);
    glBufferData(GL_ARRAY_BUFFER, LIST_VERTS * sizeof(PVertex), 0, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PVertex), (void *)offsetof(PVertex, pos));
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(PVertex), (void *)offsetof(PVertex, col));
    glVertexAttribPointer(2, 2, GL_SHORT, GL_FALSE, sizeof(PVertex), (void *)offsetof(PVertex, tc));
    for (int i = 0; i < 3; i++) glEnableVertexAttribArray((GLuint)i);
    gld_leave();
    for (int i = 0; i < 2 && complete; i++) complete = gld_target(&sPic[i], gpu3d_wide_wanted() ? 512 : 256, 256);
    if (!ok || !complete) {
        fprintf(stderr, "gl: the 3D renderer couldn't start (program %d, framebuffer %d)\n", ok, complete);
        return 0;
    }
    fprintf(stderr, "gl: the 3D drawn by the GPU (OpenGL ES 3)\n");
    return sReady = 1;
}

// ---- the texture cache: a texture decoded again only when the VRAM bytes it reads changed (gpu3d_pica.c's versions,
// gpu3d_texver.h)
#include "gpu3d_texver.h"
enum { NTEX = 256 };
typedef struct {
    u32 key, pal, used;
    u32 ver;            // the newest VRAM version under it when it was decoded
    u32 checked;        // sVer when that was last compared (unchanged since: still valid)
    u32 palLo, palLen;  // 4x4 (format 5): the palette bytes its blocks use
    int live;
    GLuint tex;
    GLint ws, wt;  // (its wrap as last set: set again only when a polygon wants another)
} TexEntry;
static TexEntry sTex[NTEX];
static u32 sDecoded;  // (textures decoded since the last report: gl_stats)
static u32 sFrame;

static TexEntry *texture(u32 tp, u32 tpal) {
    u32 key = tp & 0x3ff0ffff, pal = (tp >> 26 & 7) == 7 ? 0 : tpal & 0x1fff;
    static TexEntry *last;  // (the polygons in a row mostly share a texture)
    if (last && last->live && last->key == key && last->pal == pal && last->checked == sVer) {
        last->used = sFrame;
        return last;
    }
    TexEntry *e = 0, *lru = 0;
    for (int i = 0; i < NTEX && !e; i++) {
        TexEntry *t = &sTex[i];
        if (t->live && t->key == key && t->pal == pal) e = t;
        else if (!lru || !t->live || (lru->live && t->used < lru->used)) lru = t;
    }
    if (e && e->checked == sVer) {
        e->used = sFrame;
        return last = e;
    }
    u32 ver = texture_version(tp, tpal);
    int is4x4 = (tp >> 26 & 7) == 5;
    if (e && is4x4) {  // (its blocks unchanged since it was decoded: the palette bytes it used then)
        u32 p = newest(sPalVer, PAL_SIZE, e->palLo, e->palLen);
        if (p > ver) ver = p;
    }
    if (e && e->ver >= ver) {
        e->used = sFrame, e->checked = sVer;
        return last = e;
    }
    int w = 8 << (tp >> 20 & 7), h = 8 << (tp >> 23 & 7);
    u32 *rgba = malloc((size_t)w * h * 4);
    if (!rgba) return 0;
    if (!e) {
        e = lru;
        if (!e->live) glGenTextures(1, &e->tex);
        e->live = 1, e->key = key, e->pal = pal;
    }
    g3_texture_rgba(tp, tpal, rgba);  // (r << 24 | g << 16 | b << 8 | a: as bytes a, b, g, r)
    sDecoded++;
    for (int i = 0; i < w * h; i++) rgba[i] = __builtin_bswap32(rgba[i]);
    // (the texture bound before put back after: draw_pass may still have a draw pending with it)
    GLint bound = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
    glBindTexture(GL_TEXTURE_2D, e->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    e->ws = e->wt = 0;  // (its wrap set again at its first draw)
    glBindTexture(GL_TEXTURE_2D, (GLuint)bound);
    free(rgba);
    if (is4x4) {
        palette_4x4(tp, tpal, &e->palLo, &e->palLen);
        u32 p = newest(sPalVer, PAL_SIZE, e->palLo, e->palLen);
        if (p > ver) ver = p;
    }
    e->ver = ver, e->used = sFrame, e->checked = sVer;
    return last = e;
}
static GLint wrap(u32 tp, int axis) {
    if (!(tp >> (16 + axis) & 1)) return GL_CLAMP_TO_EDGE;
    return tp >> (18 + axis) & 1 ? GL_MIRRORED_REPEAT : GL_REPEAT;
}

// ---- a picture
// (kSize: gpu3d_lists.h's)
static u32 rgb5_to_6(u32 c) { return c ? c * 2 + 1 : 0; }

// The GL state a batch wants, set only where it differs from the batch before's (sCur; reset at each picture): a
// state change costs Mesa's driver on the Switch's CPU, and most batches in a row differ in their texture only
typedef struct { GLuint tex; int mode, wbuffer, tsize, depthEq, depthWrite, trans, ref, mask, write, blend, wrap; } GlState;
static GlState sCur;
// a batch's GL state (its texture decoded or found first)
static GlState batch_state(const G3RenderState *r, const Batch *b, TexEntry **texOut) {
    int fmt = b->texparam >> 26 & 7, trans = b->trans, fog = b->attr >> 15 & 1, id = (int)(b->attr >> 24 & 63);
    TexEntry *tex = fmt && (r->dispcnt & 1) ? texture(b->texparam, b->texpal) : 0;
    *texOut = tex;
    GlState n = {tex ? tex->tex : 0, !tex ? 0 : (b->attr >> 4 & 3) == 1 ? 2 : 1, b->wbuffer, tex ? (int)(b->texparam >> 20 & 0x3f) : -2,
                 (b->attr & 0x4000) != 0, !trans || (b->attr & 0x800), trans,
                 // not over its own ID (bits 0-6), which it then writes; the fog flag cleared unless it has fog
                 trans ? 0x40 | id : fog ? 0x80 : 0, trans ? 0x7f : 0xff, trans ? (fog ? 0x7f : 0xff) : 0x80,
                 trans && (r->dispcnt & 8)};
    // (and the texture's wrap: in the key as the texture's own state, below)
    n.wrap = tex ? (int)(b->texparam >> 16 & 15) : -2;
    return n;
}
// The GL state set, where it differs from what is set (sCur)
static void apply_state(const GlState *n, TexEntry *tex, u32 texparam) {
    if (tex) {
        if (n->tex != sCur.tex) glBindTexture(GL_TEXTURE_2D, n->tex);
        GLint ws = wrap(texparam, 0), wt = wrap(texparam, 1);
        if (ws != tex->ws) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex->ws = ws);
        if (wt != tex->wt) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex->wt = wt);
        if (n->tsize != sCur.tsize) glUniform4f(sTscaleLoc, kSize[n->tsize & 7], kSize[n->tsize >> 3], 0, 0);
    }
    if (n->mode != sCur.mode) glUniform1i(sModeLoc, n->mode);
    if (n->wbuffer != sCur.wbuffer) glUniform1i(sWbufLoc, n->wbuffer);
    if (n->depthEq != sCur.depthEq) glDepthFunc(n->depthEq ? GL_LEQUAL : GL_LESS);
    if (n->depthWrite != sCur.depthWrite) glDepthMask(n->depthWrite ? GL_TRUE : GL_FALSE);
    if (n->trans != sCur.trans || n->ref != sCur.ref || n->mask != sCur.mask)
        glStencilFunc(n->trans ? GL_NOTEQUAL : GL_ALWAYS, n->ref, (GLuint)n->mask);
    if (n->write != sCur.write) glStencilMask((GLuint)n->write);
    if (n->trans != sCur.trans || n->blend != sCur.blend) {
        if (!n->trans) glDisable(GL_BLEND);
        else {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
            glBlendFuncSeparate(GL_ONE, n->blend ? GL_ONE_MINUS_SRC_ALPHA : GL_ZERO, GL_ONE, GL_ONE);
        }
    }
    GlState keep = *n;
    if (!tex) keep.tex = sCur.tex, keep.tsize = sCur.tsize;  // (untextured: the binding and its size stay as they are)
    sCur = keep;
}
// A pass's batches (opaque, or translucent: the game sorts its translucent ones itself), in their order; batches in
// a row whose GL state is the same and whose vertices follow on in the buffer drawn as one (the geometry engine's
// batches also differ in what GL doesn't use here: the polygon ID of an opaque one, the lights, the viewport)
static u32 sDraws, sBatches;
static void draw_pass(const G3RenderState *r, const List *l, int pass) {
    int first = -1, count = 0;
    GlState run = {0};
    for (int k = 0; k < l->nbatch; k++) {
        const Batch *b = &l->batch[k];
        if (b->trans != pass || b->gpu || !b->count) continue;
        sBatches++;
        TexEntry *tex;
        GlState n = batch_state(r, b, &tex);
        if (first >= 0 && b->first == first + count && !memcmp(&n, &run, sizeof n)) {
            count += b->count;
            continue;
        }
        if (first >= 0) glDrawArrays(GL_TRIANGLES, first, count), sDraws++;
        apply_state(&n, tex, b->texparam);
        run = n, first = b->first, count = b->count;
    }
    if (first >= 0) glDrawArrays(GL_TRIANGLES, first, count), sDraws++;
}

// ---- the pictures between's vertices made ahead on core 2: after a picture between, the next one's (the same list,
// t one frame further) is replayed there while the game's thread goes on; the next g3p_render takes it if it is that
// one, else waits for the worker and makes its own (as before). ACWW_GL_AHEAD=0: never ahead.
static Thread sAheadThread;
static Mutex sAheadLock;
static CondVar sAheadCv;
static struct { const List *l; s64 t; PVertex *out; int busy, done, list, num, den; } sAhead;
static int sAheadOn = -1;
static void ahead_main(void *arg) {
    (void)arg;
    mutexLock(&sAheadLock);
    for (;;) {
        while (!sAhead.busy) condvarWait(&sAheadCv, &sAheadLock);
        mutexUnlock(&sAheadLock);
        replay(sAhead.l, sAhead.t, sAhead.out);
        mutexLock(&sAheadLock);
        sAhead.busy = 0, sAhead.done = 1;
        condvarWakeAll(&sAheadCv);
    }
}
static int ahead_start(void) {
    if (sAheadOn >= 0) return sAheadOn;
    sAheadOn = 0;
    if (getenv("ACWW_GL_AHEAD") && !atoi(getenv("ACWW_GL_AHEAD"))) return 0;
    mutexInit(&sAheadLock);
    condvarInit(&sAheadCv);
    s32 prio = 0x2c;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    if (R_FAILED(threadCreate(&sAheadThread, ahead_main, 0, 0, 64 * 1024, prio, 2)) || R_FAILED(threadStart(&sAheadThread)))
        return 0;
    fprintf(stderr, "gl: the pictures between made ahead on core 2\n");
    return sAheadOn = 1;
}
// until the worker is idle; whether it made (list, num, den)'s vertices (then in sAhead.out)
static int ahead_take(int list, int num, int den) {
    if (sAheadOn <= 0) return 0;
    mutexLock(&sAheadLock);
    if (sAhead.busy) {
        HOST_PROF(PROF_WAIT);
        while (sAhead.busy) condvarWait(&sAheadCv, &sAheadLock);
    }
    int hit = sAhead.done && sAhead.list == list && sAhead.num == num && sAhead.den == den;
    sAhead.done = 0;
    mutexUnlock(&sAheadLock);
    return hit;
}
static void ahead_give(const List *l, int list, int num, int den, PVertex *out) {
    if (!ahead_start()) return;
    mutexLock(&sAheadLock);
    sAhead.l = l, sAhead.t = ((s64)num << 16) / den, sAhead.out = out;
    sAhead.list = list, sAhead.num = num, sAhead.den = den, sAhead.busy = 1, sAhead.done = 0;
    condvarWakeAll(&sAheadCv);
    mutexUnlock(&sAheadLock);
}

// ACWW_FPS: every 300 pictures, ms a picture in g3p_render's parts (the pictures between's vertices; the clear, the
// set-up and the vertices' upload; the draws), the draws and batches a picture, and the textures decoded
static void gl_stats(u64 t0, u64 t1, u64 t2, u64 t3, int direct) {
    static int on = -1;
    static u64 prep, setup, draws;
    static u32 pictures;
    if (on < 0) on = getenv("ACWW_FPS") != 0;
    if (!on || !direct) return;
    prep += t1 - t0, setup += t2 - t1, draws += t3 - t2;
    if (++pictures % 300) return;
    fprintf(stderr, "gl3d: ms a picture: between %.2f, set-up and upload %.2f, draws %.2f; %u draws of %u batches, %u textures decoded\n",
            prep / 300e6, setup / 300e6, draws / 300e6, sDraws / 300, sBatches / 300, sDecoded);
    prep = setup = draws = 0, sDraws = sBatches = sDecoded = 0;
}

static int sAheadNext, sAheadDen;
void g3p_render(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, u32 *color, u32 *depth, u8 *flags) {
    if (!sReady) return;
    u64 t0 = host_plat_time_ns();
    gld_enter();
    sFrame++;
    if (texChanged) {  // the texture VRAM's versions brought up to date, as gpu3d_pica.c prepare
        HOST_PROF(PROF_VRAM);
        for (int k = 0; k < gG3TexParts; k++) {  // (only the parts the uploads changed: gpu3d_tex.h)
            u32 at = gG3TexPart[k][0] & 0x7fffffff, end = at + gG3TexPart[k][1];
            int p = gG3TexPart[k][0] >> 31;
            for (u32 f = at / FINE * FINE; f < end; f += FINE)
                if (memcmp((p ? sPalCopy : sImgCopy) + f, (p ? pal : img) + f, FINE)) {
                    memcpy((p ? sPalCopy : sImgCopy) + f, (p ? pal : img) + f, FINE);
                    (p ? sPalVer : sImgVer)[f / FINE] = ++sVer;
                }
        }
        if (gG3TexParts < 0) {
            track(img, sImgCopy, sImgVer, IMG_SIZE);
            track(pal, sPalCopy, sPalVer, PAL_SIZE);
        }
    }
    List *l = &sList[r->list];
    // (for the compositing, color 0: into the next picture, its first row the top one as gl_display.c's targets; the
    // DS's picture GPU3D_WIDE_X / Y in from its corner with the wide view)
    const int direct = !color, wide = direct && gpu3d_wide();
    if (direct) sPicture = sPicture < 0 ? 0 : sPicture ^ 1;
    // the pictures between the game's, as gpu3d_pica.c g3p_render and prepare: a new list paired with the one before
    // (pair), then each frame till the next drawn again at t = num / den between them (replay)
    PVertex *vb = l->v;
    if (direct) {
        int prev = -1;
        if (r->list != sShownList) {  // (a new list: the frames since the one before)
            sPeriod = sShownList >= 0 ? (int)(sFrames - sListFrame) : 0;
            prev = sShownList;
            sShownList = r->list, sListFrame = sFrames;
        }
        int num = (int)(sFrames - sListFrame) + 1, interp = interp_on() && l->recorded;
        int den = interp && sPeriod >= 2 && sPeriod <= 4 && num < sPeriod ? sPeriod : 0;
        int ahead = ahead_take(r->list, num, den);  // (the worker idle from here on: the lists and buffers this thread's)
        if (den && prev >= 0) pair(l, &sList[prev]);  // (shortcut, as gpu3d_pica.c: a list first drawn without its
                                                      // pictures between isn't paired later)
        if (den && sReplay[0]) {
            vb = sReplay[sReplayBuf ^= 1];
            if (!ahead) replay(l, ((s64)num << 16) / den, vb);
        }
        sAheadNext = den && sReplay[0] && num + 1 < den ? num + 1 : 0, sAheadDen = den;  // (the next frame's, if it is one)
    }
    u64 t1 = host_plat_time_ns();
    const float tw = direct ? (wide ? 512.0f : 256.0f) : W, th = direct ? 256.0f : H;  // (as gpu3d_pica.c)
    const float tx = 256 / tw, txw = (256 + 2 * (wide ? GPU3D_WIDE_X : 0)) / tw - 1;
    const float ty = 192 / th, tyw = 1 - (192 + 2 * (wide ? GPU3D_WIDE_Y : 0)) / th;
    const float target[16] = {tx, 0, 0, txw, 0, direct ? -ty : ty, 0, direct ? -tyw : tyw, 0, 0, 1, 0, 0, 0, 0, 1};
    glBindFramebuffer(GL_FRAMEBUFFER, direct ? sPic[sPicture].fbo : sFbo);
    const int k = direct ? sPic[0].k : 1;  // (the picture for the compositing drawn gld_scale() times as large)
    glViewport(0, 0, (GLsizei)tw * k, (GLsizei)th * k);
    u32 cc = r->clearColor, ca = r->clearAttrHi & 31, a8 = ca * 255 / 31;
    u32 cr = rgb5_to_6(cc & 31), cg = rgb5_to_6(cc >> 5 & 31), cb = rgb5_to_6(cc >> 10 & 31);
    u32 cd = (u32)(r->clearDepth & 0x7fff) * 0x200 + ((r->clearDepth & 0x7fff) == 0x7fff ? 0x1ff : 0);
    glDepthMask(GL_TRUE);
    glStencilMask(0xff);
    glClearColor(cr / 63.0f * a8 / 255, cg / 63.0f * a8 / 255, cb / 63.0f * a8 / 255, a8 / 255.0f);
    glClearDepthf(cd / 16777215.0f);
    glClearStencil(cc & 0x8000 ? 0x80 : 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glUseProgram(sProg);
    glUniform4fv(sTargetLoc, 4, target);
    glUniform4f(sCscaleLoc, 1 / 63.0f, 1 / 63.0f, 1 / 63.0f, 1 / 31.0f);
    glUniform1f(sArefLoc, r->dispcnt & 4 ? (float)((r->alphaRef * 255 + 15) / 31) / 255.0f : 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glDisable(GL_CULL_FACE);  // (the geometry engine culled already)
    glBindVertexArray(sVao);
    glBindBuffer(GL_ARRAY_BUFFER, sVbo);
    // (a new buffer each picture, "orphaning": the one the GPU may still draw the picture before from isn't written,
    // which would make the driver wait for it)
    glBufferData(GL_ARRAY_BUFFER, LIST_VERTS * sizeof(PVertex), 0, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)l->nv * (GLsizeiptr)sizeof(PVertex), vb);
    // (copied by the driver: the other replay buffer the worker's now, for the next frame's picture between)
    if (direct && sAheadNext) ahead_give(l, r->list, sAheadNext, sAheadDen, sReplay[sReplayBuf ^ 1]);
    sCur = (GlState){0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};  // (all set by the first batch)
    glActiveTexture(GL_TEXTURE0);
    u64 t2 = host_plat_time_ns();
    for (int pass = 0; pass < 2; pass++) draw_pass(r, l, pass);
    u64 t3 = host_plat_time_ns();
    gl_stats(t0, t1, t2, t3, direct);
    if (direct) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        gld_leave();
        return;
    }
    glReadPixels(0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE, sPixels);
    gld_leave();
    // into the rasteriser's formats (the rows bottom up): the colour back from premultiplied, 6 bits; alpha 5 bits
    u32 clear = cr | cg << 8 | cb << 16;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            const u8 *p = &sPixels[((H - 1 - y) * W + x) * 4];
            u32 a = p[3], c = clear;
            if (a) {
                u32 rr = (p[0] * 63u + a / 2) / a, gg = (p[1] * 63u + a / 2) / a, bb = (p[2] * 63u + a / 2) / a;
                c = (rr > 63 ? 63 : rr) | (gg > 63 ? 63 : gg) << 8 | (bb > 63 ? 63 : bb) << 16;
            }
            color[y * W + x] = c | ((a * 31 + 127) / 255) << 24;
        }
    if (r->dispcnt & 0x80)  // (the fog: no pixel flagged, so the fog pass changes nothing)
        for (int i = 0; i < W * H; i++) depth[i] = 0xffffff, flags[i] = 0;
}
void *g3p_picture(void) { return sPicture >= 0 ? &sPic[sPicture] : 0; }
void g3p_wait(void) {}

#endif

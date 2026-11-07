// Host build: the display interface (render.h) and the choice of renderer.
//
// The display is the project's own 2D renderer (gpu2d/, docs/portability.md 5.1) and 3D engine (gpu3d/, 5.2).
// melonDS (melon.cpp, GPL-3.0) is linked into the oracle build only (tools/configure_host.py --oracle; 5.3), as a
// reference renderer; the other builds have melon_none.c, which stops with a message when it is asked for:
//   ACWW_RENDERER=own (default) | melon   whose 2D pictures the screens show
//   ACWW_RENDERER3D=own | wgpu | melon    whose 3D engine runs the game's 3D (wgpu: the own engine, its polygons
//                                         drawn by the GPU: gpu3d/gpu3d_wgpu.c): answers its reads (GXSTAT, the test
//                                         results, matrices, RAM counts), raises the GX FIFO interrupt and draws the
//                                         3D layer. Default: wgpu where this build and machine have WebGPU (else own,
//                                         silently), own when comparing with melonDS (ACWW_RENDER_COMPARE, the
//                                         self-tests), melon with ACWW_RENDERER=melon. `own` needs the own 2D renderer
//                                         too (melonDS's 2D would show its own 3D), so it implies ACWW_RENDERER=own.
//                                         With both `own` melonDS isn't run at all. With wgpu the GPU composites the
//                                         screens too and shows them (wgpu_display.c): the 2D renderer leaves the 3D
//                                         layer to it, and the frames stay on the GPU (read back only for screenshots)
//   ACWW_SCALE3D=<n>                      wgpu: the 3D picture at n times the DS's resolution (1-8, default 2;
//                                         the window shows 2x; screenshots average the 3D picture's pixels)
//   ACWW_GPU2D=0                          wgpu: the 2D renderer's pictures drawn by the CPU, not by the GPU (default:
//                                         gpu2d.wgsl; the CPU keeps the per-line state and the H-blank; 12 times less
//                                         CPU time than drawing them); ACWW_GPU2D_CHECK=1: drawn by
//                                         both, the GPU's checked against the CPU's every frame (the differing
//                                         frames reported, a count at exit)
//   ACWW_GPU2D_SELFTEST=<n>               wgpu: before the game starts, n random scenes (render_selftest.c) drawn by
//                                         the CPU and the GPU and compared as with ACWW_GPU2D_CHECK, then exit
//   ACWW_WGPU_READBACK=1                  wgpu: the 3D picture read back, the screens composited on the CPU (as
//                                         with ACWW_RENDER_COMPARE; the GPU's compositing checked against it)
//   ACWW_RENDER_COMPARE=1                 render with both every frame and report the frames where they differ: the
//                                         first differing pixel of each screen and what the own renderer put there
//                                         (layers, effect); for the 3D engine also every read whose value differs,
//                                         the polygon lists (count, first differing polygon) and the 3D pictures
//                                         (first differing pixel, the polygon that drew it); ACWW_RENDER_COMPARE_MAX
//                                         lines at most of each kind (default 200), then only the counts, which are
//                                         printed at exit
//   ACWW_RENDER_SELFTEST=<n>              before the game starts: n random scenes rendered by both, compared as above,
//                                         then exit (render_selftest.c; ACWW_RENDER_SELFTEST_SEED)
//   ACWW_GPU2D_FEATURES=1                 at exit, the 2D features the frames used (own renderer or comparison)
//   ACWW_GPU3D_FEATURES=1                 at exit, the 3D features the frames used (own 3D engine or comparison)
// Both renderers get the same register writes, VRAM banks, palettes and OAM.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpu2d/gpu2d.h"
#include "gpu3d/gpu3d.h"
#include "host_mem.h"
#include "plat.h"
#include "pica_display.h"
#include "render.h"
#include "wgpu_display.h"

// melon.cpp
void melon_init(void);
void melon_write8(u32 addr, u8 v);
void melon_write16(u32 addr, u16 v);
void melon_write32(u32 addr, u32 v);
u32 melon_read32(u32 addr);
void melon_frame(void);
const uint32_t *melon_screen(int screen);
int melon_gx_fifo_irq_level(void);
void melon_debug(void);
void melon_3d_lines(uint32_t *out);
int melon_3d_poly_count(void);
int melon_3d_poly_get(int i, Gpu3dVertexOut *v, uint32_t *attr, uint32_t *texparam, uint32_t *texpal, int *flags);
// hw.c
int host_hblank(int line);
// vram.c
u8 *host_vram_bank(int i);
void host_vram_flush(void);

static int sOwn;          // the own 2D renderer's pictures on the screens
static int sOwn3d;        // the own 3D engine runs the game's 3D
static int sGpu2d;        // the own 2D renderer runs (sOwn or sCompare)
static int sGpu3d;        // the own 3D engine runs (sOwn3d or sCompare)
static int sMelon;        // melonDS runs
static int sCompare;
static int sGpuDisplay;   // the screens composited by the GPU (wgpu_display.c)
static int sPica;         // the screens composited and shown by the 3DS's GPU (pica_display.c)
static int sScale3d = 2;  // ACWW_SCALE3D
static int sGpu2dMode;    // ACWW_GPU2D: 1 the 2D pictures drawn by the GPU, 2 checked against the CPU's
static u64 sGpu2dChecked, sGpu2dDiffering;
static int sSelftest;      // ACWW_RENDER_SELFTEST (render_selftest.c compares itself)
static long sCompareMax = 200, sCompareLines;
static u64 sCompared, sDiffering, sFirstDiffering = ~0ull;
static u64 sReads3d, sReadsDiffering, sFirstReadDiffering = ~0ull;
static long sReadLines;
static u64 sPolyFrames, sPolyDiffering, sFirstPolyDiffering = ~0ull;
static long sPolyLines;
static u64 sPix3dFrames, sPix3dDiffering, sFirstPix3dDiffering = ~0ull;
static long sPix3dLines;
static uint32_t s3d[256 * 192];

static int env_is(const char *name, const char *value) {
    const char *r = getenv(name);
    return r && strcmp(r, value) == 0;
}

void render_init(void) {
    sOwn = !env_is("ACWW_RENDERER", "melon");
    if (getenv("ACWW_RENDERER") && sOwn && !env_is("ACWW_RENDERER", "own"))
        fprintf(stderr, "render: ACWW_RENDERER=%s unknown (own, melon): own\n", getenv("ACWW_RENDERER"));
    sOwn3d = getenv("ACWW_RENDERER3D") ? !env_is("ACWW_RENDERER3D", "melon") : sOwn;
    if (getenv("ACWW_RENDERER3D") && sOwn3d && !env_is("ACWW_RENDERER3D", "own") && !env_is("ACWW_RENDERER3D", "wgpu") &&
        !env_is("ACWW_RENDERER3D", "pica"))
        fprintf(stderr, "render: ACWW_RENDERER3D=%s unknown (own, wgpu, pica, melon): own\n", getenv("ACWW_RENDERER3D"));
    if (sOwn3d) sOwn = 1;
    sCompare = getenv("ACWW_RENDER_COMPARE") ? atoi(getenv("ACWW_RENDER_COMPARE")) : 0;
    // the GPU by default (the own renderers, not compared with melonDS's)
    int asked = env_is("ACWW_RENDERER3D", "wgpu");
    int wgpu = asked || (!getenv("ACWW_RENDERER3D") && sOwn3d && !sCompare && !getenv("ACWW_RENDER_SELFTEST") &&
                         !getenv("ACWW_RENDER_SELFTEST3D"));
    if (wgpu) {
        // (compared with melonDS: the 3D picture read back, the screens composited on the CPU)
        int readback = sCompare || getenv("ACWW_RENDER_SELFTEST") || getenv("ACWW_RENDER_SELFTEST3D") ||
                       env_is("ACWW_WGPU_READBACK", "1");
        if (getenv("ACWW_SCALE3D")) sScale3d = atoi(getenv("ACWW_SCALE3D"));
        sScale3d = readback || sScale3d < 1 ? 1 : sScale3d > 8 ? 8 : sScale3d;
        if (wdisp_init(asked) && gpu3d_use_wgpu(wdisp_device(), wdisp_queue(), sScale3d, readback)) sGpuDisplay = !readback;
        else if (asked) fprintf(stderr, "render: ACWW_RENDERER3D=wgpu: no WebGPU in this build or on this machine: own\n");
        if (sGpuDisplay && !env_is("ACWW_GPU2D", "0")) sGpu2dMode = env_is("ACWW_GPU2D_CHECK", "1") ? 2 : 1;
        if (sGpuDisplay && getenv("ACWW_GPU2D_SELFTEST")) sGpu2dMode = 2;
    }
#ifdef __3DS__
    // the 3DS: the polygons drawn by its GPU (gpu3d/gpu3d_pica.c), unless ACWW_RENDERER3D=own; the screens
    // composited by it (pica_display.c), unless ACWW_COMPOSITE=cpu
    int pica3d = sOwn3d && !env_is("ACWW_RENDERER3D", "own") && gpu3d_use_pica();
    if (sOwn3d && !env_is("ACWW_RENDERER3D", "own") && !pica3d)
        fprintf(stderr, "render: the 3DS GPU's renderer couldn't start: own\n");
    if (sOwn && !sCompare && !env_is("ACWW_COMPOSITE", "cpu")) {
        sPica = pdisp_init();
        if (!sPica) fprintf(stderr, "render: the 3DS GPU's compositing couldn't start: the CPU's\n");
        gpu3d_pica_direct(sPica);  // (its 3D pictures then straight from the GPU, not read back)
        gpu3d_set_wide(sPica && pica3d && gpu3d_wide_wanted());  // (ACWW_WIDE: drawn only by these two)
    }
#endif
    if (getenv("ACWW_RENDER_COMPARE_MAX")) sCompareMax = strtol(getenv("ACWW_RENDER_COMPARE_MAX"), 0, 10);
    if (getenv("ACWW_RENDER_SELFTEST") || getenv("ACWW_RENDER_SELFTEST3D")) sCompare = sCompare ? sCompare : 1;
    sGpu2d = sOwn || sCompare;
    sGpu3d = sOwn3d || sCompare;
    sMelon = !sOwn || !sOwn3d || sCompare;
    if (sMelon) melon_init();
    gpu2d_reset();
    gpu2d_set_diagnostics(sCompare || sGpu2dMode == 2);
    gpu3d_reset();
    gpu3d_set_diagnostics(sCompare);
    if (getenv("ACWW_GPU2D_SELFTEST")) {  // (render_selftest.c: the GPU's 2D pictures against the CPU's)
        if (sGpu2dMode != 2) {
            fprintf(stderr, "render: ACWW_GPU2D_SELFTEST needs ACWW_RENDERER3D=wgpu (and WebGPU)\n");
            exit(1);
        }
        sSelftest = 1;
        render_selftest_gpu2d(strtol(getenv("ACWW_GPU2D_SELFTEST"), 0, 10));
    }
    if (getenv("ACWW_RENDER_SELFTEST")) {  // (render_selftest.c)
        sGpu2d = sGpu3d = sCompare = sSelftest = sMelon = 1;
        render_selftest(strtol(getenv("ACWW_RENDER_SELFTEST"), 0, 10));
    }
    if (getenv("ACWW_RENDER_SELFTEST3D")) {  // (render_selftest3d.c)
        void render_selftest3d(long scenes) __attribute__((noreturn));
        sGpu2d = sGpu3d = sSelftest = sMelon = 1;
        render_selftest3d(strtol(getenv("ACWW_RENDER_SELFTEST3D"), 0, 10));
    }
}

static int is_3d(u32 addr) {
    return addr == 0x04000060 || addr == 0x04000304 || (addr >= 0x04000240 && addr < 0x04000247) ||
           (addr >= 0x04000320 && addr < 0x040006a4);
}
void render_write8(u32 addr, u8 v) {
    if (sMelon) melon_write8(addr, v);
    if (sGpu2d) gpu2d_write8(addr, v);
    if (sGpu3d && is_3d(addr)) gpu3d_write8(addr, v);
}
void render_write16(u32 addr, u16 v) {
    if (sMelon) melon_write16(addr, v);
    if (sGpu2d) gpu2d_write16(addr, v);
    if (sGpu3d && is_3d(addr)) gpu3d_write16(addr, v);
}
// ACWW_RENDER_COMPARE=2: the geometry engines' state (FIFO levels, the swap, RAM counts, matrices and stacks, lights)
// compared after every command write; the first write after which they differ is reported, with the own engine's
// last commands
void melon_3d_state(Gpu3dState *s);
static long sStateLines;
static void compare_state(u32 addr, u32 v) {
    Gpu3dState a, b;
    memset(&a, 0, sizeof a);
    memset(&b, 0, sizeof b);
    gpu3d_state(&a);
    melon_3d_state(&b);
    a.pipe |= gpu3d_read32(0x04000600) & 0xff00ff00;
    b.pipe |= melon_read32(0x04000600) & 0xff00ff00;
    {   // (ACWW_RENDER_COMPARE=2: melonDS's polygon setup state by the kind of polygon, printed at exit)
        int nout, clipped, type;
        unsigned serial;
        static unsigned last;
        int outcome = gpu3d_debug_last_polygon(&nout, &clipped, &serial, &type);
        if (serial != last) {
            last = serial;
            static struct { int key[4]; int pp, slots, ctr, pv; long n; } tab[512];
            static int ntab;
            int key[4] = {outcome, nout, clipped, type};
            int i;
            for (i = 0; i < ntab; i++)
                if (!memcmp(tab[i].key, key, sizeof key) && tab[i].pp == b.pp && tab[i].slots == b.slots &&
                    tab[i].ctr == b.slotctr)
                    break;
            if (i == ntab && ntab < 512) {
                memcpy(tab[i].key, key, sizeof key);
                tab[i].pp = b.pp, tab[i].slots = b.slots, tab[i].ctr = b.slotctr;
                ntab++;
                fprintf(stderr, "polysetup: outcome %d nout %d clipped %d type %d -> melon pp %d slots %x ctr %d (vp %d)\n",
                        outcome, nout, clipped, type, b.pp, b.slots, b.slotctr, b.vp);
            }
        }
    }
    if (!memcmp(&a, &b, sizeof a) || sStateLines >= sCompareMax) return;
    sStateLines++;
    fprintf(stderr, "render-compare: frame %llu after write %08x = %08x the geometry state differs:\n",
            (unsigned long long)host_frame(), addr, v);
    fprintf(stderr, "  own   pipe %x fifo %u flush %u polys %u verts %u mode %u possp %u projsp %u\n", a.pipe, a.fifo,
            a.flush, a.npolys, a.nverts, a.mtxmode, a.possp, a.projsp);
    fprintf(stderr, "  melon pipe %x fifo %u flush %u polys %u verts %u mode %u possp %u projsp %u\n", b.pipe, b.fifo,
            b.flush, b.npolys, b.nverts, b.mtxmode, b.possp, b.projsp);
    fprintf(stderr, "  pipelines own vp %d np %d pp %d slots %x ctr %d, melon vp %d np %d pp %d slots %x ctr %d\n", a.vp, a.np,
            a.pp, a.slots, a.slotctr, b.vp, b.np, b.pp, b.slots, b.slotctr);
    const int32_t *ma[4] = {a.proj, a.pos, a.vec, a.tex}, *mb[4] = {b.proj, b.pos, b.vec, b.tex};
    for (int m = 0; m < 4; m++)
        if (memcmp(ma[m], mb[m], 64)) {
            fprintf(stderr, "  %s matrix:", (const char *[]){"projection", "position", "vector", "texture"}[m]);
            for (int i = 0; i < 16; i++) fprintf(stderr, " %x/%x", ma[m][i], mb[m][i]);
            fprintf(stderr, "\n");
        }
    if (memcmp(a.light, b.light, sizeof a.light)) {
        fprintf(stderr, "  lights:");
        for (int l = 0; l < 4; l++)
            fprintf(stderr, " %d,%d,%d/%d,%d,%d", a.light[l][0], a.light[l][1], a.light[l][2], b.light[l][0], b.light[l][1],
                    b.light[l][2]);
        fprintf(stderr, "\n");
    }
    if (memcmp(a.lightColor, b.lightColor, sizeof a.lightColor) || memcmp(a.material, b.material, sizeof a.material)) {
        fprintf(stderr, "  light colours / material (diffuse, ambient, specular, emission), own/melon:");
        for (int l = 0; l < 4; l++)
            fprintf(stderr, " %d,%d,%d/%d,%d,%d", a.lightColor[l][0], a.lightColor[l][1], a.lightColor[l][2],
                    b.lightColor[l][0], b.lightColor[l][1], b.lightColor[l][2]);
        fprintf(stderr, " |");
        for (int m = 0; m < 4; m++)
            fprintf(stderr, " %d,%d,%d/%d,%d,%d", a.material[m][0], a.material[m][1], a.material[m][2], b.material[m][0],
                    b.material[m][1], b.material[m][2]);
        fprintf(stderr, "\n");
    }
    if (memcmp(a.normal, b.normal, sizeof a.normal) || memcmp(a.vtxColor, b.vtxColor, 3))
        fprintf(stderr, "  normal %d,%d,%d / %d,%d,%d  vertex colour %d,%d,%d / %d,%d,%d\n", a.normal[0], a.normal[1],
                a.normal[2], b.normal[0], b.normal[1], b.normal[2], a.vtxColor[0], a.vtxColor[1], a.vtxColor[2],
                b.vtxColor[0], b.vtxColor[1], b.vtxColor[2]);
    if (memcmp(a.vtxColor, b.vtxColor, 3)) {
        fprintf(stderr, "  vector matrix:");
        for (int i = 0; i < 16; i++) fprintf(stderr, " %d", a.vec[i]);
        fprintf(stderr, "\n  lights:");
        for (int l = 0; l < 4; l++)
            fprintf(stderr, " %d,%d,%d c%d,%d,%d", a.light[l][0], a.light[l][1], a.light[l][2], a.lightColor[l][0],
                    a.lightColor[l][1], a.lightColor[l][2]);
        fprintf(stderr, "\n  material:");
        for (int m = 0; m < 4; m++) fprintf(stderr, " %d,%d,%d", a.material[m][0], a.material[m][1], a.material[m][2]);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "  own commands:");
    gpu3d_debug_history();
}
void render_write32(u32 addr, u32 v) {
    if (sMelon) melon_write32(addr, v);
    if (sGpu2d && (addr < 0x04000400 || addr > 0x04000600)) gpu2d_write32(addr, v);  // (not the 3D command ports)
    if (sGpu3d && is_3d(addr)) {
        HOST_PROF(PROF_GEOM);
        gpu3d_write32(addr, v);
    }
    if (sCompare > 1 && !sSelftest && addr >= 0x04000400 && addr <= 0x04000600) compare_state(addr, v);
}

void render_write32_n(u32 addr, const u32 *v, u32 n) {
    // (as render_write32 for a 3D command port: not the 2D renderer's; melonDS and the comparisons word by word)
    if (sMelon || sCompare > 1 || !sGpu3d || !is_3d(addr) || addr < 0x04000400 || addr >= 0x04000600) {
        for (u32 i = 0; i < n; i++) render_write32(addr, v[i]);
        return;
    }
    HOST_PROF(PROF_GEOM);
    gpu3d_write32_n(addr, (const uint32_t *)v, n);  // (u32 is unsigned int; uint32_t on the 3DS unsigned long)
}

u32 render_read32(u32 addr) {
    HOST_PROF(PROF_GEOM);
    if (!sOwn3d) {
        u32 m = melon_read32(addr);
        if (sCompare) {
            u32 o = gpu3d_read32(addr);
            sReads3d++;
            if (o != m) {
                if (sFirstReadDiffering == ~0ull) sFirstReadDiffering = host_frame();
                sReadsDiffering++;
                { void gpu3d_debug_history(void); if (sReadLines < 3) gpu3d_debug_history(); }
                if (sReadLines++ < sCompareMax)
                    fprintf(stderr, "render-compare: frame %llu read %08x: own %08x melon %08x\n",
                            (unsigned long long)host_frame(), addr, o, m);
            }
        }
        return m;
    }
    u32 o = gpu3d_read32(addr);
    if (sCompare) {
        u32 m = melon_read32(addr);
        sReads3d++;
        if (o != m) {
            if (sFirstReadDiffering == ~0ull) sFirstReadDiffering = host_frame();
            sReadsDiffering++;
            if (sReadLines++ < sCompareMax)
                fprintf(stderr, "render-compare: frame %llu read %08x: own %08x melon %08x\n",
                        (unsigned long long)host_frame(), addr, o, m);
        }
    }
    return o;
}
// the GX FIFO interrupt (IRQ 21) is level-triggered: GXSTAT bits 30-31 choose "less than half full" or "empty"; the
// host runs commands as they are written, so the line is up whenever one of the two is chosen
int render_gx_fifo_irq_level(void) {
    if (!sOwn3d) return melon_gx_fifo_irq_level();
    u32 mode = gpu3d_read32(0x04000600) >> 30;
    return mode == 1 || mode == 2;
}
void render_debug(void) { if (sMelon) melon_debug(); }

// the first pixel where the two renderers' pictures differ, per screen (`label`: the frame, or the self-test's
// scene); 1 if they differ
int render_compare(const char *kind, u64 label) {
    sCompared++;
    int differs = 0;
    for (int s = 0; s < 2; s++) {
        const uint32_t *m = melon_screen(s), *o = gpu2d_screen(s);
        int first = -1, count = 0;
        for (int i = 0; i < 256 * 192; i++)
            if ((m[i] ^ o[i]) & 0xffffff) { if (first < 0) first = i; count++; }
        if (first < 0) continue;
        differs = 1;
        if (sCompareLines < sCompareMax) {
            char what[256];
            gpu2d_describe_pixel(s, first % 256, first / 256, what, sizeof what);
            fprintf(stderr, "render-compare: %s %llu %s screen: %d pixels differ, first (%d, %d) own %06x melon %06x: %s\n",
                    kind, (unsigned long long)label, s ? "bottom" : "top", count, first % 256, first / 256,
                    o[first] & 0xffffff, m[first] & 0xffffff, what);
            if (++sCompareLines == sCompareMax) fprintf(stderr, "render-compare: (no more lines: ACWW_RENDER_COMPARE_MAX)\n");
        }
    }
    if (differs) {
        if (sFirstDiffering == ~0ull) sFirstDiffering = label;
        sDiffering++;
    }
    return differs;
}

static void print_poly(const char *who, const Gpu3dVertexOut *v, int n, u32 attr, u32 tp, u32 pal, int flags) {
    fprintf(stderr, "    %s: attr %08x tex %08x pal %04x flags %x, %d vertices:", who, attr, tp, pal, flags, n);
    for (int k = 0; k < n; k++)
        fprintf(stderr, " (%d,%d z%06x w%x c%d,%d,%d t%d,%d)", v[k].x, v[k].y, v[k].z, v[k].w, v[k].r, v[k].g, v[k].b,
                v[k].s, v[k].t);
    fprintf(stderr, "\n");
}
// the own geometry engine's polygon list against melonDS's (the frame both just set up for rendering)
// every polygon of the two lists by what differs (vertex count, attributes, flags, position, depth, colour,
// texture coordinates; clipped or not): counts at exit, the first example of each kind
enum { PK_COUNT, PK_ATTR, PK_FLAGS, PK_POS, PK_DEPTH, PK_COLOR, PK_TEX, PK_N };
static const char *const kPolyKinds[PK_N] = {"vertex count", "attributes", "flags", "position", "depth", "colour", "texcoord"};
static u64 sPolyKind[PK_N][2], sPolysCompared;
static void census_polygons(u64 label) {
    int nm = melon_3d_poly_count(), no = gpu3d_poly_count();
    int n = nm < no ? nm : no;
    for (int i = 0; i < n; i++) {
        Gpu3dVertexOut vm[10], vo[10];
        uint32_t am, ao, tm, to, pm, po;
        int fm, fo;
        memset(vm, 0, sizeof vm);
        memset(vo, 0, sizeof vo);
        int km = melon_3d_poly_get(i, vm, &am, &tm, &pm, &fm);
        int ko = gpu3d_poly_get(i, vo, &ao, &to, &po, &fo);
        sPolysCompared++;
        int clipped = 0;
        for (int k = 0; k < km; k++) clipped |= vm[k].clipped;
        int kinds[PK_N] = {0};
        kinds[PK_COUNT] = km != ko;
        kinds[PK_ATTR] = am != ao || tm != to || pm != po;
        kinds[PK_FLAGS] = fm != fo;
        for (int k = 0; k < km && k < ko; k++) {
            kinds[PK_POS] |= vm[k].x != vo[k].x || vm[k].y != vo[k].y;
            kinds[PK_DEPTH] |= vm[k].z != vo[k].z || vm[k].w != vo[k].w;
            kinds[PK_COLOR] |= vm[k].r != vo[k].r || vm[k].g != vo[k].g || vm[k].b != vo[k].b;
            kinds[PK_TEX] |= vm[k].s != vo[k].s || vm[k].t != vo[k].t;
        }
        static FILE *dump;
        static long dumped, dumpMax = -1;
        if (dumpMax < 0) dumpMax = getenv("ACWW_RENDER_FITDUMP_MAX") ? atol(getenv("ACWW_RENDER_FITDUMP_MAX")) : 200000;
        if (!dump && getenv("ACWW_RENDER_FITDUMP")) dump = fopen(getenv("ACWW_RENDER_FITDUMP"), "w");
        if (dump && km == ko && dumped < dumpMax) {  // every vertex: the own engine's inputs, both outputs
            for (int k = 0; k < km; k++, dumped++) {
                fprintf(dump, "V %llu %d %d %d ", (unsigned long long)label, i, k, vm[k].clipped);
                fprintf(dump, "own %d %d %d %d %d %d %d %d %d melon %d %d %d %d %d %d %d %d %d ", vo[k].x, vo[k].y,
                        vo[k].z, vo[k].w, vo[k].r, vo[k].g, vo[k].b, vo[k].s, vo[k].t, vm[k].x, vm[k].y, vm[k].z, vm[k].w,
                        vm[k].r, vm[k].g, vm[k].b, vm[k].s, vm[k].t);
                fprintf(dump, "mclip %d %d %d %d ", vm[k].clip[0], vm[k].clip[1], vm[k].clip[2], vm[k].clip[3]);
                void gpu3d_debug_vertex(int i, int k, FILE *f);
                gpu3d_debug_vertex(i, k, dump);
                fprintf(dump, "\n");
            }
        }
        for (int k = 0; k < PK_N; k++) {
            if (!kinds[k]) continue;
            if (!sPolyKind[k][clipped]++) {
                fprintf(stderr, "render-compare: frame %llu polygon %d: %s differs (%s), the first such:\n",
                        (unsigned long long)label, i, kPolyKinds[k], clipped ? "clipped" : "not clipped");
                print_poly("own  ", vo, ko, ao, to, po, fo);
                print_poly("melon", vm, km, am, tm, pm, fm);
                gpu3d_debug_poly(i);
            }
            break;  // (the first kind only)
        }
    }
}
static int compare_polygons(const char *kind, u64 label) {
    sPolyFrames++;
    census_polygons(label);
    if (getenv("ACWW_RENDER_POLYDUMP") && label == strtoull(getenv("ACWW_RENDER_POLYDUMP"), 0, 10)) {
        // (both polygon lists of that frame, one polygon a line: own | melon)
        int nm = melon_3d_poly_count(), no = gpu3d_poly_count();
        for (int i = 0; i < (nm > no ? nm : no); i++) {
            Gpu3dVertexOut v[10];
            uint32_t a, t, p;
            int f;
            for (int w = 0; w < 2; w++) {
                memset(v, 0, sizeof v);
                int k = w ? melon_3d_poly_get(i, v, &a, &t, &p, &f) : gpu3d_poly_get(i, v, &a, &t, &p, &f);
                fprintf(stderr, "%s %4d %08x %08x f%d:", w ? " | melon" : "polydump own", i, a, t, f);
                for (int j = 0; j < k; j++)
                    fprintf(stderr, " %d,%d,%x c%d,%d,%d t%d,%d", v[j].x, v[j].y, v[j].z, v[j].r >> 4, v[j].g >> 4, v[j].b >> 4,
                            v[j].s, v[j].t);
            }
            fprintf(stderr, "\n");
        }
    }
    int nm = melon_3d_poly_count(), no = gpu3d_poly_count();
    int n = nm < no ? nm : no, bad = -1;
    Gpu3dVertexOut vm[10], vo[10];
    uint32_t am = 0, ao = 0, tm = 0, to = 0, pm = 0, po = 0;
    int fm = 0, fo = 0, km = 0, ko = 0;
    for (int i = 0; i < n && bad < 0; i++) {
        memset(vm, 0, sizeof vm);
        memset(vo, 0, sizeof vo);
        km = melon_3d_poly_get(i, vm, &am, &tm, &pm, &fm);
        ko = gpu3d_poly_get(i, vo, &ao, &to, &po, &fo);
        if (km != ko || am != ao || tm != to || pm != po || fm != fo || memcmp(vm, vo, sizeof vm)) bad = i;
    }
    if (bad < 0 && nm == no) return 0;
    if (bad < 0) {  // (one list is longer: its first extra polygon)
        bad = n;
        memset(vm, 0, sizeof vm);
        memset(vo, 0, sizeof vo);
        km = melon_3d_poly_get(n, vm, &am, &tm, &pm, &fm);
        ko = gpu3d_poly_get(n, vo, &ao, &to, &po, &fo);
    }
    if (sFirstPolyDiffering == ~0ull) sFirstPolyDiffering = label;
    sPolyDiffering++;
    if (sPolyLines++ < sCompareMax) {
        fprintf(stderr, "render-compare: %s %llu polygons: own %d melon %d", kind, (unsigned long long)label, no, nm);
        if (bad >= 0) {
            fprintf(stderr, ", the first differing: %d\n", bad);
            print_poly("own  ", vo, ko, ao, to, po, fo);
            print_poly("melon", vm, km, am, tm, pm, fm);
            gpu3d_debug_poly(bad);
        } else fprintf(stderr, "\n");
    }
    return 1;
}
// the own 3D picture against melonDS's
static int compare_3d(const char *kind, u64 label) {
    sPix3dFrames++;
    melon_3d_lines(s3d);
    const uint32_t *o = gpu3d_lines();
    int first = -1, count = 0;
    for (int i = 0; i < 256 * 192; i++)
        if ((o[i] ^ s3d[i]) & 0x1f3f3f3f) { if (first < 0) first = i; count++; }
    {   // ACWW_RENDER_DUMP3D=<frame>:<prefix>: that frame as the self-test dumps it (<prefix>.bin: the own picture,
        // melonDS's, the own polygon per pixel, both depth buffers; <prefix>.txt: the own polygon list)
        const char *d = getenv("ACWW_RENDER_DUMP3D");
        char *colon;
        if (d && strtoull(d, &colon, 10) == label && *colon == ':') {
            char name[512];
            snprintf(name, sizeof name, "%s.bin", colon + 1);
            FILE *f = fopen(name, "wb");
            if (f) {
                fwrite(o, 4, 256 * 192, f);
                fwrite(s3d, 4, 256 * 192, f);
                int g3_raster_pixel_polygon(int x, int y);
                for (int i = 0; i < 256 * 192; i++) {
                    int16_t v = (int16_t)g3_raster_pixel_polygon(i % 256, i / 256);
                    fwrite(&v, 2, 1, f);
                }
                u32 g3_raster_depth(int x, int y);  // (then both depth buffers)
                int melon_3d_buffers(u32 *depth, u32 *attr);
                static u32 dep[256 * 192];
                for (int i = 0; i < 256 * 192; i++) dep[i] = g3_raster_depth(i % 256, i / 256);
                fwrite(dep, 4, 256 * 192, f);
                if (!melon_3d_buffers(dep, 0)) memset(dep, 0, sizeof dep);
                fwrite(dep, 4, 256 * 192, f);
                fclose(f);
            }
            snprintf(name, sizeof name, "%s.txt", colon + 1);
            f = fopen(name, "w");
            if (f) {
                for (int i = 0; i < gpu3d_poly_count(); i++) {
                    Gpu3dVertexOut v[10];
                    uint32_t a, t, p;
                    int fl, k = gpu3d_poly_get(i, v, &a, &t, &p, &fl);
                    fprintf(f, "poly %08x %08x %04x %d %d", a, t, p, fl, k);
                    for (int j = 0; j < k; j++)
                        fprintf(f, " %d %d %d %d %d %d %d %d %d", v[j].x, v[j].y, v[j].z, v[j].w, v[j].r, v[j].g, v[j].b,
                                v[j].s, v[j].t);
                    fprintf(f, "\n");
                }
                fclose(f);
            }
        }
    }
    if (first < 0) return 0;
    if (sFirstPix3dDiffering == ~0ull) sFirstPix3dDiffering = label;
    sPix3dDiffering++;
    if (sPix3dLines++ < sCompareMax) {
        Gpu3dPolyInfo pi;
        char what[160] = "the clear colour";
        if (gpu3d_pixel_polygon(first % 256, first / 256, &pi))
            snprintf(what, sizeof what, "polygon %d (attr %08x tex %08x pal %04x, %d vertices%s)", pi.index, pi.attr,
                     pi.texparam, pi.texpal, pi.nverts, pi.translucent ? ", translucent" : "");
        fprintf(stderr, "render-compare: %s %llu 3D: %d pixels differ, first (%d, %d) own %08x melon %08x: %s\n", kind,
                (unsigned long long)label, count, first % 256, first / 256, o[first], s3d[first], what);
    }
    return 1;
}
int render_compare_3d(const char *kind, u64 label) {
    int a = compare_polygons(kind, label);
    int b = compare_3d(kind, label);
    return a || b;
}

static void gpu3d_run_frame(void) {
    const u8 *banks[9];
    for (int i = 0; i < 9; i++) banks[i] = host_vram_bank(i);
    u32 host_vram_generation(void);
    gpu3d_vram_generation(host_vram_generation());
    gpu3d_frame(banks);
}

// ACWW_GPU2D_CHECK: the GPU's 2D pictures against the CPU's, word for word
int render_check_gpu2d(void) {
    static uint32_t gpu[256 * 384 * 2];
    static long reported;
    if (!wdisp_read_2d(gpu)) return 1;
    sGpu2dChecked++;
    int count = 0, first = -1;
    for (int s = 0; s < 2; s++) {
        const uint32_t *cpu = gpu2d_deferred(s);
        for (int i = 0; i < 256 * 192 * 2; i++)
            if (cpu[i] != gpu[s * 256 * 192 * 2 + i]) {
                if (first < 0) first = s * 256 * 192 * 2 + i;
                count++;
            }
    }
    if (!count) return 0;
    sGpu2dDiffering++;
    if (reported++ < sCompareMax) {
        int px = first / 2, s = px / (256 * 192), y = px / 256 % 192, x = px % 256, w = first & 1;
        const uint32_t *cpu = gpu2d_deferred(s) + (y * 256 + x) * 2;
        char what[256];
        gpu2d_describe_pixel(s, x, y, what, sizeof what);
        fprintf(stderr, "gpu2d-check: frame %llu: %d words differ, first %s screen (%d, %d) word %d: cpu %08x %08x gpu "
                "%08x %08x (%s)\n", (unsigned long long)host_frame(), count, s ? "bottom" : "top", x, y, w, cpu[0], cpu[1],
                gpu[(px) * 2], gpu[px * 2 + 1], what);
    }
    return 1;
}

// ACWW_FPS: also the renderers' share of the time, every 120 frames (the 2D one with the game's H-blank work)
static u64 sTime2d, sTime3d, sTimeComp;
static void render_timing(u64 t0, u64 t1, u64 t2, u64 t3) {
    static int on = -1;
    if (on < 0) on = getenv("ACWW_FPS") != 0;
    if (!on) return;
    sTime2d += t1 - t0, sTime3d += t2 - t1, sTimeComp += t3 - t2;
    if (host_frame() % 120 == 0) {
        fprintf(stderr, "render: frame %llu: 2D %.1f ms, 3D %.1f ms, compositing %.1f ms a frame\n",
                (unsigned long long)host_frame(), (double)sTime2d / 120e6, (double)sTime3d / 120e6,
                (double)sTimeComp / 120e6);
        sTime2d = sTime3d = sTimeComp = 0;
    }
}

static int render_hblank(int line, void *context) {
    (void)context;
    HOST_PROF(PROF_GAME);  // (the game's H-blank work)
    return host_hblank(line);
}
// ACWW_FRAMESKIP=<n>: the screens drawn every n + 1 frames (the 3DS's default: 1, every other one; elsewhere and in
// the enhanced port, N3DS_DIRECT, 0).
// The game runs every frame as before - its H-blank work line by line, the geometry engine - only the pictures are
// skipped; the 3D one is drawn in the frame before the screens' (the 2D renderer shows the last frame's 3D picture)
static int frameskip(void) {
    static int n = -1;
    if (n < 0) {
#if defined(__3DS__) && !defined(N3DS_DIRECT)
        n = 1;
#else
        n = 0;
#endif
        if (getenv("ACWW_FRAMESKIP")) n = atoi(getenv("ACWW_FRAMESKIP"));
        if (n < 0) n = 0;
    }
    return n;
}

void render_frame(void) {
    u64 every = (u64)frameskip() + 1;
    int draw2d = host_frame() % every == 0, raster3d = (host_frame() + 1) % every == 0;
    if (!draw2d && !sCompare && !sGpuDisplay) {
        if (sOwn)
            for (int line = 0; line < 192; line++) render_hblank(line, 0);
        HOST_PROF(raster3d ? PROF_3D : PROF_GEOM);
        if (sGpu3d) gpu3d_set_raster(raster3d), gpu3d_run_frame();
        return;
    }
    // (a 3D picture drawn now would be shown by the next frame drawn, but the frame before that draws a newer one;
    // the comparisons and the PC's GPU display draw every frame)
    gpu3d_set_raster(sCompare || sGpuDisplay ? 1 : raster3d);
    // the 3D picture this frame shows (melonDS renders the next one in the frame)
    const uint32_t *lines3d = s3d;
    if (sGpu2d) {
        if (sGpuDisplay) lines3d = NULL;  // (the GPU's: gpu3d_wgpu_picture)
        else if (sOwn3d) lines3d = gpu3d_lines();
        else melon_3d_lines(s3d);
    }
    if (sMelon) melon_frame();
    HOST_PROF(PROF_2D);
    if (sPica) pdisp_wait(PDISP_COMP, 1);  // (the 2D renderer records into the record that compositing drew)
    u64 t0 = host_plat_time_ns();
    if (sGpu2d || sGpu3d) host_vram_flush();
    if (sGpu2d) {
        Gpu2dFrameInput in;
        for (int i = 0; i < 9; i++) in.bank[i] = host_vram_bank(i);
        in.palette = gHostPltt;
        in.oam = gHostOam;
        in.lines3d = lines3d;
        // the game's H-blank effects, between the lines the screens show (not when they show melonDS's pictures:
        // it draws the frame at once)
        in.hblank = sOwn ? render_hblank : 0;
        in.hblankContext = 0;
        in.defer3d = sGpuDisplay;
        in.gpu = sGpu2dMode;
        in.layers = sPica ? pdisp_layers() : NULL;
        in.deferLayers = sPica;  // (drawn by the GPU thread: pica_display.c)
        gpu2d_render_frame(&in);
        if (sGpuDisplay) {
            const uint32_t *deferred[2] = {gpu2d_deferred(0), gpu2d_deferred(1)};
            const uint32_t *lines[2] = {gpu2d_deferred_lines(0), gpu2d_deferred_lines(1)};
            if (sGpu2dMode) wdisp_frame_2d(gpu2d_gpu_frame(), in.bank, lines, gpu3d_wgpu_picture(), sScale3d);
            else wdisp_frame(deferred, lines, gpu3d_wgpu_picture(), sScale3d);
            if (sGpu2dMode == 2 && !sSelftest) render_check_gpu2d();
        }
    }
    u64 t1 = host_plat_time_ns();
    gHostProf = PROF_3D;
    void *texture3d = sPica && sOwn3d ? gpu3d_pica_picture() : NULL;  // (the last frame's, as lines3d)
    if (sGpu3d) gpu3d_run_frame();
    u64 t2 = host_plat_time_ns();
    gHostProf = PROF_COMP;
    if (sPica) pdisp_frame(texture3d, sOwn3d ? lines3d : NULL);  // (the 3D picture the 2D renderer would have shown)
    render_timing(t0, t1, t2, host_plat_time_ns());
    if (sCompare && !sSelftest) {
        render_compare("frame", host_frame());
        render_compare_3d("frame", host_frame());
    }
}

const uint32_t *render_screen(int screen) {
    if (sGpuDisplay) return wdisp_screens() + 256 * 192 * (screen & 1);
    if (sPica) return pdisp_screen(screen);
    return sOwn ? gpu2d_screen(screen) : melon_screen(screen);
}
int render_on_gpu(void) { return sGpuDisplay; }
int render_presenting(void) { return wdisp_presenting() || sPica; }

void render_exit_report(void) {
    if (sPica) pdisp_idle();  // (the GPU thread's last job done before the program ends)
    if (sCompare) {
        fprintf(stderr, "render-compare: %llu frames compared, %llu differ", (unsigned long long)sCompared,
                (unsigned long long)sDiffering);
        if (sDiffering) fprintf(stderr, " (the first: %llu)", (unsigned long long)sFirstDiffering);
        fprintf(stderr, "\nrender-compare: 3D: %llu reads compared, %llu differ", (unsigned long long)sReads3d,
                (unsigned long long)sReadsDiffering);
        if (sReadsDiffering) fprintf(stderr, " (the first in frame %llu)", (unsigned long long)sFirstReadDiffering);
        fprintf(stderr, "; %llu polygon lists compared, %llu differ", (unsigned long long)sPolyFrames,
                (unsigned long long)sPolyDiffering);
        if (sPolyDiffering) fprintf(stderr, " (the first: %llu)", (unsigned long long)sFirstPolyDiffering);
        fprintf(stderr, "; %llu pictures compared, %llu differ", (unsigned long long)sPix3dFrames,
                (unsigned long long)sPix3dDiffering);
        if (sPix3dDiffering) fprintf(stderr, " (the first: %llu)", (unsigned long long)sFirstPix3dDiffering);
        fprintf(stderr, "\nrender-compare: 3D: %llu polygons compared;", (unsigned long long)sPolysCompared);
        for (int k = 0; k < PK_N; k++)
            if (sPolyKind[k][0] || sPolyKind[k][1])
                fprintf(stderr, " %s differs: %llu (+%llu clipped)", kPolyKinds[k], (unsigned long long)sPolyKind[k][0],
                        (unsigned long long)sPolyKind[k][1]);
        fprintf(stderr, "\n");
    }
    if (sGpu2dMode == 2)
        fprintf(stderr, "gpu2d-check: %llu frames checked, %llu differ\n", (unsigned long long)sGpu2dChecked,
                (unsigned long long)sGpu2dDiffering);
    if (sGpu2d && getenv("ACWW_GPU2D_FEATURES")) gpu2d_print_features();
    if (sGpu3d && getenv("ACWW_GPU3D_FEATURES")) gpu3d_print_features();
}

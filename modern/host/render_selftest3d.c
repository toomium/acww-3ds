// Host build: the 3D engines' self-test (ACWW_RENDER_SELFTEST3D=<scenes>, render.c). Before the game starts, each
// scene sends both 3D engines (melonDS's and the own one, gpu3d/) the same commands: random polygons given directly
// in screen space (identity matrices, W = 1 or random W through a scaled position), random colours, random
// rendering registers as ACWW_RENDER_SELFTEST3D_MASK allows, a swap; then a frame is rendered by both and the 3D
// pictures are compared (render_compare_3d: the first differing pixel and the polygon that drew it). Then the run
// ends. ACWW_RENDER_SELFTEST3D_SEED picks another random sequence; ACWW_RENDER_SELFTEST3D_POLYS the polygons per scene
// (default 1); ACWW_RENDER_SELFTEST3D_MASK (default 0: flat, opaque, Z-buffered triangles): 1 quads and strips,
// 2 colours per vertex, 4 W varies, 8 W-buffering, 16 translucency, 32 textures (a texture naming its texels,
// random texture coordinates, white), 64 anti-aliasing, 128 alpha test,
// 256 fog, 512 edge marking, 1024 shadow polygons, 2048 toon / highlight, 4096 lighting (random lights, material
// and a normal per vertex; compared through the polygon lists' colours), 8192 a random W per vertex (a projection
// that makes W the vertex's z; the same screen positions).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpu3d/gpu3d.h"
int g3_raster_pixel_polygon(int x, int y);
#include "host_mem.h"
#include "render.h"

void host_vram_control(u32 addr, u8 value);  // vram.c

static u32 sRng = 0x1234567, sMask, sPolys = 1;
static u32 rnd(void) {
    sRng ^= sRng << 13;
    sRng ^= sRng >> 17;
    sRng ^= sRng << 5;
    return sRng;
}
static u32 rnd_below(u32 n) { return rnd() % n; }
enum { M_QUADS = 1, M_GOURAUD = 2, M_W = 4, M_WBUF = 8, M_TRANS = 16, M_TEX = 32, M_AA = 64, M_ATEST = 128, M_FOG = 256,
       M_EDGE = 512, M_SHADOW = 1024, M_TOON = 2048, M_LIGHT = 4096,
       M_PERSP = 8192 };

static void cmd(u32 c, u32 p) { render_write32(0x04000400 + (c - 0x10) * 4 + 0x40, p); }

// a vertex at screen position (sx, sy) in 1/16 pixels... given as clip coordinates with W = w (4.12)
static void vertex_at_w(s32 sx, s32 sy, s32 w) {  // (M_PERSP: W = w in 4.12, z 0)
    s32 vx = (s32)(((int64_t)(sx * 8 - 4096) * w) >> 12), vy = (s32)(((int64_t)(4096 - sy * 32 / 3) * w) >> 12);
    cmd(0x23, (u32)(u16)vx | (u32)(u16)vy << 16);
    cmd(0x23, (u32)(u16)w);
}
static void vertex_at(s32 sx, s32 sy, s32 w, s32 z) {
    // with identity matrices and a scale of w: clip x = vx * w / 4096 ... VTX_16 is 4.12 in [-8, 8)
    // x_screen = (x + w) * 256 / 2w  ->  x = (sx / 128 - 1) * w; we pass vx = sx / 128 - 1 in 4.12 and scale by w
    (void)w;
    s32 vx = sx * 8 - 4096, vy = 4096 - sy * 32 / 3;  // (sx, sy in quarter pixels)
    cmd(0x23, (u32)(u16)vx | (u32)(u16)vy << 16);
    cmd(0x23, (u32)(u16)z);
}

// ACWW_RENDER_SELFTEST3D_QUAD="x0 y0 x1 y1 x2 y2 x3 y3 c0 c1 c2 c3" (quarter pixels, BGR555 colours): one fixed quad;
// ACWW_RENDER_SELFTEST3D_W its W (default 0x1000)
static int fixed_scene(void) {
    const char *q = getenv("ACWW_RENDER_SELFTEST3D_QUAD");
    if (!q) return 0;
    int v[12] = {0};
    if (sscanf(q, "%d %d %d %d %d %d %d %d %x %x %x %x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8],
               &v[9], &v[10], &v[11]) != 12)
        return 0;
    render_write16(0x04000304, 0x820f);
    render_write16(0x04000060, 0);
    render_write16(0x04000350, 0);
    render_write16(0x04000352, 0x1f);
    render_write16(0x04000354, 0x7fff);
    cmd(0x10, 0);
    cmd(0x15, 0);
    cmd(0x10, 2);
    cmd(0x15, 0);
    if (getenv("ACWW_RENDER_SELFTEST3D_W")) {  // (W = this scale: x, y, z and w scaled alike)
        u32 sc = (u32)strtoul(getenv("ACWW_RENDER_SELFTEST3D_W"), 0, 0);
        cmd(0x10, 0);
        u32 m[16] = {sc, 0, 0, 0, 0, sc, 0, 0, 0, 0, sc, 0, 0, 0, 0, sc};
        for (int i = 0; i < 16; i++) cmd(0x16, m[i]);
        cmd(0x10, 2);
    }
    cmd(0x60, 0xbfff0000);
    cmd(0x29, 0x001f00c0);
    cmd(0x2a, 0);
    cmd(0x40, 1);
    for (int k = 0; k < 4; k++) {
        cmd(0x20, (u32)v[8 + k]);
        vertex_at(v[2 * k], v[2 * k + 1], 0, 0);
    }
    cmd(0x41, 0);
    cmd(0x50, 0);
    return 1;
}

// ACWW_RENDER_SELFTEST3D_CMDS="cc=pppppppp ...": these geometry commands (hex) after the usual setup (identity matrices,
// viewport, clear), then a swap (SWAP_BUFFERS 0 unless the commands end with one): for experiments on one feature;
// ACWW_RENDER_SELFTEST3D_DISP (DISP3DCNT, hex, default 0), _CLEAR (CLEAR_COLOR, hex, default 001f0000)
static int command_scene(void) {
    const char *q = getenv("ACWW_RENDER_SELFTEST3D_CMDS");
    if (!q) return 0;
    const char *disp = getenv("ACWW_RENDER_SELFTEST3D_DISP"), *clear = getenv("ACWW_RENDER_SELFTEST3D_CLEAR");
    u32 cc = clear ? (u32)strtoul(clear, 0, 16) : 0x001f0000;
    render_write16(0x04000304, 0x820f);
    render_write16(0x04000060, disp ? (u16)strtoul(disp, 0, 16) : 0);
    render_write16(0x04000350, (u16)cc);
    render_write16(0x04000352, (u16)(cc >> 16));
    render_write16(0x04000354, 0x7fff);
    cmd(0x10, 0);
    cmd(0x15, 0);
    cmd(0x10, 2);
    cmd(0x15, 0);
    cmd(0x60, 0xbfff0000);
    unsigned c, p;
    int n, swapped = 0;
    while (sscanf(q, " %x=%x%n", &c, &p, &n) == 2) {
        cmd(c, p);
        swapped |= c == 0x50;
        q += n;
    }
    if (!swapped) cmd(0x50, 0);
    return 1;
}

// M_TEX: bank A as texture slot 0 holding a 128x128 direct-colour texture whose texels name themselves (red: s bits
// 0-4, green: t bits 0-4, blue: s bits 5-6 | t bits 5-6 << 2), so a picture shows which texel each pixel sampled
static void coordinate_texture(void) {
    static int done;
    if (done++) return;
    host_vram_control(0x04000240, 0x80);
    render_write8(0x04000240, 0x80);
    u8 *v = HostVram_CpuPtr(0x06800000);
    for (int t = 0; t < 128; t++)
        for (int s = 0; s < 128; s++) {
            u16 c = (u16)((s & 31) | (t & 31) << 5 | ((s >> 5) | (t >> 5) << 2) << 10 | 0x8000);
            v[(t * 128 + s) * 2] = (u8)c;
            v[(t * 128 + s) * 2 + 1] = (u8)(c >> 8);
        }
    host_vram_control(0x04000240, 0x83);
    render_write8(0x04000240, 0x83);
}

static void scene(void) {
    if (command_scene() || fixed_scene()) return;
    if (sMask & M_TEX) coordinate_texture();
    render_write16(0x04000304, 0x820f);
    u16 disp = 0;
    if (sMask & M_TRANS) disp |= 8;
    if (sMask & M_TEX) disp |= 1;
    if (sMask & M_FOG) {  // (fog on, alpha only at times, a random shift, colour, offset and table)
        disp |= (u16)(0x80 | (rnd_below(4) ? 0 : 0x40) | rnd_below(11) << 8);
        render_write16(0x04000358, (u16)(rnd() & 0x7fff));
        render_write16(0x0400035a, (u16)(rnd() & 31));
        render_write16(0x0400035c, (u16)(rnd() & 0x7fff));
        u8 t = (u8)rnd_below(16);
        for (int i = 0; i < 32; i += 2) {
            u8 a = t, b;
            t = (u8)(t + rnd_below(9)); if (t > 127) t = 127;
            b = t;
            t = (u8)(t + rnd_below(9)); if (t > 127) t = 127;
            render_write16(0x04000360 + (u32)i, (u16)(a | b << 8));
        }
    }
    if (sMask & M_AA && rnd() & 1) disp |= 16;
    render_write16(0x04000060, disp);
    render_write16(0x04000350, (u16)(rnd() & 0x7fff) | ((sMask & M_FOG) && rnd_below(2) ? 0x8000 : 0));
    render_write16(0x04000352, (u16)(rnd_below(2) ? 0x001f : 0x0000) | (u16)(rnd_below(64) << 8));
    render_write16(0x04000354, 0x7fff);
    cmd(0x10, 0);
    cmd(0x15, 0);
    cmd(0x10, 2);
    cmd(0x15, 0);
    if (sMask & M_W) {  // a random W: scale x, y, z and w alike through the projection
        s32 s = 0x400 + (s32)rnd_below(0x7000);
        cmd(0x10, 0);
        u32 m[16] = {(u32)s, 0, 0, 0, 0, (u32)s, 0, 0, 0, 0, (u32)s, 0, 0, 0, 0, (u32)s};
        for (int i = 0; i < 16; i++) cmd(0x16, m[i]);
        cmd(0x10, 2);
    }
    if (sMask & M_PERSP) {  // clip x, y as given, z 0, W the vertex's z
        cmd(0x10, 0);
        static const u32 m[16] = {0x1000, 0, 0, 0, 0, 0x1000, 0, 0, 0, 0, 0, 0x1000, 0, 0, 0, 0};
        for (int i = 0; i < 16; i++) cmd(0x16, m[i]);
        cmd(0x10, 2);
    }
    cmd(0x60, 0xbfff0000);
    for (u32 n = 0; n < sPolys; n++) {
        u32 alpha = (sMask & M_TRANS) && rnd_below(2) ? 1 + rnd_below(30) : 31;
        u32 lights = (sMask & M_LIGHT) ? rnd() & 15 : 0;
        if (sMask & M_LIGHT) {
            for (int l = 0; l < 4; l++) {
                cmd(0x32, (rnd() & 0x3fffffff) | (u32)l << 30);
                cmd(0x33, (rnd() & 0x7fff) | (u32)l << 30);
            }
            cmd(0x30, rnd() & 0x7fff7fff & ~0x8000u);  // (diffuse, ambient; bit 15 clear: no vertex colour)
            u32 se = rnd() & 0x7fff7fff & ~0x8000u;  // (specular, emission; no shininess table)
            if (getenv("ACWW_RENDER_SELFTEST3D_NOSPEC")) se &= 0x7fff0000;  // (no specular)
            cmd(0x31, se);
        }
        cmd(0x29, 0x000000c0 | lights | alpha << 16 | rnd_below(64) << 24 | ((sMask & M_FOG) && rnd_below(4) ? 0x8000 : 0));
        cmd(0x2a, (sMask & M_TEX) ? 7u << 26 | 4u << 23 | 4u << 20 | 3u << 16 : 0);
        int quad = (sMask & M_QUADS) && rnd_below(2);
        cmd(0x40, quad ? 1 : 0);
        int nv = quad ? 4 : 3;
        s32 z = (s32)rnd_below(0x2000) - 0x1000;
        // random vertices, made convex for quads by sorting around the centre
        s32 xs[4], ys[4];
        if (quad) {
            s32 cx = (s32)rnd_below(256 * 4), cy = (s32)rnd_below(192 * 4);
            s32 rx = 8 + (s32)rnd_below(400), ry = 8 + (s32)rnd_below(300);
            static const int kx[4] = {-1, 1, 1, -1}, ky[4] = {-1, -1, 1, 1};
            for (int k = 0; k < 4; k++) {
                xs[k] = cx + kx[k] * (rx + (s32)rnd_below(60)) - (s32)rnd_below(30);
                ys[k] = cy + ky[k] * (ry + (s32)rnd_below(60)) - (s32)rnd_below(30);
            }
        } else {
            for (int k = 0; k < 3; k++) {
                xs[k] = (s32)rnd_below(256 * 4 + 200) - 100;
                ys[k] = (s32)rnd_below(192 * 4 + 200) - 100;
            }
        }
        for (int k = 0; k < nv; k++) {
            if (sMask & M_TEX) {  // (white, and random texture coordinates within +-256 texels)
                cmd(0x20, 0x7fff);
                cmd(0x22, (((rnd() & 0x1fff) - 0x1000) & 0xffff) | ((rnd() & 0x1fff) - 0x1000) << 16);
            } else if (sMask & M_LIGHT) cmd(0x21, rnd() & 0x3fffffff);
            else if (k == 0 || (sMask & M_GOURAUD)) cmd(0x20, rnd() & 0x7fff);
            if (sMask & M_PERSP) vertex_at_w(xs[k], ys[k], 0x200 + (s32)rnd_below(0x6e00));
            else vertex_at(xs[k], ys[k], 0, z + (s32)rnd_below(0x200));
        }
        cmd(0x41, 0);
    }
    cmd(0x50, (sMask & M_WBUF) ? 2 : 0);
}

int render_compare_3d(const char *kind, u64 label);
void render_selftest3d(long scenes) __attribute__((noreturn));
void render_selftest3d(long scenes) {
    if (getenv("ACWW_RENDER_SELFTEST3D_SEED")) sRng = (u32)strtoul(getenv("ACWW_RENDER_SELFTEST3D_SEED"), 0, 0) | 1;
    if (getenv("ACWW_RENDER_SELFTEST3D_MASK")) sMask = (u32)strtoul(getenv("ACWW_RENDER_SELFTEST3D_MASK"), 0, 0);
    if (getenv("ACWW_RENDER_SELFTEST3D_POLYS")) sPolys = (u32)strtoul(getenv("ACWW_RENDER_SELFTEST3D_POLYS"), 0, 0);
    long bad = 0;
    if (getenv("ACWW_RENDER_SELFTEST3D_LATENCY")) {  // (which frame's swap melonDS's 3D picture shows)
        void melon_3d_lines(u32 *out);
        static u32 l[256 * 192];
        for (int k = 1; k < 6; k++) {
            render_write16(0x04000304, 0x820f);
            render_write16(0x04000350, (u16)k);
            render_write16(0x04000352, 0x1f);
            cmd(0x50, 0);
            for (int f = 0; f < 2; f++) {
                melon_3d_lines(l);
                u32 before = l[0];
                render_frame();
                melon_3d_lines(l);
                fprintf(stderr, "latency: clear colour %d set and swapped; frame %d: before %08x after %08x\n", k, f, before, l[0]);
            }
        }
        exit(0);
    }
    scene();
    render_frame();
    for (long n = 0; n < scenes; n++) {
        scene();
        render_frame();
        int d = render_compare_3d("self-test scene", (u64)n);
        bad += d;
        if (getenv("ACWW_RENDER_SELFTEST3D_DUMP")) {  // <prefix>-<scene>.bin: the own picture, melonDS's, the polygons, the depths
            void melon_3d_lines(u32 *out);
            static u32 l[256 * 192];
            char name[512];
            snprintf(name, sizeof name, "%s-%ld.bin", getenv("ACWW_RENDER_SELFTEST3D_DUMP"), n);
            FILE *f = fopen(name, "wb");
            if (f) {
                fwrite(gpu3d_lines(), 4, 256 * 192, f);
                melon_3d_lines(l);
                fwrite(l, 4, 256 * 192, f);
                for (int y = 0; y < 192; y++)  // (then the own engine's polygon per pixel, 16 bits, -1 none)
                    for (int x = 0; x < 256; x++) {
                        int16_t i = (int16_t)g3_raster_pixel_polygon(x, y);
                        fwrite(&i, 2, 1, f);
                    }
                {   // (then both depth buffers, 32 bits a pixel: own, melonDS's)
                    u32 g3_raster_depth(int x, int y);
                    int melon_3d_buffers(u32 *depth, u32 *attr);
                    for (int y = 0; y < 192; y++)
                        for (int x = 0; x < 256; x++) l[y * 256 + x] = g3_raster_depth(x, y);
                    fwrite(l, 4, 256 * 192, f);
                    if (!melon_3d_buffers(l, 0)) memset(l, 0, sizeof l);
                    fwrite(l, 4, 256 * 192, f);
                }
                fclose(f);
            }
            snprintf(name, sizeof name, "%s-%ld.txt", getenv("ACWW_RENDER_SELFTEST3D_DUMP"), n);
            f = fopen(name, "w");
            if (f) {  // the polygons: per line, the attributes and the vertices (x y z w r g b s t); then the registers
                void gpu3d_debug_regs(FILE *f);
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
                gpu3d_debug_regs(f);
                fclose(f);
            }
        }
    }
    fprintf(stderr, "render-selftest3d: %ld scenes, %ld differ\n", scenes, bad);
    render_exit_report();
    exit(bad ? 1 : 0);
}

// The own 3D engine's rasteriser (gpu3d.h): the polygon list handed over at the last swap, drawn into a 256x192
// colour buffer with depth, attribute and fog buffers; then the frame-end passes (edge marking, fog, anti-aliasing).
// Written from the hardware's documented behaviour (GBATEK "DS 3D Rendering").

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpu3d_internal.h"
#include "gpu3d_tex.h"

enum { W = 256, H = 192 };
static u32 sFrameBuf[2][W * H];
static int sShown;
static int sDiag;

// the buffers of the frame being drawn
static u32 sColor[W * H];     // r6 | g6 << 8 | b6 << 16 | a5 << 24
static u32 sDepth[W * H];     // 24 bits
static u8 sOpaqueId[W * H];   // the polygon id of the opaque polygon there (edge marking)
static u8 sTransId[W * H];    // the polygon id of the last translucent polygon there, 0xff none
static u8 sFlags[W * H];      // 1 fog, 2 edge, 4 translucent written, 8 drawn by a polygon of facing 0
static s16 sPoly[W * H];      // (diagnostics: the polygon that wrote the pixel last, -1 none)
// anti-aliasing: the coverage (0-31) of the opaque pixel on top, and the layer below it (what an edge pixel blends with)
static u8 sCov[W * H];
static u32 sColor2[W * H], sDepth2[W * H];

// ---- interpolation (perspective-correct, from the normalised W of the two ends)
typedef struct { int shift; s64 factor; } Interp;
static void interp_setup(Interp *ip, s32 x, s32 x0, s32 x1, s32 w0, s32 w1, int shift) {
    ip->shift = shift;
    s32 span = x1 - x0, pos = x - x0;
    if (span <= 0) { ip->factor = 0; return; }
    // (a 32-bit division where the operands are non-negative and fit, the same quotient: 64-bit divisions are
    // several times slower on older x86)
    if (w0 == w1) {  // linear
        s64 num = (s64)pos << shift;
        ip->factor = num >= 0 && num <= UINT32_MAX ? (s64)((u32)num / (u32)span) : num / span;
    } else {
        s64 a = (s64)pos * w0, b = (s64)(x1 - x) * w1;
        s64 num = a << shift, den = a + b;
        ip->factor = num >= 0 && num <= UINT32_MAX && den > 0 && den <= UINT32_MAX ? (s64)((u32)num / (u32)den)
                                                                                  : num / den;
    }
}
static s32 interp(const Interp *ip, s32 a0, s32 a1) {
    return a0 + (s32)(((s64)(a1 - a0) * ip->factor) >> ip->shift);
}
// W: the same interpolation gives the perspective-correct W
static s32 interp_z_linear(s32 x, s32 x0, s32 x1, s32 z0, s32 z1) {
    if (x1 <= x0) return z0;
    return z0 + (s32)(((s64)(z1 - z0) * (x - x0)) / (x1 - x0));
}

// ---- an edge between two vertices (y0 < y1), stepped per scanline: x in 18 fractional bits. The increment is
// |dx| times the reciprocal of dy (18 bits; exactly one pixel for a diagonal). An edge is y-major (at most one pixel
// per line) when |dx| <= dy.
typedef struct {
    int v0, v1;          // vertex indices
    s32 x0, y0, x1, y1;
    s64 step;
    int xmajor, sign;    // sign: +1 right, -1 left, 0 vertical
    int bottom;          // it ends at the polygon's bottom
} Edge;
static void edge_setup(Edge *e, const G3Polygon *p, int a, int b) {
    e->v0 = a;
    e->v1 = b;
    e->x0 = p->v[a]->x;
    e->y0 = p->v[a]->y;
    e->x1 = p->v[b]->x;
    e->y1 = p->v[b]->y;
    s32 dx = e->x1 - e->x0, dy = e->y1 - e->y0, adx = dx < 0 ? -dx : dx;
    e->sign = dx > 0 ? 1 : dx < 0 ? -1 : 0;
    e->step = dy <= 0 ? 0 : adx == dy ? 1 << 18 : (s64)adx * ((1 << 18) / dy);
    e->xmajor = adx > dy;
}
// x at line y + k (k = 0 or 1), 18 fractional bits; edges going left are one unit further left
static s64 edge_x(const Edge *e, s32 y, int k) {
    s64 x = ((s64)e->x0 << 18) + e->sign * e->step * (y - e->y0 + k);
    return e->sign < 0 ? x - 1 : x;
}
// the first pixel of a span with this edge on its left, and the last one with it on its right
static s32 edge_left(const Edge *e, s32 y) {
    if (!e->sign) return e->x0;
    if (!e->xmajor) return (s32)(edge_x(e, y, 0) >> 18);
    // (x-major: where the edge leaves the line; going right on its last line, where it enters)
    return (s32)((edge_x(e, y, e->sign > 0 && y + 1 == e->y1 && e->bottom ? 0 : 1) + (1 << 17)) >> 18);
}
static s32 edge_right(const Edge *e, s32 y) {
    if (!e->sign) return e->x0 - 1;
    if (!e->xmajor) return (s32)(edge_x(e, y, 0) >> 18) - 1;
    return (s32)((edge_x(e, y, e->sign < 0 && y + 1 == e->y1 && e->bottom ? 0 : 1) + (1 << 17)) >> 18) - 1;
}

// ---- colours (fitted to melonDS with equal W; ACWW_RENDER_SELFTEST3D_MASK=2): along an edge at the line where the
// edge is furthest out (y + 1 for a left edge going left or a right edge going right, else y), exact and rounded down;
// along the span between the two edges' x at that line, not between the first and last pixel drawn
// exact only between equal W with its low 7 bits clear; otherwise a factor (9 bits along edges, 8 along spans)
static int exact_ok(s32 w0, s32 w1) { return w0 == w1 && !(w0 & 0x7f); }
static s32 lerp_floor(s32 a0, s32 a1, s32 pos, s32 span) {
    if (span <= 0) return a0;
    s64 d = (s64)(a1 - a0) * pos;
    return a0 + (s32)(d >= 0 ? d / span : -((-d + span - 1) / span));
}
// side 0 left, 1 right: returns the line offset (0 or 1) and sets *x: an x-major edge (45 degrees included) rounded at
// that line; a y-major one at line y: on the left rounded down, on the right the next integer above
static int span_end(const Edge *e, int side, s32 y, s32 *x) {
    s32 dx = e->x1 - e->x0, dy = e->y1 - e->y0;
    int out = side ? dx > 0 : dx < 0;
    if ((dx < 0 ? -dx : dx) >= dy && dx) {
        *x = (s32)((edge_x(e, y, out) + (1 << 17)) >> 18);
        return out;
    }
    s64 v = edge_x(e, y, 0);
    *x = (s32)(v >> 18) + (side && dx);  // (on the right the first x past the edge; a vertical edge's own x)
    return 0;
}

// ---- the rendering state of one polygon
static const G3RenderState *sR;

static u32 rgb5_to_6(u32 c) { return c ? c * 2 + 1 : 0; }

// depth test "equal" (attr bit 14): within a margin; "less", or less or equal for a polygon of facing 1 over a pixel
// of a polygon of facing 0 (fitted to melonDS)
static int depth_pass(const G3Polygon *p, s32 z, u32 d, u8 flags) {
    if (p->attr & 0x4000) {
        s32 m = 0x200;
        return z >= (s32)d - m && z <= (s32)d + m;
    }
    return z < (s32)d || (z == (s32)d && p->facing && (flags & 8));
}

// a translucent pixel over a framebuffer value: alpha blending (DISP3DCNT bit 3, over a non-transparent pixel); the
// larger alpha stays, blending or not
static u32 translucent_over(u32 dst, u32 r, u32 g, u32 b, u32 al) {
    u32 da = dst >> 24 & 31;
    if ((sR->dispcnt & 8) && da) {
        r = (r * (al + 1) + (dst & 63) * (31 - al)) >> 5;
        g = (g * (al + 1) + (dst >> 8 & 63) * (31 - al)) >> 5;
        b = (b * (al + 1) + (dst >> 16 & 63) * (31 - al)) >> 5;
    }
    if (da > al) al = da;
    return r | g << 8 | b << 16 | al << 24;
}
// an anti-aliased edge pixel (coverage c) blended with the layer below it (over a transparent layer: the coverage as
// alpha)
static u32 aa_resolve(u32 t, u32 b, u32 c) {
    u32 ab = b >> 24 & 31;
    if (!ab) return (t & 0xffffff) | c << 24;
    u32 r = ((t & 63) * (c + 1) + (b & 63) * (31 - c)) >> 5;
    u32 g = ((t >> 8 & 63) * (c + 1) + (b >> 8 & 63) * (31 - c)) >> 5;
    u32 bb = ((t >> 16 & 63) * (c + 1) + (b >> 16 & 63) * (31 - c)) >> 5;
    return r | g << 8 | bb << 16 | ((31 * (c + 1) + ab * (31 - c)) >> 5) << 24;
}

// one pixel of a polygon: a[] the interpolated vertex colour (9 bits each) and texture coordinates (1/16 texels)
static void put_pixel(const G3Polygon *p, int x, int y, s32 z, const s32 *a, int idx, int cov) {
    int i = y * W + x;
    int below = 0;  // (behind an anti-aliased edge pixel: drawn into the layer below it)
    if (!depth_pass(p, z, sDepth[i], sFlags[i])) {
        if (sCov[i] == 31 || z >= (s32)sDepth2[i]) return;
        below = 1;
    }
    u32 vr = (u32)a[0] >> 3, vg = (u32)a[1] >> 3, vb = (u32)a[2] >> 3;  // (6 bits)
    u32 pa = p->attr >> 16 & 31;
    if (!pa) pa = 31;  // (wireframe: drawn opaque at the edges; to do)
    u32 r = vr, g = vg, b = vb, al = pa;
    int mode = p->attr >> 4 & 3;
    if ((p->texparam >> 26 & 7) && (sR->dispcnt & 1)) {
        u32 tx = g3_texel(p->texparam, p->texpal, a[3] >> 4, a[4] >> 4);
        u32 ta = tx >> 16 & 31;
        u32 tr = rgb5_to_6(tx & 31), tg = rgb5_to_6(tx >> 5 & 31), tb = rgb5_to_6(tx >> 10 & 31);
        if (mode == 1) {  // decal
            if (ta == 31) r = tr, g = tg, b = tb;
            else if (ta) {
                r = (tr * ta + vr * (31 - ta)) >> 5;
                g = (tg * ta + vg * (31 - ta)) >> 5;
                b = (tb * ta + vb * (31 - ta)) >> 5;
            }
        } else {  // modulation
            r = ((tr + 1) * (vr + 1) - 1) >> 6;
            g = ((tg + 1) * (vg + 1) - 1) >> 6;
            b = ((tb + 1) * (vb + 1) - 1) >> 6;
            al = ((ta + 1) * (pa + 1) - 1) >> 5;
        }
    }
    if (!al) return;
    if ((sR->dispcnt & 4) && al <= sR->alphaRef) return;  // (alpha test)
    u32 fog = p->attr >> 15 & 1;
    if (below) {
        if (al == 31) sColor2[i] = r | g << 8 | b << 16 | 31u << 24, sDepth2[i] = (u32)z;
        else {  // (translucent: onto the layer below, as it would be onto the top)
            u8 id = (u8)(p->attr >> 24 & 63);
            if (sTransId[i] == id) return;
            sColor2[i] = translucent_over(sColor2[i], r, g, b, al);
            if (p->attr & 0x800) sDepth2[i] = (u32)z;
            sTransId[i] = id;
        }
        return;
    }
    if (al < 31 && sCov[i] < 31) {  // (translucent over an anti-aliased edge pixel: the edge is resolved first)
        sColor[i] = aa_resolve(sColor[i], sColor2[i], sCov[i]);
        sCov[i] = 31;
    }
    if (al == 31) {
        sColor2[i] = sColor[i];
        sDepth2[i] = sDepth[i];
        sCov[i] = (u8)cov;
        sColor[i] = r | g << 8 | b << 16 | 31u << 24;
        sDepth[i] = (u32)z;
        sOpaqueId[i] = (u8)(p->attr >> 24 & 63);
        sFlags[i] = (u8)((sFlags[i] & ~13) | fog | (p->facing ? 0 : 8));
    } else {
        u8 id = (u8)(p->attr >> 24 & 63);
        if (sTransId[i] == id) return;  // (one translucent polygon ID draws a pixel once)
        sColor[i] = translucent_over(sColor[i], r, g, b, al);
        if (p->attr & 0x800) sDepth[i] = (u32)z;  // (translucent depth update)
        sTransId[i] = id;
        sFlags[i] = (u8)((sFlags[i] & fog) | 4);
    }
    sPoly[i] = (s16)idx;
}

// the polygon's edges, set up once per polygon (edges_at picks the two that cross a line)
typedef struct {
    const G3Polygon *p;
    int n;
    s64 area;
    Edge e[10];
    int down[10];
    s32 ya[10], yb[10];
} PolyEdges;
static PolyEdges sPE;
static void poly_edges(const G3Polygon *p) {
    int n = p->nv, flatbottom = 0;
    s32 bx = -1;  // (a horizontal bottom edge: two bottom vertices at different x)
    for (int i = 0; i < n; i++)
        if (p->v[i]->y == p->ybottom) {
            if (bx >= 0 && p->v[i]->x != bx) flatbottom = 1;
            bx = p->v[i]->x;
        }
    // the winding on the screen decides which of the two chains down from the top vertex is the left one (the
    // edges can cross near a vertex)
    s64 area = 0;
    for (int i = 0; i < n; i++) {
        const G3Vertex *a = p->v[i], *b = p->v[(i + 1) % n];
        area += (s64)a->x * b->y - (s64)b->x * a->y;
    }
    sPE.p = p;
    sPE.area = area;
    sPE.n = 0;
    for (int i = 0; i < n; i++) {
        int a = i, b = (i + 1) % n;
        s32 ya = p->v[a]->y, yb = p->v[b]->y;
        if (ya == yb) continue;
        int k = sPE.n++;
        sPE.down[k] = ya < yb;  // (going down in the vertex order)
        if (ya > yb) { int t = a; a = b; b = t; s32 u = ya; ya = yb; yb = u; }
        edge_setup(&sPE.e[k], p, a, b);
        sPE.e[k].bottom = yb == p->ybottom && flatbottom;
        sPE.ya[k] = ya;
        sPE.yb[k] = yb;
    }
}
// the polygon's two edges that cross line y (the left one first); 0 if fewer than two do
static int edges_at(const G3Polygon *p, s32 y, Edge *l, Edge *r) {
    if (sPE.p != p) poly_edges(p);
    int found = 0, idx[2];
    double mid[2];
    int down[2];
    for (int k = 0; k < sPE.n && found < 2; k++) {
        down[found] = sPE.down[k];
        if (y < sPE.ya[k] || y >= sPE.yb[k]) continue;
        const Edge *e = &sPE.e[k];
        s32 dx = e->x1 - e->x0, dy = e->y1 - e->y0;
        mid[found] = e->x0 + (double)dx * (2.0 * (y - e->y0) + 1) / (2.0 * dy);
        idx[found] = k;
        found++;
    }
    if (found < 2) return 0;
    // clockwise (area > 0, y down): the chain in the vertex order is the right one
    int s = sPE.area == 0 || down[0] == down[1] ? mid[1] < mid[0] : (down[0] == (sPE.area > 0));
    *l = sPE.e[idx[s]];
    *r = sPE.e[idx[!s]];
    return 1;
}

// anti-aliasing coverage (0-31) of pixel x of line y by an edge (side 0 left, 1 right); 31 where the edge doesn't
// cut the pixel. Fitted to melonDS: y-major: the pixel at the edge's x, its area inside the edge (the slope cut to
// 8 fractional bits); x-major: the pixels whose
// centres lie in the edge's run on the line, the exact edge's height there (left edges strictly below)
static int edge_coverage(const Edge *e, int side, s32 x, s32 y, s32 snum, s32 sden) {
    s32 dx = e->x1 - e->x0, dy = e->y1 - e->y0, adx = dx < 0 ? -dx : dx;
    if (!dx || dy <= 0) return 31;
    if (adx < dy) {
        s64 xs = edge_x(e, y, 0);
        if (x != (s32)(xs >> 18)) return 31;
        s64 fr = xs & 0x3ffff, half = (e->step >> 10) << 9;
        if (dx < 0) half = -half;  // (the slope with its sign: the edge's mean x on the line)
        s64 v = side ? (fr + half) >> 13 : ((1 << 18) - fr - half) >> 13;
        return v > 31 ? 31 : v < 0 ? 0 : (int)v;
    }
    // x-major: D = 2 * (pixel centre - exact edge x at line y) * dy * sden, the run's start moved by snum / sden
    s64 d = ((s64)(2 * x + 1) * dy - 2 * ((s64)e->x0 * dy + (s64)dx * (y - e->y0))) * sden - 2 * (s64)dy * snum;
    s64 lim = 2 * (s64)adx * sden;
    if ((dx > 0 ? d < 0 : d > 0) || (d < 0 ? -d : d) > lim) return 31;
    s64 ad = d < 0 ? -d : d;
    int gt = side ? dx < 0 : dx > 0;  // (coverage grows along the run: t, else 1 - t)
    s64 num = gt ? 16 * ad : 32 * (s64)adx * sden - 16 * ad, den = (s64)adx * sden;
    s64 v = side ? num / den : (num + den - 1) / den - 1;
    return v > 31 ? 31 : v < 0 ? 0 : (int)v;
}

// a polygon whose vertices all lie on one line: the pixels from the leftmost vertex to the rightmost (that one
// excluded, one pixel at least), its attributes interpolated linearly between those two vertices
static void draw_one_line(const G3Polygon *p, int idx) {
    int l = 0, r = 0;
    for (int i = 1; i < p->nv; i++) {
        if (p->v[i]->x < p->v[l]->x) l = i;
        if (p->v[i]->x > p->v[r]->x) r = i;
    }
    s32 y = p->ytop, x0 = p->v[l]->x, x1 = p->v[r]->x;
    if (y < 0 || y >= H) return;
    s32 xe = x1 > x0 ? x1 - 1 : x0;
    const G3Vertex *a = p->v[l], *b = p->v[r];
    const s32 va[5] = {a->color[0], a->color[1], a->color[2], a->tex[0], a->tex[1]};
    const s32 vb[5] = {b->color[0], b->color[1], b->color[2], b->tex[0], b->tex[1]};
    for (s32 x = x0; x <= xe; x++) {
        if (x < 0 || x >= W) continue;
        s32 col[5];
        for (int c = 0; c < 5; c++) col[c] = lerp_floor(va[c], vb[c], x - x0, x1 - x0);
        s32 z = lerp_floor(p->z[l], p->z[r], x - x0, x1 - x0);
        put_pixel(p, x, y, z, col, idx, 31);
    }
}

static void draw_polygon(const G3Polygon *p, int idx) {
    int n = p->nv;
    if (n < 3) return;
    s32 ytop = p->ytop, ybot = p->ybottom;
    if (ytop >= ybot) {  // a polygon on one line: from its leftmost to its rightmost x (less one; one pixel at least)
        draw_one_line(p, idx);
        return;
    }
    // GBATEK "Polygon Size": the right edges (but vertical ones) are drawn too, except for opaque polygons without
    // edge marking and anti-aliasing and for translucent ones while alpha blending is off
    int full = p->translucent ? (sR->dispcnt & 8) != 0 : (sR->dispcnt & 0x30) != 0;
    {   // (a polygon without area, a line: its edges' runs are drawn as for full-size polygons)
        s64 area = 0;
        for (int i = 0; i < n; i++) {
            const G3Vertex *a = p->v[i], *b = p->v[(i + 1) % n];
            area += (s64)a->x * b->y - (s64)b->x * a->y;
        }
        if (!area) full = 1;
    }
    int aa = (sR->dispcnt & 0x10) && !p->translucent && (p->attr >> 16 & 31) == 31;  // (opaque polygons only)
    for (s32 y = ytop; y < ybot; y++) {
        if (y < 0 || y >= H) continue;
        Edge L, R;
        if (!edges_at(p, y, &L, &R)) continue;
        s32 a = edge_left(&L, y), b = edge_right(&R, y);
        if (full) {  // (and the x-major edges' runs on the line are the polygon's too)
            if (R.sign && !R.xmajor) b++;
            if (R.xmajor && R.sign < 0) {
                s32 v = (s32)((edge_x(&R, y, 0) + (1 << 17)) >> 18) - 1;
                if (v > b) b = v;
            }
            if (L.xmajor && L.sign > 0) {
                s32 v = (s32)((edge_x(&L, y, 0) + (1 << 17)) >> 18);
                if (v < a) a = v;
            }
        }
        // the left edge's own pixels are drawn even where the right edge is already left of them
        if (!L.xmajor) {
            if (b < a) b = a;
        } else if (L.sign < 0) {
            s32 run = (s32)((edge_x(&L, y, 0) + (1 << 17)) >> 18) - 1;
            if (b < run) b = run;
            if (b < a) b = a;
        }
        if (b < a) continue;
        s32 lmin = a, rmax = b;
        const Edge *el = &L, *er = &R;
        // the attributes at the two ends of the span, interpolated along the edges
        s32 ends[2][7];  // z, w, r, g, b, s, t
        s32 cx[2];       // the x the colours are interpolated between (span_end)
        const Edge *es[2] = {el, er};
        for (int k = 0; k < 2; k++) {
            const Edge *e = es[k];
            Interp ip;
            interp_setup(&ip, y, e->y0, e->y1, p->w[e->v0], p->w[e->v1], 9);
            const G3Vertex *a = p->v[e->v0], *b = p->v[e->v1];
            ends[k][0] = p->wbuffer ? interp(&ip, p->z[e->v0], p->z[e->v1])
                                    : interp_z_linear(y, e->y0, e->y1, p->z[e->v0], p->z[e->v1]);
            s32 yc = y + span_end(e, k, y, &cx[k]);
            {   // (the W the span interpolates with: at the colours' line)
                Interp iw;
                interp_setup(&iw, yc, e->y0, e->y1, p->w[e->v0] & ~1, p->w[e->v1] & ~1, 9);
                ends[k][1] = interp(&iw, p->w[e->v0], p->w[e->v1]);
                if (p->wbuffer) ends[k][0] = interp(&iw, p->z[e->v0], p->z[e->v1]);  // (the depth: W scaled)
            }
            s32 w0 = p->w[e->v0], w1 = p->w[e->v1];
            Interp ic = {0, 0};
            if (!exact_ok(w0, w1)) interp_setup(&ic, yc, e->y0, e->y1, w0 & ~1, w1 & ~1, 9);  // (W's bit 0 unused)
            const s32 va[5] = {a->color[0], a->color[1], a->color[2], a->tex[0], a->tex[1]};
            const s32 vb[5] = {b->color[0], b->color[1], b->color[2], b->tex[0], b->tex[1]};
            for (int c = 0; c < 5; c++)
                ends[k][2 + c] = exact_ok(w0, w1) ? lerp_floor(va[c], vb[c], yc - e->y0, e->y1 - e->y0)
                                                  : interp(&ic, va[c], vb[c]);
        }
        s32 xl = lmin, xr = rmax;
        // (anti-aliasing on a line where both edges start: the right edge's run begins where the left one, going
        // right, leaves the line (x-major) or after its pixel (y-major): fitted)
        s32 rsn = 0, rsd = 1;
        if (aa && y == R.y0 && y == L.y0 && R.xmajor && L.sign > 0) {
            if (L.xmajor) {
                rsn = (L.x0 - R.x0) * (L.y1 - L.y0) + (L.x1 - L.x0);
                rsd = L.y1 - L.y0;
            } else  // (after the left edge's pixel)
                rsn = (s32)(edge_x(&L, y, 0) >> 18) + 1 - R.x0;
        }
        // the span: one perspective factor per pixel, or exact linear steps (a quotient and a remainder per
        // attribute: the same values as a division per pixel)
        int ex = exact_ok(ends[0][1], ends[1][1]);
        int flat = cx[1] == cx[0];
        s32 span = cx[1] - cx[0];
        s32 q[6];
        s64 rem[6], dq[6], dr[6];
        if (!flat && ex) {
            for (int c = 0; c < 5; c++) {  // value(x) = a0 + floor(d * (x - cx0) / span)
                s64 d = (s64)ends[1][2 + c] - ends[0][2 + c], num = d * (xl - cx[0]);
                s64 fq = num >= 0 ? num / span : -((-num + span - 1) / span);
                q[c] = ends[0][2 + c] + (s32)fq;
                rem[c] = num - fq * span;
                dq[c] = d >= 0 ? d / span : -((-d + span - 1) / span);
                dr[c] = d - dq[c] * span;
            }
        }
        // (the pixels the edges' anti-aliasing can reach on this line; elsewhere the coverage is 31)
        s32 al0 = 1, al1 = 0, ar0 = 1, ar1 = 0;
        if (aa) {
            s64 l0 = edge_x(&L, y, 0) >> 18, l1 = edge_x(&L, y, 1) >> 18;
            s64 r0 = edge_x(&R, y, 0) >> 18, r1 = edge_x(&R, y, 1) >> 18;
            al0 = (s32)(l0 < l1 ? l0 : l1) - 2, al1 = (s32)(l0 > l1 ? l0 : l1) + 2;
            s32 sh = rsd ? (rsn >= 0 ? rsn : -rsn) / rsd + 1 : 0;
            ar0 = (s32)(r0 < r1 ? r0 : r1) - 2 - sh, ar1 = (s32)(r0 > r1 ? r0 : r1) + 2 + sh;
        }
        for (s32 x = xl; x <= xr; x++) {
            s32 col[5];
            if (flat) {
                for (int c = 0; c < 5; c++) col[c] = ends[0][2 + c];
            } else if (ex) {
                for (int c = 0; c < 5; c++) {
                    col[c] = q[c];
                    q[c] += (s32)dq[c];
                    rem[c] += dr[c];
                    if (rem[c] >= span) q[c]++, rem[c] -= span;
                }
            }
            if (x < 0 || x >= W) continue;
            Interp ic = {0, 0};
            if (!flat && (p->wbuffer || !ex)) interp_setup(&ic, x, cx[0], cx[1], ends[0][1], ends[1][1], 8);
            s32 z;
            if (p->wbuffer)  // (W, interpolated as the colours are: fitted to melonDS)
                z = flat ? ends[0][0] : interp(&ic, ends[0][0], ends[1][0]);
            else
                z = interp_z_linear(x, xl, xr, ends[0][0], ends[1][0]);
            if (!flat && !ex)
                for (int c = 0; c < 5; c++) col[c] = interp(&ic, ends[0][2 + c], ends[1][2 + c]);
            int cov = 31;
            if (aa) {
                // (where the left edge cuts the pixel its coverage counts, else the right edge's: fitted)
                int cl = x >= al0 && x <= al1 ? edge_coverage(&L, 0, x, y, 0, 1) : 31;
                cov = cl < 31 ? cl : x >= ar0 && x <= ar1 ? edge_coverage(&R, 1, x, y, rsn, rsd) : 31;
                if (!cov) continue;
            }
            put_pixel(p, x, y, z, col, idx, cov);
        }
    }
}

static void clear_buffers(void) {
    const G3RenderState *r = sR;
    u32 c = r->clearColor;
    u32 col = rgb5_to_6(c & 31) | rgb5_to_6(c >> 5 & 31) << 8 | rgb5_to_6(c >> 10 & 31) << 16 |
              (u32)(r->clearAttrHi & 31) << 24;
    u32 depth = (u32)(r->clearDepth & 0x7fff) * 0x200 + ((r->clearDepth & 0x7fff) == 0x7fff ? 0x1ff : 0);
    for (int i = 0; i < W * H; i++) {
        sColor[i] = col;
        sDepth[i] = depth;
        sOpaqueId[i] = (u8)(r->clearAttrHi >> 8 & 63);
        sTransId[i] = 0xff;
        sFlags[i] = c & 0x8000 ? 1 : 0;
        sPoly[i] = -1;
        sCov[i] = 31;
        sColor2[i] = col;
        sDepth2[i] = depth;
    }
}

// fog (DISP3DCNT bit 7; GBATEK "Fog"): pixels with the fog flag move towards FOG_COLOR by a density from the depth
// (FOG_TABLE between the boundaries FOG_OFFSET + FOG_STEP * (n + 1)); bit 6: alpha only
// anti-aliasing (DISP3DCNT bit 4): an edge pixel is blended with the layer below by its coverage, like alpha
// blending (fitted: below a transparent layer it keeps its colour and takes the coverage as alpha)
static void aa_pass(void) {
    if (!(sR->dispcnt & 0x10)) return;
    for (int i = 0; i < W * H; i++)
        if (sCov[i] < 31) sColor[i] = aa_resolve(sColor[i], sColor2[i], sCov[i]);
}

// (a division by 2^k as C's, truncating towards zero, with shifts: the 3DS's CPU has no divide instruction)
static inline s32 div_pow2(s32 v, int k) { return v >= 0 ? v >> k : -(-v >> k); }
static void fog_pass(void) {
    const G3RenderState *r = sR;
    if (!(r->dispcnt & 0x80) || getenv("ACWW_GPU3D_NOFOG")) return;  // (NOFOG: diagnostics)
    int shift = r->dispcnt >> 8 & 15;
    s32 step = shift <= 10 ? 0x400 >> shift : 0;
    int k = 10 - shift;  // (step = 2^k)
    u32 fc = r->fogColor;
    u32 fr = rgb5_to_6(fc & 31), fgc = rgb5_to_6(fc >> 5 & 31), fb = rgb5_to_6(fc >> 10 & 31), fa = fc >> 16 & 31;
    for (int i = 0; i < W * H; i++) {
        if (!(sFlags[i] & 1)) continue;
        s32 z = (s32)(sDepth[i] >> 9);
        s32 d;
        s32 b0 = r->fogOffset + step;
        if (z < b0 || !step) d = z < b0 ? r->fogTable[0] : r->fogTable[31];
        else {
            s32 n = (z - b0) >> k;  // (z >= b0)
            if (n >= 31) d = r->fogTable[31];
            else {
                s32 frac = (z - b0) - n * step;
                d = r->fogTable[n] + div_pow2((r->fogTable[n + 1] - r->fogTable[n]) * frac, k);
            }
        }
        if (d >= 127) d = 128;
        u32 c = sColor[i];
        u32 a = c >> 24 & 31;
        a = (fa * d + a * (128 - d)) >> 7;
        if (r->dispcnt & 0x40) c = (c & 0xffffff) | a << 24;
        else {
            u32 cr = (fr * d + (c & 63) * (128 - d)) >> 7, cg = (fgc * d + (c >> 8 & 63) * (128 - d)) >> 7,
                cb = (fb * d + (c >> 16 & 63) * (128 - d)) >> 7;
            c = cr | cg << 8 | cb << 16 | a << 24;
        }
        sColor[i] = c;
    }
}

// (diagnostics: ACWW_GPU3D_TIMING=1: the rasteriser's time per frame, printed every 1000 frames)
#include "../plat.h"  // (host_plat_time_ns: the C library's timespec_get isn't everywhere, e.g. newlib)
static double sRenderNs, sRenderMax;
static long sRenderFrames;
static void timing_report(void) {
    if (sRenderFrames)
        fprintf(stderr, "gpu3d: rasteriser %.3f ms a frame on average, %.3f ms at most, %ld frames\n",
                sRenderNs / sRenderFrames / 1e6, sRenderMax / 1e6, sRenderFrames);
}
// the texture of texparam / texpal (its size from texparam), row by row, as the 3DS GPU's RGBA8 (r << 24 | g << 16 |
// b << 8 | a): gpu3d_pica.c decodes each texture once with the same texel fetch as this rasteriser
void g3_texture_rgba(u32 texparam, u32 texpal, u32 *out) {
    int w = 8 << (texparam >> 20 & 7), h = 8 << (texparam >> 23 & 7);
    for (int t = 0; t < h; t++)
        for (int s = 0; s < w; s++) {
            u32 c = g3_texel(texparam, texpal, s, t);
            u32 r = c & 31, g = c >> 5 & 31, b = c >> 10 & 31, a = c >> 16 & 31;
            *out++ = (r << 3 | r >> 2) << 24 | (g << 3 | g >> 2) << 16 | (b << 3 | b >> 2) << 8 | (a << 3 | a >> 2);
        }
}

// ACWW_RENDERER3D=pica (the 3DS): the polygons drawn by its GPU (gpu3d_pica.c) into this rasteriser's buffers, then
// its fog pass
static int sPica, sPicaDirect, sPicaLastDirect;
int gpu3d_use_pica(void) {
    if (!g3p_init()) return 0;
    sPica = 1;
    return 1;
}
void gpu3d_pica_direct(int on) { sPicaDirect = on; }
void *gpu3d_pica_picture(void) { return sPicaLastDirect ? g3p_picture() : 0; }

// ACWW_RENDERER3D=wgpu: the polygons drawn by the GPU (gpu3d_wgpu.c), read back into the same frame buffer or not
static int sWgpu, sWgpuReadback;
int gpu3d_use_wgpu(void *device, void *queue, int scale, int readback) {
    if (!g3w_init(device, queue, readback ? 1 : scale)) return 0;
    sWgpu = 1;
    sWgpuReadback = readback;
    return 1;
}
void *gpu3d_wgpu_picture(void) { return g3w_picture(); }
void g3_render(void) {
    // the picture of the last one again: nothing it is drawn from changed (gG3Render is set at a swap, the
    // textures change with VRAMCNT: g3_tex_setup). The game swaps every third frame.
    static u32 drawnSwaps = ~0u, drawnGeneration = ~0u;
#ifdef N3DS_DIRECT  // (the direct build: the same list again between the game's pictures, interpolated: gpu3d_pica.c)
    int between = sPica && g3p_between(&gG3Render) && sPicaLastDirect;
#endif
    if (gG3Swaps == drawnSwaps && gG3VramGeneration == drawnGeneration && gG3VramGeneration != ~0u && !sDiag) {
#ifdef N3DS_DIRECT
        if (between) g3p_render(sR, sImgFlat, sPalFlat, 0, 0, 0, 0);
#endif
        return;
    }
    drawnSwaps = gG3Swaps;
    drawnGeneration = gG3VramGeneration;
    g3_render_wait();  // (the textures below)
    static int timing = -1;
    uint64_t t0 = 0;
    if (timing < 0) {
        timing = getenv("ACWW_GPU3D_TIMING") != 0;
        (void)timing_report;
    }
    if (timing) t0 = host_plat_time_ns();
    sR = &gG3Render;
    int texChanged = g3_tex_setup();
    if (sWgpu)  // (the GPU's rasteriser: gpu3d_wgpu.c)
        g3w_render(sR, sImgFlat, sPalFlat, sWgpuReadback ? sColor : NULL);
    else if (sPica) {  // (the 3DS GPU's: gpu3d_pica.c; no anti-aliasing pass, it has no coverage)
        // (straight to the GPU's compositing unless the colour fog needs the fog pass here)
        sPicaLastDirect = sPicaDirect && (sR->dispcnt & 0xc0) != 0x80;
        g3p_render(sR, sImgFlat, sPalFlat, texChanged, sPicaLastDirect ? 0 : sColor, sDepth, sFlags);
        if (!sPicaLastDirect) fog_pass();
    } else {
        clear_buffers();
        sPE.p = 0;
        for (int i = 0; i < sR->npoly; i++) draw_polygon(sR->poly[i], i);
        fog_pass();
        aa_pass();
    }
    sShown ^= 1;
    if ((!sWgpu || sWgpuReadback) && !(sPica && sPicaLastDirect)) memcpy(sFrameBuf[sShown], sColor, sizeof sColor);
    if (timing) {
        double ns = (double)(host_plat_time_ns() - t0);
        sRenderNs += ns;
        if (ns > sRenderMax) sRenderMax = ns;
        sRenderFrames++;
        if (sRenderFrames % 1000 == 0) timing_report();  // (exits with _exit skip atexit handlers)
    }
}

const u32 *g3_frame_lines(void) { return sFrameBuf[sShown]; }
int g3_raster_pixel_polygon(int x, int y) { return sPoly[y * W + x]; }
u32 g3_raster_depth(int x, int y) { return sDepth[y * W + x]; }
void g3_raster_diagnostics(int on) { sDiag = on; }
int gpu3d_pixel_polygon(int x, int y, Gpu3dPolyInfo *info) {
    int i = sPoly[y * W + x];
    if (i < 0 || i >= gG3Render.npoly) return 0;
    const G3Polygon *p = gG3Render.poly[i];
    info->index = i;
    info->attr = p->attr;
    info->texparam = p->texparam;
    info->texpal = p->texpal;
    info->nverts = p->nv;
    for (int k = 0; k < p->nv; k++) {
        info->x[k] = p->v[k]->x;
        info->y[k] = p->v[k]->y;
        info->z[k] = p->z[k];
        info->w[k] = p->w[k];
    }
    info->translucent = p->translucent;
    return 1;
}
int gG3Diagnostics;
void g3_render_wait(void) { if (sPica) g3p_wait(); }
void gpu3d_set_diagnostics(int on) { sDiag = gG3Diagnostics = on; }

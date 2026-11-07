// The own 3D engine's geometry part (gpu3d.h): the command FIFO, the matrix stacks, vertices, lighting, texture
// coordinate generation, polygon setup, clipping, the box / position / vector tests and the registers the game reads
// back. Written from GBATEK ("DS 3D Geometry Commands", "DS 3D Matrix ...", "DS 3D Polygon ...", "DS 3D Test
// Functions", "DS 3D Status").

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpu3d_internal.h"
#include "../prof.h"

// ---- state
static int sGeomOn, sRenderOn;          // POWCNT1 bits 3 and 2
static u32 sDispCnt;                    // DISP3DCNT
static u32 sGxStat;                     // the bits kept: 1 box test result, 15 stack error, 30-31 FIFO IRQ mode
static u16 sRegs[0xa0 / 2];             // 0x04000320-0x040003bf as written
static u16 sDot1Depth = 0x7fff;         // DISP_1DOT_DEPTH (0x04000610)
u8 gG3VramCnt[9];

// matrices: 4x4, m[row * 4 + col], 20.12 fixed point; a vertex is a row vector multiplied from the left
typedef struct { s32 m[16]; } Mtx;
static int sMtxMode;
static Mtx sProj, sPos, sVec, sTex, sClip;
static Mtx sProjStack, sTexStack, sPosStack[32], sVecStack[32];
static int sProjSP, sTexSP, sPosSP;
static int sClipDirty = 1;

// vertex and polygon state
static s16 sVtx[3];                     // the last vertex's coordinates (VTX_XY and friends change two of them)
static u8 sVtxColor[3];                 // 5 bits each
static s16 sTexRaw[2], sTexCoord[2];
static s16 sNormal[3];
static s16 sLightDir[4][3];
static u32 sLightParam[4]; static s32 sLightVec[4][9];  // (diagnostics)
static u8 sLightColor[4][3];
static u8 sDiffuse[3], sAmbient[3], sSpecular[3], sEmission[3];
static int sShininessOn;
static u8 sShininess[128];
static u32 sPolyAttr, sCurPolyAttr, sTexParam, sTexPal;
static u32 sViewport[6];                // x1, y1 (from the top), x2, y2, width, height
static int sPolyType;                   // BEGIN_VTXS: 0 triangles, 1 quads, 2 triangle strip, 3 quad strip
static int sInBegin;
static G3Vertex sTempBuf[4];            // the vertices of the polygon being set up, in the order of sTemp (a strip's
static G3Vertex *sTemp[4] = {&sTempBuf[0], &sTempBuf[1], &sTempBuf[2], &sTempBuf[3]};  // next polygon: turned)
static int sVtxCount, sVtxInPoly, sStripPolys;
static G3Vertex *sStripRam[2];          // a strip's last two vertices in vertex RAM
static int sStripValid;                 // ... if the next polygon can share them
static s32 sPosResult[4];
static s16 sVecResult[3];

// the two banks of vertex and polygon RAM: one filled by the geometry engine, one drawn (the direct build: the GPU's
// vertex lists instead, gpu3d_pica.c)
#ifdef N3DS_DIRECT
enum { RAM_VERTICES = 1, RAM_POLYGONS = 1 };
#else
enum { RAM_VERTICES = MAX_VERTICES, RAM_POLYGONS = MAX_POLYGONS };
#endif
static G3Vertex sVertexRam[2][RAM_VERTICES];
static G3Polygon sPolyRam[2][RAM_POLYGONS];
static int sBank;
static int sNumVerts, sNumPolys, sNumOpaque;
static int sRamOverflow;
G3RenderState gG3Render;
const uint8_t *const *gG3Banks;
u32 gG3VramGeneration = ~0u;
u32 gG3Swaps;
void gpu3d_vram_generation(uint32_t generation) { gG3VramGeneration = generation; }
static int sRaster = 1;
void gpu3d_set_raster(int on) { sRaster = on; }
// the wide view: the x and y clip planes at +-num/den w (1/1: the DS's)
static int sWide;
static s64 sClipNum[3] = {1, 1, 1}, sClipDen[3] = {1, 1, 1};
#ifdef N3DS_DIRECT  // (the enhanced port: on unless ACWW_WIDE=0)
int gpu3d_wide_wanted(void) { return !getenv("ACWW_WIDE") || atoi(getenv("ACWW_WIDE")); }
#else
int gpu3d_wide_wanted(void) { return getenv("ACWW_WIDE") && atoi(getenv("ACWW_WIDE")); }
#endif
void gpu3d_set_wide(int on) {
    sWide = on;
    sClipNum[0] = on ? 128 + GPU3D_WIDE_X : 1, sClipDen[0] = on ? 128 : 1;
    sClipNum[1] = on ? 96 + GPU3D_WIDE_Y : 1, sClipDen[1] = on ? 96 : 1;
}
int gpu3d_wide(void) { return sWide; }

// ---- feature census
static u64 sFeatureFrame[F_COUNT];
static u64 sFrame;
static int sFeaturesOn = -1;
void g3_feature(int id) {
    if (sFeaturesOn && !sFeatureFrame[id]) sFeatureFrame[id] = sFrame + 1;
}
// (the check inline: the census is mostly off, and some features are noted for every vertex)
#define g3_feature(id) (sFeaturesOn ? g3_feature(id) : (void)0)

// a 64-bit division as C's, in 32 bits when both fit (the same quotient: 64-bit divisions are many times slower on
// the 3DS's CPU, which has no divide instruction at all)
static inline s64 div64(s64 a, s64 b) {
    if (a == (s32)a && b == (s32)b && b != -1) return (s32)a / (s32)b;
    return a / b;
}

// ---- the command FIFO: a 4-entry pipe in front of a 256-entry FIFO (GXSTAT counts the FIFO's entries)
typedef struct { u8 cmd; u32 param; } Entry;
static Entry sPipe[4], sFifo[256];
static int sPipeHead, sPipeN, sFifoHead, sFifoN;
static int sPushPopQueued, sTestQueued;
static int sFlushPending;               // SWAP_BUFFERS waits for the frame's end
static u32 sFlushParam;

static const u8 kParams[256] = {
    [0x10] = 1, [0x11] = 0, [0x12] = 1, [0x13] = 1, [0x14] = 1, [0x15] = 0, [0x16] = 16, [0x17] = 12, [0x18] = 16,
    [0x19] = 12, [0x1a] = 9, [0x1b] = 3, [0x1c] = 3, [0x20] = 1, [0x21] = 1, [0x22] = 1, [0x23] = 2, [0x24] = 1,
    [0x25] = 1, [0x26] = 1, [0x27] = 1, [0x28] = 1, [0x29] = 1, [0x2a] = 1, [0x2b] = 1, [0x30] = 1, [0x31] = 1,
    [0x32] = 1, [0x33] = 1, [0x34] = 32, [0x40] = 1, [0x41] = 0, [0x50] = 1, [0x60] = 1, [0x70] = 3, [0x71] = 2,
    [0x72] = 1,
};
static int is_command(u8 c) {
    return (c >= 0x10 && c <= 0x1c) || (c >= 0x20 && c <= 0x2b) || (c >= 0x30 && c <= 0x34) || c == 0x40 ||
           c == 0x41 || c == 0x50 || c == 0x60 || (c >= 0x70 && c <= 0x72);
}

static void exec_entry(Entry e);
static void fifo_put(u8 cmd, u32 param) {
    Entry e = {cmd, param};
    // nothing queued and nothing holding it back: run at once, as the drain after the write (gpu3d_write32) would
    if (!sPipeN && !sFifoN && sGeomOn && !sFlushPending) {
        exec_entry(e);
        return;
    }
    if (cmd == 0x11 || cmd == 0x12) sPushPopQueued++;
    if (cmd >= 0x70 && cmd <= 0x72) sTestQueued++;
    if (sFifoN == 0 && sPipeN < 4) {
        sPipe[(sPipeHead + sPipeN++) & 3] = e;
    } else if (sFifoN < 256) {
        sFifo[(sFifoHead + sFifoN++) & 255] = e;
    } else {
        // full: the DS stalls the CPU until there is room; here nothing can make room while a swap waits
        g3_feature(F_FIFO_FULL);  // (dropped, as melonDS does: the DS would stall the CPU until there is room)
    }
    if (sFlushPending) g3_feature(F_FIFO_QUEUED);
    if (sFlushPending && cmd == 0x50) g3_feature(F_SWAP_QUEUED);
    if (!sGeomOn) g3_feature(F_GEOM_OFF_WRITE);
}
static Entry fifo_get(void) {
    Entry e = sPipe[sPipeHead];
    sPipeHead = (sPipeHead + 1) & 3;
    sPipeN--;
    if (sPipeN <= 2) {  // the pipe refills two entries at a time
        for (int i = 0; i < 2 && sFifoN; i++) {
            sPipe[(sPipeHead + sPipeN++) & 3] = sFifo[sFifoHead];
            sFifoHead = (sFifoHead + 1) & 255;
            sFifoN--;
        }
    }
    if (e.cmd == 0x11 || e.cmd == 0x12) sPushPopQueued--;
    if (e.cmd >= 0x70 && e.cmd <= 0x72) sTestQueued--;
    return e;
}

// ---- the geometry engine's pipelines, for GXSTAT's busy bit (27). The host runs commands as they are written, but
// a command leaves work behind that takes time on the DS: the normal's lighting, the vertex transform, the polygon
// setup that frees vertex slots every 9 cycles. GXSTAT says busy until those are done, and since no time passes
// between the game's register accesses, until a later command's cycles (or the frame's end) complete them. The game
// polls the bit (G3X_Reset, G3X_GetClipMtx), so this decides how long it waits. The cycle counts are those of the
// renderer the oracle's captures were made with (measured command by command against it).
static s32 sVtxPipe, sNrmPipe, sPolyPipe, sSlotCtr;
static u32 sSlots = 1;          // the vertex slots: bit 0 set when the next vertex can enter
static int sMtxModeForCost;
static int sPolyDone;           // set by submit_polygon: the polygon setup this vertex started (see timing_after)
static int sPolyOutcome, sPolyNOut, sPolyClipped, sFarHidden;  // (the last polygon: 0 stored, 1 culled, 2 clipped away, 3 far plane, 4 1-dot, 5 RAM full)
static unsigned sPolySerial;
int gpu3d_debug_last_polygon(int *nout, int *clipped, unsigned *serial, int *type) { *nout = sPolyNOut; *clipped = sPolyClipped; *serial = sPolySerial; *type = sPolyType | (sStripPolys > 1) << 2; return sPolyOutcome; }
static void add_cycles(s32 n) {
    sVtxPipe = n < sVtxPipe ? sVtxPipe - n : 0;
    if (n < sPolyPipe) {
        sPolyPipe -= n;
        sSlotCtr += n;
        while (sSlotCtr > 9) { sSlots >>= 1; sSlotCtr -= 9; }
    } else {
        sPolyPipe = 0;
        sSlots = 1;
        sSlotCtr = 0;
    }
}
static void next_vertex_slot(void) {
    while (!(sSlots & 1)) add_cycles(10 - sSlotCtr);
    if (sPolyPipe) sSlots &= ~1u;  // (the vertex takes the slot)
}
static int is_vertex_cmd(u8 c) { return c >= 0x23 && c <= 0x28; }
// a vertex command's first entry (VTX_16 has two): it waits for a vertex slot
static void vertex_entry_first(void) {
    if (!(sSlots & 1)) next_vertex_slot();
    else add_cycles(1);
    sNrmPipe = 0;
}
static void timing_before(u8 c) {
    sMtxModeForCost = sMtxMode;
    if (is_vertex_cmd(c)) {
        if (c != 0x23) vertex_entry_first();  // (VTX_16: at its first parameter)
        return;
    }
    switch (c) {
    case 0x20: case 0x30: case 0x31: case 0x72:  // can follow a vertex after 6 cycles
        add_cycles(sVtxPipe > 2 ? sVtxPipe - 2 + 1 : sNrmPipe + 1);
        break;
    case 0x29: case 0x2a: case 0x2b: case 0x33: case 0x34: case 0x41: case 0x60: case 0x71:  // after 8
        add_cycles(sVtxPipe > 0 ? sVtxPipe + 1 : sNrmPipe + 1);
        break;
    default:
        add_cycles(sNrmPipe + 1);
        break;
    }
    sNrmPipe = 0;
}

static s32 command_cycles(u8 c) {  // the cycles after the first
    switch (c) {
    case 0x11: case 0x13: return 16;
    case 0x12: case 0x14: return 35;
    case 0x15: return sMtxModeForCost == 3 ? 0 : 18;
    case 0x16: return sMtxModeForCost == 3 ? 10 : 18;
    case 0x17: return sMtxModeForCost == 3 ? 10 : 18;
    case 0x18: return 53;
    case 0x19: return 53;
    case 0x1a: return 56;
    case 0x1b: return 32;
    case 0x1c: return 62;
    case 0x21: return 4;
    case 0x30: case 0x31: return 3;
    case 0x32: return 5;
    case 0x33: return 1;
    case 0x50: return 324;
    default: return 0;
    }
}
static void timing_after(u8 c) {
    if (is_vertex_cmd(c)) {
        add_cycles(c == 0x23 ? 4 : 3);
        sVtxPipe = 4;
        if (sPolyDone) {  // the polygon setup: shorter for one that was rejected (culled, clipped away, ...)
            sPolyDone = 0;
            int strip = sPolyType >= 2, quad = sPolyNOut == 4;
            if (sPolyOutcome) { sPolyPipe = 5; sSlots = 0x1e; }
            else if (quad) { sPolyPipe = 32; sSlots = strip ? 0x1c : 0x1e; }
            else { sPolyPipe = 23; sSlots = strip ? 0x08 : 0x0e; }
            sSlotCtr = 4;
        }
        return;
    }
    if (c == 0x40) add_cycles(sPolyPipe);  // (BEGIN_VTXS waits for the polygon setup)
    add_cycles(command_cycles(c));
    if (c == 0x21) sNrmPipe = 7;
}
static int pipelines_busy(void) { return sVtxPipe || sNrmPipe || sPolyPipe; }

// packed commands written to GXFIFO (0x04000400): a word of up to four command bytes, then their parameters
static u32 sPackCmds;
static int sPackLeft, sPackParamsLeft;
static void pack_advance(void) {
    while (sPackLeft > 0) {
        u8 c = sPackCmds & 0xff;
        if (sPackCmds == 0) { sPackLeft = 0; break; }  // (the rest is padding)
        if (kParams[c]) { sPackParamsLeft = kParams[c]; return; }
        if (c) fifo_put(c, 0);  // a command without parameters (00: a NOP, not queued)
        sPackCmds >>= 8;
        sPackLeft--;
    }
    sPackParamsLeft = 0;
}
// (nothing is queued while the geometry engine is off: gpu3d_write32)
static void gxfifo_write(u32 v) {
    if (sPackParamsLeft == 0) {
        if (v == 0) { fifo_put(0, 0); return; }  // (a word of four NOPs is queued as one NOP)
        sPackCmds = v;
        sPackLeft = 4;
        pack_advance();
    } else {
        fifo_put(sPackCmds & 0xff, v);
        if (--sPackParamsLeft == 0) {
            sPackCmds >>= 8;
            sPackLeft--;
            pack_advance();
        }
    }
}

// ---- matrices
static void mtx_identity(Mtx *a) {
    memset(a, 0, sizeof *a);
    a->m[0] = a->m[5] = a->m[10] = a->m[15] = 0x1000;
}
// *cur = a * *cur
static void mtx_mult(Mtx *cur, const Mtx *a) {
    Mtx r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            s64 s = 0;
            for (int k = 0; k < 4; k++) s += (s64)a->m[i * 4 + k] * cur->m[k * 4 + j];
            r.m[i * 4 + j] = (s32)(s >> 12);
        }
    *cur = r;
}
static void mtx_scale(Mtx *cur, const s32 *p) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 4; j++) cur->m[i * 4 + j] = (s32)(((s64)p[i] * cur->m[i * 4 + j]) >> 12);
}
static void mtx_trans(Mtx *cur, const s32 *p) {
    for (int j = 0; j < 4; j++)
        cur->m[12 + j] += (s32)(((s64)p[0] * cur->m[j] + (s64)p[1] * cur->m[4 + j] + (s64)p[2] * cur->m[8 + j]) >> 12);
}
#ifdef N3DS_DIRECT
static int sSeg = -1;  // the id of sClip in the list being filled (g3p_matrix), -1 none yet
#endif
static void update_clip(void) {
    if (!sClipDirty) return;
    sClip = sProj;
    mtx_mult(&sClip, &sPos);  // clip = position * projection
    sClipDirty = 0;
#ifdef N3DS_DIRECT
    sSeg = -1;
#endif
}

// the matrix (or matrices) a command of the current mode changes: mode 0 projection, 1 position, 2 position and
// vector, 3 texture
static void apply_mult(const Mtx *a) {
    switch (sMtxMode) {
    case 0: mtx_mult(&sProj, a); sClipDirty = 1; break;
    case 1: mtx_mult(&sPos, a); sClipDirty = 1; break;
    case 2: mtx_mult(&sPos, a); mtx_mult(&sVec, a); sClipDirty = 1; break;
    default: mtx_mult(&sTex, a); break;
    }
}
static void apply_load(const Mtx *a) {
    switch (sMtxMode) {
    case 0: sProj = *a; sClipDirty = 1; break;
    case 1: sPos = *a; sClipDirty = 1; break;
    case 2: sPos = *a; sVec = *a; sClipDirty = 1; break;
    default: sTex = *a; break;
    }
}

static void stack_error(void) {
    sGxStat |= 1u << 15;
    g3_feature(F_STACK_ERROR);
}
// The stacks (GBATEK "DS 3D Matrix Stack"): projection and texture one entry with a 1-bit pointer, position and
// vector 31 entries (0-30) with a 6-bit pointer; the slot used is the pointer's low 5 bits, 31 and above set the
// error flag (GXSTAT bit 15) but are still used.
static void mtx_push(void) {
    if (sMtxMode == 0) {
        if (sProjSP) stack_error();
        sProjStack = sProj;
        sProjSP ^= 1;
    } else if (sMtxMode == 3) {
        if (sTexSP) stack_error();
        sTexStack = sTex;
        sTexSP ^= 1;
    } else {
        if (sPosSP >= 31) stack_error();
        sPosStack[sPosSP & 31] = sPos;
        sVecStack[sPosSP & 31] = sVec;
        sPosSP = (sPosSP + 1) & 63;
    }
}
static void mtx_pop(u32 p) {
    if (sMtxMode == 0) {
        if (!sProjSP) stack_error();
        sProjSP ^= 1;
        sProj = sProjStack;
        sClipDirty = 1;
    } else if (sMtxMode == 3) {
        if (!sTexSP) stack_error();
        sTexSP ^= 1;
        sTex = sTexStack;
    } else {
        s32 off = (s32)(p << 26) >> 26;  // signed 6 bits
        sPosSP = (sPosSP - off) & 63;
        if (sPosSP >= 31) stack_error();
        sPos = sPosStack[sPosSP & 31];
        sVec = sVecStack[sPosSP & 31];
        sClipDirty = 1;
    }
}
static void mtx_store(u32 p) {
    if (sMtxMode == 0) sProjStack = sProj;
    else if (sMtxMode == 3) sTexStack = sTex;
    else {
        u32 i = p & 0x1f;
        if (i == 31) stack_error();
        sPosStack[i] = sPos;
        sVecStack[i] = sVec;
    }
}
static void mtx_restore(u32 p) {
    if (sMtxMode == 0) { sProj = sProjStack; sClipDirty = 1; }
    else if (sMtxMode == 3) sTex = sTexStack;
    else {
        u32 i = p & 0x1f;
        if (i == 31) stack_error();
        sPos = sPosStack[i];
        sVec = sVecStack[i];
        sClipDirty = 1;
    }
}

// ---- lighting (GBATEK "DS 3D Polygon Light Parameters")
static s16 sx10(u32 v) { return (s16)((v & 0x3ff) << 6) >> 3; }  // 1.0.9 -> 1.3.12
static void light_vector(u32 p) {
    int l = p >> 30;
    s32 d[3] = {sx10(p), sx10(p >> 10), sx10(p >> 20)};
    // the direction in the vector matrix's space, kept with 9 fractional bits like the parameter
    for (int i = 0; i < 3; i++)
        sLightDir[l][i] = (s16)((((s64)d[0] * sVec.m[i] + (s64)d[1] * sVec.m[4 + i] + (s64)d[2] * sVec.m[8 + i]) >> 12) >> 3);
    sLightParam[l] = p;
    for (int i = 0; i < 9; i++) sLightVec[l][i] = sVec.m[(i / 3) * 4 + i % 3];
    static int lightLog = -1;
    if (lightLog < 0) lightLog = getenv("ACWW_GPU3D_LIGHTLOG") != 0;
    if (lightLog)
        fprintf(stderr, "light %d param %08x dir %d %d %d vec %d %d %d / %d %d %d / %d %d %d -> %d %d %d\n", l, p, d[0], d[1],
                d[2], sVec.m[0], sVec.m[1], sVec.m[2], sVec.m[4], sVec.m[5], sVec.m[6], sVec.m[8], sVec.m[9], sVec.m[10],
                sLightDir[l][0], sLightDir[l][1], sLightDir[l][2]);
}
// (diagnostics: the inputs of the last 64k lighting computations)
typedef struct { s32 n[3]; s16 raw[3]; u32 lights; s16 dir[4][3]; u8 lc[4][3], dif[3], amb[3], spe[3], emi[3], out[3]; int table; s32 vec[9]; u32 lparam[4]; s32 lvec[4][9]; } LightRec;
static LightRec *sLightRecs;
static unsigned sLightRecN;
static int sCurLightRec = -1;
void gpu3d_debug_light(int id) {
    if (!sLightRecs || id < 0) return;
    const LightRec *r = &sLightRecs[id & 0xffff];
    fprintf(stderr, "      light: normal %d %d %d (raw %d %d %d) lights %x table %d dif %d,%d,%d amb %d,%d,%d spe %d,%d,%d emi %d,%d,%d out %d,%d,%d\n",
            r->n[0], r->n[1], r->n[2], r->raw[0], r->raw[1], r->raw[2], r->lights, r->table, r->dif[0], r->dif[1], r->dif[2],
            r->amb[0], r->amb[1], r->amb[2], r->spe[0], r->spe[1], r->spe[2], r->emi[0], r->emi[1], r->emi[2], r->out[0],
            r->out[1], r->out[2]);
    for (int l = 0; l < 4; l++)
        if (r->lights & (1u << l))
            fprintf(stderr, "        light %d dir %d %d %d colour %d,%d,%d\n", l, r->dir[l][0], r->dir[l][1], r->dir[l][2],
                    r->lc[l][0], r->lc[l][1], r->lc[l][2]);
}
static void lighting(void) {
    s32 n[3];
    for (int i = 0; i < 3; i++)
        n[i] = (s32)(((s64)sNormal[0] * sVec.m[i] + (s64)sNormal[1] * sVec.m[4 + i] + (s64)sNormal[2] * sVec.m[8 + i]) >> 12);
    if (sFeaturesOn > 1) {
        if (!sLightRecs) sLightRecs = calloc(65536, sizeof *sLightRecs);
        LightRec *r = &sLightRecs[sLightRecN & 0xffff];
        sCurLightRec = (int)(sLightRecN++ & 0xffff);
        memcpy(r->n, n, sizeof n);
        memcpy(r->raw, sNormal, sizeof r->raw);
        r->lights = sCurPolyAttr & 15;
        memcpy(r->dir, sLightDir, sizeof r->dir);
        memcpy(r->lc, sLightColor, sizeof r->lc);
        memcpy(r->dif, sDiffuse, 3);
        memcpy(r->amb, sAmbient, 3);
        memcpy(r->spe, sSpecular, 3);
        memcpy(r->emi, sEmission, 3);
        r->table = sShininessOn;
        for (int i = 0; i < 9; i++) r->vec[i] = sVec.m[(i / 3) * 4 + i % 3];
        memcpy(r->lparam, sLightParam, sizeof r->lparam);
        memcpy(r->lvec, sLightVec, sizeof r->lvec);
    }
    // levels from the dot products of 9-bit-fraction vectors (diffuse 9 bits, shininess 8); the colour sums kept with
    // 14 fractional bits (each term colour * light colour / 32 * level), then cut to 5 bits
    s32 n9[3] = {n[0] >> 3, n[1] >> 3, n[2] >> 3};
    s32 c[3] = {sEmission[0] << 14, sEmission[1] << 14, sEmission[2] << 14};
    g3_feature(F_LIGHT);
    for (int l = 0; l < 4; l++) {
        if (!(sCurPolyAttr & (1u << l))) continue;
        const s16 *L = sLightDir[l];
        // (the diffuse level: the products of the reversed light vector and the normal each cut to 9 fractional
        // bits, then added; not limited above: fitted to melonDS)
        s32 diffuse = ((-L[0] * n9[0]) >> 9) + ((-L[1] * n9[1]) >> 9) + ((-L[2] * n9[2]) >> 9);
        if (diffuse < 0) diffuse = 0;
        // the half vector between the light and the line of sight (0, 0, -1)
        s32 shine = -((L[0] >> 1) * n9[0] + (L[1] >> 1) * n9[1] + ((L[2] - 0x200) >> 1) * n9[2]) >> 10;
        if (shine < 0) shine = 0;
        if (shine > 255) shine = 255;
        shine = (shine * shine) >> 8;
        if (sShininessOn) { shine = sShininess[shine >> 1]; g3_feature(F_SHININESS_TABLE); }
        for (int k = 0; k < 3; k++) {  // (the sums keep 14 fractional bits: fitted to melonDS)
            s32 lc = sLightColor[l][k];
            c[k] += sSpecular[k] * lc * shine << 1;
            c[k] += sDiffuse[k] * lc * diffuse;
            c[k] += sAmbient[k] * lc << 9;
        }
    }
    for (int k = 0; k < 3; k++) c[k] >>= 14;
    for (int k = 0; k < 3; k++) sVtxColor[k] = (u8)(c[k] > 31 ? 31 : c[k]);
    if (sLightRecs && sCurLightRec >= 0) memcpy(sLightRecs[sCurLightRec].out, sVtxColor, 3);
}

// ---- polygons
static s64 floor_div(s64 a, s64 b) {
    s64 q = div64(a, b);
    return (q * b != a && (a < 0) != (b < 0)) ? q - 1 : q;
}
// the viewport transform (GBATEK "DS 3D Viewport"): x = (x + w) * width / 2w + x1, y the same from the bottom; W and
// the coordinate are first shifted right once when W has more than 16 bits
static void screen_position(G3Vertex *v) {
    if (v->screenDone) return;
    s64 w = v->pos[3];
    int s = w >= 0x10000;  // (one shift, however large W is: fitted to melonDS)
    s64 ws = w >> s;
    if (ws == 0) ws = 1;  // (a point at the eye)
    s64 nx = ((s64)v->pos[0] + w) >> s, ny = (-(s64)v->pos[1] + w) >> s;
    // (the whole pixels from the 1/16 ones: floor(floor(16 r) / 16) = floor(r), so two divisions, not four)
    s64 hx = floor_div(nx * sViewport[4] * 16, ws * 2) + sViewport[0] * 16;
    s64 hy = floor_div(ny * sViewport[5] * 16, ws * 2) + sViewport[3] * 16;
    v->x = (s32)(hx >> 4) & 0x1ff;
    v->y = (s32)(hy >> 4) & 0x1ff;  // (192 for the bottom edge: no 8-bit wrap)
    v->hx = sWide ? (s32)hx : (s32)hx & 0x1fff;
    v->hy = sWide ? (s32)hy : (s32)hy & 0x1fff;
    v->screenDone = 1;
}

// clipping against one plane (comp 0 x, 1 y, 2 z; sign +1: c <= w, -1: c >= -w); Sutherland-Hodgman, new vertices
// interpolated from the vertex inside
static G3Vertex sClipBuf[2][10];
static void clip_interp(G3Vertex *o, const G3Vertex *in, const G3Vertex *out, int comp, int sign) {
    s64 num = sClipNum[comp], dn = sClipDen[comp];  // (the plane c = sign num/dn w; the DS's: 1/1)
    s64 din = num * in->pos[3] - sign * dn * in->pos[comp];
    s64 dout = num * out->pos[3] - sign * dn * out->pos[comp];
    s64 den = din - dout;
    // from the vertex outside towards the one inside, truncated towards zero (dout <= 0 < din: -dout / den >= 0)
#define LERP(a, b) ((s32)((b) + div64(((s64)(a) - (b)) * -dout, den)))
    for (int i = 0; i < 4; i++) o->pos[i] = LERP(in->pos[i], out->pos[i]);
    o->pos[comp] = sign * (s32)(num * o->pos[3] / dn);
    {   // (the 5-bit colours from the vertex inside, rounded up; when that vertex is the crossing of the same edge
        // with this axis's other plane, from the edge's far end instead: fitted to melonDS)
        const s32 *ic = in->color;
        s64 cdin = din, cden = den;
        if (in->farAxis == comp + 1) {
            ic = in->farColor;
            cdin = num * in->farPos[3] - sign * dn * in->farPos[comp];
            cden = cdin - dout;
        }
        for (int i = 0; i < 3; i++) {  // (from the larger colour towards the smaller by a factor rounded up to 16
            // fractional bits, the result rounded up)
            s64 a = ic[i] >> 4, b = out->color[i] >> 4, base, diff, n;
            if (a >= b) base = a, diff = a - b, n = cdin;
            else base = b, diff = b - a, n = cden - cdin;  // (towards the inside vertex: -dout)
            s64 t = div64(n * 65536 + cden - 1, cden);
            s64 v = base * 65536 - diff * t;
            s32 c = (s32)(-((-v) >> 16));
            o->color[i] = c ? c << 4 | 0xf : 0;
        }
    }
    o->farAxis = (u8)(comp + 1);
    memcpy(o->farPos, out->pos, sizeof o->farPos);
    memcpy(o->farColor, out->color, sizeof o->farColor);
    for (int i = 0; i < 2; i++) o->tex[i] = (s16)LERP(in->tex[i], out->tex[i]);
#undef LERP
    o->dbgClips = (u8)(in->dbgClips + out->dbgClips + 1);
    for (int i = 0; i < 3; i++) o->dbgClip[i] = in->color[i], o->dbgClip[3 + i] = out->color[i];
    o->dbgClip[6] = (s32)din;
    o->dbgClip[7] = (s32)dout;
    o->clipped = 1;
    o->screenDone = 0;
}
static int inside(const G3Vertex *v, int comp, int sign) {
    s64 c = sClipDen[comp] * v->pos[comp], w = sClipNum[comp] * v->pos[3];
    return sign > 0 ? c <= w : c >= -w;
}
static int clip_plane(G3Vertex *dst, const G3Vertex *src, int n, int comp, int sign) {
    int m = 0;
    for (int i = 0; i < n; i++) {
        // a vertex inside stays; one outside is replaced by the crossings of its two edges that come from inside
        // (the one from the previous vertex first)
        const G3Vertex *cur = &src[i], *prev = &src[(i + n - 1) % n], *next = &src[(i + 1) % n];
        if (inside(cur, comp, sign)) { dst[m++] = *cur; continue; }
        if (inside(prev, comp, sign)) clip_interp(&dst[m++], prev, cur, comp, sign);
        if (inside(next, comp, sign)) clip_interp(&dst[m++], next, cur, comp, sign);
    }
    return m;
}
// returns the number of vertices left in *out (0: nothing visible; *out NULL: the polygon as it came, no plane cut
// it); *clipped if any plane cut it or it crosses the far plane. (The vertices are copied only when a plane cuts.)
static int clip_polygon(G3Vertex *const *in, int n, G3Vertex **out, int farClip, int *clipped) {
    G3Vertex *a = 0, *b = sClipBuf[1];
    *clipped = 0;
    *out = 0;
    // the far plane: polygons crossing it are hidden unless attr bit 12
    for (int i = 0; i < n; i++)
        if (in[i]->pos[2] > in[i]->pos[3]) {
            *clipped = 1;
            if (!farClip) { g3_feature(F_FARCLIP_HIDE); sFarHidden = 1; return 0; }
        }
    static int kPlanes[6][2] = {{2, 1}, {2, -1}, {1, 1}, {1, -1}, {0, 1}, {0, -1}};  // (z, then y, then x)
    {   // (experiment: ACWW_GPU3D_CLIPORDER=zZxXyY, lower case the far / right / top plane)
        static int init;  // (read once: getenv here, for every clipped polygon, was much of the clipping's time)
        const char *o = init ? 0 : getenv("ACWW_GPU3D_CLIPORDER");
        init = 1;
        if (o && strlen(o) == 6)
            for (int i = 0; i < 6; i++) {
                char c = o[i];
                kPlanes[i][0] = (c | 0x20) == 'x' ? 0 : (c | 0x20) == 'y' ? 1 : 2;
                kPlanes[i][1] = c >= 'a' ? 1 : -1;
            }
    }
    for (int p = 0; p < 6; p++) {
        int comp = kPlanes[p][0], sign = kPlanes[p][1];
        int all = 1;
        for (int i = 0; i < n; i++) if (!inside(a ? &a[i] : in[i], comp, sign)) { all = 0; break; }
        if (all) continue;
        *clipped = 1;
        if (!a) {
            a = sClipBuf[0];
            for (int i = 0; i < n; i++) a[i] = *in[i];
        }
        n = clip_plane(b, a, n, comp, sign);
        G3Vertex *t = a; a = b; b = t;
        if (n == 0) return 0;
    }
    *out = a;
    return n;
}

// (diagnostics: ACWW_GPU3D_DEBUGTEX=<texparam hex>: every polygon with that texture, its outcome and vertices)
static void debug_tex_poly(const char *what, G3Vertex *const *v, int nv, int n) {
    static long want = -2;
    if (want == -2) want = getenv("ACWW_GPU3D_DEBUGTEX") ? (long)strtoul(getenv("ACWW_GPU3D_DEBUGTEX"), 0, 16) : -1;
    if (want < 0 || (u32)want != sTexParam) return;
    fprintf(stderr, "debugtex frame %llu %s type %d n %d -> %d:", (unsigned long long)sFrame, what, sPolyType, nv, n);
    for (int i = 0; i < nv; i++) fprintf(stderr, " (%d %d %d %d)", v[i]->pos[0], v[i]->pos[1], v[i]->pos[2], v[i]->pos[3]);
    fprintf(stderr, "\n");
}

static void submit_polygon(void) {
    sPolyDone = 1;
    sPolySerial++;
    int nv = sPolyType & 1 ? 4 : 3;
    G3Vertex *v[4];
    // the vertex order of the polygon: strips reuse the previous vertices
    if (sPolyType == 2 && (sStripPolys & 1)) { v[0] = sTemp[1]; v[1] = sTemp[0]; v[2] = sTemp[2]; }
    else if (sPolyType == 3) { v[0] = sTemp[0]; v[1] = sTemp[1]; v[2] = sTemp[3]; v[3] = sTemp[2]; }
    else for (int i = 0; i < nv; i++) v[i] = sTemp[i];

    // facing: the sign of the determinant of (x, y, w) of the first three vertices (clip space)
    s64 ax = v[1]->pos[0] - v[0]->pos[0], ay = v[1]->pos[1] - v[0]->pos[1], aw = v[1]->pos[3] - v[0]->pos[3];
    s64 bx = v[2]->pos[0] - v[0]->pos[0], by = v[2]->pos[1] - v[0]->pos[1], bw = v[2]->pos[3] - v[0]->pos[3];
    s64 nx = ay * bw - aw * by, ny = aw * bx - ax * bw, nw = ax * by - ay * bx;
    s64 dot = nx * v[0]->pos[0] + ny * v[0]->pos[1] + nw * v[0]->pos[3];
    int facing = dot >= 0;  // front: the vertices clockwise on the screen
    u32 attr = sCurPolyAttr;
    int culled = facing ? !(attr & 0x80) : !(attr & 0x40);
    sPolyOutcome = 0;
    sPolyNOut = 0;
    if (culled) {
        debug_tex_poly("culled", v, nv, 0);
        g3_feature(F_CULLED);
        sStripValid = 0;
        sPolyOutcome = 1;
        return;
    }

#ifdef N3DS_DIRECT
    {   // the direct build: the polygon straight into the GPU's list (g3p_polygon), which clips it; here only the DS's
        // far-plane rule (crossing it: hidden unless attr bit 12) and what lies wholly beyond the near or far plane
        int far = 0, near = 0;
        for (int i = 0; i < nv; i++) {
            far += v[i]->pos[2] > v[i]->pos[3];
            near += v[i]->pos[2] < -v[i]->pos[3];
        }
        if ((far && !(attr & 0x1000)) || far == nv || near == nv) {
            sPolyOutcome = far ? 3 : 2;
            return;
        }
        if (sNumPolys >= MAX_POLYGONS) { sPolyOutcome = 5; return; }
        u32 alpha = attr >> 16 & 31;
        int fmt = sTexParam >> 26 & 7;
        g3p_polygon(v, nv, attr, sTexParam, sTexPal, (alpha > 0 && alpha < 31) || fmt == 1 || fmt == 6,
                    sFlushParam >> 1 & 1);
        sNumPolys++;
        sNumVerts += nv;
        return;
    }
#endif
    G3Vertex *cv;
    int clipped;
    sFarHidden = 0;
    int n = clip_polygon(v, nv, &cv, attr >> 12 & 1, &clipped);
    sPolyOutcome = n == 0 ? (sFarHidden ? 3 : 2) : 0;
    debug_tex_poly(n ? "kept" : "clipped away", v, nv, n);
    sPolyNOut = n;
    sPolyClipped = clipped;
    { static int dbg = -1; if (dbg < 0) dbg = getenv("ACWW_GPU3D_DEBUGPOLY") ? atoi(getenv("ACWW_GPU3D_DEBUGPOLY")) : 0; if (dbg > 0) { dbg--; fprintf(stderr, "poly attr %08x n %d -> %d clipped %d:", attr, nv, n, clipped); for (int i = 0; i < nv; i++) fprintf(stderr, " (%d %d %d %d)", v[i]->pos[0], v[i]->pos[1], v[i]->pos[2], v[i]->pos[3]); fprintf(stderr, "\n"); } }
    if (clipped) g3_feature(F_CLIP);
    // a strip's polygon hidden by the far plane still leaves a polygon of the two vertices it shares with the
    // previous one (fitted to melonDS)
    int phantom = 0;
    if (n == 0 && sFarHidden && (sPolyType == 2 || sPolyType == 3) && sStripValid) {
        phantom = 1;
        n = 2;
        sPolyOutcome = 0;
        sPolyNOut = 2;
    }
    if (n == 0) { sStripValid = 0; return; }

    // vertex RAM: a strip's polygon shares the two vertices it has in common with the previous one, when neither
    // was clipped (or culled)
    int strip = sPolyType == 2 || sPolyType == 3;
    int shared = !clipped && strip && sStripValid ? 2 : 0;
    int need = phantom ? 0 : n - shared;
    if (sNumPolys >= MAX_POLYGONS || sNumVerts + need > MAX_VERTICES) {
        sRamOverflow = 1;
        g3_feature(F_RAM_OVERFLOW);
        sPolyOutcome = 5;
        sStripValid = 0;
        return;
    }
    G3Polygon *poly = &sPolyRam[sBank][sNumPolys];
    memset(poly, 0, offsetof(G3Polygon, orig));  // (orig, the diagnostics' copy, is read only up to norig)
    poly->index = sNumPolys;
    G3Vertex *ram = sVertexRam[sBank];
    if (phantom) {  // (the two in the order the polygon has them: odd triangles newer first)
        int rev = sPolyType == 2 && (sStripPolys & 1);
        poly->v[0] = sStripRam[rev ? 1 : 0];
        poly->v[1] = sStripRam[rev ? 0 : 1];
        sStripValid = 0;
    } else if (!clipped) {
        // the polygon's vertices by their place in the strip (or the triangle / quad)
        static const u8 kOrder[5][4] = {{0, 1, 2}, {0, 1, 2, 3}, {0, 1, 2}, {0, 1, 3, 2}, {1, 0, 2}};
        const u8 *ord = kOrder[sPolyType == 2 && (sStripPolys & 1) ? 4 : sPolyType];
        G3Vertex *at[4];
        for (int k = 0; k < nv; k++) {
            if (k < shared) at[k] = sStripRam[k];
            else { at[k] = &ram[sNumVerts++]; *at[k] = *sTemp[k]; }
        }
        for (int i = 0; i < n; i++) poly->v[i] = at[ord[i]];
        if (strip) {
            sStripRam[0] = at[nv - 2];
            sStripRam[1] = at[nv - 1];
            sStripValid = 1;
        }
    } else {
        for (int i = 0; i < n; i++) {
            G3Vertex *x = &ram[sNumVerts++];
            *x = cv[i];
            poly->v[i] = x;
        }
        sStripValid = 0;
    }
    poly->nv = n;
    poly->attr = attr;
    poly->texparam = sTexParam;
    poly->texpal = sTexPal;
    poly->facing = (u8)facing;
    u32 alpha = attr >> 16 & 31;
    int fmt = sTexParam >> 26 & 7;
    poly->translucent = (alpha > 0 && alpha < 31) || fmt == 1 || fmt == 6;
    int mode = attr >> 4 & 3;
    if (mode == 3) {
        if ((attr >> 24 & 63) == 0) poly->shadowMask = 1, g3_feature(F_SHADOW_MASK);
        else poly->shadow = 1, g3_feature(F_SHADOW);
    }
    g3_feature(F_POLYMODE + mode);
    if (sTexParam >> 26 & 7) g3_feature(F_TEXFMT + fmt);
    if (poly->translucent) g3_feature(F_TRANSLUCENT);
    if (alpha == 0) g3_feature(F_WIREFRAME);
    if (attr & (1u << 14)) g3_feature(F_DEPTH_EQUAL);
    if (attr & (1u << 15)) g3_feature(F_FOG_POLY);
    if (attr & (1u << 11)) g3_feature(F_TRANS_DEPTH_UPDATE);
    if (fmt) {
        if (sTexParam & (3u << 16)) g3_feature(F_TEX_REPEAT);
        if (sTexParam & (3u << 18)) g3_feature(F_TEX_FLIP);
        if ((sTexParam & (3u << 16)) != (3u << 16)) g3_feature(F_TEX_CLAMP);
        if (sTexParam & (1u << 29)) g3_feature(F_COLOR0_TRANSPARENT);
    }

    // screen positions, depth, the top and bottom
    // depth: W-buffering or Z-buffering as the last SWAP_BUFFERS chose (its parameter applies to the polygons that
    // follow it); W for the rasteriser's perspective correction is normalised to 16 bits (in steps of 4 bits, the
    // same shift for every vertex of the polygon)
    poly->wbuffer = (u8)(sFlushParam >> 1 & 1);
    s32 ytop = 192, ybot = 0, xtop = 0, xbot = 0;
    int vtop = 0, vbot = 0;
    int wsize = 0;
    for (int i = 0; i < n; i++) {
        G3Vertex *x = poly->v[i];
        screen_position(x);
        u32 w = (u32)x->pos[3];
        while ((w >> wsize) && wsize < 32) wsize += 4;
        s64 z;
        if (poly->wbuffer) z = 0;  // (below: the normalised W)
        else if (x->pos[3]) z = ((((s64)x->pos[2] << 14) / x->pos[3]) + 0x3fff) << 9;
        else z = 0;
        if (z < 0) z = 0;
        if (z > 0xffffff) z = 0xffffff;
        poly->z[i] = (s32)z;
        if (x->y < ytop) { ytop = x->y; xtop = x->x; vtop = i; }
        if (x->y > ybot) { ybot = x->y; xbot = x->x; vbot = i; }
    }
    for (int i = 0; i < n; i++) {
        u32 w = (u32)poly->v[i]->pos[3];
        poly->w[i] = (s32)(wsize < 16 ? w << (16 - wsize) : w >> (wsize - 16));
        if (poly->wbuffer) {  // the depth is W with the precision the normalisation left
            s64 z = wsize > 16 ? (s64)(w >> (wsize - 16)) << (wsize - 16) : (s64)w;
            poly->z[i] = (s32)(z > 0xffffff ? 0xffffff : z);
        }
    }
    poly->norig = gG3Diagnostics ? nv : 0;  // (the vertices before clipping: for the comparisons' reports only)
    if (gG3Diagnostics)
        for (int i = 0; i < nv; i++) poly->orig[i] = *v[i];
    poly->vtop = vtop;
    poly->vbottom = vbot;
    poly->ytop = ytop;
    poly->ybottom = ybot;
    poly->xtop = xtop;
    poly->xbottom = xbot;
    poly->sortKey = (u32)(ybot << 8 | ytop) | (poly->translucent ? 0x10000u : 0);
    // 1-dot polygons farther than DISP_1DOT_DEPTH are hidden unless attr bit 13
    if (!(attr & (1u << 13))) {
        int dot1 = 1;
        for (int i = 1; i < n; i++)
            if (poly->v[i]->x != poly->v[0]->x || poly->v[i]->y != poly->v[0]->y) { dot1 = 0; break; }
        if (dot1) {
            s32 wmin = 0x7fffffff;
            for (int i = 0; i < n; i++) if (poly->w[i] < wmin) wmin = poly->w[i];
            if (wmin > ((s32)sDot1Depth << 9) / 0x200 * 0x200 && wmin > (s32)sDot1Depth) {
                g3_feature(F_ONEDOT_HIDE);
                sPolyOutcome = 4;
                sNumVerts -= need;
                sStripValid = 0;
                return;
            }
        }
    }
    sNumPolys++;
    if (!poly->translucent) sNumOpaque++;
}

static void submit_vertex(void) {
    if (!sInBegin) return;
    HOST_PROF(PROF_VERTEX);
    update_clip();
    G3Vertex *v = sTemp[sVtxInPoly];
    s64 x = sVtx[0], y = sVtx[1], z = sVtx[2];
    for (int i = 0; i < 4; i++)
        v->pos[i] = (s32)((x * sClip.m[i] + y * sClip.m[4 + i] + z * sClip.m[8 + i] + 0x1000 * (s64)sClip.m[12 + i]) >> 12);
#ifdef N3DS_DIRECT
    if (sSeg < 0) sSeg = g3p_matrix(sClip.m);
    v->obj[0] = (s32)x, v->obj[1] = (s32)y, v->obj[2] = (s32)z, v->seg = (u16)sSeg;
    g3p_vertex(sSeg, v->obj);
#endif
    for (int i = 0; i < 3; i++) v->color[i] = sVtxColor[i] ? sVtxColor[i] << 4 | 0xf : 0;
    v->dbgLight = sCurLightRec;
    if ((sTexParam >> 30) == 3) {  // texture coordinates from the vertex
        for (int i = 0; i < 2; i++)
            v->tex[i] = (s16)(((x * sTex.m[i] + y * sTex.m[4 + i] + z * sTex.m[8 + i]) >> 24) + sTexRaw[i]);
    } else {
        v->tex[0] = sTexCoord[0];
        v->tex[1] = sTexCoord[1];
    }
    v->clipped = 0;
    v->farAxis = 0;
    v->dbgClips = 0;
    v->screenDone = 0;
    sVtxInPoly++;
    sVtxCount++;
    switch (sPolyType) {
    case 0:
        if (sVtxInPoly == 3) { submit_polygon(); sVtxInPoly = 0; }
        break;
    case 1:
        if (sVtxInPoly == 4) { submit_polygon(); sVtxInPoly = 0; }
        break;
    case 2:
        if (sVtxInPoly == 3) {
            submit_polygon();
            sStripPolys++;
            G3Vertex *t = sTemp[0];  // (the next polygon shares the last two: turned, not copied)
            sTemp[0] = sTemp[1];
            sTemp[1] = sTemp[2];
            sTemp[2] = t;
            sVtxInPoly = 2;
        }
        break;
    case 3:
        if (sVtxInPoly == 4) {
            submit_polygon();
            sStripPolys++;
            G3Vertex *t0 = sTemp[0], *t1 = sTemp[1];
            sTemp[0] = sTemp[2];
            sTemp[1] = sTemp[3];
            sTemp[2] = t0;
            sTemp[3] = t1;
            sVtxInPoly = 2;
        }
        break;
    }
}

// ---- the tests (GBATEK "DS 3D Test Functions")
static void pos_test(void) {
    update_clip();
    s64 x = sVtx[0], y = sVtx[1], z = sVtx[2];
    for (int i = 0; i < 4; i++)
        sPosResult[i] = (s32)((x * sClip.m[i] + y * sClip.m[4 + i] + z * sClip.m[8 + i] + 0x1000 * (s64)sClip.m[12 + i]) >> 12);
}
static void vec_test(u32 p) {
    s32 d[3] = {sx10(p), sx10(p >> 10), sx10(p >> 20)};
    for (int i = 0; i < 3; i++) {
        s32 r = (s32)(((s64)d[0] * sVec.m[i] + (s64)d[1] * sVec.m[4 + i] + (s64)d[2] * sVec.m[8 + i]) >> 12);
        sVecResult[i] = (s16)(r << 3) >> 3;  // 1.3.12, sign from bit 12... (4 integer bits kept)
    }
}
static void box_test(const u32 *p) {
    update_clip();
    s16 bx = (s16)p[0], by = (s16)(p[0] >> 16), bz = (s16)p[1];
    s16 bw = (s16)(p[1] >> 16), bh = (s16)p[2], bd = (s16)(p[2] >> 16);
    G3Vertex c[8];
    for (int i = 0; i < 8; i++) {
        s64 x = bx + (i & 1 ? bw : 0), y = by + (i & 2 ? bh : 0), z = bz + (i & 4 ? bd : 0);
        x = (s16)x; y = (s16)y; z = (s16)z;
        memset(&c[i], 0, sizeof c[i]);
        for (int k = 0; k < 4; k++)
            c[i].pos[k] = (s32)((x * sClip.m[k] + y * sClip.m[4 + k] + z * sClip.m[8 + k] + 0x1000 * (s64)sClip.m[12 + k]) >> 12);
    }
    // the six faces as polygons, clipped against the view volume: inside if anything is left
    static const u8 kFaces[6][4] = {{0, 1, 3, 2}, {4, 5, 7, 6}, {0, 1, 5, 4}, {2, 3, 7, 6}, {0, 2, 6, 4}, {1, 3, 7, 5}};
    int in = 0;
    for (int f = 0; f < 6 && !in; f++) {
        G3Vertex *face[4], *out;
        for (int k = 0; k < 4; k++) face[k] = &c[kFaces[f][k]];
        int clipped;
        if (clip_polygon(face, 4, &out, 1, &clipped) > 0) in = 1;
    }
    sGxStat = (sGxStat & ~2u) | (in ? 2u : 0);
}

// ---- command execution
static u32 sExecParams[32];
static int sExecCount, sExecCmd = -1;

static struct { u8 cmd; u32 p0; int mode, sp; } sHist[64];
static unsigned sHistPos;
void gpu3d_debug_history(void) {
    for (unsigned i = 48; i < 64; i++) {
        unsigned k = (sHistPos + i) & 63;
        fprintf(stderr, " %02x(%x)m%d,sp%d", sHist[k].cmd, sHist[k].p0, sHist[k].mode, sHist[k].sp);
    }
    fprintf(stderr, "\n");
}
static void execute(u8 cmd, const u32 *p) {
    {   // (diagnostics: ACWW_GPU3D_CMDLOG=<frame>: every command executed in that frame, with the texture param)
        static long f = -2;
        if (f == -2) f = getenv("ACWW_GPU3D_CMDLOG") ? atol(getenv("ACWW_GPU3D_CMDLOG")) : -1;
        if (f >= 0 && sFrame == (u64)f) fprintf(stderr, "cmdlog %02x %08x tex %08x vin %d\n", cmd, p[0], sTexParam, sVtxInPoly);
    }
    sHist[sHistPos & 63].cmd = cmd; sHist[sHistPos & 63].p0 = p[0]; sHist[sHistPos & 63].mode = sMtxMode; sHist[sHistPos++ & 63].sp = sPosSP;
    g3_feature(F_CMD + (cmd & 0x7f));
    Mtx a;
    switch (cmd) {
    case 0x10: sMtxMode = p[0] & 3; break;
    case 0x11: mtx_push(); break;
    case 0x12: mtx_pop(p[0]); break;
    case 0x13: mtx_store(p[0]); break;
    case 0x14: mtx_restore(p[0]); break;
    case 0x15: mtx_identity(&a); apply_load(&a); break;
    case 0x16: for (int i = 0; i < 16; i++) a.m[i] = (s32)p[i]; apply_load(&a); break;
    case 0x17:
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 3; c++) a.m[r * 4 + c] = (s32)p[r * 3 + c];
            a.m[r * 4 + 3] = r == 3 ? 0x1000 : 0;
        }
        apply_load(&a);
        break;
    case 0x18: for (int i = 0; i < 16; i++) a.m[i] = (s32)p[i]; apply_mult(&a); break;
    case 0x19:
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 3; c++) a.m[r * 4 + c] = (s32)p[r * 3 + c];
            a.m[r * 4 + 3] = r == 3 ? 0x1000 : 0;
        }
        apply_mult(&a);
        break;
    case 0x1a:
        mtx_identity(&a);
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++) a.m[r * 4 + c] = (s32)p[r * 3 + c];
        apply_mult(&a);
        break;
    case 0x1b: {
        s32 s[3] = {(s32)p[0], (s32)p[1], (s32)p[2]};
        if (sMtxMode == 3) mtx_scale(&sTex, s);
        else if (sMtxMode == 0) { mtx_scale(&sProj, s); sClipDirty = 1; }
        else { mtx_scale(&sPos, s); sClipDirty = 1; }  // (mode 2: the position matrix only)
        break;
    }
    case 0x1c: {
        s32 t[3] = {(s32)p[0], (s32)p[1], (s32)p[2]};
        switch (sMtxMode) {
        case 0: mtx_trans(&sProj, t); sClipDirty = 1; break;
        case 1: mtx_trans(&sPos, t); sClipDirty = 1; break;
        case 2: mtx_trans(&sPos, t); mtx_trans(&sVec, t); sClipDirty = 1; break;
        default: mtx_trans(&sTex, t); break;
        }
        break;
    }
    case 0x20:
        sVtxColor[0] = p[0] & 31;
        sVtxColor[1] = p[0] >> 5 & 31;
        sVtxColor[2] = p[0] >> 10 & 31;
        sCurLightRec = -1;
        break;
    case 0x21:
        sNormal[0] = sx10(p[0]);
        sNormal[1] = sx10(p[0] >> 10);
        sNormal[2] = sx10(p[0] >> 20);
        if ((sTexParam >> 30) == 2) {  // texture coordinates from the normal (its 9 fractional bits * the matrix >> 21)
            for (int i = 0; i < 2; i++)
                sTexCoord[i] = (s16)((((s64)sNormal[0] * sTex.m[i] + (s64)sNormal[1] * sTex.m[4 + i] +
                                       (s64)sNormal[2] * sTex.m[8 + i]) >> 24) + sTexRaw[i]);
        }
        {
            HOST_PROF(PROF_LIGHT);
            lighting();
        }
        break;
    case 0x22:
        sTexRaw[0] = (s16)p[0];
        sTexRaw[1] = (s16)(p[0] >> 16);
        if ((sTexParam >> 30) == 1) {  // transformed by the texture matrix
            for (int i = 0; i < 2; i++)
                sTexCoord[i] = (s16)(((s64)sTexRaw[0] * sTex.m[i] + (s64)sTexRaw[1] * sTex.m[4 + i] + sTex.m[8 + i] +
                                      sTex.m[12 + i]) >> 12);
        } else {
            sTexCoord[0] = sTexRaw[0];
            sTexCoord[1] = sTexRaw[1];
        }
        break;
    case 0x23:
        sVtx[0] = (s16)p[0];
        sVtx[1] = (s16)(p[0] >> 16);
        sVtx[2] = (s16)p[1];
        submit_vertex();
        break;
    case 0x24:
        sVtx[0] = (s16)((p[0] & 0x3ff) << 6);
        sVtx[1] = (s16)((p[0] >> 10 & 0x3ff) << 6);
        sVtx[2] = (s16)((p[0] >> 20 & 0x3ff) << 6);
        submit_vertex();
        break;
    case 0x25: sVtx[0] = (s16)p[0]; sVtx[1] = (s16)(p[0] >> 16); submit_vertex(); break;
    case 0x26: sVtx[0] = (s16)p[0]; sVtx[2] = (s16)(p[0] >> 16); submit_vertex(); break;
    case 0x27: sVtx[1] = (s16)p[0]; sVtx[2] = (s16)(p[0] >> 16); submit_vertex(); break;
    case 0x28:
        sVtx[0] += (s16)((p[0] & 0x3ff) << 6) >> 6;
        sVtx[1] += (s16)((p[0] >> 10 & 0x3ff) << 6) >> 6;
        sVtx[2] += (s16)((p[0] >> 20 & 0x3ff) << 6) >> 6;
        submit_vertex();
        break;
    case 0x29: sPolyAttr = p[0]; break;
    case 0x2a: sTexParam = p[0]; g3_feature(F_TEXGEN + (p[0] >> 30)); break;
    case 0x2b: sTexPal = p[0] & 0x1fff; break;
    case 0x30:
        sDiffuse[0] = p[0] & 31;
        sDiffuse[1] = p[0] >> 5 & 31;
        sDiffuse[2] = p[0] >> 10 & 31;
        sAmbient[0] = p[0] >> 16 & 31;
        sAmbient[1] = p[0] >> 21 & 31;
        sAmbient[2] = p[0] >> 26 & 31;
        if (p[0] & 0x8000) {  // also the vertex colour
            for (int i = 0; i < 3; i++) sVtxColor[i] = sDiffuse[i];
            sCurLightRec = -1;
        }
        break;
    case 0x31:
        sSpecular[0] = p[0] & 31;
        sSpecular[1] = p[0] >> 5 & 31;
        sSpecular[2] = p[0] >> 10 & 31;
        sEmission[0] = p[0] >> 16 & 31;
        sEmission[1] = p[0] >> 21 & 31;
        sEmission[2] = p[0] >> 26 & 31;
        sShininessOn = p[0] >> 15 & 1;
        break;
    case 0x32: light_vector(p[0]); break;
    case 0x33: {
        int l = p[0] >> 30;
        sLightColor[l][0] = p[0] & 31;
        sLightColor[l][1] = p[0] >> 5 & 31;
        sLightColor[l][2] = p[0] >> 10 & 31;
        break;
    }
    case 0x34:
        for (int i = 0; i < 32; i++)
            for (int k = 0; k < 4; k++) sShininess[i * 4 + k] = (u8)(p[i] >> (8 * k));
        break;
    case 0x40:
        sPolyType = p[0] & 3;
        sCurPolyAttr = sPolyAttr;
        sInBegin = 1;
        sVtxInPoly = 0;
        sStripPolys = 0;
        sStripValid = 0;
        break;
    case 0x41: break;  // END_VTXS: nothing
    case 0x50:
        sFlushPending = 1;
        sFlushParam = p[0] & 3;
        break;
    case 0x60:
        { static u32 last = 1; if (p[0] != last && getenv("ACWW_GPU3D_VPLOG")) fprintf(stderr, "viewport %08x frame %llu\n", p[0], (unsigned long long)sFrame); last = p[0]; }
        sViewport[0] = p[0] & 0xff;
        sViewport[1] = p[0] >> 8 & 0xff;
        sViewport[2] = p[0] >> 16 & 0xff;
        sViewport[3] = (191 - (p[0] >> 24 & 0xff)) & 0xff;
        sViewport[4] = (sViewport[2] - sViewport[0] + 1) & 0x1ff;
        sViewport[5] = ((p[0] >> 24 & 0xff) - sViewport[1] + 1) & 0xff;
#ifdef N3DS_DIRECT
        g3p_viewport(sViewport);
#endif
        break;
    case 0x70: g3_feature(F_BOX_TEST); box_test(p); break;
    case 0x71:
        g3_feature(F_POS_TEST);
        sVtx[0] = (s16)p[0];
        sVtx[1] = (s16)(p[0] >> 16);
        sVtx[2] = (s16)p[1];
        pos_test();
        break;
    case 0x72: g3_feature(F_VEC_TEST); vec_test(p[0]); break;
    default: break;
    }
}

static void exec_entry(Entry e) {
    if (sExecCmd < 0) {
        sExecCmd = e.cmd;
        sExecCount = 0;
    }
    int need = kParams[(u8)sExecCmd];
    if (need) sExecParams[sExecCount++] = e.param;
    if (sExecCount >= need) {
        u8 c = (u8)sExecCmd;
        sExecCmd = -1;
#ifdef N3DS_DIRECT  // (no busy time: GXSTAT never says busy, the game doesn't wait)
        execute(c, sExecParams);
        return;
#endif
        timing_before(c);
        execute(c, sExecParams);
        timing_after(c);
    } else {
        if (sExecCmd == 0x23 && sExecCount == 1) vertex_entry_first();
        else add_cycles(1);  // (a parameter)
    }
}

static void drain(void) {
    while (sGeomOn && !sFlushPending && sPipeN) exec_entry(fifo_get());
}

// ---- the swap: the polygon list goes to the rasteriser with the rendering registers
static int cmp_poly(const void *a, const void *b) {
    const G3Polygon *p = *(G3Polygon *const *)a, *q = *(G3Polygon *const *)b;
    if (p->sortKey != q->sortKey) return p->sortKey < q->sortKey ? -1 : 1;
    return p->index - q->index;  // (stable)
}
static void do_swap(void) {
    G3RenderState *r = &gG3Render;
    r->npoly = sNumPolys;
    r->wbuffer = sFlushParam >> 1 & 1;
    r->manualSort = sFlushParam & 1;
    if (r->wbuffer) g3_feature(F_WBUFFER);
    else if (r->npoly) g3_feature(F_ZBUFFER_POLYS);
    if (r->manualSort) g3_feature(F_MANUAL_SORT);
#ifdef N3DS_DIRECT
    r->list = g3p_swap();
    r->npoly = 0;
    sNumPolys = 0;
    sSeg = -1;  // (the new list has no matrices yet)
#endif
    for (int i = 0; i < sNumPolys; i++) {
        G3Polygon *p = &sPolyRam[sBank][i];
        if (r->manualSort && p->translucent) p->sortKey = 0x10000u;  // (translucent ones in submission order)
        r->poly[i] = p;
    }
    qsort(r->poly, (size_t)sNumPolys, sizeof r->poly[0], cmp_poly);
    r->dispcnt = sDispCnt;
    r->clearColor = sRegs[(0x350 - 0x320) / 2];
    r->clearAttrHi = sRegs[(0x352 - 0x320) / 2];
    r->clearDepth = sRegs[(0x354 - 0x320) / 2] & 0x7fff;
    r->clearOffset = sRegs[(0x356 - 0x320) / 2];
    r->alphaRef = sRegs[(0x340 - 0x320) / 2] & 31;
    memcpy(r->toon, &sRegs[(0x380 - 0x320) / 2], sizeof r->toon);
    memcpy(r->edge, &sRegs[(0x330 - 0x320) / 2], sizeof r->edge);
    r->fogColor = sRegs[(0x358 - 0x320) / 2] | (u32)sRegs[(0x35a - 0x320) / 2] << 16;
    r->fogOffset = sRegs[(0x35c - 0x320) / 2] & 0x7fff;
    for (int i = 0; i < 32; i++) r->fogTable[i] = (u8)(sRegs[(0x360 - 0x320) / 2 + i / 2] >> (8 * (i & 1))) & 0x7f;
    sBank ^= 1;
    sNumPolys = sNumVerts = sNumOpaque = 0;
    sStripValid = 0;
    sFlushPending = 0;
    gG3Swaps++;
}

// ---- the interface
void gpu3d_reset(void) {
    sGeomOn = sRenderOn = 0;
    sDispCnt = sGxStat = 0;
    memset(sRegs, 0, sizeof sRegs);
    sMtxMode = 0;
    mtx_identity(&sProj); mtx_identity(&sPos); mtx_identity(&sVec); mtx_identity(&sTex);
    sProjSP = sTexSP = sPosSP = 0;
    sClipDirty = 1;
    sPipeHead = sPipeN = sFifoHead = sFifoN = 0;
    sPushPopQueued = sTestQueued = 0;
    sFlushPending = 0;
    sPackLeft = sPackParamsLeft = 0;
    sExecCmd = -1;
    sNumPolys = sNumVerts = sNumOpaque = 0;
    memset(&gG3Render, 0, sizeof gG3Render);
    if (sFeaturesOn < 0) sFeaturesOn = getenv("ACWW_GPU3D_FEATURES") ? atoi(getenv("ACWW_GPU3D_FEATURES")) : 0;
}

void gpu3d_write8(uint32_t addr, uint8_t v) {
    if (addr >= 0x04000240 && addr < 0x04000247) gG3VramCnt[addr - 0x04000240] = v;
    else if (addr >= 0x04000248 && addr < 0x0400024a) gG3VramCnt[addr - 0x04000248 + 7] = v;
    else {
        u16 h = addr & 1 ? (u16)(v << 8) : v;  // (not used by the host: 8-bit writes are VRAMCNT's)
        (void)h;
    }
}
void gpu3d_write16(uint32_t addr, uint16_t v) {
    if (addr == 0x04000060)  // (bits 12-13: status, cleared by writing 1; the rest replaced)
        sDispCnt = (sDispCnt & 0x3000u & ~(u32)(v & 0x3000)) | (v & 0x4fff);
    else if (addr == 0x04000304) { sRenderOn = v >> 2 & 1; sGeomOn = v >> 3 & 1; drain(); }
    else if (addr >= 0x04000320 && addr < 0x040003c0) sRegs[(addr - 0x04000320) / 2] = v;
    else if (addr == 0x04000610) sDot1Depth = v & 0x7fff;
    else if (addr >= 0x04000400 && addr < 0x04000600) { /* (the host writes these 32-bit) */ }
}
void gpu3d_write32(uint32_t addr, uint32_t v) {
    if (addr >= 0x04000400 && addr < 0x04000600 && !sGeomOn) { g3_feature(F_GEOM_OFF_WRITE); return; }  // (dropped)
    if (addr >= 0x04000400 && addr < 0x04000440) gxfifo_write(v);
    else if (addr >= 0x04000440 && addr < 0x04000600) {
        u8 cmd = (u8)((addr - 0x04000400) >> 2);
        if (is_command(cmd)) fifo_put(cmd, v);
    } else if (addr == 0x04000600) {
        if (v & 0x8000) {  // acknowledge the stack error: the projection stack pointer is reset too
            sGxStat &= ~0x8000u;
            sProjSP = 0;
            sTexSP = 0;
        }
        sGxStat = (sGxStat & ~0xc0000000u) | (v & 0xc0000000u);
    } else {
        gpu3d_write16(addr, (u16)v);
        gpu3d_write16(addr + 2, (u16)(v >> 16));
        return;
    }
    drain();
}

void gpu3d_write32_n(uint32_t addr, const uint32_t *v, uint32_t n) {
    uint32_t i = 0;
#ifdef N3DS_DIRECT
    // the direct build: a block for the GX FIFO decoded where it lies, each command run with its parameters in place.
    // The FIFO only while a swap waits (the commands then wait in it) or when a packed word's parameters go on in the
    // next block: from there word by word as below.
    if (addr == 0x04000400 && sGeomOn && !sFlushPending && !sPackParamsLeft && !sPipeN && !sFifoN && sExecCmd < 0) {
        while (i < n && !sFlushPending) {
            u32 cmds = v[i], need = 0;
            for (u32 c = cmds; c; c >>= 8) need += kParams[c & 0xff];
            if (i + 1 + need > n) break;
            i++;
            for (int k = 0; cmds; k++, cmds >>= 8) {
                u8 c = cmds & 0xff;
                if (sFlushPending) {  // (SWAP_BUFFERS in this word: its other commands queue behind it)
                    sPackCmds = cmds, sPackLeft = 4 - k;
                    pack_advance();
                    break;
                }
                if (c) execute(c, &v[i]);  // (00: a NOP)
                i += kParams[c];
            }
        }
    }
#endif
    for (; i < n; i++) gpu3d_write32(addr, v[i]);
}

static u32 gxstat(void) {
    u32 s = sGxStat & (2u | 0x8000u | 0xc0000000u);
    if (sTestQueued) s |= 1;
    s |= (u32)(sPosSP & 31) << 8;
    s |= (u32)(sProjSP & 1) << 13;
    if (sPushPopQueued) s |= 1u << 14;
    s |= (u32)sFifoN << 16;
    if (sFifoN < 128) s |= 1u << 25;
    if (sFifoN == 0) s |= 1u << 26;
    if (sPipeN || pipelines_busy()) s |= 1u << 27;
    return s;
}

uint32_t gpu3d_read32(uint32_t addr) {
    drain();
    if (addr == 0x04000600) return gxstat();
    if (addr == 0x04000604) { g3_feature(F_READ_RAMCOUNT); return (u32)sNumPolys | (u32)sNumVerts << 16; }
    if (addr == 0x04000320) { g3_feature(F_READ_RDLINES); return 46; }
    if (addr >= 0x04000620 && addr < 0x04000630) { g3_feature(F_READ_POSRESULT); return (u32)sPosResult[(addr - 0x04000620) / 4]; }
    if (addr == 0x04000630) { g3_feature(F_READ_VECRESULT); return (u16)sVecResult[0] | (u32)(u16)sVecResult[1] << 16; }
    if (addr == 0x04000634) { g3_feature(F_READ_VECRESULT); return (u16)sVecResult[2]; }
    if (addr >= 0x04000640 && addr < 0x04000680) {
        g3_feature(F_READ_CLIPMTX);
        update_clip();
        return (u32)sClip.m[(addr - 0x04000640) / 4];
    }
    if (addr >= 0x04000680 && addr < 0x040006a4) {
        g3_feature(F_READ_VECMTX);
        int i = (addr - 0x04000680) / 4;
        return (u32)sVec.m[(i / 3) * 4 + i % 3];
    }
    return 0;
}

void gpu3d_frame(const uint8_t *const bank[9]) {
    gG3Banks = bank;
    sFrame++;
    sVtxPipe = sNrmPipe = sPolyPipe = sSlotCtr = 0;  // (a frame of time: the pipelines are done)
    sSlots = 1;
    if (!(sDispCnt & 1)) g3_feature(F_TEXTURE_OFF);
    if (sDispCnt & 2) g3_feature(F_TOON_HIGHLIGHT);
    if (sDispCnt & 4) g3_feature(F_ALPHA_TEST);
    if (sDispCnt & 8) g3_feature(F_ALPHA_BLEND);
    if (sDispCnt & 16) g3_feature(F_ANTIALIAS);
    if (sDispCnt & 32) g3_feature(F_EDGE_MARK);
    if (sDispCnt & 64) g3_feature(F_FOG_ALPHA_ONLY);
    if (sDispCnt & 128) g3_feature(F_FOG);
    if (sDispCnt & 0x4000) g3_feature(F_REAR_BITMAP);
    if (sFlushPending) {
        g3_render_wait();  // (the list drawn from and its polygon RAM)
        do_swap();
        drain();
    }
    if (sDispCnt & 1) {}
    if (sRaster) g3_render();
}

const uint32_t *gpu3d_lines(void) { return g3_frame_lines(); }

void gpu3d_state(Gpu3dState *s) {
    s->pipe = (u32)sPipeN;
    s->fifo = (u32)sFifoN;
    s->flush = (u32)sFlushPending;
    s->npolys = (u32)sNumPolys;
    s->nverts = (u32)sNumVerts;
    s->mtxmode = (u32)sMtxMode;
    s->possp = (u32)sPosSP;
    s->projsp = (u32)sProjSP;
    s->vp = sVtxPipe; s->np = sNrmPipe; s->pp = sPolyPipe; s->slots = (s32)sSlots; s->slotctr = sSlotCtr;
    memcpy(s->proj, sProj.m, sizeof s->proj);
    memcpy(s->pos, sPos.m, sizeof s->pos);
    memcpy(s->vec, sVec.m, sizeof s->vec);
    memcpy(s->tex, sTex.m, sizeof s->tex);
    memcpy(s->light, sLightDir, sizeof s->light);
    memcpy(s->lightColor, sLightColor, sizeof s->lightColor);
    memcpy(s->material[0], sDiffuse, 3);
    memcpy(s->material[1], sAmbient, 3);
    memcpy(s->material[2], sSpecular, 3);
    memcpy(s->material[3], sEmission, 3);
    memcpy(s->normal, sNormal, sizeof s->normal);
    memcpy(s->vtxColor, sVtxColor, 3);
}

int gpu3d_poly_count(void) { return gG3Render.npoly; }
// (diagnostics: one vertex of polygon i of the frame shown, as numbers for fitting: its clip coordinates, the
// viewport, the lighting inputs, and the polygon's vertices before clipping)
void gpu3d_debug_vertex(int i, int k, FILE *f) {
    if (i < 0 || i >= gG3Render.npoly) return;
    const G3Polygon *p = gG3Render.poly[i];
    const G3Vertex *v = p->v[k];
    fprintf(f, "clip %d %d %d %d vp %u %u %u %u", v->pos[0], v->pos[1], v->pos[2], v->pos[3], sViewport[0],
            sViewport[3], sViewport[4], sViewport[5]);
    G3Vertex t = *v;
    t.screenDone = 0;
    screen_position(&t);
    fprintf(f, " scr %d %d hi %d %d re %d %d", v->x, v->y, v->hx, v->hy, t.x, t.y);
    if (sLightRecs && v->dbgLight >= 0 && !v->clipped) {
        const LightRec *r = &sLightRecs[v->dbgLight];
        fprintf(f, " light %d %d %d %u %d", r->n[0], r->n[1], r->n[2], r->lights, r->table);
        for (int c = 0; c < 3; c++) fprintf(f, " %d %d %d %d", r->dif[c], r->amb[c], r->spe[c], r->emi[c]);
        for (int l = 0; l < 4; l++)
            fprintf(f, " %d %d %d %d %d %d", r->dir[l][0], r->dir[l][1], r->dir[l][2], r->lc[l][0], r->lc[l][1], r->lc[l][2]);
        const LightRec *a = &sLightRecs[(v->dbgLight - 1) & 0xffff], *b = &sLightRecs[(v->dbgLight + 1) & 0xffff];
        fprintf(f, " adj %d %d %d %d %d %d", a->n[0], a->n[1], a->n[2], b->n[0], b->n[1], b->n[2]);
        fprintf(f, " raw %d %d %d vec", r->raw[0], r->raw[1], r->raw[2]);
        for (int i = 0; i < 9; i++) fprintf(f, " %d", r->vec[i]);
        for (int l = 0; l < 4; l++) { fprintf(f, " lp %u", r->lparam[l]); for (int i = 0; i < 9; i++) fprintf(f, " %d", r->lvec[l][i]); }
    }
    if (v->clipped) {
        fprintf(f, " cl %d", v->dbgClips);
        for (int j = 0; j < 8; j++) fprintf(f, " %d", v->dbgClip[j]);
        fprintf(f, " orig %d", p->norig);
        for (int j = 0; j < p->norig; j++)
            fprintf(f, " %d %d %d %d %d %d %d %d %d", p->orig[j].pos[0], p->orig[j].pos[1], p->orig[j].pos[2],
                    p->orig[j].pos[3], p->orig[j].color[0], p->orig[j].color[1], p->orig[j].color[2], p->orig[j].tex[0],
                    p->orig[j].tex[1]);
    }
}
void gpu3d_debug_poly(int i) {
    if (i < 0 || i >= gG3Render.npoly) return;
    const G3Polygon *p = gG3Render.poly[i];
    fprintf(stderr, "    own polygon %d before clipping:", p->index);
    for (int k = 0; k < p->norig; k++)
        fprintf(stderr, " (%d %d %d %d c%d,%d,%d t%d,%d)", p->orig[k].pos[0], p->orig[k].pos[1], p->orig[k].pos[2],
                p->orig[k].pos[3], p->orig[k].color[0], p->orig[k].color[1], p->orig[k].color[2], p->orig[k].tex[0],
                p->orig[k].tex[1]);
    fprintf(stderr, "\n    after:");
    for (int k = 0; k < p->nv; k++)
        fprintf(stderr, " (%d %d %d %d)", p->v[k]->pos[0], p->v[k]->pos[1], p->v[k]->pos[2], p->v[k]->pos[3]);
    fprintf(stderr, "\n");
}
int gpu3d_poly_get(int i, Gpu3dVertexOut *v, uint32_t *attr, uint32_t *texparam, uint32_t *texpal, int *flags) {
    if (i < 0 || i >= gG3Render.npoly) return 0;
    const G3Polygon *p = gG3Render.poly[i];
    for (int k = 0; k < p->nv; k++) {
        for (int c = 0; c < 4; c++) v[k].clip[c] = p->v[k]->pos[c];
        v[k].x = p->v[k]->x;
        v[k].y = p->v[k]->y;
        v[k].z = p->z[k];
        v[k].w = p->w[k];
        v[k].r = p->v[k]->color[0];
        v[k].g = p->v[k]->color[1];
        v[k].b = p->v[k]->color[2];
        v[k].s = p->v[k]->tex[0];
        v[k].t = p->v[k]->tex[1];
        v[k].clipped = p->v[k]->clipped;
    }
    *attr = p->attr;
    *texparam = p->texparam;
    *texpal = p->texpal;
    *flags = p->facing | p->translucent << 1 | p->shadowMask << 2 | p->shadow << 3;
    return p->nv;
}

void gpu3d_print_features(void) {
    static const char *const kNames[] = {
        [F_LIGHT] = "lighting", [F_SHININESS_TABLE] = "shininess table", [F_CLIP] = "clipped polygons",
        [F_FARCLIP_HIDE] = "far-plane polygons hidden", [F_CULLED] = "culled polygons",
        [F_ONEDOT_HIDE] = "1-dot polygons hidden", [F_TRANSLUCENT] = "translucent polygons",
        [F_WIREFRAME] = "wireframe (alpha 0)", [F_DEPTH_EQUAL] = "depth test equal", [F_FOG_POLY] = "fog per polygon",
        [F_WBUFFER] = "W-buffering", [F_ZBUFFER_POLYS] = "Z-buffering with polygons", [F_MANUAL_SORT] = "manual translucent sort", [F_STACK_ERROR] = "matrix stack error",
        [F_RAM_OVERFLOW] = "polygon/vertex RAM overflow", [F_FIFO_QUEUED] = "commands queued behind a swap",
        [F_SWAP_QUEUED] = "a swap queued behind a swap", [F_FIFO_FULL] = "command FIFO full (writes dropped)", [F_TEX_REPEAT] = "texture repeat", [F_TEX_FLIP] = "texture flip",
        [F_TEX_CLAMP] = "texture clamp", [F_COLOR0_TRANSPARENT] = "palette colour 0 transparent",
        [F_TRANS_DEPTH_UPDATE] = "translucent depth update", [F_LINE_POLY] = "line polygons",
        [F_TEXTURE_OFF] = "texture mapping off", [F_TOON_HIGHLIGHT] = "highlight shading (DISP3DCNT bit 1)",
        [F_ALPHA_TEST] = "alpha test", [F_ALPHA_BLEND] = "alpha blending", [F_ANTIALIAS] = "anti-aliasing",
        [F_EDGE_MARK] = "edge marking", [F_FOG_ALPHA_ONLY] = "fog alpha only", [F_FOG] = "fog", [F_REAR_BITMAP] = "rear-plane bitmap",
        [F_BOX_TEST] = "box test", [F_POS_TEST] = "position test", [F_VEC_TEST] = "vector test",
        [F_READ_CLIPMTX] = "clip matrix read", [F_READ_VECMTX] = "vector matrix read", [F_READ_RAMCOUNT] = "RAM count read",
        [F_READ_POSRESULT] = "position test result read", [F_READ_VECRESULT] = "vector test result read",
        [F_READ_RDLINES] = "RDLINES_COUNT read", [F_GEOM_OFF_WRITE] = "commands while the geometry engine is off",
        [F_SHADOW_MASK] = "shadow mask polygons", [F_SHADOW] = "shadow polygons",
    };
    static const char *const kModes[] = {"modulation", "decal", "toon/highlight", "shadow"};
    static const char *const kFmts[] = {"none", "A3I5", "4-colour", "16-colour", "256-colour", "4x4 compressed", "A5I3", "direct"};
    static const char *const kGen[] = {"none", "texcoord", "normal", "vertex"};
    fprintf(stderr, "gpu3d features (the first frame that used each):\n");
    for (int i = 0; i < F_COUNT; i++) {
        if (!sFeatureFrame[i]) continue;
        char b[64];
        const char *n;
        if (i < 0x80) { snprintf(b, sizeof b, "command %02x", i); n = b; }
        else if (i < F_TEXFMT) { snprintf(b, sizeof b, "polygon mode %s", kModes[i - F_POLYMODE]); n = b; }
        else if (i < F_TEXGEN) { snprintf(b, sizeof b, "texture format %s", kFmts[i - F_TEXFMT]); n = b; }
        else if (i < F_LIGHT) { snprintf(b, sizeof b, "texture coordinates from %s", kGen[i - F_TEXGEN]); n = b; }
        else n = kNames[i] ? kNames[i] : "?";
        fprintf(stderr, "  %-44s frame %llu\n", n, (unsigned long long)sFeatureFrame[i] - 1);
    }
}

// (diagnostics: the rendering registers latched for the frame shown, one line: DISP3DCNT, CLEAR_COLOR, CLEAR_DEPTH,
// ALPHA_TEST_REF, FOG_COLOR, FOG_OFFSET, the fog table)
void gpu3d_debug_regs(FILE *f) {
    const G3RenderState *r = &gG3Render;
    fprintf(f, "regs %x %x %x %x %x %x %x", r->dispcnt, r->clearColor | (u32)r->clearAttrHi << 16, r->clearDepth, r->alphaRef,
            r->fogColor, r->fogOffset, r->wbuffer);
    for (int i = 0; i < 32; i++) fprintf(f, " %d", r->fogTable[i]);
    fprintf(f, "\n");
}

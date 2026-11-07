// Host build: the DS display (2D engines and 3D engine) rendered by melonDS (GPL-3.0; docs/host.md). A melonDS NDS
// object is created with both of its CPUs halted for good: the game runs natively, and this file feeds the GPU what
// the game writes - display and 3D registers (3D commands are executed at once, so the matrix and test results the
// game reads back are current), VRAM banks, palettes and OAM (the host objects: vram.c, include/host_mem.h) - and
// has it render one frame per VBlank.
// Built against a melonDS core patched for this (tools/host/melonds.patch: GPU3D::HostDrain and friends).

#include "NDS.h"
#include "GPU.h"
#include "GPU3D.h"
#include "GPU3D_Soft.h"
#include "gpu3d/gpu3d.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace melonDS;

extern "C" {
u8 *host_vram_bank(int i);
u32 host_vram_bank_size(int i);
void host_vram_flush(void);
extern unsigned char gHostPltt[0x800], gHostOam[0x800];  // include/host_mem.h
}

static NDS *sNds;

extern "C" void melon_init(void) {
    NDSArgs args{};
    args.Firmware = Firmware(0);
    args.JIT = std::nullopt;
    sNds = new NDS(std::move(args));
    sNds->Reset();
    sNds->Start();
    sNds->ARM9.Halt(1);
    sNds->ARM7.Halt(1);
}

extern "C" void melon_write8(u32 addr, u8 v) { sNds->ARM9IOWrite8(addr, v); }
extern "C" void melon_write16(u32 addr, u16 v) {
    if (getenv("ACWW_LOGDISP") && (addr == 0x04000304 || addr == 0x04000000 || addr == 0x04000002 || addr == 0x04001000))
        fprintf(stderr, "melon: %08x = %04x\n", addr, v);
    sNds->ARM9IOWrite16(addr, v);
}
static void gx_fifo_irq(void);
static u32 sGxFifo, sGxPort;
static FILE *sPipeLog = (FILE *)-1;
static void melon_pipelog_line(const char *what) {
    if (sPipeLog == (FILE *)-1) sPipeLog = getenv("ACWW_MELON_PIPELOG") ? fopen(getenv("ACWW_MELON_PIPELOG"), "w") : NULL;
    if (!sPipeLog || !what) return;
    GPU3D &g = sNds->GPU.GPU3D;
    fprintf(sPipeLog, "%s %d %d %d %x %d %d %u %u\n", what, g.VertexPipeline, g.NormalPipeline, g.PolygonPipeline,
            g.VertexSlotsFree, g.VertexSlotCounter, g.CycleCount, g.CmdPIPE.Level(), g.FlushRequest);
}
extern "C" void melon_write32(u32 addr, u32 v) {
    if (addr == 0x04000400) sGxFifo++; else if (addr >= 0x04000440 && addr < 0x04000600) sGxPort++;
    sNds->ARM9IOWrite32(addr, v);
    melon_pipelog_line(NULL);
    FILE *plog = sPipeLog;
    if (plog == (FILE *)-1) plog = getenv("ACWW_MELON_PIPELOG") ? fopen(getenv("ACWW_MELON_PIPELOG"), "w") : NULL;
    if (plog && addr >= 0x04000400 && addr < 0x04000600) {  // (diagnostics: the pipeline state entry by entry)
        GPU3D &g = sNds->GPU.GPU3D;
        while (g.GeometryEnabled && !g.FlushRequest && !g.CmdPIPE.IsEmpty()) {
            auto e = g.CmdPIPE.Peek();
            g.ExecuteCommand();
            fprintf(plog, "%02x %08x %d %d %d %x %d %d\n", e.Command, e.Param, g.VertexPipeline, g.NormalPipeline,
                    g.PolygonPipeline, g.VertexSlotsFree, g.VertexSlotCounter, g.CycleCount);
        }
    }
    if (addr >= 0x04000400 && addr < 0x04000600) { sNds->GPU.GPU3D.HostDrain(); melon_pipelog_line("D"); }
    if (addr >= 0x04000400 && addr <= 0x04000600) gx_fifo_irq();
}
// the GX FIFO interrupt (IRQ 21) is level-triggered: GXSTAT bits 30-31 choose "less than half full" or "empty", and
// the FIFO is always empty here (commands run at once), so the line is up whenever one of them is selected
static int sGxFifoLevel;
static void gx_fifo_irq(void) {
    u32 mode = sNds->GPU.GPU3D.Read32(0x04000600) >> 30;
    sGxFifoLevel = mode == 1 || mode == 2;
}
extern "C" int melon_gx_fifo_irq_level(void) { return sGxFifoLevel; }

extern "C" u32 melon_read32(u32 addr) {
    sNds->GPU.GPU3D.HostDrain();
    return sNds->ARM9IORead32(addr);
}

// copy what changed in the host's VRAM banks into the GPU's (marking it dirty for the renderer's caches)
static void sync_vram(void) {
    GPU &g = sNds->GPU;
    host_vram_flush();  // (the CPU's windows into the banks)
    for (int b = 0; b < 9; b++) {
        const u8 *src = host_vram_bank(b);
        u8 *dst = g.VRAM[b];
        u32 n = host_vram_bank_size(b);
        for (u32 off = 0; off < n; off += VRAMDirtyGranularity) {
            if (memcmp(dst + off, src + off, VRAMDirtyGranularity)) {
                memcpy(dst + off, src + off, VRAMDirtyGranularity);
                g.VRAMDirty[b][off / VRAMDirtyGranularity] = true;
            }
        }
    }
    static_assert(sizeof g.Palette == sizeof gHostPltt && sizeof g.OAM == sizeof gHostOam, "palette, OAM");
    memcpy(g.Palette, gHostPltt, sizeof g.Palette);
    memcpy(g.OAM, gHostOam, sizeof g.OAM);
    g.HostMarkPaletteOAMDirty();
}

extern "C" void melon_frame(void) {
    sync_vram();
    sNds->GPU.GPU3D.HostDrain();
    melon_pipelog_line("FB");
    sNds->RunFrame();
    melon_pipelog_line("FA");
}

// the 3D picture the coming frame shows (melonDS renders the next one during the frame, at line 215): 192 lines of
// 256 pixels, 6-bit red, green, blue in bits 0-5, 8-13, 16-21, alpha 0-31 in bits 24-28, before BG0HOFS's scroll -
// the own 2D renderer's BG0 (render.c)
extern "C" void melon_3d_lines(uint32_t *out) {
    if (!sNds) { memset(out, 0, 256 * 192 * 4); return; }
    GPU3D &g = sNds->GPU.GPU3D;
    if (g.AbortFrame) { memset(out, 0, 256 * 192 * 4); return; }
    for (int line = 0; line < 192; line++) memcpy(out + line * 256, g.GetCurrentRenderer().GetLine(line), 256 * 4);
}

// melonDS's depth and attribute buffers of the frame it rendered last (the top layer), 256x192 each; 0 if its renderer
// isn't the software one. (Diagnostics only, an oracle for the own rasteriser: the buffers are private members,
// reached through an explicit instantiation, which may name them; the layout is SoftRenderer's: lines of 258
// pixels, the first pixel at 259.)
namespace {
typedef u32 SoftBuffer[258 * 194 * 2];
template <typename Tag, typename Tag::type M> struct Expose { friend typename Tag::type member(Tag) { return M; } };
struct DepthTag { typedef SoftBuffer SoftRenderer::*type; friend type member(DepthTag); };
struct AttrTag { typedef SoftBuffer SoftRenderer::*type; friend type member(AttrTag); };
template struct Expose<DepthTag, &SoftRenderer::DepthBuffer>;
template struct Expose<AttrTag, &SoftRenderer::AttrBuffer>;
}
extern "C" int melon_3d_buffers(u32 *depth, u32 *attr) {
    if (!sNds) return 0;
    SoftRenderer *r = dynamic_cast<SoftRenderer *>(&sNds->GPU.GPU3D.GetCurrentRenderer());
    if (!r) return 0;
    const u32 *d = r->*member(DepthTag()), *a = r->*member(AttrTag());
    for (int y = 0; y < 192; y++)
        for (int x = 0; x < 256; x++) {
            int i = 259 + y * 258 + x;
            if (depth) depth[y * 256 + x] = d[i];
            if (attr) attr[y * 256 + x] = a[i];
        }
    return 1;
}

// melonDS's polygon list of the frame it rendered last, for comparing the own geometry engine with it (render.c):
// the count, and polygon i's vertices (screen position, depth, colour, texture coordinates) and attributes
extern "C" int melon_3d_poly_count(void) { return (int)sNds->GPU.GPU3D.RenderNumPolygons; }
extern "C" int melon_3d_poly_get(int i, Gpu3dVertexOut *v, uint32_t *attr, uint32_t *texparam, uint32_t *texpal, int *flags) {
    GPU3D &g = sNds->GPU.GPU3D;
    if (i < 0 || (u32)i >= g.RenderNumPolygons) return 0;
    const melonDS::Polygon *p = g.RenderPolygonRAM[i];
    for (u32 k = 0; k < p->NumVertices && k < 10; k++) {
        const Vertex *x = p->Vertices[k];
        for (int c = 0; c < 4; c++) v[k].clip[c] = x->Position[c];
        v[k].x = x->FinalPosition[0];
        v[k].y = x->FinalPosition[1];
        v[k].z = p->FinalZ[k];
        v[k].w = p->FinalW[k];
        v[k].r = x->FinalColor[0];
        v[k].g = x->FinalColor[1];
        v[k].b = x->FinalColor[2];
        v[k].s = x->TexCoords[0];
        v[k].t = x->TexCoords[1];
        v[k].clipped = x->Clipped;
    }
    *attr = p->Attr;
    *texparam = p->TexParam;
    *texpal = p->TexPalette;
    *flags = p->FacingView | p->Translucent << 1 | p->IsShadowMask << 2 | p->IsShadow << 3;
    return (int)p->NumVertices;
}
// the geometry engine's state (render.c compares it with the own engine's after every command write)
extern "C" void melon_3d_state(Gpu3dState *s) {
    GPU3D &g = sNds->GPU.GPU3D;
    s->pipe = g.CmdPIPE.Level();
    s->fifo = g.CmdFIFO.Level();
    s->flush = g.FlushRequest;
    s->npolys = g.NumPolygons;
    s->nverts = g.NumVertices;
    s->mtxmode = g.MatrixMode;
    s->possp = (u32)g.PosMatrixStackPointer;
    s->projsp = (u32)g.ProjMatrixStackPointer;
    s->vp = g.VertexPipeline; s->np = g.NormalPipeline; s->pp = g.PolygonPipeline; s->slots = (s32)g.VertexSlotsFree; s->slotctr = g.VertexSlotCounter;
    memcpy(s->proj, g.ProjMatrix, sizeof s->proj);
    memcpy(s->pos, g.PosMatrix, sizeof s->pos);
    memcpy(s->vec, g.VecMatrix, sizeof s->vec);
    memcpy(s->tex, g.TexMatrix, sizeof s->tex);
    for (int l = 0; l < 4; l++)  // (melonDS keeps them negated)
        for (int i = 0; i < 3; i++) s->light[l][i] = (int16_t)-g.LightDirection[l][i];
    memcpy(s->lightColor, g.LightColor, sizeof s->lightColor);
    memcpy(s->material[0], g.MatDiffuse, 3);
    memcpy(s->material[1], g.MatAmbient, 3);
    memcpy(s->material[2], g.MatSpecular, 3);
    memcpy(s->material[3], g.MatEmission, 3);
    for (int i = 0; i < 3; i++) s->normal[i] = (int16_t)(g.Normal[i] * 8);  // (melonDS: 9 fractional bits)
    memcpy(s->vtxColor, g.VertexColor, 3);
}

// screen 0 top, 1 bottom: 256x192 pixels, 0x00RRGGBB
extern "C" const uint32_t *melon_screen(int screen) {
    return sNds->GPU.Framebuffer[sNds->GPU.FrontBuffer][screen].get();
}

static u64 fnv(const u8 *p, u32 n) { u64 h = 1469598103934665603ull; while (n--) h = (h ^ *p++) * 1099511628211ull; return h; }
extern "C" void melon_debug(void) {
    GPU &g = sNds->GPU;
    fprintf(stderr, "melon: bank hashes");
    for (int b = 0; b < 9; b++) fprintf(stderr, " %c:%016llx", 'A' + b, (unsigned long long)fnv(g.VRAM[b], host_vram_bank_size(b)));
    fprintf(stderr, " pal:%016llx", (unsigned long long)fnv(g.Palette, 2048));
    for (int fb = 0; fb < 2; fb++) for (int s = 0; s < 2; s++) {
        int nz = 0; const u32 *p = g.Framebuffer[fb][s].get();
        for (int i = 0; i < 256 * 192; i++) nz += (p[i] & 0xffffff) != 0;
        fprintf(stderr, " fb%d.%d:%d", fb, s, nz);
    }
    fprintf(stderr, " bld %08x %08x/%08x %08x bg %08x %08x", sNds->ARM9IORead32(0x04000050), sNds->ARM9IORead32(0x04000054),
            sNds->ARM9IORead32(0x04001050), sNds->ARM9IORead32(0x04001054), sNds->ARM9IORead32(0x04000008), sNds->ARM9IORead32(0x0400000c));
    fprintf(stderr, " front %d bright %04x/%04x\n", g.FrontBuffer, g.GPU2D_A.MasterBrightness, g.GPU2D_B.MasterBrightness);
    fprintf(stderr, "melon: gx fifo %u port %u gxstat %08x ", sGxFifo, sGxPort, sNds->ARM9IORead32(0x04000600));
    sGxFifo = sGxPort = 0;
    fprintf(stderr, "polys %u/%u dispcnt %08x/%08x vramcnt %02x %02x %02x %02x %02x %02x %02x %02x %02x powcnt %04x\n",
            g.GPU3D.NumPolygons, g.GPU3D.RenderNumPolygons, g.GPU2D_A.DispCnt, g.GPU2D_B.DispCnt, g.VRAMCNT[0],
            g.VRAMCNT[1], g.VRAMCNT[2], g.VRAMCNT[3], g.VRAMCNT[4], g.VRAMCNT[5], g.VRAMCNT[6], g.VRAMCNT[7],
            g.VRAMCNT[8], sNds->PowerControl9);
}

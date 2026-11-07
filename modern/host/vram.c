// Host build: DS VRAM banking without fixed mappings. The nine banks (A-I) are host memory (the renderer's view:
// host_vram_bank); the CPU sees VRAM through five windows (engine A's BG and OBJ, engine B's BG and OBJ, the LCDC
// window), host arrays that HostVram_CpuPtr resolves a DS address into. VRAMCNT decides which bank shows where in
// the windows, as on the DS: a window holds the contents of the banks mapped into it, kept in step with the banks -
// the windows are written back to their banks before anything reads the banks (host_vram_flush: the renderer at
// each frame, the debug dumps) and at every VRAMCNT change, which then fills the windows from the banks' new places
// (the parts with no bank read 0, as before, when the windows were remapped with mmap). So a pointer the game keeps
// across a VRAMCNT change sees what the DS would show at that address, and a copy that runs from one bank into the
// next in a window works as on the DS. Banks used for textures, texture palettes, extended palettes or the ARM7
// aren't visible to the CPU. Where two banks overlap in a window the later one (in A-I order) shows.

#include <stdio.h>
#include <string.h>

#include "host.h"
#include "host_mem.h"
#include "prof.h"

enum { BANK_TOTAL = 0xa4000, GRANULE = 0x4000 };
static const u32 kSize[9] = {0x20000, 0x20000, 0x20000, 0x20000, 0x10000, 0x4000, 0x4000, 0x8000, 0x4000};
static const u32 kLcdc[9] = {0x06800000, 0x06820000, 0x06840000, 0x06860000, 0x06880000, 0x06890000, 0x06894000,
                             0x06898000, 0x068a0000};
static u32 sOffset[9];
#if HOST_MEM_RUNTIME_ALIGN
static u8 (*sBanksBlock)[BANK_TOTAL];  // (allocated by host_vram_init: include/host_mem.h)
#define sBanks (*sBanksBlock)
#else
static u8 sBanks[BANK_TOTAL] __attribute__((aligned(0x20000)));
#endif
static u8 sCnt[9];

u8 *host_vram_bank(int i) { return sBanks + sOffset[i]; }
u32 host_vram_bank_size(int i) { return kSize[i]; }

// the windows: their DS address, size and alignment (the size, at most 128 KB); their memory (sWinMem)
static const struct { u32 base, size, align; } kWindows[] = {
    {0x06000000, 0x80000, 0x80000},  // engine A BG
    {0x06200000, 0x20000, 0x20000},  // engine B BG
    {0x06400000, 0x40000, 0x40000},  // engine A OBJ
    {0x06600000, 0x20000, 0x20000},  // engine B OBJ
    {0x06800000, 0xa4000, 0x20000},  // LCDC
};
#define NWIN (sizeof kWindows / sizeof kWindows[0])
#if HOST_MEM_RUNTIME_ALIGN
static u8 *sWinMem[NWIN];  // (allocated by host_vram_init)
#else
static u8 sWinBgA[0x80000] __attribute__((aligned(0x80000)));
static u8 sWinBgB[0x20000] __attribute__((aligned(0x20000)));
static u8 sWinObjA[0x40000] __attribute__((aligned(0x40000)));
static u8 sWinObjB[0x20000] __attribute__((aligned(0x20000)));
static u8 sWinLcdc[0xa4000] __attribute__((aligned(0x20000)));
static u8 *const sWinMem[NWIN] = {sWinBgA, sWinBgB, sWinObjA, sWinObjB, sWinLcdc};
#endif

// the windows in granules of 16 KB (every bank's size and place is a multiple): which bank shows there (-1: none)
// and at which offset in the bank
enum { NGRAN = (0x80000 + 0x20000 + 0x40000 + 0x20000 + 0xa4000) / GRANULE };
static struct { u8 *win; s32 bank; u32 off; } sGran[NGRAN];

// the CPU address of bank i under control value c, or 0
static u32 bank_address(int i, u8 c) {
    if (!(c & 0x80)) return 0;
    u32 mst = c & 7, ofs = c >> 3 & 3;
    switch (i) {
    case 0: case 1:
        if (mst == 0) return kLcdc[i];
        if (mst == 1) return 0x06000000 + 0x20000 * ofs;
        if (mst == 2) return 0x06400000 + 0x20000 * (ofs & 1);
        return 0;
    case 2: case 3:
        if (mst == 0) return kLcdc[i];
        if (mst == 1) return 0x06000000 + 0x20000 * ofs;
        if (mst == 4) return i == 2 ? 0x06200000 : 0x06600000;
        return 0;
    case 4:
        if (mst == 0) return kLcdc[i];
        if (mst == 1) return 0x06000000;
        if (mst == 2) return 0x06400000;
        return 0;
    case 5: case 6:
        if (mst == 0) return kLcdc[i];
        if (mst == 1) return 0x06000000 + 0x4000 * (ofs & 1) + 0x10000 * (ofs >> 1);
        if (mst == 2) return 0x06400000 + 0x4000 * (ofs & 1) + 0x10000 * (ofs >> 1);
        return 0;
    case 7:
        if (mst == 0) return kLcdc[i];
        if ((mst & 3) == 1) return 0x06200000;
        return 0;
    case 8:
        if (mst == 0) return kLcdc[i];
        if ((mst & 3) == 1) return 0x06208000;
        if ((mst & 3) == 2) return 0x06600000;
        return 0;
    }
    return 0;
}

// the granule index of CPU address a, or -1
static int granule(u32 a) {
    int g = 0;
    for (unsigned w = 0; w < NWIN; w++) {
        if (a >= kWindows[w].base && a - kWindows[w].base < kWindows[w].size) return g + (int)((a - kWindows[w].base) / GRANULE);
        g += (int)(kWindows[w].size / GRANULE);
    }
    return -1;
}

// the windows' contents back into the banks that show there
void host_vram_flush(void) {
    HOST_PROF(PROF_VRAM);
    for (int g = 0; g < NGRAN; g++)
        if (sGran[g].bank >= 0) memcpy(sBanks + sOffset[sGran[g].bank] + sGran[g].off, sGran[g].win, GRANULE);
}

// bumped at every remap: the banks the CPU can't see (textures, texture palettes...) change only then
static u32 sGeneration;
u32 host_vram_generation(void) { return sGeneration; }

// Only the granules whose bank changes: their old bank gets the window's contents, then they get the new one's (or
// zeros). A granule that keeps its bank would get back what it holds. (So a write the game makes where no bank
// shows stays until a bank shows there, not until the next VRAMCNT change; neither happens on the DS, where it is
// lost. Remapping everything moved megabytes at each change, several a frame when the game loads textures.)
static void remap(void) {
    HOST_PROF(PROF_VRAM);
    sGeneration++;
    static struct { s32 bank; u32 off; } now[NGRAN];
    static int first = 1;
    for (int g = 0; g < NGRAN; g++) now[g].bank = -1, now[g].off = 0;
    for (int i = 0; i < 9; i++) {
        u32 a = bank_address(i, sCnt[i]);
        if (!a) continue;
        for (u32 off = 0; off < kSize[i]; off += GRANULE) {
            int g = granule(a + off);
            if (g < 0) { fprintf(stderr, "vram: bank %c at %08x outside the windows\n", 'A' + i, a + off); continue; }
            now[g].bank = i;
            now[g].off = off;
        }
    }
    for (int g = 0; g < NGRAN; g++)
        if (sGran[g].bank >= 0 && (sGran[g].bank != now[g].bank || sGran[g].off != now[g].off))
            memcpy(sBanks + sOffset[sGran[g].bank] + sGran[g].off, sGran[g].win, GRANULE);
    for (int g = 0; g < NGRAN; g++) {
        if (!first && sGran[g].bank == now[g].bank && sGran[g].off == now[g].off) continue;
        sGran[g].bank = now[g].bank;
        sGran[g].off = now[g].off;
        if (now[g].bank >= 0) memcpy(sGran[g].win, sBanks + sOffset[now[g].bank] + now[g].off, GRANULE);
        else memset(sGran[g].win, 0, GRANULE);
    }
    first = 0;
}

// VRAMCNT_A..I: 0x04000240-0x04000246, 0x04000248-0x04000249 (0x04000247 is WRAMCNT)
void host_vram_control(u32 addr, u8 value) {
    int i = addr == 0x04000248 ? 7 : addr == 0x04000249 ? 8 : (int)(addr - 0x04000240);
    if (i < 0 || i > 8 || addr == 0x04000247) return;
    if (sCnt[i] == value) return;
    sCnt[i] = value;
    remap();
}

// The enhanced port's texture uploads (N3DS_DIRECT: GX_LoadTex, GX_LoadTexPltt): an LCDC address of a bank in the
// bank's own memory, where the renderers read it (the banks stay mapped to the texture slots, out of the CPU's
// windows); the renderers told that the textures changed (the generation)
void *host_vram_lcdc_direct(unsigned int a) {
    for (int i = 0; i < 9; i++)
        if (a >= kLcdc[i] && a - kLcdc[i] < kSize[i]) {
            sGeneration++;
            return sBanks + sOffset[i] + (a - kLcdc[i]);
        }
    return HostVram_CpuPtr(a);
}

// the CPU's view of VRAM address a. An address outside the windows (a mirror the DS has and the host doesn't) is
// reported once and gets a scratch area, as the anonymous pages there did before
static u8 sScratch[0x80000];
void *HostVram_CpuPtr(unsigned int a) {
    for (unsigned w = 0; w < NWIN; w++)
        if (a >= kWindows[w].base && a - kWindows[w].base < kWindows[w].size) return sWinMem[w] + (a - kWindows[w].base);
    static int reported;
    if (!reported++) fprintf(stderr, "vram: CPU address %08x outside the VRAM windows (frame %llu)\n", a, (unsigned long long)host_frame());
    return sScratch;
}

void host_vram_init(void) {
#if HOST_MEM_RUNTIME_ALIGN
    sBanksBlock = HostMem_AlignedBlock(sizeof sBanks, 0x20000);
    for (unsigned w = 0; w < NWIN; w++) sWinMem[w] = HostMem_AlignedBlock(kWindows[w].size, kWindows[w].align);
#endif
    u32 total = 0;
    for (int i = 0; i < 9; i++) { sOffset[i] = total; total += kSize[i]; }
    int g = 0;
    for (unsigned w = 0; w < NWIN; w++)
        for (u32 off = 0; off < kWindows[w].size; off += GRANULE, g++) sGran[g].win = sWinMem[w] + off, sGran[g].bank = -1;
    remap();
}

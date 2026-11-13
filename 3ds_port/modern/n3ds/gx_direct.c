// The enhanced port's texture upload, moved out of src/autoload_2/unk_021120a8.c (the game's flags; modern/n3ds/ is built by
// tools/configure_host.py for the 3DS port only).
#ifdef N3DS_PORT
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#include "uptr.h"
#include "host_mem.h"

extern u32 data_021fcbf4; // sTexLCDCBlk1 (VRAM addresses: LCDC)
extern u32 data_021fcc04; // sTexLCDCBlk2
extern u32 data_021fcc08; // sSzTexBlk1
void MIi_CpuCopy32(const void *src, void *dest, u32 size);            // MI_CpuCopy32

// GX_LoadTex
void N3ds_GxLoadTex(const void *pSrc, u32 dstSlotOffset, u32 szByte) {
    // the enhanced port (VRAM): straight into the banks' own memory (vram.c), which
    // GX_BeginLoadTex leaves mapped to the texture slots; at once, by the CPU
    if (data_021fcc04 && dstSlotOffset + szByte >= data_021fcc08) {
        if (dstSlotOffset >= data_021fcc08) {
            MIi_CpuCopy32(pSrc, host_vram_lcdc_direct(data_021fcc04 + dstSlotOffset - data_021fcc08, szByte), szByte);
        } else {
            u32 sz = data_021fcc08 - dstSlotOffset;
            MIi_CpuCopy32(pSrc, host_vram_lcdc_direct(data_021fcbf4 + dstSlotOffset, sz), sz);
            MIi_CpuCopy32((const u8 *)pSrc + sz, host_vram_lcdc_direct(data_021fcc04, szByte - sz), szByte - sz);
        }
    } else {
        MIi_CpuCopy32(pSrc, host_vram_lcdc_direct(data_021fcbf4 + dstSlotOffset, szByte), szByte);
    }
}
#endif

#include "host_labels.h"
// mwcc-flags: -nothumb -O4,p
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u32 data_021fcbf4, data_021fcc00, data_021fcc04, data_021fcc08;
typedef struct { u16 x, y, z; } T6;
extern const T6 data_02139f54[16];
// f56 / f58: the y / z members (interior labels)
#define data_02139f56 HOST_LABEL(const T6 (*)[], data_02139f56)
#define data_02139f58 HOST_LABEL(const T6 (*)[], data_02139f58)
s32 GX_ResetBankForTex(void);

// GX_BeginLoadTex
void GX_BeginLoadTex(void) {
#ifdef N3DS_DIRECT  // (the banks stay mapped: GX_LoadTex writes into their memory)
    u32 GXi_TexBanks(void);
    s32 i = (s32)GXi_TexBanks();
#else
    s32 i = GX_ResetBankForTex();
#endif
    data_021fcc00 = i;
    data_021fcbf4 = (u32)data_02139f54[i].x << 12;
    data_021fcc04 = (u32)data_02139f56[i].x << 12;
    data_021fcc08 = (u32)data_02139f58[i].x << 12;
}

// ---- file-scope objects (.rodata 0x02139f54-0x02139fb4)
const T6 data_02139f54[16] = {
    {0x0000, 0x0000, 0x0000}, {0x6800, 0x0000, 0x0000}, {0x6820, 0x0000, 0x0000}, {0x6800, 0x0000, 0x0000},
    {0x6840, 0x0000, 0x0000}, {0x6800, 0x6840, 0x0020}, {0x6820, 0x0000, 0x0000}, {0x6800, 0x0000, 0x0000},
    {0x6860, 0x0000, 0x0000}, {0x6800, 0x6860, 0x0020}, {0x6820, 0x6860, 0x0020}, {0x6800, 0x6860, 0x0040},
    {0x6840, 0x0000, 0x0000}, {0x6800, 0x6840, 0x0020}, {0x6820, 0x0000, 0x0000}, {0x6800, 0x0000, 0x0000},
};

// ---- labels inside this unit's variables (generated: tools/host_labels.py) ----
#include "host_labels.h"

HOST_LABEL_DEFINE(data_02139f56, (char *)&data_02139f54[0].y);
HOST_LABEL_DEFINE(data_02139f58, (char *)&data_02139f54[0].z);

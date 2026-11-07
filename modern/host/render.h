// Host build: the display interface (render.c). hw.c, main.c and frontend.c use only these; behind them the 2D
// engines are rendered by melonDS (melon.cpp) or by the project's own renderer (gpu2d/), the 3D engine by melonDS
// for now (docs/portability.md 5.1, 5.2).
#ifndef HOST_RENDER_H
#define HOST_RENDER_H

#include "host.h"

void render_init(void);
// the display registers the game writes (hw.c forward_display): engine A and B's 0x04000000-0x0400006f and
// 0x04001000-0x0400106f, POWCNT1, VRAMCNT, the 3D registers and command ports
void render_write8(u32 addr, u8 v);
void render_write16(u32 addr, u16 v);
// the same, words to one 3D command port (HostIo_Send32)
void render_write32_n(u32 addr, const u32 *v, u32 n);
void render_write32(u32 addr, u32 v);
u32 render_read32(u32 addr);       // the 3D engine's status and results (0x04000320, 0x04000600-0x040006a3)
void render_frame(void);           // at VBlank: the frame from the VRAM banks, palettes, OAM and registers
const uint32_t *render_screen(int screen);  // 0 top, 1 bottom: 256x192 pixels, 0x00RRGGBB (the top byte is ignored)
// 1: the screens are put together by the GPU (wgpu_display.h, which can show them): render_screen reads them back
int render_on_gpu(void);
// 1: the GPU shows the screens itself (wgpu_display.c presenting, the 3DS's pica_display.c): render_screen would
// read them back
int render_presenting(void);
int render_gx_fifo_irq_level(void);    // the GX FIFO interrupt line (IRQ 21)
void render_debug(void);           // ACWW_STATS diagnostics
void render_exit_report(void);     // ACWW_RENDER_COMPARE's summary, ACWW_GPU2D_FEATURES
// render.c / render_selftest.c internals: the comparison of the two renderers' last frame, the self-test
int render_compare(const char *kind, u64 label);
int render_compare_3d(const char *kind, u64 label);
void render_selftest(long scenes) __attribute__((noreturn));
void render_selftest_gpu2d(long scenes) __attribute__((noreturn));
int render_check_gpu2d(void);  // ACWW_GPU2D_CHECK's comparison of the last frame: 1 if it differs

#endif

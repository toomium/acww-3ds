// The project's own 3D engine of the DS (docs/portability.md 5.2): the geometry engine (command FIFO, matrix stacks,
// lighting, texture coordinate generation, polygon setup and clipping, the box / position / vector tests) and a
// software rasteriser (depth buffering, perspective-correct texturing in all 7 formats, toon and highlight shading,
// alpha blending and alpha test, fog, edge marking, anti-aliasing, the rear plane, translucent polygon IDs, shadow
// polygons). Plain C, no host or emulator dependencies: it is given the 3D registers and command ports as the game
// writes them, answers the synchronous reads (GXSTAT, RAM counts, test results, the clip and vector matrices) and at
// each frame renders the polygons of the last swap from the texture image and palette VRAM.
//
// Written from the hardware's documented behaviour (GBATEK, "DS 3D Video").
//
// Timing as the host runs it: the game's code takes no time, so a command is executed when it is written - except
// after SWAP_BUFFERS, when the engine waits for the frame's end (gpu3d_frame): commands written meanwhile wait in the
// command FIFO (4-entry pipe, 256-entry FIFO), and GXSTAT shows them (busy, FIFO level) until the frame ends.

#ifndef ACWW_GPU3D_H
#define ACWW_GPU3D_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void gpu3d_reset(void);
// register writes, addresses as on the DS: the GX FIFO and command ports (0x04000400-0x040005ff, 32-bit), GXSTAT
// (0x04000600, 32-bit), DISP3DCNT (0x04000060), the rendering registers 0x04000320-0x040003bf and 0x04000610
// (16-bit halves), POWCNT1 (0x04000304: the geometry and rendering engines' enables), VRAMCNT A-G (0x04000240-
// 0x04000246, 8-bit: the texture image and palette slots); other addresses are ignored
void gpu3d_write8(uint32_t addr, uint8_t value);
void gpu3d_write16(uint32_t addr, uint16_t value);
void gpu3d_write32(uint32_t addr, uint32_t value);
void gpu3d_write32_n(uint32_t addr, const uint32_t *value, uint32_t n);  // n words to one address
// the 3D engine's status and results: RDLINES_COUNT (0x04000320), GXSTAT, RAM_COUNT, the position and vector test
// results, CLIPMTX_RESULT and VECMTX_RESULT (0x04000600-0x040006a3); word addresses
uint32_t gpu3d_read32(uint32_t addr);

// the end of a frame (VBlank): the picture rendered at the previous call becomes the one shown (gpu3d_lines); a
// swap requested since is carried out (the polygon list handed to the rasteriser, which renders it now) and the
// commands that waited for it run. `bank`: VRAM banks A-I (the texture image slots are banks A-D, the palette
// slots E-G as VRAMCNT maps them)
void gpu3d_frame(const uint8_t *const bank[9]);
// the VRAM's mapping generation (vram.c host_vram_generation): while it stays the same, the texture image and
// palette slots (banks the CPU can't see) keep their contents, and the rasteriser doesn't gather them again
void gpu3d_vram_generation(uint32_t generation);
// 0: gpu3d_frame runs the geometry engine's frame but doesn't draw the picture (a frame the screens skip)
void gpu3d_set_raster(int on);
// The wide view (the 3DS's ACWW_WIDE=1, an experiment): polygons kept beyond the DS's screen, clipped at a picture of
// (256 + 2 * GPU3D_WIDE_X) x (192 + 2 * GPU3D_WIDE_Y) with the DS's in its middle; their screen positions (hx, hy)
// then not wrapped (negative in the margins). Only for a renderer that draws them there (gpu3d_pica.c's).
enum { GPU3D_WIDE_X = 72, GPU3D_WIDE_Y = 24 };
int gpu3d_wide_wanted(void);  // ACWW_WIDE set
void gpu3d_set_wide(int on);
int gpu3d_wide(void);
// the picture the frame shows: 192 lines of 256 pixels, red bits 0-5, green 8-13, blue 16-21, alpha bits 24-28
// (0-31; 0: no pixel), before BG0HOFS's scroll (the 2D renderer's BG0 input)
const uint32_t *gpu3d_lines(void);
// the polygons drawn by the GPU (WebGPU, gpu3d_wgpu.c) instead of the software rasteriser, with the device and queue
// given (a WGPUDevice and WGPUQueue) at `scale` times the DS's resolution; readback: gpu3d_lines has the picture
// (read back every frame, at scale 1), else only gpu3d_wgpu_picture. 0 if this build can't
int gpu3d_use_wgpu(void *device, void *queue, int scale, int readback);
// the polygons drawn by the 3DS's GPU (gpu3d_pica.c) into the rasteriser's buffers; 0 if this build can't
int gpu3d_use_pica(void);
// 1: the pictures drawn for the 3DS GPU's compositing (pica_display.c) when they can be, not read back; the last
// such picture (a C3D_Tex), NULL when the last one was read back into the frame buffer (gpu3d_lines)
void gpu3d_pica_direct(int on);
void *gpu3d_pica_picture(void);
// the picture the frame shows, on the GPU: a WGPUTextureView of an R32Uint texture of 256 x 192 times the scale, in
// gpu3d_lines' format
void *gpu3d_wgpu_picture(void);

// diagnostics
typedef struct Gpu3dPolyInfo {
    int index;          // in the polygon list of the frame shown
    uint32_t attr, texparam, texpal;
    int nverts;
    int x[10], y[10];   // screen coordinates
    int32_t z[10], w[10];
    int translucent;
} Gpu3dPolyInfo;
// the polygon that drew pixel (x, y) of the frame shown (the last one written there, opaque or translucent); 0 if
// none (the clear colour); only with gpu3d_set_diagnostics(1)
int gpu3d_pixel_polygon(int x, int y, Gpu3dPolyInfo *info);
void gpu3d_set_diagnostics(int on);
void gpu3d_print_features(void);  // the features the frames used (ACWW_GPU3D_FEATURES)

// the polygon list as set up for the rasteriser (for comparing the geometry engines): the list of the frame shown
typedef struct Gpu3dVertexOut {
    int32_t x, y;           // screen position (pixels)
    int32_t z, w;           // depth value (24-bit Z or W), W
    int32_t r, g, b;        // colour as given to the rasteriser
    int16_t s, t;           // texture coordinates (1/16 texels)
    int32_t clipped;        // made by clipping
    int32_t clip[4];        // (diagnostics: the clip coordinates x, y, z, w)
} Gpu3dVertexOut;
int gpu3d_poly_count(void);
// polygon i of the frame shown: its vertices (at most 10) and attributes; returns the vertex count
int gpu3d_poly_get(int i, Gpu3dVertexOut *v, uint32_t *attr, uint32_t *texparam, uint32_t *texpal, int *flags);

// the geometry engine's state, for comparing the engines command by command (render.c)
typedef struct Gpu3dState {
    uint32_t pipe, fifo, flush, npolys, nverts, mtxmode, possp, projsp;
    int32_t vp, np, pp, slots, slotctr;  // (the pipelines behind the busy bit)
    int32_t proj[16], pos[16], vec[16], tex[16];
    int16_t light[4][3];  // the light directions (9 fractional bits, in the vector matrix's space)
    uint8_t lightColor[4][3], material[4][3];  // (material: diffuse, ambient, specular, emission)
    int16_t normal[3];                         // the last normal (as given) and the vertex colour (5 bits)
    uint8_t vtxColor[3];
} Gpu3dState;
void gpu3d_state(Gpu3dState *s);
void gpu3d_debug_history(void);
void gpu3d_debug_poly(int i);
int gpu3d_debug_last_polygon(int *nout, int *clipped, unsigned *serial, int *type);

#ifdef __cplusplus
}
#endif

#endif

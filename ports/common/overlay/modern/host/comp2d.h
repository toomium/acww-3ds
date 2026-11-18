// Host build: the console ports' compositing of the screens on a GPU (comp2d.c), split from the GPU it runs on. The
// logic is comp2d.c's, shared: the 2D renderer's layers and lines (gpu2d.h Gpu2dLayers) put together as the DS does,
// the BGs drawn from VRAM, the wide view's margins, the anchored UI, the iris, the options page. What it draws with
// is the backend's, one per console: the 3DS's pica_display.c (citro3d) and the Switch's gl_display.c (OpenGL ES).
// The backend also has the frames, the GPU thread and the screens (pica_display.h's pdisp_* API, which render.c and
// the frontends use); comp2d.c's part of that API is marked below.
#ifndef HOST_COMP2D_H
#define HOST_COMP2D_H

#include <stdint.h>

struct Gpu2dLayers;

// ---- the core (comp2d.c), for the backend
// (pdisp_layers, pdisp_frame, pdisp_screen, pdisp_touch_map: pica_display.h, in comp2d.c)
int comp2d_init(void);     // the core's textures and buffers (after the backend's own set-up); 0 if it can't
int comp2d_probe(void);    // ACWW_PICA_PROBE: the pass left out this frame (frame_time's report), -1 off

// ---- the backend, for the core
// The textures the CPU writes are in the backend's layout: CG_TILED, the PICA's (8x8 tiles, the texels of a tile in
// Morton order), else row by row. Either way the texture coordinate v runs from the bottom: v = 1 is the first row.
#if defined(__3DS__)
#define CG_TILED 1
#else
#define CG_TILED 0
#endif
typedef struct CgTex CgTex;
enum { CG_RGBA5551, CG_RGBA8 };        // (formats)
enum { CG_ANY, CG_VRAM, CG_STAGED };   // (where: the GPU's own memory if it fits; CG_STAGED: there, with a copy the CPU
                                       // writes, cg_tex_written copies it over)
CgTex *cg_tex_new(int w, int h, int fmt, int where);  // nearest filter, clamped to the edge; 0 if no memory
void cg_tex_free(CgTex *t);
void *cg_tex_texels(CgTex *t);           // what the CPU writes
void cg_tex_written(CgTex *t);           // after it wrote them
void cg_tex_repeat(CgTex *t, int on);    // repeated both ways, or clamped to a transparent border
void cg_tex_upload(CgTex *t, const void *src, int w, int rows, int fmt);  // a picture's first rows (row by row)
CgTex *cg_tex_picture(void *texture3d);  // the 3D renderer's picture (gpu3d_pica_picture: the backend's own kind)
void *cg_alloc(uint32_t bytes);          // a buffer the GPU may read (the layers' pixels, the vertices)

// what is drawn: the texture (0: the vertex colour), the blending (CG_CONSTANT: eva / 16 of the source and evb / 16
// of what is there; the others ignore them), the alpha test, the stencil test (CG_OFF: none) with what passing
// writes (REPLACE through the write mask), the colour writes, the projection (pixels; tilt: the 3DS's turned screens)
// and a box the drawing leaves out (screen pixels; on 0: none)
void cg_source(CgTex *t);
enum { CG_OPAQUE, CG_OVER, CG_ALPHA, CG_CONSTANT };  // (1, 0; 1, 1 - a; a, 1 - a; constant)
void cg_blend(int mode, uint32_t eva, uint32_t evb);
enum { CG_OFF, CG_ALWAYS, CG_GREATER, CG_EQUAL };
void cg_alpha_test(int func, int ref);
void cg_stencil(int func, uint32_t ref, uint32_t mask, uint32_t write);
void cg_colour_writes(int on);
void cg_ortho(float left, float right, float bottom, float top, int tilt);
void cg_scissor_out(int on, int x0, int y0, int x1, int y1);

typedef struct {
    float pos[3];  // pixels
    float col[4];
    float tc[2];
} CgVertex;
enum { CG_MAX_VERTS = 0x8000 };
CgVertex *cg_vertices(void);             // CG_MAX_VERTS of them, filled by the core
void cg_draw(int first, int count);      // triangles

// the screens' widths (240 high): the 3DS's (top 400, bottom 320); the Switch's both 16:9, the wide view's 3D picture
// (256 + 2 * GPU3D_WIDE_X)
#if defined(__SWITCH__)
#define CG_SCREEN_W(s) 426
#else
#define CG_SCREEN_W(s) ((s) == 0 ? 400 : 320)
#endif
// the targets: an engine's picture (256x256, its texture), a screen (s 0 top, 1 bottom: CG_SCREEN_W x 240), the
// 3DS's doubling (pdisp_scaled); drawn on from now, cleared first (clear); 0 if there is none
enum { CG_COMP0, CG_COMP1, CG_SCREEN0, CG_SCREEN1, CG_UP };
int cg_target(int which, int clear);
CgTex *cg_target_tex(int which);
int cg_screens(void);                    // the screens' targets are there (and so drawn on)
int cg_screen_stencil(void);             // they have a stencil buffer (the wide view's)
// a frame's compositing: begun (the GPU's frame open, the uploads may follow), its drawing set up, ended
void cg_frame_begin(void);
void cg_draw_begin(void);
void cg_frame_end(int nverts);
// what was drawn, read back (after the frame): a screen CG_SCREEN_W x 240 row by row, an engine's picture 256x256; RGBA8 as
// r << 24 | g << 16 | b << 8 | a; 0 if it can't be
const uint32_t *cg_screen_read(int s);
const uint32_t *cg_comp_read(int e);
uint64_t cg_time_ms(void);

#endif

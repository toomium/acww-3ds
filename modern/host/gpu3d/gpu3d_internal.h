// The own 3D engine's shared state (gpu3d.h): the geometry engine (gpu3d_geom.c) fills polygon lists, the
// rasteriser (gpu3d_raster.c) draws them.
#ifndef GPU3D_INTERNAL_H
#define GPU3D_INTERNAL_H

#include "gpu3d.h"

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum { MAX_VERTICES = 6144, MAX_POLYGONS = 2048 };

typedef struct G3Vertex {
    s32 pos[4];       // clip coordinates x, y, z, w
    s32 color[3];     // 5-bit colour << 4 | 0xf when non-zero (9 bits: clipping interpolates in that precision)
    s16 tex[2];       // texture coordinates (1.11.4)
    u8 clipped;       // made by clipping (or of a clipped polygon): not shared with the next polygon of a strip
    u8 screenDone;
    s32 x, y;         // screen position (9 and 8 bits)
    s32 hx, hy;       // the same with 4 fractional bits
    int dbgLight;     // (diagnostics: the lighting record of its colour, -1 none)
    u8 farAxis;       // made by clipping against a plane of this axis + 1 (0 none), from outside vertex far:
    s32 farPos[4], farColor[3];  // (an edge crossing both planes of an axis: the second crossing from there)
    u8 dbgClips;      // (diagnostics: the interpolations behind it; for the last one the inside and outside
    s32 dbgClip[8];   // colours, din, dout)
#ifdef N3DS_DIRECT
    s32 obj[3];       // the object coordinates and the list's id of their clip matrix (g3p_matrix): the picture can
    u16 seg;          // be drawn again with other matrices (interpolation)
#endif
} G3Vertex;

typedef struct G3Polygon {
    G3Vertex *v[10];
    int nv;
    s32 z[10];        // depth value per vertex (24-bit Z, or W)
    s32 w[10];        // W per vertex
    u32 attr, texparam, texpal;
    u8 wbuffer, facing, translucent, shadowMask, shadow, line, degenerate;
    int vtop, vbottom;  // vertex indices of the top and bottom
    s32 ytop, ybottom, xtop, xbottom;
    u32 sortKey;
    int index;          // submission order
    G3Vertex orig[4];   // (diagnostics: the vertices before clipping)
    int norig;
} G3Polygon;

// the polygon list the rasteriser draws (handed over at the swap) and the rendering registers latched with it
typedef struct G3RenderState {
    G3Polygon *poly[MAX_POLYGONS];
    int npoly;
    int wbuffer, manualSort;
    u32 dispcnt;
    u16 clearColor, clearAttrHi;   // CLEAR_COLOR low half (colour, fog bit 15), high half (alpha, polygon id)
    u16 clearDepth, clearOffset;
    u8 alphaRef;
    u16 toon[32];
    u16 edge[8];
    u32 fogColor;
    u16 fogOffset;
    u8 fogTable[32];
    int list;  // the direct build (N3DS_DIRECT): the GPU's vertex list of these polygons (g3p_swap); poly is empty
} G3RenderState;

extern G3RenderState gG3Render;
extern const uint8_t *const *gG3Banks;  // VRAM banks A-I (gpu3d_frame)
extern u8 gG3VramCnt[9];
extern int gG3Diagnostics;  // (gpu3d_set_diagnostics: the comparisons' reports want more kept)
extern u32 gG3VramGeneration;  // (gpu3d_vram_generation)
extern u32 gG3Swaps;  // SWAP_BUFFERS done: gG3Render is set then

// gpu3d_raster.c
void g3_render(void);                    // gG3Render into the frame buffer
void g3_render_wait(void);               // until the picture being drawn from gG3Render is done (the 3DS)
const u32 *g3_frame_lines(void);
int g3_raster_pixel_polygon(int x, int y);  // index into gG3Render.poly, -1 none
void g3_raster_diagnostics(int on);

// gpu3d_wgpu.c (HOST_WGPU builds; elsewhere g3w_init fails)
int g3w_init(void *device, void *queue, int scale);
void g3w_render(const G3RenderState *r, const u8 *img, const u8 *pal, u32 *out);
void *g3w_picture(void);

// gpu3d_pica.c (the 3DS; elsewhere g3p_init fails): the polygons drawn by the 3DS's GPU into the rasteriser's
// colour, depth and flag buffers (gpu3d_raster.c's formats; flags: bit 0 fog)
int g3p_init(void);
// (texChanged: the slots may have changed since the last frame; color NULL: drawn for the GPU's compositing into a
// texture, g3p_picture, nothing read back)
void g3p_render(const G3RenderState *r, const u8 *img, const u8 *pal, int texChanged, u32 *color, u32 *depth, u8 *flags);
void *g3p_picture(void);  // the last picture drawn for the GPU's compositing (a C3D_Tex), NULL none
void g3p_wait(void);  // until a picture being drawn (by the GPU thread) is done: before its list or textures change
void g3_texture_rgba(u32 texparam, u32 texpal, u32 *out);  // (gpu3d_raster.c)
// the direct build (N3DS_DIRECT): the geometry engine's polygons go straight into the GPU's
// vertex list, in the DS screen's clip space (the viewport applied, not clipped: the GPU clips), while the list
// before is drawn
void g3p_viewport(const u32 vp[6]);  // VIEWPORT: x1, y1 (from the top), x2, y2, width, height (gpu3d_geom.c's sViewport)
void g3p_polygon(G3Vertex *const *v, int n, u32 attr, u32 texparam, u32 texpal, int translucent, int wbuffer);
int g3p_swap(void);  // the list filled since the last swap handed over (its number, G3RenderState.list), a new one begun
#ifdef N3DS_DIRECT
int g3p_matrix(const s32 clip[16]);  // a clip matrix (20.12) the list being filled uses: its id (G3Vertex.seg)
void g3p_vertex(int seg, const s32 obj[3]);  // a vertex submitted under it (culled or not): what identifies it
// each frame, before g3_render's drawing: whether a picture between the game's is due (the same list again)
int g3p_between(const G3RenderState *r);
#endif

// feature census (gpu3d_geom.c)
void g3_feature(int id);
enum {
    F_CMD = 0,          // + command 0..0x7f
    F_POLYMODE = 0x80,  // + 0..3
    F_TEXFMT = 0x84,    // + 0..7
    F_TEXGEN = 0x8c,    // + 0..3
    F_LIGHT = 0x90,
    F_SHININESS_TABLE,
    F_CLIP,
    F_FARCLIP_HIDE,
    F_CULLED,
    F_ONEDOT_HIDE,
    F_TRANSLUCENT,
    F_WIREFRAME,
    F_DEPTH_EQUAL,
    F_FOG_POLY,
    F_WBUFFER,
    F_ZBUFFER_POLYS,
    F_MANUAL_SORT,
    F_STACK_ERROR,
    F_RAM_OVERFLOW,
    F_FIFO_QUEUED,
    F_SWAP_QUEUED,
    F_FIFO_FULL,
    F_TEX_REPEAT,
    F_TEX_FLIP,
    F_TEX_CLAMP,
    F_COLOR0_TRANSPARENT,
    F_TRANS_DEPTH_UPDATE,
    F_LINE_POLY,
    F_TEXTURE_OFF,      // DISP3DCNT texture mapping off
    F_TOON_HIGHLIGHT,   // DISP3DCNT bit 1
    F_ALPHA_TEST,
    F_ALPHA_BLEND,
    F_ANTIALIAS,
    F_EDGE_MARK,
    F_FOG_ALPHA_ONLY,
    F_FOG,
    F_REAR_BITMAP,
    F_BOX_TEST, F_POS_TEST, F_VEC_TEST,
    F_READ_CLIPMTX, F_READ_VECMTX, F_READ_RAMCOUNT, F_READ_POSRESULT, F_READ_VECRESULT, F_READ_RDLINES,
    F_GEOM_OFF_WRITE,
    F_SHADOW_MASK, F_SHADOW,
    F_COUNT
};

#endif

// Host build: the screens composited and shown by the 3DS's GPU (pica_display.c). The 2D renderer
// gives each line's layers and compositing state (gpu2d.h Gpu2dLayers), the 3D engine its picture; here they are put
// together as the DS does, by the GPU, and shown on the 3DS's screens. Plain C types: render.c and the frontend use it.
#ifndef HOST_PICA_DISPLAY_H
#define HOST_PICA_DISPLAY_H

#include <stdint.h>

struct Gpu2dLayers;

int pdisp_init(void);                     // 0 if this build or the GPU can't (then the CPU composites)
void pdisp_attach_screens(void);          // (the frontend, once the 3DS's screens are set up: show on them)
int pdisp_scaled(void);                   // the screens shown 1.25 times (320x240), else 1:1
// a touch on the bottom screen at (x, y) of the DS's screen in its middle: moved to where it is on the DS's screen if it
// is on a UI element moved to the wide screen's edges (the direct build)
void pdisp_touch_map(int *x, int *y);
struct Gpu2dLayers *pdisp_layers(void);   // where the 2D renderer puts this frame's layers
// a frame: the layers the 2D renderer made and the 3D picture: a texture the GPU drew (gpu3d.h gpu3d_pica_picture,
// premultiplied alpha), else picture3d (gpu3d_lines' format; NULL: none)
void pdisp_frame(void *texture3d, const uint32_t *picture3d);
// the last frame's screen (0 top, 1 bottom) read back: 256x192, 0x00RRGGBB
const uint32_t *pdisp_screen(int screen);
// The GPU thread (the 3DS's core 2): a job for it, of a kind (done at once where there is none), in turn after the
// others; waiting until the last job of a kind is done (older 1: the one before it; before what it reads changes),
// or every job (before citro3d is used elsewhere); the profile's marker its parts go to (prof.h)
enum { PDISP_3D, PDISP_COMP };
void pdisp_submit(int kind, void (*job)(void *), void *arg);
// a job's citro3d frame: begun (C3D_FrameBegin) unless this frame's 3D job began it already (the compositing goes on
// in it); after the 3D job's commands, the GPU started on them (the direct build; elsewhere the frame ended)
void pdisp_frame_open(int kind);
void pdisp_frame_run(void);
void pdisp_wait(int kind, int older);
void pdisp_idle(void);
// GSP for the GPU's renderers (gspInit, counted), and before the program ends (the frontend, before gfxExit):
// citro3d ended and those references given back, so that gfxExit's gspExit stops GSP's event thread
void pdisp_gsp_init(void);
void pdisp_quit(void);
extern volatile int *gPicaProf;

#endif

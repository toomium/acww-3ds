// Host build runtime: ACWW_FPS's profile (hw.c). The part of the program the game's thread is in, read every
// millisecond by a sampler thread; HOST_PROF(part) sets it for the rest of the enclosing block. Whatever isn't marked
// is PROF_GAME: the game with its SDK and the runtime's remaining parts. (Its own header, without host.h's types: the
// 3DS's GPU files include it beside libctru's.)
#ifndef HOST_PROF_H
#define HOST_PROF_H

enum {
    PROF_GAME, PROF_REGS, PROF_DMA, PROF_GEOM, PROF_2D, PROF_3D, PROF_COMP, PROF_SOUND, PROF_FRONT, PROF_PACE,
    PROF_GPU,  // waiting for the GPU (the 3DS)
    PROF_LIGHT, PROF_VERTEX,  // the geometry engine's lighting, and its vertices and polygons (setup, clipping)
    PROF_WAIT,  // waiting for the GPU thread (the 3DS: pica_display.c)
    PROF_IDLE,  // (the GPU thread's marker: nothing to do)
    PROF_ROM,   // reading the ROM (card.c: from the file, through its cache)
    PROF_ARM7,  // the ARM7's side: its services (pxi7.c: files, the save, the clock), the wireless (wm7.c)
    PROF_UPLOAD,  // waiting for the GPU's copies of the layers into textures (the 3DS: display transfers)
    PROF_VRAM,  // VRAM's banks and the CPU's windows kept in step (vram.c: a VRAMCNT change, the frame's flush)
    PROF_TILES, PROF_HASH, PROF_FEND,  // (the 3DS's compositing: the BGs' tile textures, the screens' hashes, the
                                       // citro3d frame's end)
    PROF_LAYERS,  // (the 3DS: the 2D renderer's layers drawn from its record, gpu2d_draw_layers)
    PROF_MTX, PROF_POLY,  // the geometry engine's matrix commands and clip matrix, its polygons' set-up (gpu3d_geom.c)
    PROF_FOG,  // (the 3DS: the fog worked into the 3D picture's vertices, gpu3d_pica.c)
    PROF_N
};
extern volatile int gHostProf;
static inline void host_prof_restore(int *saved) { gHostProf = *saved; }
#define HOST_PROF(part) int hostProfSaved_ __attribute__((cleanup(host_prof_restore))) = gHostProf; gHostProf = (part)
// the GPU thread's own marker, and HOST_PROF for a marker given (*var)
extern volatile int gHostProfWorker;
typedef struct { volatile int *var; int saved; } HostProfSaved;
static inline void host_prof_restore_on(HostProfSaved *s) { *s->var = s->saved; }
#define HOST_PROF_ON(v, part) \
    HostProfSaved hostProfSavedOn_ __attribute__((cleanup(host_prof_restore_on))) = {(v), *(v)}; *(v) = (part)

#endif

// Host build: the Switch's OpenGL ES 3, shared by its compositing (gl_display.c, comp2d.h's backend) and its 3D
// renderer (gpu3d/gpu3d_gl.c): the one context, and a texture as both know it.
#ifndef HOST_GL_DISPLAY_H
#define HOST_GL_DISPLAY_H

#include <stdint.h>

// comp2d.h's CgTex: a GL texture; a target's framebuffer object with its depth and stencil; the copy the CPU
// writes (cg_tex_texels), row by row in comp2d's formats
struct CgTex {
    unsigned tex, fbo, depth;  // (GLuint)
    int w, h, fmt;
    void *cpu;
    int border;                // clamped to a transparent border (cg_tex_repeat off), not to the edge
    int k;                     // a target: drawn k times as large as its size in the DS's pixels (gld_scale)
};

// The context: EGL's, made at the first call (without a surface until the window is there: pdisp_attach_screens);
// current between gld_enter and gld_leave, which puts back the one current before (an SDL renderer's). 0 if none.
int gld_context(void);
void gld_enter(void);
void gld_leave(void);
// a texture to draw on, w x h of the DS's pixels (RGBA8) drawn gld_scale() times as large, with a framebuffer object
// and a depth and stencil buffer; 0 if it can't be
int gld_target(struct CgTex *t, int w, int h);
// how many times as large the screens and the 3D are drawn: ACWW_SWITCH_SCALE=1-4, by default 3 (a screen's
// 426 x 240 as 1278 x 720, the display's height)
int gld_scale(void);

#endif

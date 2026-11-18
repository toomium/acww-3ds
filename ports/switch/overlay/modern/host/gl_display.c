// Host build: the screens composited and shown by the Switch's GPU (pica_display.h; OpenGL ES 3): the backend of the
// console ports' compositing (comp2d.c has the logic, as on the 3DS) and the window. Both DS screens are drawn as the
// wide view shows them (both 426 x 240 here, 16:9: CG_SCREEN_W; the DS's pictures in their middle) into textures,
// then shown on the 1280 x 720 display in one of the layouts below (pdisp_present, the frontend's each frame; L3 the
// next; the first: one screen, the world, the touch screen as a panel when it has something), a touch on the bottom
// one (the touch screen, or docked a pointer on the right stick) the DS's touch screen (pdisp_window_to_ds).
// Differences from the 3DS's backend: no GPU thread (the jobs at once, on the game's thread); the textures the CPU
// writes are rows, uploaded with glTexSubImage2D; the inverted scissor is a test in the fragment shader; the texture
// coordinates' v is turned in the vertex shader (comp2d's v = 1 is a texture's first row, GL's t = 0), and the targets
// are drawn with their first row at the top, so that a target read as a texture has the same rows.
// The screens and the 3D are drawn gld_scale() times as large (3: a screen's 426 x 240 as 1278 x 720), comp2d
// still in the DS's pixels through the projection: the 3D's edges and the affine BGs sharp at the display's
// resolution, the 2D's pixels as large squares.
// shortcut: the screens shown with the linear filter; the key binding screen (frontend_ui.c) and
// ACWW_WINSHOT aren't shown over them.
#ifdef __SWITCH__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <switch.h>

#include "comp2d.h"
#include "options_cfg.h"
#include "gl_display.h"
#include "gpu2d/gpu2d.h"
#include "gpu3d/gpu3d.h"
#include "pica_display.h"
#include "plat.h"
#include "prof.h"

volatile int *gPicaProf = &gHostProf;

// ---- the context
static EGLDisplay sDpy = EGL_NO_DISPLAY;
static EGLConfig sConfig;
static EGLContext sCtx = EGL_NO_CONTEXT;
static EGLSurface sWinSurf = EGL_NO_SURFACE;
static struct { EGLDisplay d; EGLSurface draw, read; EGLContext c; } sSaved;
static int sDepth;  // (gld_enter's nesting)

int gld_context(void) {
    static int tried;
    if (sCtx != EGL_NO_CONTEXT || tried++) return sCtx != EGL_NO_CONTEXT;
    sDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (sDpy == EGL_NO_DISPLAY || !eglInitialize(sDpy, 0, 0) || !eglBindAPI(EGL_OPENGL_ES_API)) {
        fprintf(stderr, "gl: no EGL display (%#x)\n", eglGetError());
        return 0;
    }
    static const EGLint kConfig[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                     EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE};
    static const EGLint kContext[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLint n = 0;
    if (!eglChooseConfig(sDpy, kConfig, &sConfig, 1, &n) || n < 1) {
        fprintf(stderr, "gl: no EGL config for OpenGL ES 3 (%#x)\n", eglGetError());
        return 0;
    }
    // (made current without a surface until the window's: it draws into framebuffer objects; Mesa's EGL here has
    // window surfaces only, EGL_KHR_surfaceless_context)
    if ((sCtx = eglCreateContext(sDpy, sConfig, EGL_NO_CONTEXT, kContext)) == EGL_NO_CONTEXT) {
        fprintf(stderr, "gl: no OpenGL ES 3 context (%#x)\n", eglGetError());
        return 0;
    }
    return 1;
}
// (made current only when it isn't, or not with the window's surface; and left current when no other context was:
// eglMakeCurrent costs milliseconds on the Switch, at every picture and the compositing it was a large part of the 3D's
// time; only an SDL renderer's context, when the CPU composites, is put back)
void gld_enter(void) {
    if (sDepth++) return;
    sSaved.d = eglGetCurrentDisplay(), sSaved.draw = eglGetCurrentSurface(EGL_DRAW);
    sSaved.read = eglGetCurrentSurface(EGL_READ), sSaved.c = eglGetCurrentContext();
    if (sSaved.c != sCtx || sSaved.draw != sWinSurf) eglMakeCurrent(sDpy, sWinSurf, sWinSurf, sCtx);
}
void gld_leave(void) {
    if (--sDepth) return;
    if (sSaved.c == sCtx || sSaved.c == EGL_NO_CONTEXT) return;
    eglMakeCurrent(sSaved.d, sSaved.draw, sSaved.read, sSaved.c);
}

// ---- the program: comp2d's quads (pixels through the projection; the colour or a texel, the alpha test, the
// transparent border, the box left out)
enum { W = 256, H = 192, TEX = 256 };
static GLuint sProg, sVao, sVbo, sPresentProg;
static GLint uProj, uTex, uUseTex, uAtest, uAref, uBorder, uHole, uHoleBox;
static CgVertex *sV;
static float sProj[16];
static CgTex *sSource;
static int sTargetK = 1;  // (the target drawn on: its k, for cg_scissor_out's box)
static CgTex sComp[2], sScreen[2];
static int sCompMade[2], sScreensMade;

static const char kVs[] =
    "#version 300 es\n"
    "layout(location = 0) in vec3 inpos;\n"
    "layout(location = 1) in vec4 inclr;\n"
    "layout(location = 2) in vec2 intc;\n"
    "uniform mat4 proj;\n"
    "out vec4 vclr;\n"
    "out vec2 vtc;\n"
    "void main() {\n"
    "    gl_Position = proj * vec4(inpos, 1.0);\n"
    "    vclr = inclr;\n"
    "    vtc = vec2(intc.x, 1.0 - intc.y);\n"  // (comp2d's v = 1: the first row, GL's t = 0)
    "}\n";
static const char kFs[] =
    "#version 300 es\n"
    "precision highp float;\n"
    "precision highp int;\n"
    "in vec4 vclr;\n"
    "in vec2 vtc;\n"
    "uniform sampler2D tex;\n"
    "uniform int useTex, atest, aref, border, hole;\n"  // (atest: 0 none, 2 greater, 3 equal: comp2d.h's CG_*)
    "uniform vec4 holeBox;\n"
    "out vec4 o;\n"
    "void main() {\n"
    "    if (hole != 0 && gl_FragCoord.x >= holeBox.x && gl_FragCoord.x < holeBox.z &&\n"
    "        gl_FragCoord.y >= holeBox.y && gl_FragCoord.y < holeBox.w) discard;\n"
    "    vec4 c = vclr;\n"
    "    if (useTex != 0) {\n"
    "        c = texture(tex, vtc);\n"
    "        if (border != 0 && (vtc.x < 0.0 || vtc.x >= 1.0 || vtc.y < 0.0 || vtc.y >= 1.0)) c = vec4(0.0);\n"
    "    }\n"
    "    int a = int(c.a * 255.0 + 0.5);\n"
    "    if ((atest == 2 && a <= aref) || (atest == 3 && a != aref)) discard;\n"
    "    o = c;\n"
    "}\n";
// the screens shown in the window: a texture as it is
static const char kPresentVs[] =
    "#version 300 es\n"
    "layout(location = 0) in vec3 inpos;\n"
    "layout(location = 2) in vec2 intc;\n"
    "out vec2 vtc;\n"
    "void main() { gl_Position = vec4(inpos.xy, 0.0, 1.0); vtc = intc; }\n";
static const char kPresentFs[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 vtc;\n"
    "uniform sampler2D tex;\n"
    "out vec4 o;\n"
    "void main() { o = vec4(texture(tex, vtc).rgb, 1.0); }\n";

static GLuint shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, 0);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, sizeof log, 0, log);
        fprintf(stderr, "gl: a shader didn't compile: %s\n", log);
    }
    return s;
}
static GLuint program(const char *vs, const char *fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, shader(GL_VERTEX_SHADER, vs));
    glAttachShader(p, shader(GL_FRAGMENT_SHADER, fs));
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(p, sizeof log, 0, log);
        fprintf(stderr, "gl: the shaders didn't link: %s\n", log);
        return 0;
    }
    return p;
}

int pdisp_init(void) {
    static int done;
    if (done) return done > 0;
    done = -1;
    if (!gld_context()) return 0;
    gld_enter();
    sProg = program(kVs, kFs);
    sPresentProg = program(kPresentVs, kPresentFs);
    if (sProg) {
        uProj = glGetUniformLocation(sProg, "proj");
        uTex = glGetUniformLocation(sProg, "tex");
        uUseTex = glGetUniformLocation(sProg, "useTex");
        uAtest = glGetUniformLocation(sProg, "atest");
        uAref = glGetUniformLocation(sProg, "aref");
        uBorder = glGetUniformLocation(sProg, "border");
        uHole = glGetUniformLocation(sProg, "hole");
        uHoleBox = glGetUniformLocation(sProg, "holeBox");
    }
    glGenVertexArrays(1, &sVao);
    glBindVertexArray(sVao);
    glGenBuffers(1, &sVbo);
    glBindBuffer(GL_ARRAY_BUFFER, sVbo);
    glBufferData(GL_ARRAY_BUFFER, CG_MAX_VERTS * sizeof(CgVertex), 0, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CgVertex), (void *)offsetof(CgVertex, pos));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CgVertex), (void *)offsetof(CgVertex, col));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CgVertex), (void *)offsetof(CgVertex, tc));
    for (int i = 0; i < 3; i++) glEnableVertexAttribArray((GLuint)i);
    int ok = sProg && sPresentProg && comp2d_init();
    gld_leave();
    done = ok ? 1 : -1;
    fprintf(stderr, "gl: %s\n", ok ? "the screens composited by the GPU" : "the GPU's compositing couldn't start");
    return ok;
}

// the window (the frontend's, made by now): its surface, and the screens' targets
void pdisp_attach_screens(void) {
    if (sWinSurf != EGL_NO_SURFACE || sCtx == EGL_NO_CONTEXT) return;
    NWindow *win = nwindowGetDefault();
    nwindowSetDimensions(win, 1280, 720);  // (no SDL window set them: 0 x 0 otherwise)
    sWinSurf = eglCreateWindowSurface(sDpy, sConfig, (EGLNativeWindowType)win, 0);
    if (sWinSurf == EGL_NO_SURFACE) fprintf(stderr, "gl: no window surface (%#x)\n", eglGetError());
}

int pdisp_scaled(void) { return 0; }
void pdisp_submit(int kind, void (*job)(void *), void *arg) { (void)kind, job(arg); }
int pdisp_threaded(void) { return 0; }
void pdisp_frame_open(int kind) { (void)kind; }
void pdisp_frame_run(void) {}
void pdisp_wait(int kind, int older) { (void)kind, (void)older; }
void pdisp_idle(void) {}
void pdisp_gsp_init(void) {}
void pdisp_quit(void) {}

// ---- comp2d.h's backend: OpenGL ES 3
static int target_make(CgTex *t, int w, int h);
static void tex_params(CgTex *t) {
    glBindTexture(GL_TEXTURE_2D, t->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
CgTex *cg_tex_new(int w, int h, int fmt, int where) {
    CgTex *t = calloc(1, sizeof *t);
    if (!t) return 0;
    t->w = w, t->h = h, t->fmt = fmt;
    t->border = where == CG_STAGED;  // (an affine BG without wrap: transparent outside)
    if (!(t->cpu = calloc((size_t)w * h, fmt == CG_RGBA5551 ? 2 : 4))) { free(t); return 0; }
    gld_enter();
    glGenTextures(1, &t->tex);
    tex_params(t);
    if (fmt == CG_RGBA5551) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, t->cpu);
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->cpu);
    gld_leave();
    return t;
}
void cg_tex_free(CgTex *t) {
    gld_enter();
    glDeleteTextures(1, &t->tex);
    gld_leave();
    free(t->cpu);
    free(t);
}
void *cg_tex_texels(CgTex *t) { return t->cpu; }
// rows of comp2d's pixels into the texture: RGBA5551 as GL's 5_5_5_1 (the same bits); RGBA8 (r << 24 | ... | a, as
// the PICA's) byte-swapped to GL's bytes r, g, b, a
static void put_rows(CgTex *t, const void *src, int w, int rows, int fmt) {
    glBindTexture(GL_TEXTURE_2D, t->tex);
    if (fmt == CG_RGBA5551) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, rows, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, src);
        return;
    }
    static uint32_t *swapped;
    static size_t room;
    size_t n = (size_t)w * rows;
    if (n > room) {
        uint32_t *p = realloc(swapped, n * 4);
        if (!p) return;
        swapped = p, room = n;
    }
    const uint32_t *s = src;
    for (size_t i = 0; i < n; i++) swapped[i] = __builtin_bswap32(s[i]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, rows, GL_RGBA, GL_UNSIGNED_BYTE, swapped);
}
// (the texture drawn from bound again after an upload: glBindTexture is also how a texture is written)
static void rebind(void) {
    if (sSource) glBindTexture(GL_TEXTURE_2D, sSource->tex);
}
void cg_tex_written(CgTex *t) {
    put_rows(t, t->cpu, t->w, t->h, t->fmt);
    rebind();
}
void cg_tex_repeat(CgTex *t, int on) {
    glBindTexture(GL_TEXTURE_2D, t->tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, on ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, on ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    t->border = !on;
    rebind();
}
void cg_tex_upload(CgTex *t, const void *src, int w, int rows, int fmt) {
    HOST_PROF_ON(gPicaProf, PROF_UPLOAD);
    put_rows(t, src, w, rows, fmt);
    rebind();
}
CgTex *cg_tex_picture(void *texture3d) { return texture3d; }  // (gpu3d_gl.c's pictures are CgTex)
void *cg_alloc(uint32_t bytes) { return calloc(1, bytes); }

void cg_source(CgTex *t) {
    sSource = t;
    glUniform1i(uUseTex, t != 0);
    if (!t) return;
    glBindTexture(GL_TEXTURE_2D, t->tex);
    glUniform1i(uBorder, t->border);
}
void cg_blend(int mode, uint32_t eva, uint32_t evb) {
    if (mode == CG_OPAQUE) {
        glDisable(GL_BLEND);
        return;
    }
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (mode == CG_CONSTANT) {  // (eva / 16 of the source, evb / 16 of what is there, as the 3DS's 8 bits)
        float ca = (float)(eva * 255 / 16) / 255, cb = (float)(evb * 255 / 16) / 255;
        glBlendColor(ca, ca, ca, cb);
        glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA, GL_ONE, GL_ZERO);
    } else {
        glBlendFuncSeparate(mode == CG_ALPHA ? GL_SRC_ALPHA : GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    }
}
void cg_alpha_test(int func, int ref) {
    glUniform1i(uAtest, func == CG_OFF || func == CG_ALWAYS ? 0 : func);
    glUniform1i(uAref, ref);
}
void cg_stencil(int func, uint32_t ref, uint32_t mask, uint32_t write) {
    if (func == CG_OFF) {
        glDisable(GL_STENCIL_TEST);
        return;
    }
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(func == CG_EQUAL ? GL_EQUAL : GL_ALWAYS, (GLint)ref, mask);
    glStencilMask(write);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}
void cg_colour_writes(int on) { glColorMask(on, on, on, on); }
// (y turned: the target's first row at the top, so that a target read as a texture has comp2d's rows; tilt: the
// 3DS's turned screens, none here)
void cg_ortho(float l, float r, float b, float t, int tilt) {
    (void)tilt;
    memset(sProj, 0, sizeof sProj);
    sProj[0] = 2 / (r - l), sProj[12] = -(r + l) / (r - l);
    sProj[5] = 2 / (b - t), sProj[13] = -(b + t) / (b - t);
    sProj[10] = -1, sProj[15] = 1;  // (z 0.5 for every vertex: no depth test)
    glUniformMatrix4fv(uProj, 1, GL_FALSE, sProj);
}
void cg_scissor_out(int on, int x0, int y0, int x1, int y1) {
    glUniform1i(uHole, on);
    float k = (float)sTargetK;  // (the target's pixels, rows from the top: as drawn)
    if (on) glUniform4f(uHoleBox, (float)x0 * k, (float)y0 * k, (float)x1 * k, (float)y1 * k);
}
CgVertex *cg_vertices(void) {
    if (!sV) sV = malloc(CG_MAX_VERTS * sizeof(CgVertex));
    return sV;
}
void cg_draw(int first, int count) {
    glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)first * (GLintptr)sizeof(CgVertex), (GLsizeiptr)count * (GLsizeiptr)sizeof(CgVertex),
                    sV + first);
    glDrawArrays(GL_TRIANGLES, first, count);
}

int gld_scale(void) {
    static int k;
    if (!k) {
        k = getenv("ACWW_SWITCH_SCALE") ? atoi(getenv("ACWW_SWITCH_SCALE")) : 3;
        k = k < 1 ? 1 : k > 4 ? 4 : k;
    }
    return k;
}
// a target: its texture, a framebuffer object with a depth and stencil buffer (gl_display.h: gpu3d_gl.c's too)
int gld_target(CgTex *t, int w, int h) {
    gld_enter();
    int ok = target_make(t, w, h);
    gld_leave();
    return ok;
}
static int target_make(CgTex *t, int w, int h) {
    t->k = gld_scale();
    w *= t->k, h *= t->k;
    t->w = w, t->h = h, t->fmt = CG_RGBA8;
    glGenTextures(1, &t->tex);
    tex_params(t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glGenRenderbuffers(1, &t->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, t->depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glGenFramebuffers(1, &t->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, t->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t->tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, t->depth);
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
static int screens_make(void) {
    if (!sScreensMade) {
        gld_enter();
        sScreensMade = target_make(&sScreen[0], CG_SCREEN_W(0), 240) && target_make(&sScreen[1], CG_SCREEN_W(1), 240) ? 1 : -1;
        gld_leave();
    }
    return sScreensMade > 0;
}
int cg_target(int which, int clear) {
    CgTex *t = which <= CG_COMP1 ? cg_target_tex(which) : which == CG_UP ? 0 : screens_make() ? &sScreen[which - CG_SCREEN0] : 0;
    if (!t) return 0;
    glBindFramebuffer(GL_FRAMEBUFFER, t->fbo);
    glViewport(0, 0, t->w, t->h);
    sTargetK = t->k;
    if (clear) {
        GLboolean mask[4];
        glGetBooleanv(GL_COLOR_WRITEMASK, mask);
        glColorMask(1, 1, 1, 1);
        glStencilMask(0xff);
        glClearColor(0, 0, 0, which <= CG_COMP1 ? 0.0f : 1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColorMask(mask[0], mask[1], mask[2], mask[3]);
    }
    return 1;
}
CgTex *cg_target_tex(int which) {
    if (which == CG_UP) return 0;
    int e = which - CG_COMP0;
    if (!sCompMade[e]) {
        gld_enter();
        sCompMade[e] = target_make(&sComp[e], TEX, TEX) ? 1 : -1;
        gld_leave();
    }
    return sCompMade[e] > 0 ? &sComp[e] : 0;
}
int cg_screens(void) { return screens_make(); }
int cg_screen_stencil(void) { return gpu3d_wide_wanted(); }  // (the wide view: comp2d draws straight onto the screens)

void cg_frame_begin(void) { gld_enter(); }
void cg_draw_begin(void) {
    glUseProgram(sProg);
    glBindVertexArray(sVao);
    glBindBuffer(GL_ARRAY_BUFFER, sVbo);
    glBufferData(GL_ARRAY_BUFFER, CG_MAX_VERTS * sizeof(CgVertex), 0, GL_STREAM_DRAW);  // (a new one: not the frame
                                                                                     // before's, which the GPU may still read)
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uTex, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    cg_alpha_test(CG_OFF, 0);
    cg_scissor_out(0, 0, 0, 0, 0);
}
void cg_frame_end(int nverts) {
    (void)nverts;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gld_leave();
}

// (in the DS's pixels, comp2d's sizes: of a target drawn k times as large every k-th pixel)
static const uint32_t *read_target(CgTex *t) {
    static uint32_t *full, px[512 * 256];
    static size_t room;
    size_t n = (size_t)t->w * t->h;
    if (n > room) {
        uint32_t *f = realloc(full, n * 4);
        if (!f) return 0;
        full = f, room = n;
    }
    gld_enter();
    glBindFramebuffer(GL_FRAMEBUFFER, t->fbo);
    glReadPixels(0, 0, t->w, t->h, GL_RGBA, GL_UNSIGNED_BYTE, full);  // (the first row the top one: cg_ortho)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gld_leave();
    int w = t->w / t->k, h = t->h / t->k;
    for (int y = 0; y < h; y++)  // (bytes r, g, b, a: r << 24 | ... | a)
        for (int x = 0; x < w; x++) px[y * w + x] = __builtin_bswap32(full[(size_t)y * t->k * t->w + (size_t)x * t->k]);
    return px;
}
const uint32_t *cg_screen_read(int s) { return screens_make() ? read_target(&sScreen[s]) : 0; }
const uint32_t *cg_comp_read(int e) { return cg_target_tex(CG_COMP0 + e) ? read_target(&sComp[e]) : 0; }
uint64_t cg_time_ms(void) { return host_plat_time_ns() / 1000000u; }

// ---- the window: the two screens laid out on the one display, in one of several layouts, the next one at a press of
// the left stick (L3; the buttons here are read through libnx: SDL's are the DS's); ACWW_LAYOUT=<n> the first one.
// The first, one screen: the world (the screen engine A is on, the one with the 3D) fills the display; the other
// comes over it as a panel when it is the touch screen with something on it (the game has put the world on the top
// screen: its menus, letters, the keyboard), and the right stick's press (R3) shows or hides it for a while (until the
// game changes the screens again). The others show both: side by side, one above the other, one large with the other
// small in a corner, one alone. A touch goes to the screen on top. Docked: the right stick moves a pointer, ZR touches
// where it is.
// The layout is the options page's "Screen layout" too (ACWW_LAYOUT, kept in acww.cfg), the scale its "Resolution".
enum { LAYOUTS = 7 };
static const char *const kLayoutName[LAYOUTS] = {
    "one screen", "side by side", "one above the other", "the bottom large, the top small",
    "the top large, the bottom small", "the bottom alone", "the top alone"};
static int sLayout = -1;
static int layout(void) {
    if (sLayout < 0) {
        sLayout = getenv("ACWW_LAYOUT") ? atoi(getenv("ACWW_LAYOUT")) : 0;
        if (sLayout < 0 || sLayout >= LAYOUTS) sLayout = 0;
    }
    return sLayout;
}
// (the options page's "Screen layout": options_cfg.c)
void gld_set_layout(int l) {
    if (l >= 0 && l < LAYOUTS) sLayout = l;
}
// the screen engine A (the 3D, the world) is on
static int world_screen(void) {
    const struct Gpu2dLayers *L = pdisp_layers();
    return L && L->topEngine == 0 ? 0 : 1;
}
// one screen: the other one shown as a panel (the touch screen with the world on top), R3 flipping that
static int sPeek, sAutoBefore = -1;
static int panel_shown(void) {
    int autoShown = world_screen() == 0;
    if (autoShown != sAutoBefore) sAutoBefore = autoShown, sPeek = 0;  // (the game changed the screens: R3's undone)
    return autoShown ^ sPeek;
}
// the screen drawn first (the other over it)
static int under(void) {
    int l = layout();
    return l == 0 ? world_screen() : l == 3 || l == 5 ? 1 : 0;
}
// the controller, once a frame (pdisp_present): L3 the next layout, R3 the panel, the right stick the pointer (in the
// display's 1280 x 720), ZR its touch
static PadState sPad;
static float sPtrX = 640, sPtrY = 360;
static int sPtrShown, sPtrTouch;
static void pad_poll(void) {
    static int init;
    if (!init++) {
        padConfigureInput(8, HidNpadStyleSet_NpadStandard);  // (as SDL's: the standard controllers)
        padInitializeDefault(&sPad);
    }
    padUpdate(&sPad);
    u64 down = padGetButtonsDown(&sPad), held = padGetButtons(&sPad);
    if (down & HidNpadButton_StickL) {
        sLayout = (layout() + 1) % LAYOUTS;
        fprintf(stderr, "gl: layout %d, %s\n", sLayout, kLayoutName[sLayout]);
        opt_store("ACWW_LAYOUT", sLayout);  // (the options page's "Screen layout" with it, kept in acww.cfg)
    }
    if ((down & HidNpadButton_StickR) && layout() == 0) sPeek ^= 1;
    HidAnalogStickState st = padGetStickPos(&sPad, 1);
    enum { DEAD = 6000 };
    float vx = abs(st.x) > DEAD ? (float)st.x / 32767 : 0, vy = abs(st.y) > DEAD ? (float)st.y / 32767 : 0;
    if (vx || vy) {  // (up to 14 pixels a frame, slower near the middle)
        sPtrX = fminf(1279, fmaxf(0, sPtrX + vx * fabsf(vx) * 14));
        sPtrY = fminf(719, fmaxf(0, sPtrY - vy * fabsf(vy) * 14));
        sPtrShown = 180;  // (3 seconds after it last moved or touched)
    }
    sPtrTouch = (held & HidNpadButton_ZR) && sPtrShown;
    if (sPtrTouch) sPtrShown = 180;
    else if (sPtrShown) sPtrShown--;
}
// screen s's box in a display w x h in the layout: x, y, width, height (width 0: not shown)
static void screen_box(int s, int w, int h, float box[4]) {
    const float W0 = (float)CG_SCREEN_W(0), W1 = (float)CG_SCREEN_W(1), fw = (float)w, fh = (float)h, sw = s ? W1 : W0;
    float k;
    box[0] = box[1] = box[2] = box[3] = 0;
    switch (layout()) {
    case 0:  // one screen: the world filling the display, the other a panel twice the size at its bottom
        if (s == world_screen()) {
            k = fminf(fw / sw, fh / 240);
            box[0] = (fw - sw * k) / 2, box[1] = (fh - 240 * k) / 2;
        } else {
            if (!panel_shown()) return;
            k = fminf(2, fminf(fw / sw, fh / 240));
            box[0] = (fw - sw * k) / 2, box[1] = fh - 240 * k;
        }
        break;
    case 1:  // side by side
        k = fminf(fw / (W0 + W1), fh / 240);
        box[0] = (fw - (W0 + W1) * k) / 2 + (s ? W0 * k : 0), box[1] = (fh - 240 * k) / 2;
        break;
    case 2:  // one above the other
        k = fminf(fh / 480, fw / fmaxf(W0, W1));
        box[0] = (fw - sw * k) / 2, box[1] = (fh - 480 * k) / 2 + (s ? 240 * k : 0);
        break;
    default: {  // one large (3, 5: the bottom; 4, 6: the top), the other small (3, 4) or none (5, 6)
        int big = layout() == 3 || layout() == 5;
        if (s != big && layout() >= 5) return;
        if (s == big) {
            k = fminf(fw / sw, fh / 240);
            box[0] = (fw - sw * k) / 2, box[1] = (fh - 240 * k) / 2;
        } else {  // (1:1 in the corner: the top one top left, the bottom one bottom right)
            k = 1;
            box[0] = s ? fw - W1 : 0, box[1] = s ? fh - 240 : 0;
        }
    }
    }
    box[2] = sw * k, box[3] = 240 * k;
}
void pdisp_present(void) {
    if (sWinSurf == EGL_NO_SURFACE || !screens_make()) return;
    gld_enter();
    EGLint w = 0, h = 0;  // (Mesa's EGL here gives 0 x 0 for the window: then the size it was made with)
    eglQuerySurface(sDpy, sWinSurf, EGL_WIDTH, &w);
    eglQuerySurface(sDpy, sWinSurf, EGL_HEIGHT, &h);
    if (w <= 0 || h <= 0) w = 1280, h = 720;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, w, h);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glColorMask(1, 1, 1, 1);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(sPresentProg);
    glBindVertexArray(sVao);
    glBindBuffer(GL_ARRAY_BUFFER, sVbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(CgVertex), 0, GL_STREAM_DRAW);  // (new: the compositing's draws may still read the old)
    glActiveTexture(GL_TEXTURE0);
    pad_poll();
    for (int k = 0; k < 2; k++) {
        int s = k ^ under();
        float b[4];
        screen_box(s, w, h, b);
        if (b[2] <= 0) continue;
        // (the window's normalized coordinates, y up; the texture's first row the screen's top)
        float x0 = b[0] / w * 2 - 1, x1 = (b[0] + b[2]) / w * 2 - 1, y0 = 1 - b[1] / h * 2, y1 = 1 - (b[1] + b[3]) / h * 2;
        CgVertex q[6] = {{{x0, y0, 0}, {0}, {0, 0}}, {{x1, y0, 0}, {0}, {1, 0}}, {{x1, y1, 0}, {0}, {1, 1}},
                         {{x0, y0, 0}, {0}, {0, 0}}, {{x1, y1, 0}, {0}, {1, 1}}, {{x0, y1, 0}, {0}, {0, 1}}};
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)(k * sizeof q), sizeof q, q);  // (each screen its own place)
        glBindTexture(GL_TEXTURE_2D, sScreen[s].tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glDrawArrays(GL_TRIANGLES, k * 6, 6);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // (comp2d's reads of it: nearest)
    }
    if (sPtrShown) {  // the pointer: a white square in a black one (the window's rows from the bottom)
        int px = (int)(sPtrX * (float)w / 1280), py = h - 1 - (int)(sPtrY * (float)h / 720);
        glEnable(GL_SCISSOR_TEST);
        glScissor(px - 7, py - 7, 15, 15);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glScissor(px - 4, py - 4, 9, 9);
        glClearColor(1, sPtrTouch ? 0.8f : 1.0f, sPtrTouch ? 0.2f : 1.0f, 1);  // (yellow while it touches)
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }
    eglSwapBuffers(sDpy, sWinSurf);
    gld_leave();
}
int pdisp_window_to_ds(float wx, float wy, int w, int h, int *x, int *y) {
    for (int k = 0; k < 2; k++) {
        int s = k ^ under() ^ 1;  // (the one drawn last first: it is on top)
        float b[4];
        screen_box(s, w, h, b);
        if (b[2] <= 0) continue;
        float sw = (float)CG_SCREEN_W(s), sx = (wx - b[0]) / b[2] * sw, sy = (wy - b[1]) / b[3] * 240;
        if (sx < 0 || sy < 0 || sx >= sw || sy >= 240) continue;
        // (the DS's picture in the screen's middle; the UI moved to the wide screen's edges back to where it is on the
        // DS's, before the range check, as frontend_3ds.c: such a touch is in the margins)
        int dx = (int)sx - (CG_SCREEN_W(s) - W) / 2, dy = (int)sy - 24;
        if (s == 1) pdisp_touch_map(&dx, &dy);
        if (dx < 0 || dx >= W || dy < 0 || dy >= H) return 0;
        *x = dx, *y = dy + 192 * s;
        return 1;
    }
    return 0;
}

// the touch screen, read through libnx (SDL gives no touches without a window of its own): the first finger on a DS
// screen in pdisp_window_to_ds's coordinates; 0 if none
// (or the pointer's, docked: ZR held)
int pdisp_touch_read(int *x, int *y) {
    static int init;
    if (!init++) hidInitializeTouchScreen();
    HidTouchScreenState st = {0};
    if (hidGetTouchScreenStates(&st, 1) && st.count >= 1) {
        sPtrShown = 0;  // (a finger: the pointer away)
        return pdisp_window_to_ds((float)st.touches[0].x, (float)st.touches[0].y, 1280, 720, x, y);
    }
    return sPtrTouch && pdisp_window_to_ds(sPtrX, sPtrY, 1280, 720, x, y);
}

#endif
